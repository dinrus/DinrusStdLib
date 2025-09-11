enum
{
    SUNDAY    = 0,
    MONDAY    = 1,
    TUESDAY   = 2,
    WEDNESDAY = 3,
    THURSDAY  = 4,
    FRIDAY    = 5,
    SATURDAY  = 6
};

class FlatButton : public Pusher {
public:
    Image img;
    Color fg, bg, hl;
    bool left;
    bool drawedge;
    bool highlight;

    FlatButton();

    void DrawFrame(Draw &w, const Rect &r, Color lc, Color tc, Color rc, Color bc);
    virtual void Paint(Draw &w);
    virtual void MouseEnter(Point p, dword kflags) {
        Refresh();
        Pusher::MouseEnter(p, kflags);
    }
    virtual void MouseLeave() {
        Refresh();
        Pusher::MouseLeave();
    }

    const Image& GetImage() { return img; }

    FlatButton& SetImage(const Image &_img) {
        img = _img;
        Refresh();
        return *this;
    }

    FlatButton& SetLeft()           { left = true;  return *this; }
    FlatButton& SetRight()          { left = false; return *this; }
    FlatButton& DrawEdge(bool b)    { drawedge = b; return *this; }
    FlatButton& Highlight(bool b)   { highlight = b;return *this; }
};

class FlatSpin : public Ctrl
{
private:
    FlatButton left;
    FlatButton right;
    Txt text;
    Size tsz;
    Font font;
    bool selected;
    bool selectable;

public:
    FlatSpin();
    void SetText(const Txt& s);
    void SetTips(tukk tipl, tukk tipr);
    void SetCallbacks(const Event<>& cbl, const Event<>& cbr);

    FlatSpin& Selectable(bool b = true);
    i32 GetWidth(const Txt& s, bool with_buttons = true);
    i32 GetHeight();
    void SetFont(const Font& fnt);
    void SetLeftImage(const Image &img)     { left.SetImage(img);                    }
    void SetRightImage(const Image &img)    { right.SetImage(img);                   }
    void SetHighlight(bool b)               { left.Highlight(b); right.Highlight(b); }

    virtual void MouseMove(Point p, dword keyflags);
    virtual void MouseLeave();
    virtual void LeftDown(Point p, dword keyflags);
    virtual void Layout();
    virtual void Paint(Draw& w);
    virtual Image CursorImage(Point p, dword keyflags);

    typedef FlatSpin CLASSNAME;
};

class PopUpCtrl : public Ctrl
{
protected:
    Image nbg;
    bool popup;
public:
    PopUpCtrl() : popup(false) {}

    Event<>  WhenPopDown;
    Event<>  WhenDeactivate;
    virtual void Deactivate();
    virtual Size ComputeSize() = 0;
    virtual void Reset() {}

    void PopUp(Ctrl *owner, const Rect& rt);
    Size GetPopUpSize() { return ComputeSize(); }

    bool IsPopUp() const;
    void SetPopUp(bool b = true);
};

class Calendar : public PopUpCtrl
{
public:
    struct Style : ChStyle<Style> {
        Color header;

        Color bgmain;
        Color bgtoday;
        Color bgselect;

        Color fgmain;
        Color fgtoday;
        Color fgselect;
        Color outofmonth;
        Color curdate;
        Color today;
        Color selecttoday;
        Color cursorday;
        Color selectday;
        Color line;
        Color dayname;
        Color week;
        Font  font;
        Image spinleftimg;
        Image spinrightimg;
        bool  spinhighlight;
    };

protected:
    const Style *style;
    const Style *St() const;

private:
    typedef Calendar CLASSNAME;

    FlatSpin spin_year;
    FlatSpin spin_month;
    FlatSpin spin_all;

    static i32k cols = 7;
    static i32k rows = 6;

    static const Point nullday;
    i32 bs; //border size
    i32 hs; //header vertical size
    i32 ts; //today vertical size
    float colw;
    float rowh;
    i32 cw, rh;

    i32 col;
    i32 row;
    i32 lastrow;

    i32 fh;

