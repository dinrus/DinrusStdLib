#include <drx/Esc/Esc.h>


namespace drx {

EscVal EscFromStdVal(const Val& v)
{
	EscVal r;
	Time t;
	if(!IsNull(v))
		switch(v.GetType()) {
		case BOOL_V:
		case INT_V:
		case INT64_V:
		case DOUBLE_V:
			r = (double)v;
			break;
		case TXT_V:
		case WTXT_V:
			r = (WTxt)v;
			break;
		case TIME_V:
			t = v;
			r.MapSet("hour", t.hour);
			r.MapSet("minute", t.minute);
			r.MapSet("second", t.second);
		case DATE_V:
			t = v;
			r.MapSet("year", t.year);
			r.MapSet("month", t.month);
			r.MapSet("day", t.day);
			break;
		case VALUEARRAY_V:
			ValArray va = v;
			r.SetEmptyArray();
			for(i32 i = 0; i < va.GetCount(); i++)
				r.ArrayAdd(EscFromStdVal(va[i]));
		}
	return r;
}

Val StdValFromEsc(const EscVal& v)
{
	if(v.IsNumber())
		return v.GetNumber();
	if(v.IsArray())
		return (WTxt)v;
	if(IsTime(v))
		return Time(v.GetFieldInt("year"), v.GetFieldInt("month"), v.GetFieldInt("day"),
			v.GetFieldInt("hour"), v.GetFieldInt("minute"), v.GetFieldInt("second"));
	if(IsDate(v))
		return Date(v.GetFieldInt("year"), v.GetFieldInt("month"), v.GetFieldInt("day"));
	return Val();
}

void SIC_StdFormat(EscEscape& e)
{
	e = StdFormat(StdValFromEsc(e[0]));
}

void StdValLib(ArrayMap<Txt, EscVal>& global)
{
	Escape(global, "StdFormat(x)", SIC_StdFormat);
}

}
