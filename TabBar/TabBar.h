#ifndef _TabBar_TabBar_h_
#define _TabBar_TabBar_h_

#include <drx/CtrlLib/CtrlLib.h>

#define IMAGECLASS TabBarImg
#define IMAGEFILE <drx/TabBar/TabBar.iml>
#include <drx/Draw/iml_header.h>

namespace drx {

struct AlignedFrame : FrameCtrl<Ctrl>
{
    i32 layout;
    i32 framesize;
    i32 border;
public:
    enum
    {
        LEFT = 0,
        TOP = 1,
        RIGHT = 2,
        BOTTOM = 3
    };

    AlignedFrame() : layout(TOP), framesize(0), border(0) {}

    virtual void FrameAddSize(Size& sz);
    virtual void FramePaint(Draw& w, const Rect& r);
    virtual void FrameLayout(Rect& r);

    bool          IsVert() const    { return (layout & 1) == 0; }
    bool          IsHorz() const    { return layout & 1; }
    bool          IsTL() const      { return layout < 2; }
    bool          IsBR() const      { return layout >= 2; }

    AlignedFrame& SetAlign(i32 align) { layout = align; FrameSet(); RefreshParentLayout(); return *this; }
    AlignedFrame& SetLeft()     { return SetAlign(LEFT); }
    AlignedFrame& SetTop()      { return SetAlign(TOP); }
    AlignedFrame& SetRight()    { return SetAlign(RIGHT); }
    AlignedFrame& SetBottom()   { return SetAlign(BOTTOM); }
    AlignedFrame& SetFrameSize(i32 sz, bool refresh = true);

    i32           GetAlign() const      { return layout; }
    i32           GetFrameSize() const  { return framesize; }
    i32           GetBorder() const     { return border; }
protected:
    void Fix(Size& sz);
    void Fix(Point& p);
    Size Fixed(const Size& sz);
    Point Fixed(const Point& p);

    bool          HasBorder()               { return border >= 0; }
    AlignedFrame& SetBorder(i32 _border)    { border = _border; return *this; }

    virtual void  FrameSet()                { }
};

class TabScrollBar : public AlignedFrame
{
    private:
        i32 total;
        double pos, ps;
        i32 new_pos;
        i32 old_pos;
        double start_pos;
        double size;
        double cs, ics;
        virtual void UpdatePos(bool update = true);
        void RefreshScroll();
        bool ready;
        Size sz;
    public:
        TabScrollBar();

        virtual void Paint(Draw& w);
        virtual void LeftDown(Point p, dword keyflags);
        virtual void LeftUp(Point p, dword keyflags);
        virtual void MouseMove(Point p, dword keyflags);
        virtual void MouseWheel(Point p, i32 zdelta, dword keyflags);
        virtual void Layout();

        i32  GetPos() const;
        void SetPos(i32 p, bool dontscale = false);
        void AddPos(i32 p, bool dontscale = false);
        i32  GetTotal() const;
        void AddTotal(i32 t);
        void SetTotal(i32 t);
        void GoEnd();
        void GoBegin();
        void Clear();
        void Set(const TabScrollBar& t);
        bool IsScrollable() const;
        Event<>  WhenScroll;
};

class TabBar : public AlignedFrame
{
public:
    struct Style : public TabCtrl::Style
    {
        Image crosses[3];
        Val group_separators[2];

        Style & Write() const               { return *static_cast<Style *>(&TabCtrl::Style::Write()); }

        Style&  DefaultCrosses();
        Style&  Variant1Crosses();
        Style&  Variant2Crosses();

        Style&  DefaultGroupSeparators();
        Style&  GroupSeparators(Val horz, Val vert);
        Style&  NoGroupSeparators()         { return GroupSeparators(Val(), Val()); }
    };

    TabBar& SetStyle(const TabBar::Style& s);

protected:
    enum {
        TB_MARGIN = 5,
        TB_SPACE = 10,
        TB_SBHEIGHT = 4,
        TB_SBSEPARATOR = 1,
        TB_ICON = 16,
        TB_SPACEICON = 3
    };
    const Style *style;

public:
    struct TabItem : Moveable<TabItem> {
        i32 x;
        i32 y;
        Size size;
        WTxt text;
        Color ink;
        Font font;
        Image img;
        i32 side;
        bool clickable;
        bool cross;
        i32 stacked_tab;