    i32 days[rows][cols];

    Point newday;
    Point oldday;
    Point prevday;
    Point curday, firstday;
    i32   newweek, oldweek;

    Txt stoday;
    Size sztoday;
    Txt curdate;

    bool selall;
    bool istoday;
    bool wastoday;
    bool time_mode;
    bool swap_month_year;
    bool one_button;

    Time view;
    Time today;
    Time sel;

    i32 first_day;

    void OnMonthLeft();
    void OnMonthRight();
    void OnYearLeft();
    void OnYearRight();

    void UpdateFields();

    bool MouseOnToday(Point p);
    bool MouseOnHeader(Point p);

    virtual void LeftDown(Point p, dword keyflags);
    virtual void MouseMove(Point p, dword keyflags);
    virtual void Paint(Draw &w);
    virtual bool Key(dword key, i32 count);
    virtual void MouseLeave();
    virtual void State(i32 reason);
    virtual Image CursorImage(Point p, dword keyflags);

    i32& Day(i32 x, i32 y) { return days[y][x]; }
    i32& Day(Point p)      { return Day(p.x, p.y); }
    Point GetDay(Point p);
    i32 GetWeek(Point p);

    virtual Size ComputeSize();

public:
    Calendar();
    Event<Time &> WhenTime;
    Event<Date>   WhenWeek;

    static const Style& StyleDefault();

    void Reset();

    i32  DayOfWeek(i32 day, i32 month, i32 year, i32 zelleroffset = 2);
    i32  WeekOfYear(i32 day, i32 month, i32 year);

    virtual Val GetData() const           { return time_mode ? (Time) sel : (Date) sel; }
    virtual void  SetData(const Val& v)   { time_mode ? SetTime((Time) v) : SetDate((Date) v); }

    Date GetDate() const;
    void SetDate(i32 y, i32 m, i32 d);
    void SetDate(const Date &dt);

    Date GetTime() const;
    void SetTime(i32 y, i32 m, i32 d, i32 h, i32 n, i32 s);
    void SetTime(const Time &tm);

    Date GetCursor() const             { return view;                 }
    bool HasCursor() const             { return view.day != 0;        }

    Date GetView() const               { return Date(view.year, view.month, view.day ? view.day : 1); }
    void SetView(const Time &v);

    Calendar& SetStyle(const Style& s);
    Calendar& SelectAll(bool b = true)     { selall = b; return *this;       }
    Calendar& NoSelectAll()                { selall = false; return *this;   }
    Calendar& FirstDay(i32 n = MONDAY)     { first_day = n; return *this;    }
    Calendar& TimeMode(bool b = true)      { time_mode = b; return *this;    }
    Calendar& SwapMonthYear(bool b = true) { swap_month_year = b; return *this; }
    Calendar& OneButton(bool b = true)     { one_button = b; return *this;   }
    Calendar& NoOneButton()                { one_button = false; return *this;   }

    void PopUp(Ctrl *owner, Rect &rt);

    Event<>  WhenSelect;
};

struct LineCtrl : Ctrl
{
    i32 pos, real_pos;

    virtual void Paint(Draw& w);
    void SetPos(Point p);
    i32 GetPos();
    void SetPos(i32 p);

    virtual void LeftDown(Point p, dword keyflags);
    virtual void MouseMove(Point p, dword keyflags);
    virtual void LeftUp(Point p, dword keyflags);
    LineCtrl();
};

class Clock : public PopUpCtrl
{
public:
    struct Style : ChStyle<Style> {
        Color header;
        Color bgmain;
        Color fgmain;

        Color arrowhl;
        Color arrowhour;
        Color arrowminute;
        Color arrowsecond;

        Font font;
    };

protected:
    const Style *style;
    const Style *St() const;

private:
    FlatSpin spin_hour;
    FlatSpin spin_minute;
    EditIntSpin ed_hour, ed_minute;
    LineCtrl ln_hour, ln_minute;

    //i32 hour, minute, second;
    Time sel;
    i32 hs;
    i32 dir;
    bool accept_time;

