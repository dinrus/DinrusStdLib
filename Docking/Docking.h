#ifndef _Docking_Docking_h_
#define _Docking_Docking_h_

#include <drx/CtrlLib/CtrlLib.h>
#include <drx/TabBar/TabBar.h>

namespace drx {

class DockWindow;

#include "ToolWin.h"
#include "DockableCtrl.h"
#include "DockMenu.h"
#include "DockTabBar.h"
#include "DockCont.h"
#include "DockPane.h"

class DockWindow : public TopWindow {
public:
    typedef DockWindow CLASSNAME;

    enum {
        DOCK_NONE   = -1,
        DOCK_LEFT   = SplitterFrame::LEFT,
        DOCK_TOP    = SplitterFrame::TOP,
        DOCK_RIGHT  = SplitterFrame::RIGHT,
        DOCK_BOTTOM = SplitterFrame::BOTTOM,
    };

    virtual void State(i32 reason);
    virtual bool Key(dword key, i32 count);
protected:
    enum { TIMEID_ACTION_CHECK = Ctrl::TIMEID_COUNT,
        TIMEID_ANIMATE,
        TIMEID_ANIMATE_DELAY,
        TIMEID_COUNT };
    // DnD interface
    struct HighlightCtrl : public DockableCtrl  {
        HighlightCtrl()         { BackPaint(); }
        void ClearHighlight()   { img.Clear(); buffer.Clear(); }
        void SetHighlight(const Val& hl, bool _isnested, bool cannest, Image bg = Image());
        void SetNested(bool _isnested);
        void CreateBuffer();
        virtual void Paint(Draw& w);
        i32 oldframesize;
        Rect bounds;
    private:
        Image img;
        Image buffer;
        bool isnested;
        bool cannest;
        const Val *highlight;
    };

    struct PosInfo : public Moveable<PosInfo> {
        PosInfo() : state(DockCont::STATE_NONE) {}
        DockCont::DockState state;  // State info
        Ptr<DockCont>       tabcont;// Only valid when ctrl was tabbed
        Txt data;                // Serialized context info dependent on state
    };
protected:
    // Highlight control
    virtual HighlightCtrl&  GetHighlightCtrl()                          { return hlc; }
    void            StartHighlight(DockCont *dcptr);
    void            Highlight(i32 align, DockCont& cont, DockCont *target);
    void            StopHighlight(bool do_animatehl);
    virtual bool    NeedFrameReorder(i32 align);
    bool            IsReorderingFrames()                                { return prehighlightframepos >= 0; }
    // Called by containers to signal drag-drop events
    virtual void    ContainerDragStart(DockCont& dc);
    virtual void    ContainerDragMove(DockCont& dc);
    virtual void    ContainerDragEnd(DockCont& dc);
    // Container docking/undocking
    void            DockContainer(i32 align, DockCont& c, i32 pos = -1);
    void            DockAsTab(DockCont& target, DockableCtrl& dc);
    void            DockContainerAsTab(DockCont& target, DockCont& c, bool do_nested);
    void            FloatContainer(DockCont& c, Point p = Null, bool move = true);
    void            FloatFromTab(DockCont& c, DockableCtrl& tab)        { FloatFromTab(c, *CreateContainer(tab)); }
    void            FloatFromTab(DockCont& c, DockCont& tab);
    void            AutoHideContainer(i32 align, DockCont& c);
    void            CloseContainer(DockCont& c);
    DockCont       *TabifyGroup(Txt group);
    void            Undock(DockCont& c);
    void            Unfloat(DockCont& c);
    void            Detach(DockCont& c)                                 { Undock(c); Unfloat(c); }
    // For finding drag-drop targets and computing boundary rect
    DockCont       *GetMouseDockTarget();
    DockCont       *FindDockTarget(DockCont& dc, i32& al);
    i32             FindDocker(const Ctrl *dc);
    Rect            GetAlignBounds(i32 al, Rect r, bool center, bool allow_lr = true, bool allow_tb = true);
    i32             GetPointAlign(const Point p, Rect r, bool center, bool allow_lr = true, bool allow_tb = true);
    i32             GetQuad(Point p, Rect r);
    Rect            GetFinalAnimRect(i32 align, Ctrl& c);
    // Helpers
    bool            IsTL(i32 align) const                   { return align < 2; } //ie (align == DOCK_LEFT || align == DOCK_TOP)
    bool            IsTB(i32 align) const                   { return align & 1; } //ie (align == DOCK_TOP || align == DOCK_BOTTOM)
    i32             GetDockAlign(const Ctrl& c) const;
    i32             GetDockAlign(const Point& p) const;
    i32             GetAutoHideAlign(const DockCont& c) const;
    bool            IsFrameAnimating(i32 align) const       { return frameanim[align].inc; }
    bool            IsPaneAnimating(i32 align) const        { return dockpane[align].IsAnimating(); }
    bool            CheckNesting() const                    { return (GetMouseFlags() & nesttoggle) ? !nestedtabs : nestedtabs; }

