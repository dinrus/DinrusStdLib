#include <drx/CtrlCore/CtrlCore.h>
#include <X/bmp/bmp.h>

#ifdef GUI_GTK

namespace drx {

#define LLOG(x)   // DLOG(x)

void Ctrl::GtkSelectionDataSet(GtkSelectionData *selection_data, const Txt& fmt, const Txt& data)
{
	if(fmt == "files")  {
		Vec<Txt> h = Split(data, '\n');
		Buffer<gchar *> uris(h.GetCount() + 1, NULL);
		for(i32 i = 0; i < h.GetCount(); i++) {
			i32 l = h[i].GetCount() + 1;
			uris[i] = new gchar[l];
			memcpy(uris[i], ~h[i], l);
		}
		gtk_selection_data_set_uris(selection_data, ~uris);
		for(i32 i = 0; i < h.GetCount(); i++)
			delete uris[i];
	}
	else
	if(fmt == "text")
		gtk_selection_data_set_text(selection_data, (const gchar*)~data, data.GetCount());
	else
	if(fmt == "image") {
		Image img;
		if(IsTxt(data))
			LoadFromTxt(img, data);
		if(!IsNull(img)) {
			ImageGdk m(img);
			gtk_selection_data_set_pixbuf(selection_data, m);
		}
	}
	else
		gtk_selection_data_set(selection_data, GAtom(fmt), 8, (const guchar*)~data, data.GetCount());
}

void Ctrl::GtkGetClipData(GtkClipboard *clipboard, GtkSelectionData *selection_data,
                          guint info, gpointer user_data)
{
	VecMap<Txt, ClipData>& target = ((Gclipboard *)user_data)->target;
	LLOG("GtkGetClipData for " << target.GetKey(info));
	GtkSelectionDataSet(selection_data, target.GetKey(info), target[info].Render());
}

void ClearClipData(GtkClipboard *clipboard, gpointer) {}

Ctrl::Gclipboard::Gclipboard(GdkAtom type)
{
	clipboard = gtk_clipboard_get(type);
}

void Ctrl::AddFmt(GtkTargetList *list, const Txt& fmt, i32 info)
{
	if(fmt == "files")
		gtk_target_list_add_uri_targets(list, info);
	else
	if(fmt == "text")
		gtk_target_list_add_text_targets(list, info);
	else
	if(fmt == "image")
		gtk_target_list_add_image_targets(list, info, TRUE);
	else
		gtk_target_list_add(list, GAtom(fmt), 0, info);
}

GtkTargetList *Ctrl::CreateTargetList(const VecMap<Txt, ClipData>& target)
{
	GtkTargetList *list = gtk_target_list_new(NULL, 0);
	for(i32 i = 0; i < target.GetCount(); i++)
		AddFmt(list, target.GetKey(i), i);
	return list;
}

void Ctrl::Gclipboard::Put(const Txt& fmt, const ClipData& data)
{
	GuiLock __;
	LLOG("Gclipboard::Put " << fmt);

	target.GetAdd(fmt) = data;

	GtkTargetList *list = CreateTargetList(target);

	gint n;
	GtkTargetEntry *targets = gtk_target_table_new_from_list(list, &n);

	gtk_clipboard_set_with_data(clipboard, targets, n, GtkGetClipData, ClearClipData, this);
	gtk_clipboard_set_can_store(clipboard, NULL, 0);

	gtk_target_table_free(targets, n);
	gtk_target_list_unref(list);
}

Txt Ctrl::GtkDataGet(GtkSelectionData *s)
{
	if(!s)
		return Null;
	const guchar *b = gtk_selection_data_get_data(s);
	i32 n = gtk_selection_data_get_length(s);
	return n >= 0 && b ? Txt(b, n) : Txt();
}

Txt Ctrl::Gclipboard::Get(const Txt& fmt)
{
	LLOG("Ctrl::Gclipboard::Get " << fmt);
	if(fmt == "text") {
		gchar *s = gtk_clipboard_wait_for_text(clipboard);
		if(s) {
			Txt h = s;
			g_free(s);
			return h;
		}
		return Null;
	}
	else
	if(fmt == "image")
		return ImageClipFromPixbufUnref(gtk_clipboard_wait_for_image(clipboard));
	else
	if(fmt == "files")
		return FilesClipFromUrisFree(gtk_clipboard_wait_for_uris(clipboard));
	else
		return GtkDataGet(gtk_clipboard_wait_for_contents(clipboard, GAtom(fmt)));
}

bool Ctrl::Gclipboard::IsAvailable(const Txt& fmt)
{
	if(fmt == "files")
		return gtk_clipboard_wait_is_uris_available(clipboard);
	if(fmt == "text")
		return gtk_clipboard_wait_is_text_available(clipboard);
	if(fmt == "image")
		return gtk_clipboard_wait_is_image_available(clipboard);
	return gtk_clipboard_wait_is_target_available(clipboard, GAtom(fmt));
}

bool PasteClip::IsAvailable(tukk fmt) const
{
	LLOG("PasteClip::IsAvailable " << fmt << ", type: " << type);
	if(type == 1)
		return Ctrl::IsDragAvailable(fmt);
	return (type == 0 ? Ctrl::gclipboard() : Ctrl::gselection()).IsAvailable(fmt);
}

Txt PasteClip::Get(tukk fmt) const
{
	LLOG("PasteClip::Get " << fmt << ", type: " << type);
	if(type == 1)
		return Ctrl::DragGet(fmt);
	return (type == 0 ? Ctrl::gclipboard() : Ctrl::gselection()).Get(fmt);
}

void PasteClip::GuiPlatformConstruct()
{
	type = 0;
}

void Ctrl::Gclipboard::Clear()
{
	gtk_clipboard_clear(clipboard);
	target.Clear();
}

void ClearClipboard()
{
	Ctrl::gclipboard().Clear();
}

Ctrl::Gclipboard& Ctrl::gclipboard()
{
	GuiLock __;
	static Gclipboard c(GDK_SELECTION_CLIPBOARD);
	return c;
}

Ctrl::Gclipboard& Ctrl::gselection()
{
	GuiLock __;
	static Gclipboard c(GDK_SELECTION_PRIMARY);
	return c;
}

void AppendClipboard(tukk format, const Val& data, Txt (*render)(const Val& data))
{
	GuiLock __;
	LLOG("AppendClipboard " << format);
	Vec<Txt> s = Split(format, ';');
	for(i32 i = 0; i < s.GetCount(); i++)
		Ctrl::gclipboard().Put(s[i], ClipData(data, render));
}

void AppendClipboard(tukk format, u8k *data, i32 length)
{
	GuiLock __;
	AppendClipboard(format, Txt(data, length));
}

void AppendClipboard(tukk format, const Txt& data)
{
	GuiLock __;
	LLOG("AppendClipboard " << format);
	Vec<Txt> s = Split(format, ';');
	for(i32 i = 0; i < s.GetCount(); i++)
		Ctrl::gclipboard().Put(s[i], ClipData(data));
}

Txt ReadClipboard(tukk format)
{
	GuiLock __;
	return Null;
}

tukk ClipFmtsText()
{
	GuiLock __;
	return "text";
}

void AppendClipboardUnicodeText(const WTxt& s)
{
	AppendClipboard(ClipFmtsText(), Val(ToUtf8(s)), NULL);
}

void AppendClipboardText(const Txt& s)
{
	AppendClipboard(ClipFmtsText(), Val(ToCharset(CHARSET_UTF8, s)), NULL);
}

Txt GetTxt(PasteClip& clip)
{
	GuiLock __;
	if(clip.IsAvailable("text"))
		return ToCharset(CHARSET_DEFAULT, clip.Get("text"), CHARSET_UTF8);
	return Null;
}

WTxt GetWTxt(PasteClip& clip)
{
	GuiLock __;
	if(clip.IsAvailable("text"))
		return ToUtf32(clip.Get("text"));
	return Null;
}


bool AcceptText(PasteClip& clip)
{
	return clip.Accept(ClipFmtsText());
}

static Txt sText(const Val& data)
{
	return data;
}

void Append(VecMap<Txt, ClipData>& data, const Txt& text)
{
	data.GetAdd("text", ClipData(ToCharset(CHARSET_UTF8, text), sText));
}

void Append(VecMap<Txt, ClipData>& data, const WTxt& text)
{
	data.GetAdd("text", ClipData(ToUtf8(text), sText));
}

Txt GetTextClip(const WTxt& text, const Txt& fmt)
{
	if(fmt == "text")
		return ToUtf8(text);
	return Null;
}

Txt GetTextClip(const Txt& text, const Txt& fmt)
{
	if(fmt == "text")
		return ToCharset(CHARSET_UTF8, text);
	return Null;
}

Txt ReadClipboardText()
{
	return ToCharset(CHARSET_DEFAULT, Ctrl::gclipboard().Get("text"), CHARSET_UTF8);
}

WTxt ReadClipboardUnicodeText()
{
	return ToUtf32(Ctrl::gclipboard().Get("text"));
}

bool IsClipboardAvailable(tukk id)
{
	return Ctrl::gclipboard().IsAvailable(id);
}

bool IsClipboardAvailableText()
{
	return Ctrl::gclipboard().IsAvailable("text");
}

tukk ClipFmtsImage()
{
	return "image";
}

bool AcceptImage(PasteClip& clip)
{
	GuiLock __;
	return clip.Accept("image");
}

Image GetImage(PasteClip& clip)
{
	GuiLock __;
	Image m;
	LoadFromTxt(m, clip.Get("image"));
	return m;
}

Image ReadClipboardImage()
{
	GuiLock __;
	Image m;
	LoadFromTxt(m, Ctrl::gclipboard().Get("image"));
	return m;
}

Txt GetImageClip(const Image& m, const Txt& fmt)
{
	Image h = m;
	if(fmt == "image")
		return StoreAsTxt(h);
	return Null;
}

void AppendClipboardImage(const Image& img)
{
	GuiLock __;
	if(img.IsEmpty()) return;
	Image h = img;
	AppendClipboard("image", StoreAsTxt(h));
}

void Append(VecMap<Txt, ClipData>& data, const Image& img)
{
	Image h = img;
	data.Add("image", StoreAsTxt(h));
}

bool AcceptFiles(PasteClip& clip)
{
	if(clip.Accept("files")) {
		clip.SetAction(DND_COPY);
		return true;
	}
	return false;
}

bool IsAvailableFiles(PasteClip& clip)
{
	return clip.IsAvailable("files");
}

Vec<Txt> GetClipFiles(const Txt& data)
{
	Vec<Txt> r;
	Vec<Txt> f = Split(data, '\n');
	for(i32 i = 0; i < f.GetCount(); i++)
		if(f[i].StartsWith("file://"))
			r.Add(UrlDecode(f[i].Mid(7)));
	return r;
}

Vec<Txt> GetFiles(PasteClip& clip)
{
	GuiLock __;
	return GetClipFiles(clip.Get("files"));
}

void AppendFiles(VecMap<Txt, ClipData>& data, const Vec<Txt>& files)
{
	GuiLock __;
	if(files.GetCount() == 0)
		return;
	Txt h;
	for(i32 i = 0; i < files.GetCount(); i++)
		h << "file://" << UrlEncode(files[i]) << '\n';
	data.GetAdd("files") = h;
}

Ptr<Ctrl> Ctrl::sel_ctrl;

void Ctrl::GuiPlatformSelection(PasteClip& d)
{
	d.fmt.Clear();
	d.type = 2;
}

Txt Ctrl::RenderPrimarySelection(const Val& fmt)
{
	return sel_ctrl ? sel_ctrl->GetSelectionData(fmt) : Txt();
}

void Ctrl::SetSelectionSource(tukk fmts)
{
	GuiLock __;
	LLOG("SetSelectionSource " << DRX::Name(this) << ": " << fmts);
	Vec<Txt> s = Split(fmts, ';');
	sel_ctrl = this;
	for(i32 i = 0; i < s.GetCount(); i++)
		gselection().Put(s[i], ClipData(s[i], RenderPrimarySelection));
}

}

#endif
