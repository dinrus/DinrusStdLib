#include "CtrlLib.h"

#ifndef CtrlCore_MenuImp_h
#define CtrlCore_MenuImp_h

namespace drx {

class MenuItemBase : public Ctrl, public Bar::Item {
public:
	virtual Bar::Item& Text(tukk text);
	virtual Bar::Item& Key(dword key);
	virtual Bar::Item& Image(const DRX::Image& img);
	virtual Bar::Item& Enable(bool _enable);
	virtual Bar::Item& Tip(tukk tip);
	virtual Bar::Item& Help(tukk help);
	virtual Bar::Item& Topic(tukk help);
	virtual Bar::Item& Description(tukk desc);
	virtual Bar::Item& Check(bool check);
	virtual Bar::Item& Radio(bool check);
	virtual Bar::Item& Bold(bool bold);

	virtual Txt GetDesc() const;
	virtual dword  GetAccessKeys() const;
	virtual void   AssignAccessKeys(dword used);

	using   Ctrl::Key;

protected:
	enum {
		NOTHING, CHECK0, CHECK1, RADIO0, RADIO1
	};

	enum {
		NORMAL, HIGHLIGHT, PUSH
	};

	Txt  text;
	dword   accel;
	i32     state;
	i32     leftgap, textgap;
	Font    font;
	bool    isenabled;
	u8    type;
	i32     accesskey;
	Size    maxiconsize;
	const MenuBar::Style *style;
	bool    nodarkadjust;

	MenuBar *GetMenuBar() const;

public:
	virtual void SyncState() = 0;

	void           DrawMenuText(Draw& w, i32 x, i32 y, const Txt& s, Font f, bool enabled, bool hl,
	                            Color color, Color hlcolor);
	void           PaintTopItem(Draw& w, i32 state);

	bool           IsItemEnabled() const          { return isenabled; }
	Txt         GetText() const                { return text; }
	MenuItemBase&  LeftGap(i32 cx)                { leftgap = cx; return *this; }
	MenuItemBase&  TextGap(i32 cx)                { textgap = cx; return *this; }
	MenuItemBase&  SetFont(Font f)                { font = f; return *this; }
	MenuItemBase&  Style(const MenuBar::Style *s) { style = s; return *this; }
	Font           GetFont() const                { return font; }
	MenuItemBase&  MaxIconSize(Size sz)           { maxiconsize = sz; return *this; } // deprecated
	bool           InOpaqueBar() const;
	MenuItemBase&  NoDarkAdjust(bool b = true)    { nodarkadjust = b; return *this; }

	MenuItemBase();
};

class MenuItem : public MenuItemBase {
public:
	virtual void  Paint(Draw& w);
	virtual void  MouseEnter(Point, dword);
	virtual void  MouseLeave();
	virtual Size  GetMinSize() const;
	virtual void  LeftUp(Point, dword);
	virtual void  RightUp(Point, dword);
	virtual void  GotFocus();
	virtual void  LostFocus();
	virtual bool  Key(dword key, i32 count);
	virtual bool  HotKey(dword key);
	virtual void  SyncState();

	virtual Bar::Item& Image(const DRX::Image& img);

private:
	DRX::Image licon, ricon;

	void  SendHelpLine();
	void  ClearHelpLine();
	using MenuItemBase::Key;

protected:
	virtual i32  GetVisualState();

public:
	MenuItem& RightImage(const DRX::Image& img);
};

class SubMenuBase {
protected:
	MenuBar  menu;
	Event<Bar&> proc;
	MenuBar *parentmenu;

	void     Pull(Ctrl *item, Point p, Size sz);

public:
	virtual  void Pull() = 0;

	void SetParent(MenuBar *m)                           { parentmenu = m; menu.MaxIconSize(m->GetMaxIconSize()); }
	void Set(Event<Bar&> _submenu)                   { proc = _submenu; }
	Event<Bar&> Get()                                { return proc; }

	SubMenuBase()                                        { parentmenu = NULL; }
	virtual ~SubMenuBase() {}
};

class SubMenuItem : public MenuItem, public SubMenuBase {
public:
	virtual void MouseEnter(Point, dword);
	virtual void MouseLeave();
	virtual void GotFocus();
	virtual bool HotKey(dword key);
	virtual bool Key(dword key, i32 count);
	virtual i32  GetVisualState();
	virtual void Pull();

protected:
	enum {
		TIMEID_PULL = BarCtrl::TIMEID_COUNT,
		TIMEID_COUNT
	};

public:
	typedef SubMenuItem CLASSNAME;

	SubMenuItem();
};

class TopSubMenuItem : public MenuItemBase, public SubMenuBase {
public:
	virtual void Paint(Draw& w);
	virtual void MouseEnter(Point, dword);
	virtual void MouseLeave();
	virtual void GotFocus();
	virtual void LostFocus();
	virtual void LeftDown(Point, dword);
	virtual void SyncState();
	virtual Size GetMinSize() const;
	virtual bool Key(dword key, i32);
	virtual bool HotKey(dword key);
	virtual void Pull();

private:
	i32  GetState();
	using MenuItemBase::Key;

public:
	TopSubMenuItem() { NoInitFocus(); }
};

class TopMenuItem : public MenuItemBase {
public:
	virtual void  Paint(Draw& w);
	virtual void  MouseEnter(Point, dword);
	virtual void  MouseLeave();
	virtual void  LeftUp(Point, dword);
	virtual void  LeftDown(Point, dword);
	virtual void  GotFocus();
	virtual void  LostFocus();
	virtual bool  Key(dword key, i32 count);
	virtual Size  GetMinSize() const;
	virtual void  SyncState();

	static i32 GetStdHeight(Font font = StdFont());

private:
	i32  GetState();
	using MenuItemBase::Key;

public:
	TopMenuItem() { NoInitFocus(); }
};

}

#endif