    void            DoHotKeys(dword key);

    void            SaveDockerPos(DockableCtrl& dc, PosInfo& pi);
    void            SetDockerPosInfo(DockableCtrl& dc, const PosInfo& pi);

    friend class    DockCont;
private:
    bool init;
    bool tabbing;
    bool autohide;
    bool animatehl;
    bool animatefrm;
    bool animatewnd;
    bool nestedtabs;
    bool grouping;
    bool menubtn;
    bool closebtn;
    bool hidebtn;
    bool dockable[4];
    bool locked;
    bool tabtext;
    bool tabalign;
    bool frameorder;
    bool showlockedhandles;
    bool childtoolwindows;
    dword nesttoggle;
    Txt layoutbackup;
    i32  dockframepos;
    i32  prehighlightframepos;

    Array<DockCont>             conts;
    Vec<DockableCtrl *>      dockers;
    ArrayMap<Txt, Txt>    layouts;
    Array<DockableCtrl>         ctrls;

    HighlightCtrl               hlc;
    DockPane                    dockpane[4];
    SplitterFrame               dockframe[4];
    AutoHideBar                 hideframe[4];
    DockMenu                    menu;
    Vec<PosInfo>             dockerpos;

    struct FrameAnim {
        FrameAnim() : target(0), inc(0) {}
        i32 target;
        i32 inc;
    };
    FrameAnim                   frameanim[4];
    i32                         animdelay;
public:
    // If you are attaching DockableCtrls programmatically it must be done in an overloaded DockInit.
    //  It is called after Ctrl::OPEN in order so that docking can be done with the correct window size.
    virtual void DockInit() { }
    // Attaches all the required frames to the DockWindow.
    //  It is called internally on Ctrl::Open so should not usually be needed
    void         DockLayout(bool tb_precedence = true);

    // Functions for registering/deregistering externally owned DockableCtrls
    //  NOTE: Registering is automatically done when using Dock/Float etc functions, but Register
    //  also be called before DockInit (eg in constructor), and are prefered if you are only going
    //  to be serializing the DockWindow layout
    DockableCtrl&   Register(DockableCtrl& dc);
    void            Deregister(const DockableCtrl& dc);

    // Creates an internal DockableCtrl for you to allow simple wrapping of GUI elements
    DockableCtrl&   Dockable(Ctrl& ctrl, WTxt title);
    DockableCtrl&   Dockable(Ctrl& ctrl, tukk title = 0)         { return Dockable(ctrl, (WTxt)title); }
    // More flexible methods for creating internally stored DockableCtrl derived classes
    template<class T>
    T&   CreateDockable(WTxt title);
    template<class T>
    T&   CreateDockable(tukk title = 0)                          { return CreateDockable<T>((WTxt)title); }

