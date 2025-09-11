#ifndef _VTPage_h_
#define _VTPage_h_

#include <drx/Core/Core.h>
#include "Cell.h"

namespace drx {

class VTLine : public Moveable<VTLine, Vec<VTCell>> {
public:
    VTLine();
    void            Adjust(i32 cx, const VTCell& filler);
    void            ShiftLeft(i32 begin, i32 end, i32 n, const VTCell& filler);
    void            ShiftRight(i32 begin, i32 end, i32 n, const VTCell& filler);
    bool            Fill(i32 begin, i32 end, const VTCell& filler, dword flags = 0);
    bool            FillLeft(i32 begin, const VTCell& filler, dword flags = 0);
    bool            FillRight(i32 begin, const VTCell& filler, dword flags = 0);
    bool            FillLine(const VTCell& filler, dword flags = 0);

    void            Validate(bool b = true)  const          { invalid = !b;    }
    void            Invalidate() const                      { invalid = true;  }
    bool            IsInvalid() const                       { return invalid;  }

    void            Wrap(bool b = true) const               { wrapped = b;     }
    void            Unwrap() const                          { wrapped = false; }
    bool            IsWrapped() const                       { return wrapped;  }

    static const VTLine& Void();
    bool IsVoid() const                                     { return this == &Void(); }

    Txt          ToTxt() const;
    WTxt         ToWTxt() const;

    using Range = SubRangeOf<Vec<VTCell>>;
    using ConstRange = const SubRangeOf<const Vec<VTCell>>;

private:
    mutable bool invalid:1;
    mutable bool wrapped:1;
};

WTxt AsWTxt(VTLine::ConstRange& cellrange, bool tspaces = true);

class VTPage : Moveable<VTPage> {
    struct Cursor
    {
        enum Flags : dword {
            Absolute        = 1 << 0,
            Relative        = 1 << 1,
            Marginal        = 1 << 2,
            Displaceable    = 1 << 3,
            Scroller        = 1 << 4,
            ReWrapper       = 1 << 5
        };

        i32      x, y;
        bool     eol;
        bool     displaced;
        Txt   ToTxt() const;
        void     Clear()                                    { x = y = 1; eol = displaced = false; }
        operator Point() const                              { return Point(x, y); }
        void     operator=(const Point& p)                  { x = p.x; y = p.y;   }
        Point    operator+(i32 n) const                     { return Point(x + n, y + n); }
        Point    operator-(i32 n) const                     { return Point(x - n, y - n); }
        Cursor&  operator++()                               { x++; y++; return *this; }
        Cursor&  operator--()                               { x--; y--; return *this; }
        void     operator=(const Nuller&)                   { Clear(); }
        Cursor()                                            { Clear(); }
        Cursor(i32 col, i32 row)                            { Clear(); x = max(1, col); y = max(1, row); }
    };

public:
    using Lines = Vec<VTLine>;
    using Saved = BiVec<VTLine>;

    VTPage();
    VTPage(Size sz) : VTPage()                              { SetSize(sz); }
    virtual ~VTPage()                                       {}

    Event<>         WhenUpdate;

    VTPage&         Displaced(bool b = true);
    bool            IsDisplaced() const                     { return cursor.displaced; }

    VTPage&         AutoWrap(bool b = true);
    bool            IsAutoWrapping() const                  { return autowrap; }

    VTPage&         ReverseWrap(bool b = true);
    bool            IsReverseWrapping() const               { return reversewrap; }

    VTPage&         History(bool b = true);
    bool            HasHistory() const                      { return history; }
    const Saved&    GetHistory() const                      { return saved;   }
    void            EraseHistory();
    void            SetHistorySize(i32 sz);
    i32             GetHistorySize() const                  { return historysize; };

    VTPage&         Attributes(const VTCell& attrs)         { cellattrs = attrs; return *this; }
    const VTCell&   GetAttributes() const                   { return cellattrs; }

    VTPage&         Reset();

    VTPage&         Backup();
    VTPage&         Discard();
    VTPage&         Restore();

    VTPage&         SetSize(Size sz);
    VTPage&         SetSize(i32 cx, i32 cy)                 { return SetSize(Size(cx, cy)); }
    Size            GetSize() const                         { return size; }

    Point           GetPos() const;
    Point           GetRelPos() const;

    Rect            GetView() const                         { return Rect(1, 1, size.cx, size.cy); }
    bool            ViewContains(const Point& pt) const;
    bool            ViewContains(const Rect& r) const       { return ViewContains(r.TopLeft()) && ViewContains(r.BottomRight()); }

