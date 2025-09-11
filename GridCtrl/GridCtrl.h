#ifndef _GridCtrl_GridCtrl_h_
#define _GridCtrl_GridCtrl_h_
#include <drx/CtrlLib/CtrlLib.h>
#ifdef flagGRIDSQL
#include <drx/Sql/Sql.h>
#endif

namespace drx {

#include "GridUtils.h"
#include "GridDisplay.h"

#define FOREACH_ROW(x) for(x.Begin(); x.IsEnd(); x.Next())
#define FOREACH_SELECTED_ROW(x) FOREACH_ROW(x) if(x.IsSelected())
#define FOREACH_MODIFIED_ROW(x) FOREACH_ROW(x) if(x.IsUpdatedRow())
#define FOREACH_ROW_NOT_CURRENT(x) FOREACH_ROW(x) if(!x.IsCurrentRow())

namespace GF
{
    enum {
        SKIP_CURRENT_ROW = BIT(0),
        SKIP_HIDDEN = BIT(1)
    };
};

class GridFind : public EditTxt
{
    private:
        MultiButtonFrame button;
    public:

        GridFind();

        Event<> WhenEnter;
        Callback1<Bar &> WhenBar;

        virtual bool Key(dword key, i32 count);
        virtual Size GetMinSize() const;
        void Push();

        typedef GridFind CLASSNAME;

};

class GridPopUpHeader : public Ctrl
{
    private:
        bool open;
    public:

        GridDisplay *display;
        Val val;
        i32 sortmode;
        i32 sortcol;
        i32 sortcnt;
        bool chameleon;

        GridPopUpHeader() : open(false) {}

        virtual void Paint(Draw &w);
        void PopUp(Ctrl *owner, i32 x, i32 y, i32 width, i32 height);
        virtual void Close();
};

class GridButton : public Ctrl
{
    private:
        i32 img;
        i32 n;

    public:
        typedef GridButton CLASSNAME;
        GridButton();
        virtual void Paint(Draw& w);
        virtual void LeftDown(Point p, dword flags);
        virtual void LeftUp(Point p, dword flags);
        virtual void MouseEnter(Point p, dword flags);
        virtual void MouseLeave();
        virtual void State(i32 reason);
        virtual Size GetStdSize() const;
        void SetButton(i32 n);
};

class GridResizePanel : public FrameBottom<Ctrl>
{
    private:
        GridButton close;
        Point pos;
        Rect r;
        Size minsize;
    public:

        typedef GridResizePanel CLASSNAME;
        GridResizePanel();
        virtual void Paint(Draw& w);
        virtual Image CursorImage(Point p, dword flags);
        virtual void LeftDown(Point p, dword flags);
        virtual void LeftUp(Point p, dword flags);
        virtual void MouseMove(Point p, dword flags);
        bool MouseOnGrip(const Point &p);
        void SetMinSize(Size sz);

        Event<> WhenClose;
};

class GridPopUp : public Ctrl
{
    public:

        bool open;
        Val val;
        Ctrl* ctrl;
        GridDisplay* gd;
        Color fg, bg;
        Font fnt;
        dword style;

        GridPopUp() : open(false) {}
        Point Offset(Point p);

        virtual void  Paint(Draw &w);
        virtual void  LeftDown(Point p, dword flags);
        virtual void  LeftDrag(Point p, dword flags);
        virtual void  LeftDouble(Point p, dword flags);
        virtual void  RightDown(Point p, dword flags);
        virtual void  LeftUp(Point p, dword flags);
        virtual void  MouseWheel(Point p, i32 zdelta, dword flags);
        virtual void  MouseLeave();
        virtual void  MouseEnter(Point p, dword flags);
        virtual void  MouseMove(Point p, dword flags);
        virtual Image CursorImage(Point p, dword flags);
        virtual void  LostFocus();
        void PopUp(Ctrl *owner, i32 x, i32 y, i32 width, i32 height);
        virtual void Close();
};

class GridOperation
{
    public:

        enum
        {
            INSERT,
            UPDATE,
            REMOVE,
            NONE
        };

    private:

        i32 operation;
        i32 version;

    public:

        GridOperation() : operation(NONE), version(1) {}

        void SetOperation(i32 op)
        {
            switch(operation)
            {
                case NONE: operation = op; ++version; break;
                case UPDATE:
                    if(op == REMOVE || op == NONE)
                        Clear();
                    break;
                case INSERT:
                    if(op == REMOVE || op == NONE)
                        Clear();
                    break;
            }
        }

        void Clear()        { operation = NONE; }
        void ClearVersion() { version = 0;      }
        i32  GetVersion()   { return version;   }

        GridOperation& operator=(i32k op)
        {
            SetOperation(op);
            return *this;
        }

        operator i32()          { return operation;       }
        bool operator!=(i32 op) { return operation != op; }
        bool operator==(i32 op) { return operation == op; }
};


class CtrlsHolder : public Ctrl
{
    private:
        Ctrl &parent;
        Point offset;
    public:
        CtrlsHolder(Ctrl &p) : parent(p)
        {
            Transparent();
            WantFocus(false);
        }
        virtual void  LeftUp(Point p, dword flags)      { parent.LeftUp(p + offset, flags);      }
        virtual void  LeftDown(Point p, dword flags)    { parent.LeftDown(p + offset, flags);    }
        virtual void  LeftDouble(Point p, dword flags)  { parent.LeftDouble(p + offset, flags);  }
        virtual void  RightUp(Point p, dword flags)     { parent.RightUp(p + offset, flags);     }
        virtual void  RightDown(Point p, dword flags)   { parent.RightDown(p + offset, flags);   }
        virtual void  RightDouble(Point p, dword flags) { parent.RightDouble(p + offset, flags); }
        virtual void  MouseMove(Point p, dword flags)   { parent.MouseMove(p + offset, flags);   }
        virtual void  MouseLeave()                      { parent.MouseLeave();                   }
        virtual Image CursorImage(Point p, dword flags) { return parent.CursorImage(p + offset, flags); }

        virtual void ChildMouseEvent(Ctrl *child, i32 event, Point p, i32 zdelta, dword keyflags)
        {
            Ctrl::ChildMouseEvent(child, event, p, zdelta, keyflags);
        }
        virtual void  Paint(Draw &w)
        {
            //DrawFrame(w, GetSize(), Red);
        }

        virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags)
        {
            parent.MouseWheel(p + offset, zdelta, keyflags);
        }

        void SetOffset(Point p)
        {
            offset = p;
        }
};

class GridClipboard : Moveable<GridClipboard>
{
    public:
        rval_default(GridClipboard);
        GridClipboard() {}

        struct ClipboardData : Moveable<ClipboardData>
        {
            i32 col, row;
            Val v;

            void SerializeAttrText(Stream& s, AttrText& a)
            {
                s % a.text;
                s % a.font;
                s % a.ink;
                s % a.normalink;
                s % a.paper;
                s % a.align;
                s % a.img;
                s % a.imgspc;
            }

            void Serialize(Stream &s)
            {
                s % col % row;
                dword type = IsType<AttrText>(v) ? 1 : 0;
                s / type;
                if(type == 1)
                {
                    if(s.IsStoring())
                    {
                        SerializeAttrText(s, (AttrText&) ValTo<AttrText>(v));
                    }
                    else
                    {
                        AttrText a;
                        SerializeAttrText(s, a);
                        v = RawToVal<AttrText>(a);
                    }
                }
                else
                    s % v;
            }
        };

        Vec<ClipboardData> data;
        Point minpos, maxpos;
        bool shiftmode;

        virtual void Serialize(Stream &s)
        {
            s % shiftmode;
            s % minpos % maxpos;
            s % data;
        }
};

class GridCtrl : public Ctrl
{
    friend class DropGrid;

    public:

        i32 GD_COL_WIDTH;
        i32 GD_ROW_HEIGHT;
        i32 GD_HDR_HEIGHT;
        i32 GD_IND_WIDTH;

        enum GridOrder
        {
            SORT_ID   = 0,
            SORT_UP   = 1,
            SORT_DOWN = 2
        };

    private:

        enum GridCursor
        {
            GO_PREV,
            GO_NEXT,
            GO_LEFT,
            GO_RIGHT,
            GO_BEGIN,
            GO_END,
            GO_PAGEUP,
            GO_PAGEDN
        };

        enum GridControlState
        {
            UC_SHOW      = BIT(0),
            UC_HIDE      = BIT(1),
            UC_FOCUS     = BIT(2),
            UC_NOFOCUS   = BIT(3),
            UC_GOFIRST   = BIT(4),
            UC_SCROLL    = BIT(5),
            UC_NEXT      = BIT(6),
            UC_CTRLS     = BIT(7),
            UC_CURSOR    = BIT(8),
            UC_CHECK_VIS = BIT(9),
            UC_MOUSE     = BIT(10),
            UC_CTRLS_OFF = BIT(11),
            UC_UPDATES   = BIT(12),
            UC_OLDCUR    = BIT(13)
        };

