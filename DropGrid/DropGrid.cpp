#include "DropGrid.h"

namespace drx {

DropGrid::PopUpGrid::PopUpGrid()
{
    LiveCursor();
    FullColResizing(false);
    HorzGrid(false);
    DrawLastVertLine(false);
    TabAddsRow(false);
    SearchMoveCursor(true);
    SearchHideRows(false);
    SearchDisplay(false);
    ResizePanel();
    WhenLeftClick = OTVET(CloseData);
    WhenEnter = OTVET(CloseData);
    WhenEscape = OTVET(CloseNoData);
    GridCtrl::WhenClose = OTVET(CloseNoData);
}

void DropGrid::PopUpGrid::PopUp(Ctrl *owner, const Rect &r)
{
    Close();
    SetRect(r);
    Ctrl::PopUp(owner, true, true, GUI_DropShadows());
    SetFocus();
}

void DropGrid::PopUpGrid::CloseNoData()
{
    WhenCloseNoData();
    Deactivate();
}

void DropGrid::PopUpGrid::CloseData()
{
    WhenCloseData();
    Deactivate();
}

void DropGrid::PopUpGrid::Deactivate()
{
    if(IsOpen() && IsPopUp())
    {
        WhenPopDown();
        IgnoreMouseClick();
        GridCtrl::Close();
        WhenClose();
    }
}

DropGrid::DropGrid()
{
    list.WhenCloseData = OTVET(CloseData);
    list.WhenCloseNoData = OTVET(CloseNoData);
    list.WhenClose = OTVET(Close);
    list.WhenSearchCursor = OTVET(SearchCursor);
    list.BackPaint();
    drop.AddButton().Main().WhenPush = OTVET(Drop);
    drop.SetStyle(drop.StyleFrame());
    drop.NoDisplay();
    drop.AddTo(*this);
    list_width = 0;
    list_height = 0;
    drop_lines = 16;
    display_all = false;
    header = true;
    valuekey = false;
    key_col = 0;
    find_col = 0;
    value_col = -1;
    rowid = -1;
    trowid = -2;
    notnull = false;
    display_columns = true;
    drop_enter = false;
    data_action = false;
    Searching(true);
    must_change = false;
    null_action = true;
    display = this;
    change = false;
    nodrop = false;
    clear_button = false;

    must_change_str = t_("Выберите значение.");

    clear.SetButton(1);
    clear <<= OTVET(DoClearVal);
}

void DropGrid::Close()
{
    Rect r = list.GetRect();
    list_width = r.Width();
    list_height = r.Height();
    SetFocus();
}

void DropGrid::CloseData()
{
    UpdateVal();
    DoAction(list.GetCursor());
}

void DropGrid::CloseNoData()
{
    if(list.GetRowId() != rowid)
        list.SetCursorId(rowid);
}

void DropGrid::Drop()
{
    if(!IsEditable())
        return;

    WhenDrop();

    GridDisplay &dsp = list.GetDisplay();
    if(!header)
    {
        list.HideRow(0, false);
        dsp.SetHorzMargin(2, 2);
    }
    else
        dsp.SetHorzMargin();

    list.UpdateRows(true); //СДЕЛАТЬ: try to avoid it..

    Rect rs = GetScreenRect();
    i32 width = rs.Width();
    i32 resize_height = list.GetResizePanelHeight();
    i32 list_height = max(list.GetHeight(), list.GD_ROW_HEIGHT + list.GD_HDR_HEIGHT * header);
    i32 height = list_height + 4 + resize_height;
    i32 drop_height = drop_lines * list.GD_ROW_HEIGHT + header * list.GD_HDR_HEIGHT + 4 + resize_height;
    if(!display_all && height > drop_height)
        height = drop_height;

    list.resize_panel.SetMinSize(Size(width, height));

    if(list_width > width)
        width = list_width;
    if(display_all && list_height > height) //check this
        height = list_height;

    Rect rw = Ctrl::GetWorkArea();
    Rect r;
    r.left   = rs.left;
    r.right  = rs.left + width;
    r.top    = rs.bottom;
    r.bottom = rs.bottom + height;

    if(r.bottom > rw.bottom)
    {
        r.top = rs.top - height;
        r.bottom = rs.top;
    }
    if(r.right > rw.right)
    {
        i32 diff;
        if(rs.right <= rw.right)
            diff = rs.right - r.right;
        else
            diff = rw.right - r.right;

        r.left += diff;
        r.right += diff;
    }
    if(r.left < rw.left)
    {
        i32 diff = rw.left - r.left;
        r.left += diff;
        r.right += diff;

    }

    if(searching)
        list.ClearFound();
    list.PopUp(this, r);
    list.CenterCursor();
}


void DropGrid::Paint(Draw& w)
{
    Size sz = GetSize();
    Size isz = clear.GetStdSize();
    if(clear_button && !notnull && IsEnabled() && IsSelected())
    {
        clear.Show();
        clear.RightPos(3, isz.cx).TopPos((sz.cy - isz.cy) / 2, isz.cy);
    }
    else
        clear.Hide();

    bool hf = HasFocus();
    bool isnull = rowid < 0;
    Color fg = hf ? SColorHighlightText() : IsEnabled() ? SColorText() : SColorDisabled();
    Color bg = !IsEnabled() || !IsEditable()
        ? EditField::StyleDefault().disabled
        : notnull && isnull
            ? Blend(SColorPaper, Color(255, 0, 0), 32)
            : hf ? SColorHighlight() : SColorPaper();

    i32k d = 0;

    if(isnull)
        w.DrawRect(d, d, sz.cx - d * 2, sz.cy - d * 2, bg);
    else
    {
        Font fnt(StdFont());
        Paint0(w, 1, 1, d, d, sz.cx - d * 2, sz.cy - d * 2, Format0(Null, rowid), 0, fg, bg, fnt);
    }

    if(hf)
        DrawFocus(w, d - 0, d - 0, sz.cx - (d - 0) * 2, sz.cy - (d - 0) * 2);
}


void DropGrid::LeftDown(Point p, dword keyflags)
{
    WhenLeftDown();
    if(nodrop)
        SetFocus();
    else
        Drop();
}

void DropGrid::GotFocus()
{
    drop.RefreshFrame();
}

void DropGrid::LostFocus()
{
    drop.RefreshFrame();
}

void DropGrid::Serialize(Stream& s)
{
    s % rowid;
    if(s.IsLoading())
        list.SetCursorId(rowid);
}

bool DropGrid::Accept()
{
    if(!Ctrl::Accept())
        return false;
    if(must_change && !change)
    {
        Exclamation(must_change_str);
        SetWantFocus();
        return false;
    }
    return true;
}

Size DropGrid::GetMinSize() const
{
    return drop.GetMinSize();
}

void DropGrid::State(i32 reason)
{
    if(reason == ENABLE)
    {
        bool b = IsEnabled();
        for(i32 i = 0; i < drop.GetButtonCount(); i++)
            drop.GetButton(i).Enable(b);
    }
    Ctrl::State(reason);
}

void DropGrid::Paint0(Draw &w, i32 lm, i32 rm, i32 x, i32 y, i32 cx, i32 cy, const Val &val, dword style, Color &fg, Color &bg, Font &fnt, bool found, i32 fs, i32 fe)
{
    real_size.Clear();

    w.DrawRect(x, y, cx, cy, bg);
    i32 nx = x + lm;
    i32 ny = y + tm;
    i32 ncx = cx - lm - rm;

    if(IsType< Vec<Txt> >(val))
    {
        const Vec<Txt> &v = ValTo< Vec<Txt> >(val);
        tukk  SPACE = " ";

        i32 tcx = 0;
        i32 scx = GetTextSize(SPACE, fnt).cx;

        i32 cnt = v.GetCount();
        Size isz = GridImg::Dots2().GetSize();
        for(i32 i = 0; i < cnt; i++)
        {
            bool iscol = (i + 1) & 1;
            if(!display_columns && iscol)
                continue;
            fnt.Bold(iscol);
            Size tsz = GetTextSize(v[i], fnt);
            DrawText(w, nx, x + lm + tcx,
                     ny, tcx + tsz.cx > ncx - isz.cx ? ncx - tcx: tsz.cx + isz.cx, cy,
                     GD::VCENTER, WTxt(v[i]), fnt, fg, bg, found, fs, fe, false);
            tcx += tsz.cx + scx;
        }
    }
    else
        DrawText(w, nx, nx, ny, ncx, cy, GD::VCENTER, GetStdConvertedVal(val), fnt, fg, bg, found, fs, fe, false);
}


void DropGrid::Paint(Draw &w, i32 x, i32 y, i32 cx, i32 cy, const Val &val, dword style, Color &fg, Color &bg, Font &fnt, bool found, i32 fs, i32 fe)
{
    Paint0(w, lm, rm, x, y, cx, cy, val, style, fg, bg, fnt, found, fs, fe);
}

DropGrid& DropGrid::Width(i32 w)
{
    list_width = Ctrl::VertLayoutZoom(w);
    return *this;
}

DropGrid& DropGrid::Height(i32 h)
{
    list_height = Ctrl::VertLayoutZoom(h);
    return *this;
}

DropGrid& DropGrid::SetKeyColumn(i32 n)
{
    key_col = find_col = n;
    return *this;
}

DropGrid& DropGrid::SetFindColumn(i32 n)
{
    find_col = n;
    return *this;
}

DropGrid& DropGrid::SetValColumn(i32 n)
{
    value_col = n;
    return *this;
}

DropGrid& DropGrid::AddValColumn(i32 n)
{
    value_cols.Add(n);
    return *this;
}

DropGrid& DropGrid::AddValColumns(i32 first /* = -1*/, i32 last /* = -1*/)
{
    i32 s = first < 0 ? 0: first;
    i32 e = last < 0 ? list.GetColumnCount() - 1: last;

    for(i32 i = s; i <= e; i++)
         value_cols.Add(i);

    return *this;
}

DropGrid& DropGrid::DisplayAll(bool b)
{
    display_all = b;
    return *this;
}

DropGrid& DropGrid::SetDropLines(i32 d)
{
    drop_lines = d;
    return *this;
}

DropGrid& DropGrid::Header(bool b /* = true*/)
{
    header = b;
    return *this;
}

DropGrid& DropGrid::NoHeader()
{
    return Header(false);
}

DropGrid& DropGrid::Resizeable(bool b /* = true*/)
{
    list.ResizePanel(b);
    return *this;
}

DropGrid& DropGrid::ColorRows(bool b)
{
    list.ColorRows(b);
    return *this;
}

DropGrid& DropGrid::NotNull(bool b)
{
    notnull = b;
    return *this;
}

DropGrid& DropGrid::ValAsKey(bool b)
{
    valuekey = b;
    return *this;
}

DropGrid& DropGrid::SetDisplay(GridDisplay &d)
{
    display = &d;
    return *this;
}

DropGrid& DropGrid::DisplayColumns(bool b /* = true*/)
{
    display_columns = b;
    return *this;
}

DropGrid& DropGrid::DropEnter(bool b /* = true*/)
{
    drop_enter = b;
    return *this;
}

DropGrid& DropGrid::DataAction(bool b /* = true*/)
{
    data_action = b;
    return *this;
}

DropGrid& DropGrid::Searching(bool b /* = true*/)
{
    searching = b;
    list.Searching(b);
    return *this;
}

DropGrid& DropGrid::MustChange(bool b /* = true*/, tukk s /* = ""*/)
{
    must_change = b;
    if(s)
        must_change_str = s;
    return *this;
}

DropGrid& DropGrid::NullAction(bool b /* = true*/)
{
    null_action = b;
    return *this;
}

DropGrid& DropGrid::ClearButton(bool b /* = true*/)
{
    clear_button = b;
    if(b)
        Ctrl::Add(clear);
    else
        Ctrl::RemoveChild(&clear);

    return *this;
}

DropGrid& DropGrid::NoDrop(bool b /* = true*/)
{
    nodrop = b;
    if(nodrop)
        drop.RemoveButton(0);
    return *this;
}

i32 DropGrid::GetCount() const
{
    return list.GetCount();
}

Val DropGrid::GetData() const
{
    return valuekey
        ? value
        : rowid >= 0
            ? list.Get(key_col)
            : notnull
                ? NotNullError()
                : Null;
}

Val DropGrid::GetVal() const
{
    return value;
}

Val DropGrid::GetVal(i32 r) const
{
    return MakeVal(r);
}

Val DropGrid::FindVal(const Val& v) const
{
    i32 r = list.Find(v, key_col);
    if(r < 0)
        return Null;

    return MakeVal(r);
}

Vec<Txt> DropGrid::FindVector(const Val& v) const
{
    i32 r = list.Find(v, key_col);
    if(r < 0)
        return Vec<Txt>();

    return MakeVector(r);
}

bool DropGrid::FindMove(const Val& v)
{
    i32 r = list.Find(v, key_col);
    if(r >= 0)
        list.Move(r);
    return r >= 0;
}

Val DropGrid::GetKey() const
{
    return rowid >= 0 ? list.Get(key_col) : Null;
}

void DropGrid::UpdateVal()
{
    if(!list.IsCursor())
        return;

    value = MakeVal();
}

void DropGrid::SetData(const Val& v)
{
    i32 row = list.Find(v, key_col);
    if(row >= 0)
    {
        list.SetCursor(row);
        UpdateVal();
        DoAction(row, data_action, false);
        Refresh();
    }
    else
        ClearVal();
}

DropGrid& DropGrid::SearchHideRows(bool b)
{
    list.SearchHideRows(b);
    return *this;
}

void DropGrid::DoAction(i32 row, bool action, bool chg)
{
    i32 rid = list.GetRowId(row);
    if(rid != (trowid >= -1 ? trowid : rowid))
    {
        change = chg;
        rowid = rid;
        trowid = -2;
        Update();
        if(action)
            Action();
    }
}

GridCtrl::ItemRect& DropGrid::AddColumn(tukk name, i32 width, bool idx)
{
    return list.AddColumn(name, width, idx);
}

GridCtrl::ItemRect& DropGrid::AddColumn(Id id, tukk name, i32 width, bool idx)
{
    return list.AddColumn(id, name, width, idx);
}

GridCtrl::ItemRect& DropGrid::AddIndex(tukk name)
{
    return list.AddIndex(name);
}

GridCtrl::ItemRect& DropGrid::AddIndex(Id id)
{
    return list.AddIndex(id);
}

MultiButton::SubButton& DropGrid::AddButton(i32 type, const Callback &cb)
{
    MultiButton::SubButton& btn = drop.InsertButton(1);

    Image img;
    switch(type)
    {
        case BTN_PLUS:
            img = GridImg::SelPlus;
            break;
        case BTN_SELECT:
            img = GridImg::SelDots;
            break;
        case BTN_LEFT:
            img = GridImg::SelLeft;
            break;
        case BTN_RIGHT:
            img = GridImg::SelRight;
            break;
        case BTN_UP:
            img = GridImg::SelUp;
            break;
        case BTN_DOWN:
            img = GridImg::SelDn;
            break;
        case BTN_CLEAN:
            img = GridImg::SelCross;
            break;
    }
    btn.SetImage(img);
    btn.SetMonoImage(Grayscale(img));
    btn.WhenPush = cb;
    return btn;
}

MultiButton::SubButton& DropGrid::AddSelect(const Callback &cb)
{
    return AddButton(BTN_SELECT, cb);
}

MultiButton::SubButton& DropGrid::AddPlus(const Callback &cb)
{
    return AddButton(BTN_PLUS, cb);
}

MultiButton::SubButton& DropGrid::AddEdit(const Callback &cb)
{
    return AddButton(BTN_RIGHT, cb);
}

MultiButton::SubButton& DropGrid::AddClear()
{
    return AddButton(BTN_CLEAN, OTVET(ClearVal));
}

MultiButton::SubButton& DropGrid::AddText(tukk label, const Callback& cb)
{
    MultiButton::SubButton& btn = drop.InsertButton(1);
    btn.SetLabel(label);
    btn.WhenPush = cb;
    return btn;
}

MultiButton::SubButton& DropGrid::GetButton(i32 n)
{
    return drop.GetButton(n);
}

void DropGrid::DoClearVal()
{
    ClearVal();
    SetFocus();
}

void DropGrid::ClearVal()
{
    change = false;
    value = Null;
    rowid = -1;
    list.ClearCursor();
    if(null_action)
        UpdateActionRefresh();
    else
        UpdateRefresh();
}

void DropGrid::Reset()
{
    list.Reset();
    ClearVal();
    value_cols.Clear();
}

void DropGrid::Clear()
{
    list.Clear();
    ClearVal();
}

void DropGrid::Ready(bool b /* = true*/)
{
    list.Ready(b);
}

Val DropGrid::Get(i32 c) const
{
    return list.Get(c);
}

Val DropGrid::Get(Id id) const
{
    return list.Get(id);
}

Val DropGrid::Get(i32 r, i32 c) const
{
    return list.Get(r, c);
}

Val DropGrid::Get(i32 r, Id id) const
{
    return list.Get(r, id);
}

Val DropGrid::GetPrev(i32 c) const
{
    i32 r = list.GetPrevCursor();
    return r >= 0 ? Get(r, c) : Null;
}

Val DropGrid::GetPrev(Id id) const
{
    i32 r = list.GetPrevCursor();
    return r >= 0 ? Get(r, id) : Null;
}

void DropGrid::Set(i32 c, const Val& v)
{
    list.Set(c, v);
}

void DropGrid::Set(Id id, const Val& v)
{
    list.Set(id, v);
}

void DropGrid::Set(i32 r, i32 c, const Val& v)
{
    list.Set(r, c, v);
}

void DropGrid::Set(i32 r, Id id, const Val& v)
{
    list.Set(r, id, v);
}

void DropGrid::Set(i32 r, const Vec<Val> &v, i32 data_offset, i32 column_offset)
{
    list.Set(r, v, data_offset, column_offset);
}

void DropGrid::Add(const Vec<Val> &v, i32 offset, i32 count, bool hidden)
{
    list.Add(v, offset, count, hidden);
}

Txt DropGrid::GetTxt(Id id)
{
    return list.GetTxt(id);
}

Val& DropGrid::operator() (i32 r, i32 c)
{
    return list(r, c);
}

Val& DropGrid::operator() (i32 c)
{
    return list(c);
}

Val& DropGrid::operator() (Id id)
{
    return list(id);
}

Val& DropGrid::operator() (i32 r, Id id)
{
    return list(r, id);
}

GridCtrl::ItemRect& DropGrid::GetRow(i32 r)
{
    return list.GetRow(r);
}

i32 DropGrid::Find(const Val& v, i32 col, i32 opt)
{
    return list.Find(v, col, 0, opt);
}

i32 DropGrid::Find(const Val& v, Id id, i32 opt)
{
    return list.Find(v, id, opt);
}

i32 DropGrid::GetCurrentRow() const
{
    return list.GetCurrentRow();
}

void DropGrid::CancelUpdate()
{
    i32 prevrow = list.GetPrevCursor();
    if(prevrow >= 0)
    {
        list.SetCursor(prevrow);
        UpdateVal();
        rowid = list.GetRowId(prevrow);
        Refresh();
    }
    else
        ClearVal();
}

void DropGrid::Change(i32 dir)
{
    i32 c = -1;
    if(!list.IsCursor())
    {
        if(dir < 0)
            list.GoEnd();
        else
            list.GoBegin();

        c = list.GetCursor();
    }
    else
        c = list.GetCursor() + dir;

    if(list.IsValidCursor(c) && list.IsRowClickable(c))
    {
        list.SetCursor(c);
        UpdateVal();
        DoAction(c);
    }

    Refresh();
}

void DropGrid::SearchCursor()
{
    if(!list.IsCursor())
        return;

    if(trowid < -1)
        trowid = rowid;

    value = list.Get(value_col);
    rowid = list.GetRowId();
    Refresh();
}

bool DropGrid::Key(dword k, i32)
{
    if(IsReadOnly()) return false;

    if(drop_enter && k == K_ENTER)
        k = K_ALT_DOWN;

    switch(k)
    {
        case K_ALT_DOWN:
            Drop();
            break;
        case K_DOWN:
        case K_RIGHT:
            Change(1);
            break;
        case K_UP:
        case K_LEFT:
            Change(-1);
            break;
        case K_DELETE:
            ClearVal();
            break;
        default:
            if(searching && k >= 32 && k < 65536)
            {
                if(!list.IsOpen())
                    Drop();
                list.Search(k);
            }
            return false;
    }
    return true;
}

i32 DropGrid::AddColumns(i32 cnt)
{
    if(!list.GetColumnCount())
    {
        if(cnt <= 2)
        {
            NoHeader();
            Resizeable(false);
        }
        if(cnt == 1)
        {
            list.AddColumn();
            key_col = value_col = 0;
        }
        else if(cnt == 2)
        {
            list.AddIndex();
            list.AddColumn();
            key_col = 0;
            value_col = 1;
        }
        else
            for(i32 i = 0; i < cnt; i++)
                list.AddColumn();
    }
    return list.GetCount();
}

void DropGrid::GoTop()
{
    if(list.IsFilled())
        SetIndex(0);
}

i32 DropGrid::SetIndex(i32 n)
{
    i32 r = rowid;
    n = list.GetRowId(n);
    if(n >= 0)
    {
        list.SetCursor(n);
        UpdateVal();
        DoAction(n);
    }
    return r;
}

i32 DropGrid::GetIndex() const
{
    return rowid;
}

bool DropGrid::IsSelected()
{
    return rowid >= 0;
}

bool DropGrid::IsEmpty()
{
    return list.IsEmpty();
}

bool DropGrid::IsChange()
{
    return change;
}

bool DropGrid::IsInit()
{
    return !change;
}

void DropGrid::ClearChange()
{
    change = false;
}

Vec<Txt> DropGrid::MakeVector(i32 r) const
{
    Vec<Txt> v;
    i32 cnt = value_cols.GetCount();
    i32 fc = list.GetFixedColumnCount();
    if(cnt > 0)
    {
        for(i32 i = 0; i < cnt; i++)
        {
            Val val = list.Get(r, value_cols[i]);
            if(IsNull(val))
                continue;
            v.Add(list.GetColumnName(value_cols[i]));
            v.Add(list.GetStdConvertedColumn(value_cols[i] + fc, val));
        }
    }
    return v;
}

Val DropGrid::MakeVal(i32 r, bool columns) const
{
    if(r < 0)
        r = list.GetCursor();

    if(r < 0)
        return Null;

    i32 cnt = value_cols.GetCount();
    if(cnt > 0)
    {
        Txt v;
        i32 fc = list.GetFixedColumnCount();
        for(i32 i = 0; i < cnt; i++)
        {
            Val val = list.Get(r, value_cols[i]);
            if(IsNull(val))
                continue;
            if(columns)
            {
                v += list.GetColumnName(value_cols[i]);
                v += ' ';
            }
            v += (Txt) list.GetStdConvertedColumn(value_cols[i] + fc, val);
            v += ' ';
        }
        return v;
    }
    else
        return list.Get(r, value_col);
}

Val DropGrid::Format0(const Val& q, i32 rowid) const
{
    i32 r = rowid >= 0 ? list.FindRow(rowid + list.GetFixedCount()) : list.Find(q, key_col);
    if(r < 0)
        return Null;

    if(value_cols.GetCount() > 0)
        return RawPickToVal< Vec<Txt> >(MakeVector(r));
    else
        return list.GetConvertedColumn(value_col + list.GetFixedColumnCount(), list.Get(r, value_col));
}

Val DropGrid::Format(const Val& q) const
{
    return Format0(q, -1);
}

GridCtrl::ItemRect& DropGrid::AddRow(i32 n, i32 size)
{
    return list.AddRow(n, size);
}

DropGrid& DropGrid::AddSeparator(Color c)
{
    list.AddSeparator(c);
    return *this;
}

//$-
#define E__Addv0(I)    list.Set(q, I - 1, p##I)
#define E__AddF0(I) \
DropGrid& DropGrid::Add(__List##I(E__Val)) { \
    i32 q = AddColumns(I); \
    __List##I(E__Addv0); \
    return *this; \
}
__Expand(E__AddF0)

#define E__Addv1(I)    list.Set(q, I - 1, p##I)
#define E__AddF1(I) \
GridCtrl::ItemRect& DropGrid::AddRow(__List##I(E__Val)) { \
    i32 q = AddColumns(I); \
    GridCtrl::ItemRect& ir = list.AddRow(); \
    __List##I(E__Addv1); \
    return ir; \
}
__Expand(E__AddF1)

}
