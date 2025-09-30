#include "CtrlCore.h"

#ifdef GUI_WIN

namespace drx {

#ifndef TARGET_WINCE

void WinMetaFile::Init() {
	hemf = NULL;
}

void WinMetaFile::Paint(Draw& w, const Rect& r) const {
	w.DrawRect(r, White());
	if(!hemf)
		return;
	SystemDraw *h = dynamic_cast<SystemDraw *>(&w);
	if(h)
		PlayEnhMetaFile(h->GetHandle(), hemf, r);
	else {
		Size sz = r.GetSize();
		SystemImageDraw iw(sz);
		Paint(iw, sz);
		w.DrawImage(r.left, r.top, iw);
	}
}

void WinMetaFile::Paint(Draw& w, i32 x, i32 y, i32 cx, i32 cy) const {
	Paint(w, RectC(x, y, cx, cy));
}

void WinMetaFile::ReadClipboard() {
	Clear();
	if(::IsClipboardFormatAvailable(CF_ENHMETAFILE) && ::OpenClipboard(NULL)) {
		HENHMETAFILE hemf = (HENHMETAFILE) ::GetClipboardData(CF_ENHMETAFILE);
		if(hemf) Attach(hemf);
		::CloseClipboard();
	}
}

void WinMetaFile::WriteClipboard() const {
	if(hemf && ::OpenClipboard(NULL)) {
		::EmptyClipboard();
		::SetClipboardData(CF_ENHMETAFILE, CopyEnhMetaFile(hemf, NULL));
		::CloseClipboard();
	}
}

void WinMetaFile::Clear() {
	if(hemf)
		::DeleteEnhMetaFile(hemf);
	hemf = NULL;
}

/* TODO: Remove picks
void WinMetaFile::Pick(pick_ WinMetaFile& src) {
	hemf = src.hemf;
	size = src.size;
	src.hemf = (HENHMETAFILE)(intptr_t) 0xffffffff;
}

void WinMetaFile::Copy(const WinMetaFile& src) {
	hemf = ::CopyEnhMetaFile(src.hemf, NULL);
	size = src.size;
}
*/
void WinMetaFile::Attach(HENHMETAFILE _hemf) {
	Clear();
	ENHMETAHEADER info;
	memset(&info, 0, sizeof(info));
	info.iType = EMR_HEADER;
	info.nSize = sizeof(info);
	info.dSignature = ENHMETA_SIGNATURE;
	if(_hemf && ::GetEnhMetaFileHeader(_hemf, sizeof(info), &info)
	   && info.rclFrame.left < info.rclFrame.right
	   && info.rclFrame.top < info.rclFrame.bottom) {
		size.cx = info.rclFrame.right - info.rclFrame.left;
		size.cy = info.rclFrame.bottom - info.rclFrame.top;
		size = 600 * size / 2540;
		hemf = _hemf;
	}
}

HENHMETAFILE WinMetaFile::Detach()
{
	size = Size(0, 0);
	HENHMETAFILE out = hemf;
	hemf = NULL;
	return out;
}

#pragma pack(push, 1)
struct PLACEABLE_METAFILEHEADER
{
    DWORD   key;
    WORD    hmf;
    short   left, top, right, bottom;
    WORD    inch;
    DWORD   reserved;
    WORD    checksum;
};
#pragma pack(pop)

void WinMetaFile::Set(ukk data, dword len)
{
	Clear();

	if(len <= sizeof(ENHMETAHEADER))
		return;

	i32 first = Peek32le(data);

	HENHMETAFILE hemf;
	if((hemf = ::SetEnhMetaFileBits(len, (const BYTE *)data)) != NULL)
		Attach(hemf);
	else
	if(first == (i32)0x9AC6CDD7) {
		if ( len <= 22 ) return;
		const PLACEABLE_METAFILEHEADER *mfh = (const PLACEABLE_METAFILEHEADER *)data;
		Attach(::SetWinMetaFileBits(len - 22, (const BYTE *)data + 22, NULL, NULL));
		size = 600 * Size(mfh->right - mfh->left, mfh->bottom - mfh->top) / 2540;
		return;
	}
	else
		Attach(::SetWinMetaFileBits(len, (const BYTE *)data, NULL, NULL));
}

Txt WinMetaFile::Get() const
{
	i32 size = ::GetEnhMetaFileBits(hemf, 0, 0);
	TxtBuf b(size);
	::GetEnhMetaFileBits(hemf, size, (BYTE *)~b);
	return Txt(b);
}

void WinMetaFile::Serialize(Stream& s) {
	dword size = 0;
	if(s.IsStoring()) {
		if(hemf) {
			size = ::GetEnhMetaFileBits(hemf, 0, 0);
			s % size;
			Buffer<u8> buffer(size);
			::GetEnhMetaFileBits(hemf, size, buffer);
			s.SerializeRaw(buffer, size);
		}
		else
			s % size;
	}
	else {
		Clear();
		s % size;
		if(size) {
			Txt buffer = s.GetAll(size);
			HENHMETAFILE hemf = ::SetEnhMetaFileBits(size, buffer);
			Attach(hemf);
		}
	}
}

WinMetaFile::WinMetaFile(uk data, i32 len)
{
	Init();
	Set(data, len);
}

WinMetaFile::WinMetaFile(const Txt& data)
{
	Init();
	Set(data);
}

WinMetaFile::WinMetaFile(HENHMETAFILE hemf) {
	Init();
	Attach(hemf);
}

WinMetaFile::WinMetaFile(HENHMETAFILE hemf, Size sz) {
	Init();
	Attach(hemf);
	size = sz;
}

WinMetaFile::WinMetaFile(tukk file) {
	Init();
	Load(file);
}

struct cDrawWMF : DataDrawer {
	i32  y;
	Size sz;
	WinMetaFile wmf;