        enum GridRepaintOpts
        {
            RP_TOOLBAR   = BIT(0),
            RP_UPDCTRLS  = BIT(1),
            RP_ALL       = BIT_ALL
        };

        enum GridCursorState
        {
            CU_MOUSE     = BIT(0),
            CU_HIGHLIGHT = BIT(1),
            CU_CTRLMODE  = BIT(2),
            CU_HIDECTRLS = BIT(3)
        };

        enum GridSummaryOperation
        {
            SOP_NONE = 0,
            SOP_MIN = 1,
            SOP_MAX = 2,
            SOP_SUM = 3,
            SOP_CNT = 4,
            SOP_AVG = 5,
        };

        enum GridState
        {
            GS_UP     = 0,
            GS_MOVE   = 1,
            GS_DOWN   = 2,
            GS_BORDER = 3,
            GS_POPUP  = 4
        };

        enum GridEdit
        {
            GE_ROW  = 0,
            GE_CELL = 1
        };

        enum GridItemCtrl
        {
            IC_INIT = BIT(1),
            IC_MANUAL = BIT(2),
            IC_FACTORY = BIT(3),
            IC_OWNED = BIT(4)
        };

        struct CurState
        {
            bool newx;
            bool newy;
            bool accepted;
            bool valid;

            CurState() { Clear(); }
            bool IsNewRow()   { return newy;         }
            bool IsNewCol()   { return newx;         }
            bool IsNew()      { return newx || newy; }
            bool IsAccepted() { return accepted;     }
            bool IsValid()    { return valid;        }

            void Clear()
            {
                newx = newy = false;
                accepted = false;
                valid = true;
            }

            operator bool() { return IsNew() && IsAccepted(); }
        };

        struct Edit : Moveable<Edit>
        {
            Ptr<Ctrl> ctrl;
            Convert * convert;
            Callback1<One<Ctrl>&> factory;

            Edit()
            {
                ctrl = NULL;
                convert = NULL;
            }
        };

        class Item : Moveable<Item>
        {
            public:
                friend class ItemRect;
                friend class GridCtrl;

                Item()
                {
                    ctrl    = NULL;
                    convert = NULL;
                    display = NULL;

                    fs = fe = 0;
                    style = 0;
                    editable = true;
                    clickable = true;
                    idx = 0;
                    idy = 0;
                    cx = 0;
                    cy = 0;
                    group = -1;
                    isjoined = false;

                    rcx = 0;
                    rcy = 0;

                    modified = false;
                    sync_flag = -1;
                    paint_flag = -1;
                    ctrl_flag = 0;
                }
                ~Item()
                {
                    if(ctrl_flag & IC_OWNED)
                        delete ctrl;
                }

                void SetCtrl(Ctrl& ctrl, bool owned);
                void ClearCtrl();

                void SetDisplay(GridDisplay& display);
                void NoDisplay();

            private:

                bool InvertSelect() { return BitInverse(style, GD::SELECT); }
                bool IsSelect() { return style & GD::SELECT; }
                bool IsLive()   { return style & GD::LIVE;   }
                bool IsCursor() { return style & GD::CURSOR; }
                bool IsFound()  { return style & GD::FOUND;  }

                void Select(bool s) { BitSet(style, GD::SELECT, s); }
                void Cursor(bool s) { BitSet(style, GD::CURSOR, s); }
                void Live(bool s)   { BitSet(style, GD::LIVE, s);   }
                void Found(bool s)  { BitSet(style, GD::FOUND, s);  }

                bool IsJoined() { return isjoined; }

                i32 idx, idy, cx, cy, group;
                i32 rcx, rcy;
                bool isjoined:1;
                i32 fs, fe;
                dword style;
                bool editable:1;
                bool clickable:1;
                bool modified:1;
                i32 sync_flag;
                i32 paint_flag;
                i32 ctrl_flag;

                Ptr<Ctrl> ctrl;

                Convert     *convert;
                GridDisplay *display;

            public:
                Val val;

                Item& Editable(bool b);
                Item& NoEditable();

                Item& Clickable(bool b)  { clickable = b;     return *this; }
                Item& NoClickable()      { clickable = false; return *this; }
        };

        typedef Vec< Vec<Item> > Items;
        typedef Vec< Edit > Edits;

        struct JoinRect : Moveable<JoinRect>
        {
            Rect r;
            i32 group;
            i32 idx, idy;
        };

