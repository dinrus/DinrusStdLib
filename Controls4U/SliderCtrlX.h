// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors

namespace drx {

class SliderCtrlX : public Ctrl {
    static i32k BORDER_SIZE = 2;
    static i32k BORDER1 = BORDER_SIZE;
    static i32k BORDER2 = 2 * BORDER_SIZE;

    typedef enum { HORZ, VERT } HOVE;

    typedef enum { MAJOR, MINOR } MAJORMINOR;

public:
    typedef enum { TOP, MIDDLE_TOP, MIDDLE_BOTTOM, BOTTOM,
        RIGHT = TOP, LEFT = BOTTOM, MIDDLE_LEFT = MIDDLE_BOTTOM, MIDDLE_RIGHT = MIDDLE_TOP } TICK_ALIGNMENT;

    typedef enum {CONTROL, INDICATOR} SLIDERTYPES;

    typedef enum {TYPE_0, TYPE_1, TYPE_2, TYPE_3, TYPE_4} THUMB_TYPES;

private:
    i32             border2;            //
    Color           m_FillColor;        // what color to use for filling the slider
    i32             m_nMin;
    i32             m_nMax;
    bool            m_bInverted;        // True if origin is right or down
    i32             m_nStep;            // Val of steps to inc/dec
    bool            m_bRound_step;      // Uses the defined Step
    bool            m_bJump;            // Jumps directly to mouse pos if click on the scale
    bool            m_bUseCustomThumbs;
    // eg a slider from -180 to 180, setting MajorTix to 30 would put Major tix
    // on the endpoints, as well as every 30, so: -180, -150, -120, -90, -60, -30, 0, 30...
    // Text Labels are drawn on all Major Tick points.
    // Setting MinorTix to 5 would draw minor tix every 5, eg: -175, -170, -165...
    i32             m_nMajorTicks;      // This is the "increment" for major tix
    i32             m_nMinorTicks;      // This is the "increment" for minor tix
    i32             m_nMajorTickSize;   // Percent of the space from the line to the edge, eg 30
    i32             m_nMinorTickSize;   // Percent of the space from the line to the edge, eg 20
    //i32               m_nTextAlign;
    i32             m_nTickPosition;
    i32             m_nThickness;       // Thickness of the thumb guide
    i32             m_nSliderType;      // either a CONTROL / INDICATOR
    i32             m_nThumbNumber;     // Number of the thumb
    i32             m_nThumbType;       // Index of thumb in Image list
    Image           m_ThumbImg;         // Image of the thumb
    Image           m_ThumbFImg;        // Image of the focused thumb
    Size            m_ThumbSize;        // Size of the thumb;

    Vec<Ctrl*>   m_vctrlOutput;
    Vec<i32>     m_vVals;
    Vec<Image>   m_vThumbImgs;
    Vec<Image>   m_vThumbImgsFocus;
    i32             nHalfThumb;

    i32             ClientToSlider(i32 x) const;
    i32             HoVe(i32  x, i32  y) const;
    i32&            HoVeR(i32& x, i32& y) const;
    i32             Max()   const   { return drx::max( m_nMin, m_nMax ); };
    i32             Min()   const   { return drx::min( m_nMin, m_nMax ); };
    i32             SliderToClient(i32 value) const;
    Font            fnt;

protected:

    void            DrawTick( Draw &w, MAJORMINOR Type, HOVE Orientation, i32 nPos, i32 nVal );

public:
    typedef SliderCtrl CLASSNAME;

    Callback        WhenSlideFinish;
    Callback2<Txt&, i32> LabelFormat;

                    SliderCtrlX();
    virtual         ~SliderCtrlX();

    SliderCtrlX&    AddOutCtrl( Ctrl* c );
    void            Dec();
    SliderCtrlX&    FillColor( Color c )                { m_FillColor = c; return *this; };
    virtual Val   GetData() const;
    virtual Val   GetData( i32 nIndex ) const;
    i32             GetMax() const                      { return m_nMax; }
    i32             GetMin() const                      { return m_nMin; }
    i32             GetStep() const                     { return m_nStep; }
    virtual void    GotFocus();
    void            Inc();
    bool            IsInverted() const                  { return m_bInverted; };
    bool            IsRoundStep() const                 { return m_bRound_step; }
    bool            IsVert() const;
    SliderCtrlX&    Jump(bool v = false)                { m_bJump = v; return *this; }
    virtual bool    Key(dword key, i32 repcnt);
    virtual void    LeftDown(Point pos, dword keyflags);
    virtual void    LeftRepeat(Point pos, dword keyflags);
    virtual void    LeftUp(Point pos, dword keyflags);
    virtual void    LostFocus();
    virtual void    MouseMove(Point pos, dword keyflags);
    SliderCtrlX&    MinMax(i32 _min, i32 _max);
    virtual void    Paint(Draw& draw);
    SliderCtrlX&    Range(i32 max)                      { return MinMax(0, max); }
    SliderCtrlX&    SetCustomThumb( Image i, i32 nIndex = 0 ) { m_vThumbImgs.At( nIndex ) = i; m_vThumbImgsFocus.At( nIndex ) = i; return *this; };
    virtual void    SetData(const Val& value);
    SliderCtrlX&    SetInverted(bool b = true)          { m_bInverted = b; return *this; }
    SliderCtrlX&    SetMajorTicks( i32 n )              { m_nMajorTicks = n; return *this; }
    SliderCtrlX&    SetMajorTicksSize( i32 n )          { m_nMajorTickSize = n; return *this; }
    SliderCtrlX&    SetMaxi( i32 n=100 )                { m_nMax = n; return *this; }
    SliderCtrlX&    SetMini( i32 n=0 )                  { m_nMin = n; return *this; }
    SliderCtrlX&    SetMinorTicks( i32 n )              { m_nMinorTicks = n; return *this; }
    SliderCtrlX&    SetMinorTicksSize( i32 n )          { m_nMinorTickSize = n; return *this; }
    SliderCtrlX&    SetSliderType(i32 n=0)              { m_nSliderType = n; return *this;}
    SliderCtrlX&    SetThickness( i32 n = 2 )           { m_nThickness = n; border2 = m_nThickness * BORDER_SIZE; return *this; }
    SliderCtrlX&    SetTickPosition(i32 n )             { m_nTickPosition = n; return *this; }
    Val           SetVal( const Val& v, i32 nIndex = 0 );
    SliderCtrlX&    Step(i32 _step, bool _r = true)     { m_nStep = _step; m_bRound_step = _r; return *this; }
    SliderCtrlX&    SetThumbType(i32 n=0);
    SliderCtrlX&    UseCustomThumbs( bool b = true )    { m_bUseCustomThumbs = b; return *this; }
    SliderCtrlX&    SetFont(Font font)                  { fnt = font;   return *this;}
};

}