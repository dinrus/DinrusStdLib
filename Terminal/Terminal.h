#ifndef _Terminal_TerminalCtrl_h
#define _Terminal_TerminalCtrl_h

#include <drx/CtrlLib/CtrlLib.h>
#include <X/jpg/jpg.h>

#include "Parser.h"
#include "Page.h"
#include "Sixel.h"

namespace drx {

class TerminalCtrl : public Ctrl {
public:
    i32k ANSI_COLOR_COUNT = 16;    // Actually, ANSI + aixterm colors.

    enum Colors
    {
        COLOR_BLACK = 0,
        COLOR_RED,
        COLOR_GREEN,
        COLOR_YELLOW,
        COLOR_BLUE,
        COLOR_MAGENTA,
        COLOR_CYAN,
        COLOR_WHITE,
        COLOR_LTBLACK,
        COLOR_LTRED,
        COLOR_LTGREEN,
        COLOR_LTYELLOW,
        COLOR_LTBLUE,
        COLOR_LTMAGENTA,
        COLOR_LTCYAN,
        COLOR_LTWHITE,
        COLOR_INK,
        COLOR_INK_SELECTED,
        COLOR_PAPER,
        COLOR_PAPER_SELECTED,
        MAX_COLOR_COUNT
    };

    enum ConformanceLevels
    {
        LEVEL_0 = 0,
        LEVEL_1,
        LEVEL_2,
        LEVEL_3,
        LEVEL_4
    //  LEVEL_5
    };

    enum LEDs
    {
        LED_NUMLOCK  = 0,
        LED_CAPSLOCK,
        LED_SCRLOCK,
        LED_ALL
    };

   enum TimerIds
   {
        TIMEID_REFRESH = Ctrl::TIMEID_COUNT,
        TIMEID_SIZEHINT,
        TIMEID_BLINK,
        TIMEID_COUNT
    };

    // Inline image data structure.
    struct InlineImage : ValType<InlineImage, 999, Moveable<InlineImage> > {
        Image       image;
        Size        cellsize;
        Size        fontsize;
        Rect        paintrect;
        operator    Val() const                               { return RichVal<TerminalCtrl::InlineImage>(*this); }
    };

    TerminalCtrl();
    virtual ~TerminalCtrl();

    Event<>              WhenBell;
    Event<>              WhenResize;
    Event<Size>          WhenSetSize;
    Event<Bar&>          WhenBar;
    Event<Txt>        WhenTitle;
    Event<Txt>        WhenOutput;
    Event<i32, bool>     WhenLED;
    Gate<PasteClip&>     WhenClip;
    Event<const Txt&> WhenLink;
    Event<const Txt&> WhenImage;

    // Window Ops support.
    Event<bool>          WhenWindowMinimize;
    Event<bool>          WhenWindowMaximize;
    Event<i32>           WhenWindowFullScreen;
    Event<Rect>          WhenWindowGeometryChange;

    // APC support.
    Event<const Txt&> WhenApplicationCommand;

    void            Write(ukk data, i32 size, bool utf8 = true);
    void            Write(const Txt& s, bool utf8 = true)        { Write(~s, s.GetLength(), utf8); }
    void            WriteUtf8(const Txt& s)                      { Write(s, true);         }

    TerminalCtrl&   Echo(const Txt& s);
    
    TerminalCtrl&   SetLevel(i32 level)                             { SetEmulation(level); return *this; }
    bool            IsLevel0() const                                { return !modes[DECANM]; }
    bool            IsLevel1() const                                { return modes[DECANM] && clevel >= LEVEL_1; }
    bool            IsLevel2() const                                { return modes[DECANM] && clevel >= LEVEL_2; }
    bool            IsLevel3() const                                { return modes[DECANM] && clevel >= LEVEL_3; }
    bool            IsLevel4() const                                { return modes[DECANM] && clevel >= LEVEL_4; }

    TerminalCtrl&   Set8BitMode(bool b = true)                      { eightbit = b; return *this; }
    TerminalCtrl&   No8BitMode()                                    { return Set8BitMode(false); }
    bool            Is8BitMode() const                              { return IsLevel2() && eightbit; }
    bool            Is7BitMode() const                              { return !Is8BitMode(); }

    bool            IsUtf8Mode() const                              { return charset == CHARSET_UNICODE && !legacycharsets; }

    void            HardReset()                                     { Reset(true);  }
    void            SoftReset()                                     { Reset(false); }

    TerminalCtrl&   History(bool b = true)                          { dpage.History(b); return *this; }
    TerminalCtrl&   NoHistory()                                     { return History(false); }
    TerminalCtrl&   ClearHistory()                                  { dpage.EraseHistory(); return *this; }
    bool            HasHistory() const                              { return dpage.HasHistory(); }

    TerminalCtrl&   SetHistorySize(i32 sz)                          { dpage.SetHistorySize(sz); return *this; }
    i32             GetHistorySize() const                          { return dpage.GetHistorySize(); }

    TerminalCtrl&   SetFont(Font f);
    Font            GetFont() const                                 { return font; }

    TerminalCtrl&   SetPadding(Size sz);
    Size            GetPadding() const                              { return padding; }

    void            SetCharset(u8 cs)                             { charset = ResolveCharset(cs); }
    u8            GetCharset() const                              { return charset;    }

    TerminalCtrl&   Ink(Color c)                                    { SetRefreshColor(COLOR_INK, c); return *this; }
    TerminalCtrl&   Paper(Color c)                                  { SetRefreshColor(COLOR_PAPER, c); return *this; }
    TerminalCtrl&   SelectionInk(Color c)                           { SetRefreshColor(COLOR_INK_SELECTED, c); return *this; }
    TerminalCtrl&   SelectionPaper(Color c)                         { SetRefreshColor(COLOR_PAPER_SELECTED, c); return *this; }