        public: class ItemRect : public Moveable<ItemRect>
        {
            friend class GridCtrl;

            public:
                ItemRect()
                {
                    min = 5; max = 1000000;
                    pos = npos = 0;
                    id = uid = n = 0;
                    prop = 1;
                    size = nsize = 0;
                    tsize = 0;
                    sortmode = 0;
                    hidden = false;
                    index = false;
                    convertion = true;
                    editable = true;
                    edit_insert = true;
                    edit_update = true;
                    sortable = true;
                    clickable = true;
                    locked = false;
                    skip = false;
                    ignore_display = false;
                    style = 0;

                    calign = 0;
                    sl = sr = st = sb = 0;
                    sx = sy = 0;
                    join = 0;

                    ismin = false;
                    ismax = false;

                    fnt = Null;
                    hfnt = Null;
                    balign = halign = GD::LEFT | GD::VCENTER;
                    wrap = false;
                    fg = Null;
                    bg = Null;

                    ctrl    = NULL;
                    convert = NULL;
                    display = NULL;

                    found = false;

                    level = 0;
                    sop = SOP_NONE;

                    operation = GridOperation::NONE;
                }

            private:

                Items *items;
                GridCtrl *parent;
                Edits *edits;

                i32 level;

                Ptr<Ctrl>   ctrl;
                Convert     *convert;
                GridDisplay *display;

                Callback1<One<Ctrl>&> factory;

                static VecMap<Id, i32> *aliases;

                double pos, size, prop;
                i32 npos, nsize;
                i32 min, max;
                i32 id, uid, n;
                i32 sl, sr, st, sb, sx, sy;

                double tsize;
                i32  join;

                i32 sop;
                Txt sopfrm;

                bool hidden;
                bool index;
                bool convertion;
                bool editable;
                bool sortable;
                bool clickable;
                bool locked;
                bool found:1;
                bool skip:1;
                bool edit_insert:1;
                bool edit_update:1;

                Val defval;

                dword style;

                bool ismin;
                bool ismax;

                /* operatory typu friend moga odwolywac sie tylko do zmiennych statycznych klasy */

                static i32 sortCol;
                static i32 sortMode;

                i32 sortcol;
                i32 sortmode;

                i32 balign, halign, calign;
                bool wrap;
                Font fnt, hfnt;
                Color fg, bg;
                Image img;

                GridOperation operation;

                bool ignore_display:1;

                double Left(i32 scroll = 0)    { return pos - scroll;          }
                double Right(i32 scroll = 0)   { return pos + size - scroll;   }
                double Top(i32 scroll = 0)     { return pos - scroll;          }
                double Bottom(i32 scroll = 0)  { return pos + size - scroll;   }
                double Width()                 { return size;                  }
                double Height()                { return size;                  }

                bool   InvertSelect()          { return BitInverse(style, GD::SELECT); }
                bool   IsSelect()              { return style & GD::SELECT;    }
                bool   IsLive()                { return style & GD::LIVE;      }
                bool   IsCursor()              { return style & GD::CURSOR;    }
                bool   IsHighlight()           { return style & GD::HIGHLIGHT; }

                bool   IsFound()               { return found;                 }

                void   Select(bool s)          { BitSet(style, GD::SELECT, s);    }
                void   Cursor(bool s)          { BitSet(style, GD::CURSOR, s);    }
                void   Live(bool s)            { BitSet(style, GD::LIVE, s);      }
                void   Highlight(bool s)       { BitSet(style, GD::HIGHLIGHT, s); }

                i32    nLeft(i32 scroll = 0)   { return npos - scroll;         }
                i32    nRight(i32 scroll = 0)  { return npos + nsize - scroll; }
                i32    nTop(i32 scroll = 0)    { return npos - scroll;         }
                i32    nBottom(i32 scroll = 0) { return npos + nsize - scroll; }
                i32    nWidth()                { return nsize;                 }
                i32    nHeight()               { return nsize;                 }

                ItemRect& Size(i32 n, bool hv = false);

                bool IsMin()   { return ismin; }
                bool IsMax()   { return ismax; }
                bool IsFixed() { return ismin && ismax; }

                void ChangeSortMode(bool idsort = true);

                friend bool operator<(const ItemRect& a, const ItemRect& b)
                {
                    if(sortMode)
                        return (StdValCompare(a.ExtractVal(a.id, sortCol), b.ExtractVal(b.id, sortCol), 0) < 0);
                    else
                        return a.id < b.id;
                }
                friend bool operator>(const ItemRect& a, const ItemRect& b)
                {
                    if(sortMode)
                        return (StdValCompare(a.ExtractVal(a.id, sortCol), b.ExtractVal(b.id, sortCol), 0) > 0);
                    else
                        return a.id > b.id;
                }

                Callback1<i32> SyncCtrls;

            public:

                ItemRect& Alias(Id id);
                ItemRect& Alias(tukk  s);
                ItemRect& Name(Txt &s);
                ItemRect& Name(tukk  s);
                ItemRect& Hidden(bool b = true);
                ItemRect& Width(i32 n);
                ItemRect& Height(i32 n);
                ItemRect& Min(i32 n);
                ItemRect& Max(i32 n);
                ItemRect& Fixed(i32 n);
                ItemRect& FixedAuto();
                ItemRect& Edit(Ctrl &ctrl, bool b = true);
                template<typename T>
                ItemRect& EditConvert(T &ctrl, bool b = true)    { return Edit(ctrl, b).SetConvert(ctrl); }
                template<typename T>
                ItemRect& EditConvertDisplay(T &ctrl)            { return Edit(ctrl).SetConvert(ctrl).SetDisplay(ctrl); }
                ItemRect& EditInsert(bool b = true);
                ItemRect& EditUpdate(bool b = true);
                ItemRect& SetConvert(Convert &c);
                ItemRect& NoConvert();
                ItemRect& SetFormat(tukk fmt);
                ItemRect& SetDisplay(GridDisplay &gd);
                ItemRect& IgnoreDisplay();
                ItemRect& NoConvertion();
                ItemRect& Default(Val v);
                ItemRect& Index(bool b = true);

                Txt GetName() const;
                Id     GetAlias() const;

                ItemRect& AlignTop()                             { balign = GD::TOP     | GD::LEFT;                return *this; }
                ItemRect& AlignBottom()                          { balign = GD::BOTTOM  | GD::LEFT;                return *this; }
                ItemRect& AlignLeft()                            { balign = GD::LEFT    | GD::VCENTER;             return *this; }
                ItemRect& AlignRight()                           { balign = GD::RIGHT   | GD::VCENTER;             return *this; }
                ItemRect& AlignTopLeft()                         { balign = GD::LEFT    | GD::TOP;                 return *this; }
                ItemRect& AlignTopRight()                        { balign = GD::RIGHT   | GD::TOP;                 return *this; }
                ItemRect& AlignTopCenter()                       { balign = GD::HCENTER | GD::TOP;                 return *this; }
                ItemRect& AlignCenterLeft()                      { balign = GD::VCENTER | GD::LEFT;                return *this; }
                ItemRect& AlignCenterRight()                     { balign = GD::VCENTER | GD::RIGHT;               return *this; }
                ItemRect& AlignCenter()                          { balign = GD::HCENTER | GD::VCENTER;             return *this; }
                ItemRect& AlignBottomLeft()                      { balign = GD::LEFT    | GD::BOTTOM;              return *this; }
                ItemRect& AlignBottomRight()                     { balign = GD::RIGHT   | GD::BOTTOM;              return *this; }
                ItemRect& AlignBottomCenter()                    { balign = GD::HCENTER | GD::BOTTOM;              return *this; }

                ItemRect& HeaderAlignTop()                       { halign = GD::TOP     | GD::LEFT;                return *this; }
                ItemRect& HeaderAlignBottom()                    { halign = GD::BOTTOM  | GD::LEFT;                return *this; }
                ItemRect& HeaderAlignLeft()                      { halign = GD::LEFT    | GD::VCENTER;             return *this; }
                ItemRect& HeaderAlignRight()                     { halign = GD::RIGHT   | GD::VCENTER;             return *this; }
                ItemRect& HeaderAlignTopLeft()                   { halign = GD::LEFT    | GD::TOP;                 return *this; }
                ItemRect& HeaderAlignTopRight()                  { halign = GD::RIGHT   | GD::TOP;                 return *this; }
                ItemRect& HeaderAlignTopCenter()                 { halign = GD::HCENTER | GD::TOP;                 return *this; }
                ItemRect& HeaderAlignCenterLeft()                { halign = GD::VCENTER | GD::LEFT;                return *this; }
                ItemRect& HeaderAlignCenterRight()               { halign = GD::VCENTER | GD::RIGHT;               return *this; }
                ItemRect& HeaderAlignCenter()                    { halign = GD::HCENTER | GD::VCENTER;             return *this; }
                ItemRect& HeaderAlignBottomLeft()                { halign = GD::LEFT    | GD::BOTTOM;              return *this; }
                ItemRect& HeaderAlignBottomRight()               { halign = GD::RIGHT   | GD::BOTTOM;              return *this; }
                ItemRect& HeaderAlignBottomCenter()              { halign = GD::HCENTER | GD::BOTTOM;              return *this; }

                ItemRect& CtrlAlignTop(i32 t = 0, i32 s = 0)     { calign |= GD::TOP;     st = t; sy = s;         return *this; }
                ItemRect& CtrlAlignBottom(i32 b = 0, i32 s = 0)  { calign |= GD::BOTTOM;  sb = b; sy = s;         return *this; }
                ItemRect& CtrlAlignLeft(i32 l = 0, i32 s = 0)    { calign |= GD::LEFT;    sl = l; sx = s;         return *this; }
                ItemRect& CtrlAlignRight(i32 r = 0, i32 s = 0)   { calign |= GD::RIGHT;   sr = r; sx = s;         return *this; }
                ItemRect& CtrlAlignHorzCenter(i32 s = 0)         { calign |= GD::HCENTER; sx = s;                 return *this; }
                ItemRect& CtrlAlignVertCenter(i32 s = 0)         { calign |= GD::VCENTER; sy = s;                 return *this; }
                ItemRect& CtrlAlignCenter(i32 s = 0, i32 d = 0)  { calign |= GD::VCENTER | GD::HCENTER; sx = s; sy = d; return *this; }
                ItemRect& CtrlAlignHorzPos(i32 l = 0, i32 r = 0) { calign |= GD::HPOS;    sl = l; sr = r; sx = 1; return *this; }
                ItemRect& CtrlAlignVertPos(i32 t = 0, i32 b = 0) { calign |= GD::VPOS;    st = t; sb = b; sy = 1; return *this; }

                i32       GetAlign() const                       { return balign; }
                i32       GetHeaderAlign() const                 { return halign; }
                i32       GetCtrlAlign() const                   { return calign; }

                ItemRect& WrapText()                             { wrap = true; return *this; }

                ItemRect& SetFont(Font& f)                       { fnt = f;    return *this; }
                ItemRect& SetHeaderFont(Font& f)                 { hfnt = f;   return *this; }
                ItemRect& Bg(Color c)                            { bg = c;     return *this; }
                ItemRect& Fg(Color c)                            { fg = c;     return *this; }
                ItemRect& SetImage(const Image& i)               { img = i;    return *this; }
                ItemRect& ClearImage()                           { img = Null; return *this; }
                ItemRect& Ctrls(Callback1<One<Ctrl>&> _factory);
                ItemRect& Ctrls(void (*factory)(One<Ctrl>&))     { return Ctrls(callback(factory)); }
                template<class T> ItemRect&  Ctrls()             { return Ctrls(DefaultCtrlFactory<T>()); }
                ItemRect& NoCtrls();
                ItemRect& Option();
                ItemRect& ThreeStateOption();

                ItemRect& Editable(bool b);
                ItemRect& NoEditable();

                ItemRect& Sortable(bool b)                       { sortable = b;      return *this; }
                ItemRect& NoSortable()                           { sortable = false;  return *this; }

                ItemRect& Clickable(bool b)                      { clickable = b;     return *this; }
                ItemRect& NoClickable()                          { clickable = false; return *this; }

                ItemRect& Locked(bool b)                         { locked = b;        return *this; }
                ItemRect& NoLocked(bool b)                       { locked = false;    return *this; }

                ItemRect& Skip(bool b)                           { skip = b;          return *this; }

                ItemRect& DoAvg(tukk s = "")              { sop = SOP_AVG; sopfrm = s; return *this; }
                ItemRect& DoSum(tukk s = "")              { sop = SOP_SUM; sopfrm = s; return *this; }
                ItemRect& DoMin(tukk s = "")              { sop = SOP_MIN; sopfrm = s; return *this; }
                ItemRect& DoMax(tukk s = "")              { sop = SOP_MAX; sopfrm = s; return *this; }
                ItemRect& DoCount(tukk s = "")            { sop = SOP_CNT; sopfrm = s; return *this; }

                i32  GetId()                                     { return id;                       }
                i32  GetNumber()                                 { return id - parent->fixed_cols;  }

                bool IsConvertion() const                        { return convertion; }
                bool IsHidden() const                            { return hidden;     }

                bool IsSortOrg() const                           { return sortmode == 0; }
                bool IsSortAsc() const                           { return sortmode == 1; }
                bool IsSortDsc() const                           { return sortmode == 2; }

                Val ExtractVal(i32 r, i32 c) const;

                void Serialize(Stream &s);
                void Clear();
        };

