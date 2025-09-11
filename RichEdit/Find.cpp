#include <drx/RichEdit/RichEdit.h>

namespace drx {

void RichEdit::FindReplaceAddHistory() {
	if(!Txt(~findreplace.find).IsEmpty())
		findreplace.find.AddHistory();
	if(!Txt(~findreplace.replace).IsEmpty())
		findreplace.replace.AddHistory();
}

void RichEdit::CloseFindReplace()
{
	if(!persistent_findreplace && findreplace.IsOpen())
		findreplace.Close();
}

bool compare3(wtukk s, wtukk a, wtukk b, i32 len)
{
	wtukk e = s + len;
	while(s < e) {
		if(*s != *a && *s != *b)
			return false;
		s++;
		a++;
		b++;
	}
	return true;
}

struct RichFindIterator : RichText::Iterator {
	i32 cursor;
	i32 fpos;
	WTxt upperw, lowerw;
	bool ww;
	i32  len;

	virtual bool operator()(i32 pos, const RichPara& para)
	{
		WTxt ptext = para.GetText();
		if(pos + ptext.GetLength() > cursor && ptext.GetLength() >= len) {
			wtukk q = ptext;
			wtukk e = ptext.End() - len;
			if(cursor >= pos)
				q += cursor - pos;
			while(q <= e) {
				if(compare3(q, upperw, lowerw, len) &&
				   (!ww || (q + len == e || !IsLetter(q[len])) &&
				           (q == ptext || !IsLetter(q[-1])))) {
					fpos = i32(q - ~ptext + pos);
					return true;
				}
				q++;
			}
		}
		return false;
	}
};

i32  RichEdit::FindPos()
{
	RichFindIterator fi;
	WTxt w = findreplace.find.GetText();
	if(findreplace.ignorecase) {
		fi.upperw = ToUpper(w);
		fi.lowerw = ToLower(w);
	}
	else
		fi.upperw = fi.lowerw = w;
	fi.len = w.GetLength();
	fi.ww = findreplace.wholeword;
	if(w.GetLength()) {
		fi.cursor = cursor;
		if(text.Iterate(fi))
			return fi.fpos;
	}
	return -1;
}

void RichEdit::Find()
{
	CancelSelection();
	FindReplaceAddHistory();
	if(notfoundfw)
		Move(0, false);
	found = notfoundfw = false;
	i32 pos = FindPos();
	if(pos >= 0) {
		anchor = pos;
		cursor = pos + findreplace.find.GetText().GetLength();
		Finish();
		found = true;
		Size sz = findreplace.GetSize();
		Rect sw = GetScreenView();
		Rect r = sw.CenterRect(sz);
		Rect cr = GetCaretRect();
		if(cr.top < sz.cy + 2 * cr.Height()) {
			r.bottom = sw.bottom - 8;
			r.top = r.bottom - sz.cy;
		}
		else {
			r.top = sw.top + 24;
			r.bottom = r.top + sz.cy;
		}
		findreplace.SetRect(r);
		if(!findreplace.IsOpen()) {
			findreplace.Open();
		}
		SetFocus();
	}
	else {
		CancelSelection();
		CloseFindReplace();
		notfoundfw = true;
	}
}

RichText RichEdit::ReplaceText()
{
	RichText clip;
	RichPara p;
	formatinfo.ApplyTo(p.format);
	p.part.Add();
	formatinfo.ApplyTo(p[0].format);
	p.part.Top().text = findreplace.replace.GetText();
	clip.Cat(p);
	return clip;
}

void RichEdit::Replace()
{
	NextUndo();
	if(IsSelection() && found) {
		FindReplaceAddHistory();
		i32 c = min(cursor, anchor);
		Remove(c, abs(cursor - anchor));
		anchor = cursor = c;
		Insert(cursor, ReplaceText(), false);
		cursor += findreplace.replace.GetText().GetLength();
		anchor = cursor;
		Finish();
		Find();
	}
}

void RichEdit::OpenFindReplace()
{
	NextUndo();
	if(!findreplace.IsOpen()) {
		Size sz = findreplace.GetSize();
		findreplace.SetRect(GetScreenView().CenterRect(sz));
		i32 l, h;
		if(GetSelection(l, h)) {
			findreplace.amend.Hide();
			findreplace.ok.SetLabel(t_("Заменить"));
			findreplace.Title(t_("Заменить в выделении"));
			findreplace.cancel <<= findreplace.Breaker(IDCANCEL);
			findreplace.ok <<= findreplace.Breaker(IDOK);
			if(findreplace.Execute() == IDOK) {
				i32 len = findreplace.find.GetText().GetLength();
				i32 rlen = findreplace.replace.GetText().GetLength();
				RichText rtext = ReplaceText();
				cursor = l;
				for(;;) {
					i32 pos = FindPos();
					if(pos < 0 || pos + len >= h)
						break;
					Select(pos, len);
					Remove(pos, len);
					Insert(pos, ReplaceText(), false);
					cursor += pos + rlen;
					h += rlen - len;
				}
				CancelSelection();
				Move(h, false);
			}
			FindReplaceAddHistory();
			findreplace.amend.Show();
			findreplace.ok.SetLabel(t_("Найти"));
			findreplace.Title(t_("Найти / Заменить"));
			findreplace.cancel <<= callback(&findreplace, &TopWindow::Close);
			findreplace.ok <<= OTVET(Find);
		}
		else {
			findreplace.Open();
			findreplace.find.SetFocus();
		}
	}
}

}