    TerminalCtrl&   SetColor(i32 i, Color c)                        { colortable[i] = c; return *this; }
    void            SetRefreshColor(i32 i, Color c)                 { SetColor(i, c); Refresh(); }
    Color           GetColor(i32 i) const                           { return colortable[i]; }

    TerminalCtrl&   DynamicColors(bool b = true)                    { dynamiccolors = b; return *this; }
    TerminalCtrl&   NoDynamicColors()                               { return DynamicColors(false); }
    bool            HasDynamicColors() const                        { return dynamiccolors; }

    TerminalCtrl&   LightColors(bool b = true)                      { lightcolors = b; Refresh(); return *this; }
    TerminalCtrl&   NoLightColors()                                 { return LightColors(false); }
    bool            HasLightColors() const                          { return lightcolors; }

    TerminalCtrl&   AdjustColors(bool b = true)                     { adjustcolors = b; Refresh(); return *this; }
    TerminalCtrl&   NoAdjustColors()                                { return AdjustColors(false); }
    bool            HasAdjustedColors() const                       { return adjustcolors; }

    TerminalCtrl&   ResetColors();

    TerminalCtrl&   IntensifyBoldText(bool b = true)                { intensify = b; Refresh(); return *this; }
    TerminalCtrl&   NoIntensifyBoldText()                           { return IntensifyBoldText(false); }
    bool            HasIntensifiedBoldText() const                  { return intensify; }

    TerminalCtrl&   BlinkingText(bool b = true)                     { blinkingtext = b; RefreshDisplay(); return *this; }
    TerminalCtrl&   NoBlinkingText()                                { return BlinkingText(false); }
    bool            HasBlinkingText() const                         { return blinkingtext; }

    TerminalCtrl&   BlinkInterval(i32 ms)                           { blinkinterval = clamp(ms, 100, 60000); return *this; }

    TerminalCtrl&   SetCursorStyle(i32 style, bool blink = true)    { caret.Set(style, blink); return *this;}
    i32             GetCursorStyle() const                          { return caret.GetStyle(); }
    TerminalCtrl&   BlockCursor(bool blink = true)                  { caret.Block(blink); return *this; }
    TerminalCtrl&   BeamCursor(bool blink = true)                   { caret.Beam(blink);  return *this; }
    TerminalCtrl&   UnderlineCursor(bool blink = true)              { caret.Underline(blink); return *this; }
    TerminalCtrl&   BlinkingCursor(bool b = true)                   { caret.Blink(b); return *this; }
    TerminalCtrl&   NoBlinkingCursor()                              { return BlinkingCursor(false); }
    bool            IsCursorBlinking() const                        { return caret.IsBlinking();    }
    TerminalCtrl&   LockCursor(bool b = true)                       { caret.Lock(b);  return *this; }
    TerminalCtrl&   UnlockCursor()                                  { caret.Unlock(); return *this; }
    bool            IsCursorLocked() const                          { return caret.IsLocked();      }
    Point           GetCursorPoint() const;

    TerminalCtrl&   NoBackground(bool b = true)                     { nobackground = b; Transparent(b); Refresh(); return *this; }
    bool            HasBackground() const                           { return !nobackground; }

    TerminalCtrl&   ShowSizeHint(bool b = true)                     { sizehint = b; return *this; }
    TerminalCtrl&   HideSizeHint()                                  { return ShowSizeHint(false); }
    bool            HasSizeHint() const                             { return sizehint; }
    
    TerminalCtrl&   ShowScrollBar(bool b = true);
    TerminalCtrl&   HideScrollBar()                                 { return ShowScrollBar(false);  }
    bool            HasScrollBar() const                            { return sb.IsChild();          }
    TerminalCtrl&   SetScrollBarStyle(const ScrollBar::Style& s)    { sb.SetStyle(s); return *this; }

    TerminalCtrl&   AlternateScroll(bool b = true)                  { alternatescroll = b; return *this; }
    TerminalCtrl&   NoAlternateScroll()                             { return AlternateScroll(false); }
    bool            HasAlternateScroll() const                      { return alternatescroll; }

    TerminalCtrl&   MouseWheelStep(i32 lines)                       { wheelstep = max(1, lines); return *this; }
    i32             GetMouseWheelStep() const                       { return wheelstep; }

    TerminalCtrl&   AutoHideMouseCursor(bool b = true)              { hidemousecursor = b; return *this; }
    TerminalCtrl&   NoAutoHideMouseCurosr()                         { return AutoHideMouseCursor(false); }
    bool            IsMouseCursorAutoHidden() const                 { return hidemousecursor; }

    TerminalCtrl&   KeyNavigation(bool b = true)                    { keynavigation = b; return *this; }
    TerminalCtrl&   NoKeyNavigation()                               { return KeyNavigation(false); }
    bool            HasKeyNavigation() const                        { return keynavigation; }

    TerminalCtrl&   InlineImages(bool b = true)                     { sixelimages = jexerimages = iterm2images = b; return *this; }
    TerminalCtrl&   NoInlineImages()                                { return InlineImages(false);  }
    bool            HasInlineImages() const                         { return sixelimages || jexerimages || iterm2images; }

    TerminalCtrl&   SixelGraphics(bool b = true)                    { sixelimages = b; return *this; }
    TerminalCtrl&   NoSixelGraphics()                               { return SixelGraphics(false); }
    bool            HasSixelGraphics() const                        { return sixelimages; }

