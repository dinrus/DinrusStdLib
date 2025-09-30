#include "CtrlLib.h"

namespace drx {

i32 GetTextSize(const FontInfo& fi, wtukk text, wtukk end) {
	i32 w = 0;
	while(text < end)
		w += fi[*text++];
	return w;
}

i32 GetTextSize(const FontInfo& fi, const WTxt& text) {
	return GetTextSize(fi, text, text.End());
}

wtukk GetTextFitLim(const FontInfo& fi, wtukk txt, wtukk end, i32& cx) {
	for(;;) {
		if(fi[*txt] > cx || txt >= end)
			return txt;
		cx -= fi[*txt++];
	}
}

i32 GetTextFitCount(const FontInfo& fi, const WTxt& s, i32& cx) {
	return (i32)(GetTextFitLim(fi, s, s.End(), cx) - s.Begin());
}

wtukk strdirsep(wtukk s) {
	while(*s) {
		if(*s == '\\' || *s == '/') return s;
		s++;
	}
	return NULL;
}

void DrawFName(Draw& ww, i32 x0, i32 y, i32 wcx0, i32 cy, const WTxt& mname, bool isdir, Font font,
                  Color ink, Color extink, const WTxt& desc, Font descfont, bool justname, Color uln)
{
	for(i32 pass = IsNull(uln); pass < 2; pass++) {
		NilDraw nd;
		Draw *w = pass ? &ww : &nd;
		FontInfo fi = font.Info();
		i32 extpos = (isdir ? -1 : mname.ReverseFind('.'));
		i32 slash = isdir ? -1 : max(mname.ReverseFind('\\'), mname.ReverseFind('/'));
		if(extpos < slash)
			extpos = -1;
		wtukk ext = extpos >= slash && extpos >= 0 ? mname.Begin() + extpos + 1
		                                                  : mname.End();
		wtukk name = mname;
		if(justname && slash >= 0)
			name += slash + 1;
		i32 txtcx = GetTextSize(fi, name);
		i32 x = x0;
		i32 wcx = wcx0;
		if(txtcx <= wcx) {
			if(pass == 0)
				ww.DrawRect(x0, y + fi.GetAscent() + 1, txtcx, 1, uln);
			ww.DrawText(x, y, name, font, ink, (i32)(ext - name));
			ww.DrawText(x + GetTextSize(fi, name, ext), y, ext, font, extink, (i32)(mname.End() - ext));
			if(!IsEmpty(desc) && pass)
				DrawTextEllipsis(ww, x + GetTextSize(fi, name), y, wcx - txtcx,
				                 desc, "...", descfont, extink);
			x += txtcx;
			return;
		}
		else {
			i32 dot3 = 3 * fi['.'];
			if(2 * dot3 > wcx) {
				i32 n = GetTextFitCount(fi, name, wcx);
				w->DrawText(x, y, name, font, ink, n);
				x += GetTextSize(fi, name, name + n);
			}
			else {
				wtukk end = mname.End();
				i32 dircx = 2 * fi['.'] + fi[DIR_SEP];
				wtukk bk = strdirsep(name);
				if(bk) {
					wcx -= dircx;
					w->DrawText(x, y, ".." DIR_SEPS, font, SColorDisabled, 3);
					x += dircx;
					do {
						txtcx -= GetTextSize(fi, name, bk + 1);
						name = bk + 1;
						if(txtcx < wcx) {
							w->DrawText(x, y, name, font, ink, (i32)(ext - name));
							x += GetTextSize(fi, name, ext);
							w->DrawText(x, y, ext, font, extink, (i32)(end - ext));
							x += GetTextSize(fi, ext, end);
							goto end;
						}
						bk = strdirsep(name);
					}
					while(bk);
				}
				wcx -= dot3;
				i32 extcx = GetTextSize(fi, ext, end);
				if(2 * extcx > wcx || ext == end) {
					i32 n = GetTextFitCount(fi, name, wcx);
					w->DrawText(x, y, name, font, ink, n);
					x += GetTextSize(fi, name, name + n);
					w->DrawText(x, y, "...", font, SColorDisabled, 3);
					x += dot3;
				}
				else {
					wcx -= extcx;
					i32 n = (i32)(GetTextFitLim(fi, name, end, wcx) - name);
					w->DrawText(x, y, name, font, ink, n);
					x += GetTextSize(fi, name, name + n);
					w->DrawText(x, y, "...", font, SColorDisabled, 3);
					w->DrawText(x + dot3, y, ext, font, extink, (i32)(end - ext));
					x += dot3 + extcx;
				}
			}
		}
	end:
		if(pass == 0)
			ww.DrawRect(x0, y + fi.GetAscent() + 1, x - x0, 1, uln);
	}
}

void FileList::Paint(Draw& w, const Rect& r, const Val& q,
		             Color ink, Color paper, dword style) const
{
	const File& m = ValTo<File>(q);
	bool inv = ((style & Display::SELECT) || (style & Display::CURSOR)) && (style & Display::FOCUS);
	w.DrawRect(r, inv ? SColorHighlight() : paper);
	i32 x = r.left + 2;
	w.DrawImage(x, r.top + (r.Height() - m.icon.GetSize().cy) / 2, m.icon);
	x += iconwidth;
	x += 2;
	FontInfo fi = m.font.Info();
	DrawFName(w, x, r.top + (r.Height() - fi.GetHeight()) / 2,
	             r.right - x - 2, r.Height(), WTxt(m.name), m.isdir, m.font,
	             inv ? SColorHighlightText : m.ink,
	             inv ? SColorHighlightText : m.extink,
	             WTxt(m.desc), m.descfont, justname, m.underline);
}

Size FileList::GetStdSize(const Val& q) const
{
	const File& m = ValTo<File>(q);
	FontInfo fi = m.font.Info();
	i32 cx = GetTextSize(fi, WTxt(m.name)) + 2 + iconwidth + 2 + 3;
	if(!IsNull(m.desc))
		cx += GetTextSize(m.descfont.Info(), WTxt(m.desc)) + fi.GetHeight();
	return Size(cx, GetItemHeight());
}

void FileList::StartEdit() {
	if(GetCursor() < 0) return;
	Rect r = GetItemRect(GetCursor());
	const File& cf = Get(GetCursor());
	Font f = cf.font;
	i32 fcy = f.Info().GetHeight();
	r.left += iconwidth + 2;
	r.top += (r.Height() - fcy - 4) / 2;
	r.bottom = r.top + fcy + 2;
	edit.SetRect(r);
	edit.SetFont(cf.font);
	edit <<= cf.name.ToWTxt();
	edit.Show();
	edit.SetFocus();
	i32 pos = i32(GetFileExtPos(cf.name) - ~cf.name);
	edit.SetSelection(0, pos);
	sb.Disable();
}

void FileList::EndEdit() {
	KillEdit();
	i32 b = edit.HasFocus();
	edit.Hide();
	if(b) SetFocus();
}

void FileList::OkEdit() {
	EndEdit();
	i32 c = GetCursor();
	if(c >= 0 && c < GetCount())
		WhenRename(Get(c).name, ~edit);
}

void FileList::KillEdit()
{
	sb.Enable();
	KillTimeCallback(TIMEID_STARTEDIT);
}

void FileList::LeftDown(Point p, dword flags) {
	i32 c = GetCursor();
	if(IsEdit()) {
		OkEdit();
		c = -1;
	}
	ColumnList::LeftDown(p, flags);
	KillEdit();
	if(c == GetCursor() && c >= 0 && !HasCapture() && renaming && WhenRename && !(flags & (K_SHIFT|K_CTRL)))
		SetTimeCallback(750, OTVET(StartEdit), TIMEID_STARTEDIT);
}

bool FileList::FindChar(i32 from, i32 chr) {
	for(i32 i = max(0, from); i < GetCount(); i++) {
		WTxt x = Get(i).name.ToWTxt();
		if((i32)ToUpper(ToAscii(x[0])) == chr) {
			ClearSelection();
			SetCursor(i);
			return true;
		}
	}
	return false;
}

bool FileList::Key(dword key, i32 count) {
	if(key == K_ESCAPE && IsEdit()) {
		EndEdit();
		return true;
	}
	if(key == K_ENTER && IsEdit()) {
		OkEdit();
		return true;
	}
	if(accelkey) {
		i32 c = ToUpper((i32)key);
		if(key < 256 && IsAlNum(c)) {
			if(!FindChar(GetCursor() + 1, c))
				FindChar(0, c);
			return true;
		}
	}
	return ColumnList::Key(key, count);
}

void FileList::Insert(i32 ii,
                      const Txt& name, const Image& icon, Font font, Color ink,
				      bool isdir, z64 length, Time time, Color extink,
				      const Txt& desc, Font descfont, Val data, Color uln,
				      bool unixexe, bool hidden)
{
	Val v;
	File& m = CreateRawVal<File>(v);
	m.isdir = isdir;
	m.unixexe = unixexe;
	m.hidden = hidden;
	m.icon = icon;
	m.name = name;
	m.font = font;
	m.ink = ink;
	m.length = length;
	m.time = time;
	m.extink = IsNull(extink) ? ink : extink;
	m.desc = desc;
	m.descfont = descfont;
	m.data = data;
	m.underline = uln;
	ColumnList::Insert(ii, v, !m.isdir || selectdir);
	KillEdit();
}

void FileList::Set(i32 ii,
                   const Txt& name, const Image& icon, Font font, Color ink,
				   bool isdir, z64 length, Time time, Color extink,
				   const Txt& desc, Font descfont, Val data, Color uln,
				   bool unixexe, bool hidden)
{
	Val v;
	File& m = CreateRawVal<File>(v);
	m.isdir = isdir;
	m.unixexe = unixexe;
	m.hidden = hidden;
	m.icon = icon;
	m.name = name;
	m.font = font;
	m.ink = ink;
	m.length = length;
	m.time = time;
	m.extink = IsNull(extink) ? ink : extink;
	m.desc = desc;
	m.descfont = descfont;
	m.data = data;
	m.underline = uln;
	ColumnList::Set(ii, v, !m.isdir || selectdir);
	KillEdit();
}

void FileList::Set(i32 ii, const File& f)
{
	Val v;
	File& m = CreateRawVal<File>(v);
	m = f;
	ColumnList::Set(ii, v, !m.isdir);
	KillEdit();
}

void FileList::SetIcon(i32 ii, const Image& icon)
{
	Val v;
	File& m = CreateRawVal<File>(v);
	m = Get(ii);
	m.icon = icon;
	ColumnList::Set(ii, v, !m.isdir);
	KillEdit();
}

void FileList::Add(const File& f)
{
	Val v;
	File& m = CreateRawVal<File>(v);
	m = f;
	ColumnList::Add(v, !m.isdir);
}

void FileList::Add(const Txt& name, const Image& icon, Font font, Color ink,
				   bool isdir, z64 length, Time time, Color extink,
				   const Txt& desc, Font descfont, Val data, Color uln,
				   bool unixexe, bool hidden)
{
	Val v;
	File& m = CreateRawVal<File>(v);
	m.isdir = isdir;
	m.unixexe = unixexe;
	m.hidden = hidden;
	m.icon = icon;
	m.name = name;
	m.font = font;
	m.ink = ink;
	m.length = length;
	m.time = time;
	m.extink = IsNull(extink) ? ink : extink;
	m.desc = desc;
	m.descfont = descfont;
	m.data = data;
	m.underline = uln;
	ColumnList::Add(v, !m.isdir || selectdir);
}

const FileList::File& FileList::Get(i32 i) const
{
	return ValTo<File>(ColumnList::Get(i));
}

Txt FileList::GetCurrentName() const
{
	return GetCount() && GetCursor() >= 0 && GetCursor() < GetCount() ? Get(GetCursor()).name
	       : Null;
}

i32 FileList::Find(tukk s) {
	for(i32 i = 0; i < GetCount(); i++)
		if(Get(i).name == s) return i;
	return -1;
}

bool FileList::FindSetCursor(tukk name) {
	i32 i = Find(name);
	if(i < 0) return false;
	SetCursor(i);
	return true;
}

struct FileList::FileOrder : public ValOrder {
	const FileList::Order *order;

