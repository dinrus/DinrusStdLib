#include "CtrlCore.h"

namespace drx {

#define LLOG(x)    // DLOG(x)
#define LTIMING(x) // RTIMING(x)

bool Ctrl::Logc::IsEmpty() const {
	return GetAlign() == SIZE ? GetB() <= GetA() : GetB() <= 0;
}

Size Ctrl::PosVal(i32 v) const {
	switch(v) {
	case MINSIZE: return GetMinSize();
	case STDSIZE: return GetStdSize();
	case MAXSIZE: return GetMaxSize();
	}
	return Size(v, v);
}

void Ctrl::Lay1(i32& pos, i32& r, i32 align, i32 a, i32 b, i32 sz) const
{
	pos = a;
	i32 size = b;
	switch(align) {
	case CENTER:
		pos = (sz - b) / 2 + a;
		break;
	case RIGHT:
		pos = sz - (a + b);
		break;
	case SIZE:
		size = sz - (a + b);
		break;
	}
	r = pos + max(size, 0);
}

Rect Ctrl::CalcRect(LogPos pos, const Rect& prect, const Rect& pview) const
{
	Rect r;
	Size sz = InFrame() ? prect.Size() : pview.Size();
	Lay1(r.left, r.right, pos.x.GetAlign(),
	     PosVal(pos.x.GetA()).cx, PosVal(pos.x.GetB()).cx, sz.cx);
	Lay1(r.top, r.bottom, pos.y.GetAlign(),
	     PosVal(pos.y.GetA()).cy, PosVal(pos.y.GetB()).cy, sz.cy);
	return r;
}

Rect Ctrl::CalcRect(const Rect& prect, const Rect& pview) const
{
	return CalcRect(pos, prect, pview);
}

Rect Ctrl::GetRect() const
{
	return rect;
}

Rect Ctrl::GetView() const
{
	GuiLock __;
	i32 n = GetFrameCount();
	return n == 0 ? Rect(Size(rect.Size())) : Rect(GetFrame0(n - 1).GetView());
}

Size Ctrl::GetSize() const
{
	return GetView().GetSize();
}

Rect  Ctrl::GetScreenRect() const
{
	GuiLock __;
	Rect r = GetRect();
	Ctrl *parent = GetParent();
	if(parent) {
		Rect pr = inframe ? parent->GetScreenRect() : parent->GetScreenView();
		r = r + pr.TopLeft();
	}
	else
		GuiPlatformGetTopRect(r);
	return r;
}

Rect  Ctrl::GetScreenView() const
{
	Rect r = GetScreenRect();
	return GetView() + r.TopLeft();
}

Rect  Ctrl::GetVisibleScreenRect() const
{
	GuiLock __;
	Rect r = GetRect();
	Ctrl *parent = GetParent();
	if(parent) {
		Rect pr = inframe ? parent->GetVisibleScreenRect() : parent->GetVisibleScreenView();
		Rect pr1 = inframe ? parent->GetScreenRect() : parent->GetScreenView();
		r = (r + pr1.TopLeft()) & pr;
	}
	else
		GuiPlatformGetTopRect(r);
	return r & GetVirtualScreenArea();
}

Rect  Ctrl::GetVisibleScreenView() const
{
	Rect r = GetVisibleScreenRect();
	return (GetView() + r.TopLeft()) & r;
}

Size  Ctrl::AddFrameSize(i32 cx, i32 cy) const
{
	GuiLock __;
	Size sz = Size(cx, cy);
	for(i32 i = GetFrameCount() - 1; i >= 0; i--)
		GetFrame0(i).frame->FrameAddSize(sz);
	return sz;
}

i32 EditFieldIsThin();

Size Ctrl::GetMinSize() const
{
	i32 fcy = Draw::GetStdFontCy();
	return AddFrameSize(fcy / 2, fcy + 2 + 2 * EditFieldIsThin());
}

Size Ctrl::GetStdSize() const
{
	Size sz = GetMinSize();
	sz.cx *= 10;
	return sz;
}

Size Ctrl::GetMaxSize() const
{
	return GetVirtualScreenArea().Size();
}

void Ctrl::SyncLayout(i32 force)
{
	GuiLock __;
	if(destroying)
		return;
	LLOG("SyncLayout " << Name() << " size: " << GetSize());
	bool refresh = false;
	Rect oview = GetView();
	Rect view = GetRect().Size();
	overpaint = OverPaint();
	i32 n = GetFrameCount();
	for(i32 i = 0; i < n; i++) {
		Frame& f = GetFrame0(i);
		f.frame->FrameLayout(view);
		if(view != f.GetView()) {
			f.SetView(view);
			refresh = true;
		}
		i32 q = f.frame->OverPaint();
		if(q > overpaint) overpaint = q;
	}
	if(oview.Size() != view.Size() || force > 1) {
		for(Ctrl& q : *this) {
			q.rect = q.CalcRect(rect, view);
			LLOG("Layout set rect " << q.Name() << " " << q.rect);
			q.SyncLayout(force > 1 ? force : 0);
		}
		Refresh();
	}
	if(oview != view || force) {
		State(LAYOUTPOS);
		Layout();
	}
	if(refresh)
		RefreshFrame();
}

void Ctrl::RefreshParentLayout()
{
	Ctrl *parent = GetParent();
	if(parent)
		parent->RefreshLayout();
}

void Ctrl::UpdateParentLayout()
{
	Ctrl *parent = GetParent();
	if(parent)
		parent->UpdateLayout();
}

i32 Ctrl::FindMoveCtrl(const VecMap<Ctrl *, MoveCtrl>& m, Ctrl *x)
{
	i32 q = m.Find(x);
	return q >= 0 && m[q].ctrl ? q : -1;
}

Ctrl::MoveCtrl *Ctrl::FindMoveCtrlPtr(VecMap<Ctrl *, MoveCtrl>& m, Ctrl *x)
{
	i32 q = FindMoveCtrl(m, x);
	return q >= 0 ? &m[q] : NULL;
}

void Ctrl::SetPos0(LogPos p, bool _inframe)
{
	GuiLock __;
	if(p == pos && inframe == _inframe) return;
	Ctrl *parent = GetParent();
	if(parent && !IsDHCtrl()) {
		if(!globalbackbuffer) {
			Rect from = GetRect().Size();
			Top *top = GetTopRect(from, true)->GetTop();
			if(top) {
				LTIMING("SetPos0 MoveCtrl");
				pos = p;
				inframe = _inframe;
				Rect to = GetRect().Size();
				UpdateRect0();
				GetTopRect(to, true);
				MoveCtrl *s = FindMoveCtrlPtr(top->scroll_move, this);
				if(s && s->from == from && s->to == to) {
					s->ctrl = NULL;
					LLOG("SetPos Matched " << from << " -> " << to);
				}
				else {
					MoveCtrl& m = top->move.Add(this);
					m.ctrl = this;
					m.from = from;
					m.to = to;
					LLOG("SetPos Add " << DRX::Name(this) << from << " -> " << to);
				}
				StateH(POSITION);
				return;
			}
		}
		RefreshFrame();
	}
	pos = p;
	inframe = _inframe;
	UpdateRect();
	StateH(POSITION);
}

void Ctrl::UpdateRect0(bool sync)
{
	GuiLock __;
	LTIMING("UpdateRect0");
	Ctrl *parent = GetParent();
	if(parent)
		rect = CalcRect(parent->GetRect(), parent->GetView());
	else {
		static Rect pwa;
		ONCELOCK {
			pwa = GetPrimaryWorkArea();
		}
		rect = CalcRect(pwa, pwa);
	}
	LLOG("UpdateRect0 " << Name() << " to " << rect);
	LTIMING("UpdateRect0 SyncLayout");
	if(sync)
		SyncLayout();
}


void Ctrl::UpdateRect(bool sync)
{
	GuiLock __;
	UpdateRect0(sync);
	if(GetParent()) RefreshFrame();
}

Ctrl& Ctrl::SetPos(LogPos p, bool _inframe)
{
	GuiLock __;
	Ctrl *parent = GetParent();
	if(p != pos || inframe != _inframe) {
		if(parent || !IsOpen())
			SetPos0(p, _inframe);
		else {
			Rect wa = GetWorkArea();
			WndSetPos(CalcRect(p, wa, wa));
			StateH(POSITION);
		}
	}
	return *this;
}

Ctrl& Ctrl::SetPos(LogPos p)
{
	return SetPos(p, false);
}

Ctrl& Ctrl::SetPosX(Logc x)
{
	return SetPos(LogPos(x, pos.y));
}

Ctrl& Ctrl::SetPosY(Logc y)
{
	return SetPos(LogPos(pos.x, y));
}

Ctrl& Ctrl::SetFramePos(LogPos p)
{
	return SetPos(p, true);
}

Ctrl& Ctrl::SetFramePosX(Logc x) {
	return SetPos(LogPos(x, pos.y), true);
}

Ctrl& Ctrl::SetFramePosY(Logc y) {
	return SetPos(LogPos(pos.x, y), true);
}

void  Ctrl::SetRect(i32 x, i32 y, i32 cx, i32 cy)
{
	LLOG("SetRect " << Name() << " rect: " << RectC(x, y, cx, cy));
	auto clampc = [](i32 c) { return clamp(c, -10000, 10000); }; // Logc vals only have 15 bits
	SetPos(PosLeft(clampc(x), clampc(cx)), PosTop(clampc(y), clampc(cy)));
}

void  Ctrl::SetWndRect(const Rect& r)
{
	LLOG("SetWndRect " << Name() << " rect: " << r << " (Ctrl::GetRect = " << GetRect() << ")");
	SetPos0(LogPos(PosLeft(r.left, r.Width()), PosTop(r.top, r.Height())), false);
}

void Ctrl::SetRect(const Rect& r)
{
	LLOG("SetRect " << Name() << " rect: " << r << " (Ctrl::GetRect = " << GetRect() << ")");
	SetRect(r.left, r.top, r.Width(), r.Height());
}

void Ctrl::SetRectX(i32 x, i32 cx) {
	SetPosX(PosLeft(x, cx));
}

void Ctrl::SetRectY(i32 y, i32 cy) {
	SetPosY(PosTop(y, cy));
}

void  Ctrl::SetFrameRect(i32 x, i32 y, i32 cx, i32 cy) {
	SetFramePos(PosLeft(x, cx), PosTop(y, cy));
}

void Ctrl::SetFrameRect(const Rect& r) {
	SetFrameRect(r.left, r.top, r.Width(), r.Height());
}

void Ctrl::SetFrameRectX(i32 x, i32 cx) {
	SetFramePosX(PosLeft(x, cx));
}

void Ctrl::SetFrameRectY(i32 y, i32 cy) {
	SetFramePosY(PosTop(y, cy));
}

Ctrl& Ctrl::LeftPos(i32 a, i32 size) {
	return SetPosX(PosLeft(a, size));
}

Ctrl& Ctrl::RightPos(i32 a, i32 size) {
	return SetPosX(PosRight(a, size));
}

Ctrl& Ctrl::TopPos(i32 a, i32 size) {
	return SetPosY(PosTop(a, size));
}

Ctrl& Ctrl::BottomPos(i32 a, i32 size) {
	return SetPosY(PosBottom(a, size));
}

Ctrl& Ctrl::HSizePos(i32 a, i32 b) {
	return SetPosX(PosSize(a, b));
}

Ctrl& Ctrl::VSizePos(i32 a, i32 b) {
	return SetPosY(PosSize(a, b));
}

Ctrl& Ctrl::SizePos() {
	return HSizePos().VSizePos();
}

Ctrl& Ctrl::HCenterPos(i32 size, i32 delta) {
	return SetPosX(PosCenter(size, delta));
}

Ctrl& Ctrl::VCenterPos(i32 size, i32 delta) {
	return SetPosY(PosCenter(size, delta));
}

Ctrl& Ctrl::LeftPosZ(i32 a, i32 size) {
	return LeftPos(HorzLayoutZoom(a), HorzLayoutZoom(size));
}

Ctrl& Ctrl::RightPosZ(i32 a, i32 size) {
	return RightPos(HorzLayoutZoom(a), HorzLayoutZoom(size));
}

Ctrl& Ctrl::TopPosZ(i32 a, i32 size) {
	return TopPos(VertLayoutZoom(a), VertLayoutZoom(size));
}

Ctrl& Ctrl::BottomPosZ(i32 a, i32 size) {
	return BottomPos(VertLayoutZoom(a), VertLayoutZoom(size));
}

Ctrl& Ctrl::HSizePosZ(i32 a, i32 b) {
	return HSizePos(HorzLayoutZoom(a), HorzLayoutZoom(b));
}

Ctrl& Ctrl::VSizePosZ(i32 a, i32 b) {
	return VSizePos(VertLayoutZoom(a), VertLayoutZoom(b));
}

Ctrl& Ctrl::HCenterPosZ(i32 size, i32 delta) {
	return HCenterPos(HorzLayoutZoom(size), HorzLayoutZoom(delta));
}

Ctrl& Ctrl::VCenterPosZ(i32 size, i32 delta) {
	return VCenterPos(VertLayoutZoom(size), VertLayoutZoom(delta));
}

Rect Ctrl::GetWorkArea(Point pt)
{
	GuiLock __;
	static Array<Rect> rc;
	if (rc.IsEmpty())
		GetWorkArea(rc);
	for(i32 i = 0; i < rc.GetCount(); i++)
		if(rc[i].Contains(pt))
			return rc[i];
	return GetPrimaryWorkArea();
}

Rect Ctrl::StdGetWorkArea() const
{
	GuiLock __;

	static Array<Rect> rc;
	if(rc.IsEmpty())
		GetWorkArea(rc);
	
	const Ctrl *top = GetTopCtrl();
	if(top && top->IsOpen())
		return GetWorkArea(top->GetScreenRect().TopLeft());
	return GetPrimaryWorkArea();
}

}