    TerminalCtrl&   JexerGraphics(bool b = true)                    { jexerimages = b; return *this; }
    TerminalCtrl&   NoJexerGraphics()                               { return JexerGraphics(false); }
    bool            HasJexerGraphics() const                        { return jexerimages; }

    TerminalCtrl&   iTerm2Graphics(bool b = true)                   { iterm2images = b; return *this; }
    TerminalCtrl&   NoiTerm2Graphics(bool b = true)                 { return iTerm2Graphics(false); }
    bool            HasiTerm2Graphics() const                       { return iterm2images; }

    TerminalCtrl&   Hyperlinks(bool b = true)                       { hyperlinks = b; return *this; }
    TerminalCtrl&   NoHyperlinks()                                  { return Hyperlinks(false);     }
    bool            HasHyperlinks() const                           { return hyperlinks; }

    TerminalCtrl&   ReverseWrap(bool b = true)                      { XTrewrapm((reversewrap = b)); return *this; }
    TerminalCtrl&   NoReverseWrap()                                 { return ReverseWrap(false); }
    bool            HasReverseWrap() const                          { return reversewrap; }

    TerminalCtrl&   DelayedRefresh(bool b = true)                   { delayedrefresh = b; return *this; }
    TerminalCtrl&   NoDelayedRefresh()                              { return DelayedRefresh(false); }
    bool            IsDelayingRefresh() const                       { return delayedrefresh; }

    TerminalCtrl&   LazyResize(bool b = true)                       { lazyresize = b; return *this; }
    TerminalCtrl&   NoLazyResize()                                  { return LazyResize(false);     }
    bool            IsLazyResizing() const                          { return lazyresize; }

    TerminalCtrl&   WindowOps(bool b = true)                        { windowactions = windowreports = b; return *this; }
    TerminalCtrl&   NoWindowOps()                                   { return WindowOps(false);      }
    bool            HasWindowOps() const                            { return windowactions || windowreports; }

    TerminalCtrl&   WindowReports(bool b = true)                    { windowreports = b; return *this; }
    TerminalCtrl&   NoWindowReports()                               { return WindowReports(false);  }
    bool            HasWindowReports() const                        { return windowreports; }

    TerminalCtrl&   WindowActions(bool b = true)                    { windowactions = b; return *this; }
    TerminalCtrl&   NoWindowActions()                               { return WindowActions(false);  }
    bool            HasWindowActions() const                        { return windowactions; }

    TerminalCtrl&   PermitClipboardAccess(bool b = true)            { return PermitClipboardRead(b).PermitClipboardWrite(b); }
    TerminalCtrl&   ForbidClipboardAccess()                         { clipaccess = CLIP_NONE;  return *this; }
    bool            IsClipboardAccessPermitted() const              { return clipaccess != CLIP_NONE; }

    TerminalCtrl&   PermitClipboardRead(bool b = true)              { if(b) clipaccess |= CLIP_READ; else clipaccess &= ~CLIP_READ; return *this; }
    TerminalCtrl&   ForbidClipboardRead()                           { return PermitClipboardRead(false); }
    bool            IsClipboardReadPermitted() const                { return clipaccess & CLIP_READ; }

    TerminalCtrl&   PermitClipboardWrite(bool b = true)             { if(b) clipaccess |= CLIP_WRITE; else clipaccess &= ~CLIP_WRITE; return *this; }
    TerminalCtrl&   ForbidClipboardWrite()                          { return PermitClipboardWrite(false); }
    bool            IsClipboardWritePermitted() const               { return clipaccess & CLIP_WRITE; }
 
    TerminalCtrl&   PCStyleFunctionKeys(bool b = true)              { pcstylefunctionkeys = b; return *this; }
    TerminalCtrl&   NoPCStyleFunctionKeys()                         { return PCStyleFunctionKeys(false); }
    bool            HasPCStyleFunctionKeys() const                  { return pcstylefunctionkeys; }
    
    TerminalCtrl&   SetImageDisplay(const Display& d)               { imgdisplay = &d; return *this; }
    const Display&  GetImageDisplay() const                         { return *imgdisplay; }

    TerminalCtrl&   UDK(bool b = true)                              { userdefinedkeys = b; return *this;  }
    TerminalCtrl&   NoUDK()                                         { return UDK(false);     }
    bool            HasUDK() const                                  { return userdefinedkeys; }
    TerminalCtrl&   LockUDK(bool b = true)                          { userdefinedkeyslocked = b;  return *this; }
    TerminalCtrl&   UnlockUDK()                                     { return LockUDK(false); }
    bool            IsUDKLocked() const                             { return userdefinedkeyslocked; }

    Size            GetFontSize() const                             { return Size(max(font.GetWidth('M'), font.GetWidth('W')), font.GetCy()); }
    Size            GetCellSize() const                             { return GetFontSize() + padding * 2; }
    Size            GetPageSize() const                             { Size csz = GetCellSize(); return clamp(GetSize() / csz, Size(1, 1), GetScreenSize() / csz); }

    Size            PageSizeToClient(Size sz) const                 { return AddFrameSize(sz * GetCellSize()); }
    Size            PageSizeToClient(i32 col, i32 row) const        { return PageSizeToClient(Size(col, row)); }

    Size            GetMinSize() const override                     { return PageSizeToClient(Size(2, 2)); }
    Size            GetStdSize() const override                     { return PageSizeToClient(Size(80, 24)); }
    Size            GetMaxSize() const override                     { return PageSizeToClient(Size(132, 24)); }

    void            Copy()                                          { Copy(GetSelectedText()); }
    void            Copy(const WTxt& s);
    void            Paste()                                         { DragAndDrop(Null, Clipboard()); }
    void            Paste(const WTxt& s, bool filter = false);
    void            SelectAll(bool history = false);
    bool            IsSelection() const                             { return !IsNull(anchor) && anchor != selpos && seltype != SEL_NONE; }

