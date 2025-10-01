#ifndef _TextDiffCtrl_TextDiffCtrl_h
#define _TextDiffCtrl_TextDiffCtrl_h

#include <drx/CtrlLib/CtrlLib.h>

namespace drx {

#define IMAGECLASS DiffImg
#define IMAGEFILE <drx/TextDiffCtrl/Diff.iml>
#include <drx/Draw/iml_header.h>

class TextSection
{
public:
	TextSection(i32 start1, i32 count1, i32 start2, i32 count2, bool same)
		: start1(start1), count1(count1), start2(start2), count2(count2), same(same) {}

public:
	i32      start1;
	i32      count1;
	i32      start2;
	i32      count2 : 31;
	unsigned same   : 1;
};

Array<TextSection> CompareLineMaps(const Vec<Txt>& l1, const Vec<Txt>& l2);
Vec<Txt>     GetLineMap(Stream& stream);
Vec<Txt>     GetFileLineMap(const Txt& path);
Vec<Txt>     GetTxtLineMap(const Txt &s);

class TextCompareCtrl : public Ctrl {
public:
	virtual void   Paint(Draw& draw);
	virtual void   Layout();
	virtual void   MouseWheel(Point pt, i32 zdelta, dword keyflags);
	virtual void   MouseMove(Point pt, dword keyflags);
	virtual void   LeftDown(Point pt, dword keyflags);
	virtual void   LeftDouble(Point pt, dword keyflags);
	virtual void   LeftUp(Point pt, dword keyflags);
	virtual void   LeftRepeat(Point pt, dword keyflags);
	virtual void   RightDown(Point p, dword keyflags);
	virtual bool   Key(dword key, i32 repcnt);
	virtual void   LostFocus();

private:
	void           SelfScroll();
	void           PairScroll(TextCompareCtrl *ctrl);
	void           UpdateWidth();
	WTxt        ExpandTabs(wtukk line) const;
	i32            MeasureLength(wtukk line) const;
	bool           GetSelection(i32& l, i32& h) const;
	void           DoSelection(i32 y, bool shift);
	void           Copy();
	i32            GetLineNo(i32 y, i32& yy);
	i32            GetMatchLen(wtukk s1, wtukk s2, i32 len);
	bool           LineDiff(bool left, Vec<LineEdit::Highlight>& hln, Color eq_color,
	                        wtukk s1, i32 l1, i32 h1,
	                        wtukk s2, i32 l2, i32 h2, i32 depth);

private:
	struct Line {
		Line() : number(Null), level(0) {}
		i32    number;
		bool   diff;
		Txt text;
		i32    level;
		Txt text_diff;
		i32    number_diff;
	};
	Array<Line>    lines;
	i32            maxwidth;
	ScrollBars     scroll;
	Font           font;
	Font           number_font;
	Color          number_bg;
	Color          gutter_fg;
	Color          gutter_bg;
	Size           letter;
	i32            tabsize;
	i32            number_width;
	i32            number_yshift;
	i32            gutter_width;
	i32            cursor;
	i32            anchor;
	bool           gutter_capture;
	bool           show_line_number;
	bool           show_white_space;
	bool           show_diff_highlight;
	bool           change_paper_color;
	bool           left = false;

	typedef TextCompareCtrl CLASSNAME;
	
	friend struct TextDiffCtrl;

public:
	Event<>        WhenSel;
	Event<>        WhenScroll;
	Callback2<i32, i32> WhenLeftDouble;
	Event<Vec<LineEdit::Highlight>&, const WTxt&> WhenHighlight;

	void           SetCount(i32 c);
	void           AddCount(i32 c);
	i32            GetCount() const { return lines.GetCount(); }

	void           SetFont(Font f, Font nf);
	void           SetFont(Font f);
	Font           GetFont() const { return font; }
	Font           GetNumberFont() const { return number_font; }

	void           NumberBgColor(Color bg)  { number_bg = bg; Refresh(); }
	Color          GetNumberBgColor() const { return number_bg; }

	void           AutoHideSb(bool ssb=true){ scroll.AutoHide(ssb); }
	void           ShowSb(bool ssb)         { scroll.ShowY(ssb); }
	void           HideSb()                 { ShowSb(false); }

