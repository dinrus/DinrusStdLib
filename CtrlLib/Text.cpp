#include <drx/CtrlLib/CtrlLib.h>

#define LTIMING(x)  // RTIMING(x)

namespace drx {

TextCtrl::TextCtrl()
{
	Unicode();
	undosteps = 1000;
	Clear();
	undoserial = 0;
	incundoserial = false;
	undo_op = false;
	WhenBar = OTVET(StdBar);
	charset = CHARSET_UTF8;
	color[INK_NORMAL] = SColorText;
	color[INK_DISABLED] = SColorDisabled;
	color[INK_SELECTED] = SColorHighlightText;
	color[PAPER_NORMAL] = SColorPaper;
	color[PAPER_READONLY] = SColorFace;
	color[PAPER_SELECTED] = SColorHighlight;
	color[WHITESPACE] = Blend(SColorLight, SColorHighlight);
	color[WARN_WHITESPACE] = Blend(SColorLight, SRed);

	processtab = true;
	processenter = true;
	nobg = false;
	rectsel = false;
#ifdef CPU_64
	max_total = 2047 * 1024 * 1024;
#else
#ifdef _DEBUG
	max_total = 100 * 1024 * 1024;
#else
	max_total = 200 * 1024 * 1024;
#endif
#endif
	max_line_len = 100000;
	truncated = false;
}

TextCtrl::~TextCtrl() {}

void TextCtrl::MiddleDown(Point p, dword flags)
{
	if(IsReadOnly())
		return;
	if(AcceptText(Selection())) {
		WTxt w = GetWTxt(Selection());
		selclick = false;
		LeftDown(p, flags);
		Paste(w);
		Action();
	}
}

void TextCtrl::CancelMode()
{
	selclick = false;
	dropcaret = Null;
	isdrag = false;
}

void TextCtrl::Clear()
{
	GuiLock __;
	view = NULL;
	viewlines = 0;
	cline = 0;
	cpos = 0;
	total = 0;
	truncated = false;
	lin.Clear();
	ClearLines();
	lin.Add();
	InsertLines(0, 1);
	DirtyFrom(0);
	undo.Clear();
	redo.Clear();
	ClearDirty();
	anchor = -1;
	cursor = 0;
	SetSb();
	PlaceCaret(0);
	SelectionChanged();
	Refresh();
}

void TextCtrl::DirtyFrom(i32 line) {}
void TextCtrl::SelectionChanged() {}
void TextCtrl::ClearLines() {}
void TextCtrl::InsertLines(i32 line, i32 count) {}
void TextCtrl::RemoveLines(i32 line, i32 count) {}
void TextCtrl::PreInsert(i32 pos, const WTxt& text) {}
void TextCtrl::PostInsert(i32 pos, const WTxt& text) {}
void TextCtrl::PreRemove(i32 pos, i32 size) {}
void TextCtrl::PostRemove(i32 pos, i32 size) {}
void TextCtrl::RefreshLine(i32 i) {}
void TextCtrl::InvalidateLine(i32 i) {}
void TextCtrl::SetSb() {}
void TextCtrl::PlaceCaret(z64 newcursor, bool sel) {}

i32 TextCtrl::RemoveRectSelection() { return 0; }
WTxt TextCtrl::CopyRectSelection() { return Null; }
i32 TextCtrl::PasteRectSelection(const WTxt& s) { return 0; }

void   TextCtrl::CachePos(z64 pos)
{
	GuiLock __;
	z64 p = pos;
	cline = GetLinePos64(p);
	cpos = pos - p;
}

void   TextCtrl::CacheLinePos(i32 linei)
{
	GuiLock __;
	if(linei >= 0 && linei < GetLineCount()) {
		cpos = GetPos64(linei);
		cline = linei;
	}
}

bool   TextCtrl::IsUnicodeCharset(u8 charset)
{
	return findarg(charset, CHARSET_UTF8, CHARSET_UTF8_BOM, CHARSET_UTF16_LE, CHARSET_UTF16_BE,
	                        CHARSET_UTF16_LE_BOM, CHARSET_UTF16_BE_BOM) >= 0;
}

i32   TextCtrl::Load0(Stream& in, u8 charset_, bool view) {
	GuiLock __;
	Clear();
	lin.Clear();
	ClearLines();
	total = 0;
	SetCharset(charset_);
	truncated = false;
	viewlines = 0;
	this->view = NULL;
	view_all = false;
	offset256.Clear();
	total256.Clear();
	view_cache[0].blk = view_cache[1].blk = -1;
	if(view) {
		this->view = &in;
		SetReadOnly();
	}
	if(charset == CHARSET_UTF8_BOM && in.GetLeft() >= 3) {
		z64 pos = in.GetPos();
		u8 h[3];
		if(!(in.Get(h, 3) == 3 && h[0] == 0xEF && h[1] == 0xBB && h[2] == 0xBF))
			in.Seek(pos);
		charset = CHARSET_UTF8;
	}
	i32 be16 = findarg(charset, CHARSET_UTF16_LE_BOM, CHARSET_UTF16_BE_BOM);
	if(be16 >= 0 && in.GetLeft() >= 2) {
		z64 pos = in.GetPos();
		dword h = in.Get16le();
		if(h != (be16 ? 0xfffe : 0xfeff))
			in.Seek(pos);
		charset = be16 ? CHARSET_UTF16_BE : CHARSET_UTF16_LE;
	}

	if(view) {
		view_loading_pos = in.GetPos();
		view_loading_lock = 0;
		ViewLoading();
		PlaceCaret(0);
		return 0;
	}

	i32 m = LoadLines(lin, INT_MAX, total, in, charset, max_line_len, max_total, truncated);

	InsertLines(0, lin.GetCount());
	Update();
	SetSb();
	PlaceCaret(0);
	return m;
}

i32 TextCtrl::LoadLines(Vec<Ln>& ls, i32 n, z64& total, Stream& in, u8 charset,
                        i32 max_line_len, i32 max_total, bool& truncated,
                        i32 *view_line_count) const
{
	TxtBuf ln;
	bool cr = false;
	u8 b8 = 0;
	auto line_count = [&] { return view_line_count ? *view_line_count : ls.GetCount(); };
	if(charset == CHARSET_UTF16_LE || charset == CHARSET_UTF16_BE) {
		WTxtBuf wln;
		auto put_wln = [&]() {
			if(view_line_count)
				(*view_line_count)++;
			else {
				Ln& ln = ls.Add();
				ln.len = wln.GetCount();
				ln.text = ToUtf8(~wln, ln.len);
			}
		};
		for(;;) {
			i32 c = charset == CHARSET_UTF16_LE ? in.Get16le() : in.Get16be();
			if(c < 0) {
				total += wln.GetCount();
				put_wln();
				goto finish;
			}
			if(c == '\r')
				cr = true;
			else
			if(c == '\n') {
			truncate_line:
				total += wln.GetCount() + 1;
				put_wln();
				if(line_count() >= n)
					goto finish;
				wln.Clear();
			}
			else {
				wln.Cat(c);
				if(wln.GetCount() >= max_line_len)
					goto truncate_line;
			}
		}
	}
	else {
		for(;;) {
			u8 h[200];
			i32 size;
			z64 pos = in.GetPos();
			u8k *s = in.GetSzPtr(size);
			if(size == 0)  {
				size = in.Get(h, 200);
				s = h;
				if(size == 0)
					break;
			}
			u8k *posptr = s;
			u8k *e = s + size;
			while(s < e) {
				u8k *b = s;
				u8k *ee = s + min(size_t(e - s), size_t(max_line_len - ln.GetCount()));
				{
					while(s < ee && *s != '\r' && *s != '\n') {
						b8 |= *s++;
						while(s < ee && *s >= ' ' && *s < 128) // Interestingly, this speeds things up
							s++;
						while(s < ee && *s >= ' ')
							b8 |= *s++;
					}
				}
				if(b < s) {
					if(s - b + ln.GetCount() > max_total)
						ln.Cat((tukk )b, max_total - ln.GetCount());
					else
						ln.Cat((tukk )b, (tukk )s);
				}
				auto put_ln = [&]() -> bool {
					if(view_line_count) {
						(*view_line_count)++;
						total += (charset == CHARSET_UTF8 && (b8 & 0x80) ? Utf32Len(~ln, ln.GetCount())
						                                                 : ln.GetCount()) + 1;
					}
					else {
						Ln& l = ls.Add();
						if(charset == CHARSET_UTF8) {
							l.len = (b8 & 0x80) ? Utf32Len(~ln, ln.GetCount()) : ln.GetCount();
							l.text = ln;
						}
						else {
							l.len = ln.GetCount();
							l.text = ToCharset(CHARSET_UTF8, ln, charset);
						}
						if(total + l.len + 1 > max_total) {
							ls.Drop();
							truncated = true;
							return false;
						}
						total += l.len + 1;
					}
					return true;
				};
				while(ln.GetCount() >= max_line_len) {
					i32 ei = max_line_len;
					if(charset == CHARSET_UTF8)
						while(ei > 0 && ei > max_line_len - 6 && !((u8)ln[ei] < 128 || IsUtf8Lead((u8)ln[ei]))) // break lse at whole utf8 codepoint if possible
							ei--;
					Txt nln(~ln + ei, ln.GetCount() - ei);
					ln.SetCount(ei);
					truncated = true;
					if(!put_ln())
						goto out_of_limit;
					if(line_count() >= n) {
						in.Seek(s - posptr + pos);
						goto finish;
					}
					ln = nln;
				}
				if(s < e && *s == '\r') {
					s++;
					cr = true;
				}
				if(s < e && *s == '\n') {
					if(!put_ln())
						goto out_of_limit;
					s++;
					if(line_count() >= n) {
						in.Seek(s - posptr + pos);
						goto finish;
					}
					ln.Clear();
					b8 = 0;
				}
			}
		}
	}

out_of_limit:
	{
		WTxt w = ToUnicode(~ln, ln.GetCount(), charset);
		if(total + w.GetLength() <= max_total) {
			if(view_line_count) {
				(*view_line_count)++;
				total += w.GetCount();
			}
			else {
				Ln& ln = ls.Add();
				ln.len = w.GetCount();
				ln.text = ToUtf8(~w, ln.len);
				total += ln.len;
			}
		}
	}
finish:
	return ls.GetCount() > 1 ? cr ? LE_CRLF : LE_LF : LE_DEFAULT;
}

void TextCtrl::ViewLoading()
{
	GuiLock __;
	if(view_all || !view)
		return;
	i32 start = msecs();
	view->Seek(view_loading_pos);
	i32 lines0 = viewlines;
	for(;;) {
		offset256.Add(view->GetPos());
		Vec<Ln> l;
		bool b;
		z64 t = 0;

		i32 line_count = 0;
		LoadLines(l, 256, t, *view, charset, 10000, INT_MAX, b, &line_count);
		viewlines += line_count;
		total += t;
		total256.Add((i32)t);

	#ifdef CPU_32
		enum { MAX_LINES = 128000000 };
	#else
		enum { MAX_LINES = INT_MAX - 512 };
	#endif

		if(view->IsEof() || viewlines > MAX_LINES) {
			WhenViewMapping(view->GetPos());
			view_all = true;
			break;
		}

		if(view_loading_lock) {
			view_loading_pos = view->GetPos();
			WhenViewMapping(view_loading_pos);
			break;
		}

		if(msecs(start) > 20) {
			view_loading_pos = view->GetPos();
			PostCallback([=, this] { ViewLoading(); });
			WhenViewMapping(view_loading_pos);
			break;
		}
	}
	InsertLines(lines0, viewlines - lines0);
	SetSb();
	Update();
}

void TextCtrl::UnlockViewMapping()
{
	view_loading_lock--;
	ViewLoading();
}

void TextCtrl::WaitView(i32 line, bool progress)
{
	if(view) {
		if(progress) {
			LockViewMapping();
			Progress pi("Сканируется указанный файл");
			pi.Delay(1000);
			while(view && !view_all && viewlines < line) {
				if(pi.SetCanceled(i32(view_loading_pos >> 10), i32(view->GetSize()) >> 10))
					break;
				ViewLoading();
			}
			UnlockViewMapping();
		}
		else
			while(view && !view_all && viewlines <= line)
				ViewLoading();
	}
}

void TextCtrl::SerializeViewMap(Stream& s)
{
	GuiLock __;
	i32 version = 0;
	s / version;
	s.Magic(327845692);
	s % view_loading_pos
	  % total
	  % viewlines
	  % view_all
	  % total256
	  % offset256
	;
	if(s.IsLoading()) {
		SetSb();
		Update();
		Refresh();
	}
}

const TextCtrl::Ln& TextCtrl::GetLn(i32 i) const
{
	if(view) {
		GuiLock __;
		i32 blk = i >> 8;
		if(view_cache[0].blk != blk)
			Swap(view_cache[0], view_cache[1]); // trivial LRU
		if(view_cache[0].blk != blk) {
			Swap(view_cache[0], view_cache[1]); // trivial LRU
			view->Seek(offset256[blk]);
			z64 t = 0;
			bool b;
			view_cache[0].line.Clear();
			view_cache[0].blk = blk;
			LoadLines(view_cache[0].line, 256, t, *view, charset, 10000, INT_MAX, b);
		}
		return view_cache[0].line[i & 255];
	}
	else
		return lin[i];
}

const Txt& TextCtrl::GetUtf8Line(i32 i) const
{
	return GetLn(i).text;
}

i32 TextCtrl::GetLineLength(i32 i) const
{
	return GetLn(i).len;
}

void   TextCtrl::Save(Stream& s, u8 charset, i32 line_endings) const {
	if(charset == CHARSET_UTF8_BOM) {
		static u8 bom[] = { 0xEF, 0xBB, 0xBF };
		s.Put(bom, 3);
		charset = CHARSET_UTF8;
	}
	if(charset == CHARSET_UTF16_LE_BOM) {
		s.Put16le(0xfeff);
		charset = CHARSET_UTF16_LE;
	}
	if(charset == CHARSET_UTF16_BE_BOM) {
		s.Put16be(0xfeff);
		charset = CHARSET_UTF16_BE;
	}
	charset = ResolveCharset(charset);
	Txt le = "\n";
#ifdef TARGET_WIN32
	if(line_endings == LE_DEFAULT)
		le = "\r\n";
#endif
	if(line_endings == LE_CRLF)
		le = "\r\n";
	i32 be16 = findarg(charset, CHARSET_UTF16_LE, CHARSET_UTF16_BE);
	if(be16 >= 0) {
		Txt wle;
		for(i32 i = 0; i < le.GetCount(); i++) {
			if(be16)
				wle.Cat(0);
			wle.Cat(le[i]);
			if(!be16)
				wle.Cat(0);
		}
		for(i32 i = 0; i < GetLineCount(); i++) {
			if(i)
				s.Put(wle);
			WTxt txt = GetWLine(i);
			wtukk e = txt.End();
			if(be16)
				for(wtukk w = txt; w != e; w++)
					if(*w < 0x10000)
						s.Put16be((word)*w);
					else {
						char16 h[2];
						ToUtf16(h, w, 1);
						s.Put16be(h[0]);
						s.Put16be(h[1]);
					}
			else
				for(wtukk w = txt; w != e; w++)
					if(*w < 0x10000)
						s.Put16le((word)*w);
					else {
						char16 h[2];
						ToUtf16(h, w, 1);
						s.Put16le(h[0]);
						s.Put16le(h[1]);
					}
		}
		return;
	}
	for(i32 i = 0; i < GetLineCount(); i++) {
		if(i)
			s.Put(le);
		if(charset == CHARSET_UTF8)
			s.Put(GetUtf8Line(i));
		else {
			Txt txt = FromUnicode(GetWLine(i), charset);
			tukk e = txt.End();
			for(tukk w = txt; w != e; w++)
				s.Put(*w == DEFAULTCHAR ? '?' : *w);
		}
	}
}

void   TextCtrl::Set(const Txt& s, u8 charset) {
	TxtStream ss(s);
	Load(ss, charset);
}

Txt TextCtrl::Get(u8 charset) const
{
	TxtStream ss;
	Save(ss, charset);
	return ss;
}

i32 TextCtrl::GetInvalidCharPos(u8 charset) const
{
	i32 q = 0;
	if(!IsUnicodeCharset(charset))
		for(i32 i = 0; i < GetLineCount(); i++) {
			WTxt txt = GetWLine(i);
			WTxt ctxt = ToUnicode(FromUnicode(txt, charset), charset);
			for(i32 w = 0; w < txt.GetLength(); w++)
				if(txt[w] != ctxt[w])
					return q + w;
			q += txt.GetLength() + 1;
		}
	return -1;
}

void   TextCtrl::ClearDirty()
{
	dirty = 0;
	ClearModify();
	WhenState();
}

TextCtrl::UndoData TextCtrl::PickUndoData()
{
	UndoData data;
	data.undo = pick(undo);
	data.redo = pick(redo);
	data.undoserial = undoserial;
	return data;
}

void TextCtrl::SetPickUndoData(TextCtrl::UndoData&& data)
{
	undo = pick(data.undo);
	redo = pick(data.redo);
	undoserial = data.undoserial;
	incundoserial = true;
}

void TextCtrl::Set(const WTxt& s)
{
	Clear();
	Insert0(0, s);
}

void  TextCtrl::SetData(const Val& v)
{
	Set((WTxt)v);
}

Val TextCtrl::GetData() const
{
	return GetW();
}

Txt TextCtrl::GetEncodedLine(i32 i, u8 charset) const
{
	charset = ResolveCharset(charset);
	Txt h = GetUtf8Line(i);
	return charset == CHARSET_UTF8 ? h : FromUnicode(ToUtf32(h), charset);
}

i32   TextCtrl::GetLinePos64(z64& pos) const {
	GuiLock __;
	if(pos < cpos && cpos - pos < pos && !view) {
		i32 i = cline;
		z64 ps = cpos;
		for(;;) {
			ps -= GetLineLength(--i) + 1;
			if(ps <= pos) {
				pos = pos - ps;
				return i;
			}
		}
	}
	else {
		i32 i = 0;
		if(view) {
			GuiLock __;
			i32 blk = 0;
			for(;;) {
				i32 n = total256[blk];
				if(pos < n)
					break;
				pos -= n;
				if(++blk >= total256.GetCount()) {
					pos = GetLineLength(GetLineCount() - 1);
					return GetLineCount() - 1;
				}
			}
			i = blk << 8;
		}
		else
		if(pos >= cpos) {
			pos -= cpos;
			i = cline;
		}
		for(;;) {
			i32 n = GetLineLength(i) + 1;
			if(pos < n) return i;
			pos -= n;
			i++;
			if(i >= GetLineCount()) {
				pos = GetLineLength(GetLineCount() - 1);
				return GetLineCount() - 1;
			}
		}
	}
	return 0; // just silencing GCC warning, cannot get here
}

z64  TextCtrl::GetPos64(i32 ln, i32 lpos) const {
	GuiLock __;
	ln = minmax(ln, 0, GetLineCount() - 1);
	i32 i;
	z64 pos;
	if(ln < cline && cline - ln < ln && !view) {
		pos = cpos;
		i = cline;
		while(i > ln)
			pos -= GetLineLength(--i) + 1;
	}
	else {
		pos = 0;
		i = 0;
		if(view) {
			for(i32 j = 0; j < ln >> 8; j++) {
				pos += total256[j];
				i += 256;
			}
		}
		else
		if(ln >= cline) {
			pos = cpos;
			i = cline;
		}
		while(i < ln)
			pos += GetLineLength(i++) + 1;
	}
	return pos + min(GetLineLength(ln), lpos);
}

WTxt TextCtrl::GetW(z64 pos, i32 size) const
{
	i32 i = GetLinePos64(pos);
	WTxtBuf r;
	for(;;) {
		if(i >= GetLineCount()) break;
		WTxt ln = GetWLine(i++);
		i32 sz = min(LimitSize(ln.GetLength() - pos), size);
		if(pos == 0 && sz == ln.GetLength())
			r.Cat(ln);
		else
			r.Cat(ln.Mid((i32)pos, sz));
		size -= sz;
		if(size == 0) break;
#ifdef TARGET_WIN32
		r.Cat('\r');
#endif
		r.Cat('\n');
		size--;
		if(size == 0) break;
		pos = 0;
	}
	return WTxt(r);
}

Txt TextCtrl::Get(z64 pos, i32 size, u8 charset) const
{
	if(charset == CHARSET_UTF8) {
		i32 i = GetLinePos64(pos);
		TxtBuf r;
		for(;;) {
			if(i >= GetLineCount()) break;
			i32 sz = min(LimitSize(GetLineLength(i) - pos), size);
			const Txt& h = GetUtf8Line(i);
			tukk s = h;
			i32 n = h.GetCount();
			i++;
			if(pos == 0 && sz == n)
				r.Cat(s, n);
			else
				r.Cat(ToUtf32(s, n).Mid((i32)pos, sz).ToTxt());
			size -= sz;
			if(size == 0) break;
	#ifdef TARGET_WIN32
			r.Cat('\r');
	#endif
			r.Cat('\n');
			size--;
			if(size == 0) break;
			pos = 0;
		}
		return Txt(r);
	}
	return FromUnicode(GetW(pos, size), charset);
}

i32  TextCtrl::GetChar(z64 pos) const {
	if(pos < 0 || pos >= GetLength64())
		return 0;
	i32 i = GetLinePos64(pos);
	WTxt ln = GetWLine(i);
	i32 c = ln.GetLength() == pos ? '\n' : ln[(i32)pos];
	return c;
}

i32 TextCtrl::GetLinePos32(i32& pos) const
{
	z64 p = pos;
	i32 l = GetLinePos64(p);
	pos = (i32)p;
	return l;
}

bool TextCtrl::GetSelection32(i32& l, i32& h) const
{
	z64 ll, hh;
	bool b = GetSelection(ll, hh);
	if(hh >= INT_MAX) {
		l = h = (i32)cursor;
		return false;
	}
	l = (i32)ll;
	h = (i32)hh;
	return b;
}

i32 TextCtrl::GetCursor32() const
{
	z64 h = GetCursor64();
	return h < INT_MAX ? (i32)h : 0;
}

i32 TextCtrl::GetLength32() const
{
	z64 h = GetLength64();
	return h < INT_MAX ? (i32)h : 0;
}

i32 TextCtrl::Insert0(i32 pos, const WTxt& txt) { // TODO: Do this with utf8
	GuiLock __;
	i32 inspos = pos;
	PreInsert(inspos, txt);
	if(pos < cpos)
		cpos = cline = 0;
	i32 i = GetLinePos32(pos);
	DirtyFrom(i);
	i32 size = 0;

	WTxtBuf lnb;
	Vec<WTxt> iln;
	wtukk s = txt;
	while(s < txt.End())
		if(*s >= ' ') {
			wtukk b = s;
			while(*s >= ' ') // txt is zero teminated...
				s++;
			i32 sz = i32(s - b);
			lnb.Cat(b, sz);
			size += sz;
		}
		else
		if(*s == '\t') {
			lnb.Cat(*s);
			size++;
			s++;
		}
		else
		if(*s == '\n') {
			iln.Add(lnb);
			size++;
			lnb.Clear();
			s++;
		}
		else
			s++;
	WTxt ln = lnb;
	WTxt l = GetWLine(i);
	if(iln.GetCount()) {
		iln[0] = l.Mid(0, pos) + iln[0];
		ln.Cat(l.Mid(pos));
		SetLine(i, ln);
		InvalidateLine(i);
		LineInsert(i, iln.GetCount());
		for(i32 j = 0; j < iln.GetCount(); j++)
			SetLine(i + j, iln[j]);
		InsertLines(i, iln.GetCount());
		Refresh();
	}
	else {
		SetLine(i, l.Mid(0, pos) + ln + l.Mid(pos));
		InvalidateLine(i);
		RefreshLine(i);
	}
	total += size;
	SetSb();
	Update();
	ClearSelection();
	PostInsert(inspos, txt);
	return size;
}

void TextCtrl::Remove0(i32 pos, i32 size) {
	GuiLock __;
	i32 rmpos = pos, rmsize = size;
	PreRemove(rmpos, rmsize);
	total -= size;
	if(pos < cpos)
		cpos = cline = 0;
	i32 i = GetLinePos32(pos);
	DirtyFrom(i);
	WTxt ln = GetWLine(i);
	i32 sz = min(LimitSize(ln.GetLength() - pos), size);
	ln.Remove(pos, sz);
	size -= sz;
	SetLine(i, ln);
	if(size == 0) {
		InvalidateLine(i);
		RefreshLine(i);
	}
	else {
		size--;
		i32 j = i + 1;
		for(;;) {
			i32 sz = GetLineLength(j) + 1;
			if(sz > size) break;
			j++;
			size -= sz;
		}
		WTxt p1 = GetWLine(i);
		WTxt p2 = GetWLine(j);
		p1.Insert(p1.GetLength(), p2.Mid(size, p2.GetLength() - size));
		SetLine(i, p1);
		LineRemove(i + 1, j - i);
		RemoveLines(i + 1, j - i);
		InvalidateLine(i);
		Refresh();
	}
	Update();
	ClearSelection();
	PostRemove(rmpos, rmsize);
	SetSb();
}

void TextCtrl::Undodo()
{
	while(undo.GetCount() > undosteps)
		undo.DropHead();
	redo.Clear();
}

void TextCtrl::NextUndo()
{
	undoserial += incundoserial;
	incundoserial = false;
}

void TextCtrl::IncDirty() {
	dirty++;
	if(dirty == 0 || dirty == 1)
	{
		if(dirty)
			SetModify();
		else
			ClearModify();
		WhenState();
	}
}

void TextCtrl::DecDirty() {
	dirty--;
	if(dirty == 0 || dirty == -1)
	{
		if(dirty)
			SetModify();
		else
			ClearModify();
		WhenState();
	}
}

i32 TextCtrl::InsertU(i32 pos, const WTxt& txt, bool typing) {
	i32 sz = Insert0(pos, txt);
	if(undosteps) {
		if(undo.GetCount() > 1 && typing && *txt != '\n' && IsDirty()) {
			UndoRec& u = undo.Tail();
			if(u.typing && u.pos + u.size == pos) {
				u.size += txt.GetLength();
				return sz;
			}
		}
		UndoRec& u = undo.AddTail();
		incundoserial = true;
		IncDirty();
		u.serial = undoserial;
		u.pos = pos;
		u.size = sz;
		u.typing = typing;
	}
	return sz;
}

void TextCtrl::RemoveU(i32 pos, i32 size) {
	if(size + pos > total)
		size = i32(total - pos);
	if(size <= 0) return;
	if(undosteps) {
		UndoRec& u = undo.AddTail();
		incundoserial = true;
		IncDirty();
		u.serial = undoserial;
		u.pos = pos;
		u.size = 0;
		u.SetText(Get(pos, size, CHARSET_UTF8));
		u.typing = false;
	}
	Remove0(pos, size);
}

i32 TextCtrl::Insert(i32 pos, const WTxt& _txt, bool typing) {
	if(pos + _txt.GetCount() > max_total)
		return 0;
	WTxt txt = _txt;
	if(!IsUnicodeCharset(charset))
		for(i32 i = 0; i < txt.GetCount(); i++)
			if(FromUnicode(txt[i], charset) == DEFAULTCHAR)
				txt.Set(i, '?');
	i32 sz = InsertU(pos, txt, typing);
	Undodo();
	return sz;
}

i32 TextCtrl::Insert(i32 pos, const Txt& txt, u8 charset)
{
	return Insert(pos, ToUnicode(txt, charset), false);
}

void TextCtrl::Remove(i32 pos, i32 size) {
	RemoveU(pos, size);
	Undodo();
}

void TextCtrl::Undo() {
	if(undo.IsEmpty()) return;
	undo_op = true;
	i32 nc = 0;
	i32 s = undo.Tail().serial;
	while(undo.GetCount()) {
		const UndoRec& u = undo.Tail();
		if(u.serial != s)
			break;
		UndoRec& r = redo.AddTail();
		r.serial = s;
		r.typing = false;
		nc = r.pos = u.pos;
		CachePos(r.pos);
		if(u.size) {
			r.size = 0;
			r.SetText(Get(u.pos, u.size, CHARSET_UTF8));
			Remove0(u.pos, u.size);
		}
		else {
			WTxt text = ToUtf32(u.GetText());
			r.size = Insert0(u.pos, text);
			nc += r.size;
		}
		undo.DropTail();
		DecDirty();
	}
	ClearSelection();
	PlaceCaret(nc, false);
	Action();
	undo_op = false;
}

void TextCtrl::Redo() {
	if(!redo.GetCount()) return;
	NextUndo();
	i32 s = redo.Tail().serial;
	i32 nc = 0;
	while(redo.GetCount()) {
		const UndoRec& r = redo.Tail();
		if(r.serial != s)
			break;
		nc = r.pos + r.size;
		CachePos(r.pos);
		if(r.size)
			RemoveU(r.pos, r.size);
		else
			nc += InsertU(r.pos, ToUtf32(r.GetText()));
		redo.DropTail();
		IncDirty();
	}
	ClearSelection();
	PlaceCaret(nc, false);
	Action();
}

void  TextCtrl::ClearSelection() {
	if(anchor >= 0) {
		anchor = -1;
		Refresh();
		SelectionChanged();
		WhenSel();
	}
}

void   TextCtrl::SetSelection(z64 l, z64 h) {
	if(l != h) {
		PlaceCaret(minmax(l, (z64)0, total), false);
		PlaceCaret(minmax(h, (z64)0, total), true);
	}
	else
		SetCursor(l);
}

bool   TextCtrl::GetSelection(z64& l, z64& h) const {
	if(anchor < 0 || anchor == cursor) {
		l = h = cursor;
		return false;
	}
	else {
		l = min(anchor, cursor);
		h = max(anchor, cursor);
		return !rectsel;
	}
}

Txt TextCtrl::GetSelection(u8 charset) const {
	z64 l, h;
	if(GetSelection(l, h))
		return Get(l, LimitSize(h - l), charset);
	return Txt();
}

WTxt TextCtrl::GetSelectionW() const {
	z64 l, h;
	if(GetSelection(l, h))
		return GetW(l, LimitSize(h - l));
	return WTxt();
}

bool   TextCtrl::RemoveSelection() {
	z64 l, h;
	if(anchor < 0) return false;
	if(IsRectSelection())
		l = RemoveRectSelection();
	else {
		if(!GetSelection(l, h))
			return false;
		Remove((i32)l, i32(h - l));
	}
	anchor = -1;
	Refresh();
	PlaceCaret(l);
	Action();
	return true;
}

void TextCtrl::RefreshLines(i32 l1, i32 l2) {
	i32 h = max(l1, l2);
	for(i32 i = min(l1, l2); i <= h; i++)
		RefreshLine(i);
}

void TextCtrl::Cut() {
	if(!IsReadOnly() && IsAnySelection()) {
		Copy();
		RemoveSelection();
	}
}

void TextCtrl::Copy() {
	z64 l, h;
	if(!GetSelection(l, h) && !IsAnySelection()) {
		i32 i = GetLine(cursor);
		l = GetPos64(i);
		h = l + GetLineLength(i) + 1;
	}
	WTxt txt;
	if(IsRectSelection())
		txt = CopyRectSelection();
	else
		txt = GetW(l, LimitSize(h - l));
	ClearClipboard();
	AppendClipboardUnicodeText(txt);
	AppendClipboardText(txt.ToTxt());
}

void TextCtrl::SelectAll() {
	SetSelection();
}

i32  TextCtrl::Paste(const WTxt& text) {
	if(IsReadOnly()) return 0;
	i32 n;
	if(IsRectSelection())
		n = PasteRectSelection(text);
	else {
		RemoveSelection();
		n = Insert((i32)cursor, text);
		PlaceCaret(cursor + n);
	}
	Refresh();
	return n;
}

Txt TextCtrl::GetPasteText()
{
	return Null;
}

void TextCtrl::Paste() {
	WTxt w = ReadClipboardUnicodeText();
	if(w.IsEmpty())
		w = ReadClipboardText().ToWTxt();
	if(w.IsEmpty())
		w = GetPasteText().ToWTxt();
	Paste(w);
	Action();
}

void TextCtrl::StdBar(Bar& menu) {
	NextUndo();
	if(undosteps) {
		menu.Add(undo.GetCount() && IsEditable(), t_("Отменить"), CtrlImg::undo(), OTVET(Undo))
			.Key(K_ALT_BACKSPACE)
			.Key(K_CTRL_Z);
		menu.Add(redo.GetCount() && IsEditable(), t_("Восстановить"), CtrlImg::redo(), OTVET(Redo))
			.Key(K_SHIFT|K_ALT_BACKSPACE)
			.Key(K_SHIFT_CTRL_Z);
		menu.Separator();
	}
	menu.Add(IsEditable() && IsAnySelection(),
			t_("Вырезать"), CtrlImg::cut(), OTVET(Cut))
		.Key(K_SHIFT_DELETE)
		.Key(K_CTRL_X);
	menu.Add(IsAnySelection(),
			t_("Копировать"), CtrlImg::copy(), OTVET(Copy))
		.Key(K_CTRL_INSERT)
		.Key(K_CTRL_C);
	bool canpaste = IsEditable() && IsClipboardAvailableText();
	menu.Add(canpaste,
			t_("Вставить"), CtrlImg::paste(), OTVET(DoPaste))
		.Key(K_SHIFT_INSERT)
		.Key(K_CTRL_V);
	LineEdit *e = dynamic_cast<LineEdit *>(this);
	if(e) {
		menu.Add(canpaste,
				 t_("Вставить в столбец"), CtrlImg::paste_vert(), callback(e, &LineEdit::DoPasteColumn))
			.Key(K_ALT_V|K_SHIFT);
		menu.Add(e->IsRectSelection(),
				 t_("Сортировать"), CtrlImg::sort(), callback(e, &LineEdit::Sort));
	}
	menu.Add(IsEditable() && IsAnySelection(),
			t_("Стереть"), CtrlImg::remove(), OTVET(DoRemoveSelection))
		.Key(K_DELETE);
	menu.Separator();
	menu.Add(GetLength64(),
			t_("Выделить Все"), CtrlImg::select_all(), OTVET(SelectAll))
		.Key(K_CTRL_A);
}

Txt TextCtrl::GetSelectionData(const Txt& fmt) const
{
	return GetTextClip(GetSelectionW(), fmt);
}

void TextCtrl::EditPos::Serialize(Stream& s) {
	i32 version = 1;
	s / version;
	if(version >= 1)
		s % sby % cursor;
	else {
		i32 c = (i32)cursor;
		s % sby % c;
		cursor = c;
	}
}

}