        typedef Vec<ItemRect> RectItems;
        typedef Vec<ItemRect> VItems;
        typedef Vec<ItemRect> HItems;
        typedef Vec<JoinRect> JItems;

        typedef GridCtrl CLASSNAME;

        Items  items;
        HItems hitems;
        VItems vitems;
        JItems joins;

        Vec<Item> summary;
        Vec<Val> rowbkp;

        VecMap<Id, i32> aliases;
        Edits edits;

        CtrlsHolder holder;

        ScrollBar      sbx;
        ScrollBar      sby;
        ToolBar        bar;

        GridResizePanel resize_panel;
        GridPopUp popup;

        bool resize_panel_open:1;

        GridFind find;
        Label info;
        Button close;

        GridDisplay *display;
        GridDisplay *orgdisp;

        /* Properties */

        bool select_row:1;
        bool multi_select:1;
        bool indicator:1;
        bool resizing_cols:1;
        bool resizing_rows:1;
        bool moving_cols:1;
        bool moving_rows:1;
        bool resizing_fixed_cols:1;
        bool resizing_fixed_rows:1;
        bool dragging:1;
        bool horz_grid:1;
        bool vert_grid:1;
        bool draw_last_horz_line:1;
        bool draw_last_vert_line:1;
        bool sorting:1;
        bool sorting_multicol:1;
        bool header:1;
        bool live_cursor:1;
        bool row_changing:1;
        i32  coloring_mode;

        i32  resize_paint_mode;
        i32  resize_col_mode;
        i32  resize_row_mode;
        i32  edit_mode;
        bool one_click_edit:1;
        bool goto_first_edit:1;

        bool inserting:1;
        bool appending:1;
        bool duplicating:1;
        bool removing:1;
        bool accepting:1;
        bool canceling:1;
        bool moving:1;
        bool navigating:1;
        bool searching:1;
        bool closing:1;
        bool editing:1;
        bool edits_in_new_row:1;
        bool hiding:1;
        bool clipboard:1;
        bool extra_paste:1;
        bool fixed_paste:1;
        bool copy_allowed:1;
        bool cut_allowed:1;
        bool paste_allowed:1;
        bool copy_column_names:1;
        bool draw_focus:1;
        bool cancel_all:1;
        bool ask_remove:1;

        bool reject_null_row:1;
        bool tab_changes_row:1;
        bool tab_adds_row:1;
        bool enter_like_tab:1;
        bool keep_last_row:1;
        bool remove_hides:1;

        bool full_col_resizing:1;
        bool full_row_resizing:1;

        bool chameleon:1;
        bool summary_row:1;
        bool update_summary:1;
        bool popups:1;
        bool focus_lost_accepting:1;

        bool search_hide:1;
        bool search_highlight:1;
        bool search_highlight_first:1;
        bool search_immediate:1;
        bool search_case:1;
        bool search_move_cursor:1;
        bool search_display:1;
        i32  find_offset;

        static bool index_as_column;
        static bool reverse_sort_icon;

        /* States */

        bool newrow_inserted:1;
        bool newrow_appended:1;
        i32  row_modified;
        bool row_removed:1;
        bool sel_begin:1;
        bool sel_end:1;
        bool moving_header:1;
        bool moving_body:1;
        bool moving_allowed:1;
        bool row_order:1;
        bool row_data:1;
        bool scrollLeftRight:1;
        bool doscroll:1;
        bool ready:1;
        bool isedit:1;
        i32  genr_ctrls;
        bool edit_ctrls:1;
        bool recalc_cols;
        bool recalc_rows;
        bool cancel_update_cell:1;
        bool cancel_update:1;
        bool cancel_insert:1;
        bool cancel_remove:1;
        bool cancel_accept:1;
        bool cancel_duplicate:1;
        bool cancel_cursor:1;
        bool cancel_move:1;
        bool mouse_move:1;
        bool is_clipboard:1;
        bool enabled:1;

        bool call_whenchangecol:1;
        bool call_whenchangerow:1;
        bool call_whenremoverow:1;
        bool call_whenupdaterow:1;
        bool call_wheninsertrow:1;

        bool resizing;
        bool fixed_click;
        bool fixed_top_click;
        bool fixed_left_click;
        bool size_changed;
        bool top_click;
        bool just_clicked;
        bool synced;

        bool valid_cursor;


        /* Vals */

        i32  curSplitCol, curSplitRow;
        i32  firstRow,    lastRow;
        i32  firstCol,    lastCol;
        i32  firstVisCol, lastVisCol;
        i32  firstVisRow, lastVisRow;
        i32  moveCol,     oldMoveCol;
        i32  moveRow,     oldMoveRow;
        i32  total_cols,  total_rows;
        i32  fixed_cols,  fixed_rows;
        i32  total_width, total_height;
        i32  fixed_width, fixed_height;
        i32  summary_height;

        i32  oldSplitCol, oldSplitRow;

        i32  selected_rows;
        i32  selected_items;
        i32  colidx;
        i32  rowidx;
        i32  bains;
        i32  coluid;
        i32  rowuid;
        i32  rowfnd;

        i32  bkp_rowidx;

        i32  minRowSelected;
        i32  maxRowSelected;

        i32  sc_fr;
        i32  sc_lr;

        i32  join_group;

        i32  sync_flag;
        i32  paint_flag;

        /* Points */

        Point curpos; // cursor position
        Point anchor; // selection anchor

        Point oldpos;
        Point oldcur;
        Point curid;
        Point livecur;
        Point ctrlpos;
        Point ctrlid;
        Point leftpnt;
        Size  osz;

        /* Others */

        Ctrl * focused_ctrl;
        i32    focused_ctrl_id;
        Val  focused_ctrl_val;
        i32    focused_col;

        WTxt search_string;

        bool curResizeCol;
        bool curResizeRow;

        bool resizeCol;
        bool resizeRow;

        i32 splitCol, splitRow;
        i32 firstColPos, firstRowPos;
        i32 hcol, hrow;
        i32 dx, dy;

        GridPopUpHeader pophdr;
        FrameRight<StaticRect> scrollbox;

        i32 sortCol;
        Vec<i32> sortOrder;

    public:

        struct SortOrder : Moveable<SortOrder>
        {
            i32 id;
            Id name;
            bool ascending;
            bool descending;
        };

    public:

        /* Colors */

        Color fg_focus,  bg_focus;
        Color fg_select, bg_select;
        Color fg_live,   bg_live;
        Color fg_found,  bg_found;
        Color fg_even,   bg_even;
        Color fg_odd,    bg_odd;
        Color fg_grid;


    public:
        GridCtrl();
        ~GridCtrl();

        /* Properties */
        GridCtrl& HorzGrid(bool b = true)         { horz_grid           = b; return *this; }
        GridCtrl& VertGrid(bool b = true)         { vert_grid           = b; return *this; }
        GridCtrl& DrawLastHorzLine(bool b = true) { draw_last_horz_line = b; return *this; }
        GridCtrl& DrawLastVertLine(bool b = true) { draw_last_vert_line = b; return *this; }
        GridCtrl& MultiSelect(bool b = true)      { multi_select        = b; return *this; }
        GridCtrl& ResizingCols(bool b = true)     { resizing_cols       = b; return *this; }
        GridCtrl& ResizingRows(bool b = true)     { resizing_rows       = b; return *this; }
        GridCtrl& MovingCols(bool b = true)       { moving_cols         = b; return *this; }
        GridCtrl& MovingRows(bool b = true)       { moving_rows         = b; return *this; }
        GridCtrl& Dragging(bool b = true)         { dragging            = b; return *this; }
        GridCtrl& ResizePaintMode(i32 m = 0)      { resize_paint_mode   = m; return *this; }
        GridCtrl& LiveCursor(bool b = true)       { live_cursor         = b; return *this; }
        GridCtrl& Sorting(bool b = true)          { sorting             = b; return *this; }
        GridCtrl& MultiSorting(bool b = true)     { sorting_multicol    = b; return *this; }
        GridCtrl& Header(bool b = true)           { header              = b; return *this; }
        GridCtrl& EditMode(i32 m)                 { edit_mode           = m; return *this; }
        GridCtrl& EditRow()                       { edit_mode           = GE_ROW;  return *this; }
        GridCtrl& EditCell()                      { edit_mode           = GE_CELL; return *this; }
        GridCtrl& OneClickEdit(bool b = true)     { one_click_edit      = b; return *this; }
        GridCtrl& GotoFirstEdit(bool b = true)    { goto_first_edit     = b; return *this; }
        GridCtrl& Absolute()                      { return ResizeColMode(0);               }
        GridCtrl& Proportional()                  { return ResizeColMode(1);               }
        GridCtrl& SelectRow(bool b = true)        { select_row          = b; return *this; }
        GridCtrl& AutoHideHorzSb(bool b = true)   { sbx.AutoHide(b);         return *this; }
        GridCtrl& AutoHideVertSb(bool b = true)   { sby.AutoHide(b);         return *this; }
        GridCtrl& AutoHideSb(bool b = true)       { sbx.AutoHide(b);
                                                    sby.AutoHide(b);         return *this; }