    Rect            GetMargins() const                      { return margins; }
    bool            MarginsContain(const Point& pt) const;
    bool            MarginsContain(const Rect& r) const     { return MarginsContain(r.TopLeft()) && MarginsContain(r.BottomRight()); }

    VTPage&         SetHorzMargins(i32 l, i32 r);
    VTPage&         SetVertMargins(i32 t, i32 b);
    VTPage&         SetMargins(const Rect& r);
    VTPage&         ResetMargins();

    VTPage&         SetCell(i32 x, i32 y, const VTCell& cell);
    VTPage&         SetCell(Point pt, const VTCell& cell)   { return SetCell(pt.x, pt.y, cell); }
    VTPage&         SetCell(const VTCell& cell)             { return SetCell(cursor, cell); }
    const VTCell&   GetCell(i32 x, i32 y) const;
    const VTCell&   GetCell(Point pt) const                 { return GetCell(pt.x, pt.y);   }
    const VTCell&   GetCell() const                         { return GetCell(cursor);       }
    i32             AddCell(const VTCell& cell)             { return CellAdd(cell, cell.GetWidth()); }
    VTPage&         InsertCell(const VTCell& cell);
    VTPage&         RepeatCell(i32 n);

    VTPage&         MoveTo(i32 x, i32 y);
    VTPage&         MoveTo(Point pt)                        { return MoveTo(pt.x, pt.y); }
    VTPage&         MoveToLine(i32 n, bool relative = false);
    VTPage&         MoveToColumn(i32 n, bool relative = false);

    VTPage&         MoveUp(i32 n = 1);
    VTPage&         MoveDown(i32 n = 1);
    VTPage&         MoveRight(i32 n = 1);
    VTPage&         MoveLeft(i32 n = 1);
    VTPage&         MoveHome();
    VTPage&         MoveEnd();
    VTPage&         MoveTopLeft();
    VTPage&         MoveBottomRight();

    VTPage&         EraseLine(dword flags = 0);
    VTPage&         EraseLeft(dword flags = 0);
    VTPage&         EraseRight(dword flags = 0);

    VTPage&         ErasePage(dword flags = 0);
    VTPage&         EraseBefore(dword flags = 0);
    VTPage&         EraseAfter(dword flags = 0);

    VTPage&         ScrollUp(i32 n = 1);
    VTPage&         ScrollDown(i32 n = 1);
    VTPage&         ScrollLeft(i32 n = 1);
    VTPage&         ScrollRight(i32 n = 1);

    VTPage&         PanLeft(i32 n = 1);
    VTPage&         PanRight(i32 n = 1);

    VTPage&         InsertLines(i32 pos, i32 n);
    VTPage&         InsertLines(i32 n)                      { return InsertLines(cursor.y, n); }
    VTPage&         RemoveLines(i32 pos, i32 n);
    VTPage&         RemoveLines(i32 n)                      { return RemoveLines(cursor.y, n); }

    VTPage&         InsertCells(i32 pos, i32 n);
    VTPage&         InsertCells(i32 n)                      { return InsertCells(cursor.x, n); }
    VTPage&         RemoveCells(i32 pos, i32 n);
    VTPage&         RemoveCells(i32 n)                      { return RemoveCells(cursor.x, n); }
    VTPage&         EraseCells(i32 n, dword flags = 0);

    VTPage&         NextLine(i32 n = 1);
    VTPage&         PrevLine(i32 n = 1);
    VTPage&         NewLine(i32 n = 1)                      { return NextLine(n).MoveHome(); }
    VTPage&         NextColumn(i32 n = 1);
    VTPage&         PrevColumn(i32 n = 1);

    VTPage&         FillRect(const Rect& r, const VTCell& filler, dword flags = 0);
    VTPage&         FillRect(const Rect& r, dword chr);
    VTPage&         CopyRect(const Point& pt, const Rect& r, dword flags = 0);
    VTPage&         EraseRect(const Rect& r, dword flags = 0);
    void            GetRectArea(const Rect& r, Event<Point> consumer, bool displaced = false);

    VTPage&         FillStream(const Rect& r, const VTCell& filler, dword flags);
    VTPage&         FillStream(const Rect& r, dword chr);

    VTPage&         AddImage(Size sz, dword imageid, bool scroll, bool relpos = false);

