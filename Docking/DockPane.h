class DockPane : public Splitter {
public:
    typedef DockPane CLASSNAME;

private:
    struct DummyCtrl : public Ctrl  {
        DummyCtrl() { minsize = stdsize = Null; Transparent(); NoWantFocus(); }
        Size minsize;
        Size stdsize;
        virtual Size GetMinSize() const { return minsize; }
        virtual Size GetStdSize() const { return stdsize; }
    };
    DummyCtrl   dummy;
    i32         animtick;
    i32         animinterval;
    i32         animmaxticks;
    Vec<i32> animpos;
    Vec<i32> savedpos;

    void    StartAnimate(i32 ix, Size sz, bool restore);
    void    AnimateTick();
    void    EndAnimate();

    void    SmartReposUp(Vec<i32>& p, i32 ix, i32 sz);
    void    SmartReposDown(Vec<i32>& p, i32 ix);
    void    SimpleRepos(Vec<i32>& p, i32 ix, i32 sz);
    i32     GetMinPos(i32 notix);
    void    FixChildSizes();

    i32     NormalPos(i32 ix) const         { return (ix > 0) ? pos[ix] - pos[ix-1] : pos[ix]; }
    void    CumulativePos(Vec<i32>& p) const;
    void    NonCumulativePos(Vec<i32>& p) const;

    enum { TIMEID_ACTION_CHECK = Ctrl::TIMEID_COUNT,
           TIMEID_ANIMATE,
           TIMEID_COUNT };
public:
    enum { LEFT, TOP, RIGHT, BOTTOM };

    i32     GetCount() const                        { return pos.GetCount(); }

    void    Dock(Ctrl& newctrl, Size sz, i32 pos, bool animate, bool save = false);
    void    Undock(Ctrl& newctrl, bool animate, bool restore = false);

    void    SavePos()                               { savedpos <<= pos; }
    void    RestorePos()                            { ASSERT(savedpos.GetCount() == pos.GetCount()); pos <<= savedpos; Layout(); }

    i32     GetCtrlSize(i32 i) const                { return NormalPos(i); }
    void    SetCtrlSize(i32 i, i32 sz)              { pos[i] = (i == 0) ? sz : sz + pos[i-1]; }

    void    Clear()                                 { pos.Clear(); savedpos.Clear(); }

    bool    IsAnimating() const                     { return animpos.GetCount(); }
    void    SetAnimateRate(i32 ticks, i32 ms)       { animmaxticks = max(1, ticks); animinterval = max(0, ms); }
    i32     GetAnimMaxTicks() const                 { return animmaxticks; }
    i32     GetAnimInterval() const                 { return animinterval; }
    Rect    GetFinalAnimRect(Ctrl& ctrl);

    virtual void Serialize(Stream& s)               { s % pos; if (s.IsLoading()) FixChildSizes(); }

    DockPane();
};
