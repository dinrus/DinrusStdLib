#ifndef _HexView_HexView_h
#define _HexView_HexView_h

#include <drx/CtrlLib/CtrlLib.h>

namespace drx {

#define LAYOUTFILE <drx/HexView/HexView.lay>
#include <drx/CtrlCore/lay.h>

class HexViewInfo : public FrameBottom<Ctrl> {
	virtual void Paint(Draw& w);

private:
	Font  font;
	z64 pos;
	i32   data[80];
	i32   mode;
	bool  longmode;
	bool  empty = false;

	void  PrintVal(Draw& w, i32 x, i32 y, i32 bytes, bool be);

public:
	void  SetEmpty()                   { empty = true; Refresh(); }
	void  SetPos(z64 p, bool lm)     { pos = p; longmode = lm; empty = false; Refresh(); }
	void  Set(i32 i, i32 d)            { ASSERT(i >= 0 && i < 80); data[i] = d; Refresh(); }
	void  SetMode(i32 m);
	i32   GetMode() const              { return mode; }

	HexViewInfo();
};

class HexView : public Ctrl {
public:
	virtual void Paint(Draw& w);
	virtual void Layout();
	virtual void LeftDown(Point p, dword);
	virtual bool Key(dword key, i32);
	virtual void MouseWheel(Point, i32 zdelta, dword);
	virtual void RightDown(Point, dword);

private:
	Font      font;
	i32       fcx3;
	Size      fsz;
	i32       fixed;
	i32       columns, rows, bytes;
	i32       sbm;
	zu64    sc = 0;
	zu64    cursor = 0;
	zu64    total = 0;
	zu64    start = 0;
	u8      charset;
	ScrollBar sb;

	HexViewInfo info;

	WithHexGotoLayout<TopWindow> go;

	void      SetSb();
	void      Scroll();
	void      SetCharset(i32 chr);
	void      SetColumns(i32 x);
	void      SetInfo(i32 m);
	void      Goto();
	void      RefreshInfo();

public:
	virtual i32 Byte(z64 adr);

	Event<Bar&>          WhenBar;
	Event<const Txt&> WhenGoto;
	Event<>              WhenGotoDlg;

	void   ColumnsMenu(Bar& bar);
	void   CharsetMenu(Bar& bar);
	void   InfoMenu(Bar& bar);
	void   StdMenu(Bar& bar);

	void   StdGoto(const Txt& text);

	void   GotoAddHistory()            { go.text.AddHistory(); }

	bool   IsLongMode() const          { return total > 0xffffffff; }
	void   SetStart(zu64 start);
	void   SetTotal(zu64 _total);
	void   SetSc(zu64 address);
	zu64 GetSc() const               { return sc; }
	void   SetCursor(zu64 address);
	zu64 GetCursor() const           { return cursor; }

	void   SerializeSettings(Stream& s);

	HexView& SetFont(Font fnt);
	HexView& Charset(u8 chrset)     { charset = chrset; Refresh(); return *this; }
	HexView& FixedColumns(i32 c = 0)  { fixed = c; Layout(); Refresh(); return *this; }
	HexView& InfoMode(i32 m = 1)      { info.SetMode(m); return *this; }

	typedef HexView CLASSNAME;

	HexView();
};

}

#endif
