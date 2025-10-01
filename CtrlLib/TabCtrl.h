class TabCtrl : public Ctrl {
public:
	virtual bool  Accept();
	virtual void  Paint(Draw& draw);
	virtual void  CancelMode();
	virtual void  MouseMove(Point p, dword keyflags);
	virtual void  LeftDown(Point p, dword keyflags);
	virtual void  MouseLeave();
	virtual bool  Key(dword key, i32 count);
	virtual bool  HotKey(dword key);
	virtual void  Layout();
	virtual Rect  GetOpaqueRect() const;
	virtual Val GetData() const;
	virtual void  SetData(const Val& data);

public:
	class Item {
		TabCtrl  *owner;

		i32       x;
		Point     pictpos;
		Point     textpos;
		i32       cx;

		Txt    text;
		PaintRect pict;
		Ptr<Ctrl> ctrl;
		bool      enabled;
		Ptr<Ctrl> slave;
		dword     key;

		friend class TabCtrl;

		void Layout(i32 x, i32 y, i32 cy);
		void Paint(Draw& w, i32 state);
		i32  Right() const { return x + cx; }

	public:
		Item&          Text(const Txt& _text);
		Item&          Picture(PaintRect d);
		Item&          SetImage(const DRX::Image& _im)  { return Picture(PaintRect(ImageDisplay(), _im)); }
		Item&          SetCtrl(Ctrl *_ctrl);
		Item&          SetCtrl(Ctrl& c)                 { return SetCtrl(&c); }
		Item&          Slave(Ctrl *_slave);
		Item&          Key(dword _key)                  { key = _key; return *this; }

		Item&          Enable(bool _en = true);
		Item&          Disable()                        { return Enable(false); }
		bool           IsEnabled() const                { return enabled; }
		Ctrl          *GetSlave()                       { return slave; }
		const Ctrl    *GetSlave() const                 { return slave; }
		Ctrl          *GetCtrl()                        { return ctrl; }
		const Ctrl    *GetCtrl() const                  { return ctrl; }
		Txt         GetText() const                  { return text; }
		PaintRect      GetPicture() const               { return pict; }

		Item();

	//deprecated:
		Item&          Control(Ctrl *c)                 { return SetCtrl(c); }
		Item&          Image(const DRX::Image& m)       { return SetImage(m); }
	};

	struct Style : ChStyle<Style> {
		i32 tabheight, margin, extendleft;
		Rect sel, edge;
		Val normal[4], first[4], last[4], both[4], body;
		Color text_color[4];
		Font font;
	};

private:
	struct Tabs : public Ctrl {
		virtual void Paint(Draw& w);
	};

	Array<Item> tab;
	i32         x0;
	i32         hot;
	i32         sel;
	Tabs        tabs;
	Button      left, right;
	ParentCtrl  pane;
	bool        accept_current, no_accept;

	const Style *style;

	static Image Fade(i32 i);

	void       PaintTabs(Draw& w);
	void       ScrollInto(i32 i);
	void       Left();
	void       Right();
	void       SyncHot();
	void       SyncTabs();
	i32        TabsRight();
	void       Go(i32 d);
	i32        FindInsert(Ctrl& slave);

public:
	Event<>  WhenSet;

	TabCtrl::Item& Add();
	TabCtrl::Item& Add(tukk text);
	TabCtrl::Item& Add(const Image& m, tukk text);
	TabCtrl::Item& Add(Ctrl& slave, tukk text);
	TabCtrl::Item& Add(Ctrl& slave, const Image& m, tukk text);

	TabCtrl::Item& Insert(i32 i);
	TabCtrl::Item& Insert(i32 i, tukk text);
	TabCtrl::Item& Insert(i32 i, const Image& m, tukk text);
	TabCtrl::Item& Insert(i32 i, Ctrl& slave, tukk text);
	TabCtrl::Item& Insert(i32 i, Ctrl& slave, const Image& m, tukk text);

	void  Remove(i32 i);

	i32   GetTab(Point p) const;

	i32   GetCount() const                       { return tab.GetCount(); }
	Item& GetItem(i32 i)                         { return tab[i]; }
	const Item& GetItem(i32 i) const             { return tab[i]; }

	void Set(i32 i);
	i32  Get() const                             { return sel; }

	i32  Find(const Ctrl& slave) const;
	void Set(Ctrl& slave);
	bool IsAt(Ctrl& slave)                       { return Get() == Find(slave); }

	TabCtrl::Item& Insert(Ctrl& before_slave);
	TabCtrl::Item& Insert(Ctrl& before_slave, tukk text);
	TabCtrl::Item& Insert(Ctrl& before_slave, const Image& m, tukk text);
	TabCtrl::Item& Insert(Ctrl& before_slave, Ctrl& slave, tukk text);
	TabCtrl::Item& Insert(Ctrl& before_slave, Ctrl& slave, const Image& m, tukk text);

	void  Remove(Ctrl& slave);

	void GoNext()                                { Go(1); }
	void GoPrev()                                { Go(-1); }

	Size     ComputeSize(Size pane);
	Size     ComputeSize();
	void     Add(Ctrl& c)                        { pane.Add(c.SizePos()); }
//	TabCtrl& operator<<(Ctrl& c)                 { Add(c); return *this; } // ambiguos with operator<<(lambda)

	static const Style& StyleDefault();

	TabCtrl& NoAccept(bool ac = true)            { no_accept = ac; return *this; }
	TabCtrl& AcceptCurrent(bool ac = true)       { accept_current = ac; return *this; }
	TabCtrl& AcceptAll()                         { return AcceptCurrent(false); }
	TabCtrl& SetStyle(const Style& s)            { style = &s; Refresh(); return *this; }

	void Reset();

	typedef TabCtrl CLASSNAME;

	TabCtrl();
};

class TabDlg : public TopWindow {
	void    Rearrange();

	bool    binit;
	Size    sz;

	void           PlaceButton(Button& b, i32& r);
	TabCtrl::Item& Add0(Ctrl& tab, tukk text);
	TabDlg&        AButton(Button& b);

public:
	TabCtrl  tabctrl;
	Button   ok;
	Button   cancel;
	Button   exit;
	Button   apply;

	template <class T>
	TabCtrl::Item& Add(T& tab, tukk text)                   { CtrlLayout(tab); return Add0(tab, text); }
	template <class T>
	TabCtrl::Item& Add(T& tab, const Image& img, tukk name) { return Add(tab, name).Image(img); }
	template <class T>
	TabDlg&  operator()(T& tab, tukk text)                  { Add(tab, text); return *this; }
	template <class T>
	TabDlg&  operator()(T& tab, const Image& img, tukk txt) { Add(tab, img, txt); return *this; }

	TabDlg&  OK()                                                  { return AButton(ok); }
	TabDlg&  Cancel()                                              { return AButton(cancel); }
	TabDlg&  OKCancel()                                            { return OK().Cancel(); }
	TabDlg&  Exit()                                                { return AButton(exit); }
	TabDlg&  Apply()                                               { return AButton(apply); }

	TabDlg();
};
