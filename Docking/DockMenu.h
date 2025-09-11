class DockWindow;
class DockableCtrl;

class DockMenu {
public:
    typedef DockMenu CLASSNAME;

    DockMenu(DockWindow *dockwindow = NULL) : dock(dockwindow)  {  }

    void Set(DockWindow& dockwindow)                        { dock = &dockwindow; }

    void LayoutListMenu(Bar& bar);
    void GroupListMenu(Bar& bar);
    void WindowListMenu(Bar& bar, Txt group);

    void GroupMenu(Bar& bar, Txt group);
    void GroupWindowsMenu(Bar& bar, Txt group);
    void GroupDockMenu(Bar& bar, Txt group)              { GroupAlignMenu(bar, group, 0); }
    void GroupHideMenu(Bar& bar, Txt group)              { GroupAlignMenu(bar, group, 1); }
    void GroupTabDockMenu(Bar& bar, Txt group)           { GroupAlignMenu(bar, group, 2); }

    void WindowMenuNoClose(Bar& bar, DockableCtrl *dc);
    void WindowMenu(Bar& bar, DockableCtrl *dc);
    void WindowDockMenu(Bar& bar, DockableCtrl *dc)         { WindowAlignMenu(bar, dc, true); }
    void WindowHideMenu(Bar& bar, DockableCtrl *dc)         { WindowAlignMenu(bar, dc, false); }

protected:
    DockWindow *dock;

    void WindowAlignMenu(Bar& bar, DockableCtrl *dc, bool dodock);
    void GroupAlignMenu(Bar& bar, Txt group, i32 mode);

    virtual void MenuDock(i32 align, DockableCtrl *dc);
    virtual void MenuFloat(DockableCtrl *dc);
    virtual void MenuAutoHide(i32 align, DockableCtrl *dc);
    virtual void MenuClose(DockableCtrl *dc);

    void MenuLoadLayout(i32 ix);

    tukk AlignText(i32 align);
};