    Txt          GetSelectionData(const Txt& fmt) const override;
    
    void            StdBar(Bar& menu);
    void            EditBar(Bar& menu);
    void            LinksBar(Bar& menu);
    void            ImagesBar(Bar& menu);
    void            OptionsBar(Bar& menu);

    void            Layout() override                               { SyncSize(true); SyncSb(); }

    void            Paint(Draw& w)  override                        { Paint0(w); }
    void            PaintPage(Draw& w)                              { Paint0(w, true); }

    bool            Key(dword key, i32 count) override;
    virtual bool    VTKey(dword key, i32 count);
    virtual bool    UDKey(dword key, i32 count);
    virtual bool    NavKey(dword key, i32 count);

    TerminalCtrl&   MetaEscapesKeys(bool b = true)                  { if(b) metakeyflags |= MKEY_ESCAPE; else metakeyflags &= ~MKEY_ESCAPE; return *this; }
    TerminalCtrl&   MetaShiftsKeys(bool b = true)                   { if(b) metakeyflags |= MKEY_SHIFT;  else metakeyflags &= ~MKEY_SHIFT;  return *this; }
    TerminalCtrl&   MetaKeyDoesNothing()                            { modes.Set(XTALTESCM, false); metakeyflags = MKEY_NONE; return *this; }

    void            LeftDown(Point pt, dword keyflags) override;
    void            LeftUp(Point pt, dword keyflags) override;
    void            LeftDrag(Point pt, dword keyflags) override;
    void            LeftDouble(Point pt, dword keyflags) override;
    void            LeftTriple(Point pt, dword keyflags) override;
    void            MiddleDown(Point pt, dword keyflags) override;
    void            MiddleUp(Point pt, dword keyflags) override;
    void            RightDown(Point pt, dword keyflags) override;
    void            RightUp(Point pt, dword keyflags) override;
    void            MouseMove(Point pt, dword keyflags) override;
    void            MouseWheel(Point pt, i32 zdelta, dword keyflags) override;
    Image           MouseEvent(i32 event, Point pt, i32 zdelta, dword keyflags) override;
    void            VTMouseEvent(Point pt, dword event, dword keyflags, i32 zdelta = 0);

    bool            IsMouseOverImage() const                        { Point pt = GetMouseViewPos(); return IsMouseOverImage(ClientToPagePos(pt)); }
    bool            IsMouseOverHyperlink() const                    { Point pt = GetMouseViewPos(); return IsMouseOverHyperlink(ClientToPagePos(pt)); }

    bool            IsTracking() const;

    const VTCell&   GetCellAtMousePos() const                       { Point pt = GetMouseViewPos(); return page->FetchCell(ClientToPagePos(pt));; }
    const VTCell&   GetCellAtCursorPos() const                      { return page->GetCell(); };

    Txt          GetHyperlinkUri()                               { return GetHyperlinkURI(mousepos, true); }
    Image           GetInlineImage()                                { return GetInlineImage(mousepos, true);  }

    void            DragAndDrop(Point pt, PasteClip& d) override;

    void            GotFocus() override                             { if(modes[XTFOCUSM]) PutCSI('I'); Refresh(); }
    void            LostFocus() override                            { if(modes[XTFOCUSM]) PutCSI('O'); Refresh(); }

    void            RefreshDisplay();

    Rect            GetCaret() const override                       { return caret.IsBlinking() ? caretrect : Null; }

    Image           CursorImage(Point p, dword keyflags) override;

    void            AnswerBackMessage(const Txt& s)              { answerback = s; }

    void            State(i32 reason) override;

    void            Serialize(Stream& s) override;
    void            Jsonize(JsonIO& jio) override;
    void            Xmlize(XmlIO& xio) override;

    static void     ClearImageCache();
    static void     SetImageCacheMaxSize(i32 maxsize, i32 maxcount);

    static void     ClearHyperlinkCache();
    static void     SetHyperlinkCacheMaxSize(i32 maxcount);

private:
    void        InitParser(VTInStream& vts);
    
    void        PreParse()                                      { /*ScheduleRefresh();*/ }
    void        PostParse()                                     { ScheduleRefresh(); }

    void        SyncPage(bool notify = true);
    void        SwapPage();

    void        ScheduleRefresh();

    void        Blink(bool b);

    void        Scroll();
    void        SyncSb();

    void        SyncSize(bool notify = true);

    Tuple<Txt, Size> GetSizeHint();
    void        RefreshSizeHint();

    Rect        GetCaretRect();
    void        PlaceCaret(bool scroll = false);

    i32         GetSbPos() const                                { return IsAlternatePage() ? 0 : sb; }

    Point       GetCursorPos() const                            { return --page->GetPos(); /* VT cursor position is 1-based */ }

    Point       ClientToPagePos(Point pt) const;
    Point       SelectionToPagePos(Point pt) const;

    void        SetSelection(Point  pl, Point ph, dword selflag);
    bool        GetSelection(Point& pl, Point& ph) const;
    Rect        GetSelectionRect() const;
    void        ClearSelection();
    bool        IsSelected(Point pt) const;
    WTxt     GetSelectedText() const;
    void        GetLineSelection(const Point& pt, Point& pl, Point& ph) const;
    bool        GetWordSelection(const Point& pt, Point& pl, Point& ph) const;
    void        GetWordPosL(const VTLine& line, Point& pl) const;
    void        GetWordPosH(const VTLine& line, Point& ph) const;

    bool        IsMouseOverImage(Point pt) const                { return !IsSelected(pt) && page->FetchCell(pt).IsImage(); }
    bool        IsMouseOverHyperlink(Point pt) const            { return !IsSelected(pt) && page->FetchCell(pt).IsHyperlink(); }