    // Get a list of all registered DockableCtrls (including externally owned ones
    const Vec<DockableCtrl *>& GetDockableCtrls() const              { return dockers; }

    // Simple docking control
    void            DockLeft(DockableCtrl& dc, i32 pos = -1)            { Dock(DOCK_LEFT, dc, pos); }
    void            DockTop(DockableCtrl& dc, i32 pos = -1)             { Dock(DOCK_TOP, dc, pos); }
    void            DockRight(DockableCtrl& dc, i32 pos = -1)           { Dock(DOCK_RIGHT, dc, pos); }
    void            DockBottom(DockableCtrl& dc, i32 pos = -1)          { Dock(DOCK_BOTTOM, dc, pos); }
    void            Dock(i32 align, DockableCtrl& dc, i32 pos = -1);
    // Attaches 'dc' as a tab to 'target', wherever it may be
    void            Tabify(DockableCtrl& target, DockableCtrl& dc);
    // Detaches the passed ctrl as a floating window
    void            Float(DockableCtrl& dc, Point p = Null);
    void            Float(DockableCtrl& dc, tukk title, Point p = Null);
    // Attaches the passed ctrl to the autohide frames on the window edges
    //  If no alignment is given the ctrl will be autohidden where it is docked or to the top bar if not docked
    void            AutoHide(DockableCtrl& dc);
    void            AutoHide(i32 align, DockableCtrl& dc);
    // Closes the window, detaching it from and docking panes or autohide frames.
    //  This just 'hides' the window, it can be opened again at any time.
    void            Close(DockableCtrl& dc);

    // Activate the passed ctrl. If open this will make sure it's the active tab, if
    //   closed the ctrls last known position will be restored (see RestoreDockerPos)
    void            ActivateDockable(Ctrl& c);
    // As above, but you can pass any child ctrl of the DockableCtrl (useful with the Dockable functions)
    void            ActivateDockableChild(Ctrl& c);

    // When DockableCtrls change state a record of their last position is stored internally.
    //  These functions can restore the previous DockableCtrl's postion of force a save of the current one
    void            SaveDockerPos(DockableCtrl& dc);
    void            RestoreDockerPos(DockableCtrl& dc, bool savefirst = false);
    // Toggle window visibility. If the window is open it is hidden, if it is hidden it is restored
    void            HideRestoreDocker(DockableCtrl& dc);

    // Group docking ctrl. These work the same way as for single DockableCtrls but apply to the passed group
    //  TabDockGroup and DockGroup will respect the DockAllowed settings for DockableCtrls
    void            DockGroup(i32 align, Txt group, i32 pos = -1);
    void            FloatGroup(Txt group);
    void            AutoHideGroup(i32 align, Txt group);
    void            AutoHideGroup(Txt group);
    void            TabFloatGroup(Txt group);
    void            CloseGroup(Txt group);
    void            TabDockGroup(i32 align, Txt group, i32 pos = -1);
    //  These variants of DockGroup and DockTabGroup ignore the DockAllowed settings
    void            ForceDockGroup(i32 align, Txt group, i32 pos = -1);
    void            ForceTabDockGroup(i32 align, Txt group, i32 pos = -1);

    // Check docking frame visibility. If there are no docked ctrls it will be invisibly
    bool            IsDockVisible(i32 align) const      { ASSERT(align >= 0 && align <= 4); return dockpane[align].IsVisible(); }
    // Manually sets the size of a docking frame
    void            SetFrameSize(i32 align, i32 size);

    // Animation settings. Disabling various forms of animation can improve performance when
    //  you have complex displays/GUIs in either DockableCtrls of the DockWindow client area
    DockWindow&     AnimateDelay(i32 ms)                { animdelay = max(ms, 0); return *this; }
    DockWindow&     Animate(bool highlight = true, bool frames = true, bool windows = true, i32 ticks = 10, i32 interval = 20);
    DockWindow&     NoAnimate()                         { return Animate(false, false); }
    bool            IsAnimated() const                  { return animatehl || animatewnd; }
    bool            IsAnimatedHighlight() const         { return animatehl; }
    bool            IsAnimatedFrames() const            { return animatefrm; }
    bool            IsAnimatedWindows() const           { return animatewnd; }