	void           Gutter(i32 size)         { gutter_width = size; Refresh(); }
	void           NoGutter()               { gutter_width = 0; Refresh(); }

	void           TabSize(i32 t);
	i32            GetTabSize() const { return tabsize; }

	void           Set(i32 line, Txt text, bool diff, i32 number, i32 level, Txt text_diff, i32 number_diff);
	Txt         GetText(i32 line) const { return lines[line].text; }
	bool           GetDiff(i32 line) const { return lines[line].diff; }
	i32            GetNumber(i32 line) const { return lines[line].number; }
	i32            GetNumberDiff(i32 line) const { return lines[line].number_diff; }
	bool           HasLine(i32 line) const { return !IsNull(lines[line].number); }

	Point          GetPos() const;
	void           SetPos(Point pos);

	i32            GetSb() const { return scroll.Get().y; }
	void           SetSb(i32 y)  { scroll.Set(0, y); }

	void           ClearSelection()           { cursor = Null; Refresh(); WhenSel(); }
	void           SetSelection(i32 l, i32 h) { cursor = l; anchor = h; Refresh(); WhenSel(); }
	bool           IsSelection() const        { return cursor >= 0; }
	bool           IsSelected(i32 i) const;

	void           ShowLineNumber(bool sln)   { show_line_number = sln; Refresh(); }
	void           HideLineNumber()           { ShowLineNumber(false); }

	void           ShowWhiteSpace(bool sws)   { show_white_space = sws; Refresh(); }
	void           HideWhiteSpace()           { ShowWhiteSpace(false); }

	void           DiffHighlight(bool dh)     { show_diff_highlight = dh; Refresh(); }
	void           NoDiffHighlight()          { DiffHighlight(false); }

	void           ChangePaperColor(bool cpc) { change_paper_color = cpc; Refresh(); }
	void           NoChangePaperColor()       { ChangePaperColor(false); }
	
	void           SetLeft()                  { left = true; }
	
	Txt         RemoveSelected(bool cr);

	Event<>        ScrollWhen(TextCompareCtrl& pair) { return OTVET1(PairScroll, &pair); }

	TextCompareCtrl();
};

struct TextDiffCtrl : public Splitter {
	TextCompareCtrl left;
	TextCompareCtrl right;
	FrameTop<Button> next, prev;

	typedef TextDiffCtrl CLASSNAME;
	
	virtual bool Key(dword key, i32 count);

	void Set(Stream& l, Stream& r);
	void Set(const Txt& l, const Txt& r);
	void InsertFrameLeft(CtrlFrame& f)                     { left.InsertFrame(0, f); }
	void InsertFrameRight(CtrlFrame& f)                    { right.InsertFrame(0, f); }
	void AddFrameLeft(CtrlFrame& f)                        { left.AddFrame(f); }
	void AddFrameRight(CtrlFrame& f)                       { right.AddFrame(f); }
	void SetFont(Font f, Font nf)                          { left.SetFont(f, nf); right.SetFont(f, nf); }
	void SetFont(Font f)                                   { left.SetFont(f); right.SetFont(f); }

	void GetLeftLine(i32 number, i32 line);
	void GetRightLine(i32 number, i32 line);
	
	void FindDiff(bool fw);

	Txt Merge(bool l, bool cr);
	
	i32  GetSc() const                                     { return left.GetSb(); }
	void Sc(i32 sc)                                        { left.SetSb(sc); }

	Callback1<i32> WhenLeftLine;
	Callback1<i32> WhenRightLine;

	TextDiffCtrl();
};

struct DiffDlg : public TopWindow {
	bool Key(dword key, i32 count) override;

	TextDiffCtrl         diff;
	FrameTop<StaticRect> p;
	DataPusher           l;
	Button               write;
	Button               revert;
	Button               remove;
	Txt               editfile;
	Txt               backup;
	Txt               extfile;

	typedef DiffDlg CLASSNAME;

	void Refresh();
	void Write();
	void Execute(const Txt& f);

	static Event<const Txt&, Vec<LineEdit::Highlight>&, const WTxt&> WhenHighlight;

	DiffDlg();
};

bool HasCrs(const Txt& path);

FileSel& DiffFs();

struct FileDiff : DiffDlg {
	FrameTop<DataPusher> r;

