#include "CtrlCore.h"

#ifdef GUI_X11

namespace drx {

#define LLOG(x)  // LOG(x)

Index<Txt> Ctrl::sel_formats;
Ptr<Ctrl>     Ctrl::sel_ctrl;

void Ctrl::SetSelectionSource(tukk fmts)
{
	GuiLock __;
	LLOG("SetSelectionSource " << DRX::Name(this) << ": " << fmts);
	sel_formats = Split(fmts, ';');
	sel_ctrl = this;
	XSetSelectionOwner(Xdisplay, XAtom("PRIMARY"), xclipboard().win, CurrentTime);
}

Ctrl::Xclipboard::Xclipboard()
{
	GuiLock __;
	XSetWindowAttributes swa;
	win = XCreateWindow(Xdisplay, RootWindow(Xdisplay, Xscreenno),
	                    0, 0, 10, 10, 0, CopyFromParent, InputOnly, CopyFromParent,
	                    0, &swa);
	XSelectInput(Xdisplay, win, PropertyChangeMask);
}

Ctrl::Xclipboard::~Xclipboard()
{
	GuiLock __;
	XDestroyWindow(Xdisplay, win);
}

void Ctrl::Xclipboard::Write(i32 fmt, const ClipData& _data)
{
	GuiLock __;
	LLOG("SetSelectionOwner " << XAtomName(fmt));
	data.GetAdd(fmt) = _data;
	XSetSelectionOwner(Xdisplay, XAtom("CLIPBOARD"), win, CurrentTime);
}

void Ctrl::Xclipboard::Request(XSelectionRequestEvent *se)
{
	GuiLock __;
	LLOG("Request " << XAtomName(se->target));
	XEvent e;
	e.xselection.type      = SelectionNotify;
	e.xselection.display   = Xdisplay;
	e.xselection.requestor = se->requestor;
	e.xselection.selection = se->selection;
	e.xselection.target    = se->target;
	e.xselection.time      = se->time;
	e.xselection.property  = se->property;
	if(se->target == XAtom("TARGETS")) {
		LLOG("Request targets:");
		if(se->selection == XAtom("PRIMARY")) {
			Buffer<Atom> x(sel_formats.GetCount());
			for(i32 i = 0; i < sel_formats.GetCount(); i++) {
				x[i] = XAtom(sel_formats[i]);
				LLOG('\t' << sel_formats[i]);
			}
			XChangeProperty(Xdisplay, se->requestor, se->property, XAtom("ATOM"),
			                32, 0, (u8*)~x,
			                sel_formats.GetCount());
		}
		else {
			Buffer<Atom> x(data.GetCount());
			for(i32 i = 0; i < data.GetCount(); i++) {
				x[i] = data.GetKey(i);
				LLOG('\t' << XAtomName(x[i]));
			}
			XChangeProperty(Xdisplay, se->requestor, se->property, XAtom("ATOM"),
			                32, 0, (u8*)~x,
			                data.GetCount());
		}
	}
	else {
		if(se->selection == XAtom("PRIMARY")) {
			LLOG("Request PRIMARY data " << XAtomName(se->target));
			Txt fmt = XAtomName(se->target);
			i32 i = sel_formats.Find(fmt);
			if(i >= 0 && sel_ctrl) {
				Txt d = sel_ctrl->GetSelectionData(fmt);
				XChangeProperty(Xdisplay, se->requestor, se->property, se->target, 8, PropModeReplace,
				                d, d.GetLength());
			}
			else
			    e.xselection.property = None;
		}
		else {
			LLOG("Request CLIPBOARD data " << XAtomName(se->target));
			i32 i = data.Find(se->target);
			if(i >= 0) {
				Txt d = data[i].Render();
				XChangeProperty(Xdisplay, se->requestor, se->property, se->target, 8, PropModeReplace,
				                d, d.GetLength());
			}
			else
			    e.xselection.property = None;
		}
	}
	XSendEvent(Xdisplay, se->requestor, XFalse, 0, &e);
}

Txt Ctrl::Xclipboard::Read(i32 fmt, i32 selection, i32 property)
{
	GuiLock __;
	if(data.GetCount() && (dword)selection == XAtom("CLIPBOARD")) {
		i32 q = data.Find(fmt);
		return q >= 0 ? data[q].Render() : Txt();
	}
	if(sel_ctrl && (dword)selection == XAtom("PRIMARY"))
		return sel_ctrl->GetSelectionData(XAtomName(fmt));
	XConvertSelection(Xdisplay, selection, fmt, property, win, CurrentTime);
	XFlush(Xdisplay);
	XEvent event;
	for(i32 i = 0; i < 20; i++) {
		if(XCheckTypedWindowEvent(Xdisplay, win, SelectionNotify, &event)) {
			if(event.xselection.property != None) {
				XSync(Xdisplay, false);
				return ReadPropertyData(win, event.xselection.property);
			}
			return Null;
		}
		if(XCheckTypedWindowEvent(Xdisplay, win, SelectionRequest, &event) &&
		   event.xselectionrequest.owner == win)
			Request(&event.xselectionrequest);
		if(XCheckTypedWindowEvent(Xdisplay, win, SelectionClear, &event) &&
		   event.xselectionclear.window == win) {
			if(event.xselectionclear.selection == XAtom("CLIPBOARD"))
				Clear();
			if(event.xselectionclear.selection == XAtom("PRIMARY")) {
				sel_ctrl = NULL;
				sel_formats.Clear();
			}
		}
		Sleep(10);
	}
	return Null;
}

Ctrl::Xclipboard& Ctrl::xclipboard()
{
	static Xclipboard xc;
	return xc;
}

void ClearClipboard()
{
	GuiLock __;
	Ctrl::xclipboard().Clear();
}

void AppendClipboard(tukk format, const Val& data, Txt (*render)(const Val& data))
{
	GuiLock __;
	Vec<Txt> s = Split(format, ';');
	for(i32 i = 0; i < s.GetCount(); i++)
		Ctrl::xclipboard().Write(XAtom(s[i]), ClipData(data, render));
}

Txt sRawClipData(const Val& data);

void AppendClipboard(tukk fmt, const Txt& data)
{
	GuiLock __;
	AppendClipboard(fmt, data, sRawClipData);
}

Txt ReadClipboard(tukk fmt)
{
	GuiLock __;
	return Ctrl::xclipboard().Read(XAtom(fmt), XAtom("CLIPBOARD"), XAtom("CLIPDATA"));
}

void AppendClipboardText(const Txt& s)
{
	GuiLock __;
	AppendClipboard("STRING", s);
}

Txt ReadClipboardText()
{
	GuiLock __;
	return ReadClipboard("STRING");
}

void AppendClipboardUnicodeText(const WTxt& s)
{
	GuiLock __;
	AppendClipboard("UTF8_TXT", ToUtf8(s));
}

WTxt ReadClipboardUnicodeText()
{
	GuiLock __;
	return ToUtf32(ReadClipboard("UTF8_TXT"));
}

bool Ctrl::Xclipboard::IsAvailable(i32 fmt, tukk type)
{
	GuiLock __;
	if(data.GetCount())
		return data.Find(fmt) >= 0;
	Txt formats = Read(XAtom("TARGETS"), XAtom(type), XAtom("CLIPDATA"));
	i32 c = formats.GetCount() / sizeof(Atom);
	const Atom *m = (Atom *) ~formats;
	for(i32 i = 0; i < c; i++) {
		if(m[i] == (dword)fmt)
			return true;
	}
	return false;
}

bool Ctrl::ClipHas(i32 type, tukk fmt)
{
	GuiLock __;
	LLOG("ClipHas " << type << ": " << fmt);
	if(strcmp(fmt, "files") == 0)
		fmt = "text/uri-list";
	if(type == 0)
		return Ctrl::xclipboard().IsAvailable(XAtom(fmt), "CLIPBOARD");
	if(type == 2) {
		if(sel_ctrl)
			return sel_formats.Find(fmt) >= 0;
		return Ctrl::xclipboard().IsAvailable(XAtom(fmt), "PRIMARY");
	}
	return drop_formats.Find(fmt) >= 0;
}

Txt DnDGetData(const Txt& f);

Txt Ctrl::ClipGet(i32 type, tukk fmt)
{
	GuiLock __;
	LLOG("ClipGet " << type << ": " << fmt);
	if(strcmp(fmt, "files") == 0)
		fmt = "text/uri-list";
	if(type && GetDragAndDropSource())
		return DnDGetData(fmt);
	return Ctrl::xclipboard().Read(
	           XAtom(fmt),
	           XAtom(type == 2 ? "PRIMARY" : type == 1 ? "XdndSelection" : "CLIPBOARD"),
	           type == 1 ? XA_SECONDARY : XAtom("CLIPDATA")
	       );
}

tukk ClipFmtsText()
{
	return "TXT;UTF8_TXT;text/plain;text/unicode";
}

Txt GetTxt(PasteClip& clip)
{
	GuiLock __;
	if(clip.Accept("STRING") || clip.Accept("text/plain"))
		return ~clip;
	if(clip.Accept("UTF8_TXT"))
		return ToUtf32(~clip).ToTxt();
	return Null;
}

WTxt GetWTxt(PasteClip& clip)
{
	GuiLock __;
	if(clip.Accept("UTF8_TXT"))
		return ToUtf32(~clip);
	if(clip.Accept("STRING") || clip.Accept("text/plain"))
		return ToUnicode(~clip, CHARSET_ISO8859_1);
	return Null;
}

Txt GetTextClip(const WTxt& text, const Txt& fmt)
{
	GuiLock __;
	if(fmt == "STRING" || fmt == "text/plain")
		return text.ToTxt();
	if(fmt == "UTF8_TXT")
		return ToUtf8(text);
	return Null;
}

Txt GetTextClip(const Txt& text, const Txt& fmt)
{
	GuiLock __;
	if(fmt == "STRING" || fmt == "text/plain")
		return text;
	if(fmt == "UTF8_TXT")
		return ToUtf8(text.ToWTxt());
	return Null;
}

bool AcceptText(PasteClip& clip)
{
	GuiLock __;
	return clip.Accept(ClipFmtsText());
}

void Append(VecMap<Txt, ClipData>& data, const Txt& text) // optimize
{
	GuiLock __;
	data.GetAdd("STRING", text);
	data.GetAdd("text/plain", text);
	data.GetAdd("UTF8_TXT", ToUtf8(text.ToWTxt()));
}

void Append(VecMap<Txt, ClipData>& data, const WTxt& text) // optimize
{
	GuiLock __;
	data.GetAdd("STRING", text.ToTxt());
	data.GetAdd("text/plain", text.ToTxt());
	data.GetAdd("UTF8_TXT", ToUtf8(text));
}

bool IsClipboardAvailable(tukk fmt)
{
	GuiLock __;
	return Ctrl::xclipboard().IsAvailable(XAtom(fmt), "CLIPBOARD");
}

bool IsClipboardAvailableText()
{
	GuiLock __;
	return IsClipboardAvailable("STRING") ||
	       IsClipboardAvailable("UTF8_TXT") ||
	       IsClipboardAvailable("text/plain") ||
	       IsClipboardAvailable("text/unicode");
}

bool AcceptFiles(PasteClip& clip)
{
	GuiLock __;
	return clip.Accept("text/uri-list");
}

bool IsAvailableFiles(PasteClip& clip)
{
	GuiLock __;
	return clip.IsAvailable("text/uri-list");
}

i32 JustLf(i32 c)
{
	return (u8)c >= 32 || c == '\n' ? c : 0;
}

Vec<Txt> GetClipFiles(const Txt& data)
{
	Vec<Txt> r;
	Vec<Txt> f = Split(Filter(data, JustLf), '\n');
	for(i32 i = 0; i < f.GetCount(); i++)
		if(f[i].StartsWith("file://"))
			r.Add(UrlDecode(f[i].Mid(7)));
	return r;
}

Vec<Txt> GetFiles(PasteClip& clip) {
	GuiLock __;
	Vec<Txt> r;
	if(clip.Accept("text/uri-list")) {
		Txt txt = clip;
		r = GetClipFiles(txt);
	}
	return r;
}

void AppendFiles(VecMap<Txt, ClipData>& data, const Vec<Txt>& files)
{
	GuiLock __;
	if(files.GetCount() == 0)
		return;
	Txt h;
	for(i32 i = 0; i < files.GetCount(); i++)
		h << "file://" << UrlEncode(files[i]) << '\n';
	data.GetAdd("text/uri-list") = h;
}

bool PasteClip::IsAvailable(tukk fmt) const
{
	return Ctrl::ClipHas(type, fmt);
}

Txt PasteClip::Get(tukk fmt) const
{
	return Ctrl::ClipGet(type, fmt);
}

void PasteClip::GuiPlatformConstruct()
{
	type = 0;
}

}

#endif