    // Lock control. When the layout is locked the user cannot use the drag-drop functionality, the
    //  title bar buttons or the titlebar context menus. ShowLockedHandles determines whether the
    //  title bar is shown on docked ctrls when the layout is locked.
    void            LockLayout(bool lock = true);
    void            UnlockLayout()                      { LockLayout(true); }
    bool            IsLocked() const                    { return locked; }
    DockWindow&     ShowLockedHandles(bool show = true) { showlockedhandles = show; SyncAll(); return *this; }
    bool            IsShowingLockedHandles() const      { return showlockedhandles; }

    // Enableing TabAutoAlign will move docked tab bars depending on where their contained is docked.
    //  For instance, a collection of controls docked one the left edge will have it's tab bar moved
    //  to its ;left hand side
    DockWindow&     TabAutoAlign(bool al = true);
    // If this is disabled only icons will be shown when windows are tabbed (in containers of autohidden)
    DockWindow&     TabShowText(bool text = true);

    // Enable/Disable tabbing support
    DockWindow&     Tabbing(bool _tabbing = true)       { tabbing = _tabbing; return *this; }
    DockWindow&     NoTabbing()                         { return Tabbing(false); }
    bool            IsTabbing() const                   { return tabbing; }
    // Enable/Disable multi-layer tab support (groups of tabs stored as a tab in another group)
    //  When enabled the user can use this function by holding NestedToggleKey during a drag-drop
    DockWindow&     NestedTabs(bool _nestedtabs = true) { nestedtabs = _nestedtabs; return *this; }
    DockWindow&     NoNestedTabs()                      { return NestedTabs(false); }
    bool            IsNestedTabs() const                { return nestedtabs; }
    dword           NestedToggleKey()                   { return nesttoggle; }
    DockWindow&     SetNestedToggleKey(dword key)       { nesttoggle = key; return *this; }
    // Enable/Disable grouping support
    DockWindow&     Grouping(bool grouping = true);
    DockWindow&     NoGrouping()                        { return Grouping(false); }
    bool            IsGrouping() const                  { return grouping; }
    // Allows the user to change docking frame precedence/ordering. By default the left and right
    //  frames take precedence over the top/bottom frames (meaning they occupy the entire height of the window)
    //  When this option is enabled the user chan change this by dragging to the appropriate corner of the frame
    DockWindow&     FrameReordering(bool reorder = true){ frameorder = reorder; return *this;}
    DockWindow&     NoFrameReordering()                 { return FrameReordering(false); }
    bool            IsFrameReordering() const           { return frameorder; }
    DockWindow&     SetFrameOrder(i32 first, i32 second = DOCK_NONE, i32 third = DOCK_NONE, i32 fourth = DOCK_NONE);
    // Allow/Block docking to particular sides of the DockWindow. This works in conjunction with
    //  the settings in the DockableCtrl class.
    DockWindow&     AllowDockAll();
    DockWindow&     AllowDockNone();
    DockWindow&     AllowDockLeft(bool v = true)    { dockable[DOCK_LEFT] = v; return *this; }
    DockWindow&     AllowDockTop(bool v = true)     { dockable[DOCK_TOP] = v; return *this; }
    DockWindow&     AllowDockRight(bool v = true)   { dockable[DOCK_RIGHT] = v; return *this; }
    DockWindow&     AllowDockBottom(bool v = true)  { dockable[DOCK_BOTTOM] = v; return *this; }
    DockWindow&     AllowDock(i32 a, bool v = true) { ASSERT(a >= 0 && a < 4); dockable[a] = v; return *this; }
    bool            IsDockAllowed(i32 align) const  { ASSERT(align >= 0 && align < 4); return dockable[align]; }
    bool            IsDockAllowed(i32 align, DockableCtrl& dc) const;
    bool            IsDockAllowedLeft() const       { return dockable[DOCK_LEFT]; }
    bool            IsDockAllowedTop() const        { return dockable[DOCK_TOP]; }
    bool            IsDockAllowedRight() const      { return dockable[DOCK_RIGHT]; }
    bool            IsDockAllowedBottom() const     { return dockable[DOCK_BOTTOM]; }
    bool            IsDockAllowedAny() const        { return dockable[DOCK_LEFT] || dockable[DOCK_TOP]
                                                                    || dockable[DOCK_RIGHT] || dockable[DOCK_BOTTOM]; }
    bool            IsDockAllowedNone() const       { return !IsDockAllowedAny(); }
    // Enable/Disable the AutoHide functions
    DockWindow&     AutoHide(bool v = true);
    bool            IsAutoHide()                    { return autohide; }
    // Determines which buttons are visible on docked/autohidden DockableCtrl title bars
    DockWindow&     WindowButtons(bool menu, bool hide, bool close);
    bool            HasMenuButtons() const          { return menubtn; }
    bool            HasHideButtons() const          { return hidebtn; }
    bool            HasCloseButtons() const         { return closebtn; }
    // When enabled (default) floating DockableCtrl windows are set to be tool windows
    //  (exactly what this means varies between OSs)
    void            ChildToolWindows(bool v = true) { childtoolwindows = v; SyncAll(); }
    bool            HasToolWindows() const          { return childtoolwindows; }

