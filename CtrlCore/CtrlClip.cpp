#include "CtrlCore.h"

namespace drx {

Txt sRawClipData(const Val& data)
{
	return data;
}

ClipData::ClipData(const Val& data, Txt (*render)(const Val& data))
:	data(data), render(render ? render : sRawClipData)
{}

ClipData::ClipData(const Txt& data)
:	data(data), render(sRawClipData)
{}

ClipData::ClipData()
:	render(sRawClipData)
{}

void Ctrl::DragAndDrop(Point p, PasteClip& d)       {}
void Ctrl::FrameDragAndDrop(Point p, PasteClip& d)  {}
void Ctrl::DragEnter()                              {}
void Ctrl::DragRepeat(Point p) {}
void Ctrl::DragLeave() {}

PasteClip& Ctrl::Clipboard()
{
	GuiLock __;
	static PasteClip d;
	d.fmt.Clear();
	return d;
}

PasteClip& Ctrl::Selection()
{
	GuiLock __;
	static PasteClip d;
	GuiPlatformSelection(d);
	return d;
}

Txt Ctrl::GetDropData(const Txt& fmt) const
{
	return GetSelectionData(fmt);
}

Txt Ctrl::GetSelectionData(const Txt& fmt) const
{
	return Null;
}

bool PasteClip::Accept()
{
	accepted = true;
	return paste;
}

bool   PasteClip::Accept(tukk _fmt)
{
	Vec<Txt> f = Split(_fmt, ';');
	for(i32 i = 0; i < f.GetCount(); i++) {
		if(IsAccepted() && fmt == f[i])
			return paste;
		if(IsAvailable(f[i])) {
			accepted = true;
			if(paste) {
				fmt = f[i];
				data = Get(f[i]);
				return true;
			}
			break;
		}
	}
	return false;
}

PasteClip::PasteClip()
{
	paste = true;
	accepted = false;
	GuiPlatformConstruct();
}

i32 Ctrl::DoDragAndDrop(tukk fmts, const Image& sample, dword actions)
{
	VecMap<Txt, ClipData> dummy;
	return DoDragAndDrop(fmts, sample, actions, dummy);
}

i32 Ctrl::DoDragAndDrop(const VecMap<Txt, ClipData>& data, const Image& sample, dword actions)
{
	return DoDragAndDrop("", sample, actions, data);
}

Uuid        sDndUuid;
ukk sInternalPtr;

Txt GetInternalDropId__(tukk type, tukk id)
{
	return "U++ Internal clip:" + AsTxt(sDndUuid) + '-' + type + '-' + id;
}

void NewInternalDrop__(ukk ptr)
{
	sDndUuid = Uuid::Create();
	sInternalPtr = ptr;
}

ukk GetInternalDropPtr__()
{
	return sInternalPtr;
}

void GuiPlatformAdjustDragImage(ImageBuffer& b);

Image MakeDragImage(const Image& arrow, Image sample)
{
	ImageBuffer b;
	if(IsNull(sample)) {
		sample = CtrlCoreImg::DndData();
		b = sample;
		Over(b, Point(0, 0), arrow, arrow.GetSize());
	}
	else {
		b.Create(128, 128);
		memset(~b, 0, sizeof(RGBA) * b.GetLength());
		Over(b, Point(2, 22), sample, sample.GetSize());
		Unmultiply(b);
		for(i32 y = 20; y < 96; y++) {
			RGBA *s = b[y];
			RGBA *e = s + 96;
			while(s < e)
				(s++)->a >>= 1;
			e += 32;
			i32 q = 128;
			while(s < e) {
				s->a = (s->a * q) >> 8;
				q -= 4;
				s++;
			}
		}
		i32 qq = 128;
		for(i32 y = 96; y < 128; y++) {
			RGBA *s = b[y];
			RGBA *e = s + 96;
			while(s < e) {
				s->a = (s->a * qq) >> 8;
				s++;
			}
			e += 32;
			i32 q = 255;
			while(s < e) {
				s->a = (s->a * q * qq) >> 16;
				q -= 8;
				s++;
			}
			qq -= 4;
		}
		RGBA *s = b[21] + 1;
		RGBA c1 = Blue();
		RGBA c2 = White();
		for(i32 a = 255; a > 0; a -= 3) {
			c1.a = c2.a = a;
			*s++ = c1;
			Swap(c1, c2);
		}
		s = b[21] + 1;
		c1 = Black();
		c2 = White();
		for(i32 a = 255; a > 0; a -= 8) {
			c1.a = c2.a = a;
			*s = c1;
			s += b.GetWidth();
			Swap(c1, c2);
		}
		Premultiply(b);
		GuiPlatformAdjustDragImage(b);
		Over(b, Point(0, 0), arrow, arrow.GetSize());
	}

	return static_cast<Image>(b);
}

Ptr<Ctrl> Ctrl::dndctrl;
Point     Ctrl::dndpos;
bool      Ctrl::dndframe;
PasteClip Ctrl::dndclip;

void Ctrl::DnDRepeat()
{
	GuiLock __;
	if(dndctrl) {
		dndctrl->DragRepeat(dndpos);
		if(dndctrl) {
			PasteClip d = dndclip;
			if(dndframe)
				dndctrl->FrameDragAndDrop(dndpos, d);
			else
				dndctrl->DragAndDrop(dndpos, d);
		}
	}
	else
		DRX::KillTimeCallback(&dndpos);
}

void Ctrl::DnD(Point p, PasteClip& clip)
{
	GuiLock __;
	DRX::KillTimeCallback(&dndpos);
	dndclip = clip;
	Point hp = p - GetScreenRect().TopLeft();
	Ptr<Ctrl> ctrl = this;
	while(ctrl && ctrl->IsEnabled()) {
		Rect view = ctrl->GetScreenView();
		if(ctrl->IsMouseActive()) {
			if(view.Contains(p)) {
				dndpos = p - view.TopLeft();
				dndframe = false;
				ctrl->DragAndDrop(dndpos, clip);
				if(clip.IsAccepted())
					break;
			}
			else {
				dndpos = p - ctrl->GetScreenRect().TopLeft();
				dndframe = true;
				ctrl->FrameDragAndDrop(dndpos, clip);
				if(clip.IsAccepted())
					break;
			}
		}
		ctrl = ctrl->ChildFromPoint(hp);
	}
	if(ctrl != dndctrl) {
		if(dndctrl)
			dndctrl->DragLeave();
		dndctrl = ctrl;
		if(dndctrl)
			dndctrl->DragEnter();
	}
	if(dndctrl)
		DRX::SetTimeCallback(-40, callback(DnDRepeat), &dndpos);
}

void Ctrl::DnDLeave()
{
	GuiLock __;
	if(dndctrl) {
		dndctrl->DragLeave();
		DRX::KillTimeCallback(&dndpos);
		dndctrl = NULL;
	}
}

Ctrl *Ctrl::GetDragAndDropTarget()
{
	GuiLock __;
	return dndctrl;
}

void AppendClipboard(tukk format, const ClipData& data)
{
	AppendClipboard(format, data.data, data.render);
}

void AppendClipboard(const VecMap<Txt, ClipData>& data)
{
	for(i32 i = 0; i < data.GetCount(); i++)
		AppendClipboard(data.GetKey(i), data[i]);
}

void InitRichImage(Txt      (*fGetImageClip)(const Image& img, const Txt& fmt),
                   bool        (*fAcceptImage)(PasteClip& clip),
                   Image       (*fGetImage)(PasteClip& clip),
                   tukk (*fClipFmtsImage)());

INITBLOCK {
	InitRichImage(GetImageClip, AcceptImage, GetImage, ClipFmtsImage);
}

tukk sClipFmtsRTF = "Rich Text Format;text/rtf;application/rtf";

tukk ClipFmtsRTF()
{
	return sClipFmtsRTF;
}

#ifdef TARGET_POSIX

void WriteClipboardHTML(const Txt& html)
{
    WriteClipboard("text/html", html);
}

#else

void WriteClipboardHTML(const Txt& html)
{
	Txt data;
	
	data =
		"Version:0.9\n"
		"StartHTML:#000001#\n"
		"EndHTML:#000002#\n"
		"StartFragment:#000003#\n"
		"EndFragment:#000004#\n"
		"StartSelection:#000003#\n"
		"EndSelection:#000004#\n"
		"<HTML>\n"
		"<!--StartFragment -->"
	    "<!--EndFragment -->\n"
		"</HTML>\n"
	;
	
	data.Replace("#000001#", Sprintf("%08u", data.Find("<HTML>") + 6));
	i32 start = data.Find("<!--StartFragment -->") + 21;
	data.Replace("#000003#", Sprintf("%08u", start));
	data.Replace("#000004#", Sprintf("%08u", data.Find("<!--EndFragment -->") + html.GetCount()));
	data.Replace("#000002#", Sprintf("%08u", data.Find("</HTML>") + html.GetCount()));
	
	data.Insert(start, html);
	
	WriteClipboard("HTML Format", data);
}

#endif

}
