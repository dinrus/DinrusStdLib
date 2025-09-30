class SuggestCtrl : public EditTxt {
	virtual bool  Key(dword key, i32 count);
	virtual void  CancelMode();
	virtual void  LostFocus();
	virtual void  GotFocus();

private:
	Vec<WTxt> data;
	ArrayCtrl       list;
	i32             droplines;
	i32             delimiter_char;
	i32           (*delimiter_filter)(i32 c);
	i32           (*compare_filter)(i32 c);
	bool            just_start;

	void            Cancel();
	void            Select();
	WTxt         ReadLast(i32& h);
	i32             IsDelimiter(i32 c);
	WTxt         CF(const WTxt& src);

	typedef SuggestCtrl CLASSNAME;

public:
	void ClearList()                                  { data.Clear(); }
	void AddList(const WTxt& s)                    { data.Add(s); }
	void AddList(const Txt& s)                     { data.Add(s.ToWTxt()); }
	void AddList(tukk s)                       { data.Add(s); }
	void FindAddList(const WTxt& s)                { if(FindIndex(data,s) < 0) data.Add(s); }
	void FindAddList(const Txt& s)                 { FindAddList(s.ToWTxt()); }
	void FindAddList(tukk s)                   { FindAddList(Txt(s)); }

	const Vec<WTxt>& GetList()                  { return data; }
	void Pick(Vec<WTxt>&& list)                 { data = pick(list); }

	SuggestCtrl& SetDropLines(i32 n)                  { droplines = n; return *this; }
	SuggestCtrl& Delimiter(i32 chr)                   { delimiter_char = chr; return *this; }
	SuggestCtrl& Delimiter(i32 (*filter)(i32 c))      { delimiter_filter = filter; return *this; }
	SuggestCtrl& CompareFilter(i32 (*filter)(i32 c))  { compare_filter = filter; return *this; }
	SuggestCtrl& JustStart(bool b = true)             { just_start = b; return *this; }

	SuggestCtrl();
};