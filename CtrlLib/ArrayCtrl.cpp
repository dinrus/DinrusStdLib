#include "CtrlLib.h"

namespace drx {

#define LTIMING(x)  // DTIMING(x)

ArrayCtrl::Column::Column() {
	convert = NULL;
	edit = NULL;
	display = &StdDisplay();
	accel = NULL;
	margin = -1;
	cached = false;
	clickedit = true;
	index = -1;
	order = NULL;
}

ArrayCtrl::Column& ArrayCtrl::Column::Cache() {
	cached = true;
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::NoEdit()
{
	if(edit)
		edit->Remove();
	edit = NULL;
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::Edit(Ctrl& e) {
	e.Hide();
	e.SetFrame(NullFrame());
	edit = &e;
	arrayctrl->AddChild(edit);
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::SetConvert(const Convert& c) {
	convert = &c;
	ClearCache();
	arrayctrl->Refresh();
	arrayctrl->SyncInfo();
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::ConvertBy(Function<Val(const Val&)> cv)
{
	convertby = cv;
	ClearCache();
	arrayctrl->Refresh();
	arrayctrl->SyncInfo();
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::SetFormat(tukk fmt)
{
	FormatConvert::SetFormat(fmt);
	return SetConvert(*this);
}

ArrayCtrl::Column& ArrayCtrl::Column::SetDisplay(const Display& d)
{
	display = &d;
	ClearCache();
	arrayctrl->SyncCtrls();
	arrayctrl->Refresh();
	arrayctrl->SyncInfo();
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::Ctrls(Event<i32, One<Ctrl>&> factory)
{
	return WithLined(factory);
}

ArrayCtrl::Column& ArrayCtrl::Column::WithLined(Event<i32, One<Ctrl>&> f)
{
	factory = f;
	arrayctrl->hasctrls = arrayctrl->headerctrls = true;
	arrayctrl->SyncCtrls();
	arrayctrl->Refresh();
	arrayctrl->SyncInfo();
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::With(Event<One<Ctrl>&> factory)
{
	return WithLined([=](i32, One<Ctrl>& x) { factory(x); });
}

ArrayCtrl::Column& ArrayCtrl::Column::Ctrls(Callback1<One<Ctrl>&> _factory)
{
	return Ctrls([=](i32, One<Ctrl>& x) { _factory(x); });
}

void ArrayCtrl::Column::ClearCache() {
	cache.Clear();
}

void ArrayCtrl::Column::Sorts()
{
	HeaderTab().WhenAction = callback1(arrayctrl, &ArrayCtrl::ToggleSortColumn, index);
}

ArrayCtrl::Column& ArrayCtrl::Column::Sorting(const ValOrder& o)
{
	order = &o;
	cmp.Clear();
	line_order.Clear();
	Sorts();
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::SortingBy(Function<i32 (const Val& a, const Val& b)> c)
{
	cmp = c;
	order = NULL;
	line_order.Clear();
	Sorts();
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::SortingLined(Gate<i32, i32> aorder)
{
	line_order = aorder;
	order = NULL;
	cmp.Clear();
	Sorts();
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::Sorting()
{
	return SortingBy(StdValCompare);
}

ArrayCtrl::Column& ArrayCtrl::Column::SortDefault(bool desc)
{
	if(!cmp && !order && !line_order)
		Sorting();
	arrayctrl->SetSortColumn(index, desc);
	return *this;
}

void ArrayCtrl::Column::InvalidateCache(i32 i) {
	if(i < 0) return;
	if(cache.Is< Vec<Txt> >() && i < cache.Get< Vec<Txt> >().GetCount())
		cache.Get< Vec<Txt> >()[i] = Txt::GetVoid();
	if(cache.Is< Vec<Val> >() && i < cache.Get< Vec<Val> >().GetCount())
		cache.Get< Vec<Val> >()[i] = Val();
}

void ArrayCtrl::Column::InsertCache(i32 i, i32 count) {
	if(i < 0) return;
	if(cache.Is< Vec<Txt> >() && i < cache.Get< Vec<Txt> >().GetCount()) {
		Vec<Txt>& v = cache.Get< Vec<Txt> >();
		v.InsertN(i, count);
		while(count--)
			v[i++] = Txt::GetVoid();
	}
	if(cache.Is< Vec<Val> >() && i < cache.Get< Vec<Val> >().GetCount())
		cache.Get< Vec<Val> >().Insert(i);
}

void ArrayCtrl::Column::RemoveCache(i32 i) {
	if(i < 0) return;
	if(cache.Is< Vec<Txt> >() && i < cache.Get< Vec<Txt> >().GetCount())
		cache.Get< Vec<Txt> >().Remove(i);
	if(cache.Is< Vec<Val> >() && i < cache.Get< Vec<Val> >().GetCount())
		cache.Get< Vec<Val> >().Remove(i);
}

ArrayCtrl::Column& ArrayCtrl::Column::InsertVal(const Val& v) {
	arrayctrl->IndexInfo(arrayctrl->Pos(pos[0])).InsertVal(v);
	return *this;
}

ArrayCtrl::Column& ArrayCtrl::Column::InsertVal(ValGen& g) {
	arrayctrl->IndexInfo(arrayctrl->Pos(pos[0])).InsertVal(g);
	return *this;
}

HeaderCtrl::Column& ArrayCtrl::Column::HeaderTab() {
	return arrayctrl->header.Tab(arrayctrl->header.FindIndex(index));
}

const HeaderCtrl::Column& ArrayCtrl::Column::HeaderTab() const
{
	return arrayctrl->header.Tab(arrayctrl->header.FindIndex(index));
}

void ArrayCtrl::InvalidateCache(i32 ri)
{
	for(i32 i = 0; i < column.GetCount(); i++)
		column[i].InvalidateCache(ri);
}

void ArrayCtrl::CellCtrl::LeftDown(Point, dword)
{
	if(ctrl->IsWantFocus())
		ctrl->SetFocus();
}

void ArrayCtrl::CellInfo::Free()
{
	if(ptr.GetBit()) {
		CellCtrl *cc = (CellCtrl *)ptr.GetPtr();
		if(cc->owned)
			delete cc->ctrl;
		delete cc;
	}
}

void ArrayCtrl::CellInfo::Set(Ctrl *ctrl, bool owned, bool value)
{
	Free();
	CellCtrl *cc = new CellCtrl;
	cc->ctrl = ctrl;
	cc->Add(*ctrl);
	cc->owned = owned;
	cc->value = value;
	ptr.Set1(cc);
}

ArrayCtrl::CellInfo::CellInfo(CellInfo&& s)
{
	ptr = s.ptr;
	const_cast<CellInfo&>(s).ptr.SetPtr(NULL);
	const_cast<CellInfo&>(s).ptr.SetBit(0);
}

ArrayCtrl::CellInfo::~CellInfo()
{
	Free();
}

Ctrl& ArrayCtrl::SetCtrl(i32 i, i32 j, Ctrl *newctrl, bool owned, bool value)
{
	if(value)
		newctrl->SetData(GetColumn(i, j));
	hasctrls = true;
	RefreshRow(i);
	CellInfo& ci = cellinfo.At(i).At(j);
	ci.Set(newctrl, owned, value);
	Ctrl& c = ci.GetCtrl();
	if(newctrl->GetPos().x.GetAlign() == LEFT && newctrl->GetPos().x.GetB() == 0)
		newctrl->HSizePos().VCenterPos(STDSIZE);
	j--;
	for(;;) {
		while(j >= 0) {
			if(IsCtrl(i, j)) {
				AddChild(&c, &GetCellCtrl(i, j));
				SyncInfo();
				return c;
			}
			j--;
		}
		i--;
		if(i < 0) {
			AddChild(&c, NULL);
			SyncInfo();
			return c;
		}
		j = cellinfo[i].GetCount();
	}
	SyncInfo();
}

void  ArrayCtrl::SetCtrl(i32 i, i32 j, Ctrl& ctrl, bool value)
{
	SetCtrl(i, j, &ctrl, false, value);
	SyncLineCtrls(i);
}

Ctrl *ArrayCtrl::GetCtrl(i32 i, i32 col)
{
	SyncLineCtrls(i);
	if(IsCtrl(i, col))
		return GetCellCtrl(i, col).ctrl;
	return NULL;
}

bool  ArrayCtrl::IsCtrl(i32 i, i32 j) const
{
	return i < cellinfo.GetCount() && j < cellinfo[i].GetCount() && cellinfo[i][j].IsCtrl();
}

ArrayCtrl::CellCtrl& ArrayCtrl::GetCellCtrl(i32 i, i32 j)
{
	LTIMING("GetCellCtrl");
	return cellinfo[i][j].GetCtrl();
}

const ArrayCtrl::CellCtrl& ArrayCtrl::GetCellCtrl(i32 i, i32 j) const
{
	return cellinfo[i][j].GetCtrl();
}

void ArrayCtrl::SetDisplay(i32 i, i32 j, const Display& d)
{
	cellinfo.At(i).At(j).Set(d);
	RefreshRow(i);
}

void ArrayCtrl::SetRowDisplay(i32 i, const Display& d)
{
	if(i >= 0 && i < GetCount())
		for (i32 j = 0 ; j < GetColumnCount(); j++)
			this->cellinfo.At(i).At(j).Set(d);
	RefreshRow(i);
}

void ArrayCtrl::SetColumnDisplay(i32 j, const Display& d)
{
	if(j >= 0 && j < GetColumnCount())
		for (i32 i = 0 ; i < GetCount(); i++) {
			this->cellinfo.At(i).At(j).Set(d);
			RefreshRow(i);
		}
}

const Display& ArrayCtrl::GetDisplay(i32 i, i32 j)
{
	if(i < cellinfo.GetCount() && j < cellinfo[i].GetCount() && cellinfo[i][j].IsDisplay())
		return cellinfo[i][j].GetDisplay();
	return *column[j].display;
}

const Display& ArrayCtrl::GetDisplay(i32 j)
{
	return *column[j].display;
}

i32 ArrayCtrl::Pos(i32 pos) const {
	if(pos >= 0) return pos;
	pos = idx.Find(id_ndx[-pos]);
	ASSERT(pos >= 0);
	return pos;
}

Val ArrayCtrl::Get0(i32 i, i32 ii) const {
	ASSERT(ii >= 0);
	if(hasctrls)
		for(i32 j = 0; j < column.GetCount(); j++)
			if(IsCtrl(i, j)) {
				const Column& m = column[j];
				ASSERT(m.pos.GetCount() == 1);
				if(Pos(m.pos[0]) == ii) {
					const CellCtrl& c = GetCellCtrl(i, j);
					if(c.value)
						return c.ctrl->GetData();
				}
			}
	if(i < array.GetCount()) {
		const Vec<Val>& v = array[i].line;
		if(ii < v.GetCount())
			return v[ii];
	}
	return Null;
}

Val ArrayCtrl::Get(i32 ii) const {
	ASSERT(IsCursor());
	i32 i;
	for(i = 0; i < column.GetCount(); i++) {
		const Column& m = column[i];
		if(m.edit)
			for(i32 j = 0; j < m.pos.GetCount(); j++)
				if(Pos(m.pos[j]) == ii) {
					Val v = m.edit->GetData();
					return m.pos.GetCount() > 1 && IsValArray(v) ? ValArray(v)[j] : v;
				}
	}
	for(i = 0; i < control.GetCount(); i++) {
		const Control& c = control[i];
		if(Pos(c.pos) == ii)
			return c.ctrl->GetData();
	}
	return Get0(cursor, ii);
}

Val ArrayCtrl::Get(const Id& id) const {
	return Get(GetPos(id));
}

Val ArrayCtrl::GetOriginal(i32 ii) const {
	return Get0(cursor, ii);
}

Val ArrayCtrl::GetOriginal(const Id& id) const {
	return GetOriginal(GetPos(id));
}

Val ArrayCtrl::GetKey() const {
	return IsCursor() ? Get(0) : Null;
}

Val ArrayCtrl::GetOriginalKey() const {
	return IsCursor() ? GetOriginal(0) : Null;
}

bool ArrayCtrl::IsModified(i32 ii) const {
	ASSERT(IsCursor());
	i32 i;
	if(ii < modify.GetCount() && modify[ii]) return true;
	for(i = 0; i < column.GetCount(); i++) {
		const Column& m = column[i];
		if(m.edit)
			for(i32 j = 0; j < m.pos.GetCount(); j++)
				if(Pos(m.pos[j]) == ii && m.edit->IsModified())
					return true;
	}
	for(i = 0; i < control.GetCount(); i++) {
		const Control& c = control[i];
		if(Pos(c.pos) == ii && c.ctrl->IsModified())
			return true;
	}
	return false;
}

bool ArrayCtrl::IsModified(const Id& id) const {
	return IsModified(GetPos(id));
}

void ArrayCtrl::ColEditSetData(i32 j) {
	ASSERT(IsCursor());
	Column& m = column[j];
	if(m.edit)
		m.edit->SetData(GetColumn(cursor, j));
}

void ArrayCtrl::CtrlSetData(i32 j) {
	ASSERT(IsCursor());
	Control& c = control[j];
	c.ctrl->Enable();
	if(IsNull(c.pos))
		c.ctrl->SetData(cursor);
	else
		c.ctrl->SetData(GetOriginal(Pos(c.pos)));
}

void ArrayCtrl::SetCtrlVal(i32 i, i32 ii, const Val& v)
{
	if(hasctrls)
		for(i32 j = 0; j < column.GetCount(); j++)
			if(IsCtrl(i, j)) {
				const Column& m = column[j];
				ASSERT(m.pos.GetCount() == 1);
				if(Pos(m.pos[0]) == ii) {
					const CellCtrl& c = GetCellCtrl(i, j);
					if(c.value)
						c.ctrl->SetData(v);
					return;
				}
			}
}

void ArrayCtrl::Set(i32 ii, const Val& v) {
	ASSERT(IsCursor());
	array.At(cursor).line.At(ii) = v;
	i32 i;
	for(i = 0; i < column.GetCount(); i++) {
		Column& m = column[i];
		if(m.edit)
			for(i32 j = 0; j < m.pos.GetCount(); j++)
				if(Pos(m.pos[j]) == ii) {
					ColEditSetData(i);
					break;
				}
	}
	for(i = 0; i < control.GetCount(); i++) {
		const Control& c = control[i];
		if(Pos(c.pos) == ii)
			CtrlSetData(i);
	}
	SetCtrlVal(cursor, ii, v);
	modify.At(ii, false) = true;
	InvalidateCache(cursor);
	RefreshRow(cursor);
	WhenArrayAction();
}

void ArrayCtrl::Set(const Id& id, const Val& v) {
	Set(GetPos(id), v);
}

void ArrayCtrl::Set0(i32 i, i32 ii, const Val& v) {
	if(i == cursor)
		Set(ii, v);
	else
		SetCtrlVal(i, ii, v);
	array.At(i).line.At(ii) = v;
}

void  ArrayCtrl::Set(i32 i, const Vec<Val>& v) {
	array.At(i).line.Clear();
	for(i32 ii = 0; ii < v.GetCount(); ii++)
		Set0(i, ii, v[ii]);
	AfterSet(i);
	RefreshRow(i);
	WhenArrayAction();
}

void  ArrayCtrl::Set(i32 i, Vec<Val>&& v) {
	Vec<Val>& line = array.At(i).line;
	if(hasctrls || i == cursor) {
		line.Clear();
		for(i32 ii = 0; ii < v.GetCount(); ii++)
			Set0(i, ii, v[ii]);
	}
	else
		line = pick(v);
	AfterSet(i);
	RefreshRow(i);
	WhenArrayAction();
}

void ArrayCtrl::AfterSet(i32 i, bool sync_ctrls)
{
	SetSb();
	Refresh();
	SyncInfo();
	if(sync_ctrls)
		SyncLineCtrls(i);
	InvalidateCache(i);
}

void ArrayCtrl::Set(i32 i, i32 ii, const Val& v)
{
	Set0(i, ii, v);
	bool sync_ctrls = false;
	for(i32 col : FindColumnsWithPos(ii))
		if(IsCtrl(i, col))
			sync_ctrls = true;
	AfterSet(i, sync_ctrls);
	WhenArrayAction();
}

void ArrayCtrl::Set(i32 i, const Id& id, const Val& v) {
	Set(i, GetPos(id), v);
}

Val ArrayCtrl::Get(i32 i, i32 ii) const {
	return i == cursor ? Get(ii) : Get0(i, ii);
}

Val ArrayCtrl::Get(i32 i, const Id& id) const {
	return Get(i, GetPos(id));
}

void  ArrayCtrl::SetCount(i32 n) {
	ASSERT(n >= 0);
	if(cursor >= n)
		CancelCursor();
	array.SetCount(n);
	if(cellinfo.GetCount() > array.GetCount())
		cellinfo.SetCount(array.GetCount());
	Refresh();
	SyncInfo();
	SyncCtrls();
	SetSb();
	PlaceEdits();
	WhenArrayAction();
}

Val ArrayCtrl::GetColumn(i32 row, i32 col) const {
	const Column& c = column[col];
	if(c.pos.GetCount() == 0) return row;
	if(c.pos.GetCount() == 1) return Get0(row, Pos(c.pos[0]));
	ValMap vm;
	for(i32 i = 0; i < c.pos.GetCount(); i++) {
		i32 ii = Pos(c.pos[i]);
		vm.Add(ii < idx.GetCount() ? idx.GetKey(ii) : Id(), Get0(row, ii));
	}
	return vm;
}

Val ArrayCtrl::GetConvertedColumn(i32 i, i32 col) const {
	LTIMING("GetConvertedColumn");
	const Column& m = column[col];
	Val v = GetColumn(i, col);
	if(!m.convert && !m.convertby) return v;
	if(m.cache.Is< Vec<Txt> >() && i < m.cache.Get< Vec<Txt> >().GetCount()) {
		const Txt& s = m.cache.Get< Vec<Txt> >()[i];
		if(!s.IsVoid()) return s;
	}
	if(m.cache.Is< Vec<Val> >() && i < m.cache.Get< Vec<Val> >().GetCount()) {
		const Val& v = m.cache.Get< Vec<Val> >()[i];
		if(!v.IsVoid()) return v;
	}
	if(m.convertby)
		v = m.convertby(v);
	if(m.convert)
		v = m.convert->Format(v);
	if(m.cached) {
		if(m.cache.IsEmpty())
			m.cache.Create< Vec<Txt> >();
		if(IsTxt(v) && m.cache.Is< Vec<Txt> >())
			m.cache.Get< Vec<Txt> >().At(i, Txt::GetVoid()) = v;
		if(!IsTxt(v) && m.cache.Is< Vec<Txt> >())
			m.cache.Create< Vec<Val> >();
		if(m.cache.Is< Vec<Val> >())
			m.cache.Get< Vec<Val> >().At(i) = v;
		ASSERT(m.pos.GetCount() || m.cache.IsEmpty());
	}
	return v;
}

i32 ArrayCtrl::GetCount() const {
	return max(virtualcount, array.GetCount());
}

void ArrayCtrl::SetVirtualCount(i32 c) {
	virtualcount = c;
	Refresh();
	SyncInfo();
	SetSb();
	if(cursor >= virtualcount)
		GoEnd();
}

void ArrayCtrl::SetSb() {
	sb.SetTotal(GetTotalCy() + IsInserting() * (GetLineCy() + horzgrid));
	sb.SetPage(GetSize().cy);
	MinMaxLine();
}

void ArrayCtrl::Layout() {
	SyncColumnsPos();
	SetSb();
	HeaderScrollVisibility();
	PlaceEdits();
	SyncCtrls();
}

void ArrayCtrl::Reline(i32 i, i32 y)
{
	i32 i0 = i;
	if(WhenLineVisible)
		while(i < ln.GetCount()) {
			Ln& p = ln[i];
			p.y = y;
			if(IsLineVisible(i))
				y += Nvl(p.cy, linecy) + horzgrid;
			i++;
		}
	else {
		while(i < ln.GetCount()) {
			Ln& p = ln[i];
			p.y = y;
			if(IsLineVisible0(i))
				y += Nvl(p.cy, linecy) + horzgrid;
			i++;
		}
	}
	SetSb();
	Refresh();
	SyncCtrls(i0);
}

i32  ArrayCtrl::GetLineY(i32 i) const
{
	return i < ln.GetCount() ? ln[i].y
	                         : (ln.GetCount() ? ln.Top().y + IsLineVisible(ln.GetCount() - 1) * (Nvl(ln.Top().cy, linecy) + horzgrid) : 0)
	                           + (linecy + horzgrid) * (i - ln.GetCount());
}

Rect ArrayCtrl::GetScreenCellRect(i32 i, i32 col) const
{
	return GetCellRect(i, col).Offseted(GetScreenView().TopLeft());
}

Rect ArrayCtrl::GetScreenCellRectM(i32 i, i32 col) const
{
	return GetCellRectM(i, col).Offseted(GetScreenView().TopLeft());
}

ArrayCtrl& ArrayCtrl::SetLineCy(i32 cy)
{
	linecy = cy;
	Reline(0, 0);
	sb.SetLine(cy);
	Refresh();
	return *this;
}

void ArrayCtrl::SetLineCy(i32 i, i32 cy)
{
	i32 q = ln.GetCount();
	if(i < q) {
		ln[i].cy = cy;
		Reline(i, ln[i].y);
	}
	else {
		while(ln.GetCount() <= i)
			ln.Add().cy = Null;
		ln[i].cy = cy;
		if(q > 0)
			Reline(q - 1, ln[q - 1].y);
		else
			Reline(0, 0);
	}
}

void ArrayCtrl::SetLineColor(i32 i, Color c)
{
	Color& lc = array.At(i).paper;
	if(lc != c) {
		lc = c;
		RefreshRow(i);
	}
}

i32  ArrayCtrl::GetTotalCy() const
{
	i32 i = GetCount();
	return i ? GetLineY(i - 1) + GetLineCy(i - 1) + horzgrid: 0;
}

i32  ArrayCtrl::GetLineCy(i32 i) const {
	return i < ln.GetCount() ? Nvl(ln[i].cy, linecy) : linecy;
}

i32  ArrayCtrl::GetLineAt(i32 y) const {
	LTIMING("GetLineAt");
	if(!GetCount()) return Null;
	if(y < 0 || y >= GetTotalCy()) return Null;
	i32 l = 0;
	i32 h = GetCount();
	while(l < h) {
		i32 m = (l + h) / 2;
		i32 yy = GetLineY(m);
		if(yy == y) return m;
		if(yy < y)
			l = m + 1;
		else
			h = m;
	}
	return l > 0 ? l - 1 : l;
}

Ctrl *ArrayCtrl::SyncLineCtrls(i32 i, Ctrl *p)
{
	if(!hasctrls)
		return NULL;
	Size sz = GetSize();
	bool visible = IsLineVisible(i);
	for(i32 j = 0; j < column.GetCount(); j++) {
		bool ct = IsCtrl(i, j);
		if(!ct && column[j].factory) {
			LTIMING("Create");
			One<Ctrl> newctrl;
			column[j].factory(i, newctrl);
			newctrl->SetData(GetColumn(i, j));
			newctrl->WhenAction << Proxy(WhenCtrlsAction);
			if(newctrl->GetPos().x.GetAlign() == LEFT && newctrl->GetPos().x.GetB() == 0)
				newctrl->HSizePos().VCenterPos(STDSIZE);
			CellInfo& ci = cellinfo.At(i).At(j);
			ci.Set(newctrl.Detach(), true, true);
			ct = true;
		}
		if(ct) {
			LTIMING("PlaceCtrls");
			Ctrl& c = GetCellCtrl(i, j);
			if(!c.HasFocusDeep() || c.GetParent() != this) {
				for(i32 ii = i - 1; ii >= 0 && !p; ii--) // find previous Ctrl for the correct order
					for(i32 j = column.GetCount() - 1; j >= 0; j--) {
						if(IsCtrl(ii, j)) {
							p = &GetCellCtrl(ii, j);
							break;
						}
						if(column[j].factory) { // was not created yet...
							for(i32 q = 0; q <= i; q++)
								p = SyncLineCtrls(q, p);
							return p;
						}
					}
				AddChild(&c, p);
			}
			p = &c;
			Rect r;
			if(i < min_visible_line || i > max_visible_line || !visible)
				r.bottom = r.top = -1;
			else
				r = GetCellRectM(i, j);
			if(r.bottom < 0 || r.top > sz.cy) {
				if(c.GetRect().top != -100000)
					c.SetRect(-1000, -100000, 1, 1);
			}
			else {
				c.SetRect(r);
				ctrl_low = min(ctrl_low, i);
				ctrl_high = max(ctrl_high, i);
			}
		}
	}
	return p;
}

void  ArrayCtrl::SyncPageCtrls()
{
	LTIMING("SyncPageCtrls");
	if(!hasctrls)
		return;
	Ctrl *p = NULL;
	for(i32 i = max(ctrl_low, 0); i <= min(ctrl_high, GetCount() - 1); i++)
		p = SyncLineCtrls(i, p);
	ctrl_low = GetCount() - 1;
	ctrl_high = 0;
	p = NULL;
	for(i32 i = min_visible_line; i <= min(max_visible_line, GetCount() - 1); i++)
		p = SyncLineCtrls(i, p);
}

void  ArrayCtrl::SyncCtrls(i32 i0)
{
	LTIMING("SyncCtrls");
	if(!hasctrls)
		return;
	ctrl_low = GetCount() - 1;
	ctrl_high = 0;
	Ctrl *p = NULL;
	for(i32 i = i0; i < GetCount(); i++)
		p = SyncLineCtrls(i, p);
}

Point ArrayCtrl::FindCellCtrl(Ctrl *ctrl)
{
	if(hasctrls)
		for(i32 i = 0; i < cellinfo.GetCount(); i++) {
			Vec<CellInfo>& ci = cellinfo[i];
			for(i32 j = 0; j < ci.GetCount(); j++)
				if(IsCtrl(i, j)) {
					CellCtrl& c = GetCellCtrl(i, j);
					if(&c == ctrl || c.ctrl == ctrl || c.ctrl->HasChildDeep(ctrl))
						return Point(j, i);
				}
		}
	return Null;
}

void ArrayCtrl::ChildGotFocus()
{
	if(cursor >= 0)
		RefreshRow(cursor);
	if(!acceptingrow) {
		Point p = FindCellCtrl(GetFocusCtrl());
		if(!IsNull(p)) {
			if(nocursor)
				ScrollInto(p.y);
			else
			if(p.y != cursor) // avoid setting cursor if it is already there - important for multiselect
				SetCursor(p.y);
		}
	}
	Ctrl::ChildGotFocus();
}

void ArrayCtrl::ChildLostFocus()
{
	if(cursor >= 0)
		RefreshRow(cursor);
	Ctrl::ChildLostFocus();
}

const Display& ArrayCtrl::GetCellInfo(i32 i, i32 j, bool f0,
                                      Val& v, Color& fg, Color& bg, dword& st)
{
	st = 0;
	bool hasfocus = f0 && !isdrag;
	bool drop = i == dropline && (dropcol == DND_LINE || dropcol == j);
	if(IsReadOnly())
		st |= Display::READONLY;
	if(i < array.GetCount() && array[i].select)
		st |= Display::SELECT;
	if(i == cursor && !nocursor)
		st |= Display::CURSOR;
	if(drop) {
		hasfocus = true;
		st |= Display::CURSOR;
	}
	if(hasfocus)
		st |= Display::FOCUS;
	bg = i & 1 ? evenpaper : oddpaper;
	if(nobg)
		bg = Null;
	if(i < array.GetCount() && !IsNull(array[i].paper))
		bg = array[i].paper;
	fg = i & 1 ? evenink : oddink;
	if((st & Display::SELECT) ||
	    !multiselect && (st & Display::CURSOR) && !nocursor ||
	    drop) {
		fg = hasfocus ? SColorHighlightText : SColorText;
		bg = hasfocus ? SColorHighlight : Blend(SColorDisabled, SColorPaper);
	}
	v = IsCtrl(i, j) && GetCellCtrl(i, j).value ? Null : GetConvertedColumn(i, j);
	return GetDisplay(i, j);
}

void ArrayCtrl::SpanWideCell(const Display& d, const Val& q, i32 cm, i32& cw, Rect& r, i32 i, i32& j)
{
	i32 cx = d.GetStdSize(q).cx;
	while(cx > r.Width() - 2 * cm && j + 1 < column.GetCount()) {
		Val v = GetConvertedColumn(i, j + 1);
		if(!IsNull(v))
			break;
		j++;
		cw += header.GetTabWidth(j);
		r.right = r.left + cw - vertgrid + (j == column.GetCount() - 1);
	}
}

Size  ArrayCtrl::DoPaint(Draw& w, bool sample) {
	LTIMING("Paint");
	SyncColumnsPos();
	bool hasfocus0 = HasFocusDeep() || sample;
	Size size = sample ? StdSampleSize() : GetSize();
	Rect r;
	r.bottom = 0;
	i32 i = sample ? 0 : GetLineAt(sb);
	i32 xs = sample ? 0 : -header.GetScroll();
	i32 js;
	for(js = 0; js < column.GetCount(); js++) {
		i32 cw = header.GetTabWidth(js);
		if ((xs + cw - vertgrid + (js == column.GetCount() - 1)) >= 0)
			break;
		xs += cw;
	}
	i32 sy = 0;
	if(!IsNull(i))
		while(i < GetCount()) {
			if((!sample || i == cursor || i < array.GetCount() && array[i].select) && IsLineVisible(i)) {
				r.top = sample ? sy : GetLineY(i) - sb;
				if(r.top > size.cy)
					break;
				r.bottom = r.top + GetLineCy(i);
				i32 x = xs;
				dword st = 0;
				if(r.bottom > r.top)
					for(i32 j = js; j < column.GetCount(); j++) {
						i32 cw = header.GetTabWidth(j);
						i32 jj = header.GetTabIndex(j);
						i32 cm = column[jj].margin;
						if(cm < 0)
							cm = header.Tab(j).GetMargin();
						if(x > size.cx) break;
						r.left = x;
						r.right = x + cw - vertgrid + (jj == column.GetCount() - 1);
						dword st;
						Color fg, bg;
						Val q;
						bool heading = IsHeading(i);
						const Display& d = GetCellInfo(i, jj, hasfocus0, q, fg, bg, st);
						if(sample || w.IsPainting(r)) {
							if(heading)
								r.right = cw = size.cx;
							else
							if(spanwidecells)
								SpanWideCell(d, q, cm, cw, r, i, j);

							if(cw < 2 * cm || editmode && i == cursor && column[jj].edit)
								d.PaintBackground(w, r, q, fg, bg, st);
							else {
								d.PaintBackground(w, RectC(r.left, r.top, cm, r.Height()), q, fg, bg, st);
								r.left += cm;
								r.right -= cm;
								d.PaintBackground(w, RectC(r.right, r.top, cm, r.Height()), q, fg, bg, st);
								w.Clip(r);
								d.Paint(w, r, q, fg, bg, st);
								w.End();
							}
						}
						x += cw;
						if(vertgrid)
							w.DrawRect(x - 1, r.top, 1, r.Height(), gridcolor);
						if(heading)
							break;
					}
				if(horzgrid)
					w.DrawRect(0, r.bottom, size.cx, 1, gridcolor);
				r.left = 0;
				r.right = x;
				if(i == cursor && !nocursor && multiselect && (GetSelectCount() != 1 || !IsSel(i)) && hasfocus0 && !isdrag)
					DrawFocus(w, r, st & Display::SELECT ? SColorPaper() : SColorText());
				r.bottom += horzgrid;
				r.left = x;
				r.right = size.cx;
				if(!nobg)
					w.DrawRect(r, SColorPaper);
				if(i == dropline && dropcol == DND_INSERTLINE)
					DrawHorzDrop(w, 0, r.top - (i > 0), size.cx);
				sy = r.bottom;
			}
			i++;
		}
	if(sample) return Size(r.left, sy);
	i32 ldy = r.bottom;
	r.left = 0;
	r.right = size.cx;
	r.top = r.bottom;
	if(IsAppendLine() && !IsCursor()) {
		r.bottom = r.top + linecy;
		w.DrawRect(r, HasFocus() ? SColorHighlight : SColorFace);
		r.top = r.bottom;
	}
	r.bottom = size.cy;
	if(!nobg)
		w.DrawRect(r, SColorPaper);
	if(GetCount() == dropline && dropcol == DND_INSERTLINE)
		DrawHorzDrop(w, 0, ldy - 1, size.cx);
	scroller.Set(Point(header.GetScroll(), sb));
	return Size();
}

void ArrayCtrl::Paint(Draw& w)
{
	DoPaint(w, false);
}

Image ArrayCtrl::GetDragSample()
{
	ImageDraw iw(StdSampleSize());
	Size sz = DoPaint(iw, true);
	return Crop(iw, 0, 0, sz.cx, sz.cy);
}

void ArrayCtrl::MinMaxLine()
{
	min_visible_line = Nvl(GetLineAt(sb), 0);
	max_visible_line = Nvl(GetLineAt((i32)sb + sb.GetPage()), GetCount());
}

void ArrayCtrl::Scroll() {
	SyncColumnsPos();
	MinMaxLine();
	SyncPageCtrls();
	PlaceEdits();
	scroller.Scroll(*this, GetSize(), Point(header.GetScroll(), sb));
	info.Cancel();
	WhenScroll();
}

void ArrayCtrl::SyncColumnsPos()
{
	i32 x = 0;
	column_pos.Clear();
	column_width.Clear();
	for(i32 i = 0; i < header.GetCount(); i++) {
		i32 w = header.GetTabWidth(i);
		i32 ii = header.GetTabIndex(i);
		column_pos.At(ii, 0) = x;
		column_width.At(ii, 0) = w;
		x += w;
	}
}

void ArrayCtrl::HeaderLayout() {
	SyncColumnsPos();
	MinMaxLine();
	Refresh();
	SyncInfo();
	SyncPageCtrls();
	PlaceEdits();
	WhenHeaderLayout();
}

void ArrayCtrl::HeaderScroll()
{
	Scroll();
	WhenHeaderLayout();
}

void ArrayCtrl::HeaderScrollVisibility()
{
	scrollbox.SetMargins(Rect(0, 0, 0, header.IsScroll() ? ScrollBarSize() : 0));
	scrollbox.SetColor(IsTransparent() ? Null : SColorFace());
}

void ArrayCtrl::RefreshRow(i32 i)
{
	if(i >= 0 && i < GetCount())
		Refresh(0, GetLineY(i) - sb, GetSize().cx, GetLineCy(i) + horzgrid);
	if(IsAppendLine() || i == GetCount())
		Refresh(0, GetLineY(GetCount()) - sb, GetSize().cx, linecy + horzgrid);
	SyncInfo();
}

ArrayCtrl::IdInfo& ArrayCtrl::IndexInfo(i32 i) {
	return idx[i];
}

ArrayCtrl::IdInfo& ArrayCtrl::IndexInfo(const Id& id) {
	return idx.Get(id);
}

ArrayCtrl::IdInfo& ArrayCtrl::AddIndex(const Id& id) {
	return idx.Add(id);
}

ArrayCtrl::IdInfo& ArrayCtrl::AddIndex() {
	return idx.Add(Id());
}

ArrayCtrl::IdInfo& ArrayCtrl::SetId(i32 i, const Id& id) {
	while(idx.GetCount() < i + 1)
		idx.Add(Id());
	idx.SetKey(i, id);
	return idx[i];
}

ArrayCtrl::Column& ArrayCtrl::AddColumn(tukk text, i32 w) {
	AddIndex();
	return AddColumnAt(idx.GetCount() - 1, text, w);
}

ArrayCtrl::Column& ArrayCtrl::AddColumn(const Id& id, tukk text, i32 w) {
	AddIndex(id);
	return AddColumnAt(idx.GetCount() - 1, text, w);
}

ArrayCtrl::Column& ArrayCtrl::AddColumnAt(i32 pos, tukk text, i32 w) {
	header.Add(text, w);
	Column& m = column.Add();
	m.arrayctrl = this;
	m.index = column.GetCount() - 1;
	m.Add(pos);
	if(allsorting)
		m.Sorting();
	return m;
}

ArrayCtrl::Column& ArrayCtrl::AddColumnAt(const Id& id, tukk text, i32 w) {
	header.Add(text, w);
	Column& m = column.Add();
	m.arrayctrl = this;
	m.index = column.GetCount() - 1;
	m.Add(id);
	if(allsorting)
		m.Sorting();
	return m;
}

ArrayCtrl::Column& ArrayCtrl::AddRowNumColumn(tukk text, i32 w) {
	header.Add(text, w);
	Column& m = column.Add();
	m.arrayctrl = this;
	m.index = column.GetCount() - 1;
	if(allsorting)
		m.Sorting();
	return m;
}

i32 ArrayCtrl::FindColumnWithPos(i32 pos) const
{
	for(i32 i = 0; i < column.GetCount(); i++) {
		const Vec<i32>& m = column[i].pos;
		for(i32 j = 0; j < m.GetCount(); j++)
			if(Pos(m[j]) == pos) return i;
	}
	return -1;
}

i32 ArrayCtrl::FindColumnWithId(const Id& id) const
{
	return FindColumnWithPos(GetPos(id));
}

Vec<i32> ArrayCtrl::FindColumnsWithPos(i32 pos) const
{
	Vec<i32> r;
	for(i32 i = 0; i < column.GetCount(); i++) {
		const Vec<i32>& m = column[i].pos;
		for(i32 j = 0; j < m.GetCount(); j++)
			if(Pos(m[j]) == pos)
				r.Add(i);
	}
	return r;
}

Vec<i32> ArrayCtrl::FindColumnsWithId(const Id& id) const
{
	return FindColumnsWithPos(GetPos(id));
}

ArrayCtrl::IdInfo& ArrayCtrl::AddCtrl(Ctrl& ctrl) {
	IdInfo& f = AddIndex();
	AddCtrlAt(idx.GetCount() - 1, ctrl);
	return f;
}

ArrayCtrl::IdInfo& ArrayCtrl::AddCtrl(const Id& id, Ctrl& ctrl) {
	IdInfo& f = AddIndex(id);
	AddCtrlAt(idx.GetCount() - 1, ctrl);
	return f;
}

void ArrayCtrl::AddCtrlAt(i32 ii, Ctrl& ctrl) {
	Control& c = control.Add();
	c.pos = ii;
	c.ctrl = &ctrl;
	ctrl.Disable();
	ctrl <<= Null;
}

void ArrayCtrl::AddCtrlAt(const Id& id, Ctrl& ctrl) {
	AddCtrlAt(-AsNdx(id), ctrl);
}

void ArrayCtrl::AddRowNumCtrl(Ctrl& ctrl) {
	AddCtrlAt(Null, ctrl);
}

Rect ArrayCtrl::GetCellRect(i32 i, i32 col) const
{
	LTIMING("GetCellRect");
	Rect r;
	r.top = GetLineY(i) - sb;
	r.bottom = r.top + GetLineCy(i);
	r.left = (col < column_pos.GetCount() ? column_pos[col] : 0) - header.GetScroll();
	r.right = r.left + (col < column_width.GetCount() ? column_width[col] : 0) - vertgrid +
	          (col == column.GetCount() - 1);
	return r;
}

Rect ArrayCtrl::GetCellRectM(i32 i, i32 col) const
{
	LTIMING("GetCellRectM");
	Rect r = GetCellRect(i, col);
	i32 cm = column[col].margin;
	if(cm < 0)
		cm = header.Tab(header.FindIndex(col)).GetMargin();
	r.left += cm;
	r.right -= cm;
	return r;
}

void ArrayCtrl::Remove0(i32 i) {
	array.Remove(i);
	for(i32 j = 0; j < column.GetCount(); j++)
		column[j].RemoveCache(i);
	if(i < ln.GetCount()) {
		i32 y = ln[i].y;
		ln.Remove(i);
		Reline(i, y);
	}
	if(i < cellinfo.GetCount())
		cellinfo.Remove(i);
	if(virtualcount > 0) virtualcount--;
	Refresh();
	PlaceEdits();
	SyncCtrls();
	SyncInfo();
	SetSb();
	selectiondirty = true;
	WhenArrayAction();
}

void ArrayCtrl::DisableCtrls() {
	for(i32 i = 0; i < control.GetCount(); i++) {
		Ctrl& c = *control[i].ctrl;
		c <<= Null;
		c.Disable();
	}
}

void ArrayCtrl::RejectRow() {
	bool rm_cursor = false;
	if(IsCursor()) {
		i32 i;
		for(i = 0; i < column.GetCount(); i++) {
			Column& m = column[i];
			if(m.edit) m.edit->Reject();
		}
		for(i = 0; i < control.GetCount(); i++)
			control[i].ctrl->Reject();
		if(IsInsert()) {
			Remove0(cursor);
			DisableCtrls();
			cursor = -1;
			rm_cursor = true;
		}
		else
			RefreshRow(cursor);
	}
	EndEdit();
	WhenArrayAction();
	if(rm_cursor) {
		WhenCursor();
		WhenSel();
	}
	SyncInfo();
}

void ArrayCtrl::Reject() {
	RejectRow();
}

void ArrayCtrl::CancelCursor() {
	RejectRow();
	DisableCtrls();
	cursor = anchor = -1;
	WhenCursor();
	WhenSel();
	SyncInfo();
}

bool ArrayCtrl::UpdateRow() {
	ASSERT(IsCursor());
	bool iv = false;
	i32 i;
	for(i = 0; i < column.GetCount(); i++) {
		Column& m = column[i];
		if(m.edit && m.edit->IsModified()) {
			Val v = m.edit->GetData();
			if(m.pos.GetCount() == 1) {
				Val& vv = array.At(cursor).line.At(Pos(m.pos[0]));
				if(vv != v) {
					iv = true;
					vv = v;
				}
			}
			else
			if(m.pos.GetCount() > 1) {
				ValArray va = v;
				for(i32 j = 0; j < m.pos.GetCount(); j++) {
					Val& vv = array.At(cursor).line.At(Pos(m.pos[j]));
					if(vv != va[j]) {
						iv = true;
						vv = va[j];
					}
				}
			}
		}
	}
	for(i = 0; i < control.GetCount(); i++) {
		Control& c = control[i];
		if(!IsNull(c.pos) && c.ctrl->IsModified()) {
			Val v = c.ctrl->GetData();
			Val& vv = array.At(cursor).line.At(Pos(c.pos));
			if(vv != v) {
				iv = true;
				vv = v;
			}
		}
	}
	if(iv) {
		RefreshRow(cursor);
		InvalidateCache(cursor);
		WhenArrayAction();
	}
	WhenUpdateRow();
	return true;
}

void ArrayCtrl::ClearModify() {
	i32 i;
	modify.Set(0, false, idx.GetCount());
	for(i = 0; i < column.GetCount(); i++)
		if(column[i].edit)
			column[i].edit->ClearModify();
	for(i = 0; i < control.GetCount(); i++)
		control[i].ctrl->ClearModify();
}

bool ArrayCtrl::AcceptRow(bool endedit) {
	ASSERT(IsCursor());
	if(acceptingrow) // prevent recursion
		return true;
	i32 i;
	for(i = 0; i < column.GetCount(); i++) {
		Column& m = column[i];
		if(m.edit && !m.edit->Accept())
			return false;
		if(IsCtrl(cursor, i) && !endedit) {
			Ctrl *c =  GetCellCtrl(cursor, i).ctrl;
			acceptingrow++;
			bool b = c->Accept();
			acceptingrow--;
			if(!b)
				return false;
		}
	}
	for(i = 0; i < control.GetCount(); i++)
		if(!control[i].ctrl->Accept())
			return false;
	acceptingrow++;
	bool ar = WhenAcceptRow() && UpdateRow();
	acceptingrow--;
	if(!ar) {
		SetCursorEditFocus();
		return false;
	}
	bool b = editmode;
	EndEdit();
	if(!endedit)
		SetCtrls();
	ClearModify();
	if(b)
		WhenAcceptEdit();
	return true;
}

bool ArrayCtrl::Accept() {
	return IsCursor() ? AcceptRow() :
	       accept_edits ? Ctrl::Accept() : true;
}

bool ArrayCtrl::KillCursor0() {
	if(IsCursor()) {
		if(!AcceptRow()) return false;
		i32 c = cursor;
		DisableCtrls();
		WhenKillCursor();
		cursor = -1;
		RefreshRow(c);
	}
	return true;
}

bool ArrayCtrl::KillCursor() {
	bool b = KillCursor0();
	if(b) {
		WhenCursor();
		WhenSel();
		SyncInfo();
	}
	return b;
}

void ArrayCtrl::SetCtrls() {
	i32 i;
	for(i = 0; i < column.GetCount(); i++)
		ColEditSetData(i);
	for(i = 0; i < control.GetCount(); i++)
		CtrlSetData(i);
}

void ArrayCtrl::ScrollInto(i32 line)
{
	if(line < 0)
		sb.Begin();
	else
	if(line >= GetCount())
		sb.End();
	else
		sb.ScrollInto(GetLineY(line), GetLineCy(line));
}

void ArrayCtrl::ScrollIntoCursor()
{
	if(IsCursor())
		ScrollInto(cursor);
}

void ArrayCtrl::SetCursorEditFocus()
{
	if(!IsEdit() && cursor >= 0 && hasctrls)
		for(i32 j = 0; j < column.GetCount(); j++)
			if(IsCtrl(cursor, j) && GetCellCtrl(cursor, j).ctrl->SetWantFocus())
				break;
}

bool ArrayCtrl::SetCursor0(i32 i, bool dosel) {
	if(nocursor || GetCount() == 0 || !IsLineEnabled(i))
		return false;
	i = minmax(i, 0, GetCount() - 1);
	bool sel = false;
	if(i != cursor) {
		RefreshRow(cursor);
		if(cursor >= 0) {
			if(!KillCursor0()) return false;
		}
		cursor = i;
		ScrollIntoCursor();
		RefreshRow(cursor);
		SetCtrls();
		for(i32 j = 0; j < column.GetCount(); j++)
			if(IsCtrl(cursor, j) && GetCellCtrl(cursor, j).ctrl->HasFocus())
				goto nosetfocus;
		SetCursorEditFocus();
	nosetfocus:
		ClearModify();
		Action();
		WhenEnterRow();
		WhenCursor();
		sel = true;
	}
	if(dosel && multiselect) {
		ClearSelection(false);
		anchor = cursor;
		SelectOne(cursor, true, false);
		sel = true;
	}
	if(sel) {
		selectiondirty = true;
		WhenSelection();
		WhenSel();
	}
	SyncInfo();
	return true;
}

bool ArrayCtrl::SetCursor(i32 i)
{
	return SetCursor0(i);
}

void ArrayCtrl::ShowAppendLine() {
	sb.ScrollInto(GetTotalCy(), GetLineCy());
}

void ArrayCtrl::GoBegin() {
	if(nocursor)
		sb.Begin();
	else {
		i32 q = FindEnabled(0, 1);
		if(q >= 0)
			SetCursor(q);
	}
}

void ArrayCtrl::GoEnd() {
	if(nocursor)
		sb.End();
	else {
		i32 q = FindEnabled(GetCount() - 1, -1);
		if(q >= 0)
			SetCursor(q);
	}
}

i32  ArrayCtrl::GetCursorSc() const {
	return IsCursor() ? GetLineY(cursor) - sb : 0;
}

void ArrayCtrl::ScCursor(i32 a) {
	if(IsCursor())
		sb = GetLineY(cursor) - max(a, 0);
}

void ArrayCtrl::SyncSelection() const
{
	if(!selectiondirty)
		return;
	selectcount = 0;
	for(i32 i = 0; i < array.GetCount(); i++)
		if(array[i].select)
			selectcount++;
	selectiondirty = false;
}

i32 ArrayCtrl::GetSelectCount() const
{
	SyncSelection();
	return selectcount;
}

void ArrayCtrl::SelectOne(i32 i, bool sel, bool raise)
{
	array.At(i).select = sel;
	selectiondirty = true;
	RefreshRow(i);
	if(raise) {
		WhenSelection();
		WhenSel();
	}
	SyncInfo();
}

void ArrayCtrl::Select(i32 i, i32 count, bool sel)
{
	while(count--) {
		array.At(i).select = sel;
		RefreshRow(i++);
	}
	selectiondirty = true;
	WhenSelection();
	WhenSel();
	SyncInfo();
}

void ArrayCtrl::EnableLine(i32 i, bool en)
{
	array.At(i).enabled = en;
}

bool ArrayCtrl::IsLineEnabled(i32 i) const
{
	bool b = i < 0 ? false : i < array.GetCount() ? array[i].enabled : true;
	WhenLineEnabled(i, b);
	return b;
}

void ArrayCtrl::ShowLine(i32 i, bool e)
{
	i32 q = ln.GetCount();
	array.At(i).visible = e;
	ln.At(i);
	if(q > 0 && i > 0)
		Reline(i - 1, ln[i - 1].y);
	else
		Reline(0, 0);
}

bool ArrayCtrl::IsLineVisible(i32 i) const
{
	bool b = IsLineVisible0(i);
	WhenLineVisible(i, b);
	return b;
}

void ArrayCtrl::ClearSelection(bool raise)
{
	if(IsSelection()) {
		for(i32 i = 0; i < array.GetCount(); i++)
			if(array[i].select) {
				RefreshRow(i);
				array[i].select = false;
			}
		selectiondirty = false;
		selectcount = 0;
		if(raise) {
			WhenSelection();
			WhenSel();
		}
		SyncInfo();
	}
}

bool ArrayCtrl::IsSel(i32 i) const
{
	return multiselect ? IsSelected(i) : GetCursor() == i;
}

Vec<i32> ArrayCtrl::GetSelKeys() const
{
	Vec<i32> id;
	for(i32 i = 0; i < GetCount(); i++)
		if(IsSel(i))
			id.Add(Get(i, 0));
	return id;
}

i32  ArrayCtrl::GetScroll() const
{
	return sb;
}

void ArrayCtrl::ScrollTo(i32 sc)
{
	sb = sc;
}

void ArrayCtrl::CenterCursor() {
	if(IsCursor())
		ScCursor((GetSize().cy - GetLineCy(cursor)) / 2);
}

i32 ArrayCtrl::FindEnabled(i32 i, i32 dir)
{
	ASSERT(dir == -1 || dir == 1);
	while(i >= 0 && i < GetCount()) {
		if(IsLineEnabled(i))
			return i;
		i += dir;
	}
	return -1;
}

void ArrayCtrl::Page(i32 q) {
	q = q * max(GetSize().cy - (linecy + horzgrid), linecy + horzgrid);
	i32 a = GetCursorSc();
	if(IsCursor()) {
		i32 i = FindEnabled(cursor + q / (linecy + horzgrid), sgn(q));
		if(i >= 0) {
			SetCursor(i);
			ScCursor(a);
		}
	}
	else
	if(q > 0)
		sb.NextPage();
	else
		sb.PrevPage();
}

void ArrayCtrl::DoPoint(Point p, bool dosel) {
	p.y += sb;
	if(p.y >= GetTotalCy() && IsAppendLine()) {
		if(IsMultiSelect())
			ClearSelection();
		KillCursor();
	}
	clickpos.y = GetLineAt(p.y);
	if(!IsNull(clickpos.y))
		SetCursor0(clickpos.y, dosel);
	else
	if(IsCursor())
		AcceptRow(true); // true not to reenable ctrls
	if(!HasFocusDeep())
		SetWantFocus();
}

i32  ArrayCtrl::GetClickColumn(i32 ii, Point p)
{
	for(i32 i = 0; i < column.GetCount(); i++)
		if(GetCellRect(ii, i).Contains(p))
			return i;
	return Null;
}

void ArrayCtrl::ClickColumn(Point p)
{
	clickpos.x = Null;
	if(clickpos.y >= 0)
		clickpos.x = GetClickColumn(clickpos.y, p);
}

void ArrayCtrl::ClickSel(dword flags)
{
	if(cursor >= 0 && multiselect) {
		if(flags & K_CTRL) {
			SelectOne(cursor, !IsSelected(cursor));
			anchor = cursor;
		}
		else {
			ClearSelection(false);
			if((flags & K_SHIFT) && anchor >= 0)
				Select(min(anchor, cursor), abs(anchor - cursor) + 1, true);
			else {
				anchor = cursor;
				SelectOne(cursor, true);
			}
		}
		Action();
	}
}

void ArrayCtrl::LeftDown(Point p, dword flags)
{
	if(IsReadOnly()) return;
	i32 q = header.GetSplit(p.x);
	if(q >= 0) {
		header.StartSplitDrag(q);
		return;
	}
	q = clickpos.y = GetLineAt(p.y + sb);
	ClickColumn(p);
	selclick = false;
	if(q >= 0 && q < GetCount() && IsSel(q) && (flags & (K_CTRL|K_SHIFT)) == 0) {
		selclick = true;
		return;
	}
	DoClick(p, flags);
}

void ArrayCtrl::DoClick(Point p, dword flags) {
	i32 c = cursor;
	bool b = HasFocus();
	DoPoint(p, !(flags & (K_CTRL|K_SHIFT)));
	ClickColumn(p);
	if(!IsNull(clickpos.x) && c == cursor && cursor >= 0 && b && column[clickpos.x].clickedit
	   && (flags & (K_CTRL|K_SHIFT)) == 0)
		StartEdit(clickpos.x);
	else
		ClickSel(flags);
	WhenLeftClick();
}

void ArrayCtrl::LeftUp(Point p, dword flags)
{
	if(selclick)
		DoClick(p, flags);
	selclick = false;
}

void ArrayCtrl::LeftDouble(Point p, dword flags)
{
	if(IsReadOnly()) return;
	DoPoint(p, !(flags & (K_CTRL|K_SHIFT)));
	ClickColumn(p);
	ClickSel(flags);
	if((IsInserting() || IsAppending()) && IsAppendLine() && !IsCursor())
		DoAppend();
	if(!multiselect || (flags & (K_CTRL|K_SHIFT)) == 0)
		WhenLeftDouble();
}

void ArrayCtrl::LeftDrag(Point p, dword)
{
	i32 ii = GetLineAt(p.y + sb);
	if(!IsNull(ii) && IsSel(ii))
		WhenDrag();
}

void ArrayCtrl::SyncInfo()
{
	if((HasMouse() || info.HasMouse()) && !IsEdit() && popupex) {
		Point p = GetMouseViewPos();
		Point c;
		c.y = GetLineAt(p.y + sb);
		if(c.y >= 0) {
			for(c.x = 0; c.x < column.GetCount(); c.x++) {
				if(!IsCtrl(c.y, c.x)) {
					Rect r = GetCellRect(c.y, c.x);
					i32 cm = column[c.x].margin;
					if(cm < 0)
						cm = header.Tab(header.FindIndex(c.x)).GetMargin();
					dword st;
					Color fg, bg;
					Val q;
					const Display& d = GetCellInfo(c.y, c.x, HasFocusDeep(), q, fg, bg, st);
					i32 cw = r.Width();
					if(spanwidecells)
						SpanWideCell(d, q, cm, cw, r, c.y, c.x);
					if(r.Contains(p)) {
						info.Set(this, r, q, &d, fg, bg, st, cm);
						return;
					}
				}
			}
		}
	}
	info.Cancel();
}

void ArrayCtrl::MouseMove(Point p, dword)
{
	i32 ii = Null;
	if(WhenMouseMove) {
		ii = GetLineAt(p.y + sb);
		if(IsNull(ii))
			WhenMouseMove(Null);
		else
			WhenMouseMove(Point(GetClickColumn(ii, p), ii));
	}
	if(mousemove && !IsReadOnly()) {
		if(IsNull(ii))
			ii = GetLineAt(p.y + sb);
		if(!IsNull(ii)) SetCursor(ii);
	}
	SyncInfo();
}

void ArrayCtrl::MouseLeave()
{
	WhenMouseMove(Null);
}

Image ArrayCtrl::CursorImage(Point p, dword)
{
	if(!IsNull(cursor_override))
		return cursor_override;
	return header.GetSplit(p.x) < 0 || header.GetMode() == HeaderCtrl::FIXED ? Image::Arrow()
	                                                                         : CtrlImg::HorzPos();
}

void ArrayCtrl::RightDown(Point p, dword flags) {
	if((flags & (K_CTRL|K_SHIFT)) == 0) {
		DoPoint(p, false);
		ClickColumn(p);
		if(cursor >= 0 && multiselect) {
			if(!IsSelected(cursor)) {
				ClearSelection();
				SelectOne(anchor = cursor);
			}
			Action();
		}
	}
	MenuBar::Execute(WhenBar);
}

bool ArrayCtrl::TestKey(i32 i, wchar key) {
	for(i32 j = 0; j < column.GetCount(); j++) {
		wchar (*af)(wchar) = column[j].accel;
		if(af) {
			char c = (*af)(key);
			if(c && (*af)(StdFormat(GetColumn(i, j))[0]) == c) {
				SetCursor(i);
				return true;
			}
		}
	}
	for(i32 ii = 0; ii < idx.GetCount(); ii++) {
		wchar (*af)(wchar) = idx[ii].accel;
		if(af) {
			wchar c = (*af)(key);
			if(c && (*af)(StdFormat(Get(i, ii))[0]) == c) {
				SetCursor(i);
				return true;
			}
		}
	}
	return false;
}

i32 ArrayCtrl::GetEditColumn() const {
	if(!IsEdit()) return -1;
	for(i32 i = 0; i < column.GetCount(); i++) {
		const Column& m = column[i];
		if(m.edit && m.edit->HasFocusDeep())
			return i;
	}
	return -1;
}

void ArrayCtrl::KeyMultiSelect(i32 aanchor, dword key)
{
	if(!multiselect)
		return;
	ClearSelection();
	if(key & K_SHIFT) {
		anchor = aanchor;
		if(anchor >= 0 && cursor >= 0)
			Select(min(anchor, cursor), abs(anchor - cursor) + 1);
	}
	else
	if(cursor >= 0) {
		anchor = cursor;
		SelectOne(anchor);
	}
}

bool ArrayCtrl::Key(dword key, i32) {
	if(IsReadOnly()) return false;
	i32 aanchor = anchor;
	if(!editmode) {
		if(key > 32 && key < 256) {
			i32 cr = cursor >= 0 ? cursor + 1 : 0;
			for(i32 j = 0; j < column.GetCount(); j++)
				if(column[j].accel) {
					i32 i;
					for(i = cr; i < array.GetCount(); i++)
						if(TestKey(i, key)) return true;
					for(i = 0; i < cr; i++)
						if(TestKey(i, key)) return true;
					return true;
				}
		}
		switch(key) {
		case K_PAGEUP:
		case K_SHIFT_PAGEUP:
			Page(-1);
			KeyMultiSelect(aanchor, key);
			return true;
		case K_PAGEDOWN:
		case K_SHIFT_PAGEDOWN:
			if((IsInserting() || IsAppending()) && IsAppendLine() && cursor == GetCount() - 1)
				KillCursor();
			else {
				Page(1);
				KeyMultiSelect(aanchor, key);
			}
			return true;
		case K_HOME:
		case K_CTRL_HOME:
		case K_CTRL_PAGEUP:
		case K_SHIFT_HOME:
		case K_SHIFT|K_CTRL_HOME:
		case K_SHIFT|K_CTRL_PAGEUP:
			GoBegin();
			KeyMultiSelect(aanchor, key);
			return true;
		case K_END:
		case K_CTRL_END:
		case K_CTRL_PAGEDOWN:
		case K_SHIFT|K_END:
		case K_SHIFT|K_CTRL_END:
		case K_SHIFT|K_CTRL_PAGEDOWN:
			GoEnd();
			KeyMultiSelect(aanchor, key);
			return true;
		case K_CTRL_A:
			if(multiselect) {
				GoEnd();
				anchor = 0;
				KeyMultiSelect(0, K_SHIFT);
			}
			return true;
		}
	}
	switch(key) {
	case K_UP:
	case K_SHIFT_UP:
		if(IsCursor()) {
			i32 d = GetEditColumn();
			i32 i = FindEnabled(cursor - 1,  -1);
			if(i >= 0 && SetCursor0(i)) {
				if(d >= 0)
					StartEdit(d);
				else
					KeyMultiSelect(aanchor, key);
			}
		}
		else
		if((IsInserting() || IsAppending()) && IsAppendLine())
			SetCursor(GetCount() - 1);
		else
			sb.PrevLine();
		return true;
	case K_DOWN:
	case K_SHIFT_DOWN:
		if((IsInserting() || IsAppending()) && IsAppendLine() && cursor == GetCount() - 1 && !editmode) {
			if(IsMultiSelect())
				ClearSelection();
			KillCursor();
		}
		else
		if(IsCursor()) {
			i32 d = GetEditColumn();
			i32 i = FindEnabled(cursor + 1, 1);
			if(i >= 0 && SetCursor0(i) && d >= 0)
				StartEdit(d);
			else
				KeyMultiSelect(aanchor, key);
		}
		else
			sb.NextLine();
		return true;
	case K_ENTER:
		if(!IsCursor() && (IsInserting() || IsAppending()) && IsAppendLine()) {
			DoAppend();
			return true;
		}
		if(editmode) {
			bool ins = IsInsert() && autoappend;
			if(AcceptEnter() && ins)
				DoAppend();
			return true;
		}
		if(WhenEnterKey && IsCursor()) {
			WhenEnterKey();
			return true;
		}
		break;
	case K_ESCAPE:
		if(IsEdit()) {
			i32 c = cursor;
			CancelCursor();
			SetCursor(c);
			return true;
		}
		break;
	}
	return MenuBar::Scan(WhenBar, key);
}

bool ArrayCtrl::AcceptEnter() {
	if(editmode) {
		bool ins = IsInsert();
		if(!Accept())
			return false;
		if(GetCursor() == array.GetCount() - 1 &&
		(IsInserting() || IsAppending()) && IsAppendLine() && ins && !noinsertappend) {
			if(IsMultiSelect())
				ClearSelection();
			KillCursor();
			ShowAppendLine();
		}
	}
	return true;
}

void  ArrayCtrl::Add() {
	if(GetIndexCount()) {
		Vec<Val> x;
		for(i32 ii = 0; ii < idx.GetCount(); ii++)
			x.Add(idx[ii].GetInsertVal());
		Set(array.GetCount(), x);
	}
	else {
		i32 i = array.GetCount();
		array.At(i);
		SetSb();
		Refresh();
		InvalidateCache(cursor);
		InvalidateCache(i);
		RefreshRow(i);
	}
	WhenArrayAction();
}

void ArrayCtrl::Set(i32 ii, const VecMap<Txt, Val>& m)
{
	for(i32 i = 0; i < m.GetCount(); i++) {
		i32 j = GetPos(m.GetKey(i));
		if(j >= 0)
			Set(ii, j, m[i]);
	}
}

void ArrayCtrl::SetMap(i32 ii, const ValMap& m)
{
	for(i32 i = 0; i < m.GetCount(); i++) {
		i32 j = GetPos((Txt)m.GetKey(i));
		if(j >= 0)
			Set(ii, j, m.GetVal(i));
	}
}

void ArrayCtrl::Add(const VecMap<Txt, Val>& m)
{
	Set(array.GetCount(), m);
}

void ArrayCtrl::AddMap(const ValMap& m)
{
	SetMap(array.GetCount(), m);
}

ValMap ArrayCtrl::GetMap(i32 i) const
{
	ValMap m;
	for(i32 j = 0; j < GetIndexCount(); j++) {
		Txt id = ~GetId(j);
		if(id.GetCount())
			m(id, Get(i, j));
	}
	return m;
}

ValArray ArrayCtrl::GetArray(i32 i) const
{
	return ValArray(clone(GetLine(i)));
}

void ArrayCtrl::SetArray(i32 i, const ValArray& va)
{
	Set(i, va.Get());
}

void ArrayCtrl::AddArray(const ValArray& va)
{
	SetArray(array.GetCount(), va);
}

struct ArrayCtrlSeparatorDisplay : Display {
	virtual void Paint(Draw& w, const Rect& r, const Val& q, Color ink, Color paper, dword style) const {
		i32 y = r.top + r.GetHeight() / 2;
		w.DrawRect(r, paper);
		w.DrawRect(r.left, y, r.GetWidth(), 1, SColorShadow());
		w.DrawRect(r.left, y + 1, r.GetWidth(), 1, SColorLight());
	}
	virtual Size GetStdSize(const Val& q) const {
		return Size(0, 2);
	}
};

void ArrayCtrl::AddSeparator()
{
	i32 ii = GetCount();
	Add();
	SetLineCy(ii, Draw::GetStdFontCy() / 2);
	for(i32 i = 0; i < GetColumnCount(); i++)
		SetDisplay(ii, i, Single<ArrayCtrlSeparatorDisplay>());
	DisableLine(ii);
}

void ArrayCtrl::AddHeading(const Val& v)
{
	i32 ii = GetCount();
	Add(v);
	array.At(ii).heading = true;
	DisableLine(ii);
}

#define E__Addv(I)    Set0(q, I - 1, p##I)
#define E__AddF(I) \
void ArrayCtrl::Add(__List##I(E__Val)) { \
	i32 q = array.GetCount(); \
	__List##I(E__Addv); \
	AfterSet(q); \
}
__Expand(E__AddF)

void  ArrayCtrl::Insert(i32 i, i32 count) {
	if(i < array.GetCount()) {
		array.InsertN(i, count);
		for(i32 j = 0; j < column.GetCount(); j++)
			column[j].InsertCache(i, count);
	}
	else
		array.At(i + count - 1);
	if(i <= cursor) cursor++;
	if(i < cellinfo.GetCount())
		cellinfo.InsertN(i, count);
	if(i < ln.GetCount()) {
		i32 y = ln[i].y;
		ln.InsertN(i, count);
		Reline(i, y);
	}
	if(virtualcount >= 0) virtualcount += count;
	while(count--) {
		for(i32 ii = 0; ii < idx.GetCount(); ii++) {
			Val v = idx[ii].GetInsertVal();
			if(!IsNull(v))
				Set(i, ii, v);
		}
		i++;
	}
	Refresh();
	SetSb();
	PlaceEdits();
	SyncCtrls();
	SyncInfo();
	WhenArrayAction();
}

void  ArrayCtrl::Insert(i32 i)
{
	Insert(i, 1);
}

void ArrayCtrl::Insert(i32 i, const Vec<Val>& v) {
	Insert(i);
	Set(i, v);
}

void ArrayCtrl::Insert(i32 i, const Vec< Vec<Val> >& v) {
	Insert(i, v.GetCount());
	for(i32 ii = 0; ii < v.GetCount(); ii++)
		Set(i++, v[ii]);
}

void  ArrayCtrl::Remove(i32 i) {
	i32 c = cursor;
	if(i == cursor) CancelCursor();
	Remove0(i);
	if(c >= 0)
		SetCursor0(c - (i < c), false);
	anchor = -1;
	PlaceEdits();
	SyncCtrls();
	SetSb();
	Refresh();
	SyncInfo();
}

void ArrayCtrl::PlaceEdits() {
	for(i32 i = 0; i < column.GetCount(); i++) {
		Column& m = column[i];
		if(m.edit) {
			m.edit->Show(editmode && header.IsTabVisible(i));
			if(m.edit->IsVisible())
				m.edit->SetRect(GetCellRectM(cursor, i));
		}
	}
}

bool ArrayCtrl::IsEditing() const {
	for(i32 i = 0; i < column.GetCount(); i++)
		if(column[i].edit) return true;
	return false;
}

bool ArrayCtrl::StartEdit(i32 d) {
	if(IsReadOnly()) return false;
	if(!IsEditing()) return false;
	editmode = true;
	PlaceEdits();
	for(i32 i = 0; i < column.GetCount(); i++) {
		Column& m = column[(i + d) % column.GetCount()];
		if(m.edit && (m.edit->SetWantFocus() || IterateFocusForward(m.edit, m.edit)))
			break;
	}
	NoWantFocus();
	RefreshRow(cursor);
	WhenStartEdit();
	SyncInfo();
	return true;
}

void ArrayCtrl::EndEdit() {
	if(!IsEditing())
		return;
	bool f = HasFocusDeep();
	editmode = false;
	insertmode = false;
	PlaceEdits();
	WantFocus();
	if(f) SetWantFocus();
	SyncInfo();
}

void ArrayCtrl::LostFocus() {
	if(GetSelectCount() > 1)
		Refresh();
	else
	if(IsCursor())
		RefreshRow(cursor);
	SyncInfo();
}

void ArrayCtrl::GotFocus() {
	if(GetSelectCount() > 1)
		Refresh();
	else
	if(IsCursor())
		RefreshRow(cursor);
	else
	if(focussetcursor)
		GoBegin();
	SetCursorEditFocus();
	SyncInfo();
}

void ArrayCtrl::DoEdit() {
	if(IsReadOnly()) return;
	if(!editmode && IsCursor())
		StartEdit();
}

void ArrayCtrl::DoInsert(i32 c) {
	if(IsReadOnly()) return;
	Insert(c);
	SetCursor(c);
	insertmode = true;
	DoEdit();
	if(!IsEdit())
		insertmode = false;
	WhenArrayAction();
}

void ArrayCtrl::DoInsertBefore() {
	DoInsert(IsCursor() ? GetCursor() : array.GetCount());
}

void ArrayCtrl::DoInsertAfter() {
	DoInsert(IsCursor() ? GetCursor() + 1 : array.GetCount());
}

void ArrayCtrl::DoAppend() {
	DoInsert(array.GetCount());
}

void ArrayCtrl::DoSelectAll()
{
	Select(0, GetCount());
}

Txt ArrayCtrl::RowFormat(tukk s)
{
	return Sprintf(s, ~row_name);
}

bool ArrayCtrl::DoRemove()
{
	if(IsReadOnly()) return false;
	if(!IsCursor() || askremove && !PromptOKCancel(RowFormat(t_("Вы действительно хотите удалить выделенный %s ?"))))
		return false;
	if(multiselect) {
		Bits sel;
		for(i32 i = 0; i < GetCount(); i++)
			sel.Set(i, IsSelected(i));
		for(i32 i = GetCount() - 1; i >= 0; i--)
			if(sel[i])
				Remove(i);
	}
	else
		Remove(cursor);
	WhenArrayAction();
	return true;
}

void ArrayCtrl::DoRemovem()
{
	DoRemove();
}

void ArrayCtrl::DoDuplicate() {
	if(IsReadOnly()) return;
	if(!IsCursor()) return;
	i32 c = cursor;
	if(!KillCursor()) return;
	Vec<Val> va = ReadRow(c);
	c = IsAppending() ? array.GetCount() : c + 1;
	Insert(c, va);
	SetCursor(c);
	DoEdit();
	WhenArrayAction();
}

void ArrayCtrl::StdBar(Bar& menu)
{
	bool e = IsEditable();
	bool c = !IsEdit() && e;
	bool d = c && IsCursor();
	if(inserting)
		menu.Add(e, RowFormat(t_("Вставить %s")), OTVET(DoInsertBefore))
			.Help(RowFormat(t_("Вставить новый %s в таблицу.")))
			.Key(K_INSERT);
	if(bains == 1) {
		menu.Add(e, RowFormat(t_("Вставить %s перед")), OTVET(DoInsertBefore))
		    .Help(RowFormat(t_("Вставить новый %s в таблицу перед текущим")))
		    .Key(K_INSERT);
		menu.Add(e, RowFormat(t_("Вставить %s после")), OTVET(DoInsertAfter))
		    .Help(RowFormat(t_("Вставить новый %s в таблицу после текущего")))
		    .Key(K_SHIFT_INSERT);
	}
	if(bains == 2) {
		menu.Add(e, RowFormat(t_("Вставить %s после")), OTVET(DoInsertAfter))
		    .Help(RowFormat(t_("Вставить новый %s в таблицу после текущего")))
		    .Key(K_INSERT);
		menu.Add(e, RowFormat(t_("Вставить %s перед")), OTVET(DoInsertBefore))
		    .Help(RowFormat(t_("Вставить новый %s в таблицу перед текущим")))
		    .Key(K_SHIFT_INSERT);
	}
	if(IsAppending())
		menu.Add(c, RowFormat(t_("Приставить %s")), OTVET(DoAppend))
			.Help(RowFormat(t_("Приставить новый %s в конце таблицы.")))
			.Key(inserting ? (i32)K_SHIFT_INSERT : (i32)K_INSERT);
	if(duplicating)
		menu.Add(d, RowFormat(t_("Дублировать %s")), OTVET(DoDuplicate))
			.Help(RowFormat(t_("Лублировать текущую таблицу %s.")))
			.Key(K_CTRL_INSERT);
	if(IsEditing())
		menu.Add(d, RowFormat(t_("Редактировать %s")), OTVET(DoEdit))
			.Help(RowFormat(t_("Редактировать активный %s.")))
			.Key(K_CTRL_ENTER);
	if(removing)
		menu.Add(d, RowFormat(t_("Удалить %s\tУдалить")), OTVET(DoRemovem))
			.Help(RowFormat(t_("Удалить активный %s.")))
			.Key(K_DELETE);
	if(moving) {
		menu.Add(GetCursor() > 0, RowFormat(t_("Поднять %s")), OTVET(SwapUp))
			.Help(RowFormat(t_("Поменять %s местами с предыдущим, подняв его.")))
			.Key(K_CTRL_UP);
		menu.Add(GetCursor() >= 0 && GetCursor() < GetCount() - 1,
		         RowFormat(t_("Понизить %s")), OTVET(SwapDown))
			.Help(RowFormat(t_("Поменять %s местами со следующим, понизив его.")))
			.Key(K_CTRL_DOWN);
	}
	if(multiselect) {
		menu.Add(GetCount() > 0, RowFormat(t_("Выбрать все")), CtrlImg::select_all(), OTVET(DoSelectAll))
			.Help(t_("Выделить все ряды таблицы"))
			.Key(K_CTRL_A);
	}
}

i32 ArrayCtrl::Find(const Val& v, i32 ii, i32 i) const {
	ASSERT(ii >= 0);
	i32 n = GetCount();
	while(i < n) {
		if(IsLineEnabled(i) && Get(i, ii) == v) return i;
		i++;
	}
	return -1;
}

i32  ArrayCtrl::Find(const Val& v, const Id& id, i32 i) const {
	return Find(v, GetPos(id), i);
}

bool ArrayCtrl::FindSetCursor(const Val& val, i32 ii, i32 i) {
	i = Find(val, ii, i);
	if(i < 0) return false;
	if(!SetCursor(i)) return false;
	return true;
}

bool ArrayCtrl::FindSetCursor(const Val& val, const Id& id, i32 i) {
	return FindSetCursor(val, idx.Find(id), i);
}

void ArrayCtrl::ClearCache() {
	for(i32 i = 0; i < column.GetCount(); i++)
		column[i].ClearCache();
}

struct ArrayCtrl::SortPredicate {
	const ArrayCtrl::Order *order;
	bool operator()(const Line& a, const Line& b) const {
		return order->operator()(a.line, b.line);
	}
};

void ArrayCtrl::SortA()
{
	if(hasctrls) {
		for(i32 i = 0; i < array.GetCount(); i++)
			for(i32 j = 0; j < column.GetCount(); j++)
				if(IsCtrl(i, j)) {
					const Column& m = column[j];
					ASSERT(m.pos.GetCount() == 1);
					array[i].line.At(m.pos[0]) = GetCellCtrl(i, j).ctrl->GetData();
				}
	}
}

void ArrayCtrl::SortB(const Vec<i32>& o)
{
	Vec<Line> narray;
	narray.SetCount(array.GetCount());
	Vec<Ln> nln;
	Vec< Vec<CellInfo> > ncellinfo;
	for(i32 i = 0; i < o.GetCount(); i++) {
		i32 oi = o[i];
		narray[i] = pick(array[oi]);
		if(oi < cellinfo.GetCount())
			ncellinfo.At(i) = pick(cellinfo[oi]);
		if(oi < ln.GetCount())
			nln.At(i) = ln[oi];
	}
	array = pick(narray);
	cellinfo = pick(ncellinfo);
	ln = pick(nln);
	Reline(0, 0);
	if(hasctrls) {
		for(i32 i = 0; i < array.GetCount(); i++)
			for(i32 j = 0; j < column.GetCount(); j++)
				if(IsCtrl(i, j)) {
					const Column& m = column[j];
					ASSERT(m.pos.GetCount() == 1);
					array[i].line.At(m.pos[0]) = Null;
				}
		SyncCtrls();
		ChildGotFocus();
	}
}

void ArrayCtrl::ReArrange(const Vec<i32>& order)
{
	KillCursor();
	ClearSelection();
	ClearCache();
	SortA();
	SortB(order);
	Refresh();
	SyncInfo();
}

void ArrayCtrl::Sort(i32 from, i32 count, Gate<i32, i32> order)
{
	KillCursor();
	ClearSelection();
	ClearCache();
	Vec<i32> h;
	for(i32 i = 0; i < array.GetCount(); i++)
		h.Add(i);
	SortA();
	CoStableSort(SubRange(h, from, count).Write(), order);
	SortB(h);
	Refresh();
	SyncInfo();
}

void ArrayCtrl::Sort(Gate<i32, i32> order)
{
	if(sorting_from < array.GetCount())
		Sort(sorting_from, array.GetCount() - sorting_from, order);
}

bool ArrayCtrl::OrderPred(i32 i1, i32 i2, const ArrayCtrl::Order *o)
{
	return (*o)(array[i1].line, array[i2].line);
}

void ArrayCtrl::Sort(i32 from, i32 count, const ArrayCtrl::Order& order)
{
	Sort(from, count, OTVET1(OrderPred, &order));
}

void ArrayCtrl::Sort(const ArrayCtrl::Order& order)
{
	if(sorting_from < array.GetCount())
		Sort(sorting_from, array.GetCount() - sorting_from, order);
}

struct sAC_ColumnSort : public ValOrder {
	bool                                           descending;
	const ValOrder                              *order;
	Function<i32 (const Val& a, const Val& b)> cmp;

	virtual bool operator()(const Val& a, const Val& b) const {
		return descending ? order ? (*order)(b, a) : cmp(b, a) < 0
		                  : order ? (*order)(a, b) : cmp(a, b) < 0;
	}
};

bool ArrayCtrl::ColumnSortPred(i32 i1, i32 i2, i32 column, const ValOrder *o)
{
	return (*o)(GetConvertedColumn(i1, column), GetConvertedColumn(i2, column));
}

void ArrayCtrl::ColumnSort(i32 column, Gate<i32, i32> order)
{
	Val key = GetKey();
	CHECK(KillCursor());
	if(columnsortsecondary)
		Sort(*columnsortsecondary);
	Sort(order);
	if(columnsortfindkey)
		FindSetCursor(key);
}

void ArrayCtrl::ColumnSort(i32 column, const ValOrder& order)
{
	ColumnSort(column, OTVET2(ColumnSortPred, column, &order));
}

void ArrayCtrl::ColumnSort(i32 column, i32 (*compare)(const Val& a, const Val& b))
{
	sAC_ColumnSort cs;
	cs.descending = false;
	cs.order = NULL;
	cs.cmp = compare;
	if(!cs.cmp)
		cs.cmp = StdValCompare;
	ColumnSort(column, cs);
}

void ArrayCtrl::SetSortColumn(i32 ii, bool desc)
{
	sortcolumn = ii;
	sortcolumndescending = desc;
	DoColumnSort();
}

void ArrayCtrl::ToggleSortColumn(i32 ii)
{
	if(sortcolumn == ii)
		sortcolumndescending = !sortcolumndescending;
	else {
		sortcolumn = ii;
		sortcolumndescending = false;
	}
	DoColumnSort();
}

void ArrayCtrl::DoColumnSort()
{
	if(sortcolumn < 0) {
		sortcolumndescending = false;
		for(i32 i = 0; i < column.GetCount(); i++)
			if(column[i].order || column[i].cmp) {
				sortcolumn = i;
				break;
			}
	}
	if(sortcolumn < 0)
		return;
	for(i32 i = 0; i < header.GetCount(); i++)
		header.Tab(i).SetRightImage(header.GetTabIndex(i) == sortcolumn ?
		                                 sortcolumndescending ? CtrlImg::SortUp()
		                                                      : CtrlImg::SortDown()
		                            : Image());
	if(sortcolumn >= 0 && sortcolumn < column.GetCount()) {
		sAC_ColumnSort cs;
		const Column& c = column[sortcolumn];
		if(c.line_order)
			ColumnSort(sortcolumn, c.line_order);
		else {
			cs.descending = sortcolumndescending;
			cs.order = c.order;
			cs.cmp = c.cmp;
			if(!cs.order && !cs.cmp)
				cs.cmp = StdValCompare;
			ColumnSort(sortcolumn, cs);
		}
	}
	WhenColumnSorted();
}

ArrayCtrl& ArrayCtrl::AllSorting()
{
	allsorting = true;
	for(i32 i = 0; i < column.GetCount(); i++)
		column[i].Sorting();
	return *this;
}

struct ArrayCtrl::RowOrder : public ArrayCtrl::Order {
	i32 (*compare)(const Vec<Val>& v1, const Vec<Val>& v2);

	virtual bool operator()(const Vec<Val>& row1, const Vec<Val>& row2) const {
		return (*compare)(row1, row2) < 0;
	}
};

void ArrayCtrl::Sort(i32 from, i32 count, i32 (*compare)(const Vec<Val>& v1, const Vec<Val>& v2))
{
	RowOrder io;
	io.compare = compare;
	Sort(from, count, io);
}

void ArrayCtrl::Sort(i32 (*compare)(const Vec<Val>& v1, const Vec<Val>& v2)) {
	Sort(0, GetCount(), compare);
}

struct ArrayCtrl::ItemOrder : public ArrayCtrl::Order {
	i32 ii;
	i32 (*compare)(const Val& v1, const Val& v2);

	virtual bool operator()(const Vec<Val>& row1, const Vec<Val>& row2) const {
		return (*compare)(row1[ii], row2[ii]) < 0;
	}
};

void ArrayCtrl::Sort(i32 ii, i32 (*compare)(const Val& v1, const Val& v2)) {
	ItemOrder io;
	io.ii = ii;
	io.compare = compare;
	Sort(io);
}

void ArrayCtrl::Sort(const Id& id, i32 (*compare)(const Val& v1, const Val& v2)) {
	Sort(idx.Find(id), compare);
}

void ArrayCtrl::Clear() {
	EndEdit();
	i32 oc = cursor;
	if(cursor >= 0) {
		WhenKillCursor();
		cursor = -1;
		info.Cancel();
	}
	array.Clear();
	if(oc >= 0) {
		WhenCursor();
		WhenSel();
	}
	cellinfo.Clear();
	ln.Clear();
	ClearCache();
	DisableCtrls();
	virtualcount = -1;
	anchor = -1;
	SetSb();
	Refresh();
	SyncInfo();
}

void ArrayCtrl::Shrink() {
	array.Shrink();
	cellinfo.Shrink();
	ln.Shrink();
}

void ArrayCtrl::SerializeSettings(Stream& s)
{
	i32 version = 0;
	s / version;
	header.Serialize(s);
	s % sortcolumn % sortcolumndescending;
	if(s.IsLoading())
		DoColumnSort();
}

void ArrayCtrl::Serialize(Stream& s)
{
}

void ArrayCtrl::Reset() {
	header.Reset();
	idx.Clear();
	id_ndx.Clear();
	id_ndx.Add(Id());
	column.Clear();
	control.Clear();
	nocursor = false;
	vertgrid = horzgrid = true;
	mousemove = false;
	inserting = false;
	popupex = true;
	appending = false;
	removing = false;
	askremove = true;
	duplicating = false;
	moving = false;
	appendline = false;
	noinsertappend = false;
	editmode = false;
	insertmode = false;
	multiselect = false;
	selectiondirty = false;
	hasctrls = false;
	headerctrls = false;
	nobg = false;
	selectcount = 0;
	bains = 0;
	row_name = t_("ряд");
	gridcolor = SColorShadow;
	autoappend = false;
	focussetcursor = true;
	sortcolumn = -1;
	allsorting = false;
	acceptingrow = 0;
	columnsortfindkey = false;
	spanwidecells = false;
	linecy = Draw::GetStdFontCy();
	Clear();
	sb.SetLine(linecy);
	columnsortsecondary = NULL;
	sorting_from = 0;
	min_visible_line = 0;
	max_visible_line = INT_MAX;
	ctrl_low = ctrl_high = 0;
	accept_edits = false;
}

void ArrayCtrl::CancelMode()
{
	isdrag = false;
	selclick = false;
	dropline = dropcol = -1;
	info.Cancel();
}

void ArrayCtrl::MouseWheel(Point p, i32 zdelta, dword keyflags) {
	if(keyflags & K_SHIFT)
		header.sb.Wheel(zdelta);
	else
		sb.Wheel(zdelta);
}

Vec<Val> ArrayCtrl::ReadRow(i32 i) const {
	Vec<Val> v;
	i32 n = max(idx.GetCount(), i < array.GetCount() ? array[i].line.GetCount() : 0);
	for(i32 j = 0; j < n; j++)
		v.Add(Get(i, j));
	return v;
}

void ArrayCtrl::Move(i32 dir) {
	i32 c = GetCursor();
	i32 d = c + dir;
	if(c < 0 || c >= GetCount() || d < 0 || d >= GetCount()) return;
	Vec<Val> row(ReadRow(c), 0);
	Set(c, ReadRow(d));
	Set(d, row);
	SetCursor(d);
	WhenArrayAction();
}

void ArrayCtrl::SwapUp() {
	Move(-1);
}

void ArrayCtrl::SwapDown() {
	Move(1);
}

ArrayCtrl& ArrayCtrl::ColumnWidths(tukk s)
{
	Vec<Txt> w = Split(s, ' ');
	for(i32 i = 0; i < min(w.GetCount(), header.GetCount()); i++) {
		i32 q = header.FindIndex(i);
		if(q >= 0)
			header.SetTabRatio(q, atoi(w[i]));
	}
	return *this;
}

Txt ArrayCtrl::GetColumnWidths()
{
	Txt text;
	for(i32 i = 0; i < header.GetCount(); i++)
		text << Format(i ? " %d" : "%d",
		              header.GetMode() == HeaderCtrl::SCROLL ? (i32)header[i].GetRatio()
		                                                     : header.GetTabWidth(i));
	return text;
}

ArrayCtrl& ArrayCtrl::OddRowColor(Color paper, Color ink)
{
	oddpaper = paper;
	oddink = ink;
	Refresh();
	SyncInfo();
	return *this;
}

ArrayCtrl& ArrayCtrl::EvenRowColor(Color paper, Color ink)
{
	evenpaper = paper;
	evenink = ink;
	Refresh();
	SyncInfo();
	return *this;
}

void ArrayCtrl::RefreshSel()
{
	Size sz = GetSize();
	i32 i = GetLineAt(sb);
	if(i >= 0) {
		i32 y = GetLineY(i) - sb;
		while(y < sz.cy && i < array.GetCount()) {
			if(IsSelected(i))
				RefreshRow(i);
			if(IsLineVisible(i))
				y += GetLineCy(i++);
		}
	}
}

void ArrayCtrl::DnD(i32 line, i32 col)
{
	if(dropline != line || dropcol != col) {
		RefreshRow(dropline - 1);
		RefreshRow(dropline);
		dropline = line;
		dropcol = col;
		RefreshRow(dropline - 1);
		RefreshRow(dropline);
	}
}

bool ArrayCtrl::DnDInsert(i32 line, i32 py, PasteClip& d, i32 q)
{
	i32 y = GetLineY(line);
	i32 cy = GetLineCy();
	if(py < y + cy / q) {
		WhenDropInsert(line, d);
		if(d.IsAccepted()) {
			DnD(line, DND_INSERTLINE);
			return true;
		}
	}
	if(py >= y + (q - 1) * cy / q && line < GetCount()) {
		WhenDropInsert(line + 1, d);
		if(d.IsAccepted()) {
			DnD(line + 1, DND_INSERTLINE);
			return true;
		}
	}
	return false;
}

void ArrayCtrl::DragAndDrop(Point p, PasteClip& d)
{
	if(IsReadOnly())
		return;
	if(!isdrag) {
		isdrag = true;
		RefreshSel();
	}
	i32 py = p.y + sb;
	i32 line = GetLineAt(py);
	i32 col = -1;
	if(line >= 0) {
		for(i32 i = 0; i < column.GetCount(); i++)
			if(GetCellRect(line, i).Contains(p)) {
				col = i;
				break;
			}
		if(col >= 0) {
			WhenDropCell(line, col, d);
			if(d.IsAccepted()) {
				DnD(line, col);
				return;
			}
		}
		if(DnDInsert(line, py, d, 4)) return;
		if(line >= 0) {
			WhenDropLine(line, d);
			if(d.IsAccepted()) {
				DnD(line, DND_LINE);
				return;
			}
		}
		if(DnDInsert(line, py, d, 2)) return;
	}
	i32 ci = GetCount();
	if(py < GetLineY(ci) + GetLineCy(ci) / 4 || !WhenDrop) {
		WhenDropInsert(ci, d);
		if(d.IsAccepted()) {
			DnD(GetCount(), DND_INSERTLINE);
			return;
		}
	}
	WhenDrop(d);
	DnD(-1, -1);
}

void ArrayCtrl::DragRepeat(Point p)
{
	sb = sb + GetDragScroll(this, p, 16).y;
}

void ArrayCtrl::DragLeave()
{
	isdrag = false;
	RefreshSel();
	DnD(-1, -1);
}

void ArrayCtrl::RemoveSelection()
{
	i32 ci = cursor;
	KillCursor();
	for(i32 i = GetCount() - 1; i >= 0; i--)
		if(IsSel(i) || i == ci)
			Remove(i); // Optimize!
}

void ArrayCtrl::InsertDrop(i32 line, const Vec< Vec<Val> >& data, PasteClip& d, bool self)
{
	if(data.GetCount() == 0)
		return;
	if(d.GetAction() == DND_MOVE && self) {
		if(IsMultiSelect())
			for(i32 i = GetCount() - 1; i >= 0; i--) {
				if(IsSel(i)) {
					Remove(i); // Optimize!
					if(i < line)
						line--;
				}
			}
		else
		if(IsCursor()) {
			if(GetCursor() < line)
				line--;
			Remove(GetCursor());
		}
		KillCursor();
		d.SetAction(DND_COPY);
	}
	Insert(line, data);
	ClearSelection();
	SetCursor(line + data.GetCount() - 1);
	if(IsMultiSelect())
		Select(line, data.GetCount());
}

void ArrayCtrl::InsertDrop(i32 line, const Vec<Val>& data, PasteClip& d, bool self)
{
	Vec< Vec<Val> > x;
	x.Add() <<= data;
	InsertDrop(line, x, d, self);
}

void ArrayCtrl::InsertDrop(i32 line, const Val& data, PasteClip& d, bool self)
{
	Vec<Val> x;
	x.Add(data);
	InsertDrop(line, x, d, self);
}

void ArrayCtrl::InsertDrop(i32 line, const ArrayCtrl& src, PasteClip& d)
{
	Vec< Vec<Val> > data;
	for(i32 i = 0; i < src.GetCount(); i++)
		if(src.IsSel(i))
			data.Add(src.GetLine(i));
	InsertDrop(line, data, d, &src == this);
}

void ArrayCtrl::InsertDrop(i32 line, PasteClip& d)
{
	InsertDrop(line, GetInternal<ArrayCtrl>(d), d);
}

Txt ArrayCtrl::AsText(Txt (*format)(const Val&), bool sel,
                         tukk tab, tukk row,
                         tukk hdrtab, tukk hdrrow) const
{
	Txt txt;
	if(hdrtab) {
		for(i32 i = 0; i < GetColumnCount(); i++) {
			if(i)
				txt << hdrtab;
			txt << (*format)(HeaderTab(i).GetText());
		}
		if(hdrrow)
			txt << hdrrow;
	}
	bool next = false;
	for(i32 r = 0; r < GetCount(); r++)
		if(!sel || IsSel(r)) {
			if(next)
				txt << row;
			for(i32 i = 0; i < GetColumnCount(); i++) {
				if(i)
					txt << tab;
				txt << (*format)(GetConvertedColumn(r, i));
			}
			next = true;
		}
	return txt;
}

void ArrayCtrl::SetClipboard(bool sel, bool hdr) const
{
	ClearClipboard();
	AppendClipboardText(AsText(AsTxt, sel, "\t", "\r\n", hdr ? "\t" : NULL, "\r\n"));
}

static Txt sQtfFormat(const Val& v)
{
	return "\1" + AsTxt(v) + "\1";
}

Txt ArrayCtrl::AsQtf(bool sel, bool hdr)
{
	Txt qtf;
	qtf << "{{";
	for(i32 i = 0; i < GetColumnCount(); i++) {
		if(i)
			qtf << ":";
		qtf << header.GetTabWidth(i);
	}
	if(hdr)
		qtf << "@L [*";
	return qtf << ' ' << AsText(sQtfFormat, sel, ":: ", ":: ", hdr ? ":: " : NULL, "::@W ]") << "}}";
}

static Txt sCsvFormat(const Val& v)
{
	return IsNumber(v) ? AsTxt(v) : CsvTxt(AsTxt(v));
}

Txt ArrayCtrl::AsCsv(bool sel, i32 sep, bool hdr)
{
	char h[2] = { (char)sep, 0 };
	return AsText(sCsvFormat, sel, h, "\r\n", hdr ? h : NULL, "\r\n");
}

ArrayCtrl::ArrayCtrl() {
	cursor = -1;
	Reset();
	AddFrame(sb);
	AddFrame(header);
	sb.AddFrame(scrollbox);
	header.WhenLayout = OTVET(HeaderLayout);
	header.WhenScroll = OTVET(HeaderScroll);
	sb.WhenScroll = OTVET(Scroll);
	header.Moving();
	WhenAcceptRow = [] { return true; };
	WhenBar = OTVET(StdBar);
	SetFrame(ViewFrame());
	oddpaper = evenpaper = SColorPaper;
	oddink = evenink = SColorText;
}

ArrayCtrl::~ArrayCtrl() {}

ArrayOption::ArrayOption()
{
	array = NULL;
	f = 0;
	t = 1;
	hswitch = vswitch = false;
	threestate = false;
	frame = true;
}

ArrayOption::~ArrayOption()
{
}

void ArrayOption::Click()
{
	if(!array)
		return;
	i32 c;
	for(c = index.GetCount(); --c >= 0;)
		if(array->FindColumnWithPos(index[c]) == array->GetClickColumn())
			break;
	i32 cr = array->GetCursor();
	if(c >= 0 && cr >= 0) {
		Val v = array->Get(index[c]);
		if(!IsNull(v) && v != t && v != f) {
			BeepExclamation();
			return;
		}
		if(hswitch)
			for(i32 i = 0; i < index.GetCount(); i++)
				if(i != c)
					array->Set(index[i], f);
		array->Set(index[c], threestate && v == t
			? Val() : v != t && !(threestate && IsNull(v)) ? t : f);
		if(vswitch) {
			for(i32 r = 0; r < array->GetCount(); r++)
				if(r != cr) {
					if(hswitch)
						for(i32 i = 0; i < index.GetCount(); i++)
							if(i != c)
								array->Set(r, index[i], f);
					array->Set(r, index[c], f);
				}
		}
		WhenAction();
		array->Action();
		array->WhenArrayAction();
	}
}

void ArrayOption::Disconnect()
{
	array = NULL;
	index.Clear();
}

void ArrayOption::Connect(ArrayCtrl& a, i32 i)
{
	array = &a;
	if(index.IsEmpty()) {
		a.WhenLeftClick << OTVET(Click);
		a.WhenLeftDouble << OTVET(Click);
	}
	index.Add(i);
}

ArrayCtrl::Column& ArrayOption::AddColumn(ArrayCtrl& ac, tukk text, i32 w)
{
	return AddColumn(ac, Id(), text, w).NoClickEdit();
}

ArrayCtrl::Column& ArrayOption::AddColumn(ArrayCtrl& ac, const Id& id, tukk text, i32 w)
{
	Connect(ac, ac.GetIndexCount());
	return ac.AddColumn(id, text, w).SetDisplay(*this).InsertVal(f).NoClickEdit();
}

void ArrayOption::Paint(Draw& w, const Rect& r, const Val& q,
		                Color ink, Color paper, dword style) const
{
	bool gray = (array && !array->IsEnabled());
	w.DrawRect(r, paper);

	i32 st = (gray ? CTRL_DISABLED : CTRL_NORMAL);
	i32 g = threestate && (q != t && q != f) ? CtrlsImg::I_O2 : q == t ? CtrlsImg::I_O1
		: CtrlsImg::I_O0;

	Image img = CtrlsImg::Get(g + st);

	Size crsz = min(img.GetSize(), r.Size());
	Rect cr = r.CenterRect(crsz);

	Point p = cr.CenterPos(img.GetSize());
	w.DrawImage(p.x, p.y, img);
}

}