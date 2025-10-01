#include "CtrlCore.h"

#ifdef GUI_WIN

namespace drx {

#define LLOG(x) // LOG(x)
#define LTIMING(x) // RTIMING(x)

static COLORREF sLightGray;

Rect SystemDraw::GetVirtualScreenArea()
{
	GuiLock __;
	return RectC(GetSystemMetrics(SM_XVIRTUALSCREEN),
		GetSystemMetrics(SM_YVIRTUALSCREEN),
		GetSystemMetrics(SM_CXVIRTUALSCREEN),
		GetSystemMetrics(SM_CYVIRTUALSCREEN));
}

dword SystemDraw::GetInfo() const
{
	return DATABANDS|(native || !(style & DOTS) ? style|NATIVE : style);
}

Size SystemDraw::GetPageSize() const
{
	return native && Dots() ? nativeSize : pageSize;
}

Size SystemDraw::GetNativeDpi() const
{
	return nativeDpi;
}

#ifndef TARGET_WINCE
void Add(LOGPALETTE *pal, i32 r, i32 g, i32 b)
{
	pal->palPalEntry[pal->palNumEntries].peRed   = min(r, 255);
	pal->palPalEntry[pal->palNumEntries].peGreen = min(g, 255);
	pal->palPalEntry[pal->palNumEntries].peBlue  = min(b, 255);
	pal->palPalEntry[pal->palNumEntries++].peFlags = PC_NOCOLLAPSE;
}

HPALETTE GetQlibPalette()
{
	static HPALETTE hQlibPalette;
	if(hQlibPalette) return hQlibPalette;
	SystemDraw::InitColors();
	LOGPALETTE *pal = (LOGPALETTE *) new u8[sizeof(LOGPALETTE) + 256 * sizeof(PALETTEENTRY)];
	pal->palNumEntries = 0;
	pal->palVersion    = 0x300;
	for(i32 r = 0; r < 6; r++)
		for(i32 g = 0; g < 6; g++)
			for(i32 b = 0; b < 6; b++)
				Add(pal, 255 * r / 5, 255 * g / 5, 255 * b / 5);
	for(i32 q = 0; q <= 16; q++)
		Add(pal, 16 * q, 16 * q, 16 * q);
	Add(pal, GetRVal(sLightGray), GetGVal(sLightGray), GetBVal(sLightGray));
	hQlibPalette = CreatePalette(pal);
	delete[] pal;
	return hQlibPalette;
}
#endif

SystemDraw& ScreenInfo()
{
	static ScreenDraw sd(true);
	return sd;
}

HDC ScreenHDC()
{
	static HDC hdc;
	ONCELOCK {
		hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	}
	return hdc;
}

static bool _AutoPalette = true;
bool SystemDraw::AutoPalette() { return _AutoPalette; }
void SystemDraw::SetAutoPalette(bool ap) { _AutoPalette = ap; }

COLORREF SystemDraw::GetColor(Color c) const {
	COLORREF color = c;
#ifdef TARGET_WINCE
	return color;
#else
	if(!palette)
		return color;
	static Index<dword> *SColor;
	ONCELOCK {
		static Index<dword> StaticColor;
		StaticColor << RGB(0x00, 0x00, 0x00) << RGB(0x80, 0x00, 0x00) << RGB(0x00, 0x80, 0x00)
					<< RGB(0x80, 0x80, 0x00) << RGB(0x00, 0x00, 0x80) << RGB(0x80, 0x00, 0x80)
					<< RGB(0x00, 0x80, 0x80) << RGB(0xC0, 0xC0, 0xC0) << RGB(0xC0, 0xDC, 0xC0)
					<< RGB(0xA6, 0xCA, 0xF0) << RGB(0xFF, 0xFB, 0xF0) << RGB(0xA0, 0xA0, 0xA4)
					<< RGB(0x80, 0x80, 0x80) << RGB(0xFF, 0x00, 0x00) << RGB(0x00, 0xFF, 0x00)
					<< RGB(0xFF, 0xFF, 0x00) << RGB(0x00, 0x00, 0xFF) << RGB(0xFF, 0x00, 0xFF)
					<< RGB(0x00, 0xFF, 0xFF) << RGB(0xFF, 0xFF, 0xFF);
		SColor = &StaticColor;
	}
	if(color16 || !AutoPalette())
		return GetNearestColor(handle, color);
	if(SColor->Find(color) >= 0)
		return color;
	if(color == sLightGray)
		return PALETTEINDEX(216 + 17);
	i32 r = GetRVal(color);
	i32 g = GetGVal(color);
	i32 b = GetBVal(color);
	return PALETTEINDEX(r == g && g == b ? (r + 8) / 16 + 216
		                                 : (r + 25) / 51 * 36 +
		                                   (g + 25) / 51 * 6 +
		                                   (b + 25) / 51);
#endif
}

void SystemDraw::InitColors()
{
}

void SystemDraw::SetColor(Color color)
{
	GuiLock __;
	LLOG("SetColor " << color);
	if(color != lastColor) {
		LLOG("Setting, lastColor:" << FormatIntHex(lastColor.GetRaw())
			<< " color:" << FormatIntHex(color.GetRaw()) <<
		     " GetColor:" << FormatIntHex(GetColor(color)) << " palette:" << palette);
		HBRUSH oldBrush = actBrush;
		HBRUSH h;
		if(!IsNull(color))
			h = (HBRUSH) SelectObject(handle, actBrush = CreateSolidBrush(GetColor(color)));
		else {
			HGDIOBJ empty = GetStockObject(HOLLOW_BRUSH);
			h = (HBRUSH) SelectObject(handle, empty);
			actBrush = NULL;
		}
		ASSERT(h);
		if(!orgBrush) orgBrush = h;
		if(oldBrush) DeleteObject(oldBrush);
		lastColor = color;
	}
}

void SystemDraw::SetDrawPen(i32 width, Color color) {
	GuiLock __;
	if(IsNull(width))
		width = PEN_NULL;
	if(width != lastPen || color != lastPenColor) {
		static i32 penstyle[] = {
			PS_NULL, PS_SOLID, PS_DASH,
		#ifndef TARGET_WINCE
			PS_DOT, PS_DASHDOT, PS_DASHDOTDOT
		#endif
		};
		HPEN oldPen = actPen;
		actPen = CreatePen(width < 0 ? penstyle[-width - 1] : PS_SOLID,
			               width < 0 ? 0 : width, GetColor(color));
		HPEN h = (HPEN) SelectObject(handle, actPen);
		if(!orgPen) orgPen = h;
		if(oldPen) DeleteObject(oldPen);
		lastPen = width;
		lastPenColor = color;
	}
}

void SystemDraw::SetOrg() {
	GuiLock __;
#ifdef TARGET_WINCE
	::SetViewportOrgEx(handle, actual_offset.x, actual_offset.y, 0);
#else
	LLOG("SystemDraw::SetOrg: clip = " << GetClip() << ", offset = " << actual_offset);
	::SetWindowOrgEx(handle, -actual_offset.x, -actual_offset.y, 0);
	LLOG("//SystemDraw::SetOrg: clip = " << GetClip());
#endif
}

#ifndef TARGET_WINCE
Point SystemDraw::LPtoDP(Point p) const {
	GuiLock __;
	::LPtoDP(handle, p, 1);
	return p;
}

Point SystemDraw::DPtoLP(Point p) const {
	GuiLock __;
	::DPtoLP(handle, p, 1);
	return p;
}

Rect  SystemDraw::LPtoDP(const Rect& r) const {
	GuiLock __;
	Rect w = r;
	::LPtoDP(handle, reinterpret_cast<POINT *>(&w), 2);
	return w;
}

Rect  SystemDraw::DPtoLP(const Rect& r) const {
	GuiLock __;
	Rect w = r;
	::LPtoDP(handle, reinterpret_cast<POINT *>(&w), 2);
	return w;
}
#endif

Size SystemDraw::GetSizeCaps(i32 i, i32 j) const {
	GuiLock __;
	return Size(GetDeviceCaps(handle, i), GetDeviceCaps(handle, j));
}

void SystemDraw::DotsMode()
{
	::SetMapMode(handle, MM_ANISOTROPIC);
	::SetViewportExtEx(handle, nativeDpi.cx, nativeDpi.cy, NULL);
	::SetViewportOrgEx(handle, 0, 0, NULL);
	::SetWindowExtEx(handle, 600, 600, NULL);
	::SetWindowOrgEx(handle, 0, 0, NULL);
}

void SystemDraw::BeginNative()
{
	if(GetPixelsPerInch() != nativeDpi && ++native == 1) {
		::SetMapMode(handle, MM_TEXT);
		actual_offset_bak = actual_offset;
		Native(actual_offset);
		SetOrg();
	}
}

void SystemDraw::EndNative()
{
	if(GetPixelsPerInch() == nativeDpi && --native == 0) {
		DotsMode();
		actual_offset = actual_offset_bak;
		SetOrg();
	}
}

i32 SystemDraw::GetCloffLevel() const
{
	return cloff.GetCount();
}

void SystemDraw::LoadCaps() {
	GuiLock __;
	color16 = false;
	palette = (GetDeviceCaps(handle, RASTERCAPS) & RC_PALETTE);
	if(palette)
		color16 = GetDeviceCaps(handle, SIZEPALETTE) != 256;
	nativeSize = pageSize = GetSizeCaps(HORZRES, VERTRES);
	nativeDpi = GetSizeCaps(LOGPIXELSX, LOGPIXELSY);
	is_mono = GetDeviceCaps(handle, BITSPIXEL) == 1 && GetDeviceCaps(handle, PLANES) == 1;
}

void SystemDraw::Cinit() {
	GuiLock __;
	lastColor = Color::FromCR(COLORREF(-5));
	lastPenColor = Color::FromCR(COLORREF(-5));
	lastTextColor = COLORREF(-1);
	lastPen = Null;
	actBrush = orgBrush = NULL;
	actPen = orgPen = NULL;
}

void SystemDraw::Init() {
	GuiLock __;
	drawingclip = Rect(-(INT_MAX >> 1), -(INT_MAX >> 1), +(INT_MAX >> 1), +(INT_MAX >> 1));
	Cinit();
	SetBkMode(handle, TRANSPARENT);
	::SetTextAlign(handle, TA_BASELINE);
#ifdef TARGET_WINCE
	actual_offset = Point(0, 0);
#else
	::GetViewportOrgEx(handle, actual_offset);
#endif
	LoadCaps();
}

void SystemDraw::InitClip(const Rect& clip)
{
	drawingclip = clip;
}

void SystemDraw::Reset() {
	GuiLock __;
	style = 0;
}

SystemDraw::SystemDraw() {
	GuiLock __;
	native = 0;
	InitColors();
	actual_offset = Point(0, 0);
	Reset();
	handle = NULL;
	dcMem = NULL;
}

SystemDraw::SystemDraw(HDC hdc) {
	GuiLock __;
	native = 0;
	InitColors();
	Reset();
	Attach(hdc);
}

void SystemDraw::Unselect0() {
	GuiLock __;
	if(orgPen) SelectObject(handle, orgPen);
	if(orgBrush) SelectObject(handle, orgBrush);
	if(actPen) DeleteObject(actPen);
	if(actBrush) DeleteObject(actBrush);
	Cinit();
}

void SystemDraw::Unselect() {
	GuiLock __;
	while(cloff.GetCount())
		End();
	Unselect0();
}

void  SystemDraw::Attach(HDC ahandle)
{
	handle = ahandle;
	dcMem = ::CreateCompatibleDC(handle);
	Init();
}

HDC   SystemDraw::Detach()
{
	Unselect();
	HDC h = handle;
	handle = NULL;
	::DeleteDC(dcMem);
	dcMem = NULL;
	return h;
}

SystemDraw::~SystemDraw() {
	GuiLock __;
	Detach();
}

HDC SystemDraw::BeginGdi() {
	GuiLock __;
	Begin();
	return handle;
}

void SystemDraw::EndGdi() {
	GuiLock __;
	Unselect0();
	End();
}

void BackDraw::Create(SystemDraw& w, i32 cx, i32 cy) {
	ASSERT(w.GetHandle());
	GuiLock __;
	Destroy();
	size.cx = cx;
	size.cy = cy;
	hbmp = ::CreateCompatibleBitmap(w.GetHandle(), cx, cy);
	handle = ::CreateCompatibleDC(w.GetHandle());
	dcMem = ::CreateCompatibleDC(handle);
	ASSERT(hbmp);
	ASSERT(handle);
#ifndef TARGET_WINCE
	if(AutoPalette()) {
		::SelectPalette(handle, GetQlibPalette(), FALSE);
		::RealizePalette(handle);
	}
#endif
	hbmpold = (HBITMAP) ::SelectObject(handle, hbmp);
	Init();
	InitClip(size);
}

void BackDraw::Put(SystemDraw& w, i32 x, i32 y) {
	GuiLock __;
	ASSERT(handle);
	LTIMING("BackDraw::Put");
#ifdef TARGET_WINCE
	::SetViewportOrgEx(handle, 0, 0, 0);
#else
	::SetWindowOrgEx(handle, 0, 0, NULL);
#endif
	::BitBlt(w, x, y, size.cx, size.cy, *this, 0, 0, SRCCOPY);
}

void BackDraw::Destroy() {
	GuiLock __;
	if(handle) {
		Unselect();
		::SelectObject(handle, hbmpold);
		::DeleteDC(handle);
		::DeleteObject(hbmp);
		handle = NULL;
		::DeleteDC(dcMem);
		dcMem = NULL;
	}
}

ScreenDraw::ScreenDraw(bool ic) {
	GuiLock __;
#ifdef TARGET_WINCE
	Attach(CreateDC(NULL, NULL, NULL, NULL));
#else
	Attach(ic ? CreateIC("DISPLAY", NULL, NULL, NULL) : CreateDC("DISPLAY", NULL, NULL, NULL));
	InitClip(GetVirtualScreenArea());
	if(AutoPalette()) {
		SelectPalette(handle, GetQlibPalette(), TRUE);
		RealizePalette(handle);
	}
#endif
}

ScreenDraw::~ScreenDraw() {
	GuiLock __;
	Unselect();
	DeleteDC(handle);
}

#ifndef TARGET_WINCE

void PrintDraw::InitPrinter()
{
	GuiLock __;
	Init();
	style = PRINTER|DOTS;
	DotsMode();
	native = 0;
	actual_offset = Point(0, 0);
	aborted = false;
	pageSize.cx = 600 * nativeSize.cx / nativeDpi.cx; 
	pageSize.cy = 600 * nativeSize.cy / nativeDpi.cy;
	InitClip(pageSize);
}

void PrintDraw::StartPage()
{
	GuiLock __;
	if(aborted) return;
	Unselect();
	if(::StartPage(handle) <= 0)
		aborted = true;
	else
		InitPrinter();
}

void PrintDraw::EndPage()
{
	GuiLock __;
	if(aborted) return;
	Unselect();
	if(::EndPage(handle) <= 0)
		aborted = true;
}

PrintDraw::PrintDraw(HDC hdc, tukk docname)
   : SystemDraw(hdc)
{
	GuiLock __;
	DOCINFOW di;
	memset(&di, 0, sizeof(di));
	di.cbSize = sizeof(di);
	Vec<char16> sys_docname = ToSystemCharsetW(docname);
	di.lpszDocName = sys_docname;
	if(::StartDocW(hdc, &di) <= 0)
		aborted = true;
	else
		InitPrinter();
}

PrintDraw::~PrintDraw() {
	GuiLock __;
	if(aborted)
		::AbortDoc(handle);
	else
		::EndDoc(handle);
}
#endif

}

#endif
