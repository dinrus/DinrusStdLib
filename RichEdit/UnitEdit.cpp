#include "RichEdit.h"

namespace drx {

double UnitMultiplier(i32 unit) {
	static double m[] =
	{
		1,
		72.0 / 600,
		1.0 / 600,
		25.4 / 600,
		2.54 / 600,
	};
	ASSERT(unit >= UNIT_DOT && unit <= UNIT_CM);
	return m[unit];
}

tukk UnitText(i32 unit) {
	static tukk txt[] =
	{
		"dt",
		"pt",
		"\"",
		"mm",
		"cm"
	};
	return txt[unit];
}

void UnitEdit::Read(double& d, i32& u) const
{
	Txt txt = GetText().ToTxt();
	tukk s = txt;
	u = unit;
	d = Null;
	if(s && *s) {
		tukk e;
		i32 sign = 1;
		for(;;) {
			if(*s == '-' && sgn)
				sign = -1;
			else
			if(*s != ' ')
				break;
			s++;
		}
		d = ScanDouble(s, &e);
		if(IsNull(d))
			return;
		d *= sign;
		if(e == s) {
			d = Null;
			return;
		}
		while(*e == ' ') e++;
		if(*e == '\"' || *e == 'i')
			u = UNIT_INCH;
		if(*e == 'm')
			u = UNIT_MM;
		if(*e == 'p' || *e == 'b')
			u = UNIT_POINT;
		if(*e == 'c')
			u = UNIT_CM;
		if(*e == 'd')
			u = UNIT_DOT;
	}
}

Val UnitEdit::GetData() const
{
	double q;
	i32 u;
	Read(q, u);
	return IsNull(q) ? Null : i32(q / UnitMultiplier(u) + 0.5);
}

Txt UnitEdit::AsText(double d, i32 unit)
{
	if(IsNull(d))
		return Null;
	Txt utxt = UnitText(unit);
	if(unit == UNIT_POINT)
		d = floor(10 * d + 0.5) / 10;
	return FormatF(d, unit == UNIT_DOT ? 0 : unit == UNIT_MM ? 1 : 2) + ' ' + utxt;
}

Txt UnitEdit::DotAsText(i32 dot, i32 unit)
{
	if(IsNull(dot)) return Null;
	return AsText(dot * UnitMultiplier(unit), unit);
}

Val UnitEdit::Format(const Val& v) const
{
	return DotAsText(v, unit);
}

void UnitEdit::SetData(const Val& v)
{
	SetText(DotAsText(v, unit).ToWTxt());
}

bool UnitEdit::Key(dword key, i32 repcnt)
{
	if(key == K_UP)   { Spin(+1); return true; }
	if(key == K_DOWN) { Spin(-1); return true; }
	return EditField::Key(key, repcnt);
}

void UnitEdit::MouseWheel(Point p, i32 zdelta, dword keyflags)
{
	Spin(zdelta < 0 ? -1 : 1);
}

void UnitEdit::Spin(i32 delta)
{
	double q;
	i32 u;
	Read(q, u);
	if(IsNull(q))
		q = 0;
	else {
		double h = 10;
		switch(u) {
		case UNIT_DOT:   h = 10; break;
		case UNIT_POINT: h = 0.5; break;
		case UNIT_MM:    h = 0.5; break;
		case UNIT_CM:
		case UNIT_INCH:  h = 0.05; break;
		default:         NEVER();
		}
		h *= delta;
		q = ceil(q / h + 1e-2) * h;
		if(!sgn && q < 0)
			q = 0;
	}
	SetText(AsText(q, unit).ToWTxt());
	UpdateAction();
}

wchar CharFilterUnitEdit(wchar c)
{
	return IsAlpha(c) ? ToLower(c) : IsDigit(c) || c == ' ' || c == '\"' || c == '.' ? c : 0;
}

wchar CharFilterUnitEditSgn(wchar c)
{
	return c == '-' ? c : CharFilterUnitEdit(c);
}

UnitEdit& UnitEdit::WithSgn(bool b)
{
	sgn = b;
	SetFilter(b ? CharFilterUnitEditSgn : CharFilterUnitEdit);
	return *this;
}

UnitEdit::UnitEdit()
{
	AddFrame(spin);
	spin.inc.WhenRepeat = spin.inc.WhenPush = OTVET1(Spin, 1);
	spin.dec.WhenRepeat = spin.dec.WhenPush = OTVET1(Spin, -1);
	unit = UNIT_DOT;
	WithSgn(false);
}

}