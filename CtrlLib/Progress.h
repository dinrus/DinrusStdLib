class ProgressIndicator : public Ctrl {
public:
	virtual void   Paint(Draw& draw);
	virtual void   Layout();

public:
	struct Style : ChStyle<Style> {
		bool  classic;
		bool  bound;
		bool  nomargins;
		Val vlook, vchunk, hlook, hchunk;
	};

protected:
	i32   total, actual;
	i32   pxp;
	bool  percent:1;
	const Style *style;
	Color color;

	Rect GetMargins();
	Size GetMsz();

public:
	void  Set(i32 actual, i32 total);
	void  Set(i32 _actual)                      { Set(_actual, total); }

	void  operator=(i32 i)                      { Set(i); }
	i32   operator++()                          { Set(actual + 1); return actual; }
	i32   operator++(i32)                       { i32 i = actual; Set(actual + 1); return i; }
	i32   operator+=(i32 i)                     { Set(actual + i); return actual; }

	i32   Get() const                           { return actual; }
	i32   GetTotal() const                      { return total; }

	operator i32()                              { return actual; }

	static const Style& StyleDefault();

	ProgressIndicator& SetTotal(i32 _total)     { Set(actual, _total); return *this; }
	ProgressIndicator& Percent(bool b = true)   { percent = b; Refresh(); return *this; }
	ProgressIndicator& NoPercent()              { return Percent(false); }
	ProgressIndicator& SetColor(Color c)        { color = c; Refresh(); return *this; }
	ProgressIndicator& SetStyle(const Style& s) { style = &s; Refresh(); return *this; }

	ProgressIndicator();
	virtual ~ProgressIndicator();
};

class Progress : public WithProgressLayout<TopWindow> {
public:
	ProgressIndicator pi;
	virtual void Close();
	virtual void Layout();

protected:
	bool     cancel;
	i32      total;
	i32      pos;
	Txt      text;
	i32      granularity;
	i32      show_delay;
	dword    set_time;
	dword    show_time;
	Ctrl*    owner;
	Modality modality;
	Mutex    mtx;

	void     Setxt0();
	void     Setxt();
	void     Init();
	void     Process();

public:
	void     SetOwner(Ctrl* _owner)        { owner = _owner; }

	void     Create();
	void     Cancel();

	void     Set(i32 pos, i32 total);
	void     SetPos(i32 pos);
	void     SetText(tukk s)        { SetText((Txt)s); }
	void     SetText(const Txt& s);
    void     UpdateText(const Txt& s);
	void     SetTotal(i32 total);
	void     Step(i32 steps = 1);
	i32      GetPos() const                { return pos; }
	i32      GetTotal() const              { return total; }
	Txt      GetText() const               { return text; }

	bool     Canceled();
	bool     SetCanceled(i32 pos, i32 total);
	bool     SetPosCanceled(i32 pos);
	bool     StepCanceled(i32 steps = 1);

	void     Reset();

	void     Granularity(i32 ms)           { granularity = ms; }
	void     Delay(i32 ms)                 { show_delay = ms; }

	void     AlignText(i32 align)          { info.SetAlign(align); }

	operator Gate<i32, i32>()             { return callback(this, &Progress::SetCanceled); }

	Progress();
	Progress(Ctrl* _owner);
	Progress(tukk txt, i32 total = 0);
	Progress(Ctrl* _owner, tukk txt, i32 total = 0);
	virtual ~Progress();
};
