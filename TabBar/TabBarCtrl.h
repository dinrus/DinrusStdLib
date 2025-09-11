class TabBarCtrl : public TabBar
{
private:
    VecMap<Val, Ctrl *> ctrls;
    StaticRect pane;

public:
    TabBarCtrl()                            { Ctrl::Add(pane); }
    virtual void Layout()                   { TabBar::Layout(); pane.SetRect(GetClientArea()); }

    TabBarCtrl& AddCtrl(Ctrl &ctrl, Val key, Val value, Image icon = Null, Txt group = Null, bool make_active = false);
    TabBarCtrl& AddCtrl(Ctrl &ctrl, Val value, Image icon = Null, Txt group = Null, bool make_active = false);
    TabBarCtrl& InsertCtrl(i32 ix, Ctrl &ctrl, Val key, Val value, Image icon = Null, Txt group = Null, bool make_active = false);
    TabBarCtrl& InsertCtrl(i32 ix, Ctrl &ctrl, Val value, Image icon = Null, Txt group = Null, bool make_active = false);

    void    RemoveCtrl(Val key);
    void    RemoveCtrl(i32 ix);
    void    RemoveCtrl(Ctrl &c);

    Ctrl *  GetCtrl(Val key);
    Ctrl *  GetCtrl(i32 ix);
    Ctrl *  GetCurrentCtrl();
    i32     GetCurrentIndex();

    void    SetCtrl(Val key);
    void    SetCtrl(i32 ix);
    void    SetCtrl(Ctrl &ctrl);

    Vec<Ctrl*> GetCtrlGroup(const Txt& group);

protected:
    virtual void CursorChanged();
    virtual void TabClosed(Val key);
};
