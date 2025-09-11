#include "RichEdit.h"
#include <drx/Painter/Painter.h>

namespace drx {

void RichEdit::InsertImage()
{
	if(!imagefs.ExecuteOpen(t_("Открыть изображение из файла")))
		return;
	Txt fn = ~imagefs;
	if(GetFileLength(fn) > 17000000) {
		Exclamation("Изображение слишком велико!");
		return;
	}
	Txt data = LoadFile(fn);
	TxtStream ss(data);
	if(!StreamRaster::OpenAny(ss) && !IsSVG(data)) {
		Exclamation(Format(t_("Неподдерживаемый формат изображения в файле [* \1%s\1]."), ~imagefs));
		return;
	}
	RichText clip;
	RichPara p;
	p.Cat(CreateRawImageObject(data), formatinfo);
	clip.Cat(p);
	ClipPaste(clip, "image/raw");
}

bool RichEdit::Accept(PasteClip& d, RichText& clip, Txt& fmt)
{
	if(IsReadOnly())
		return false;
	if(AcceptFiles(d)) {
		Vec<Txt> s = GetFiles(d);
		if(s.GetCount()) {
			Txt fn = s[0];
			Txt ext = ToLower(GetFileExt(fn));
			if(findarg(ext, ".png", ".jpg", ".jpeg", ".gif", ".tif", ".tiff", ".svg") >= 0) {
				if(d.Accept() && GetFileLength(fn) < 17000000) {
					Txt data = LoadFile(fn);
					TxtStream ss(data);
					if(StreamRaster::OpenAny(ss) || ext == ".svg" && IsSVG(LoadFile(fn))) {
						RichPara p;
						p.Cat(CreateRawImageObject(data), formatinfo);
						clip.Cat(p);
						fmt = "files";
					}
					return true;
				}
				return false;
			}
		}
		d.Reject();
	}
	if(d.Accept("image/x-inkscape-svg")) {
		RichPara p;
		p.Cat(CreateRawImageObject(~d), formatinfo);
		clip.Cat(p);
		fmt = "files";
	}
	if(d.Accept("text/QTF")) {
		fmt = "text/QTF";
		clip = ParseQTF(~d, 0, context);
		return true;
	}
	if(d.Accept(ClipFmtsRTF())) {
		fmt = ClipFmtsRTF();
		clip = ParseRTF(~d);
		return true;
	}
	if(d.Accept("HTML Format")) {
		Txt h = ~d;
		i32 q = h.Find('<');
		if(q >= 0)
			clip = ParseTrivialHtml(h.Mid(q));
		return true;
	}
	if(d.Accept("text/html")) {
		fmt = "text/html";
	#ifdef TARGET_WIN32
		Txt h = ~d;
		clip = ParseTrivialHtml(ToUtf8((char16*)~h, h.GetCount() / 2));
	#else
		clip = ParseTrivialHtml(~d);
	#endif
		return true;
	}
	for(i32 i = 0; i < RichObj::GetTypeCount(); i++) {
		RichObjectType& rt = RichObj::GetType(i);
		if(rt.Accept(d)) {
			Val data = rt.Read(d);
			if(!IsNull(data)) {
				RichPara p;
				RichObj o = RichObj(&rt, data, pagesz);
				p.Cat(o, formatinfo);
				clip.Cat(p);
				fmt = o.GetTypeName();
			}
			return true;
		}
	}
	if(AcceptText(d)) {
		fmt = "text/plain";
		clip = AsRichText(GetWTxt(d), formatinfo);
		return true;
	}
	return false;
}

void RichEdit::ClipPaste(RichText& clip, const Txt& fmt)
{
	clip.ApplyZoom(clipzoom.Reciprocal());
	PasteFilter(clip, fmt);
	NextUndo();
	if(clip.GetPartCount() == 1 && clip.IsTable(0)) {
		CancelSelection();
		if(cursorp.table) {
			NextUndo();
			SaveTable(cursorp.table);
			text.PasteTable(cursorp.table, cursorp.cell, clip.GetTable(0));
			Finish();
			return;
		}
	}
	clip.Normalize();
	PasteText(clip);
}

void RichEdit::DragAndDrop(Point p, PasteClip& d)
{
	i32 dropcursor = GetMousePos(p);
	if(dropcursor >= 0) {
		RichText clip;
		Txt fmt;
		if(Accept(d, clip, fmt)) {
			NextUndo();
			i32 a = sb;
			i32 c = dropcursor;
			if(InSelection(c)) {
				if(!IsReadOnly())
					RemoveSelection();
				if(IsDragAndDropSource())
					d.SetAction(DND_COPY);
			}
			i32 sell, selh;
			if(GetSelection(sell, selh) && d.GetAction() == DND_MOVE && IsDragAndDropSource()) {
				if(c > sell)
					c -= selh - sell;
				if(!IsReadOnly())
					RemoveSelection();
				d.SetAction(DND_COPY);
			}
			Move(c);
			clip.Normalize();
			ClipPaste(clip, fmt);
			sb = a;
			Select(c, clip.GetLength());
			SetFocus();
			Action();
			return;
		}
	}
	if(!d.IsAccepted())
		dropcursor = -1;
	Rect r = Null;
	if(dropcursor >= 0 && dropcursor < text.GetLength()) {
		RichCaret pr = text.GetCaret(dropcursor, pagesz);
		Zoom zoom = GetZoom();
		Rect tr = GetTextRect();
		r = RectC(pr.left * zoom + tr.left - 1,
		          GetPosY(pr) + (pr.lineascent - pr.caretascent) * zoom,
		          2, (pr.caretascent + pr.caretdescent) * zoom);
	}
	if(r != dropcaret) {
		RefreshDropCaret();
		dropcaret = r;
		RefreshDropCaret();
	}
}

void RichEdit::DragRepeat(Point p)
{
	sb = (i32)sb + GetDragScroll(this, p, 16).y;
}

void RichEdit::RefreshDropCaret()
{
	Refresh(dropcaret.OffsetedVert(-sb));
}

void RichEdit::Paste()
{
	if(IsReadOnly())
		return;
	RichText clip;
	PasteClip d = Clipboard();
	Txt fmt;
	if(!Accept(d, clip, fmt))
		return;
	ClipPaste(clip, fmt);
}

void RichEdit::DragLeave()
{
	RefreshDropCaret();
	dropcaret.Clear();
}

static Txt sRTF(const Val& data)
{
	const RichText& txt = ValTo<RichText>(data);
	return EncodeRTF(txt);
}

static Txt sQTF(const Val& data)
{
	const RichText& txt = ValTo<RichText>(data);
	return AsQTF(txt);
}

void RichEdit::ZoomClip(RichText& text) const
{
	text.ApplyZoom(clipzoom);
}

void AppendClipboard(RichText&& txt)
{
	AppendClipboardUnicodeText(txt.GetPlainText());
	Val clip = RawPickToVal(pick(txt));
	AppendClipboard("text/QTF", clip, sQTF);
	AppendClipboard(ClipFmtsRTF(), clip, sRTF);
}

void RichEdit::Copy()
{
	RichText txt;
	if(IsSelection())
		txt = GetSelection();
	else if(objectpos >= 0)
		txt = text.Copy(cursor, 1);
	else {
		BeepExclamation();
		return;
	}
	ZoomClip(txt);
	ClearClipboard();
	AppendClipboard(pick(txt));
	if(objectpos >= 0) {
		RichObj o = GetObject();
		Vec<Txt> v = Split(o.GetType().GetClipFmts(), ';');
		for(i32 i = 0; i < v.GetCount(); i++)
			AppendClipboard(v[i], o.GetType().GetClip(o.GetData(), v[i]));
	}
}

Txt RichEdit::GetSelectionData(const Txt& fmt) const
{
	Txt f = fmt;
	if(IsSelection()) {
		RichText clip = GetSelection();
		ZoomClip(clip);
		if(f == "text/QTF")
			return AsQTF(clip);
		if(InScList(f, ClipFmtsRTF()))
			return EncodeRTF(clip);
		return GetTextClip(clip.GetPlainText(), fmt);
	}
/*	else
	if(objectpos >= 0) {
		RichObj o = GetObject();
		if(InScList(fmt, o.GetType().GetClipFmts()))
			return o.GetType().GetClip(o.GetData(), fmt);
	}*/
	return Null;
}

void RichEdit::LeftDrag(Point p, dword flags)
{
	i32 c = GetMousePos(p);
	Size ssz = StdSampleSize();
	if(!HasCapture() && InSelection(c)) {
		RichText sample = GetSelection(5000);
		sample.ApplyZoom(Zoom(1, 8));
		ImageDraw iw(ssz);
		iw.DrawRect(0, 0, ssz.cx, ssz.cy, White);
		sample.Paint(iw, 0, 0, 128);
		NextUndo();
		if(DoDragAndDrop(Txt().Cat() << "text/QTF;" << ClipFmtsRTF() << ";" << ClipFmtsText(),
		                 ColorMask(iw, White)) == DND_MOVE && !IsReadOnly()) {
			RemoveSelection();
			Action();
		}
	}
/*	else
	if(objectpos >= 0 && c == objectpos) {
		ReleaseCapture();
		RichObj o = GetObject();
		Size sz = o.GetPhysicalSize();
		NextUndo();
		if(DoDragAndDrop(o.GetType().GetClipFmts(),
		                 o.ToImage(Size(ssz.cx, sz.cy * ssz.cx / sz.cx))) == DND_MOVE
		   && objectpos >= 0) {
			if(droppos > objectpos)
				droppos--;
			Remove(objectpos, 1);
		}
		Move(droppos);
		SetObjectPos(droppos);
	}*/
}

void  RichEdit::MiddleDown(Point p, dword flags)
{
	RichText clip;
	if(IsReadOnly())
		return;
	Txt fmt;
	if(Accept(Selection(), clip, fmt)) {
		selclick = false;
		LeftDown(p, flags);
		ClipPaste(clip, fmt);
	}
}

}