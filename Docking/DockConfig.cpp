#include "Docking.h"

namespace drx {

#define HIGHLIGHT_DURATION 1000

DockConfigDlg::DockConfigDlg(DockWindow& dockwindow)
: dock(dockwindow),
  dockers(dockwindow.GetDockableCtrls()),
  menu(&dockwindow),
  highlight(NULL)
{
    CtrlLayoutOKCancel(*this, t_("Менеджер Докируемых Окон"));
    Sizeable().Zoomable();

    // Make backup of layout
    TxtStream s;
    dock.SerializeLayout(s, true);
    backup = s;

    // Setup groups
    RefreshTree(true);

    // Setup layout list
    const ArrayMap<Txt, Txt>& l = dock.GetLayouts();
    for (i32 i = 0; i < l.GetCount(); i++)
        list.Add(l.GetKey(i));

    tree.MultiSelect(true);
    tree.WhenCursor     = OTVET(OnTreeCursor);
    tree.WhenBar        = OTVET(OnTreeContext);
    tree.WhenDropInsert = OTVET(OnTreeDrop);
    tree.WhenDrag       = OTVET(OnTreeDrag);

    list.MultiSelect(false);
    list.WhenSel    = OTVET(OnListCursor);
    list.WhenSel    = OTVET(OnListCursor);

    savelayout  <<= OTVET(OnSaveLayout);
    loadlayout  <<= OTVET(OnLoadLayout);
    deletelayout <<= OTVET(OnDeleteLayout);
    newgroup    <<= OTVET(OnNewGroup);
    deletegroup <<= OTVET(OnDeleteGroup);
    ok          <<= OTVET(OnOK);
    cancel      <<= OTVET(OnCancel);
    WhenClose   =   OTVET(OnCancel);
    locked      <<= OTVET(OnLock);

    animatehighlight    <<= dock.IsAnimatedHighlight();
    animateframes       <<= dock.IsAnimatedFrames();
    animatewindows      <<= dock.IsAnimatedWindows();
    locked      <<= dock.IsLocked();
    tabbing     <<= dock.IsTabbing();
    tabnesting  <<= dock.IsNestedTabs();
    autohide    <<= dock.IsAutoHide();
    grouping    <<= dock.IsGrouping();

    closebtn    <<= dock.HasCloseButtons();
    autohidebtn <<= dock.HasHideButtons();
    menubtn     <<= dock.HasMenuButtons();

    OnTreeCursor();
    OnListCursor();
}

void DockConfigDlg::RefreshTree(bool dogroups)
{
    TreeCtrl::Node n;

    if (dogroups) {
        tree.NoRoot(true).Clear();
        groups.Clear();
        n.Set(-1, t_("Все")).CanSelect(false).CanOpen(true);
        all = tree.Add(0, n);
        for (i32 i = 0; i < dockers.GetCount(); i++) {
            Txt s = dockers[i]->GetGroup();
            i32 group = s.IsEmpty() ? -1 : groups.Find(s);
            if (!s.IsEmpty() && group < 0) {
                n.Set(groups.GetCount(), s);
                group = tree.Add(0, n);
                tree.Open(group);
                groups.Add(s, group);
            }
        }
        if (!groups.GetCount()) tree.Open(all);
    }
    else {
        for (i32 i = 0; i < tree.GetChildCount(0); i++)
            tree.RemoveChildren(tree.GetChild(0, i));
    }
    n.CanSelect(true).CanOpen(false);
    for (i32 i = 0; i < dockers.GetCount(); i++) {
        Txt s = dockers[i]->GetGroup();
        i32 group = s.IsEmpty() ? -1 : groups.Find(s);
        s = DockerTxt(dockers[i]);
        n.SetImage(dockers[i]->GetIcon()).Set(i, s);
        if (group >= 0)
            tree.Add(groups[group], n);
        tree.Add(all, n);
    }
}

void DockConfigDlg::OnTreeContext(Bar& bar)
{
    i32 id = tree.GetCursor();
    if (id >= 0) {
        i32 p = tree.GetParent(id);
        if (p != 0) {
            menu.WindowMenu(bar, dockers[(i32)tree.Get(id)]);
            bar.Separator();
        }
        menu.GroupMenu(bar, (Txt)tree.GetVal((p==0) ? id : p));
        if (p == 0 && id != all) {
            bar.Separator();
            bar.Add(t_("Удалить группу"), OTVET1(DeleteGroup, id));
        }
    }
}

void DockConfigDlg::OnSaveLayout()
{
    i32 ix = ListIndex();
    Txt s = (ix >= 0) ? (Txt)list.Get(ix) : Null;
    if (EditText(s, t_("Новая выкладка"), t_("Имя выкладки:"), 25)) {
        if (!s.IsEmpty()) {
            ix = dock.GetLayouts().Find(s);
            if (ix < 0 || PromptOKCancel(Format(t_("Переписать выкладку '%s'?"), s))) {
                dock.SaveLayout(s);
                if (ix < 0) {
                    list.Add(s);
                    list.SetCursor(list.GetCount()-1);
                    OnListCursor();
                }
            }
        }
    }
}

void DockConfigDlg::OnLoadLayout()
{
    Ctrl *focus = GetFocusCtrl();
    i32 ix = ListIndex();
    ASSERT(ix >= 0);
    dock.LoadLayout((Txt)list.Get(ix));
    dock.DisableFloating();
    if (focus) focus->SetFocus();
    RefreshTree(true);
}

void DockConfigDlg::OnDeleteLayout()
{
    i32 ix = ListIndex();
    if (!PromptOKCancel(Format(t_("Удалить выкладку '%s'?"), (Txt)list.Get(ix)))) return;
    dock.DeleteLayout((Txt)list.Get(ix));
    list.Remove(ix);
}

void DockConfigDlg::OnTreeCursor()
{
    i32 id = tree.GetCursor();
    if (id <= 0) { deletegroup.Disable(); return; }
    i32 p = tree.GetParent(id);
    if (p == 0)
        deletegroup.Enable();
    else
        Highlight(dockers[(i32)tree.Get(id)]);
}


void DockConfigDlg::OnListCursor()
{
    loadlayout.Enable(list.GetCursor() >= 0);
    deletelayout.Enable(list.GetCursor() >= 0);
}

void DockConfigDlg::OnNewGroup()
{
    Txt s;
    if (EditText(s, t_("Новая группа"), t_("Имя группы:"), 25)) {
        if (!s.IsEmpty()) {
            if (groups.Find(s) < 0) {
                i32 id = tree.Add(0, Image(), Val(-1), Val(s));
                groups.Add(s, id);
                tree.SetCursor(list.GetCount()-1);
                OnTreeCursor();
            }
            else {
                PromptOK(t_("Группа '%s' уже существует."));
                OnNewGroup();
            }
        }
    }
}

void DockConfigDlg::OnDeleteGroup()
{
    i32 id = tree.GetCursor();
    if (id < 0) return;
    i32 p = tree.GetParent(id);
    if (p != 0) id = p;
    if (id == all) return;
    DeleteGroup(id);
}

void DockConfigDlg::DeleteGroup(i32 id)
{
    Txt s = (Txt)tree.GetVal(id);
    if (!PromptOKCancel(Format(t_("Удалить группу '%s'?"), s))) return;
    i32 ix = groups.Find(s);
    if (ix >= 0) {
        Txt g = Null;
        Txt name = groups.GetKey(ix);
        for (i32 i = 0; i < dockers.GetCount(); i++)
            if (dockers[i]->GetGroup() == name)
                dockers[i]->SetGroup(g);
        tree.RemoveChildren(id);
        tree.Remove(id);
    }
}

void DockConfigDlg::OnOK()
{
    dock.Animate(~animatehighlight, ~animateframes, ~animatewindows);
    dock.Tabbing(~tabbing);
    dock.NestedTabs(~tabnesting);
    dock.AutoHide(~autohide);
    dock.Grouping(~grouping);
    dock.WindowButtons(~menubtn, ~autohidebtn, ~closebtn);
    Close();
    dock.EnableFloating();
}

void DockConfigDlg::OnCancel()
{
    // Restore layout
    TxtStream s(backup);
    dock.SerializeLayout(s, true);
    Close();
}

void DockConfigDlg::OnLock()
{
    dock.LockLayout(locked);
}

void DockConfigDlg::OnTreeDrag()
{
    if (tree.GetCursor() >= 0 && tree.GetParent(tree.GetCursor()) == 0)
        return;
    tree.DoDragAndDrop(InternalClip(tree, "dockwindowdrag"), tree.GetDragSample(), DND_MOVE);
}

void DockConfigDlg::OnTreeDrop(i32 parent, i32 ii, PasteClip& d)
{
    if (parent == 0)
        return d.Reject();
    i32 p = tree.GetParent(parent);
    if (p != 0)
        parent = p;

    if(AcceptInternal<TreeCtrl>(d, "dockwindowdrag")) {
        Vec<i32> sel = tree.GetSel();
        for (i32 i = 0; i < sel.GetCount(); i++) {
            i32 ix = tree.Get(sel[i]);
            ASSERT(ix >= 0);
            dockers[ix]->SetGroup((parent == all) ? Null : (Txt)tree.GetVal(parent));
        }
        if (tree.GetCursor()) {
            i32 ix = tree.Get(tree.GetCursor());
            ASSERT(ix >= 0);
            dockers[ix]->SetGroup((parent == all) ? Null : (Txt)tree.GetVal(parent));
        }
        tree.SetFocus();
        RefreshTree();
        return;
    }
}

void DockConfigDlg::Highlight(DockableCtrl *dc)
{
    if (dc != highlight) {
        if (DockCont *cont = dynamic_cast<DockCont *>(dc->GetParent())) {
            if (!highlight || cont != highlight->GetParent()) {
                StopHighlight();
                cont->Highlight();
            }
        }
        else {
            StopHighlight();
            dc->Highlight();
        }
        highlight = dc;
    }
    KillSetTimeCallback(HIGHLIGHT_DURATION, OTVET(StopHighlight), 0);
}

void DockConfigDlg::StopHighlight()
{
    if (highlight) {
        if (DockCont *dc = dynamic_cast<DockCont *>(highlight->GetParent()))
            dc->Refresh();
        else
            highlight->Refresh();
    }
    highlight = NULL;
}

i32 DockConfigDlg::ListIndex() const
{
    if (!list.IsSelection()) {
        if (list.IsCursor())
            return list.GetCursor();
        else
            return -1;
    }
    for (i32 i = 0; i < list.GetCount(); i++)
        if (list.IsSelected(i))
            return i;
    return -1;
}


Txt DockConfigDlg::DockerTxt(DockableCtrl *dc) const
{
    return Format("%20<s %s", dc->GetTitle(), PositionTxt(dc));
}

Txt DockConfigDlg::PositionTxt(DockableCtrl *dc) const
{
    if (dc->IsFloating())
        return t_("Плавающая");
    else if (dc->IsTabbed())
        return t_("С вкладками");
    else if (dc->IsDocked()) {
        i32 align = dc->GetDockAlign();
        switch (align) {
        case DockWindow::DOCK_LEFT:
            return Format(t_("С доком(%s)"), t_("Слева"));
        case DockWindow::DOCK_TOP:
            return Format(t_("С доком(%s)"), t_("Вверху"));
        case DockWindow::DOCK_RIGHT:
            return Format(t_("С доком(%s)"), t_("Справа"));
        case DockWindow::DOCK_BOTTOM:
            return Format(t_("С доком(%s)"), t_("Внизу"));
        default:
            return t_("С доком");
        }
    }
    else if (dc->IsAutoHide())
        return t_("Автоскрывание");
    else
        return t_("Скрытая");
}

}
