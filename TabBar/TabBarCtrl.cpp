#include "TabBar.h"

namespace drx {

TabBarCtrl& TabBarCtrl::AddCtrl(Ctrl &ctrl, Val key, Val value, Image icon, Txt group, bool make_active)
{
    return InsertCtrl(GetCount(), ctrl, key, value, icon, group, make_active);
}

TabBarCtrl& TabBarCtrl::AddCtrl(Ctrl &ctrl, Val value, Image icon, Txt group, bool make_active)
{
    return InsertCtrl(GetCount(), ctrl, value, value, icon, group, make_active);
}

TabBarCtrl& TabBarCtrl::InsertCtrl(i32 ix, Ctrl &ctrl, Val key, Val value, Image icon, Txt group, bool make_active)
{
    ctrls.Add(key, &ctrl);
    ctrl.Hide();
    pane.Add(ctrl.SizePos());
    TabBar::InsertKey(ix, key, value, icon, group, make_active);
    return *this;
}

TabBarCtrl& TabBarCtrl::InsertCtrl(i32 ix, Ctrl &ctrl, Val value, Image icon, Txt group, bool make_active)
{
    return InsertCtrl(ix, ctrl, value, value, icon, group, make_active);
}

void TabBarCtrl::RemoveCtrl(Val key)
{
    i32 ix = ctrls.Find(key);
    if (ix >= 0) {
        ctrls[ix]->Remove();
        ctrls.Remove(ix);
    }
    CloseKey(key);
}

void TabBarCtrl::RemoveCtrl(i32 ix)
{
    RemoveCtrl(GetKey(ix));
}

void TabBarCtrl::RemoveCtrl(Ctrl &ctrl)
{
    for (i32 i = 0; i < ctrls.GetCount(); ++i)
        if (ctrls[i] == &ctrl)
            return RemoveCtrl(i);
}

Ctrl * TabBarCtrl::GetCtrl(Val key)
{
    i32 ix = ctrls.Find(key);
    return (ix >= 0) ? ctrls[ix] : NULL;
}

Ctrl * TabBarCtrl::GetCtrl(i32 ix)
{
    ASSERT(ix >= 0 && ix < ctrls.GetCount());
    return GetCtrl(GetKey(ix));
}

Ctrl * TabBarCtrl::GetCurrentCtrl()
{
    return GetCtrl(GetData());
}

i32 TabBarCtrl::GetCurrentIndex()
{
    return FindKey(GetData());
}

void TabBarCtrl::SetCtrl(Val key)
{
    i32 ix = ctrls.Find(key);
    if (ix < 0)
        return;
    SetData(key);
}

void TabBarCtrl::SetCtrl(i32 ix)
{
    ASSERT(ix < GetCount());
    return SetCtrl(GetKey(ix));
}

void TabBarCtrl::SetCtrl(Ctrl &ctrl)
{
    for(i32 i = 0; i < ctrls.GetCount(); i++)
        if(ctrls[i] == &ctrl)
        {
            SetData(ctrls.GetKey(i));
            return;
        }
    SetData(-1);
}

void TabBarCtrl::CursorChanged()
{
    Val ix = GetData();
    for (i32 i = 0; i < ctrls.GetCount(); i++)
        ctrls[i]->Show(ctrls.GetKey(i) == ix);
}

void TabBarCtrl::TabClosed(Val key)
{
    ctrls.RemoveKey(key);
}


Vec<Ctrl*> TabBarCtrl::GetCtrlGroup(const Txt& group)
{
    Vec<Ctrl*> v;

    for(const TabBar::Tab& t : tabs)
        if(t.group == group) {
            Ctrl *c = GetCtrl(t.key);
            if(c) v.Add(c);
        }

    return pick(v);
}


}