    void        HighlightHyperlink(Point pt);

    Txt      GetHyperlinkURI(Point pt, bool modifier);
    Image       GetInlineImage(Point pt, bool modifier);

private:
    using       ImagePart  = Tuple<dword, Point, Rect>;
    using       ImageParts = Vec<ImagePart>;

	// TODO: Needs a rewrite to be more flexible.
    struct ImageTxt : Moveable<ImageTxt> {
        Txt  data;
        Size    size;
        bool    encoded:1;
        bool    keepratio:1;
        bool    transparent:1;
        dword   GetHashVal() const                            { return FoldHash(CombineHash(data, size, encoded, keepratio)); }
        void    SetNull()                                       { data = Null; size = Null; encoded = keepratio = true; }
        bool    IsNullInstance() const                          { return drx::IsNull(data); }
        ImageTxt()                                           { SetNull(); }
        ImageTxt(const Nuller&)                              { SetNull(); }
        ImageTxt(Txt&& s)                                 { SetNull(); data = s;  }
    };

    struct InlineImageMaker : LRUCache<InlineImage>::Maker {
        dword   id;
        const   Size& fontsize;
        const   ImageTxt& imgs;
        Txt  Key() const override;
        i32     Make(InlineImage& imagedata) const override;
        InlineImageMaker(i32 i, const ImageTxt& s, const Size& sz)
        : id(i)
        , imgs(s)
        , fontsize(sz)
        {
        }
    };

    struct HyperlinkMaker : LRUCache<Txt>::Maker {
        dword   id;
        const   Txt& url;
        Txt  Key() const override;
        i32     Make(Txt& link) const override;
        HyperlinkMaker(i32 i, const Txt& s)
        : id(i)
        , url(s)
        {
        }
    };

    void        Paint0(Draw& w, bool print = false);
    void        PaintSizeHint(Draw& w);
    void        PaintImages(Draw& w, ImageParts& parts, const Size& csz);

    void        RenderImage(const ImageTxt& simg, bool scroll);
    const InlineImage& GetCachedImageData(dword id, const ImageTxt& simg, const Size& csz);

    void        RenderHyperlink(const Txt& uri);
    Txt      GetCachedHyperlink(dword id, const Txt& data = Null);

private:
    enum TextSelectionTypes : dword {
        SEL_NONE    = 0,
        SEL_TEXT    = 1,
        SEL_RECT    = 2,
        SEL_WORD    = 3,
        SEL_LINE    = 4
    };
    
    enum ModifierKeyFlags : dword {
        MKEY_NONE   = 0,
        MKEY_ESCAPE = 1,
        MKEY_SHIFT  = 2
    };

    enum ClipboardAccessFlags : dword {
        CLIP_NONE   = 0,
        CLIP_READ   = 1,
        CLIP_WRITE  = 2
    };

    const Display *imgdisplay;
    VScrollBar  sb;
    Scroller    scroller;
    Point       mousepos;
    Font        font            = Monospace();
    u8        charset;
    Rect        caretrect;
    Point       anchor          = Null;
    Point       selpos          = Null;
    dword       seltype         = SEL_NONE;
    bool        multiclick      = false;
    bool        ignorescroll    = false;
    bool        mousehidden     = false;
    bool        resizing        = false;
    bool        hinting         = false;
    bool        blinking        = false;
    i32         blinkinterval   = 500;
    i32         wheelstep       = GUI_WheelScrollLines();
    i32         metakeyflags    = MKEY_ESCAPE;
    i32         clipaccess      = CLIP_NONE;
    dword       activelink      = 0;
    dword       prevlink        = 0;
    Size        padding         = { 0, 0 };

    bool        eightbit;
    bool        reversewrap;
    bool        keynavigation;
    bool        legacycharsets;
    bool        alternatescroll;
    bool        pcstylefunctionkeys;
    bool        userdefinedkeys;
    bool        userdefinedkeyslocked;
    bool        windowactions;
    bool        windowreports;
    bool        sixelimages;
    bool        jexerimages;
    bool        iterm2images;
    bool        hyperlinks;
    bool        delayedrefresh;
    bool        lazyresize;
    bool        sizehint;
    bool        nobackground;
    bool        intensify;
    bool        blinkingtext;
    bool        dynamiccolors;
    bool        adjustcolors;
    bool        lightcolors;
    bool        hidemousecursor;

// Down beloe is the emulator stuff, formerley knonw as "Console"...

private:
    VTPage*     page;

private:
    Color       GetColorFromIndex(const VTCell& cell, i32 which) const;
    void        SetInkAndPaperColor(const VTCell& cell, Color& ink, Color& paper);
    void        ReportANSIColor(i32 opcode, i32 index, const Color& c);
    void        ReportDynamicColor(i32 opcode, const Color& c);
    void        SetProgrammableColors(const VTInStream::Sequence& seq, i32 opcode);
    void        ResetProgrammableColors(const VTInStream::Sequence& seq, i32 opcode);
    bool        SetSaveColor(i32 index, const Color& c);
    bool        ResetLoadColor(i32 index);
    void        ParseExtendedColors(VTCell& attrs, const Vec<Txt>& opcodes, i32& index);

    VecMap<i32, Color> savedcolors;
    Color       colortable[MAX_COLOR_COUNT];

    struct ColorTableSerializer {
        Color   *table;
        void    Serialize(Stream& s);
        void    Jsonize(JsonIO& jio);
        void    Xmlize(XmlIO& xio);
        ColorTableSerializer(Color *ct) : table(ct) {}
    };

private:
    void        PutChar(i32 c);
    i32         LookupChar(i32 c);

