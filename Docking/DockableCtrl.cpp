#include "Docking.h"

namespace drx {

#define IMAGECLASS DockingImg
#define IMAGEFILE <drx/Docking/Docking.iml>
#include <drx/Draw/iml_source.h>

DockCont * DockableCtrl::GetContainer() const
{
    return dynamic_cast<DockCont *>(GetParent());
}

DockableCtrl&  DockableCtrl::Set(const Image& _icon, tukk _title, Txt _group)
{
    return SetGroup(_group).Title(_title).Icon(_icon);
}

bool DockableCtrl::IsHotKey(dword key)
{
    return (keycode == key || (keyinfo &&
        (key == keyinfo().key[0] || key == keyinfo().key[1] || key == keyinfo().key[2] || key == keyinfo().key[3])));
}

DockableCtrl&  DockableCtrl::SizeHint(const Size& min, const Size& max, const Size& std)
{
    minsize = min; maxsize = max; stdsize = std;
    return *this;
}

DockableCtrl& DockableCtrl::Title(const WTxt& _title)
{
    title = _title;
    if (GetContainer())
        GetContainer()->ChildTitleChanged(*this);
    return *this;
}

DockableCtrl& DockableCtrl::Icon(const Image& m)
{
    icon = m;
    if (GetContainer())
        GetContainer()->ChildTitleChanged(*this);
    return *this;
}

bool DockableCtrl::IsFloating() const
{
    DockCont *c = GetContainer();
    return c && c->IsFloating();
}

bool DockableCtrl::IsDocked() const
{
    DockCont *c = GetContainer();
    return c && c->IsDocked();
}

bool DockableCtrl::IsAutoHide() const
{
    DockCont *c = GetContainer();
    return c && c->IsAutoHide();
}

bool DockableCtrl::IsTabbed() const
{
    DockCont *c = GetContainer();
    return c && (c->IsTabbed() || c->GetCount() > 1);
}

bool DockableCtrl::IsHidden() const
{
    DockCont *c = GetContainer();
    return !c || c->IsHidden();
}

i32 DockableCtrl::GetDockAlign() const
{
    DockCont *c = GetContainer();
    return c ? c->GetDockAlign() : -1;
}

DockableCtrl&  DockableCtrl::SetGroup(const Txt& g)
{
    DockCont *c = GetContainer();
    group = g;
    if (c) c->GroupRefresh();
    return *this;
}

void DockableCtrl::Highlight()
{
    if (!GetParent() || !IsOpen()) return;
    ViewDraw v(this);
    ChPaint(v, GetSize(), GetStyle().highlight[1]);
}

void DockableCtrl::TimedHighlight(i32 ms)
{
    Highlight();
    SetTimeCallback(ms, OTVET(StopHighlight), TIMEID_HIGHLIGHT);
}

DockableCtrl::DockableCtrl()
{
    style = NULL;
    group = Null;
    maxsize = Null;
    stdsize = Null;
    minsize = Size(0, 0);
    keycode = 0;
    keyinfo = 0;
    AllowDockAll();
    Transparent(true);
    BackPaint();
}

void DockableCtrlImgsLook(Val* look, i32 i, i32 n)
{
    while(n--)
        *look++ = ChLookWith(Null, DockingImg::Get(i++), Point(0, 0)); // Center image
}

Image ChCrop(const Val& element, Size canvas, Rect crop, Color baseline)
{
    ImageDraw draw(canvas);
    ChPaint(draw, canvas, element);
    if (!IsNull(baseline)) {
        draw.DrawLine(0, canvas.cy-1, canvas.cx+1, canvas.cy, 1, baseline);
    }
    return Crop(draw, crop);
}

Image StandardHighlight(Color inside, Color border)
{
    Size sz(5, 5);
    ImageBuffer ib(sz);
    RGBA *q = ~ib;
    for (i32 i = 0; i < 5; i++)
        for (i32 j = 0; j < 5; j++)
            *(q++) = (i == 4 || j == 4 || !i || !j) ? border : inside;
    ib.SetHotSpot(Point(1, 1));
    ib.Set2ndSpot(Point(3, 3));
    return static_cast<Image>(ib);
}

Image AlphaHighlight(const Image& img, i32 alpha)
{
    ImageDraw draw(img.GetSize());
    draw.Alpha().DrawRect(img.GetSize(), GrayColor(alpha));
    draw.DrawImage(0, 0, img);
    // Is there a better way to set hotspots than this?
    Image temp = (Image)draw; // GCC
    ImageBuffer ib(temp);
    ib.SetHotSpot(Point(1, 1));
    ib.Set2ndSpot(Point(3, 3));
    return static_cast<Image>(ib);
}

CH_STYLE(DockableCtrl, Style, StyleDefault)
{
    const TabCtrl::Style *style = &TabCtrl::StyleDefault();

#ifdef TARGET_COCOA
    handle[0] = SColorFace();
    handle[1] = SColorHighlight();
#else
    handle[0] = ChCrop(style->normal[0], Size(20, 20), Rect(2, 2, 11, 20), SColorShadow); // No focus
    handle[1] = Colorize(handle[0], SColorHighlight(), 160); // Focus
#endif
    handle_margins = Rect(2, 0, 0, 0);
    handle_vert = false;

    title_font = StdFont();
    title_ink[0] = SColorInfoText();        // No focus
    title_ink[1] = SColorHighlightText();   // Focus

    DockableCtrlImgsLook(close, DockingImg::I_DClosed, 4);
    DockableCtrlImgsLook(windowpos, DockingImg::I_DMenud, 4);
    DockableCtrlImgsLook(autohide, DockingImg::I_DHided, 4);
    DockableCtrlImgsLook(pin, DockingImg::I_DPind, 4);

    Image img = StandardHighlight(Blend(SColorHighlight, SColorPaper, 90), Blend(SColorHighlight, SBlack, 90));
    highlight[0] = img;
    highlight[1] = AlphaHighlight(img, 160);
}

CH_STYLE(DockableCtrl, Style, StyleDefaultVert)
{
    Assign(DockableCtrl::StyleDefault());

    handle[0] = RotateAntiClockwise(handle[0]); // No focus
    handle[1] = RotateAntiClockwise(handle[1]); // Focus
    handle_margins = Rect(0, 2, 2, 1);
    handle_vert = true;
}

}
