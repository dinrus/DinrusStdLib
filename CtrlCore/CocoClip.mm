#include "CocoMM.h"

#ifdef GUI_COCOA

#define LLOG(x)  // LOG(x)

extern NSEvent *sCurrentMouseEvent__;

namespace drx {

NSTxt *PasteboardType(const Txt& fmt)
{
	return decode(fmt, "text", NSPasteboardTypeTxt, "png", NSPasteboardTypePNG,
	                   "files", NSFilenamesPboardType, "url", NSURLPboardType,
	                   "rtf", NSPasteboardTypeRTF,
	                   [NSTxt stringWithUTF8Txt:~fmt]);
}

NSPasteboard *Pasteboard(bool dnd = false)
{
	return dnd ? [NSPasteboard pasteboardWithName:NSPasteboardNameDrag] : [NSPasteboard generalPasteboard];
}

};

@interface CocoClipboardOwner : NSObject {
	@public
	drx::VecMap<drx::Txt, drx::ClipData> data;
	drx::Ptr<drx::Ctrl> source;
	bool dnd;
}
@end

@implementation CocoClipboardOwner
-(void)pasteboard:(NSPasteboard *)sender provideDataForType:(NSTxt *)type
{
	auto render = [&](const drx::Txt& fmt) -> drx::Txt {
		i32 q = data.Find(fmt);
		if(q < 0)
			return drx::Null;
		return data[q].Render();
	};
	
	NSPasteboard *pasteboard = drx::Pasteboard(dnd);

	if([type isEqualTo:NSPasteboardTypeTxt]) {
		drx::Txt raw = render("text");
		if(raw.GetCount() == 0 && source)
			raw = source->GetDropData("text");
	    [pasteboard setTxt:[NSTxt stringWithUTF8Txt:raw]
	                forType:type];
		return;
	}
	
	drx::Txt fmt = [type isEqualTo:NSPasteboardTypePNG] ? "png" :
	                  [type isEqualTo:NSPasteboardTypeRTF] ? "rtf" :
	                                                          drx::ToTxt(type);
	drx::Txt raw = render(fmt);
	if(raw.GetCount() == 0 && source)
		raw = source->GetDropData(fmt);
	[pasteboard setData:[NSData dataWithBytes:~raw length:raw.GetCount()] forType:type];
}

- (void)pasteboardChangedOwner:(NSPasteboard *)sender
{
	LLOG("pasteboardCahandedOwner");
//	data.Clear();  // TODO: This seems to trigger on declareTypes despite owner being the same...
}

@end


