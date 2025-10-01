#include "HexView.h"

namespace drx {

inline i32 FormatHexDigit(i32 c) {
	return c < 10 ? c + '0' : c - 10 + 'a';
}

void FormatHex(tuk buffer, z64 number, i32 n) {
	buffer[n] = '\0';
	while(n) {
		buffer[--n] = FormatHexDigit((u8)number & 0x0f);
		number >>= 4;
	}
}

void HexViewInfo::PrintVal(Draw& w, i32 x, i32 y, i32 bytes, bool be)
{
	dword d = 0;
	Size fsz = GetTextSize("X", font);
	for(i32 i = 0; i < bytes; i++) {
		i32 b = data[be ? i : bytes - i - 1];
		if(b < 0) {
			w.DrawText(x, y, Txt('?', 2 * bytes), font, SColorHighlight);
			x += 2 * bytes * fsz.cx;
			w.DrawText(x, y, "=", font);
			x += fsz.cx;
			w.DrawText(x, y, "?", font, Red);
			return;
		}
		d = (d << 8) | (u8) b;
	}
	w.DrawText(x, y, FormatIntHex(d, 2 * bytes), font, SColorHighlight);
	x += 2 * bytes * fsz.cx;
	w.DrawText(x, y, "=", font);
	x += fsz.cx;
	Txt txt = FormatUnsigned(d);
	w.DrawText(x, y, txt, font, Red);
	x += GetTextSize(txt, font).cx;
	w.DrawText(x, y, "=", font);
	x += fsz.cx;
	i32 q = d;
	if(bytes == 1)
		q = (i8) d;
	else
	if(bytes == 2)
		q = (i16) d;
	w.DrawText(x, y, FormatInt(q), font, Magenta);
}

void HexViewInfo::Paint(Draw& w)
{
	Size sz = GetSize();
	w.DrawRect(sz, SColorLtFace);
	if(mode < 1 || empty)
		return;
	Size fsz = GetTextSize("X", font);
	char h[17];
	FormatHex(h, pos, longmode ? 16 : 8);
	i32 xx = 0;
	w.DrawText(xx, 0, h, font, SColorHighlight);
	xx += (longmode ? 16 : 8) * fsz.cx;
	w.DrawText(xx, 0, "=", font);
	xx += fsz.cx;
	w.DrawText(xx, 0, Format64(pos), font, Red);
	xx += (longmode ? 22 : 12) * fsz.cx;
	i32 y = 0;
	i32 x = 0;
	for(i32 q = 0; q < mode; q++) {
		x = xx;
		if(q < 1)
			PrintVal(w, x, y, 1, q);
		x += 12 * fsz.cx;
		PrintVal(w, x, y, 2, q);
		x += 18 * fsz.cx;
		PrintVal(w, x, y, 4, q);
		x += 32 * fsz.cx;
		y += fsz.cy;
	}
	char sh[80];
	memset(sh, 0, sizeof(sh));
	i32 i;
	for(i = 0; i < 80; i++) {
		if(data[i] < 0)
			break;
		sh[i] = data[i];
	}
	WTxt ws = ToUtf32(sh, i);
	w.DrawText(x, 0, ws, font, Cyan, i);
	if(mode < 2)
		return;
	wchar wh[40];
	memset(wh, 0, sizeof(wh));
	for(i = 0; i < 40; i++) {
		if(data[2 * i] < 0 || data[2 * i + 1] < 0)
			break;
		wh[i] = MAKEWORD(data[2 * i], data[2 * i + 1]);
	}
	w.DrawText(x, fsz.cy, wh, font, Cyan, i);
	Txt txt;
	Txt ftxt;
	i = 0;
	for(;;) {
		if(data[i] < 0) {
			if((unsigned)i < sizeof(float))
				ftxt = "?";
			txt = "?";
			break;
		}
		if((unsigned)i >= sizeof(double)) {
			double h;
			memcpy(&h, sh, sizeof(double));
			txt = Sprintf("%.8g", h);
			break;
		}
		if(i == sizeof(float)) {
			float h;
			memcpy(&h, sh, sizeof(float));
			ftxt = Sprintf("%.6g", h);
		}
		sh[i] = data[i];
		i++;
	}
	w.DrawText(0, fsz.cy, txt, font, Red);
	w.DrawText(18 * fsz.cx, fsz.cy, ftxt, font, Red);
}

void HexViewInfo::SetMode(i32 _mode)
{
	mode = _mode;
	Height(mode * GetTextSize("X", CourierZ(12)).cy + 3);
	Show(mode);
}

HexViewInfo::HexViewInfo()
{
	SetMode(0);
	AddFrame(TopSeparatorFrame());
	AddFrame(RightSeparatorFrame());
	font = CourierZ(12);
}

i32 HexView::Byte(z64 adr)
{
	return 0;
}

void HexView::Paint(Draw& w)
{
	Size sz = GetSize();
	w.DrawRect(sz, SColorPaper);
	if(!total) {
		w.DrawText(Zx(10), Zx(10), "Нет данных", ArialZ(20).Italic(), SRed());
		return;
	}
	i32 y = 0;
	zu64 adr = sc;
	while(y < sz.cy) {
		char h[17];
		FormatHex(h, adr + start, IsLongMode() ? 16 : 8);
		w.DrawText(0, y, h, font);
		i32 x = (IsLongMode() ? 17 : 9) * fsz.cx;
		i32 tx = x + columns * fcx3;
		for(i32 q = columns; q--;) {
			if(adr >= total)
				return;
			if(adr == cursor) {
				w.DrawRect(x, y, fsz.cx * 2, fsz.cy, LtCyan);
				w.DrawRect(tx, y, fsz.cx, fsz.cy, LtCyan);
			}
			i32 b = Byte(adr++);
			if(b < 0) {
				w.DrawText(x, y, "??", font, Brown);
				w.DrawText(tx, y, "?", font, Brown);
			}
			else {
				h[0] = FormatHexDigit((b & 0xf0) >> 4);
				h[1] = FormatHexDigit(b & 0x0f);
				h[2] = '\0';
				w.DrawText(x, y, h, font, SColorText);
				Color color = SColorMark;
				switch(b) {
				case '\a': *h = 'a'; break;
				case '\b': *h = 'b'; break;
				case '\t': *h = 't'; break;
				case '\f': *h = 'f'; break;
				case '\r': *h = 'r'; break;
				case '\n': *h = 'n'; break;
				case '\v': *h = 'v'; break;
				case '\0': *h = '0'; break;
				default:
					if(b >= 32) {
						*h = b;
						color = SColorText;
					}
					else {
						*h = '.';
						color = SColorDisabled;
					}
				}
				h[1] = '\0';
				w.DrawText(tx, y, h, charset, font, color);
			}
			tx += fsz.cx;
			x += fcx3;
		}
		y += fsz.cy;
	}
}

void HexView::MouseWheel(Point, i32 zdelta, dword)
{
	sb.Wheel(zdelta);
}

void HexView::SetSb()
{
	sbm = 0;
	zu64 sz = total;
	while((sz >> sbm) > (1 << 30))
		sbm++;
	sb.SetTotal(i32(sz >> sbm) / columns + 2);
	sb.SetPage(i32(rows >> sbm));
	sb.Set(i32(sc >> sbm) / columns + 1);
}

void HexView::Layout()
{
	Size sz = GetSize();
	columns = fixed ? fixed : max(4, (sz.cx - (IsLongMode() ? 18 : 10) * fsz.cx) / (4 * fsz.cx));
	rows = max(1, sz.cy / fsz.cy);
	bytes = columns * rows;
	SetSb();
}

void HexView::SetStart(zu64 start_)
{
	start = start_;
	Layout();
	SetSb();
	Refresh();
	RefreshInfo();
}

void HexView::SetTotal(zu64 _total)
{
	total = _total;
	Layout();
	SetSb();
	Refresh();
	RefreshInfo();
}

void HexView::SetSc(zu64 address)
{
	sc = minmax(address, start, total);
	SetSb();
	Refresh();
}

void HexView::Scroll()
{
	z64 q = (z64)(i32)sb << sbm;
	if(q == 0)
		sc = 0;
	else
		sc = (q - 1) * columns + sc % columns;
	Refresh();
}

void HexView::RefreshInfo()
{
	if(total) {
		info.SetPos(cursor + start, IsLongMode());
		for(i32 i = 0; i < 80; i++)
			info.Set(i, Byte(cursor + i));
	}
	else
		info.SetEmpty();
}

void HexView::SetCursor(zu64 _cursor)
{
	cursor = _cursor;
	
	if(cursor > total)
		cursor = total - 1;
	i32 q = i32(sc % columns);
	if(cursor >= sc + bytes)
		sc = cursor - bytes + columns;
	if(cursor < sc) {
		sc = cursor;
	}
	if(sc > (zu64)q)
		sc = (sc - q) / columns * columns + q;
	if(sc >= total)
		sc = total - 1;
	SetSb();
	Refresh();
	RefreshInfo();
}

void HexView::LeftDown(Point p, dword)
{
	i32 rowi = p.y / fsz.cy;
	i32 x = (IsLongMode() ? 17 : 9) * fsz.cx;
	i32 tx = x + columns * fcx3;
	if(p.x >= x && p.x < tx) {
		x = p.x - x;
		i32 q = x / fcx3;
		if(x - q * fcx3 < 2 * fsz.cx && q < columns) {
			zu64 c = sc + rowi * columns + q;
			if(c < total)
				SetCursor(c);
		}
	}
	else
	if(p.x >= tx) {
		i32 q = (p.x - tx) / fsz.cx;
		if(q >= 0 && q < columns) {
			zu64 c = sc + rowi * columns + q;
			if(c < total)
				SetCursor(c);
		}
	}
	SetFocus();
}

bool HexView::Key(dword key, i32)
{
	i32 pg = max(columns, bytes - columns);
	i32 q = i32(sc % columns);
	switch(key) {
	case K_LEFT:
		SetCursor(cursor - 1);
		return true;
	case K_RIGHT:
		SetCursor(cursor + 1);
		return true;
	case K_UP:
		SetCursor(cursor - columns);
		return true;
	case K_DOWN:
		SetCursor(cursor + columns);
		return true;
	case K_PAGEUP:
		SetSc(sc - pg);
		SetCursor(cursor - pg);
		return true;
	case K_PAGEDOWN:
		SetSc(sc + pg);
		SetCursor(cursor + pg);
		return true;
	case K_CTRL_LEFT:
		SetSc(sc - 1);
		break;
	case K_CTRL_RIGHT:
		SetSc(sc + 1);
		break;
	case K_CTRL_UP:
		SetSc(sc - columns);
		break;
	case K_CTRL_DOWN:
		SetSc(sc + columns);
		break;
	case K_HOME:
		SetCursor((cursor - q) / columns * columns + q);
		break;
	case K_END:
		SetCursor((cursor - q) / columns * columns + q + columns - 1);
		break;
	case K_CTRL_HOME:
	case K_CTRL_PAGEUP:
		SetCursor(0);
		break;
	case K_CTRL_END:
	case K_CTRL_PAGEDOWN:
		SetCursor(total - 1);
		break;
	}
	return MenuBar::Scan(WhenBar, key);
}

void HexView::SetColumns(i32 x)
{
	FixedColumns(x);
}

void HexView::SetCharset(i32 chr)
{
	Charset(chr);
}

void HexView::StdGoto(const Txt& s)
{
	CParser p(s);
	i32 n = 10;
	if(p.Char2('0', 'x') || p.Char('$') || p.Char('#'))
		n = 16;
	if(p.IsNumber(n)) {
		zu64 a = p.ReadNumber(n);
		if(a < total) {
			SetCursor(a);
			SetSc(a);
			return;
		}
	}
	Exclamation("Неверная позиция!");
}

void HexView::Goto()
{
	if(go.Execute() == IDOK)
		WhenGoto((Txt)~go.text);
}

void HexView::ColumnsMenu(Bar& bar)
{
	bar.Add("Auto", OTVET1(SetColumns, 0))
	   .Radio(fixed == 0);
	bar.Add("8", OTVET1(SetColumns, 8))
	   .Radio(fixed == 8);
	bar.Add("16", OTVET1(SetColumns, 16))
	   .Radio(fixed == 16);
	bar.Add("32", OTVET1(SetColumns, 32))
	   .Radio(fixed == 32);
}

void HexView::SetInfo(i32 m)
{
	info.SetMode(m);
}

void HexView::InfoMenu(Bar& bar)
{
	bar.Add("None", OTVET1(SetInfo, 0))
	   .Check(info.GetMode() == 0);
	bar.Add("Standard", OTVET1(SetInfo, 1))
	   .Check(info.GetMode() == 1);
	bar.Add("Extended", OTVET1(SetInfo, 2))
	   .Check(info.GetMode() == 2);
}

void HexView::CharsetMenu(Bar& bar)
{
	for(i32 i = 1; i < CharsetCount(); i++)
		bar.Add(CharsetName(i), OTVET1(SetCharset, i))
		   .Radio(charset == i);
}

void HexView::StdMenu(Bar& bar)
{
	bar.Add("Перейти к..", [=, this] { WhenGotoDlg(); })
	   .Key(K_CTRL_G);
	bar.Add("Колонки", OTVET(ColumnsMenu));
	bar.Add("Символ", OTVET(CharsetMenu));
	bar.Add("Инфо о позиции", OTVET(InfoMenu));
}

void HexView::RightDown(Point p, dword w)
{
	LeftDown(p, w);
	MenuBar::Execute(WhenBar);
}

HexView& HexView::SetFont(Font fnt)
{
	font = fnt;
	fsz = GetTextSize("X", font);
	fcx3 = 3 * fsz.cx;
	Layout();
	Refresh();
	SetSb();
	return *this;
}

void  HexView::SerializeSettings(Stream& s)
{
	i32 version = 0;
	s / version;
	s / fixed;
	s % charset;
	i32 mode = info.GetMode();
	s / mode;
	info.SetMode(mode);
	go.text.SerializeList(s);
}

HexView::HexView()
{
	SetFont(CourierZ(12));
	BackPaint();
	charset = CHARSET_WIN1252;
	sb <<= OTVET(Scroll);
	SetFrame(InsetFrame());
	AddFrame(sb);
	cursor = sc = 0;
	total = 0;
	fixed = 0;
	SetSc(0);
	SetCursor(0);
	AddFrame(info);
	info.SetMode(1);
	WhenBar = OTVET(StdMenu);
	CtrlLayoutOKCancel(go, "Перейти");
	WhenGoto = OTVET(StdGoto);
	WhenGotoDlg = OTVET(Goto);
}

}