    struct Line {
        Pointf s, e;
    };

    struct MinMax {
        i32 diff;
        i32 value;
    };

    Line lines[3];
    Size sz;
    Pointf cm; //circle middle
    Pointf cf; //circle factor

    z64 cur_time;
    i32   cur_line;
    i32   prv_line;
    i32   cur_hour;
    i32   cur_minute;
    i32   cur_second;
    i32   cur_point;
    i32   prv_point;

    bool seconds;
    bool colon;

    void PaintPtr(i32 n, Draw& w, Pointf p, double pos, double m, double rd, i32 d, Color color, Point cf);
    void PaintCenteredText(Draw& w, i32 x, i32 y, tukk text, const Font& fnt, Color c);
    void PaintCenteredImage(Draw &w, i32 x, i32 y, const Image& img);

    void SetHourEdit();
    void SetMinuteEdit();
    void SetHourLine();
    void SetMinuteLine();
    void SetHourLeft();
    void SetHourRight();
    void SetMinuteLeft();
    void SetMinuteRight();

    MinMax SetMinMax(i32 v, i32 min, i32 max);
    void UpdateFields();
    void UpdateTime();
    i32  IncFactor(i32 dir, i32 pp, i32 cp);

    i32  GetDir(i32 prev_point, i32 cur_point);
    i32  GetPointedLine(Point p);
    i32  GetPoint(Pointf p, double tolerance = 4.0);
    bool IsCircle(Pointf p, Pointf s, double r);
    bool IsLine(Pointf s, Pointf e, Pointf p, double tolerance = 3.0);
    void CalcSizes();

    void Timer();

public:
    virtual bool Key(dword key, i32 count);
    virtual void Paint(Draw& w);
    virtual void State(i32 reason);
    virtual void LeftDown(Point p, dword keyflags);
    virtual void LeftUp(Point p, dword keyflags);
    virtual void MouseMove(Point p, dword keyflags);
    virtual Image CursorImage(Point p, dword keyflags);
    virtual Size ComputeSize();

    virtual Val GetData() const;
    virtual void  SetData(const Val& v);

    Time GetTime() const;
    void SetTime(i32 h, i32 n, i32 s);
    void SetTime(const Time& tm);

    static const Style& StyleDefault();

    void Reset();

    i32 GetHour() const   { return sel.hour;   }
    i32 GetMinute() const { return sel.minute; }
    i32 GetSecond() const { return sel.second; }

    Clock& SetStyle(const Style& s);
    Clock& Seconds(bool b = true)    { seconds = b; return *this; }
    Clock& NoSeconds()               { return Seconds(false); }
    Clock& Colon(bool b = true)      { colon = b; return *this; }
    Clock& NoColon()                 { return Colon(false); }

    Clock();

    typedef Clock CLASSNAME;
};

class CalendarClock : public Ctrl
{
private:
    i32 mode;

    Size calendar_size;
    Size clock_size;
    Size sz;

public:
    enum {
        MODE_DATE = 0,
        MODE_TIME = 1
    };

    typedef CalendarClock CLASSNAME;

    Calendar calendar;
    Clock clock;

    CalendarClock(i32 m = MODE_TIME);
    Event<>  WhenPopDown;

    virtual void Deactivate();
    virtual bool Key(dword key, i32 count);
    virtual void Layout();

    Size ComputeSize();
    Size GetCalendarClockSize() { return ComputeSize(); }
    void UpdateTime(Time &tm);

    void PopUp(Ctrl *owner, const Rect& rt);
};

template<class T>
class DateTimeCtrl : public T {
    MultiButtonFrame drop;
    CalendarClock cc;

    i32 mode;

    void OnCalendarChoice() {
        Date dt = ~cc.calendar;
        ConvertTime *cv = dynamic_cast<ConvertTime *>(this);
        if(cv && cv->IsDayEnd()) {
            Time tm = ToTime(dt);
            tm.hour = 23;
            tm.minute = tm.second = 59;
            this->SetData(tm);
        }
        else
            this->SetData(dt);
        this->WhenAction();
    }