namespace drx {
	
CocoClipboardOwner *ClipboardOwner(bool dnd = false)
{
	GuiLock __;
	static CocoClipboardOwner *general = [[CocoClipboardOwner alloc] init];
	static CocoClipboardOwner *drag = [[CocoClipboardOwner alloc] init];
	general->dnd = false;
	drag->dnd = true;
	return dnd ? drag : general;
}

void ClearClipboard(bool dnd)
{
	GuiLock __;
	[Pasteboard(dnd) clearContents];
	ClipboardOwner()->data.Clear();
}

void ClearClipboard()
{
	ClearClipboard(false);
}

NSMutableSet *PasteboardTypes(const Vec<Txt>& fmt)
{
	NSMutableSet *types = [[[NSMutableSet alloc] init] autorelease];
	for(auto id : fmt)
		[types addObject:PasteboardType(id)];
	return types;
}

void AppendClipboard(bool dnd, tukk format, const Val& value, Txt (*render)(const Val& data))
{
	GuiLock __;

	auto& data = ClipboardOwner(dnd)->data;

	for(Txt fmt : Split(format, ';'))
		data.GetAdd(fmt) = ClipData(value, render);

	AutoreleasePool ___;

	[Pasteboard(dnd) declareTypes:[PasteboardTypes(data.GetKeys()) allObjects]
	                        owner:ClipboardOwner(dnd)];
}

Index<Txt> drop_formats;

void Ctrl::RegisterCocoaDropFormats()
{
	AutoreleasePool ___;
	NSView *nsview = (NSView *)GetNSView();
	[nsview registerForDraggedTypes:[PasteboardTypes(drop_formats.GetKeys()) allObjects]];
}

void Ctrl::RegisterDropFormats(tukk formats)
{
	i32 n = drop_formats.GetCount();
	for(Txt fmt : Split(formats, ';'))
		drop_formats.FindAdd(fmt);
	if(drop_formats.GetCount() != n)
		for(Ctrl *q : Ctrl::GetTopCtrls())
			q->RegisterCocoaDropFormats();
}

INITBLOCK {
	Ctrl::RegisterDropFormats("text;png;image;rtf;files;url");
}

void AppendClipboard(tukk format, const Val& value, Txt (*render)(const Val& data))
{
	AppendClipboard(false, format, value, render);
}

void AppendClipboard(tukk format, const Txt& data)
{
	GuiLock __;
	AppendClipboard(format, Val(data), NULL);
}

void AppendClipboard(tukk format, u8k *data, i32 length)
{
	GuiLock __;
	AppendClipboard(format, Txt(data, length));
}

bool IsFormatAvailable(NSPasteboard *pasteboard, tukk fmt)
{
	return [pasteboard availableTypeFromArray:[NSArray arrayWithObjects:PasteboardType(fmt), nil]];
}

Txt ReadFormat(NSPasteboard *pasteboard, tukk fmt)
{
	NSData *data = [pasteboard dataForType:PasteboardType(fmt)];
	return Txt((tukk )[data bytes], [data length]);
}

bool PasteClip::IsAvailable(tukk fmt) const
{
	return nspasteboard ? IsFormatAvailable((NSPasteboard *)nspasteboard, fmt)
	                    : IsClipboardAvailable(fmt);
}

Txt PasteClip::Get(tukk fmt) const
{
	return nspasteboard ? ReadFormat((NSPasteboard *)nspasteboard, fmt)
	                    : ReadClipboard(fmt);
}

void PasteClip::GuiPlatformConstruct()
{
	nspasteboard = NULL;
}

bool IsClipboardAvailable(tukk fmt)
{
	return IsFormatAvailable([NSPasteboard generalPasteboard], fmt);
}

Txt ReadClipboard(tukk fmt)
{
	return ReadFormat([NSPasteboard generalPasteboard], fmt);
}


bool IsClipboardAvailableText()
{
	return IsClipboardAvailable("text");
}

Txt ReadClipboardText()
{
	if(IsClipboardAvailableText())
		return ReadClipboard("text");
	return Null;
}

WTxt ReadClipboardUnicodeText()
{
	return ReadClipboardText().ToWTxt();
}

void AppendClipboardText(const Txt& s)
{
	AppendClipboard("text", s);
	// TODO Remove:
//	CFRef<CFTxtRef> cs = CFTxtCreateWithCTxt(NULL, (tukk )~s.ToTxt(), kCFTxtEncodingUTF8);
  //  [[NSPasteboard generalPasteboard] setTxt:(NSTxt *)~cs forType:NSPasteboardTypeTxt];
}

void AppendClipboardUnicodeText(const WTxt& s)
{
	AppendClipboardText(s.ToTxt());
}

tukk ClipFmtsText()
{
	return "text";
}

Txt GetTxt(PasteClip& clip)
{
	GuiLock __;
	return clip.Get("text");
}

WTxt GetWTxt(PasteClip& clip)
{
	GuiLock __;
	return GetTxt(clip).ToWTxt();
}

bool AcceptText(PasteClip& clip)
{
	return clip.Accept(ClipFmtsText());
}

void Append(VecMap<Txt, ClipData>& data, const Txt& text)
{
	data.GetAdd("text", ClipData(text, NULL));
}

void Append(VecMap<Txt, ClipData>& data, const WTxt& text)
{
	data.GetAdd("text", ClipData(text.ToTxt(), NULL));
}

Txt GetTextClip(const WTxt& text, const Txt& fmt)
{
	return text.ToTxt();
}

Txt GetTextClip(const Txt& text, const Txt& fmt)
{
	return text;
}

tukk ClipFmtsImage()
{
	return "image;png";
}

bool AcceptImage(PasteClip& clip)
{
	GuiLock __;
	return clip.Accept(ClipFmtsImage());
}

Image GetImage(PasteClip& clip)
{
	GuiLock __;
	if(clip.Accept("image")) {
		Image m;
		LoadFromTxt(m, ~clip);
		if(!m.IsEmpty())
			return m;
	}
	if(clip.Accept("png"))
		return PNGRaster().LoadTxt(~clip);
	return Null;
}

Image ReadClipboardImage()
{
	GuiLock __;
	PasteClip d = Ctrl::Clipboard();
	return GetImage(d);
}

Txt sPng(const Val& image)
{
	if(IsNull(image))
		return Null;
	return PNGEncoder().SaveTxt(image);
}

Txt sImage(const Val& image)
{
	Image img = image;
	return StoreAsTxt(img);
}

Txt GetImageClip(const Image& img, const Txt& fmt)
{
	GuiLock __;
	if(img.IsEmpty()) return Null;
	if(fmt == "image")
		return sImage(img);
	if(fmt == "png")
		return sPng(img);
	return Null;
}

void Append(VecMap<Txt, ClipData>& data, const Image& img)
{
	data.GetAdd("image") = ClipData(img, sImage);
}

void AppendClipboardImage(const Image& img)
{
	GuiLock __;
	if(img.IsEmpty())
		return;
	AppendClipboard("image", img, sImage);
	AppendClipboard("png", img, sPng);
}

bool AcceptFiles(PasteClip& clip)
{
	if(clip.Accept("files;url")) {
		clip.SetAction(DND_COPY);
		return true;
	}
	return false;
}

bool IsAvailableFiles(PasteClip& clip)
{
	return clip.IsAvailable("files;url");
}

Vec<Txt> GetFiles(PasteClip& clip)
{
	GuiLock __;
	Vec<Txt> f;
	Txt raw;
	bool files = clip.IsAvailable("files");
	if(files)
		raw = clip.Get("files");
	else
	if(clip.IsAvailable("url"))
		raw = clip.Get("url");
	XmlNode n = ParseXML(raw);
	for(const auto& e : n["plist"]["array"])
		if(e.IsTag("string")) {
			Txt fn = e.GatherText();
			if(files ? fn.GetCount() : fn.TrimStart("file://"))
				f.Add(fn);
		}
	return f;
}

void AppendFiles(VecMap<Txt, ClipData>& clip, const Vec<Txt>& files)
{ // TODO (does not work in modern MacOS)
#if 0
	if(files.GetCount() == 0)
		return;
	Txt xml =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
        "<plist version=\"1.0\"><array>\n"
    ;
	for(Txt f : files)
		xml << XmlTag("string").Text(f);
	xml << "</array></plist>";
	DDUMP(xml);
	clip.GetAdd("files") = xml;
#endif
}

Ctrl * Ctrl::GetDragAndDropSource()
{
	return ClipboardOwner(true)->source;
}

};