	virtual bool operator()(const Val& a, const Val& b) const {
		return (*order)(ValTo<File>(a), ValTo<File>(b));
	}
};

void FileList::Sort(const Order& order)
{
	KillEdit();
	FileOrder fo;
	fo.order = &order;
	i32 i = GetCursor();
	Txt fn;
	if(i >= 0)
		fn = Get(i).name;
	ColumnList::Sort(fo);
	if(i >= 0)
		FindSetCursor(fn);
}

FileList::FileList() {
	iconwidth = DPI(16);
	ItemHeight(max(Draw::GetStdFontCy(), DPI(17)));
	Ctrl::Add(edit);
	edit.Hide();
	edit.SetFrame(BlackFrame());
	renaming = false;
	justname = false;
	accelkey = false;
	selectdir = false;
	SetDisplay(*this);
}

FileList::~FileList() {}

struct FileListSort : public FileList::Order {
	i32  kind;

	virtual bool operator()(const FileList::File& a, const FileList::File& b) const {
		if(a.isdir != b.isdir)
			return a.isdir;
		if(a.name == "..")
			return b.name != "..";
		if(b.name == "..")
			return false;
		i32 q = 0;
		i32 k = kind & ~FILELISTSORT_DESCENDING;
		if(k == FILELISTSORT_TIME)
			q = SgnCompare(a.time, b.time);
		else
		if(k == FILELISTSORT_SIZE)
			q = SgnCompare(a.length, b.length);
		else
		if(k == FILELISTSORT_EXT) {
			tukk ae = strrchr(a.name, '.');
			tukk be = strrchr(b.name, '.');
			if(ae == NULL || be == NULL)
				q = ae ? -1 : be ? 1 : 0;
			else
				q = stricmp(ae, be);
		}
		if(!q)
			q = stricmp(a.name, b.name);
		return kind & FILELISTSORT_DESCENDING ? q > 0 : q < 0;
	}
};

void SortBy(FileList& list, i32 kind)
{
	FileListSort fs;
	fs.kind = kind;
	list.Sort(fs);
}

}