    void OnClockChoice() {
        this->SetData(~cc.clock);
        this->WhenAction();
    }

    void OnClose() {
        this->SetFocus();
    }

    void OnDrop() {
        if(!this->IsEditable())
            return;

        Size sz = cc.GetCalendarClockSize();

        i32 width = sz.cx;
        i32 height = sz.cy;

        Rect rw = this->Ctrl::GetWorkArea();
        Rect rs = this->GetScreenRect();
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
        if(WhenWeek)
            cc.calendar.WhenWeek = WhenWeek.Proxy();
        else
            cc.calendar.WhenWeek.Clear();
        cc.PopUp(this, r);
        cc.calendar <<= this->GetData();
        cc.clock <<= this->GetData();
    }

public:
    typedef DateTimeCtrl CLASSNAME;

    Event<Date> WhenWeek;

    DateTimeCtrl(i32 m) : cc(m) {
        drop.AddTo(*this);
        drop.AddButton().Main() <<= OTVET(OnDrop);
        drop.NoDisplay();
        drop.SetStyle(drop.StyleFrame());
        drop.GetButton(0).SetMonoImage(Grayscale(CtrlsImg::DA()));
        cc.calendar   <<= OTVET(OnCalendarChoice);
        cc.clock      <<= OTVET(OnClockChoice);
        cc.WhenPopDown  = OTVET(OnClose);
        cc.calendar.WhenSelect = WhenSelect.Proxy();
        this->WhenPaper = [=, this](Color c) {
            drop.SetPaper(c);
        };
    }

    virtual void GotFocus()  { T::GotFocus(); drop.RefreshFrame(); }
    virtual void LostFocus() { T::LostFocus(); drop.RefreshFrame(); }
    virtual Size GetMinSize() const { return drop.GetMinSize(); }

    DateTimeCtrl& SetCalendarStyle(const Calendar::Style& style)   { cc.calendar.SetStyle(style); return *this;  }
    DateTimeCtrl& SetClockStyle(const Clock::Style& style)         { cc.clock.SetStyle(style); return *this;  }
    DateTimeCtrl& SetButtonStyle(const MultiButton::Style& style)  { drop.SetStyle(style); return *this; }
    DateTimeCtrl& SelectAll(bool b = true)                         { cc.calendar.SelectAll(b); return *this;     }
    DateTimeCtrl& NoSelectAll()                                    { cc.calendar.SelectAll(false); return *this; }
    DateTimeCtrl& Seconds(bool b = true)                           { cc.clock.Seconds(b); return *this; }
    DateTimeCtrl& NoSeconds()                                      { cc.clock.Seconds(false); return *this; }
    DateTimeCtrl& Colon(bool b = true)                             { cc.clock.Colon(b); return *this; }
    DateTimeCtrl& NoColon()                                        { cc.clock.Colon(false); return *this; }
    DateTimeCtrl& SwapMonthYear(bool b = true)                     { cc.calendar.SwapMonthYear(b); return *this; }
    DateTimeCtrl& OneButton(bool b = true)                         { cc.calendar.OneButton(true); return *this; }
    DateTimeCtrl& NoOneButton()                                    { cc.calendar.OneButton(false); return *this; }
    DateTimeCtrl& DayEnd(bool b = true)                            { ConvertTime::DayEnd(b); return *this; }
    DateTimeCtrl& TimeAlways(bool b = true)                        { ConvertTime::TimeAlways(b); return *this; }

    Event<>  WhenSelect;
};

class DropDate : public DateTimeCtrl<EditDate>
{
public:
    DropDate();
    DropDate& SetDate(i32 y, i32 m, i32 d);
};

class DropTime : public DateTimeCtrl<EditTime>
{
public:
    DropTime();
    DropTime& SetTime(i32 y, i32 m, i32 d, i32 h, i32 n, i32 s);
    DropTime& Seconds(bool b = true)                           { DateTimeCtrl<EditTime>::Seconds(b); EditTime::Seconds(b); return *this; }
    DropTime& NoSeconds()                                      { return Seconds(false); }
};