    void        ParseControlChars(u8 c)                                               { DispatchCtl(c); }
    void        ParseEscapeSequences(const VTInStream::Sequence& seq);
    void        ParseCommandSequences(const VTInStream::Sequence& seq);
    void        ParseDeviceControlTxts(const VTInStream::Sequence& seq);
    void        ParseOperatingSystemCommands(const VTInStream::Sequence& seq);
    void        ParseApplicationProgrammingCommands(const VTInStream::Sequence& seq)    { WhenApplicationCommand(seq.payload); }

    bool        Convert7BitC1To8BitC1(const VTInStream::Sequence& seq);

    void        ClearPage(const VTInStream::Sequence& seq, dword flags);
    void        ClearLine(const VTInStream::Sequence& seq, dword flags);
    void        ClearTabs(const VTInStream::Sequence& seq);

    void        ReportMode(const VTInStream::Sequence& seq);
    void        ReportDeviceStatus(const VTInStream::Sequence& seq);
    void        ReportDeviceParameters(const VTInStream::Sequence& seq);
    void        ReportDeviceAttributes(const VTInStream::Sequence& seq);
    void        ReportControlFunctionSettings(const VTInStream::Sequence& seq);
    void        ReportRectAreaChecksum(const VTInStream::Sequence &seq);
    void        ReportPresentationState(const VTInStream::Sequence& seq);

    void        RestorePresentationState(const VTInStream::Sequence& seq);

    void        SelectGraphicsRendition(const VTInStream::Sequence& seq);
    void        SetGraphicsRendition(VTCell& attrs, const Vec<Txt>& opcodes);
    void        InvertGraphicsRendition(VTCell& attrs, const Vec<Txt>& opcodes);
    Txt      GetGraphicsRenditionOpcodes(const VTCell& attrs);

    void        ParseSixelGraphics(const VTInStream::Sequence& seq);
    void        ParseJexerGraphics(const VTInStream::Sequence& seq);
    void        ParseiTerm2Graphics(const VTInStream::Sequence& seq);

    void        ParseHyperlinks(const VTInStream::Sequence& seq);

    void        ParseClipboardRequests(const VTInStream::Sequence& seq);
    
    void        SetCaretStyle(const VTInStream::Sequence& seq);

    void        SetProgrammableLEDs(const VTInStream::Sequence& seq);

    void        SetDeviceConformanceLevel(const VTInStream::Sequence& seq);

    void        SetUserDefinedKeys(const VTInStream::Sequence& seq);

    void        CopyRectArea(const VTInStream::Sequence& seq);
    void        FillRectArea(const VTInStream::Sequence& seq);
    void        ClearRectArea(const VTInStream::Sequence& seq, bool selective = false);
    void        SelectRectAreaAttrsChangeExtent(const VTInStream::Sequence& seq);
    void        ChangeRectAreaAttrs(const VTInStream::Sequence& seq, bool invert);

    void        HandleWindowOpsRequests(const VTInStream::Sequence& seq);
    void        WindowMoveRequest(TopWindow *w, i32 x, i32 y);
    void        WindowResizeRequest(TopWindow *w, i32 cx, i32 cy);
    void        WindowPageResizeRequest(TopWindow *w, i32 cx, i32 cy);
    void        WindowMaximizeHorzRequest(TopWindow *w);
    void        WindowMaximizeVertRequest(TopWindow *w);

    void        SetHorizontalMargins(const VTInStream::Sequence& seq);
    void        SetVerticalMargins(const VTInStream::Sequence& seq);
    void        SetLinesPerPage(const VTInStream::Sequence& seq);

    void        SetColumns(i32 cols)                                { WhenSetSize(PageSizeToClient(Size(cols, page->GetSize().cy))); }
    void        SetRows(i32 rows)                                   { WhenSetSize(PageSizeToClient(Size(page->GetSize().cx, rows))); }

    void        SetDECStyleCellProtection(bool b)                   { page->Attributes(cellattrs.ProtectDEC(b)); }
    dword       GetDECStyleFillerFlags() const;
    void        SetISOStyleCellProtection(bool b)                   { page->Attributes(cellattrs.ProtectISO(b)); }
    dword       GetISOStyleFillerFlags() const;
    
    void        Backup(bool tpage = true, bool csets = true, bool attrs = true);
    void        Restore(bool tpage = true, bool csets = true, bool attrs = true);

    void        SetEmulation(i32 level, bool reset = true);

    void        Reset(bool full);

    void        AlternateScreenBuffer(bool b);
    
    void        VT52MoveCursor();   // VT52 direct cursor addressing.

private:
    VTInStream  parser;
    VTPage      dpage;
    VTPage      apage;
    VTCell      cellattrs;
    VTCell      cellattrs_backup;
    Txt      out;
    Txt      answerback;
    u8        clevel;
    bool        streamfill:1;

private:
    const VTCell&   GetAttrs() const                            { return cellattrs;  }

    void        SetPageSize(Size sz)                            { page->SetSize(sz); }
    VTPage&     GetDefaultPage()                                { return dpage; }
    bool        IsDefaultPage() const                           { return page == &dpage; }
    VTPage&     GetAlternatePage()                              { return apage; }
    bool        IsAlternatePage() const                         { return page == &apage; }

