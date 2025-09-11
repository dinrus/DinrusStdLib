inline
void ValMap::Add(const Val& key, const Val& value) {
	Data& d = UnShare();
	d.key.Add(key);
	d.value.Add(value);
}

inline
i32 StdValCompare(const Val& a, const Val& b, const LangInfo& f)
{
	return IsTxt(a) && IsTxt(b) ? CompareTxts(a, b, f) : a.Compare(b);
}

inline
i32 StdValCompare(const Val& a, const Val& b, i32 language)
{
	return StdValCompare(a, b, GetLangInfo(language));
}

inline
i32 StdValCompare(const Val& a, const Val& b)
{
	return StdValCompare(a, b, GetLangInfo());
}

inline
i32 StdValCompareDesc(const Val& a, const Val& b, const LangInfo& f)
{
	return -StdValCompare(a, b, f);
}

inline
i32 StdValCompareDesc(const Val& a, const Val& b, i32 language)
{
	return -StdValCompare(a, b, language);
}

inline
i32 StdValCompareDesc(const Val& a, const Val& b)
{
	return -StdValCompare(a, b);
}


#ifdef DEPRECATED
template <class T>
struct RawRef : public RefManager {
	virtual void  SetVal(uk p, const Val& v)       { *(T *) p = RawVal<T>::Extract(v); }
	virtual Val   GetVal(ukk p)                 { return RawVal<T>(*(const T *) p); }
	virtual i32   GetType()                               { return GetValTypeNo<T>(); }
	virtual ~RawRef() {}
};

template <class T>
Ref RawAsRef(T& x) {
	return Ref(&x, &Single< RawRef<T> >());
}

template <class T>
struct RichRef : public RawRef<T> {
	virtual Val   GetVal(ukk p)                 { return RichToVal(*(T *) p); }
	virtual bool  IsNull(ukk p)                   { return DRX::IsNull(*(T *) p); }
	virtual void  SetVal(uk p, const Val& v)       { *(T *) p = T(v); }
	virtual void  SetNull(uk p)                        { DRX::SetNull(*(T *)p); }
};

template <class T>
Ref RichAsRef(T& x) {
	return Ref(&x, &Single< RichRef<T> >());
}
#endif