        TabItem& Clickable(bool b = true) { clickable = b; return *this; }
        void Clear();

        TabItem() : side(LEFT), clickable(false), cross(false), stacked_tab(-1) {}
        Txt ToTxt() const {
            return Format("%d, %d - %s", x, y, text);
        }
    };

    struct Tab : Moveable<Tab> {
        i32 id;

        Image  img;
        Color  col;
        Val  key;
        Val  value;
        Txt group;

        Txt  stackid;
        i32     stack;

        bool visible;

        Point pos;
        Size  size;

        Point cross_pos;
        Size  cross_size;

        Point tab_pos;
        Size  tab_size;

        Txt ToTxt() const
        {
            return Format("Key: %`, Group: %`, StackId: %`, Stack: %d", key, group, stackid, stack);
        }

        virtual void Serialize(Stream& s);

        Array<TabItem> items;
        i32 itn;

        Tab();
        Tab(const Tab& t) { Set(t); }

        void Set(const Tab& t);

        bool HasMouse(const Point& p) const;
        bool HasMouseCross(const Point& p) const;
        bool HasIcon() const                        { return !img.IsEmpty(); }
        i32  Right() const                          { return pos.x + size.cx; }

        TabItem& AddItem();
        void Clear();
        TabItem& AddVal(const Val& q, const Font& font = StdFont(), const Color& ink = SColorText);
        TabItem& AddText(const WTxt& s, const Font& font = StdFont(), const Color& ink = SColorText);
        TabItem& AddImage(const Image& img, i32 side = LEFT);
        TabItem& AddSpace(i32 space = 5, i32 side = LEFT);

        virtual ~Tab() {}
    };

    // Tab sorting structures
    struct TabSort {
        virtual bool operator()(const Tab& a, const Tab& b) const = 0;
    };
    struct TabGroupSort : public TabSort {
        virtual bool operator()(const Tab& a, const Tab& b) const { return a.group < b.group; }
    };
protected:
    struct Group : Moveable<Group> {
        Group() {}
        Txt name;
        i32 active;
        i32 count;
        i32 first;
        i32 last;
        virtual void Serialize(Stream& s);
        Txt ToTxt() const { return Format("%s - %d", name, count); }

        virtual ~Group() {}
    };

    struct TabValSort : public TabSort {
        virtual bool operator()(const Tab& a, const Tab& b) const { return (*vo)(a.value, b.value); }
        const ValOrder *vo;
    };
    struct TabKeySort : public TabSort {
        virtual bool operator()(const Tab& a, const Tab& b) const { return (*vo)(a.key, b.key); }
        const ValOrder *vo;
    };

protected:
    TabScrollBar    sc;

    Array<Group>    groups;
    Array<Tab>      tabs;
    Array<i32>      separators;
    i32             active;
    i32             id;

    i32 highlight;
    i32 drag_highlight;
    i32 target;
    i32 cross;
    bool crosses;
    i32 crosses_side;
    bool isctrl;
    bool isdrag;
    bool grouping;
    bool autoscrollhide;
    bool nosel;
    bool nohl;
    bool inactivedisabled;
    bool stacking;
    bool stacksort;
    bool groupsort;
    bool groupseps;
    bool tabsort;
    bool allownullcursor;
    bool icons;
    bool contextmenu;
    i32 mintabcount;
    Point mouse, oldp;
    i32 group;
    const Display *display;
    Image dragtab;
    i32 stackcount;
    i32 scrollbar_sz;
    bool allowreorder;

    TabSort *tabsorter;
    TabSort *groupsorter;
    TabSort *stacksorter;
    TabValSort valuesorter_inst;
    TabKeySort   keysorter_inst;
    TabValSort stacksorter_inst;

    void    PaintTab(Draw& w, const Size& sz, i32 i, bool enable, bool dragsample = false);

    i32     TabPos(const Txt& g, bool& first, i32 i, i32 j, bool inactive);
    void    ShowScrollbarFrame(bool b);
    void    SyncScrollBar(bool synctotal = true);
    void    Scroll();