    // Opens the standard Docking Configuration dialog (see DockConfig)
    void            DockManager();
    // Returns the standard Docking menu so that an application can add it to it's main menu bar
    void            DockWindowMenu(Bar& bar);

    // SerializeWindow serialized both the DockWindow layout and the window position/state (using TopWindow::SerializePlacement)
    void            SerializeWindow(Stream& s);
    // SerializeLayout only serializes the DockQWindow layout
    void            SerializeLayout(Stream& s, bool withsavedlayouts = true);
    // Saves/Load/Manage dockwindow layouts. Theese are handy for saving different
    //  GUI configurations for different application states (like editing/debugging)
    i32             SaveLayout(Txt name);
    void            LoadLayout(i32 ix);
    void            LoadLayout(Txt name);
    void            DeleteLayout(Txt name)       { layouts.RemoveKey(name); }
    Txt          GetLayoutName(i32 ix) const     { return layouts.GetKey(ix); }
    i32             LayoutCount() const             { return layouts.GetCount(); }
    const ArrayMap<Txt, Txt>&  GetLayouts() const { return layouts; }
    // Serializes/Loads layout to/from an internal buffer (used for cancelling changes in the DockConfig window)
    // The difference between this and Save/LoadLayout is that it also backs-up the saved layouts
    void            BackupLayout();
    void            RestoreLayout();
    // This enables/disables all floating windows. This can be used to prevent the user
    //  moving windows when a dialog is visible (like DockConfig)
    void            DisableFloating()               { EnableFloating(false); }
    void            EnableFloating(bool enable = true);
    // Changes the Chameleon style for the highlight ctrl
    void            SetHighlightStyle(DockableCtrl::Style& s)   { GetHighlightCtrl().SetStyle(s); }
    // Constructer
    DockWindow();
private:
    // Container management
    DockCont       *GetContainer(Ctrl& dc)      { return dynamic_cast<DockCont *>(dc.GetParent()); }
    DockCont       *CreateContainer();
    DockCont       *CreateContainer(DockableCtrl& dc);
    void            DestroyContainer(DockCont& c);
    DockCont       *GetReleasedContainer(DockableCtrl& dc);

    void            SyncContainer(DockCont& c);
    void            SyncAll();