        GridCtrl& ResizeColMode(i32 m = 0);
        GridCtrl& ResizeRowMode(i32 m = 0);

        GridCtrl& Indicator(bool b = true, i32 size = -1);

        GridCtrl& GridColor(Color fg = SColorShadow);
        GridCtrl& FocusColor(Color fg = SColorHighlightText, Color bg = SColorHighlight);
        GridCtrl& LiveColor(Color fg = SColorText, Color bg = SColorHighlight);
        GridCtrl& OddColor(Color fg = SColorText, Color bg = SColorInfo);
        GridCtrl& EvenColor(Color fg = SColorText, Color bg = Blend(SColorHighlight, SColorPaper, 220));
        GridCtrl& ColoringMode(i32 m);

        GridCtrl& ColorRows(bool b = true) { return ColoringMode(2 * b).EvenColor(); }
        GridCtrl& ColorCols(bool b = true) { return ColoringMode(1 * b).EvenColor(); }


        GridCtrl& SetDefaultRowHeight(i32 h)    { GD_ROW_HEIGHT = h; sby.SetLine(h); return *this; }
        GridCtrl& SetColWidth(i32 n, i32 width, bool recalc = true);
        GridCtrl& SetRowHeight(i32 n, i32 height, bool recalc = true);
        GridCtrl& SetColsMin(i32 size);
        GridCtrl& SetColsMax(i32 size);

        i32 GetDefaultRowHeight() { return GD_ROW_HEIGHT; }

        GridCtrl& Inserting(bool b = true)          { inserting            = b;  return *this; }
        GridCtrl& Appending(bool b = true)          { appending            = b;  return *this; }
        GridCtrl& Duplicating(bool b = true)        { duplicating          = b;  return *this; }
        GridCtrl& Moving(bool b = true)             { moving               = b;  return *this; }
        GridCtrl& Removing(bool b = true)           { removing             = b;  return *this; }
        GridCtrl& Accepting(bool b = true)          { accepting            = b;  return *this; }
        GridCtrl& Canceling(bool b = true)          { canceling            = b;  return *this; }
        GridCtrl& Navigating(bool b = true)         { navigating           = b;  return *this; }
        GridCtrl& Searching(bool b = true)          { searching            = b;  return *this; }
        GridCtrl& Closing(bool b = true)            { closing              = b;  return *this; }
        GridCtrl& Editing(bool b = true)            { editing              = b;  return *this; }
        GridCtrl& EditsInNewRow(bool b = true)      { edits_in_new_row     = b;  return *this; }
        GridCtrl& Hiding(bool b = true)             { hiding               = b;  return *this; }
        GridCtrl& Clipboard(bool b = true)          { clipboard            = b;  return *this; }
        GridCtrl& ExtraPaste(bool b = true)         { extra_paste          = b;  return *this; }
        GridCtrl& FixedPaste(bool b = true)         { fixed_paste          = b;  return *this; }
        GridCtrl& ClipboardCopy(bool b = true)      { copy_allowed         = b;  return *this; }
        GridCtrl& ClipboardCut(bool b = true)       { cut_allowed          = b;  return *this; }
        GridCtrl& ClipboardPaste(bool b = true)     { paste_allowed        = b;  return *this; }
        GridCtrl& CopyColumnNames(bool b = true)    { copy_column_names    = b;  return *this; }
        GridCtrl& AskRemove(bool b = true)          { ask_remove           = b;  return *this; }
        GridCtrl& RowChanging(bool b = true)        { row_changing         = b;  return *this; }

        GridCtrl& DrawFocus(bool b = true)          { draw_focus           = b;  return *this; }

        GridCtrl& RejectNullRow(bool b = true)      { reject_null_row      = b;  return *this; }
        GridCtrl& KeepLastRow(bool b = true)        { keep_last_row        = b;  return *this; }
        GridCtrl& RemoveHides(bool b = true)        { remove_hides         = b;  return *this; }
        GridCtrl& TabChangesRow(bool b = true)      { tab_changes_row      = b;  return *this; }
        GridCtrl& TabAddsRow(bool b = true)         { tab_adds_row         = b;  return *this; }
        GridCtrl& EnterLikeTab(bool b = true)       { enter_like_tab       = b;  return *this; }
        GridCtrl& FullColResizing(bool b = true)    { full_col_resizing    = b;  return *this; }
        GridCtrl& FullRowResizing(bool b = true)    { full_row_resizing    = b;  return *this; }
        GridCtrl& Chameleon(bool b = true)          { chameleon            = b;  return *this; }
        GridCtrl& SummaryRow(bool b = true)         { summary_row          = b;  return *this; }
        GridCtrl& Popups(bool b = true)             { popups               = b;  return *this; }
        GridCtrl& FocusLostAccepting(bool b = true) { focus_lost_accepting = b;  return *this; }

        GridCtrl& SearchOffset(i32 offset)          { find_offset     = offset;  return *this; }
        GridCtrl& SearchMoveCursor(bool b = true)   { search_move_cursor   = b;  return *this; }
        GridCtrl& SearchImmediate(bool b = true)    { search_immediate     = b;  return *this; }
        GridCtrl& SearchHideRows(bool b = true)     { search_hide          = b;  return *this; }
        GridCtrl& SearchDisplay(bool b = true)      { search_display       = b;  return *this; }
        static void IndexAsColumn(bool b = true)    { index_as_column      = b;                }
        static void ReverseSortIcon(bool b = true)  { reverse_sort_icon    = b;                }

        GridCtrl& SetToolBar(bool b = true, i32 align = BarCtrl::BAR_BOTTOM, i32 frame = 1);
        ToolBar&  GetToolBar() { return bar; }

        GridCtrl& ResizePanel(bool b = true);

        GridCtrl& BeforeAfterInserting()     { bains = 1; return *this; }
        GridCtrl& AfterBeforeInserting()     { bains = 2; return *this; }

        GridCtrl& HideColumn(i32 n = -1, bool refresh = true);
        GridCtrl& ShowColumn(i32 n, bool refresh = true);

        GridCtrl& HideRow(i32 n = -1, bool refresh = true);
        GridCtrl& ShowRow(i32 n, bool refresh = true);

        void MenuHideColumn(i32 n);

        void HideRows(bool repaint = false);
        void ShowRows(bool repaint = false);

        /* Virutals */

        virtual void Paint(Draw &w);
        virtual void MouseMove(Point p, dword keyflags);
        virtual void LeftDown(Point p, dword keyflags);
        virtual void LeftUp(Point p, dword keyflags);
        virtual void LeftDouble(Point p, dword keyflags);
        virtual void LeftRepeat(Point p, dword keyflags);
        virtual void RightDown(Point p, dword keyflags);
        virtual void Layout();
        virtual void MouseLeave();
        virtual bool Key(dword key, i32);
        virtual void GotFocus();
        virtual void LostFocus();
        virtual void ChildGotFocus();
        virtual void ChildLostFocus();
        virtual void MouseWheel(Point p, i32 zdelta, dword keyflags);
        virtual Image CursorImage(Point p, dword);
        virtual void State(i32 reason);
        virtual void ChildMouseEvent(Ctrl *child, i32 event, Point p, i32 zdelta, dword keyflags);
        virtual void ChildFrameMouseEvent(Ctrl *child, i32 event, Point p, i32 zdelta, dword keyflags);
        virtual bool Accept();
        virtual void Reject();

        virtual void DragAndDrop(Point p, PasteClip& d);

        void ChildAction(Ctrl *child, i32 event);
        void RestoreFocus();

        /* Methods */

        ItemRect& InsertColumn(i32 pos, tukk name = "", i32 size = -1, bool idx = false);
        ItemRect& AddColumn(const Id id, tukk name = "", i32 size = -1, bool idx = false);
        ItemRect& AddColumn(const Txt& name, i32 size = -1, bool idx = false);
        ItemRect& AddColumn(tukk name = "", i32 size = -1, bool idx = false);

        void RemoveColumn(i32 n, i32 count = 1);

        ItemRect& AddHiddenColumn()                 { return AddColumn(0, 0);    }
        ItemRect& AddHiddenColumn(tukk name) { return AddColumn(name, 0); }
        ItemRect& AddHiddenColumn(Txt &name)     { return AddColumn(name, 0); }

