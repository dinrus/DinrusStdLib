// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifdef flagGUI

#include <X/tif/tif.h>
#include <X/jpg/jpg.h>
#include <X/gif/gif.h>
#include <X/bmp/bmp.h>

#include <drx/CtrlLib/CtrlLib.h>
#include <drx/Funcs4U/Funcs4U_Gui.h>

namespace drx {

Image NativePathIconX(tukk path, bool folder, i32 flags)
{
    if (!(flags & BROWSE_LINKS))
        return NativePathIcon(path, folder);
    if (!IsSymLink(path))
        return NativePathIcon(path, folder);

    Txt linkPath = GetSymLinkPath(path);
    if (linkPath.IsEmpty())
        linkPath = path;

    static Image ilink = Null;

    if (!ilink) {
        ImageDraw drw(8, 8);

        Rect r(0, 0, 8, 8);

        drw.DrawRect(r, White());
        drw.DrawImage(r, CtrlImg::smallright);
        drw.DrawLine(r.left, r.top, r.right-1, r.top, 1, GrayColor(100));
        drw.DrawLine(r.right-1, r.top, r.right-1, r.bottom-1, 1, GrayColor(100));
        drw.DrawLine(r.right-1, r.bottom-1, r.left, r.bottom-1, 1, GrayColor(100));
        drw.DrawLine(r.left, r.bottom-1, r.left, r.top, 1, GrayColor(100));

        ilink = drw;
    }

    Image img = NativePathIcon(linkPath, DirExists(linkPath));
    i32 w = img.GetWidth();
    i32 h = img.GetHeight();

    ImageDraw drw(w, h);

    drw.Alpha().DrawImage(0, 0, img, GrayColor(255));
    drw.DrawImage(0, 0, img);

    i32 ypos = img.GetHeight() - ilink.GetHeight();
    drw.Alpha().DrawImage(0, ypos, ilink, GrayColor(255));
    drw.DrawImage(0, ypos, ilink);

    return drw;
}

bool SaveImage(const Image &img, i32 qualityBpp, const Txt &fileName, Txt ext) {
    if(!img)
        return false;

    if (ext == "")
        ext = GetFileExt(fileName);

    if (ext == ".jpg" || ext == ".jpeg") {
        JPGEncoder jpg;
        if (IsNull(qualityBpp))
            qualityBpp = 85;
        else {
            if (qualityBpp > 100)
                qualityBpp = 100;       // Max 100
        }
        jpg.Quality(qualityBpp);
        jpg.SaveFile(fileName, img);
    } else if (ext == ".gif") {
        GIFEncoder gif;
        gif.SaveFile(fileName, img);
    } else if (ext == ".tif" || ext == ".tiff") {
        TIFEncoder tif;
        if (IsNull(qualityBpp))
            qualityBpp = 24;
        tif.Bpp(qualityBpp);    //1, 2, 4, 8, 24, 32
        tif.SaveFile(fileName, img);
    } else if (ext == ".bmp") {
        BMPEncoder bmp;
        if (IsNull(qualityBpp))
            qualityBpp = 24;
        bmp.Bpp(qualityBpp);    //1, 4, 8, 24, 32   and Bpp(4).Mono(); Bpp(8).Mono();
        bmp.SaveFile(fileName, img);
    } else if (ext == ".png") {
        PNGEncoder png;
        if (IsNull(qualityBpp))
            qualityBpp = 24;
        png.Bpp(qualityBpp);    //1, 4, 8, 24, 32   and only with 8 png.Interlace();
        png.SaveFile(fileName, img);
    } else
        return false;
    return true;
}

bool PrintImage(const Image &img, i32 x, i32 y, i32 width, i32 height) {
    if(!img)
        return false;

    PrinterJob pd(t_("Печатается документ"));
    if(!pd.Execute())
        return false;

    Draw& w = pd.GetDraw();
    w.StartPage();
    if (IsNull(width))
        w.DrawImage(x, y, img);
    else
        w.DrawImage(x, y, width, height, img);
    w.EndPage();

    return true;
}

void DrawRectLine(Draw& w, i32 x, i32 y, i32 width, i32 height, i32 lineWidth, const Color &color) {
    w.DrawLine(x, y, x+width, y, lineWidth, color);
    w.DrawLine(x+width, y, x+width, y+height, lineWidth, color);
    w.DrawLine(x+width, y+height, x, y+height, lineWidth, color);
    w.DrawLine(x, y+height, x, y, lineWidth, color);
}

void DrawRectLine(Draw& w, Point &pos, const Size &s, i32 lineWidth, const Color &color) {
    w.DrawLine(pos.x, pos.y, pos.x+s.cx, pos.y, lineWidth, color);
    w.DrawLine(pos.x+s.cx, pos.y, pos.x+s.cx, pos.y+s.cy, lineWidth, color);
    w.DrawLine(pos.x+s.cx, pos.y+s.cy, pos.x, pos.y+s.cy, lineWidth, color);
    w.DrawLine(pos.x, pos.y+s.cy, pos.x, pos.y, lineWidth, color);
}

void DrawRectLine(Draw& w, Rect &r, i32 lineWidth, const Color &color) {
    w.DrawLine(r.left, r.top, r.right, r.top, lineWidth, color);
    w.DrawLine(r.right, r.top, r.right, r.bottom, lineWidth, color);
    w.DrawLine(r.right, r.bottom, r.left, r.bottom, lineWidth, color);
    w.DrawLine(r.left, r.bottom, r.left, r.top, lineWidth, color);
}

i32 GetEditWidth(const Txt &_str, const Font font = StdFont()) {
    WTxt str(_str);
    i32 ret = 0;
    for (i32 i = 0; i < str.GetCount(); ++i)
        ret += font.GetWidth(str[i]);
    return ret;
}

Size GetEditSize(const Txt &_str, const Font font = StdFont()) {
    WTxt str(_str);
    Size ret(0, 0);
    i32 retx = 0, nlines = 1;
    for (i32 i = 0; i < str.GetCount(); ++i) {
        i32 c = str[i];
        if (c == '\n') {
            nlines++;
            ret.cx = max(ret.cx, retx);
            retx = 0;
        } else
            retx += font.GetWidth(c);
    }
    ret.cx = max(ret.cx, retx);
    ret.cy = nlines*font.GetHeight();
    return ret;
}

bool ConsoleOutput::Init(bool forceWindow) {
#ifdef TARGET_WIN32
    ownConsole = true;
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        if (GetLastError() != ERROR_ACCESS_DENIED) {
            if (!AttachConsole(GetCurrentProcessId())) {
                if (GetLastError() != ERROR_ACCESS_DENIED) {
                    ownConsole = false;
                    if (!forceWindow)
                        return false;

                    if (!AllocConsole())
                        return false;
                    AttachConsole(GetCurrentProcessId()) ;
                }
            }
        }
    }
    freopen("CONIN$",  "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    return true;
#else
    if (forceWindow)
        return false;
    return true;
#endif
}

#ifdef TARGET_WIN32
ConsoleOutput::~ConsoleOutput() {
    if (ownConsole && (GetConsoleWindow() == GetForegroundWindow())) {
        INPUT input;
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = 0;
        input.ki.time = 0;
        input.ki.dwExtraInfo = 0;
        input.ki.wVk = K_ENTER;
        input.ki.dwFlags = 0;
        SendInput(1, &input, sizeof(INPUT));
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
}
#endif

void ArrayCtrlWhenBar(Bar &menu, ArrayCtrl &array, bool header, bool edit) {
    i32 num = array.GetCount();
    if (num == 0)
        menu.Add(t_("Пустой список"), Null, Null).Enable(false).Bold(true);
    else {
        menu.Add(t_("Выбрать все"), Null, [&] {ArrayCtrlRowSelect(array);})
            .Key(K_CTRL_A).Help(t_("Выбрать все ряды"));

        i32 count = array.GetSelectCount();
        if (count == 0)
            menu.Add(t_("Ряд не выделен"), Null, Null).Enable(false).Bold(true);
        else {
            menu.Add(Format(t_("Выделено %d рядов"), count), Null, Null).Enable(false).Bold(true);
            menu.Add(t_("Копировать"), Null, [&] {ArrayCtrlRowCopy(array, header);})
                .Key(K_CTRL_C).Help(t_("Копировать выделенные ряды в буфер обмена"));
        }
    }
    if (edit) {
            menu.Add(t_("Вставить"), Null, [&] {ArrayCtrlRowPaste(array);})
                .Key(K_CTRL_V).Help(t_("Вставить ряды из буфера обмена"));
            menu.Add(t_("Приставить"), Null, [&] {array.Add();})
                .Key(K_CTRL_INSERT).Help(t_("Приставить ряд"));
            if (num > 0) {
                menu.Add(t_("Удалить"), Null, [&] {
                    for (i32 r = array.GetCount()-1; r >= 0; --r)
                        if (array.IsSelected(r))
                            array.Remove(r);
                }).Key(K_DELETE).Help(t_("Удалить выделенные ряды"));
                menu.Add(t_("Удалить все"), Null, [&] {
                    for (i32 r = array.GetCount()-1; r >= 0; --r)
                        array.Remove(r);
                }).Help(t_("Удалить все ряды"));
            }
    }
}

void ArrayCtrlRowCopy(const ArrayCtrl &array, bool header) {
    WaitCursor wait;
    array.SetClipboard(true, header);
}

void ArrayCtrlRowPaste(ArrayCtrl &array) {
    WaitCursor wait;
    Txt str = ReadClipboardText();
    i32 numcol = array.GetColumnCount();
    Vec<Txt> split = Split(str, "\r\n");
    for (i32 r = 0; r < split.GetCount(); ++r) {
        array.Add();
        i32 rr = array.GetCount()-1;
        Vec<Txt> cells = Split(split[r], "\t");
        i32 mincol = min(cells.GetCount(), numcol);
        for (i32 c = 0; c < mincol; ++c)
            array.Set(rr, c, cells[c]);
    }
}

void ArrayCtrlRowSelect(ArrayCtrl &array) {
    array.Select(0, array.GetCount(), true);
}

Vec<i32> ArrayCtrlSelectedGet(const ArrayCtrl &array) {
    Vec<i32> selected;
    for (i32 r = 0; r < array.GetCount(); ++r) {
        if (array.IsSelected(r))
            selected << r;
    }
    return selected;
}

i32 ArrayCtrlSelectedGetCount(const ArrayCtrl &array) {
    i32 num = 0;
    for (i32 r = 0; r < array.GetCount(); ++r) {
        if (array.IsSelected(r))
            num++;
    }
    return num;
}

Vec<Vec<Val>> ArrayCtrlGet(const ArrayCtrl &arr) {
    Vec<Vec<Val>> ret;
    for (i32 row = 0; row < arr.GetCount(); ++row) {
        Vec<Val> &rowvals = ret.Add();
        rowvals = arr.ReadRow(row);
    }
    return ret;
}

void ArrayCtrlSet(ArrayCtrl &arr, const Vec<Vec<Val>> &vals, i32 fromRow, i32 fromCol) {
    for (i32 row = 0; row < vals.GetCount(); ++row) {
        const Vec<Val> &rowvals = vals[row];
        for (i32 col = 0; col < rowvals.GetCount(); ++col)
            arr.Set(row + fromRow, col + fromCol, rowvals[col]);
    }
}

}

#endif