    TerminalCtrl&   Put0(const Txt& s, i32 cnt = 1);
    TerminalCtrl&   Put0(i32 c, i32 cnt = 1);
    TerminalCtrl&   Put(const WTxt& s, i32 cnt = 1);
    TerminalCtrl&   Put(i32 c, i32 cnt = 1);
    TerminalCtrl&   PutRaw(const Txt& s, i32 cnt = 1);
    TerminalCtrl&   PutESC(const Txt& s, i32 cnt = 1);
    TerminalCtrl&   PutESC(i32 c, i32 cnt = 1);
    TerminalCtrl&   PutCSI(const Txt& s, i32 cnt = 1);
    TerminalCtrl&   PutCSI(i32 c, i32 cnt = 1);
    TerminalCtrl&   PutOSC(const Txt& s, i32 cnt = 1);
    TerminalCtrl&   PutOSC(i32 c, i32 cnt = 1);
    TerminalCtrl&   PutDCS(const Txt& s, i32 cnt = 1);
    TerminalCtrl&   PutDCS(i32 c, i32 cnt = 1);
    TerminalCtrl&   PutSS2(const Txt& s, i32 cnt = 1);
    TerminalCtrl&   PutSS2(i32 c, i32 cnt = 1);
    TerminalCtrl&   PutSS3(const Txt& s, i32 cnt = 1);
    TerminalCtrl&   PutSS3(i32 c, i32 cnt = 1);
    TerminalCtrl&   PutEncoded(const WTxt& s, bool noctl = false);
    TerminalCtrl&   PutEol();

    void        Flush();
    void        CancelOut()                                     { out.Clear(); }

    void        DisplayAlignmentTest();
	i32			ReadInt(const Txt& s, i32 def);

private:
    bool        GetUDKTxt(dword key, Txt& val);

private:
    VecMap<dword, Txt> udk;

private:
    Bits        modes;

private:
     // ANSI modes.
    void        ANSIkam(bool b);
    void        ANSIcrm(bool b);
    void        ANSIirm(bool b);
    void        ANSIsrm(bool b);
    void        ANSIlnm(bool b);
    void        ANSIerm(bool b);

    // DEC private modes.
    void        DECanm(bool b);
    void        DECarm(bool b);
    void        DECawm(bool b);
    void        DECbkm(bool b);
    void        DECckm(bool b);
    void        DECcolm(bool b);
    void        DECkpam(bool b);
    void        DEClrmm(bool b);
    void        DECom(bool b);
    void        DECsclm(bool b);
    void        DECscnm(bool b);
    void        DECsdm(bool b);
    void        DECtcem(bool b);

    // Private mode extensions.
    void        XTasbm(i32 mode, bool b);
    void        XTanymm(bool b);
    void        XTascm(bool b);
    void        XTbrpm(bool b);
    void        XTdragm(bool b);
    void        XTfocusm(bool b);
    void        XTaltkeym(bool b);
    void        XTpcfkeym(bool b);
    void        XTrewrapm(bool b);
    void        XTsgrmm(bool b);
    void        XTsgrpxmm(bool b);
    void        XTsrcm(bool b);
    void        XTutf8mm(bool b);
    void        XTx10mm(bool b);
    void        XTx11mm(bool b);

    void        SetMode(const VTInStream::Sequence& seq, bool enable);

    using CbControl  = Tuple<u8, u8, Event<TerminalCtrl&, u8> >;
    using CbFunction = Tuple<u8, u8, Event<TerminalCtrl&, const VTInStream::Sequence&> >;
    using CbMode     = Tuple<word, u8, u8, Event<TerminalCtrl&, i32, bool> >;

    const CbFunction* FindFunctionPtr(const VTInStream::Sequence& seq);
    const CbMode*     FindModePtr(word modenum, u8 modetype);
    void              DispatchCtl(u8 ctl);

private:
    // Key manipulation and VT and PC-style function keys support.
    struct FunctionKey : Moveable<FunctionKey> {
        enum Type : dword { Cursor, EditPad, NumPad, Programmable, Function };
        dword       type;
        dword       level;
        tukk code;
        tukk altcode;

        FunctionKey(dword type, dword level, tukk code, tukk altcode = nullptr)
        : type(type)
        , level(level)
        , code(code)
        , altcode(altcode)
        {
        }
    };

    bool ProcessKey(dword key, bool ctrlkey, bool altkey, i32 count);
    bool ProcessVTStyleFunctionKey(const FunctionKey& k, dword modkeys, i32 count);
    bool ProcessPCStyleFunctionKey(const FunctionKey& k, dword modkeys, i32 count);

public:
    // DEC and xterm style caret (cursor) support.
    class Caret {
        i32       style;
        bool      blinking;
        bool      locked;
    public:
        enum : i32
        {
            BLOCK = 0,
            BEAM,
            UNDERLINE
        };
        Event<> WhenAction;
        void    Set(i32 style_, bool blink);
        Caret&  Block(bool blink = true)                        { Set(BLOCK, blink); return *this; }
        Caret&  Beam(bool blink = true)                         { Set(BEAM, blink);  return *this; }
        Caret&  Underline(bool blink = true)                    { Set(UNDERLINE, blink); return *this; }
        Caret&  Blink(bool b = true)                            { if(!locked) { blinking = b; WhenAction(); }; return *this; }
        Caret&  Lock(bool b = true)                             { locked = b; return *this; }
        Caret&  Unlock()                                        { return Lock(false); }
        i32     GetStyle() const                                { return style;    }
        bool    IsBlinking() const                              { return blinking; }
        bool    IsLocked() const                                { return locked;   }
        void    Serialize(Stream& s);
        void    Jsonize(JsonIO& jio);
        void    Xmlize(XmlIO& xio);
        Caret();
        Caret(i32 style, bool blink, bool lock);
    };

private:
    Caret       caret;

public:
    // Terminal legacy character sets ("G-set") support.
    class GSets {
        u8  g[4];
        u8  d[4];
        u8  ss;
        i32   l, r;
    public:
        GSets&     G0toGL()                                     { l = 0; return *this; }
        GSets&     G1toGL()                                     { l = 1; return *this; }
        GSets&     G2toGL()                                     { l = 2; return *this; }
        GSets&     G3toGL()                                     { l = 3; return *this; }
        GSets&     G0toGR()                                     { r = 0; return *this; }
        GSets&     G1toGR()                                     { r = 1; return *this; }
        GSets&     G2toGR()                                     { r = 2; return *this; }
        GSets&     G3toGR()                                     { r = 3; return *this; }

