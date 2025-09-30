#include "CodeEditor.h"

namespace drx {

void Renumber(LineInfo& lf)
{
	LineInfo tf;
	i32 l = 0;
	if(lf.GetCount()) {
		LineInfoRecord& t = tf.Add();
		t.breakpoint = lf[0].breakpoint;
		t.lineno = 0;
		t.count = lf[0].count;
		t.firstedited = lf[0].firstedited;
		t.edited = lf[0].edited;
		l += t.count;
	}
	for(i32 i = 1; i < lf.GetCount(); i++) {
		LineInfoRecord& r = lf[i];
		if(r.breakpoint.IsEmpty() && r.edited == 0 &&
			tf.Top().breakpoint.IsEmpty() && tf.Top().edited == 0)
			tf.Top().count += r.count;
		else {
			LineInfoRecord& t = tf.Add();
			t.breakpoint = r.breakpoint;
			t.firstedited = r.firstedited;
			t.edited = r.edited;
			t.count = r.count;
			t.lineno = l;
		}
		l += r.count;
	}
	lf = pick(tf);
}

void ClearBreakpoints(LineInfo& lf)
{
	for(i32 i = 0; i < lf.GetCount(); i++)
		lf[i].breakpoint.Clear();
}

void ValidateBreakpoints(LineInfo& lf)
{
	for(i32 i = 0; i < lf.GetCount(); i++)
		if(lf[i].breakpoint[0] == 0xe)
			lf[i].breakpoint = "1";
}

void EditorBar::sPaintImage(Draw& w, i32 y, i32 fy, const Image& img)
{
	w.DrawImage(0, y + (fy - img.GetSize().cy) / 2, img);
}

void EditorBar::Paint(Draw& w)
{
	Color bg = IsDarkTheme() ? GrayColor(70) : SColorLtFace();
	Size sz = GetSize();
	w.DrawRect(0, 0, sz.cx, sz.cy, bg);
	for(i32 i = 0; i < animate.GetCount(); i++)
		w.DrawRect(i, 0, 1, sz.cy, animate[i]);
	if(!editor) return;
	for(i32 pass = 0; pass < 2; pass++) {
		i32 fy = editor->GetFontSize().cy;
		i32 hy = fy >> 1;
		i32 y = 0;
		i32 i = editor->GetScrollPos().y;
		i32 cy = GetSize().cy;
		Txt hl = editor->GetHighlight();
		bool hi_if = (hilite_if_endif && findarg(hl, "cpp", "cs", "java") >= 0);
		Vec<IfState> previf;
		if(hi_if)
			previf <<= editor->GetIfStack(i);
		i32 ptri[2];
		for(i32 q = 0; q < 2; q++)
			ptri[q] = ptrline[q] >= 0 ? GetLineNo(ptrline[q]) : -1;
		while(y < cy) {
			Txt b;
			i32 err = 0;
			i32 edit = 0;
			Txt ann;
			Image  icon;
			if(i < li.GetCount()) {
				const LnInfo& l = li[i];
				b = l.breakpoint;
				edit = l.edited;
				icon = l.icon;
				ann = l.annotation;
			}
			if(pass == 0) {
				if(editor->GetCaret().top == y && editor->barline)
					w.DrawRect(0, y, sz.cx, fy, Blend(SColorHighlight(), bg, 200));
				if(line_numbers && i < editor->GetLineCount()) {
					Txt n = AsTxt((i + 1) % 1000000);
					Font fnt = editor->GetFont();
					Size tsz = GetTextSize(n, fnt);
					w.DrawText(sz.cx - Zx(4 + 12) - tsz.cx, y + (fy - tsz.cy) / 2, n, fnt, SBrown());
				}
				if(hi_if) {
					Vec<IfState> nextif;
					if(i < li.GetCount())
						nextif <<= editor->GetIfStack(i + 1);
					i32 pifl = previf.GetCount(), nifl = nextif.GetCount();
					i32 dif = max(pifl, nifl);
					if(--dif >= 0) {
						char p = (dif < pifl ? previf[dif].state : 0);
						char n = (dif < nifl ? nextif[dif].state : 0);
						i32 wd = min(2 * (dif + 1), sz.cx);
						i32 x = sz.cx - wd;
						Color cn = EditorSyntax::IfColor(n);
						if(p == n)
							w.DrawRect(x, y, 1, fy, cn);
						else {
							Color cp = EditorSyntax::IfColor(p);
							w.DrawRect(x, y, 1, hy, cp);
							w.DrawRect(x, y + hy, wd, 1, Nvl(cn, cp));
							w.DrawRect(x, y + hy, 1, fy - hy, cn);
							if(--dif >= 0) {
								x = sz.cx - min(2 * (dif + 1), sz.cx);
								if(!p)
									w.DrawRect(x, y, 1, hy, EditorSyntax::IfColor(dif < pifl ? previf[dif].state : 0));
								if(!n)
									w.DrawRect(x, y + hy, 1, fy - hy, EditorSyntax::IfColor(dif < nifl ? nextif[dif].state : 0));
							}
						}
					}
					previf = pick(nextif);
				}
				if(editor->GetMarkLines()) {
					i32 width = CodeEditorImg::Breakpoint().GetWidth() >> 1;
					if(edit)
					{
						i32 age = (i32)(log((double)(editor->GetUndoCount() + 1 - edit)) * 30);
						w.DrawRect(0, y, width, fy, Blend(SLtBlue(), bg, min(220, age)));
					}
					if(err)
						w.DrawRect(width, y, width, fy, err == 1 ? LtRed() : (err == 2 ? Color(255, 175, 0) : SGreen()));
				}
			}

			if(pass == 1) {
				if(!b.IsEmpty())
					sPaintImage(w, y, fy, b == "1"   ? CodeEditorImg::Breakpoint() :
					                      b == "\xe" ? CodeEditorImg::InvalidBreakpoint() :
					                                   CodeEditorImg::CondBreakpoint());
				for(i32 q = 0; q < 2; q++)
					if(ptri[q] == i)
						sPaintImage(w, y, fy, ptrimg[q]);

				if(annotations && !IsNull(icon))
					w.DrawImage(sz.cx - annotations, y + (fy - icon.GetSize().cy) / 2, icon);
			}

			y += fy;
			i++;
		}
		if(pass == 0)
			w.DrawImage(0, 0, status_image);
	}
}

void EditorBar::MouseMove(Point p, dword flags)
{
	i32 pa = active_annotation;
	if(p.x > GetSize().cx - annotations)
		active_annotation = p.y / editor->GetFont().Info().GetHeight() + editor->GetScrollPos().y;
	else
		active_annotation = -1;
	if(active_annotation >= editor->GetLineCount())
		active_annotation = -1;
	if(pa != active_annotation)
		WhenAnnotationMove();
	if(editor)
		editor->MouseMove(Point(0, p.y), flags);
}

void EditorBar::MouseLeave()
{
	i32 pa = active_annotation;
	active_annotation = -1;
	if(pa != active_annotation)
		WhenAnnotationMove();
}

void EditorBar::LeftDown(Point p, dword flags)
{
	if(p.x > GetSize().cx - annotations)
		WhenAnnotationClick();
	else
	if(editor) {
		editor->LeftDown(Point(0, p.y), flags);
		ReleaseCtrlCapture();
	}
}

Txt& EditorBar::PointBreak(i32& y)
{
	y = minmax(y / editor->GetFont().Info().GetHeight()
		+ editor->GetScrollPos().y, 0, editor->GetLineCount());
	static Txt e;
	return y < 100000 ? li.At(y).breakpoint : e;
}

void EditorBar::LeftDouble(Point p, dword flags)
{
	if(!editor || !bingenabled) return;
	Txt& b = PointBreak(p.y);
	if(b.IsEmpty())
		b = "1";
	else
		b.Clear();
	WhenBreakpoint(p.y);
	Refresh();
}

void EditorBar::RightDown(Point p, dword flags)
{
	if(p.x > GetSize().cx - annotations)
		WhenAnnotationRightClick();
}

void EditorBar::MouseWheel(Point p, i32 zdelta, dword keyflags)
{
	if(editor) {
		i32 i = editor->GetScrollPos().y;
		editor->MouseWheel(p, zdelta, keyflags);
		if(i != editor->GetScrollPos().y)
			MouseMove(p, keyflags);
	}
}

void EditorBar::InsertLines(i32 i, i32 count)
{
	li.InsertN(minmax(i + 1, 0, li.GetCount()), max(count, 0));
	if(editor->GetCheckEdited()) {
		if(editor->IsUndoOp() && li_removed.GetCount() >= count) {
			for(i32 t = 0; t < count; t++) {
				li.At(i + t).firstedited = li_removed[li_removed.GetCount() - count + t].firstedited;
				li[i + t].edited = li_removed[li_removed.GetCount() - count + t].edited;
			}
			li_removed.Drop(count);
			SetEdited(i + count, 1);
			ignored_next_edit = true;
		}
		else {
			if (li[i].firstedited == 0) {
				bool fe = li[i].firstedited;
				li.At(i + count).firstedited = fe;
			}
			SetEdited(i + 1, count);
		}
	}
	Refresh();
}

void EditorBar::RemoveLines(i32 i, i32 count)
{
	if(editor->GetCheckEdited() && !editor->IsUndoOp()) {
		for(i32 t = i - 1; t < i + count - 1; t++) {
			LineInfoRemRecord& rm = li_removed.Add();
			rm.firstedited = li[t].firstedited;
			rm.edited = li[t].edited;
		}
		if(li.At(i + count - 1).firstedited)
			next_age = li[i + count - 1].firstedited;
		else
			next_age = editor->GetUndoCount();
	}
	i = minmax(i, 0, li.GetCount());
	li.Remove(i, minmax(count, 0, li.GetCount() - i));
	Refresh();
}

void EditorBar::ClearLines()
{
	li.Clear();
	li.Shrink();
	li_removed.Clear();
	li_removed.Shrink();
	Refresh();
}

LineInfo EditorBar::GetLineInfo() const
{
	LineInfo lf;
	i32 l = -2;
	for(i32 i = 0; i < li.GetCount(); i++) {
		const LnInfo& ln = li[i];
		if(!ln.breakpoint.IsEmpty() || ln.edited) {
			LineInfoRecord& r = lf.Add();
			r.lineno = ln.lineno;
			r.count = 1;
			r.breakpoint = ln.breakpoint;
			r.firstedited = ln.firstedited;
			r.edited = ln.edited;
			l = -2;
		}
		else
		if(ln.lineno != l) {
			LineInfoRecord& r = lf.Add();
			r.lineno = l = ln.lineno;
			r.count = 1;
		}
		else
			lf.Top().count++;
		if(l >= 0) l++;
	}
	return lf;
}

void EditorBar::SetLineInfo(const LineInfo& lf, i32 total)
{
	li.Clear();
	if(lf.GetCount() == 0) {
		for(i32 i = 0; i < total; i++)
			li.Add().lineno = i;
	}
	else {
		for(i32 i = 0; i < lf.GetCount() && (total < 0 || li.GetCount() < total); i++) {
			const LineInfoRecord& r = lf[i];
			i32 l = r.lineno;
			for(i32 j = r.count; j-- && li.GetCount() < total;) {
				LnInfo& ln = li.Add();
				ln.lineno = l;
				ln.breakpoint = r.breakpoint;
				ln.firstedited = r.firstedited;
				ln.edited = r.edited;
				if(l >= 0) l++;
			}
		}
		while(li.GetCount() < total)
			li.Add().lineno = -1;
	}
}

void EditorBar::Renumber(i32 linecount)
{
	li.SetCount(linecount);
	for(i32 i = 0; i < linecount; i++)
		li[i].lineno = i;
}

void EditorBar::ClearBreakpoints()
{
	for(i32 i = 0; i < li.GetCount(); i++)
		li[i].breakpoint.Clear();
	Refresh();
}

void EditorBar::ValidateBreakpoints()
{
	for(i32 i = 0; i < li.GetCount(); i++)
		if(li[i].breakpoint[0] == 0xe)
			li[i].breakpoint = "1";
	Refresh();
}

Txt EditorBar::GetBreakpoint(i32 ln)
{
	return ln < li.GetCount() ? li[ln].breakpoint : Null;
}

void EditorBar::ClearAnnotations()
{
	for(i32 i = 0; i < li.GetCount(); i++) {
		li[i].icon.Clear();
		li[i].annotation.Clear();
	}
}

void EditorBar::SetAnnotation(i32 line, const Image& img, const Txt& ann)
{
	if(line >= 0 && line < li.GetCount()) {
		li[line].icon = img;
		li[line].annotation = ann;
	}
	Refresh();
}

Txt EditorBar::GetAnnotation(i32 line) const
{
	return line >= 0 && line < li.GetCount() ? li[line].annotation : Txt();
}

void EditorBar::SetBreakpoint(i32 ln, const Txt& s)
{
	li.At(ln).breakpoint = s;
	WhenBreakpoint(ln);
}

void EditorBar::SetEdited(i32 ln, i32 count)
{
	if(ignored_next_edit) {
		ignored_next_edit = false;
		return;
	}
	i32 age = editor->GetUndoCount() + 1;
	bool undo = editor->IsUndoOp();
	for(i32 i = 0; i < count; i++) {
		if(undo) {
			if (li.At(ln + i).firstedited >= age - 1) {
				li[ln + i].firstedited = 0;
				li[ln + i].edited = 0;
			}
		}
		else {
			if(next_age) {
				li[ln + i].firstedited = next_age;
				li[ln + i].edited = age;
				next_age = 0;
			}
			else {
				if(li.At(ln + i).firstedited == 0)
					li[ln + i].firstedited = age;
				li[ln + i].edited = age;
			}
		}
	}
	Refresh();
}

void EditorBar::ClearEdited()
{
	for(i32 i = 0; i < li.GetCount(); i++) {
		li.At(i).firstedited = 0;
		li[i].edited = 0;
	}
	li_removed.Clear();
	li_removed.Shrink();
	Refresh();
}

void EditorBar::SetError(i32 ln, i32 err)
{
	li.At(ln).error = err;
}

void EditorBar::ClearErrors(i32 line)
{
	i32 count;
	if(line < 0) {
		line = 0;
		count = li.GetCount();
	}
	else
	if(line >= li.GetCount())
		return;
	else
		count = line + 1;

	for(i32 i = line; i < count; i++)
		li[i].error = 0;
}

i32  EditorBar::GetLineNo(i32 lineno) const {
	for(i32 i = 0; i < li.GetCount(); i++) {
		if(lineno <= li[i].lineno)
			return i;
	}
	return lineno;
}

i32  EditorBar::GetNoLine(i32 line) const {
	i32 n = 0;
	for(i32 i = 0; i < li.GetCount(); i++) {
		if(li[i].lineno >= 0)
			n = li[i].lineno;
		if(i == line) return n;
	}
	return n;
}

void EditorBar::SetPtr(i32 line, const Image& img, i32 i)
{
	ASSERT(i >= 0 && i < 2);
	ptrline[i] = line;
	ptrimg[i] = img;
	Refresh();
}

void EditorBar::HidePtr()
{
	ptrline[0] = ptrline[1] = -1;
	Refresh();
}

void EditorBar::SyncSize()
{
	i32 n = editor ? editor->GetLineCount() : 0;
	i32 i = 0;
	while(n) {
		i++;
		n /= 10;
	}
	i32 w = max(DPI(32), (line_numbers && editor ? editor->GetFont()['0'] * i : 0) + Zx(12 + 4) + annotations);
	if(w != GetWidth())
		Width(w);
	Refresh();
}

void EditorBar::LineNumbers(bool b)
{
	line_numbers = b;
	SyncSize();
}

void EditorBar::Annotations(i32 width)
{
	annotations = width;
	SyncSize();
}

EditorBar::EditorBar()
{
	editor = NULL;
	line_numbers = false;
	bingenabled = true;
	hilite_if_endif = true;
	line_numbers = false;
	annotations = 0;
	ignored_next_edit = false;
	next_age = 0;
	active_annotation = -1;
	SyncSize();
}

void EditorBar::StatusImage(const Image& m)
{
	if(status_image.IsSame(m))
		return;
	status_image = m;
	Refresh();
}

EditorBar::~EditorBar()
{
}

void ClearErrors(LineInfo& li)
{
	for(i32 i = 0; i < li.GetCount(); i++)
		li[i].error = 0;
}

void SetError(LineInfo& li, i32 line, i32 err)
{
	li.At(line).error = err;
}

}