	virtual void Open(const Txt& data, i32 cx, i32 cy);
	virtual void Render(ImageBuffer& ib);
};

void cDrawWMF::Open(const Txt& data, i32 cx, i32 cy)
{
	y = 0;
	wmf.Set(data);
	sz = Size(cx, cy);
}

void cDrawWMF::Render(ImageBuffer& ib)
{
	if(wmf) {
		ImageDraw iw(ib.GetSize());
		wmf.Paint(iw, 0, -y, sz.cx, sz.cy);
		y += ib.GetHeight();
		Image img(iw);
		ib = img;
	}
	else
		Fill(~ib, RGBAZero(), ib.GetLength());
}

INITBLOCK
{
	DataDrawer::Register<cDrawWMF>("wmf");
};

void DrawWMF(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Txt& wmf)
{
	w.DrawData(x, y, cx, cy, wmf, "wmf");
}

void DrawWMF(Draw& w, i32 x, i32 y, const Txt& wmf)
{
	WinMetaFile h(wmf);
	Size sz = h.GetSize();
	DrawWMF(w, x, y, sz.cx, sz.cy, wmf);
}

Drawing LoadWMF(tukk path, i32 cx, i32 cy)
{
	DrawingDraw iw(cx, cy);
	DrawWMF(iw, 0, 0, cx, cy, LoadFile(path));
	return iw;
}

Drawing LoadWMF(tukk path)
{
	Txt wmf = LoadFile(path);
	WinMetaFile h(wmf);
	if(h) {
		Size sz = h.GetSize();
		DrawingDraw iw(sz.cx, sz.cy);
		DrawWMF(iw, 0, 0, sz.cx, sz.cy, wmf);
		return iw;
	}
	return Null;
}

bool WinMetaFileDraw::Create(HDC hdc, i32 cx, i32 cy, tukk app, tukk name, tukk file) {
	if(handle) Close();

	Txt s;
	if(app) s.Cat(app);
	s.Cat('\0');
	if(name) s.Cat(name);
	s.Cat('\0');

	bool del = false;
	if(!hdc) {
		hdc = ::GetWindowDC((HWND) NULL);
		del = true;
	}
	size = Size(iscale(cx, 2540, 600), iscale(cy, 2540, 600));

	Rect r = size;

	HDC mfdc = ::CreateEnhMetaFile(hdc, file, r, name || app ? (tukk )s : NULL);
	if(!mfdc)
		return false;
	Size px(max(1, ::GetDeviceCaps(mfdc, HORZRES)),  max(1, ::GetDeviceCaps(mfdc, VERTRES)));
	Size mm(max(1, ::GetDeviceCaps(mfdc, HORZSIZE)), max(1, ::GetDeviceCaps(mfdc, VERTSIZE)));
	Attach(mfdc);

	Init();

	style = DOTS;

	::SetMapMode(handle, MM_ANISOTROPIC);
	::SetWindowOrgEx(handle, 0, 0, 0);
	::SetWindowExtEx(handle, 600, 600, 0);
	::SetViewportOrgEx(handle, 0, 0, 0);
	::SetViewportExtEx(handle, px.cx * 254 / (10 * mm.cx), px.cy * 254 / (10 * mm.cy), 0);
	::SelectClipRgn(mfdc, NULL);
	::IntersectClipRect(mfdc, 0, 0, cx, cy);

	if(del) {
		::ReleaseDC((HWND) NULL, hdc);
		hdc = NULL;
	}

	actual_offset = Point(0, 0);

	return true;
}

bool WinMetaFileDraw::Create(i32 cx, i32 cy, tukk app, tukk name, tukk file) {
	return Create(NULL, cx, cy, app, name, file);
}

WinMetaFile WinMetaFileDraw::Close() {
	HDC hdc = Detach();
	ASSERT(hdc);
	return WinMetaFile(CloseEnhMetaFile(hdc), size);
}

WinMetaFileDraw::~WinMetaFileDraw() {
	if(handle) Close();
}

WinMetaFileDraw::WinMetaFileDraw(HDC hdc, i32 cx, i32 cy, tukk app, tukk name, tukk file) {
	Create(hdc, cx, cy, app, name, file);
}

WinMetaFileDraw::WinMetaFileDraw(i32 cx, i32 cy, tukk app, tukk name, tukk file) {
	Create(cx, cy, app, name, file);
}

Txt AsWMF(const Drawing& iw)
{
	Size sz = iw.GetSize();
	WinMetaFileDraw wd(sz.cx, sz.cy);
	wd.DrawDrawing(0, 0, sz.cx, sz.cy, iw);
	return wd.Close().Get();
}

#endif

}

#endif
