#include "../Core.h"

namespace drx {

template <class S, class Char, class F>
Vec<S> SplitGeneric(i32 maxcount, const F& delim, const Char *s, bool ignoreempty = true)
{
	Vec<S> r;
	r.Reserve(min(maxcount, 8));
	const Char *t = s;
	while(*t && r.GetCount() < maxcount) {
		const Char *q = delim(t);
		if(q) {
			if(!ignoreempty || t > s)
				r.Add().Set(s, i32(t - s)); // This is faster than r.Add(Txt(s, t))...
			t = s = q;
		}
		else
			t++;
	}
	if((!ignoreempty || t > s) && r.GetCount() < maxcount)
		r.Add().Set(s, i32(t - s));
	return r;
}

Vec<Txt> Split(i32 maxcount, tukk s, tukk  (*text_filter)(tukk ), bool ignoreempty)
{
	return SplitGeneric<Txt>(maxcount, text_filter, s, ignoreempty);
}

struct SplitDelimFilter__ {
	wchar (*filter)(wchar);
	tukk operator()(tukk s) const { return (*filter)((u8)*s) ? s + 1 : NULL; }
};

Vec<Txt> Split(i32 maxcount, tukk s, wchar (*filter)(wchar), bool ignoreempty)
{
	SplitDelimFilter__ delim;
	delim.filter = filter;
	return SplitGeneric<Txt>(maxcount, delim, s, ignoreempty);
}

struct SplitDelimChar__ {
	wchar chr;
	tukk operator()(tukk s) const { return *s == chr ? s + 1 : NULL; }
};

Vec<Txt> Split(i32 maxcount, tukk s, wchar chr, bool ignoreempty)
{
	SplitDelimChar__ delim;
	delim.chr = chr;
	return SplitGeneric<Txt>(maxcount, delim, s, ignoreempty);
}

struct SplitDelimText__ {
	tukk ds;
	i32 l;
	tukk operator()(tukk s) const { return strncmp(s, ds, l) == 0 ? s + l : NULL; }
};

Vec<Txt> Split(i32 maxcount, tukk s, tukk text, bool ignoreempty)
{
	SplitDelimText__ delim;
	delim.ds = text;
	delim.l = (i32)strlen(text);
	return delim.l ? SplitGeneric<Txt>(maxcount, delim, s, ignoreempty) : Vec<Txt>();
}

Vec<Txt> Split(tukk s, tukk  (*text_filter)(tukk ), bool ignoreempty)
{
	return Split(INT_MAX, s, text_filter, ignoreempty);
}

Vec<Txt> Split(tukk s, wchar (*filter)(wchar), bool ignoreempty)
{
	return Split(INT_MAX, s, filter, ignoreempty);
}

Vec<Txt> Split(tukk s, wchar chr, bool ignoreempty)
{
	return Split(INT_MAX, s, chr, ignoreempty);
}

Vec<Txt> Split(tukk s, tukk text, bool ignoreempty)
{
	return Split(INT_MAX, s, text, ignoreempty);
}

Vec<WTxt> Split(i32 maxcount, wtukk s, wtukk  (*text_filter)(wtukk ), bool ignoreempty)
{
	return SplitGeneric<WTxt>(maxcount, text_filter, s, ignoreempty);
}

struct SplitDelimWFilter__ {
	wchar (*filter)(wchar);
	wtukk operator()(wtukk s) const { return (*filter)((u8)*s) ? s + 1 : NULL; }
};

Vec<WTxt> Split(i32 maxcount, wtukk s, wchar (*filter)(wchar), bool ignoreempty)
{
	SplitDelimWFilter__ delim;
	delim.filter = filter;
	return SplitGeneric<WTxt>(maxcount, delim, s, ignoreempty);
}

struct SplitDelimWChar__ {
	wchar chr;
	wtukk operator()(wtukk s) const { return *s == chr ? s + 1 : NULL; }
};

Vec<WTxt> Split(i32 maxcount, wtukk s, wchar chr, bool ignoreempty)
{
	SplitDelimWChar__ delim;
	delim.chr = chr;
	return SplitGeneric<WTxt>(maxcount, delim, s, ignoreempty);
}

i32 w_strncmp(wtukk s, wtukk t, i32 n)
{
	while(*s && *t && n > 0) {
		i32 q = (i32)(u16)*s - (i32)(u16)*t;
		if(q)
			return q;
		s++;
		t++;
		n--;
	}
	return 0;
}

struct SplitDelimWText {
	wtukk ds;
	i32 l;
	wtukk operator()(wtukk s) const { return w_strncmp(s, ds, l) == 0 ? s + l : NULL; }
};

Vec<WTxt> Split(i32 maxcount, wtukk s, wtukk text, bool ignoreempty)
{
	SplitDelimWText delim;
	delim.ds = text;
	delim.l = strlen__(text);
	return delim.l ? SplitGeneric<WTxt>(maxcount, delim, s, ignoreempty) : Vec<WTxt>();
}

Vec<WTxt> Split(wtukk s, wchar (*filter)(wchar), bool ignoreempty)
{
	return Split(INT_MAX, s, filter, ignoreempty);
}

Vec<WTxt> Split(wtukk s, wchar chr, bool ignoreempty)
{
	return Split(INT_MAX, s, chr, ignoreempty);
}

Vec<WTxt> Split(wtukk s, wtukk text, bool ignoreempty)
{
	return Split(INT_MAX, s, text, ignoreempty);
}

Vec<WTxt> Split(wtukk s, wtukk  (*text_filter)(wtukk ), bool ignoreempty)
{
	return SplitGeneric<WTxt>(INT_MAX, text_filter, s, ignoreempty);
}

template <class B, class T>
T Join_(const Vec<T>& im, const T& delim, bool ignoreempty) {
	B r;
	bool next = false;
	for(i32 i = 0; i < im.GetCount(); i++)
		if(!ignoreempty || im[i].GetCount()) {
			if(next)
				r.Cat(delim);
			r.Cat(im[i]);
			next = true;
		}
	return T(r);
}

Txt Join(const Vec<Txt>& im, const Txt& delim, bool ignoreempty) {
	return Join_<TxtBuf, Txt>(im, delim, ignoreempty);
}

WTxt Join(const Vec<WTxt>& im, const WTxt& delim, bool ignoreempty) {
	return Join_<WTxtBuf, WTxt>(im, delim, ignoreempty);
}

}