        ItemRect& AddIndex()                        { return AddColumn("", 0, true);    }
        ItemRect& AddIndex(Id id)                   { return AddColumn(id, ~id, 0, true); }
        ItemRect& AddIndex(tukk name)        { return AddColumn(name, 0, true); }
        ItemRect& AddIndex(Txt &name)            { return AddColumn(name, 0, true); }

        ItemRect& AddRow(i32 n = 1, i32 size = -1);
        GridCtrl& Add() { AddRow(); return *this; }

        //$-GridCtrl& Add(const Val& [, const Val& ]...);
        #define  E__Add(I)      GridCtrl& Add(__List##I(E__Val));
            __Expand(E__Add)
        #undef   E__Add
        //$+

        //$-ItemRect& AddRow(const Val& [, const Val& ]...);
        #define  E__Add(I)      ItemRect& AddRow(__List##I(E__Val));
            __Expand(E__Add)
        #undef   E__Add
        //$+

        GridCtrl& AddSeparator(Color c);

        i32       GetColumnCount() const      { return total_cols - fixed_cols; }
        i32       GetFixedColumnCount() const { return fixed_cols;              }

        ItemRect& GetColumn(i32 n);
        ItemRect& GetColumn();
        ItemRect& GetRow(i32 n);
        ItemRect& GetRow();
        Item&     GetCell(i32 n, i32 m);
        Item&     GetCell(i32 n, Id id);

        bool IsColumn(const Id& id);

        i32  GetCurrentRow() const;
        bool IsCurrentRow() const;
        void RestoreCurrentRow();

        i32 GetWidth(i32 n = -1);
        i32 GetHeight(i32 n = -1);
        i32 GetFixedWidth();
        i32 GetFixedHeight();

        void Set(i32 r, i32 c, const Val &val);
        void Set(i32 r, Id id, const Val &val);
        void Set(i32 r, tukk s, const Val &val);
        void Set(i32 c, const Val &val);
        void Set(Id id, const Val &val);
        void Set(i32 r, const Vec<Val> &v, i32 data_offset = 0, i32 column_offset = 0);
        void Set(const Vec<Val> &v, i32 data_offset = 0, i32 column_offset = 0);
        void SetAny(i32 r, i32 c, const Val &val);
        void SetRaw(i32 r, i32 c, const Val &val);
        void SetIndicator(i32 r, const Val &val);

        void SetCtrl(i32 r, i32 c, Ctrl& ctrl);
        void SetCtrl(i32 r, i32 c, Ctrl* ctrl);
        void SetCtrl(i32 c, Ctrl& ctrl);
        void SetCtrl(i32 c, Ctrl* ctrl);

        void ClearCtrl(i32 r, i32 c);
        void SetCtrlVal(i32 r, i32 c, const Val &val);
        void SetCtrlVal(i32 c, const Val &val);

        void   SetLast(i32 c, const Val &val);
        void   SetFixed(i32 r, i32 c, const Val &val);
        Val  GetFixed(i32 r, i32 c) const;
        Val  GetFixed(i32 c) const;
        Val  Get(i32 r, i32 c) const;
        Val  Get(i32 c) const;
        Val  Get(Id id) const;
        Val  Get(i32 r, Id id) const;
        Val  Get() const;
        Val  Get(tukk  alias) const;
        Val  Get(i32 r, tukk  alias) const;
        Val  GetRaw(i32 r, i32 c) const;
        Val  GetNew(i32 c) const;
        Val  GetFirst(i32 c) const;
        Val  GetLast(i32 c) const;
        Val  GetPrev(Id id) const;
        Val  GetPrev(i32 c) const;
        Val& operator() (i32 r, i32 c);
        Val& operator() (i32 c);
        Val& operator() (Id id);
        Val& operator() (i32 r, Id id);
        Val& operator() (tukk  alias);
        Val& operator() (i32 r, tukk  alias);
        void   SetSummary(i32 c, const Val& val);
        void   SetSummary(Id id, const Val& val);
        Val  GetSummary(i32 c);
        Val  GetSummary(Id id);

        Vec< Vec<Val> > GetVals();
        void SetVals(const Vec< Vec<Val> >& v);

        using Ctrl::IsModified;

        bool IsModified(i32 r, i32 c);
        bool IsModified(i32 c);
        bool IsModified(i32 r, Id id);
        bool IsModified(Id id);

        /* valid only in callbacks */

        bool IsNewRow()         { return newrow_inserted || newrow_appended; }
        bool IsNewRowInserted() { return newrow_inserted; }
        bool IsNewRowAppended() { return newrow_appended; }
        bool IsModifiedRow()    { return row_modified > 0; }
        void CommitNewRow()     { newrow_inserted = newrow_appended = false; }

        bool IsUpdatedRow()     { return vitems[rowidx].operation == GridOperation::UPDATE; }
        bool IsInsertedRow()    { return vitems[rowidx].operation == GridOperation::INSERT; }
        bool IsRemovedRow()     { return vitems[rowidx].operation == GridOperation::REMOVE; }
        bool IsChangedRow()     { return IsUpdatedRow() || IsInsertedRow();                 }
        bool IsVersionedRow()   { return vitems[rowidx].operation.GetVersion() > 0;         }
        i32  GetRowOperation()  { return vitems[rowidx].operation;                          }

        Vec<Val> ReadCol(i32 n = -1, i32 start_row = -1, i32 end_row = -1) const;
        Vec<Val> ReadRow(i32 n = -1, i32 start_col = -1, i32 end_col = -1) const;
        GridCtrl& Add(const Vec<Val> &v, i32 offset = 0, i32 count = -1, bool hidden = false);

        void SetFixedRows(i32 n = 1);
        void SetFixedCols(i32 n = 1);

        i32  GetStdRowHeight() { return GD_ROW_HEIGHT; }

        void RefreshRow(i32 n = -1, bool relative = true, bool fixed = false);
        void RefreshCol(i32 n = -1, bool relative = true, bool fixed = false);
        void RefreshRows(i32 from, i32 to, bool relative = true, bool fixed = false);
        void RefreshItem(i32 r, i32 c, bool relative = true);
        void RefreshNewRow();
        void RefreshFrom(i32 from);

        void RefreshTop();
        void RefreshLeft();
        void RefreshSummary();

        void Repaint(bool recalc_cols = false, bool recalc_rows = false, i32 opt = RP_ALL);
        void Ready(bool b);

        i32  GetMouseRow(Point &p, bool relative = true, bool fixed = false, bool full = true);
        i32  GetMouseCol(Point &p, bool relative = true, bool fixed = false, bool full = true);
        Point GetPos(Point p);

        void UpdateCursor();

        i32 Find(const Val &v, i32 col = 0, i32 start_from = 0, i32 opt = 0) const;
        i32 Find(const Val &v, Id id, i32 opt = 0) const;
        i32 Find(const Val &v0, Id id0, const Val &v1, Id id1, i32 opt = 0) const;
        i32 FindCurrent(Id id, i32 opt = GF::SKIP_CURRENT_ROW) const;
        i32 FindCurrent(Id id0, Id id1, i32 opt = GF::SKIP_CURRENT_ROW) const;

        i32 FindInRow(const Val& v, i32 row = 0, i32 start_from = 0) const;

        GridDisplay& GetDisplay() { return *display; }
        GridCtrl&    SetDisplay(GridDisplay &gd) { display = &gd; return *this; }

        void SetDisplay(i32 r, i32 c, GridDisplay& gd);

        bool IsEdit()  { return isedit; }
        bool StartEdit();
        bool SwitchEdit();
        bool EndEdit(bool accept = true, bool doall = false, bool remove_row = true);
        bool CancelEdit(bool remove_row = true) { return EndEdit(false, false, remove_row); }

        i32  Append(i32 cnt = 1, bool refresh = true, i32 height = -1);
        void Insert(i32 i, i32 cnt = 1);
        void Remove(i32 i = -1, i32 cnt = 1);
        void RemoveFirst(i32 cnt = 1);
        void RemoveLast(i32 cnt = 1);
        void Duplicate(i32 i, i32 cnt = 1);

        void CancelRemove()     { cancel_remove      = true; }
        void CancelUpdate()     { cancel_update      = true; }
        void CancelInsert()     { cancel_insert      = true; }
        void CancelUpdateCell() { cancel_update_cell = true; }
        void CancelAccept()     { cancel_accept      = true; }
        void CancelDuplicate()  { cancel_duplicate   = true; }
        void CancelCursor()     { cancel_cursor      = true; }
        void CancelMove()       { cancel_move        = true; }

        void ClearCursor(bool remove = false);
        void ClearRow(i32 r = -1, i32 column_offset = 0);
        void Clear(bool columns = false);
        void Reset();

        void ClearOperations();
        void ClearVersions();