    // Helpers
    void            Dock0(i32 align, Ctrl& c, i32 pos, bool do_animatehl = false, bool ishighlight = false);
    void            Dock0(i32 align, Ctrl& c, i32 pos, Size sz, bool do_animatehl = false, bool ishighlight = false);
    void            Undock0(Ctrl& c, bool do_animatehl = false, i32 fsz = -1, bool ishighlight = false);
    void            Activate(DockableCtrl& dc);

    void            DoFrameSize(bool animate, i32 align, i32 targetsize);
    void            FrameAnimateTick();
    void            DoFrameReorder(i32 align);
    void            UndoFrameReorder();

    Size            CtrlBestSize(const Ctrl& c, i32 align, bool restrict = true) const;

    void            ClearLayout();
};

class PopUpDockWindow : public DockWindow {
public:
    struct Style : ChStyle<Style> {
        Val inner[5];
        Val outer[4];
        Val hide[4];
        Val highlight;
        i32 innersize;
        i32 outersize;
    };
    static const Style& StyleDefault();
protected:
    virtual void ContainerDragStart(DockCont& dc);
    virtual void ContainerDragMove(DockCont& dc);
    virtual void ContainerDragEnd(DockCont& dc);

private:
    struct PopUpButton : public Ctrl {
        const Val *icon;
        const Val *hlight;
        virtual void Paint(Draw& w) { Rect r = GetSize(); ChPaint(w, r, *icon); if (hlight) ChPaint(w, r, *hlight); }
        PopUpButton() { hlight = NULL; }
    };

    const Style    *style;
    DockCont       *last_target;
    PopUpButton    *last_popup;
    PopUpButton     inner[5];
    PopUpButton     outer[4];
    PopUpButton     hide[4];
    bool            showhide;

    i32     PopUpHighlight(PopUpButton *pb, i32 cnt);
    void    ShowOuterPopUps(DockCont& dc);
    void    ShowInnerPopUps(DockCont& dc, DockCont *target);
    void    ShowPopUp(PopUpButton& pb, const Rect& r);
    void    HidePopUps(bool _inner, bool _outer);
public:
    PopUpDockWindow&    SetStyle(const Style& s);
    PopUpDockWindow();

    PopUpDockWindow&    AutoHidePopUps(bool v = true) { showhide = v; return *this; }

};

#define LAYOUTFILE <drx/Docking/Docking.lay>
#include <drx/CtrlCore/lay.h>

class DockConfigDlg : public WithDockConfigLayout<TopWindow>
{
public:
    typedef DockConfigDlg CLASSNAME;

    DockConfigDlg(DockWindow& dockwindow);
private:
    DockWindow &                    dock;
    const Vec<DockableCtrl *>&   dockers;
    VecMap<Txt, i32>          groups;
    DockMenu                        menu;
    Txt                          backup;
    i32                             all;
    DockableCtrl                   *highlight;

    void RefreshTree(bool dogroups = false);

    void OnTreeContext(Bar& bar);
    void OnSaveLayout();
    void OnLoadLayout();
    void OnDeleteLayout();
    void OnTreeCursor();
    void OnListCursor();
    void OnNewGroup();
    void OnDeleteGroup();
    void OnOK();
    void OnCancel();
    void OnLock();

    void OnTreeDrag();
    void OnTreeDrop(i32 parent, i32 ii, PasteClip& d);

    void    Highlight(DockableCtrl *dc);
    void    StopHighlight();

    void    DeleteGroup(i32 id);

    Txt  DockerTxt(DockableCtrl *dc) const;
    Txt  PositionTxt(DockableCtrl *dc) const;
    i32     ListIndex() const;
    void    DupePrompt(Txt type);
};

template <class T>
T& DockWindow::CreateDockable(WTxt title)
{
    T &q = ctrls.Create<T>();
    Register(q.Title(title));
    return q;
}

}

#endif
