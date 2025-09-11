#include "CtrlLib.h"

namespace drx {

void SuggestCtrl::CancelMode()
{
    Cancel();
}

i32 SuggestCtrl::IsDelimiter(i32 c)
{
    if(delimiter_filter || delimiter_char) {
        if(delimiter_filter)
            c = (*delimiter_filter)(c);
        if(delimiter_char && delimiter_char != c)
            c = 0;
        return c;
    }
    return 0;
}

WTxt SuggestCtrl::CF(const WTxt& src)
{
    return compare_filter ? DRX::Filter(src, compare_filter) : src;
}

WTxt SuggestCtrl::ReadLast(i32& h)
{
    i32 l;
    GetSelection(l, h);
    if(l != h)
        return Null;
    WTxt text = GetText();
    WTxt x;
    while(--l >= 0 && !IsDelimiter(text[l])) {
        h = l;
        x.Insert(0, text[l]);
    }
    return drx::TrimLeft(x);
}

void SuggestCtrl::Select()
{
    if(list.IsCursor()) {
        i32 h;
        WTxt x = ReadLast(h);
        i32 q;
        if(GetSelection(q, q))
            return;
        Remove(h, q - h);
        x = list.GetKey();
        Insert(h, x);
        h += x.GetCount();
        SetSelection(h, h);
        Cancel();
        Action();
    }
}

bool SuggestCtrl::Key(dword key, i32 count)
{
    if(list.IsOpen()) {
        if(key == K_UP || key == K_PAGEUP || key == K_CTRL_PAGEDOWN) {
            if(list.IsCursor())
                return list.Key(key, count);
            else {
                list.GoEnd();
                return true;
            }
        }
        if(key == K_DOWN || key == K_PAGEDOWN || key == K_CTRL_PAGEUP) {
            if(list.IsCursor())
                return list.Key(key, count);
            else {
                list.GoBegin();
                return true;
            }
        }
    }
    if(key == K_ENTER && list.IsOpen() && list.IsCursor()) {
        Select();
        return true;
    }
    if(key == K_ESCAPE && list.IsOpen()) {
        Close();
        return true;
    }
    i32 cc = IsDelimiter(key);
    if(cc)
        key = cc;
    if(EditTxt::Key(key, count)) {
        if(key >= 32 && key < K_CHAR_LIM || key == K_BACKSPACE || key == K_CTRL_SPACE) {
            i32 h;
            WTxt x = CF(ReadLast(h));
            list.Clear();
            for(i32 i = 0; i < data.GetCount(); i++) {
                WTxt h = CF(data[i]);
                if(just_start ? h.StartsWith(x) : h.Find(x) >= 0)
                    list.Add(data[i]);
            }
            if(list.GetCount() == 0) {
                Cancel();
                return true;
            }
            Rect cr = GetCaretRect(h) + GetScreenView().TopLeft();
            Rect wr = GetWorkArea();
            i32 c = droplines * Draw::GetStdFontCy();
            Rect r = Rect(cr.BottomLeft(), Size(c, c));
            r.right = GetScreenRect().right;
            if(r.bottom > wr.bottom) {
                r.top = cr.top - c;
                r.bottom = r.top + c;
            }
            if(r.right > wr.right) {
                r.left = cr.left - c;
                r.right = r.left + c;
            }
            list.SetRect(r);
            if(!list.IsOpen())
                list.Ctrl::PopUp(GetParent(), false, false, true);
        }
        else
            Cancel();
        return true;
    }
    return false;
}

void SuggestCtrl::Cancel()
{
    if(list.IsOpen())
        list.Close();
}

void SuggestCtrl::LostFocus()
{
    EditField::LostFocus();
    Cancel();
}

void SuggestCtrl::GotFocus()
{
    EditField::GotFocus();
    Move(GetLength());
}

SuggestCtrl::SuggestCtrl()
{
    list.AddColumn();
    list.NoHeader().NoGrid();
    list.SetFrame(BlackFrame());
    list.MouseMoveCursor();
    list.WhenLeftClick = OTVET(Select);
    list.NoWantFocus();
    delimiter_char = 0;
    delimiter_filter = NULL;
    droplines = 16;
    compare_filter = NULL;
    just_start = false;
}

}