        void Begin();
        void Next();
        void Prev();
        void End();
        void Move(i32 r);
        bool IsEnd();

        bool IsFirst();
        bool IsLast();
        bool IsNext();
        bool IsPrev();

        i32  SetCursor(i32 n);
        void SetCursor(const Point& p);
        i32  SetCursorId(i32 n);
        i32  GetCursor(bool rel = false) const;
        i32  GetPrevCursor(bool rel = false) const;
        i32  GetCursor(i32 uid) const;
        Point GetCursorPos() const;
        void CenterCursor();
        bool IsCursor() const      { return valid_cursor; }
        bool IsCursorBegin() const { return curpos.y == fixed_rows; }
        bool IsCursorEnd() const   { return curpos.y == total_rows - 1; }

        i32  GetNewRowPos();
        i32  GetNewRowId();
        i32  GetRemovedRowPos();

        i32  GetCursorId() const { return GetCursor(true); }
        i32  GetPrevCursorId() const { return GetPrevCursor(true); }
        i32  GetColId() const;
        i32  GetRowId() const;
        i32  GetColId(i32 n) const;
        i32  GetRowId(i32 n) const;
        i32  GetColUId() const;
        i32  GetRowUId() const;
        i32  FindCol(i32 id) const;
        i32  FindCol(const Id& id) const;
        i32  FindCol(const Txt& s) const;
        i32  FindRow(i32 id) const;

        i32  GetNextRow(i32 n);
        i32  GetPrevRow(i32 n);

        bool GoBegin(bool scroll = true);
        bool GoEnd(bool scroll = true, bool goleft = false);
        bool GoNext(bool scroll = true);
        bool GoPrev(bool scroll = true);
        bool GoLeft(bool scroll = true, bool ctrlmode = false);
        bool GoRight(bool scroll = true, bool ctrlmode = false);
        bool GoPageUp(bool scroll = true);
        bool GoPageDn(bool scroll = true);
        bool GoFirstVisible(bool scroll = true);
        void GoTo(i32 r, bool setcursor = true, bool scroll = true);
        void GoTo(i32 r, i32 c, bool setcursor = true, bool scroll = true);
        bool GoTop() { return GoBegin(); }

        void SwapCols(i32 n, i32 m);
        bool SwapRows(i32 n, i32 m, bool repaint = true);

        void SwapUp(i32 cnt = 1);
        void SwapDown(i32 cnt = 1);

        bool CanMoveCol(i32 n, i32 m);

        void MoveCol(i32 n, i32 m);
        bool MoveRow(i32 n, i32 m, bool repaint = true);

        void MoveRows(i32 n, bool onerow = false);

        i32  GetCount() const;
        i32  GetRowCount() const { return GetCount(); }
        i32  GetVisibleCount() const;
        i32  GetFixedCount() const;
        i32  GetTotalCount() const;

        void SetRowCount(i32 n, i32 size = -1);
        void SetColCount(i32 n, i32 size = 100);

        void Select(i32 n, i32 cnt = 1);
        void ClearSelection();
        bool IsSelection()           { return selected_rows > 0; }
        bool IsSelectionBegin()      { return sel_begin;         }
        bool IsSelectionEnd()        { return sel_end;           }
        i32  GetSelectedCount()      { return selected_rows;     }
        i32  GetSelectedItemsCount() { return selected_items;    }
        bool IsSelected(i32 n, bool relative = true);
        bool IsSelected(i32 n, i32 m, bool relative = true);
        bool IsSelected();

        void Copy(bool all = false)  { SetClipboard(all, true);  }
        void CopyAll()               { Copy(true);               }

        void DoInsert0(bool edit, bool after);
        void DoInsertBefore0(bool edit);
        void DoInsertBefore();
        void DoInsertBeforeNoEdit();
        void DoInsertAfter0(bool edit);
        void DoInsertAfter();
        void DoInsertAfterNoEdit();
        void DoRemove();
        void DoAppend0(bool edit);
        void DoAppend();
        void DoAppendNoEdit();
        void DoDuplicate0();
        void DoDuplicate();
        void DoEdit();
        void DoEndEdit();
        void DoCancelEdit();
        void DoSelectAll();
        void DoSwapUp();
        void DoSwapDown();
        void DoGoBegin();
        void DoGoEnd();
        void DoGoNext();
        void DoGoPrev();
        void DoGoLeft();
        void DoGoRight();
        void DoGoPageUp();
        void DoGoPageDn();
        void DoFind();
        void DoPaste();

        void StdMenuBar(Bar &bar);
        void StdToolBar(Bar &bar);
        void FindOptsBar(Bar &bar);
        void FindBar(Bar &bar, i32 cx);
        void InfoBar(Bar &bar, i32 cx);
        void NavigatingBar(Bar &bar);
        void RemovingMenu(Bar &bar);
        void MovingMenu(Bar &bar);
        void SelectMenu(Bar &bar);
        void ColumnsMenu(Bar &bar);
        void ColumnList(Bar &bar);
        void ClipboardMenu(Bar &bar);
        void PasteAsMenu(Bar &bar);

        void RebuildToolBar() { bar.Set(WhenToolBar); }
        void SetToolBarInfo(Txt inf);

        void ClearFound(bool showrows = true, bool clear_string = true);

        bool IsEmpty()        { return total_rows <= fixed_rows; }
        bool IsFilled()       { return total_rows > fixed_rows;  }
        bool IsOrderChanged() { return row_order; }
        bool IsDataChanged()  { return row_data; }
        bool IsChanged()      { return row_order || row_data; }
        void ClearChanged()   { row_order = row_data = false; ClearModify(); }

        bool IsRowEditable(i32 r = -1);
        bool IsRowClickable(i32 r = -1);

        void Serialize(Stream &s);

        Ctrl * GetCtrl(i32 r, i32 c); // checks visibility - if widget is out of view, returns NULL
        Ctrl * GetCtrlAt(i32 r, i32 c); // does not check visibility
        Ctrl * GetCtrl(i32 c); // checks visibility - if widget is out of view, returns NULL

        static i32 GetStdHeight() { return Draw::GetStdFontCy() + 4; }

        void Debug(i32 n = 0);
        void UpdateScrollbars() { UpdateSb(); }

        void JoinCells(i32 left, i32 top, i32 right, i32 bottom, bool relative = true);
        void JoinFixedCells(i32 left, i32 top, i32 right, i32 bottom);
        void JoinRow(i32 n, i32 left, i32 right);
        void JoinRow(i32 left = -1, i32 right = -1);
        void UpdateJoins(i32 row, i32 col, i32 cnt = 1);

        GridCtrl& Sort(i32 sort_col, i32 sort_mode = SORT_UP, bool multisort = false, bool repaint = true);
        GridCtrl& Sort(Id id, i32 sort_mode = SORT_UP, bool multisort = false, bool repaint = true);
        GridCtrl& MultiSort(i32 sort_col, i32 sort_mode = SORT_UP);
        GridCtrl& MultiSort(Id id, i32 sort_mode = SORT_UP);
        void ClearSort();
        void ReSort();
        void MarkSort(i32 col, i32 sort_mode);
        void MarkSort(Id id, i32 sort_mode);

        Vec<SortOrder> GetSortOrder() const;
        Vec<Id> GetSortOrderId() const;

        ScrollBar& HorzScroll() { return sbx; }
        ScrollBar& VertScroll() { return sby; }

        Val GetStdConvertedVal(const Val& v) const;
        Val GetConvertedColumn(i32 col, const Val& v) const;
        Val GetStdConvertedColumn(i32 col, const Val& v) const;
        Txt GetTxt(Id id) const;

        bool Search(dword key);
        i32 GetResizePanelHeight() const;
        Txt GetColumnName(i32 n) const;
        Id GetColumnId(i32 n) const;

        void SetCtrlFocus(i32 col);
        void SetCtrlFocus(Id id);

        Txt GetColumnWidths();
        void ColumnWidths(tukk s);
        #ifdef flagGRIDSQL
        void FieldLayout(FieldOperator& f);
        operator Fields() { return OTVET(FieldLayout); }
        operator SqlSet() const { return GetColumnList(); }
        SqlSet GetColumnList(bool skip_hidden = false) const;
        #endif

        void UpdateSummary(bool b = true);

    private:
        bool IsSelect(i32 n, i32 m, bool relative);

        bool TabKey(bool enter_mode);

        bool Go0(i32 jump, bool scroll = true, bool goleft = false, bool ctrlmode = false);

        CurState SetCursor0(Point p, i32 opt = 0, i32 dirx = 0, i32 diry = 0);
        CurState SetCursor0(i32 x, i32 y, i32 opt = 0, i32 dirx = 0, i32 diry = 0);
        i32 SetCursor0(i32 n);

