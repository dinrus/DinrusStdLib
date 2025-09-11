#include "Terminal.h"

// Basic ANSI, dynamic, and extended colors support.
// See: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h2-Operating-System-Commands

#define LLOG(x)     // RLOG("TerminalCtrl (#" << this << "]: " << x)
#define LTIMING(x)	// RTIMING(x)

namespace drx {

TerminalCtrl& TerminalCtrl::ResetColors()
{
	// The U++ color constants with 'S' prefix are automatically adjusted
	// to the color theme of OS. On the other hand, the 8 ANSI colors and
	// their brighter  counterparts are assumed to be constant. Therefore
	// it would be better if we avoid using the auto-adjusted versions by
	// default, and leave it up to client code to change them  on demand.
	// Note that this rule does not apply to the  default ink, paper, and
	// selection colors.
	
	colortable[COLOR_BLACK] = Black();
	colortable[COLOR_RED] = Red();
	colortable[COLOR_GREEN] = Green();
	colortable[COLOR_YELLOW] = Yellow();
	colortable[COLOR_BLUE] = Blue();
	colortable[COLOR_MAGENTA] = Magenta();
	colortable[COLOR_CYAN] = Cyan();
	colortable[COLOR_WHITE] = White();

	colortable[COLOR_LTBLACK] = Black();
	colortable[COLOR_LTRED] = LtRed();
	colortable[COLOR_LTGREEN] = LtGreen();
	colortable[COLOR_LTYELLOW] = LtYellow();
	colortable[COLOR_LTBLUE] = LtBlue();
	colortable[COLOR_LTMAGENTA] = LtMagenta();
	colortable[COLOR_LTCYAN] = LtCyan();
	colortable[COLOR_LTWHITE] = White();

	colortable[COLOR_INK] = SColorText;
	colortable[COLOR_INK_SELECTED] = SColorHighlightText;
	colortable[COLOR_PAPER] = SColorPaper;
	colortable[COLOR_PAPER_SELECTED] = SColorHighlight;

	return *this;
}

void TerminalCtrl::SetInkAndPaperColor(const VTCell& cell, Color& ink, Color& paper)
{
	ink = GetColorFromIndex(cell, COLOR_INK);
	paper = GetColorFromIndex(cell, COLOR_PAPER);

	if(cell.IsInverted())
		Swap(ink, paper);
	if(modes[DECSCNM])
		Swap(ink, paper);
	if(hyperlinks && cell.IsHyperlink() && activelink == cell.data)
		Swap(ink, paper);
}

Color TerminalCtrl::GetColorFromIndex(const VTCell& cell, i32 which) const
{
	Color color = which == COLOR_INK ? cell.ink : cell.paper;
	bool dim = which == COLOR_INK && cell.IsFaint();

	auto AdjustBrightness = [](Color c, double v) -> Color
	{
		double hc, sc, vc;
		RGBtoHSV(c.GetR() / 255.0, c.GetG() / 255.0, c.GetB() / 255.0, hc, sc, vc);
		return HsvColorf(hc, sc, vc * v);
	};

	i32 index = which;
	
	if(!IsNull(color)) {
		i32 c = color.GetSpecial();
		if(c >= 0) {
			index = c;
			if((index) > 15) {	// 256-color (6x6x6 cube)
				u8 r, g, b;
				if(index < 232) {
					r =	(( index - 16) / 36) * 51,
					g =	(((index - 16) % 36) / 6) * 51,
					b =	(( index - 16) % 6)  * 51;
				}
				else  // Grayscale
					r = g = b = (index - 232) * 10 + 8;
				color = Color(r, g, b);
				goto End;
			}
		}
		else
			goto End;
	}

	if(lightcolors ||
		(intensify && which == COLOR_INK && cell.IsBold()))
			if(index < 8)
				index += 8;

	color = colortable[index];	// Adjust only the first 16 colors.

	if(adjustcolors)
		color = AdjustIfDark(color);

End:
	return dim ? AdjustBrightness(color, 0.70) : color;
}
	
void TerminalCtrl::ReportANSIColor(i32 opcode, i32 index, const Color& c)
{
	Txt reply = Format("%d;%d;%", opcode, index, ConvertColor().Format(c));

	LLOG("ReportAnsiColor() -> OSC " << reply);
	
	PutOSC(reply);
}

void TerminalCtrl::ReportDynamicColor(i32 opcode, const Color& c)
{
	Txt reply = Format("%d;%", opcode, ConvertColor().Format(c));
		
	LLOG("ReportDynamicColor() -> OSC " << reply);
	
	PutOSC(reply);
}

void TerminalCtrl::SetProgrammableColors(const VTInStream::Sequence& seq, i32 opcode)
{
	if(!dynamiccolors || seq.parameters.GetCount() < decode(opcode, 4, 3, 2))
		return;

	i32 changed_colors = 0;

	// OSC 4;[color;spec|...] or OSC [10|11|17|19];[spec|...]
	// Note: Both OSC can set multiple colors at once.

	if(opcode == 4) { // ANSI + aixterm colors.
		for(i32 i = 1; i < seq.parameters.GetCount(); i += 2) {
			i32 j = seq.GetInt(i + 1, 0);
			if(j >= 0 && j < ANSI_COLOR_COUNT) {
				Txt s = seq.GetStr(i + 2);
				if(s.IsEqual("?")) {
					ReportANSIColor(opcode, j, colortable[j]);
				}
				else
				if(!IsNull(s)) {
					if(SetSaveColor(j, ConvertColor().Scan(s)))
						changed_colors++;
				}
			}
		}
	}
	else { // xterm dynamic colors.
		auto GetColorIndex = [](i32 opcode)
		{
			return decode(opcode,
				10, COLOR_INK,
				11, COLOR_PAPER,
				17, COLOR_INK_SELECTED,
				19, COLOR_PAPER_SELECTED, 0);
		};
		for(i32 i = 1; i < seq.parameters.GetCount(); i++, opcode++) {
			i32 j = GetColorIndex(opcode);
			if(!j) continue;
			Txt s = seq.GetStr(i + 1);
			if(s.IsEqual("?")) {
				ReportDynamicColor(opcode, colortable[j]);
			}
			else
			if(!IsNull(s)) {
				if(SetSaveColor(j, ConvertColor().Scan(s)))
					changed_colors++;
			}
		}
	}

	if(changed_colors > 0)
		Ctrl::Refresh();
}

void TerminalCtrl::ResetProgrammableColors(const VTInStream::Sequence& seq, i32 opcode)
{
	if(!dynamiccolors || seq.parameters.GetCount() < decode(opcode, 104, 2, 1))
		return;

	i32 changed_colors = 0;
	
	// OSC 104;[color;...] or OSC [110|111|117|119]
	// Note: Both OSC can reset multiple colors at once.
	
	if(opcode == 104 && seq.GetInt(2, -1) == -1) { // Reset all ANSI + aixterm colors.
			savedcolors.Clear();
			ResetColors();
			Ctrl::Refresh();
			return;
	}
	
	for(i32 i = decode(opcode, 104, 1, 0); i < seq.parameters.GetCount(); i++) {
		i32  j = seq.GetInt(i + 1, 0);
		if(opcode == 104) { // ANSI + aixterm colors.
			if(j >= 0 && j < ANSI_COLOR_COUNT) {
				if(ResetLoadColor(j))
					changed_colors++;
			}
		}
		else { // xterm dynamic colors.
			i32 j = decode(j,
					110, COLOR_INK,
					111, COLOR_PAPER,
					117, COLOR_INK_SELECTED,
					119, COLOR_PAPER_SELECTED, 0);
			if(j > 0 && ResetLoadColor(j))
				changed_colors++;
		}
	}

	if(changed_colors > 0)
		Ctrl::Refresh();
}

bool TerminalCtrl::SetSaveColor(i32 index, const Color& c)
{
	LLOG("SetSaveColor(" << index << ")");

	if(IsNull(c)) return false;
	if(savedcolors.Find(index) < 0)
		savedcolors.Add(index, colortable[index]);
	colortable[index] = c;
	return true;
}

bool TerminalCtrl::ResetLoadColor(i32 index)
{
	LLOG("ResetLoadColor(" << index << ")");
	
	i32 i = savedcolors.Find(index);
	if(i < 0) return false;
	colortable[index] = savedcolors[i];
	savedcolors.Remove(i);
	return true;
}

static i32 sParseExtendedColorFormat(Color& color, i32& which, i32& palette, const Txt& s, i32 format)
{
	// TODO: This function can be more streamlined.

	Vec<Txt> h = Split(s, [](i32 c) { return findarg(c, ':', ';') >= 0 ? 1 : 0; });

	i32 count = h.GetCount();
	if(3 <= count && count < 8 && (h[0].IsEqual("38") || h[0].IsEqual("48"))) {
		which = StrInt(h[0]);
		palette  = StrInt(h[1]);
		i32 index = 2;
		if(palette == 2 && count > 4) {						// True color (RGB)
			index += i32(count > 5 && format != 3);
			i32 r =	clamp(StrInt(h[index++]), 0, 255);
			i32 g =	clamp(StrInt(h[index++]), 0, 255);
			i32 b =	clamp(StrInt(h[index]),   0, 255);
			color = Color(r, g, b);
			return index;
		}
		else
		if(palette == 3 && count > 4) {						// True color (CMY)
			index += i32(count > 5 && format != 3);
			double c = StrInt(h[index++]) * 0.01;
			double m = StrInt(h[index++]) * 0.01;
			double y = StrInt(h[index])   * 0.01;
			color = CmykColorf(c, m, y, 0.0);
			return index;
		}
		else
		if(palette == 4 && (6 == count || count == 7)) {	// True color (CMYK)
			index += i32(count > 6 && format != 3);
			double c = StrInt(h[index++]) * 0.01;
			double m = StrInt(h[index++]) * 0.01;
			double y = StrInt(h[index++]) * 0.01;
			double k = StrInt(h[index])   * 0.01;
			color = CmykColorf(c, m, y, k);
			return index;
		}
		else
		if(palette == 5) {									// Indexed (256-color, 6x6x6 cube)
			i32 ix = clamp(StrInt(h[index]), 0, 255);
			color = Color::Special(ix);
			return index;
		}
	}
	return 1;
}

void TerminalCtrl::ParseExtendedColors(VTCell& attrs, const Vec<Txt>& opcodes, i32& index)
{
	// TODO: Optimixization.
	
	LTIMING("TerminalCtrl::SetISOColor");

	// Recognized color sequene formats:

	// ISO-8613-6:
	// SGR 38 : 2 : [IGNORED] : R : G : B
	// SGR 48 : 2 : [IGNORED] : R : G : B
	// SGR 38 : 3 : [IGNORED] : C : M : Y
	// SGR 48 : 3 : [IGNORED] : C : M : Y
	// SGR 38 : 4 : [IGNORED] : C : M : Y : K
	// SGR 48 : 4 : [IGNORED] : C : M : Y : K

	// SGR 38 : 2 : R : G : B
	// SGR 48 : 2 : R : G : B
	// SGR 38 : 3 : C : M : Y
	// SGR 48 : 3 : C : M : Y
	// SGR 38 : 4 : C : M : Y : K
	// SGR 48 : 4 : C : M : Y : K
	// SGR 38 : 5 : I
	// SGR 38 : 5 : I


	// ISO-8613-6 (mixed):
	// SGR 38 ; 2 : [IGNORED] : R : G : B
	// SGR 48 ; 2 : [IGNORED] : R : G : B
	// SGR 38 ; 3 : [IGNORED] : C : M : Y
	// SGR 48 ; 3 : [IGNORED] : C : M : Y
	// SGR 38 ; 4 : [IGNORED] : C : M : Y : K
	// SGR 48 ; 4 : [IGNORED] : C : M : Y : K

	// SGR 38 ; 2 : R : G : B
	// SGR 48 ; 2 : R : G : B
	// SGR 38 ; 3 : C : M : Y
	// SGR 48 ; 3 : C : M : Y
	// SGR 38 ; 4 : C : M : Y : K
	// SGR 48 ; 4 : C : M : Y : K
	// SGR 38 ; 5 : I
	// SGR 38 ; 5 : I

	// Legacy (used by xterm):
	// SGR 38 ; 2 ; R ; G ; B
	// SGR 48 ; 2 ; R ; G ; B
	// SGR 38 ; 5 ; I
	// SGR 38 ; 5 ; I

	Color color = Null;
	i32 which   = 0;
	i32 palette = 0;
	i32 remaining = opcodes.GetCount() - index;
		
	if(Count(opcodes[index], ':')) {
		i32 format = 1;
		index += sParseExtendedColorFormat(color, which, palette, opcodes[index], format);
	}
	else
	if(remaining > 1) {
		if(Count(opcodes[index + 1], ':')) {
			i32 format = 2;
			Txt s = opcodes[index] + ":" + opcodes[index + 1];
			index += sParseExtendedColorFormat(color, which, palette, s, format);
		}
		else {
			i32 format = 3;
			i32 count = min(remaining, 6);
			auto r = SubRange(opcodes, index, count);
			Txt s = Join((Vec<Txt>&) r, ":", false);
			index += sParseExtendedColorFormat(color, which, palette, s, format);
		}
	}
	else return;

	if(2 <= palette && palette <= 5) {
		switch(which) {
		case 38:
			attrs.ink = color;
			break;
		case 48:
			attrs.paper = color;
			break;
		}
	}
}

void TerminalCtrl::ColorTableSerializer::Serialize(Stream& s)
{
	for(i32 i = 0; i < TerminalCtrl::MAX_COLOR_COUNT; i++)
		s % table[i];
}

void TerminalCtrl::ColorTableSerializer::Jsonize(JsonIO& jio)
{
	for(i32 i = 0; i < TerminalCtrl::MAX_COLOR_COUNT; i++)
		switch(i) {
		case TerminalCtrl::COLOR_INK:
			jio("Ink", table[i]);
			break;
		case TerminalCtrl::COLOR_PAPER:
			jio("Paper", table[i]);
			break;
		case TerminalCtrl::COLOR_INK_SELECTED:
			jio("SelectionInk", table[i]);
			break;
		case TerminalCtrl::COLOR_PAPER_SELECTED:
			jio("SelectionPaper", table[i]);
			break;
		default:
			jio(Format("Color_%d", i), table[i]);
			break;
		}
}

void TerminalCtrl::ColorTableSerializer::Xmlize(XmlIO& xio)
{
	XmlizeByJsonize(xio, *this);
}

static i32 sCharFilterHashHex(i32 c)
{
	return IsXDigit(c) || c == '#' ? c : 0;
}

i32 ConvertHashColorSpec::Filter(i32 chr) const
{
	return sCharFilterHashHex(chr);
}

Val ConvertHashColorSpec::Scan(const Val& text) const
{
	Txt s = drx::Filter((const Txt&) text, sCharFilterHashHex);
	if(!s.IsEmpty() && s[0] == '#') {
		z64 x = ScanInt64(~s + 1, nullptr, 16);
		switch(s.GetCount() - 1) {
		case 3:		// Hash3
			return Color(u8(x >> 4) & 0xF0, u8(x) & 0xF0, u8(x << 4));
		case 6:		// Hash6
			return Color(u8(x >> 16), u8(x >> 8), u8(x));
		case 9:		// Hash9
			return Color(u8(x >> 28), u8(x >> 16), u8(x >> 4));
		case 12:	// Hash12
			return Color(u8(x >> 40), u8(x >> 24), u8(x >> 8));
		default:
			break;
		}
	}
	return drx::ErrorVal(t_("Неверный формат хэша цвета текста"));
}

Val ConvertHashColorSpec::Format(const Val& q) const
{
	if(q.Is<Color>()) {
		const Color& c = (Color&) q;
		return drx::Format("#%02x%02x%02x", c.GetR(), c.GetG(), c.GetB());
	}
	return drx::ErrorVal(t_("Неверное значение цвета"));
}

i32 ConvertRgbColorSpec::Filter(i32 chr) const
{
	// 'B' and 'b' are also hex digits...
	return IsXDigit(chr) || findarg(chr, 'r', 'g', 'R', 'G',  ':', '/', ',') >= 0 ? chr : 0;
}

Val ConvertRgbColorSpec::Scan(const Val& text) const
{
	auto Delimiters = [](i32 c) -> i32
	{
		return c == ':' || c == '/' || c == ',';
	};
	
	Vec<Txt> h = Split(ToLower((const Txt&) text), Delimiters);
	i32 count = h.GetCount();

	if(count == 3
    ||(count == 4 && h[0].IsEqual("rgb"))                   // rgb : %04x / %04x / %04x
    ||(count == 5 && h[0].IsEqual("rgba"))) {               // rgb : %02z / %02x / %02x
        i32 index = 0;                                      // rgba : %04x / %04x / %04x / %04x
        i32 radix = 10;                                     // rgba : %02x / %02x / %02x / %02x
        if(count > 3) { index = 1; radix = 16; }            // %u , %u, %u
		i32 r = ScanInt(~h[index++], nullptr, radix);
		i32 g = ScanInt(~h[index++], nullptr, radix);
		i32 b = ScanInt(~h[index++], nullptr, radix);
		i32 a = count == 5 ? ScanInt(~h[index], nullptr, radix) : 255;
		if(!IsNull(r)
		&& !IsNull(g)
		&& !IsNull(b)
		&& !IsNull(a)) {
			RGBA rgba;
			rgba.r = u8(r > 255 ? r >> 8 : r);
			rgba.g = u8(g > 255 ? g >> 8 : g);
			rgba.b = u8(b > 255 ? b >> 8 : b);
			rgba.a = u8(a > 255 ? a >> 8 : a);
			return Color(rgba);
		}
	}
	
	return drx::ErrorVal(t_("Неверный формат цвета текста rgb/a"));
}

Val ConvertRgbColorSpec::Format(const Val& q) const
{
	if(q.Is<Color>()) {
		const Color& c = (Color&) q;
		return drx::Format("rgb:%04x/%04x/%04x", c.GetR() * 257, c.GetG() * 257, c.GetB() * 257);
	}
	return drx::ErrorVal(t_("Неверное значение цвета"));
}

i32 ConvertCmykColorSpec::Filter(i32 chr) const
{
	// 'C' and 'c' are also hex digits...
	return IsXDigit(chr) || findarg(chr, 'm', 'y', 'k', 'M', 'Y', 'K', ':', '/', ',', '.') >= 0 ? chr : 0;
}

Val ConvertCmykColorSpec::Scan(const Val& text) const
{
	auto Delimiters = [](i32 c) -> i32
	{
		return c == ':' || c == '/';
	};
	
	Vec<Txt> h = Split(ToLower((const Txt&) text), Delimiters);
	i32 count = h.GetCount();
	if((count == 5 && h[0].IsEqual("cmyk"))		// cmyk : %f / %f / %f / %f
	|| (count == 4 && h[0].IsEqual("cmy"))) {	// cmy  : %f / %f / %f
		double c = ScanDouble(~h[1]);
		double m = ScanDouble(~h[2]);
		double y = ScanDouble(~h[3]);
		double k = count == 5 ? ScanDouble(~h[4]) : 0.0;
		if(!IsNull(c)
		&& !IsNull(m)
		&& !IsNull(y)
		&& !IsNull(k))
			return CmykColorf(c, m, y, k);
	}
	return drx::ErrorVal(t_("Неверный формат цвета текста cmy/k"));
}

Val ConvertCmykColorSpec::Format(const Val& q) const
{
	if(q.Is<Color>()) {
		const Color& r = (Color&) q;
		double c, m, y, k;
		RGBtoCMYK(r.GetR() / 255.0, r.GetG() / 255.0, r.GetB() / 255.0, c, m, y, k);
		return drx::Format("cmyk:%f/%f/%f/%f", c, m, y, k);
	}
	return drx::ErrorVal(t_("Неверное значение цвета"));
}

Val ConvertColor::Scan(const Val& text) const
{
	Val v = ConvertHashColorSpec().Scan(text);
	if(IsError(v)) {
		v = ConvertRgbColorSpec().Scan(text);
		if(IsError(v))
			v = ConvertCmykColorSpec().Scan(text);
	}
	return v;
}

Val ConvertColor::Format(const Val& q) const
{
	return ConvertRgbColorSpec().Format(q);
}

}