    i32     FindId(i32 id) const;
    i32     GetNext(i32 n, bool drag = false) const;
    i32     GetPrev(i32 n, bool drag = false) const;

    i32     GetWidth(i32 n);
    i32     GetExtraWidth(i32 n);
    i32     GetWidth() const;
    i32     GetHeight(bool scrollbar = true) const;

    bool    SetCursor0(i32 n, bool action = false);

    void    DoStacking();
    void    DoUnstacking();
    void    InsertIntoStack(Tab& t, i32 ix);
    i32     GetStackCount(i32 stackix) const;
    i32     FindStackHead(i32 stackix) const;
    i32     FindStackTail(i32 stackix) const;
    bool    IsStackHead(i32 n) const;
    bool    IsStackTail(i32 n) const;
    i32     SetStackHead(Tab& t);
    void    CycleTabStack(i32 head, i32 n);
    i32     CycleTabStack(i32 n);

    i32     GetNextId();
    i32     GetScrollPos()              { return sc.GetPos(); }

    i32 GetStyleHeight() const;
    static Image AlignImage(i32 align, const Image& img);
    static Val AlignVal(i32 align, const Val& v, const Size& isz);

    using Ctrl::GetStdSize;
    using Ctrl::Close;
public:
    enum { JumpDirLeft, JumpDirRight };

    struct JumpStack : Moveable< JumpStack > {
        i32        All;
        i32        Rest;
        i32        jump_direct;

        void Reset()                          { All = 0; Rest = 0; jump_direct = JumpDirLeft; }
        bool IsReset() const                  { return ( All == 0 ); }
        bool IsFull() const                   { return ( All == Rest ); }
        void Activate( i32 N, i32 jd )        { All = N; Rest = N; jump_direct = jd; }

        JumpStack() { Reset(); }
    };

    JumpStack jump_stack;
    i32  GetTabLR( i32 jd );
    i32  GetTabStackLR( i32 jd );
    i32  GetLR( i32 c, i32 jd );

protected:
    virtual void Paint(Draw& w);
    virtual void LeftDown(Point p, dword keysflags);
    virtual void LeftUp(Point p, dword keysflags);
    virtual void LeftDouble(Point p, dword keysflags);
    virtual void RightDown(Point p, dword keyflags);
    virtual void MiddleDown(Point p, dword keyflags);
    virtual void MiddleUp(Point p, dword keyflags);
    virtual void MouseMove(Point p, dword keysflags);
    virtual void MouseLeave();
    virtual void DragAndDrop(Point p, PasteClip& d);
    virtual void LeftDrag(Point p, dword keyflags);
    virtual void DragEnter();
    virtual void DragLeave();
    virtual void DragRepeat(Point p);
    virtual void CancelMode();
    virtual void MouseWheel(Point p, i32 zdelta, dword keyflags);
    virtual void FrameSet();
    virtual void Layout();

    // Mouse handling/tab positioning
    Point AdjustMouse(Point const &p) const;
    bool ProcessMouse(i32 i, const Point& p);
    bool ProcessStackMouse(i32 i, const Point& p);
    void SetHighlight(i32 n);
    i32  GetTargetTab(Point p);
    void Repos();
    Size GetBarSize(Size ctrlsz) const;
    Rect GetClientArea() const;

    // Grouping
    void MakeGroups();
    i32  FindGroup(const Txt& g) const;
    void CloseAll(i32 exception, i32 last_closed = 0);
    void GoGrouping(i32 n);
    void DoGrouping(i32 n);
    void DoCloseGroup(i32 n);
    void NewGroup(const Txt& name);
    void DoTabSort(TabSort& sort);
    void SortTabs0();
    void SortStack(i32 stackix);
    void SortStack(i32 stackix, i32 head, i32 tail);

    void CloseGroup();

    // Insertion without repos/refresh - for batch actions
    i32 InsertKey0(i32 ix, const Val& key, const Val& value, Image icon = Null, Txt group = Null);

