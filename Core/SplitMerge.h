Vec<Txt> Split(i32 maxcount, tukk s, tukk  (*text_filter)(tukk ), bool ignoreempty = true);
Vec<Txt> Split(i32 maxcount, tukk s, wchar (*filter)(wchar), bool ignoreempty = true);
Vec<Txt> Split(i32 maxcount, tukk s, wchar chr, bool ignoreempty = true);
Vec<Txt> Split(i32 maxcount, tukk s, tukk text, bool ignoreempty = true);
Vec<Txt> Split(tukk s, tukk (*text_filter)(tukk ), bool ignoreempty = true);
Vec<Txt> Split(tukk s, wchar (*filter)(wchar), bool ignoreempty = true);
Vec<Txt> Split(tukk s, wchar chr, bool ignoreempty = true);
Vec<Txt> Split(tukk s, tukk text, bool ignoreempty = true);

Vec<WTxt> Split(i32 maxcount, wtukk s, wtukk  (*text_filter)(wtukk ), bool ignoreempty = true);
Vec<WTxt> Split(i32 maxcount, wtukk s, wchar (*filter)(wchar), bool ignoreempty = true);
Vec<WTxt> Split(i32 maxcount, wtukk s, wchar chr, bool ignoreempty = true);
Vec<WTxt> Split(i32 maxcount, wtukk s, wtukk text, bool ignoreempty = true);
Vec<WTxt> Split(wtukk s, wtukk (*text_filter)(wtukk ), bool ignoreempty = true);
Vec<WTxt> Split(wtukk s, wchar (*filter)(wchar), bool ignoreempty = true);
Vec<WTxt> Split(wtukk s, wchar chr, bool ignoreempty = true);
Vec<WTxt> Split(wtukk s, wtukk text, bool ignoreempty = true);

Txt  Join(const Vec<Txt>& im, const Txt& delim, bool ignoreempty = false);
WTxt Join(const Vec<WTxt>& im, const WTxt& delim, bool ignoreempty = false);

template <typename... Args>
bool SplitTo(tukk s, wchar delim, bool ignoreempty, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim, ignoreempty), args...);
}

template <typename... Args>
bool SplitTo(tukk s, wchar delim, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim), args...);
}

template <typename... Args>
bool SplitTo(tukk s, wchar (*delim)(wchar), bool ignoreempty, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim, ignoreempty), args...);
}

template <typename... Args>
bool SplitTo(tukk s, wchar (*delim)(wchar), Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim), args...);
}

template <typename... Args>
bool SplitTo(tukk s, tukk delim, bool ignoreempty, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim, ignoreempty), args...);
}

template <typename... Args>
bool SplitTo(tukk s, tukk delim, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim), args...);
}

template <typename... Args>
void MergeWith(Txt& dest, tukk delim, const Args& ...args)
{
	foreach_arg([&](const Txt& arg) {
		if(arg.GetCount()) {
			if(dest.GetCount())
				dest << delim;
			dest << arg;
		}
	}, args...);
}

template <typename... Args>
Txt Merge(tukk delim, const Args& ...args)
{
	Txt r;
	MergeWith(r, delim, args...);
	return r;
}

template <typename... Args>
bool SplitTo(wtukk s, wchar delim, bool ignoreempty, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim, ignoreempty), args...);
}

template <typename... Args>
bool SplitTo(wtukk s, wchar delim, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim), args...);
}

template <typename... Args>
bool SplitTo(wtukk s, wchar (*delim)(wchar), bool ignoreempty, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim, ignoreempty), args...);
}

template <typename... Args>
bool SplitTo(wtukk s, wchar (*delim)(wchar), Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim), args...);
}

template <typename... Args>
bool SplitTo(wtukk s, wtukk delim, bool ignoreempty, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim, ignoreempty), args...);
}

template <typename... Args>
bool SplitTo(wtukk s, wtukk delim, Args& ...args)
{
	return scatter(Split(sizeof...(args), s, delim), args...);
}

template <typename... Args>
void MergeWith(WTxt& dest, wtukk delim, const Args& ...args)
{
	foreach_arg([&](const WTxt& arg) {
		if(arg.GetCount()) {
			if(dest.GetCount())
				dest << delim;
			dest << arg;
		}
	}, args...);
}

template <typename... Args>
WTxt Merge(wtukk delim, const Args& ...args)
{
	WTxt r;
	MergeWith(r, delim, args...);
	return r;
}