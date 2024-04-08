#include "TextDiffCtrl.h"

namespace Upp {

DirDiffDlg::DirDiffDlg()
{
	int div = HorzLayoutZoom(4);
	int cy = dir1.GetStdSize().cy;

	int bcx = GetTextSize(t_("Сравнить"), StdFont()).cx * 12 / 10 + 2 * div;

	hidden.SetLabel(t_("Скрытый"));
	split_lines.SetLabel(t_("Разделить длинные строки"));
	
	added.SetColor(Green()).SetLabel(t_("Новый"));
	modified.SetLabel(t_("Изменён"));
	removed.SetColor(Red()).SetLabel(t_("Удалён"));
	
	recent <<= Null;
	recent.Add(Null, "Все");
	recent.Add(1, "1 День");
	recent.Add(3, "3 Дня");
	recent.Add(7, "7 Дней");
	recent.Add(14, "14 Дней");
	recent.Add(32, "28 Дней");
	
	compare.SetLabel(t_("Сравнить"));
	int bcy = max(cy, compare.GetStdSize().cy);
	
	files_pane.Add(dir1.TopPos(0, cy).HSizePos());
	files_pane.Add(dir2.TopPos(cy + div, cy).HSizePos());
	files_pane.Add(hidden.TopPos(2 * cy + 2 * div, bcy).LeftPos(0, bcx));
	files_pane.Add(split_lines.TopPos(2 * cy + 2 * div, bcy).LeftPosZ(52, 100));
	
	files_pane.Add(   added.TopPos(3 * cy + 3 * div, bcy).LeftPosZ(2, 60));
	files_pane.Add(modified.TopPos(3 * cy + 3 * div, bcy).LeftPosZ(52, 70));
	files_pane.Add( removed.TopPos(3 * cy + 3 * div, bcy).LeftPosZ(128, 80));
	files_pane.Add(  recent.TopPos(3 * cy + 3 * div, bcy).RightPos(0, bcx));
	
	removed = 1;
	added = 1;
	modified = 1;
	find.NullText(t_("Найти (Ctrl+F)"));
	clearFind.SetLabel("X");
	clearFind.RightPosZ(1, 16).VSizePosZ(1, 1);
	find.AddChild(&clearFind);
	
	files_pane.Add(compare.TopPos(2 * cy + 2 * div, bcy).RightPos(0, bcx));
	files_pane.Add(files.VSizePos(3 * cy + bcy + 4 * div, Zy(24)).HSizePos());
	files_pane.Add(find.BottomPosZ(4, 19).HSizePosZ());

	Add(files_diff.SizePos());
	files_diff.Set(files_pane, diff);
	files_diff.SetPos(1500);//2000
	files_diff.SetMinPixels(0, Zx(256));
	
	Sizeable().Zoomable();
	
	seldir1.Attach(dir1);
	seldir2.Attach(dir2);
	
	seldir1.Title("Первая сравниваемая папка");
	seldir2.Title("Вторая сравниваемая папка");
	
	compare <<= THISBACK(Compare);
	dir1 <<= THISBACK(ClearFiles);
	dir2 <<= THISBACK(ClearFiles);
	
	modified	<< [=, this] { ShowResult(); };
	removed		<< [=, this] { ShowResult(); };
	added		<< [=, this] { ShowResult(); };
	find		<< [=, this] { ShowResult(); };
	clearFind	<< [=, this] { find.Clear(); ShowResult();};
	
	files.WhenSel = THISBACK(File);

	diff.InsertFrameLeft(left);
	diff.InsertFrameRight(right);

	left.Height(EditField::GetStdHeight());
	lfile.SetReadOnly();
	left.Add(lfile.VSizePos().HSizePosZ(0, 222));
	left.Add(copyright.VSizePos().RightPosZ(0, 70));
	left.Add(removeleft.VSizePos().RightPosZ(74, 70));
	left.Add(revertleft.VSizePos().RightPosZ(148, 70));

	right.Height(EditField::GetStdHeight());
	rfile.SetReadOnly();
	right.Add(rfile.VSizePos().HSizePosZ(222, 0));
	right.Add(copyleft.VSizePos().LeftPosZ(0, 70));
	right.Add(removeright.VSizePos().LeftPosZ(74, 70));
	right.Add(revertright.VSizePos().LeftPosZ(148, 70));
	
	auto SetupCopy = [=, this](Button& copy, bool left) {
		copy.SetImage(left ? DiffImg::CopyLeft() : DiffImg::CopyRight());
		copy.SetLabel("Копировать");
		copy.Tip("F5");
		copy.Disable();
		copy << [=, this] { Copy(left); };
	};
	
	SetupCopy(copyleft, true);
	SetupCopy(copyright, false);

	auto SetupRevert = [=, this](Button& revert, EditString *dir) {
		revert.Disable();
		revert.SetLabel("Отменить");
		revert.SetImage(CtrlImg::undo());
		revert << [=, this] {
			String path = AppendFileName(~*dir, files.GetCurrentName());
			int q = backup.Find(path);
			if(q >= 0 && PromptYesNo("Отменить изменения?")) {
				SaveFile(path, ZDecompress(backup[q]));
				backup.Remove(q);
				Refresh();
			}
		};
	};
	
	SetupRevert(revertleft, &dir1);
	SetupRevert(revertright, &dir2);
	
	auto SetupRemove = [=, this](Button& remove, TextCompareCtrl *text, EditString *dir)
	{
		remove.SetLabel("Удалить");
		remove.Tip("F8");
		remove.SetImage(CtrlImg::remove());
		
		remove << [=, this] {
			String path = AppendFileName(~*dir, files.GetCurrentName());
			Backup(path);
			SaveFile(path, text->RemoveSelected(HasCrs(path)));
			Refresh();
		};
	
		text->WhenSel << [=, &remove] {
			remove.Enable(text->IsSelection());
		};
	};
	
	SetupRemove(removeleft, &diff.left, &dir1);
	SetupRemove(removeright, &diff.right, &dir2);
	
	split_lines << [=, this] { File(); };

	Icon(DiffImg::DirDiff());

	WhenIcon = [](const char *path) -> Image { return NativePathIcon(path); };
	
	Title("Сравнить папки");
};

void DirDiffDlg::GatherFilesDeep(Index<String>& files, const String& base, const String& path)
{
	FindFile ff(AppendFileName(AppendFileName(base, path), "*.*"));
	while(ff) {
		String p = (path.GetCount() ? path + '/' : String()) + ff.GetName();
		if(hidden || !ff.IsHidden()) {
			if(ff.IsFile())
				files.FindAdd(p);
			else
			if(ff.IsFolder())
				GatherFilesDeep(files, base, p);
		}
		ff.Next();
	}
}

bool DirDiffDlg::FileEqual(const String& f1, const String& f2, int& n)
{
	FileIn in1(f1);
	FileIn in2(f2);
	if(in1 && in2) {
		in1.SetBufferSize(256 * 1024);
		in2.SetBufferSize(256 * 1024);
		while(!in1.IsEof() && !in2.IsEof()) {
			String a = in1.GetLine();
			String b = in2.GetLine();
			if(a != b)
				return false;
		}
		return true;
	}
	else
	{
		n = (in1 ? DELETED_FILE : NEW_FILE);
	}
	
	return false;
}

void DirDiffDlg::Compare()
{
	Index<String> fs;
	GatherFilesDeep(fs, ~dir1, Null);
	GatherFilesDeep(fs, ~dir2, Null);

	copyleft.Disable();
	copyright.Disable();
	
	files.Clear();
	Vector<String> f = fs.PickKeys();
	Sort(f);
	Progress pi(t_("Сравниваются.."));
	pi.SetTotal(f.GetCount());
	
	Date dlim = IsNull(recent) ? Null : GetSysDate() - (int)~recent;

	list.Clear();
	for(int i = 0; i < f.GetCount(); i++) {
		if(pi.StepCanceled())
			break;
		String p1 = AppendFileName(~dir1, f[i]);
		String p2 = AppendFileName(~dir2, f[i]);
		int n = NORMAL_FILE;
		if((IsNull(dlim) || FileGetTime(p1) >= dlim || FileGetTime(p2) >= dlim) && !FileEqual(p1, p2, n))
			list.Add(MakeTuple(f[i], p1, p2, n));
	}
	
	ShowResult();
}

FileList::File DirDiffDlg::MakeFile(int i)
{
	static Color cs[] = { SColorText(), SRed(), SGreen(), SRed(), SLtBlue() };
	FileList::File m;
	m.isdir = false;
	m.unixexe = false;
	m.hidden = false;
	Image icn = WhenIcon(FileExists(list[i].b) ? list[i].b : list[i].c);
	int k = list[i].d;
	if(IsNull(icn))
		icn = CtrlImg::File();
	m.icon = decode(k, FAILED_FILE, MakeImage(icn, [] (const Image& m) { return GetOver(m, DiffImg::Failed()); }),
	                   PATCHED_FILE, MakeImage(icn, [] (const Image& m) { return GetOver(m, DiffImg::Patched()); }),
	                   icn);
	m.name = list[i].a;
	m.font = decode(k, FAILED_FILE, StdFont().Strikeout().Italic(),
	                   PATCHED_FILE, StdFont().Italic(), StdFont());
	m.ink = cs[k];
	m.length = 0;
	m.time = Null;
	m.extink = m.ink;
	m.data = i;
	return m;
}

void DirDiffDlg::ShowResult()
{
	files.Clear();
	String sFind = ToLower((String)~find);
	for(int i = 0; i < list.GetCount(); i++)
	{
		int n = list[i].d;
		if((n == NORMAL_FILE && modified || n == DELETED_FILE && removed
		    || n == NEW_FILE && added || n == FAILED_FILE || n == PATCHED_FILE)
		   && ToLower(list[i].a).Find(sFind) >= 0)
			files.Add(MakeFile(i));
	}
	Title(AsString(files.GetCount()) + " файлов");
	clearFind.Show(!IsNull(find));
}

void DirDiffDlg::ClearFiles()
{
	files.Clear();
	compare.Enable(!IsNull(dir1) && !IsNull(dir2));
}

WString ExpandTabs(const wchar *text)
{
	WString out;
	for(wchar c; (c = *text++);)
		if(c == '\t')
			out.Cat(' ', 4 - out.GetLength() % 4);
		else
			out.Cat(c);
	return out;
}

String SplitLines(const String& s)
{
    StringStream ss(s);
    WString result;
    while(!ss.IsEof()) {
        WString l = ExpandTabs(ss.GetLine().ToWString());
        int q = 0;
        while(l.GetCount() - q > 80) {
            result.Cat(~l + q, 80);
            result.Cat('\n');
            q += 80;
        }
        result.Cat(~l + q, l.GetCount() - q);
        result.Cat('\n');
    }
    return result.ToString();
}

void DirDiffDlg::File()
{
	String fn = files.GetCurrentName();
	String p1 = AppendFileName(~dir1, fn);
	String p2 = AppendFileName(~dir2, fn);

	diff.right.WhenHighlight = diff.left.WhenHighlight = [=, this](Vector<LineEdit::Highlight>& hl, const WString& ln) {
		DiffDlg::WhenHighlight(AppendFileName(p1, files.GetCurrentName()), hl, ln);
	};

	diff.Set(Null, Null);
	String f1 = LoadFile(p1);
	String f2 = LoadFile(p2);
	if(split_lines) {
	    f1 = SplitLines(f1);
	    f2 = SplitLines(f2);
	}
	if(GetFileLength(p1) < 4 * 1024 * 1024 && GetFileLength(p2) < 4 * 1024 * 1024)
		diff.Set(f1, f2);
	lfile <<= p1;
	rfile <<= p2;
	copyleft.Enable();
	copyright.Enable();
	revertleft.Enable(backup.Find(p1) >= 0);
	revertright.Enable(backup.Find(p2) >= 0);
}

void DirDiffDlg::Refresh()
{
	int sc = diff.GetSc();
	File();
	diff.Sc(sc);
}

void DirDiffDlg::Backup(const String& path)
{
	int q = backup.Find(path);
	if(q < 0 && GetFileLength(path) < 4 * 1024 * 1024 && backup.GetCount() < 100)
		backup.Add(path, ZCompress(LoadFile(path)));
}

void DirDiffDlg::Copy(bool left)
{
	String src = ~lfile;
	String dst = ~rfile;
	if(left)
		Swap(src, dst);
	if(left ? diff.right.IsSelection() : diff.left.IsSelection()) {
		Backup(dst);
		SaveFile(dst, diff.Merge(left, HasCrs(dst)));
		Refresh();
		return;
	}
	if(PromptYesNo("Копировать [* \1" + src + "\1]&в [* \1" + dst + "\1] ?")) {
		Backup(dst);
		FileIn  in(src);
		FileOut out(dst);
		CopyStream(out, in);
		out.Close();
		Refresh();
	}
}

bool Upp::DirDiffDlg::HotKey(dword key)
{
	if(key == K_CTRL_F) {
		ActiveFocus(find);
		return true;
	}
	return false;
}

bool DirDiffDlg::Key(dword key, int count)
{
	bool left;
	if(diff.left.HasFocus())
		left = true;
	else
	if(diff.right.HasFocus())
		left = false;
	else
		return TopWindow::Key(key, count);
	switch(key) {
	case K_F5:
	case K_INSERT:
	case K_ENTER:
	case K_SPACE:
		(left ? copyright : copyleft).WhenAction();
		return true;
	case K_F8:
	case K_DELETE:
		(left ? removeleft : removeright).WhenAction();
		return true;
	}
	return TopWindow::Key(key, count);
}

};