    // Sub-class Paint override
    void PaintTabItems(Tab& t, Draw &w, const Rect& rn, i32 align);
    virtual void ComposeTab(Tab& tab, const Font &font, Color ink, i32 style);
    virtual void ComposeStackedTab(Tab& tab, const Tab& stacked_tab, const Font& font, Color ink, i32 style);
    virtual Size GetStdSize(const Tab& t);
    virtual Size GetStackedSize(const Tab& t);
    Size         GetStdSize(const Val& v);

    // Paint helpers
    i32     GetTextAngle();
    Point   GetTextPosition(i32 align, const Rect& r, i32 cy, i32 space) const;
    Point   GetImagePosition(i32 align, const Rect& r, i32 cx, i32 cy, i32 space, i32 side, i32 offset = 2) const;
bool    PaintIcons()                                    { return icons; }

    // Sorting/Stacking overrides
    virtual Txt      GetStackId(const Tab& a)            { return a.group; }
    // For sub-classes to receive cursor changes without using WhenAction
    virtual void CursorChanged() { }
    // for sub-classes to receive tab closes without using WhenClose
    virtual void TabClosed(Val key) { }

    bool IsCancelClose(i32 id);
    bool IsCancelCloseAll(i32 exception, i32 last_closed = 0);

public:
    typedef TabBar CLASSNAME;

    Event<>                     WhenHighlight;      // Executed on tab mouse-over
    Event<>                     WhenLeftDouble;     // Executed on left-button double-click (clicked tab will be the active tab)
    Gate<Val>                 CancelClose;        // Return true to allow cancel button. Parameter: Key of closed tab
    Gate<Val>                 ConfirmClose;       // Called before closing
    Event<Val>                WhenClose;          // Executed before tab closing. Parameter: Key of closed tab
    Gate<>                      CancelCloseAll;     // Return true to allow close all action
    Gate<>                      ConfirmCloseAll;    // Called before closing
    Event<>                     WhenCloseAll;       // Executed before 'Close All' action
    Gate<ValArray>            CancelCloseSome;    // Return true to cancel action (executed with list of closing tabs)
    Gate<ValArray>            ConfirmCloseSome;   // Called before closing the group
    Event<ValArray>           WhenCloseSome;      // Executed before any 'Close' action (with list of closing tabs)
    Gate<i32, i32>              CancelDragAndDrop;  // Return true to cancel drag and drop from tab to tab

    TabBar();
    TabBar& CopyBaseSettings(const TabBar& src);
    void    Set(const TabBar& t);
    TabBar& Add(const Val& value, Image icon = Null, Txt group = Null, bool make_active = false);
    TabBar& Insert(i32 ix, const Val& value, Image icon = Null, Txt group = Null, bool make_active = false);

    TabBar& AddKey(const Val& key, const Val& value, Image icon = Null, Txt group = Null, bool make_active = false);
    TabBar& InsertKey(i32 ix, const Val& key, const Val& value, Image icon = Null, Txt group = Null, bool make_active = false);

    void    CloseForce(i32 n, bool action = true);
    void    Close(i32 n, bool action = true);
    void    CloseKey(const Val& key);
    void    Clear();

    TabBar& Crosses(bool b = true, i32 side = RIGHT);
    TabBar& Stacking(bool b = true);
    TabBar& Grouping(bool b = true);
    TabBar& ContextMenu(bool b = true);
    TabBar& GroupSeparators(bool b = true);
    TabBar& AutoScrollHide(bool b = true);
    TabBar& InactiveDisabled(bool b = true);
    TabBar& AllowNullCursor(bool b = true);
    TabBar& Icons(bool v = true);

    TabBar& SortTabs(bool b = true);
    TabBar& SortTabsOnce();
    TabBar& SortTabsOnce(TabSort& sort);
    TabBar& SortTabs(TabSort& sort);

    TabBar& SortTabVals(ValOrder& sort);
    TabBar& SortTabValsOnce(ValOrder& sort);
    TabBar& SortTabKeys(ValOrder& sort);
    TabBar& SortTabKeysOnce(ValOrder& sort);

    TabBar& SortGroups(bool b = true);
    TabBar& SortGroupsOnce();
    TabBar& SortGroupsOnce(TabSort& sort);
    TabBar& SortGroups(TabSort& sort);