    VTPage&         SetTabAt(i32 col, bool b = true)         { SetTabStop(col, b); return *this; }
    VTPage&         SetTab(bool b = true)                    { SetTabStop(cursor.x, b); return *this; }
    VTPage&         SetTabs(i32 tsz);
    void            GetTabs(Vec<i32>& tabstops);
    VTPage&         NextTab(i32 n = 1);
    VTPage&         PrevTab(i32 n = 1);
    VTPage&         ClearTabs()                              { tabs.Clear(); tabsync = false; return *this; }

    void            SetEol(bool b = true)                    { cursor.eol = b;     }
    void            ClearEol()                               { cursor.eol = false; }
    bool            IsEol() const                            { return cursor.eol;  }

    void            Invalidate()                             { for(auto& line : lines) line.Invalidate(); }
    void            Invalidate(i32 begin, i32 end);

    // Index: 0-based.
    const VTLine&   FetchLine(i32 i) const;
    const VTLine&   operator[](i32 i) const                  { return FetchLine(i); }
    i32             GetLineCount() const                     { return lines.GetCount() + saved.GetCount(); }

    // Point: 0-based.
    const VTCell&   FetchCell(const Point& pt) const;
    const VTCell&   operator()(const Point& pt) const        { return FetchCell(pt);  }

    // Rect: 0-based.
    bool            FetchRange(const Rect& r, Gate<const VTLine&, VTLine::ConstRange&> consumer, bool rect = false) const;

    const VTLine*    begin() const                           { return lines.begin(); }
    VTLine*          begin()                                 { return lines.begin(); }
    const VTLine*    end() const                             { return lines.end();   }
    VTLine*          end()                                   { return lines.end();   }

    virtual void    Serialize(Stream& s);
    virtual void    Jsonize(JsonIO& jio);
    virtual void    Xmlize(XmlIO& xio);

private:
    bool            HorzMarginsExist() const                                        { return margins.Width()  < size.cx - 1; }
    bool            VertMarginsExist() const                                        { return margins.Height() < size.cy - 1; }
    bool            HorzMarginsContain(i32 col) const                               { return margins.left <= col && col <= margins.right; }
    bool            VertMarginsContain(i32 row) const                               { return margins.top <= row && row <= margins.bottom; }
    Point           Bind(const Rect& r, const Point& p) const;
    VTPage&         MoveHorz(i32 pos, dword flags = Cursor::Absolute);
    VTPage&         MoveVert(i32 pos, dword flags = Cursor::Absolute);
    i32             GetNextRowPos(i32 n, i32 offset, bool rel = false) const        { return rel ? n + cursor.y : offset + n - 1; }
    i32             GetNextColPos(i32 n, i32 offset, bool rel = false) const        { return rel ? n + cursor.x : offset + n - 1; }
    i32             GetAbsCol(i32 col) const                                        { return cursor.displaced ? col - margins.left + 1 : col; }
    i32             GetAbsRow(i32 row) const                                        { return cursor.displaced ? row - margins.top + 1 : row;  }
    VTPage&         RewrapCursor(i32 n);
    i32             LineInsert(i32 pos, i32 n, const VTCell& attrs);
    i32             LineRemove(i32 pos, i32 n, const VTCell& attrs);
    i32             CellAdd(const VTCell& cell, i32 width = 1);
    i32             CellInsert(i32 pos, i32 n, const VTCell& attrs, bool pan);
    i32             CellRemove(i32 pos, i32 n, const VTCell& attrs, bool pan);
    i32             SetTabStop(i32 col, bool b);
    bool            IsTabStop(i32 col) const                                        { return tabs[col]; }
    void            AdjustHistorySize();
    bool            SaveToHistory(i32 pos);
    void            UnwindHistory(const Size& prevsize);
    void            RewindHistory(const Size& prevsize);
    Rect            AdjustRect(const Rect& r, bool displaced = true);
    void            RectFill(const Rect& r, const VTCell& filler, dword flags = 0);
    void            RectCopy(const Point& p, const Rect& r, const Rect& rr, dword flags = 0);
    void            LineFill(i32 pos, i32 begin, i32 end, const VTCell& filler, dword flags = 0);

private:
    Lines           lines;
    Saved           saved;
    Cursor          cursor;
    Cursor          backup;
    Size            size;
    Rect            margins;
    Bits            tabs;
    i32             tabsize;
    i32             historysize;
    bool            history;
    bool            autowrap;
    bool            reversewrap;
    bool            tabsync;
    VTCell          cellattrs;
};

WTxt AsWTxt(const VTPage& page, const Rect& r, bool rectsel = false, bool tspaces = true);

}
#endif
