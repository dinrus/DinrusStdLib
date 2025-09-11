class InfoCtrl : public FrameLR<Ctrl> {
public:
	virtual void Paint(Draw& w);
	virtual void FrameLayout(Rect& r);

private:
	struct Tab {
		PaintRect             info;
		i32                   width;

		Tab() { width = 0; }
	};

	Array<Tab>   tab;
	PaintRect    temp;
	bool         right;
	Txt       defaulttext;
	TimeCallback temptime;

public:
	void Set(i32 tab, const PaintRect& info, i32 width);
	void Set(i32 tab, const Val& info, i32 width);
	void Set(const PaintRect& info);
	void Set(const Val& info);
	void Temporary(const PaintRect& info, i32 timeoout = 2000);
	void Temporary(const Val& info, i32 timeout = 2000);
	void EndTemporary();

	i32  GetTabCount() const              { return tab.GetCount(); }
	i32  GetTabOffset(i32 t) const;
	i32  GetRealTabWidth(i32 tabi, i32 width) const;

	void operator=(const Txt& s)       { Set(s); }

	InfoCtrl& SetDefault(const Txt& d) { defaulttext = d; Refresh(); return *this; }
	InfoCtrl& Left(i32 w);
	InfoCtrl& Right(i32 w);

	typedef InfoCtrl CLASSNAME;

	InfoCtrl();
};

class StatusBar : public InfoCtrl {
public:
	virtual void FrameLayout(Rect& r);
	virtual void FrameAddSize(Size& sz);
	virtual void Paint(Draw& w);

public:
	struct Style : public ChStyle<Style> {
		Val look;
	};

private:
	i32      cy;
	SizeGrip grip;
	
	struct TopFrame : public CtrlFrame {
		virtual void FrameLayout(Rect& r);
		virtual void FramePaint(Draw& draw, const Rect& r);
		virtual void FrameAddSize(Size& sz);
		
		const Style *style;
	};
	
	TopFrame frame;

	void    SetText(const Txt& s)          { Set(s); }

public:
	void operator=(const Txt& s)           { Set(s); }

	operator Event<const Txt&>()       { return pteback(this, &StatusBar::SetText); }

	Event<const Txt&> operator~()      { return pteback(this, &StatusBar::SetText); }

	StatusBar&  Height(i32 _cy);
	StatusBar&  NoSizeGrip()                  { RemoveFrame(grip); return *this; }

	static const Style& StyleDefault();

	InfoCtrl& SetStyle(const Style& s)        { frame.style = &s; Refresh(); return *this; }

	StatusBar();
	~StatusBar();
};


Display& ProgressDisplay();

class ProgressInfo {
	InfoCtrl *info;
	Txt    text;
	i32       tw;
	i32       tabi;
	i32       cx;
	i32       total;
	i32       pos;
	
	i32       granularity;
	dword     set_time;

	void Refresh();
	void Reset();

public:
	ProgressInfo& Text(const Txt& s)         { text = s; Refresh(); return *this; }
	ProgressInfo& TextWidth(i32 cx)             { tw = cx; Refresh(); return *this; }
	ProgressInfo& Width(i32 _cx)                { cx = _cx; Refresh(); return *this;  }
	ProgressInfo& Placement(i32 _tabi)          { tabi = _tabi; Refresh(); return *this; }
	ProgressInfo& Info(InfoCtrl& _info)         { info = &_info; info->EndTemporary(); Refresh(); return *this; }
	ProgressInfo& Total(i32 _total)             { total = _total; Refresh(); return *this; }

	ProgressInfo& Set(i32 _pos, i32 _total);

	void          Set(i32 _pos)                 { Set(_pos, total); }
	i32           Get() const                   { return pos; }
	i32           GetTotal() const              { return total; }

	void operator=(i32 p)                       { Set(p); }
	void operator++()                           { Set(pos + 1, total); }
	operator i32()                              { return pos; }

	ProgressInfo()                              { Reset(); }
	ProgressInfo(InfoCtrl& f)                   { Reset(); Info(f); }
	~ProgressInfo();
};