    TabBar& SortStacks(bool b = true);
    TabBar& SortStacksOnce();
    TabBar& SortStacksOnce(TabSort& sort);
    TabBar& SortStacks(TabSort& sort);

    TabBar& SortStacks(ValOrder& sort);

    bool    IsValSort() const             { return tabsort; }
    bool    IsGroupSort() const             { return groupsort; }
    bool    IsStackSort() const             { return stacksort; }

    TabBar& AllowReorder(bool v = true)             { allowreorder = v; return *this; }

    bool    IsGrouping() const              { return grouping; }
    bool    HasGroupSeparators() const      { return separators.GetCount(); }
    bool    IsStacking() const              { return stacking; }
    bool    IsShowInactive() const          { return inactivedisabled; }

    TabBar& NeverEmpty()                    { return MinTabCount(1); }
    TabBar& MinTabCount(i32 cnt)            { mintabcount = max(cnt, 0); Refresh(); return *this; }

    TabBar& SetDisplay(const Display& d)    { display =& d; Refresh(); return *this; }
    TabBar& SetBorder(i32 border)           { AlignedFrame::SetBorder(border); return *this; }
    i32     FindKey(const Val& v) const;
    i32     FindVal(const Val& v) const;

    Val   GetKey(i32 n) const             { ASSERT(n >= 0 && n < tabs.GetCount()); return tabs[n].key;}
    Val   GetVal(i32 n) const           { ASSERT(n >= 0 && n < tabs.GetCount()); return tabs[n].value;}
    Val   Get(const Val& key) const     { return GetVal(FindKey(key)); }
    void    Set(i32 n, const Val& newkey, const Val& newvalue);
    void    Set(i32 n, const Val& newkey, const Val& newvalue, Image icon);
    void    SetVal(const Val &key, const Val &newvalue);
    void    SetVal(i32 n, const Val &newvalue);
    void    SetKey(i32 n, const Val &newkey);
    void    SetIcon(i32 n, Image icon);
    void    SetTabGroup(i32 n, const Txt& group);

    void    SetColor(i32 n, Color c);

    const Tab& operator[] (i32 n) const     { return tabs[n]; }

    virtual Val   GetData() const;
    virtual void    SetData(const Val& key);

    Txt  GetGroupName() const            { return (group == 0) ? Null : groups[group].name; }
    Txt  GetGroupName(i32 i) const       { return groups[i].name;       }
    i32     SetGroup(const Txt& s)       { DoGrouping(max(0, FindGroup(s))); return group; }
    i32     SetGroup(i32 c)                 { DoGrouping(c); return group;     }
    i32     GetGroup() const                { return group;                }
    i32     GetGroupCount() const           { return groups.GetCount();    }
    void    SetGroupActive(i32 id)          { groups[group].active = id;   }
    i32     GetGroupActive() const          { return groups[group].active; }
    i32     GetFirst() const                { return groups[group].first;  }
    i32     GetLast() const                 { return groups[group].last;   }
    bool    IsGroupAll() const              { return group == 0;           }

    i32     GetCursor() const               { return active; }
    bool    HasCursor() const               { return active >= 0; }
    i32     GetHighlight() const            { return highlight; }
    bool    HasHighlight() const            { return highlight >= 0; }
    i32     GetCount() const                { return tabs.GetCount(); }

    void    SetCursor(i32 n);
    void    KillCursor()                    { SetCursor(-1); Refresh(); }

    Image   GetDragSample();
    Image   GetDragSample(i32 n);

    i32             GetScrollPos() const            { return sc.GetPos(); }
    TabBar&         SetScrollThickness(i32 sz);

    void AddFrameToScrollBar(CtrlFrame& fr)  { sc.AddFrame(fr); }

    Vec<Val>   GetKeys() const;
    Vec<Image>   GetIcons() const;
    TabBar&         CopySettings(const TabBar& src);
    virtual void    Serialize(Stream& s);

    const Style&    GetStyle() const                    { ASSERT(style); return *style; }

    virtual void    ContextMenu(Bar& bar);

    static const Style& StyleDefault();
};

#include "FileTabs.h"
#include "TabBarCtrl.h"

}

#endif
