class DockTabBar : public TabBar {
public:
    typedef DockTabBar CLASSNAME;

    Callback1<i32>  WhenContext;
    Callback1<i32>  WhenDrag;

    virtual void    FrameAddSize(Size& sz);
    virtual void    FrameLayout(Rect& r);
    bool            IsAutoHide() const              { return GetCount() <= autohide; }

    DockTabBar&     AutoHideMin(i32 hidemin = 1)    { autohide = hidemin; return *this; }
    DockTabBar&     Icons(bool b = true)            { icons = b; return *this; }

    void            SyncRepos()                     { Repos(); }
    void            ShowText(bool show)             { showtext = show; }

    DockTabBar();
protected:
    i32 autohide;
    bool icons:1;
    bool showtext:1;

    virtual Size GetStdSize(const Tab &t);

    virtual void RightDown(Point p, dword keyflags);
    virtual void LeftDown(Point p, dword keyflags)  { TabBar::LeftDown(p, keyflags &= ~K_SHIFT); }
    virtual void LeftUp(Point p, dword keyflags)    { TabBar::LeftUp(p, keyflags &= ~K_SHIFT); }
    virtual void LeftDrag(Point p, dword keyflags);
};

class DockCont;

class AutoHideBar : public DockTabBar {
public:
    typedef AutoHideBar CLASSNAME;

    virtual void MouseEnter(Point p, dword keyflags);
    virtual void MouseLeave();

    void    AddCtrl(DockCont& c, const Txt& group = Null);
    i32     FindCtrl(const DockCont& c) const;
    DockCont *GetCtrl(i32 ix)   const               { return ValTo<DockCont *>(GetKey(ix));  }
    void    RemoveCtrl(i32 ix);
    void    RemoveCtrl(DockCont& c)                 { return RemoveCtrl(c, FindCtrl(c)); }
    void    RemoveCtrl(DockCont& c, i32 ix);
    bool    HasCtrl(const DockCont& c) const        { return (FindCtrl(c) >= 0); }

    void    ShowAnimate(Ctrl *c);

    static void SetTimeout(i32 delay_ms)            { ASSERT(delay_ms > 0); autohide_timeout = delay_ms; }

    AutoHideBar();

    virtual void ComposeTab(Tab& tab, const Font &font, Color ink, i32 style);

private:
    static i32 autohide_timeout;

    struct HidePopup : public Ctrl  {
        HidePopup() { BackPaint(); }
        Callback WhenEnter;
        Callback WhenLeave;
        virtual void ChildMouseEvent(Ctrl *child, i32 event, Point p, i32 zdelta, dword keyflags);
    };

    enum { TIMEID_ACTION_CHECK = Ctrl::TIMEID_COUNT,
           TIMEID_HIDE_TIMEOUT,
           TIMEID_COUNT };
    Ctrl        *ctrl;
    HidePopup    popup;

    void    TabDrag(i32 ix);
    void    TabHighlight();
    void    TabClose(Val v);
    void    HideAnimate(Ctrl *c);
    void    AdjustSize(Rect& r, const Size& sz);
};

struct DockTabable
{
    virtual Val GetSortVal() = 0;
};