        bool IsValidCursor(const Point &p, i32 fc, i32 lc, i32 fr, i32 lr) const;
        bool IsValidCursorVis(const Point &p) const;
        bool IsValidCursorAll(const Point &p) const;
        bool IsValidCursor(const Point &p) const;
        bool IsValidCursor(i32 c) const;

        void SetItemCursor(Point p, bool b, bool highlight);

        void Insert0(i32 row, i32 cnt = 1, bool recalc = true, bool refresh = true, i32 size = -1);
        bool Remove0(i32 row, i32 cnt = 1, bool recalc = true, bool refresh = true, bool whens = true);
        bool Duplicate0(i32 row, i32 cnt = 1, bool recalc = true, bool refresh = true);
        i32  Append0(i32 cnt = 1, i32 size = -1, bool refresh = true);

        void GoCursorLeftRight();

        void UpdateSb(bool horz = true, bool vert = true);
        void UpdateHolder(bool force = false);
        bool UpdateCols(bool force = false);
        bool UpdateRows(bool force = false);
        bool UpdateSizes();
        void UpdateHighlighting(i32 mode, Point p = Null);
        void UpdateVisColRow(bool col);

        bool HasCtrls();
        bool ShowNextCtrl();
        bool ShowPrevCtrl();
        public:
        void SyncCtrls(i32 r = -1, i32 c = -1);
        private:
        void UpdateCtrls(i32 opt = UC_CHECK_VIS | UC_SHOW | UC_CURSOR | UC_FOCUS);
        void SyncSummary();
        void SyncPopup();

        void SetCtrlsData();
        bool GetCtrlsData(bool samerow = false, bool doall = false, bool updates = true);
        bool CancelCtrlsData(bool all = false);
        void UpdateDefaults(i32 ri);

        i32  GetFocusedCtrlIndex();
        Point GetCtrlPos(Ctrl * ctrl);

        void Split(i32 state = 0, bool sync = false);
        void Scroll();

        bool IsTopHeader()  { return fixed_rows > 0; }
        bool IsLeftHeader() { return fixed_cols > 1 || indicator; }

        bool IsMouseBody(Point &p);

        void DrawLine(bool iniLine, bool delLine);

        Rect GetItemRect(i32 r, i32 c, bool hgrid = false, bool vgrid = false, bool hrel = false, bool vrel = false);

        bool Match(const WTxt &f, const WTxt &s, i32 &fs, i32 &fe);
        i32  ShowMatchedRows(const WTxt &f);

        void SelectCount(i32 i, i32 cnt = 1, bool sel = true);
        void SelectRange(i32 from, i32 to, bool sel = true);
        void SelectRange(Point from, Point to, bool sel = true, bool fullrow = false);
        void SelectRange(const Rect& r, bool sel , bool fullrow );
        void ShiftSelect(i32 from, i32 to);
        void ShiftSelect(Point from, Point to);
        void SelectInverse(i32 from, i32 to);
        i32  GetMinRowSelected();
        i32  GetMaxRowSelected();

        void CloseGrid();
        Txt RowFormat(tukk s);

        void SetFindOpts(i32 n);

        Item& GetItem(const Point &p) { return GetItem(p.y, p.x); }
        Item& GetItem(i32 n, i32 m)   { return items[vitems[n].id][hitems[m].id]; }

        void Set0(i32 r, i32 c, const Val &val, bool paste = false);
        Val Get0(i32 r, i32 c) const;

        i32 GetSplitCol(const Point &p, i32 splitSize = 5, bool full = false);
        i32 GetSplitRow(const Point &p, i32 splitSize = 5, bool full = false);

        i32 GetFirst0(Vec<ItemRect> &its, i32 total, i32 sb, i32 diff);
        i32 GetFirstVisCol(i32 diff);
        i32 GetFirstVisRow(i32 diff);

        bool SetDiffItemSize(bool horizontal, RectItems &its, i32 n, i32 diff, bool newsize = true);
        void Recalc(bool horizontal, RectItems &its, i32 n, double size, double diff);
        bool Recalc(bool horizontal, RectItems &its, i32 mode = -1);
        bool RecalcCols(i32 mode = -1);
        bool RecalcRows(i32 mode = -1);
        void CalcIntPos(RectItems &its, i32 n, i32 maxsize, i32 cnt, i32 resize_mode, bool renumber = false);

        Rect& AlignRect(Rect &r, i32 i);

        void MouseAccel(const Point &p, bool horz, bool vert, dword keyflags);

        void GSort(i32 scol = -1);
        void GSort(i32 col, i32 order, i32 from, i32 count = -1);
        void Multisort();
        i32  InMultisort(i32 col);
        void ClearMultisort(i32 n = 0);
        bool IsSorted();
        void MarkSort(i32 col, i32 sort_mode, bool refresh);

        void DrawHorzDragLine(Draw &w, i32 pos, i32 cx, i32 size, Color c);
        void DrawVertDragLine(Draw &w, i32 pos, i32 size, i32 dx, Color c);

        void SetOrder();

        void Nothing();
        void Init();

        Ctrl * GetCtrl(i32 x, i32 y, bool check_visibility, bool hrel = false, bool vrel = false, bool check_edits = true);
        Ctrl * GetCtrl(const Point &p, bool check_visibility, bool hrel = false, bool vrel = false, bool check_edits = true);
        bool IsCtrl(Point &p, bool check_visibility = true);

        GridClipboard GetClipboard();
        void SetClipboard(bool all = false, bool silent = false);
        bool IsClipboardAvailable();
        void PasteCallbacks(bool new_row);
        void Paste(i32 mode = 0);
        void DoCopy();
        void DoPasteInsertedRows();
        void DoPasteAppendedRows();

        Point GetBarOffset();
        void ClearModified();

        i32  GetIdCol(i32 id, bool checkall = false) const;
        i32  GetIdRow(i32 id, bool checkall = false) const;
        Val GetItemVal(const Item& it, i32 id, const ItemRect& hi, const ItemRect& vi);
        void  GetItemAttrs(const Item& it, const Val& val, i32 r, i32 c, const ItemRect& vi, const ItemRect& hi, dword& style, GridDisplay*& gd, Color& fg, Color& bg, Font& fnt);
        Item& GetItemSize(i32 &r, i32 &c, i32 &x, i32 &y, i32 &cx, i32 &cy, bool &skip, bool relx = true, bool rely = true);
        Val GetAttrTextVal(const Val& val);

        Image HorzPosImage();
        Image VertPosImage();

    private:
        bool WhenInsertRow0();

    public:

        Event<Bar&> WhenMenuBar;
        Event<Bar&> WhenToolBar;

        Event<> WhenLeftDouble;
        Event<> WhenLeftClick;
        Event<> WhenEnter;
        Event<> WhenEscape;

        Event<> WhenStartEdit;
        Event<> WhenEndEdit;

        Event<> WhenCreateRow;

        Event<> WhenAcceptRow;
        Event<> WhenAcceptedRow;

        Event<> WhenInsertRow;
        Event<> WhenUpdateRow;
        Event<> WhenRemoveRow;
        Event<> WhenRemovedRow;
        Event<> WhenDuplicateRow;

        Callback2<i32, i32> WhenMoveRow;

        Event<> WhenCancelNewRow;

        Event<> WhenUpdateCell;

        Event<> WhenUpdateSummary;

        Event<> WhenNewRow;
        Event<> WhenChangeCol;
        Event<> WhenBeforeChangeCol;
        Event<> WhenChangeRow;
        Event<> WhenBeforeChangeRow;
        Event<> WhenCursor;
        Event<> WhenEmpty;

        Event<> WhenCtrlAction;
        Event<> WhenCtrlsAction;

        Event<> WhenSearchCursor;
        Event<> WhenClose;
        Event<> WhenChangeOrder;

        Event<> StdInsert;
        Event<> StdAppend;
        Event<> StdRemove;
        Event<> StdDuplicate;
        Event<> StdEdit;

        Event<> WhenSort;
        Event<> WhenSorted;

        Event<Val&> ProcessSummaryVal;

        Event<i32, i32, Val&> WhenPasteCell;

        Event<Txt&> WhenPasting;
        Event<>        WhenPaste;
};

class GridText : Ctrl
{
    private:
        GridDisplay display;
        GridCtrl* parent;
        const Id* column;
        Color fg, bg;
        Font fnt;

    public:

        GridText() {}

        virtual void Paint(Draw& w)
        {
            Rect r = GetRect();
            display.Paint(w, r.left, r.top, r.Width(), r.Height(), parent->Get(*column), 0, fg, bg, fnt);
        }

        void Ink(Color c)    { fg = c;  }
        void Paper(Color c)  { bg = c;  }
        void SetFont(Font f) { fnt = f; }
        void Column(const Id& c) { column = &c; }
};

template<> void Xmlize(XmlIO& xml, GridCtrl& g);
template<> void Jsonize(JsonIO& json, GridCtrl& g);

}

#endif