        GSets&     G0(u8 c)                                   { g[0] = c; return *this; }
        GSets&     G1(u8 c)                                   { g[1] = c; return *this; }
        GSets&     G2(u8 c)                                   { g[2] = c; return *this; }
        GSets&     G3(u8 c)                                   { g[3] = c; return *this; }
        GSets&     SS(u8 c)                                   { ss   = c; return *this; }
        GSets&     Broadcast(u8 c)                            { g[0] = g[1] = g[2] = g[3] = c; return *this; }
        
        u8        Get(i32 c, bool allowgr = true) const       { return c < 0x80 || !allowgr ? g[l] : g[r]; }

        i32         GetGLNum()                                  { return l; }
        i32         GetGRNum()                                  { return r; }

        u8        GetGL() const                               { return g[l]; }
        u8        GetGR() const                               { return g[r]; }
        u8        GetG0() const                               { return g[0]; }
        u8        GetG1() const                               { return g[1]; }
        u8        GetG2() const                               { return g[2]; }
        u8        GetG3() const                               { return g[3]; }
        u8        GetSS() const                               { return ss;   }

        void        ConformtoANSILevel1();
        void        ConformtoANSILevel2();
        void        ConformtoANSILevel3();

        GSets&      ResetG0()                                   { g[0] = d[0]; return *this; }
        GSets&      ResetG1()                                   { g[1] = d[1]; return *this; }
        GSets&      ResetG2()                                   { g[2] = d[2]; return *this; }
        GSets&      ResetG3()                                   { g[3] = d[3]; return *this; }

        void        Reset();
        void        Serialize(Stream& s);
        void        Jsonize(JsonIO& jio);
        void        Xmlize(XmlIO& xio);

        GSets(u8 defgset = CHARSET_ISO8859_1);
        GSets(u8 g0, u8 g1, u8 g2, u8 g3);
    };

    void            SetLegacyCharsets(GSets newgsets)           { gsets = newgsets;  }
    const GSets&    GetLegacyCharsets() const                   { return gsets;      }
    TerminalCtrl&   LegacyCharsets(bool b = true)               { legacycharsets = b; return *this; }
    TerminalCtrl&   NoLegacyCharsets()                          { return LegacyCharsets(false); }

private:
    u8            ResolveVTCharset(u8 cs)                   { return ResolveCharset(legacycharsets ? cs : charset); }
    i32             DecodeCodepoint(i32 c, u8 gset);
    i32             EncodeCodepoint(i32 c, u8 gset);
    WTxt         DecodeDataTxt(const Txt& s);
    Txt          EncodeDataTxt(const WTxt& ws);

private:
    GSets           gsets;
    GSets           gsets_backup;


    // Currently supported ANSI and private terminal modes.

    enum TerminalModes : u8
    {
        GATM = 0,
        KAM,
        CRM,
        IRM,
        SRTM,
        ERM,
        VEM,
        HEM,
        PUM,
        SRM,
        FEAM,
        FETM,
        MATM,
        TTM,
        SATM,
        TSM,
        EBM,
        LNM,
        DECANM,
        DECARM,
        DECAWM,
        DECBKM,
        DECCKM,
        DECCOLM,
        DECKPAM,
        DECLRMM,
        DECOM,
        DECSCLM,
        DECSCNM,
        DECSDM,
        DECTCEM,
        XTASBM,
        XTASCM,
        XTBRPM,
        XTDRAGM,
        XTANYMM,
        XTFOCUSM,
        XTALTESCM,
        XTPCFKEYM,
        XTREWRAPM,
        XTSPREG,
        XTSRCM,
        XTSGRMM,
        XTSGRPXMM,
        XTUTF8MM,
        XTX10MM,
        XTX11MM,
        XTSHOWSB,
        VTMODECOUNT
    };
};

// Custom displays.

const Display& NormalImageCellDisplay();
const Display& ScaledImageCellDisplay();

// Color formatters and converters.

class ConvertHashColorSpec : public Convert {
public:
    ConvertHashColorSpec() {}
    i32     Filter(i32 chr) const override;
    Val   Scan(const Val& text) const override;
    Val   Format(const Val& q) const override;
};

class ConvertRgbColorSpec : public Convert {
public:
    ConvertRgbColorSpec() {}
    i32     Filter(i32 chr) const override;
    Val   Scan(const Val& text) const override;
    Val   Format(const Val& q) const override;
};

class ConvertCmykColorSpec : public Convert {
public:
    ConvertCmykColorSpec() {}
    i32     Filter(i32 chr) const override;
    Val   Scan(const Val& text) const override;
    Val   Format(const Val& q) const override;
};

class ConvertColor : public Convert {
public:
    Val   Scan(const Val& text) const override;
    Val   Format(const Val& q) const override;
};

// Legacy charsets.

extern u8 CHARSET_DEC_VT52;   // DEC VT52 graphics character set.
extern u8 CHARSET_DEC_DCS;    // DEC VT100+ line drawing character set.
extern u8 CHARSET_DEC_MCS;    // DEC VT200+ multinational character set.
extern u8 CHARSET_DEC_TCS;    // DEC VT300+ technical character set.

INITIALIZE(DECGSets);
}
#endif
