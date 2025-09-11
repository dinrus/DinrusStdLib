#include "Docking.h"

namespace drx {

#define DOCKBACK(x)               callback(dock, &DockWindow::x)
#define DOCKBACK1(x, arg)         callback1(dock, &DockWindow::x, arg)
#define DOCKBACK2(m, a, b)        callback2(dock, &DockWindow::m, a, b)
#define DOCKBACK3(m, a, b, c)     callback3(dock, &DockWindow::m, a, b, c)
#define DOCKBACK4(m, a, b, c, d)  callback4(dock, &DockWindow::m, a, b, c, d)

#define GROUPMACRO(a, g, m) (m == 0) ? DOCKBACK3(DockGroup, a, g, -1) : ((m == 1) ? DOCKBACK2(AutoHideGroup, a, g) : DOCKBACK3(TabDockGroup, a, g, -1))

#define DOCK_LEFT   (i32)DockWindow::DOCK_LEFT
#define DOCK_TOP    (i32)DockWindow::DOCK_TOP
#define DOCK_RIGHT  (i32)DockWindow::DOCK_RIGHT
#define DOCK_BOTTOM (i32)DockWindow::DOCK_BOTTOM

void DockMenu::LayoutListMenu(Bar& bar)
{
    i32 cnt = dock->LayoutCount();
    for (i32 i = 0; i < cnt; i++)
        bar.Add(dock->GetLayoutName(i), OTVET1(MenuLoadLayout, i));
}

void DockMenu::GroupListMenu(Bar& bar)
{
    const Vec<DockableCtrl *>& dcs = dock->GetDockableCtrls();
    Array<Txt> groups;
    for (i32 i = 0; i < dcs.GetCount(); i++) {
        Txt s = dcs[i]->GetGroup();
        if (!s.IsEmpty()) {
            bool found = false;
            for (i32 j = 0; j < groups.GetCount(); j++)
                if (groups[j] == s) {
                    found = true;
                    break;
                }
            if (!found)
                groups.Add(s);
        }
    }
    Sort(groups);
    for (i32 i = 0; i < groups.GetCount(); i++)
        bar.Add(groups[i], OTVET1(GroupWindowsMenu, groups[i]));
    if (dcs.GetCount())
        bar.Add(t_("Все"), OTVET1(GroupWindowsMenu, Txt(Null)));
}

void DockMenu::WindowListMenu(Bar& bar, Txt group)
{
    const Vec<DockableCtrl *>& dcs = dock->GetDockableCtrls();
    if (group == t_("Все")) group = Null;

    for (i32 i = 0; i < dcs.GetCount(); i++) {
        if (IsNull(group) || group == dcs[i]->GetGroup())
            bar.Add(AsTxt(dcs[i]->GetTitle()), dcs[i]->GetIcon(), OTVET1(WindowMenu, dcs[i]));
    }
}

void DockMenu::GroupMenu(Bar& bar, Txt group)
{
    if (group.IsEmpty()) group = t_("Все");
    Txt text = group;
    text.Insert(0, ' ');

    bar.Add(true, t_("Док") + text,  OTVET1(GroupDockMenu, group));
    bar.Add(true, t_("Флот") + text,    DOCKBACK1(FloatGroup, group));
    if (dock->IsAutoHide()) bar.Add(true, t_("Авто-Скрывать") + text, OTVET1(GroupHideMenu, group));
    bar.Separator();
    bar.Add(true, t_("Вкладки и Док") + text, OTVET1(GroupTabDockMenu, group));
    bar.Add(true, t_("Вкладки и Флот") + text, DOCKBACK1(TabFloatGroup, group));
    if(dock->HasCloseButtons())
    {
        bar.Separator();
        bar.Add(true, t_("Закрыть") + text, CtrlImg::Remove(), DOCKBACK1(CloseGroup, group));
    }
}

void DockMenu::GroupWindowsMenu(Bar& bar, Txt group)
{
    GroupMenu(bar, group);
    bar.Separator();
    WindowListMenu(bar, group);
}

void DockMenu::WindowMenu(Bar& bar, DockableCtrl *dc)
{
    WindowMenuNoClose(bar, dc);
    if(dock->HasCloseButtons())
    {
        bar.Separator();
        bar.Add(true, t_("Закрыть"), CtrlImg::Remove(), OTVET1(MenuClose, dc));
    }
}

void DockMenu::WindowMenuNoClose(Bar& bar, DockableCtrl *dc)
{
    bar.Add(true, t_("Док"),         OTVET1(WindowDockMenu, dc)).Check(dc->IsDocked() || dc->IsTabbed());
    bar.Add(true, t_("Флот"),       OTVET1(MenuFloat, dc)).Check(dc->IsFloating());
    if (dock->IsAutoHide())
        bar.Add(true, t_("Авто-Скрывать"),  OTVET1(WindowHideMenu, dc)).Check(dc->IsAutoHide());
}

void DockMenu::GroupAlignMenu(Bar& bar, Txt group, i32 mode)
{
    for (i32 i = 0; i < 4; i++)
        bar.Add(AlignText(i), GROUPMACRO(i, group, mode));
}

void DockMenu::WindowAlignMenu(Bar& bar, DockableCtrl *dc, bool dodock)
{
    for (i32 i = 0; i < 4; i++)
        bar.Add(!dodock || dock->IsDockAllowed(i, *dc), AlignText(i),
            dodock ? OTVET2(MenuDock, i, dc) : OTVET2(MenuAutoHide, i, dc));
}

void DockMenu::MenuDock(i32 align, DockableCtrl *dc)
{
    dock->Dock(align, *dc);
}

void DockMenu::MenuFloat(DockableCtrl *dc)
{
    dock->Float(*dc);
}

void DockMenu::MenuAutoHide(i32 align, DockableCtrl *dc)
{
    dock->AutoHide(align, *dc);
}

void DockMenu::MenuClose(DockableCtrl *dc)
{
    dock->Close(*dc);
}

void DockMenu::MenuLoadLayout(i32 ix)
{
    dock->LoadLayout(ix);
}

tukk  DockMenu::AlignText(i32 align)
{
    switch (align) {
    case DOCK_LEFT:
        return t_("Слева");
    case DOCK_TOP:
        return t_("Вверху");
    case DOCK_RIGHT:
        return t_("Справа");
    case DOCK_BOTTOM:
        return t_("Внизу");
    }
    return 0;
}

}
