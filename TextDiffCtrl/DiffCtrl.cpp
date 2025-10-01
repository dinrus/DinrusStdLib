#include "TextDiffCtrl.h"

namespace drx {

#define IMAGECLASS DiffImg
#define IMAGEFILE <drx/TextDiffCtrl/Diff.iml>
#include <drx/Draw/iml_source.h>

TextDiffCtrl::TextDiffCtrl()
{
	left.SetLeft();
	left.Gutter(30);
	next.SetImage(DiffImg::Next());
	prev.SetImage(DiffImg::Prev());
	left.scroll.y.AddFrame(prev);
	left.scroll.y.AddFrame(next);
	next << [=, this] { FindDiff(true); };
	prev << [=, this] { FindDiff(false); };
	right.NoGutter();
	Horz(left, right);
	left.WhenScroll = right.ScrollWhen(left);
	right.WhenScroll = left.ScrollWhen(right);
	right.HideSb();
	left.WhenLeftDouble = OTVET(GetLeftLine);
	right.WhenLeftDouble = OTVET(GetRightLine);
}

void TextDiffCtrl::GetLeftLine(i32 number, i32 line)
{
	WhenLeftLine(number);
}

void TextDiffCtrl::GetRightLine(i32 number, i32 line)
{
	WhenRightLine(number);
}

static bool SmallDiff(tukk s1, tukk s2)
{
	for(;;) {
		while(*s1 && (u8)*s1 <= ' ')
			s1++;
		while(*s2 && (u8)*s2 <= ' ')
			s2++;
		if(!*s1 || !*s2)
			return !*s1 && !*s2;
		if(*s1 != *s2)
			return false;
		while(*s1 && *s1 == *s2)
			s1++, s2++;
		if((u8)s1[-1] <= ' ')
			continue;
		if((u8)*s1 > ' ' || (u8)*s2 > ' ')
			return false;
	}
}

void TextDiffCtrl::Set(Stream& l, Stream& r)
{
	Vec<Txt> ll = GetLineMap(l);
	Vec<Txt> rl = GetLineMap(r);
	Array<TextSection> sections = CompareLineMaps(ll, rl);
	i32 outln = 0;
	left.SetCount(0);
	right.SetCount(0);
	i32 firstdiff = -1;
	for(i32 i = 0; i < sections.GetCount(); i++) {
		const TextSection& sec = sections[i];
		bool diff = !sec.same;
		if(firstdiff < 0 && diff)
			firstdiff = outln;
		i32 maxcount = max(sec.count1, sec.count2);
		left.AddCount(maxcount);
		i32 l;
		for(l = 0; l < sec.count1; l++) {
			i32 level = (diff ? l < sec.count2 && SmallDiff(ll[sec.start1 + l], rl[sec.start2 + l]) ? 1 : 2 : 0);
			left.Set(outln + l, ll[sec.start1 + l], diff, sec.start1 + l + 1, level, diff && l < sec.count2 ? rl[sec.start2 + l] : Null, sec.start1 + l + 1);
		}
		for(; l < maxcount; l++)
			left.Set(outln + l, Null, diff, Null, 2, Null, Null);
		right.AddCount(maxcount);
		for(l = 0; l < sec.count2; l++) {
			i32 level = (diff ? l < sec.count1 && SmallDiff(rl[sec.start2 + l], ll[sec.start1 + l]) ? 1 : 2 : 0);
			right.Set(outln + l, rl[sec.start2 + l], diff, sec.start2 + l + 1, level,  diff && l < sec.count1 ? ll[sec.start1 + l] : Null, sec.start2 + l + 1);
		}
		for(; l < maxcount; l++)
			right.Set(outln + l, Null, diff, Null, 2, Null, Null);
		outln += maxcount;
	}
	if(firstdiff >= 0)
		left.SetSb(max(firstdiff - 2, 0));
	left.ClearSelection();
	right.ClearSelection();
}

Txt TextDiffCtrl::Merge(bool l, bool cr)
{
	ASSERT(left.GetCount() == right.GetCount());
	const TextCompareCtrl& target = l ? left : right;
	const TextCompareCtrl& source = l ? right : left;
	Txt r;
	Txt eol = cr ? "\r\n" : "\n";
	for(i32 i = 0; i < target.GetCount(); i++) {
		if(source.IsSelected(i) && source.HasLine(i))
			r << source.GetText(i) << eol;
		else
		if(target.HasLine(i))
			r << target.GetText(i) << eol;
	}
	r.TrimEnd(eol);
	return r;
}

Txt TextCompareCtrl::RemoveSelected(bool cr)
{
	Txt r;
	Txt eol = cr ? "\r\n" : "\n";
	for(i32 i = 0; i < GetCount(); i++)
		if(!IsSelected(i) && HasLine(i))
			r << GetText(i) << eol;
	r.TrimEnd(eol);
	return r;
}

void TextDiffCtrl::Set(const Txt& l, const Txt& r)
{
	TxtStream sl(l);
	TxtStream sr(r);
	Set(sl, sr);
}

void TextDiffCtrl::FindDiff(bool fw)
{
	i32 i = left.scroll.GetY() + (fw ? 2 * left.scroll.GetPage().cy / 3 : -1);
	while(i > 0 && i < left.lines.GetCount()) {
		if(left.lines[i].diff) {
			left.SetSb(max(i - 2, 0));
			return;
		}
		i += fw ? 1 : -1;
	}
}

bool TextDiffCtrl::Key(dword key, i32 count)
{
	switch(key) {
	case K_F3:
		FindDiff(true);
		return true;
	case K_SHIFT_F3:
		FindDiff(false);
		return true;
	}
	return Splitter::Key(key, count);
}

INITBLOCK {
	RegisterGlobalConfig("diff");
}

bool DiffDlg::Key(dword key, i32 count)
{
	switch(key) {
	case K_F5:
	case K_INSERT:
	case K_ENTER:
	case K_SPACE:
		Write();
		return true;
	case K_F8:
	case K_DELETE:
		remove.WhenAction();
		return true;
	}
	return TopWindow::Key(key, count);
}

void DiffDlg::Execute(const Txt& f)
{
	editfile = f;
	l <<= editfile;
	Title(editfile);
	Txt h;
	{
		LoadFromGlobal(h, "diff");
		TxtStream ss(h);
		SerializePlacement(ss);
	}
	TopWindow::Execute();
	{
		TxtStream ss;
		SerializePlacement(ss);
		h = ss;
		StoreToGlobal(h, "diff");
	}
}

void DiffDlg::Refresh()
{
	i32 sc = diff.GetSc();
	diff.Set(LoadFile(editfile), extfile);
	diff.Sc(sc);
}

bool HasCrs(const Txt& path)
{
	FileIn in(path);
	if(in) {
		while(!in.IsEof()) {
			i32 c = in.Get();
			if(c == '\r')
				return true;
			if(c == '\n')
				return false;
		}
	}
	return false;
}

void DiffDlg::Write()
{
	if(diff.right.IsSelection()) {
		SaveFile(editfile, diff.Merge(true, HasCrs(editfile)));
		Refresh();
		revert.Enable();
		return;
	}
	if(PromptYesNo("Хотите перезаписать&[* " + DeQtf(editfile) + "] ?")) {
		SaveFile(editfile, extfile);
		Break(IDOK);
		revert.Enable();
	}
}

Event<const Txt&, Vec<LineEdit::Highlight>&, const WTxt&> DiffDlg::WhenHighlight;

DiffDlg::DiffDlg()
{
	Add(diff.SizePos());
	Sizeable().Zoomable();
	diff.InsertFrameLeft(p);

	p.Height(EditField::GetStdHeight());
	p.Add(l.VSizePos().HSizePosZ(0, 222));
	p.Add(write.VSizePos().RightPosZ(0, 70));
	p.Add(remove.VSizePos().RightPosZ(74, 70));
	p.Add(revert.VSizePos().RightPosZ(148, 70));

	write << [=, this] { Write(); };
	write.SetLabel("Копировать");
	write.SetImage(DiffImg::CopyLeft());
	write.Tip("F5");

	revert.Disable();
	revert.SetLabel("Отменить");
	revert.SetImage(CtrlImg::undo());
	revert << [=, this] {
		if(PromptYesNo("Отменить изменения?")) {
			SaveFile(editfile, backup);
			Refresh();
		}
	};
	
	remove.SetLabel("Удалить");
	remove.SetImage(CtrlImg::remove());
	remove.Tip("F8");
	remove << [=, this] {
		SaveFile(editfile, diff.left.RemoveSelected(HasCrs(editfile)));
		Refresh();
		revert.Enable();
	};
	
	diff.left.WhenSel << [=, this] {
		remove.Enable(diff.left.IsSelection());
	};
	
	l.SetReadOnly();

	diff.right.WhenHighlight = diff.left.WhenHighlight = [=, this](Vec<LineEdit::Highlight>& hl, const WTxt& ln) {
		DiffDlg::WhenHighlight(editfile, hl, ln);
	};
}

void FileDiff::Open()
{
	if(IsNull(r)) {
		if(!fs.ExecuteOpen())
			return;
		r <<= ~fs;
	}
	if(IsNull(r))
		return;
	backup = LoadFile(editfile);
	diff.Set(backup, extfile = LoadFile(~~r));
}

void FileDiff::Execute(const Txt& f)
{
	editfile = f;
	Open();
	if(IsNull(r))
		return;
	DiffDlg::Execute(f);
}

FileDiff::FileDiff(FileSel& fs_)
: fs(fs_)
{
	r.Height(EditField::GetStdHeight());
	Icon(DiffImg::Diff());
	diff.InsertFrameRight(r);
	r <<= OTVET(Open);
}

void FileDiff::Execute(const Txt& lpath, const Txt& rpath)
{
	r <<= rpath;
	Execute(lpath);
}

FileSel& DiffFs() {
	static FileSel fs;
	ONCELOCK {
		fs.Type("Пропатчить файл (*.diff, *.patch)", "*.diff *.patch");
		fs.AllFilesType();
	}
	return fs;
}

};