	virtual void Open();
	void Execute(const Txt& f);

	typedef FileDiff CLASSNAME;

	Txt GetExtPath() const { return ~r; }

	FileDiff(FileSel& fs);

	FileSel& fs;

	void Execute(const Txt& lpath, const Txt& rpath);
};

class DirDiffDlg : public TopWindow {
public:
	virtual bool HotKey(dword key);
	virtual bool Key(dword key, i32 count);

protected:
	Splitter                   files_diff;
	ParentCtrl                 files_pane;
	FileList                   files;
	DropList                   recent;

	SelectDirButton            seldir1;
	WithDropChoice<EditTxt> dir1;
	SelectDirButton            seldir2;
	WithDropChoice<EditTxt> dir2;
	Option                     hidden;
	Option                     split_lines;
	Button                     compare;
	
	Option                     removed, added, modified;
	EditTxt				   find;
	Button					   clearFind;

	FrameTop<ParentCtrl>       left, right;
	EditTxt                 lfile, rfile;
	Button                     copyleft, copyright;
	Button                     revertleft, revertright;
	Button                     removeleft, removeright;
	
	VecMap<Txt, Txt>  backup;
	
	enum { NORMAL_FILE, DELETED_FILE, NEW_FILE, FAILED_FILE, PATCHED_FILE };
	
	Array<Tuple<Txt, Txt, Txt, i32>> list;

	static bool FileEqual(const Txt& f1, const Txt& f2, i32& n);

	void GatherFilesDeep(Index<Txt>& files, const Txt& base, const Txt& path);
	void Compare();
	void ShowResult();
	void ClearFiles();
	virtual void File();
	void Refresh();
	void Backup(const Txt& path);
	void Copy(bool left);
	FileList::File MakeFile(i32 i);
	
	friend class PatchDiff;

public:
	Function<Image(tukk path)> WhenIcon;

	TextDiffCtrl               diff;

	typedef DirDiffDlg CLASSNAME;

	void SetFont(Font fnt)                      { diff.SetFont(fnt); }
	void Dir1(const Txt& dir)                { dir1 <<= dir; }
	void Dir2(const Txt& dir)                { dir2 <<= dir; }
	void Dir1AddList(const Txt& dir)         { dir1.AddList(dir); }
	void Dir2AddList(const Txt& dir)         { dir2.AddList(dir); }

	Txt GetLeftFile() const                  { return ~lfile; }
	Txt GetRightFile() const                 { return ~rfile; }

	DirDiffDlg();
};

struct Patch {
	struct Chunk {
		i32            line;
		Vec<Txt> orig;
		Vec<Txt> patch;
		Txt         src;
	};
	
	VecMap<Txt, Array<Chunk>> file;
	Txt  common_path;
	Txt  target_dir;

	static i32 MatchLen(const Txt& a, const Txt& b);
	i32        MatchCount(tukk dir);
	bool       Load0(Stream& in, Progress& pi);

public:
	bool Load(Stream& in, Progress& pi);
	bool Load(tukk fn, Progress& pi);
	bool MatchFiles(const Vec<Txt>& dir, Progress& pi);
	void SetTargetDir(tukk dir) { target_dir = AppendFName(dir, common_path); }
	
	i32    GetCount() const            { return file.GetCount(); }
	Txt GetTargetDir() const        { return target_dir; }
	Txt GetFile(i32 i) const        { return file.GetKey(i); }
	Txt GetPath(i32 i) const        { return AppendFName(target_dir, file.GetKey(i)); }
	Txt GetPatch(i32 i) const;
	Txt GetPatchedFile(i32 i, const Txt& file) const;
};

class PatchDiff : public DirDiffDlg {
	Patch  patch;
	Txt file_path;
	Txt patched_file;
	
	EditTxt patch_file;
	EditTxt target_dir;
	Label      failed;

	OpenFileButton  selfile;
	SelectDirButton seldir;
	
	i32             failed_count = 0;
	
	void File();
	Txt GetBackup(const Txt& path);
	i32  GetFileIndex() const;

public:
	bool Open(tukk patch_path, const Vec<Txt>& target_dirs);
	
	PatchDiff();
};

};

#endif