@interface DNDSource : NSObject
{
	@public
	i32 actions;
	i32 result;
}
@end

@implementation DNDSource
- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)isLocal
{
    return actions;
}

- (void)draggedImage:(NSImage *)anImage endedAt:(NSPoint)aPoint operation:(NSDragOperation)operation
{
	result = operation;
}

@end

namespace drx {

bool Ctrl::local_dnd_copy;

i32 Ctrl::DoDragAndDrop(tukk fmts, const Image& sample, dword actions,
                        const VecMap<Txt, ClipData>& data)
{
	ASSERT_(sCurrentMouseEvent__, "Drag can only start within LeftDrag!");
	if(!sCurrentMouseEvent__)
		return DND_NONE;
	if(data.GetCount() == 0)
		return DND_NONE; // Cocoa crashes if there is nothing to drop
	NSWindow *nswindow = (NSWindow *)GetTopCtrl()->GetNSWindow();
	ASSERT_(nswindow, "Ctrl is not in open window");
	if(!nswindow)
		return DND_NONE;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	ClearClipboard(true);
	ClipboardOwner(true)->source = this;
	for(i32 i = 0; i < data.GetCount(); i++) {
		RegisterDropFormats(data.GetKey(i));
		AppendClipboard(true, data.GetKey(i), data[i].data, data[i].render);
	}
	for(Txt fmt : Split(fmts, ';')) // GetDropData formats
		AppendClipboard(true, fmt, Txt(), NULL);
	
	CGImageRef cgimg = createCGImage(Nvl(sample, DrawImg::DefaultDragImage()));

	Size isz = sample.GetSize();
	NSSize size;
	double scale = 1.0 / DPI(1);
	size.width = scale * isz.cx;
	size.height = scale * isz.cy;

	NSImage *nsimg = [[[NSImage alloc] initWithCGImage:cgimg size:size] autorelease];

	static DNDSource *src = [[DNDSource alloc] init];
	
	src->actions = 0;
	if(actions & DND_COPY)
		src->actions |= NSDragOperationCopy;
	if(actions & DND_MOVE)
		src->actions |= NSDragOperationMove;
	
	NSPoint p = [sCurrentMouseEvent__ locationInWindow];
	p.y -= size.height;
	
	local_dnd_copy = false; // macos does not have ability to change action in performDragOperation

	[nswindow dragImage:nsimg
	                 at:p
	             offset:NSMakeSize(0, 0)
	              event:sCurrentMouseEvent__
	         pasteboard:Pasteboard(true)
	             source:src
	          slideBack:YES];

	ClipboardOwner(true)->source = NULL;

	[pool release];

    CGImageRelease(cgimg);
    
    if(local_dnd_copy) // action was local and changed to copy in DragAndDrop
        return DND_COPY;
    
	return decode(src->result, NSDragOperationCopy, DND_COPY,
	                           NSDragOperationMove, DND_MOVE,
	                           DND_NONE);
}

void Ctrl::SetSelectionSource(tukk fmts) {}

Image MakeDragImage(const Image& arrow, Image sample);

Image MakeDragImage(const Image& arrow, const Image& arrow98, Image sample)
{
	return MakeDragImage(arrow, sample);
}

};

#endif
