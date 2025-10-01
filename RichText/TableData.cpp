#include "RichText.h"

namespace drx {

void RichTable::Invalidate()
{
	clayout.py.page = -1;
	length = tabcount = -1;
}

void RichTable::InvalidateRefresh(i32 i, i32 j)
{
	if(i < format.header)
		r_row = -1;
	else
	if(r_row != i || r_column != j) {
		if(r_row == -2 && clayout.sz == GetSize()) {
			r_row = i;
			r_column = j;
			r_py = clayout.py;
			r_pyy = clayout[min(GetRows() - 1, i + cell[i][j].vspan)].pyy;
			r_first_page = clayout.first_page;
			r_next_page = clayout.next_page;
		}
		else
			r_row = -1;
	}
	Invalidate();
}

i32  RichTable::GetInvalid(PageY& top, PageY& bottom, RichContext rc) const
{
	if(r_row == -2)
		return -1;
	const TabLayout& tab = Realize(rc);
	Rect first_page, next_page;
	Reduce(rc, first_page, next_page);
	if(r_row >= 0 && r_first_page == first_page && r_next_page == next_page
	   && r_py == rc.py && tab[min(GetRows() - 1, r_row + cell[r_row][r_column].vspan)].pyy == r_pyy) {
		const PaintRow& pr = tab[r_row];
		const RichCell& cl = cell[r_row][r_column];
		top = pr.py;
		bottom = tab[min(cell.GetCount() - 1, r_row + cl.vspan)].pyy;
		return 0;
	}
	return 1;
}

void RichTable::Normalize0()
{
	i32 nx = format.column.GetCount();
	i32 ny = cell.GetCount();

	for(i32 i = 0; i < ny; i++)
		cell[i].SetCount(nx);

	Invalidate();
	r_py.page = -1;
	r_row = -1;

	ci.Alloc(ny);
	for(i32 i = 0; i < ny; i++)
		ci[i].Alloc(nx);
	for(i32 i = 0; i < ny; i++)
		for(i32 j = 0; j < nx; j++)
			if(ci[i][j].valid) {
				RichCell& c = cell[i][j];
				i32 vs = i < format.header ? min(format.header - 1, i + c.vspan) : min(ny - 1, i + c.vspan);
				i32 hs = min(nx - 1, j + c.hspan);
				c.vspan = vs - i;
				c.hspan = hs - j;
				for(i32 ii = i; ii <= vs; ii++)
					for(i32 jj = j; jj <= hs; jj++) {
						if((i != ii || j != jj) && ii < ny && jj < nx) {
							CellInfo& f = ci[ii][jj];
							f.valid = false;
							cell[ii][jj].Clear();
							f.master = Point(j, i);
							cell[ii][jj].hspan = hs - jj;
						}
					}
			}
}

bool RichTable::IsRowEmpty(i32 row)
{
	for(i32 i = 0; i < GetColumns(); i++)
		if(ci[row][i].valid && cell[row][i].vspan == 0)
			return false;
	return true;
}

bool RichTable::IsColumnEmpty(i32 column)
{
	for(i32 i = 0; i < GetRows(); i++)
		if(ci[i][column].valid)
			return false;
	return true;
}

void RichTable::RemoveRow0(i32 rowi)
{
	if(rowi < format.header)
		format.header--;
	for(i32 i = 0; i < GetColumns(); i++) {
		CellInfo& cf = ci[rowi][i];
		if(!cf.valid && cf.master.x == i)
			cell[cf.master.y][cf.master.x].vspan--;
	}
	cell.Remove(rowi);
}

void RichTable::RemoveColumn0(i32 column)
{
	for(i32 i = 0; i < cell.GetCount(); i++) {
		CellInfo& cf = ci[i][column];
		if(!cf.valid && cf.master.y == i)
			cell[cf.master.y][cf.master.x].hspan--;
		cell[i].Remove(column);
	}
	format.column.Remove(column);
}

void RichTable::Normalize()
{
	Normalize0();
	i32 i = 1;
	while(i < GetRows())
		if(IsRowEmpty(i)) {
			Array<RichCell> r(cell[i], 1);
			RemoveRow0(i);
			for(i32 j = 0; j < GetColumns(); j++)
				if(ci[i][j].valid) {
					cell[i][j] <<= r[j];
					cell[i][j].vspan--;
				}
			Normalize0();
		}
		else
			i++;
	i32 j = 1;
	while(j < GetColumns())
		if(IsColumnEmpty(j)) {
			Array<RichCell> r;
			for(i32 i = 0; i < GetRows(); i++)
				r.Add() <<= cell[i][j];
			i32 c = format.column[j];
			RemoveColumn0(j);
			format.column[min(GetColumns() - 1, j - 1)] += c;
			for(i32 i = 0; i < GetRows(); i++)
				if(ci[i][j].valid)
					cell[i][j] <<= r[i];
			Normalize0();
		}
		else
			j++;
	i32 sum = Sum(format.column);
	if(sum != 10000) {
		r_row = -1;
		if(format.column.GetCount()) {
			if(format.column.GetCount() > 1) {
				i32 q = 0;
				for(i32 i = 0; i < format.column.GetCount() - 1; i++)
					q += sum <= 0 ? (format.column[i] = 10000 / format.column.GetCount())
					              : (format.column[i] = format.column[i] * 10000 / sum);
				format.column[format.column.GetCount() - 1] = 10000 - q;
			}
			else
				format.column[0] = 10000;
		}
	}
}

i32 RichTable::GetLength() const
{
	if(length < 0) {
		length = 0;
		for(i32 i = 0; i < cell.GetCount(); i++)
			for(i32 j = 0; j < format.column.GetCount(); j++)
				if(ci[i][j].valid)
					length += cell[i][j].text.GetLength() + 1;
	}
	return length ? length - 1 : 0;
}

Point   RichTable::FindCell(i32& pos) const
{
	for(i32 i = 0; i < cell.GetCount(); i++)
		for(i32 j = 0; j < format.column.GetCount(); j++)
			if(ci[i][j].valid) {
				i32 l = cell[i][j].text.GetLength() + 1;
				if(pos < l)
					return Point(j, i);
				pos -= l;
			}
	NEVER();
	return Point();
}

i32 RichTable::GetCellPos(i32 ii, i32 jj) const
{
	i32 pos = 0;
	for(i32 i = 0; i < ii; i++)
		for(i32 j = 0; j < format.column.GetCount(); j++)
			if(ci[i][j].valid)
				pos += cell[i][j].text.GetLength() + 1;
	for(i32 j = 0; j < jj; j++)
		if(ci[ii][j].valid)
			pos += cell[ii][j].text.GetLength() + 1;
	return pos;
}

Point RichTable::GetMasterCell(i32 i, i32 j) const
{
	i = min(GetRows() - 1, i);
	j = min(GetColumns() - 1, j);
	const CellInfo& cf = ci[i][j];
	return cf.valid ? Point(j, i) : cf.master;
}

const RichCell& RichTable::GetMaster(i32 i, i32 j) const
{
	Point p = GetMasterCell(i, j);
	return cell[p.y][p.x];
}

i32 RichTable::GetTableCount(i32 ii, i32 jj) const
{
	i32 ti = 0;
	for(i32 i = 0; i < ii; i++)
		for(i32 j = 0; j < format.column.GetCount(); j++)
			if(ci[i][j].valid)
				ti += cell[i][j].text.GetTableCount();
	for(i32 j = 0; j < jj; j++)
		if(ci[ii][j].valid)
			ti += cell[ii][j].text.GetTableCount();
	return ti;
}


void RichTable::ClearSpelling()
{
	for(i32 i = 0; i < cell.GetCount(); i++)
		for(i32 j = 0; j < format.column.GetCount(); j++)
			if(ci[i][j].valid)
				cell[i][j].text.ClearSpelling();
}

i32 RichTable::GetTableCount() const
{
	if(tabcount < 0) {
		tabcount = 0;
		for(i32 i = 0; i < cell.GetCount(); i++) {
			for(i32 j = 0; j < format.column.GetCount(); j++)
				if(ci[i][j].valid)
					tabcount += cell[i][j].text.GetTableCount();
		}
	}
	return tabcount;
}

void RichTable::Validate()
{
	r_row = -2;
}

void RichTable::AddColumn(i32 cx)
{
	format.column.Add(cx);
}

void RichTable::SetPick(i32 i, i32 j, RichTxt&& text)
{
	cell.At(i).At(j).text = pick(text);
}

RichTxt RichTable::GetPick(i32 i, i32 j)
{
	return pick(cell[i][j].text);
}

void RichTable::SetQTF(i32 i, i32 j, tukk qtf)
{
	SetPick(i, j, ParseQTF(qtf));
}

void RichTable::SetFormat(i32 i, i32 j, const RichCell::Format& fmt)
{
	cell.At(i).At(j).format = fmt;
}

void RichTable::SetSpan(i32 i, i32 j, i32 vspan, i32 hspan)
{
	RichCell& c = cell.At(i).At(j);
	c.vspan = vspan;
	c.hspan = hspan;
}

Size RichTable::GetSpan(i32 i, i32 j) const
{
	const RichCell& c = cell[i][j];
	return Size(c.hspan, c.vspan);
}

void RichTable::SetFormat(const Format& fmt)
{
	format = fmt;
	drx::SetQTF(header, fmt.header_qtf);
	drx::SetQTF(footer, fmt.footer_qtf);
}

RichTable RichTable::Copy(const Rect& sel) const
{
	RichTable r;
	r.format = format;
	r.format.header = max(0, format.header - sel.top);
	r.format.column.Remove(0, sel.left);
	r.format.column.SetCount(sel.right - sel.left + 1);
	for(i32 i = sel.top; i <= sel.bottom; i++)
		for(i32 j = sel.left; j <= sel.right; j++)
			r.cell.At(i - sel.top).At(j - sel.left) <<= cell[i][j];
	r.Normalize();
	return r;
}

void RichTable::RemoveRow(i32 rowi)
{
	RemoveRow0(rowi);
	Normalize();
}

void RichTable::InsertRow(i32 rowi, const RichStyles& style)
{
	if(rowi < format.header)
		format.header++;
	i32 si;
	if(rowi < GetRows()) {
		for(i32 i = 0; i < GetColumns(); i++) {
			CellInfo& cf = ci[rowi][i];
			if(!cf.valid && cf.master.x == i)
				cell[cf.master.y][cf.master.x].vspan++;
		}
		si = rowi + 1;
	}
	else
		si = rowi - 1;
	cell.Insert(rowi).SetCount(GetColumns());
	if(si >= 0)
		for(i32 i = 0; i < GetColumns(); i++) {
			RichCell& c = cell[rowi][i];
			const RichCell& sc = cell[si][i];
			c.format = sc.format;
			DUMP(sc.hspan);
			c.hspan = sc.hspan;
			c.ClearText(sc.text.GetFirstFormat(style), style);
		}
	Normalize();
}

void RichTable::RemoveColumn(i32 column)
{
	RemoveColumn0(column);
	Normalize();
}

void RichTable::InsertColumn(i32 column, const RichStyles& style)
{
	i32 sci = column < GetColumns() ? column + 1 : column - 1;
	for(i32 i = 0; i < cell.GetCount(); i++) {
		CellInfo& cf = ci[i][column];
		if(!cf.valid && cf.master.y == i)
			cell[cf.master.y][cf.master.x].hspan++;
		RichCell& c = cell[i].Insert(column);
		if(sci >= 0) {
			const RichCell& sc = cell[i][sci];
			c.format = sc.format;
			c.vspan = sc.vspan;
			c.ClearText(sc.text.GetFirstFormat(style), style);
		}
	}
	i32 c = format.column[min(column, GetColumns() - 1)];
	format.column.Insert(column, c);
	Normalize();
}

void  RichTable::SplitCell(Point cl, Size sz, const RichStyles& style)
{
	const RichCell& sc = cell[cl.y][cl.x];
	i32 exty = sz.cy - cell[cl.y][cl.x].vspan - 1;
	i32 extx = sz.cx - cell[cl.y][cl.x].hspan - 1;
	i32 ny = cell.GetCount() + exty;
	i32 nx = format.column.GetCount() + extx;
	if(ny < 0 || ny > 20000 || nx < 0 || nx > 200)
		return;
	if(exty > 0) {
		cell.InsertN(cl.y + 1, exty);
		if(cl.y < format.header)
			format.header += exty;
		for(i32 i = 0; i < exty; i++) {
			cell[cl.y + 1 + i].SetCount(GetColumns());
			for(i32 j = 0; j < GetColumns(); j++) {
				RichCell& c = cell[cl.y + 1 + i][j];
				const RichCell& sc = cell[cl.y][j];
				c.format = sc.format;
				c.hspan = sc.hspan;
				c.ClearText(sc.text.GetFirstFormat(style), style);
			}
		}
		for(i32 i = 0; i < GetColumns(); i++) {
			CellInfo& cf = ci[cl.y][i];
			if(cf.valid)
				cell[cl.y][i].vspan += exty;
			else
			if(cf.master.x == i)
				cell[cf.master.y][cf.master.x].vspan += exty;
		}
	}

	cell[cl.y][cl.x].vspan = 0;
	if(exty < 0)
		cell[cl.y + sz.cy - 1][cl.x].vspan = -exty;
	for(i32 i = 1; i < sz.cy; i++) {
		RichCell& c = cell[cl.y + i][cl.x];
		c.format = sc.format;
		c.hspan = sc.hspan;
		c.ClearText(sc.text.GetFirstFormat(style), style);
	}

	Normalize0();
	// ext = sz.cx - cell[cl.y][cl.x].hspan - 1;
	if(extx > 0) {
		i32 clx = 0;
		for(i32 i = 0; i <= cell[cl.y][cl.x].hspan; i++)
			clx += format.column[cl.x + i];
		format.column.InsertN(cl.x, extx);
		i32 q = clx / sz.cx;
		for(i32 i = 1; i < sz.cx; i++)
			format.column[cl.x + i] = q;
		format.column[cl.x] = clx - (sz.cx - 1) * q;
		for(i32 i = 0; i < cell.GetCount(); i++) {
			cell[i].InsertN(cl.x + 1, extx);
			for(i32 q = 0; q < extx; q++) {
				RichCell& c = cell[i][cl.x + 1 + q];
				const RichCell& sc = cell[i][cl.x];
				c.format = sc.format;
				c.vspan = sc.vspan;
				c.ClearText(sc.text.GetFirstFormat(style), style);
			}
			CellInfo& cf = ci[i][cl.x];
			if(cf.valid)
				cell[i][cl.x].hspan += extx;
			else
			if(cf.master.y == i)
				cell[cf.master.y][cf.master.x].hspan += extx;
		}
	}
	for(i32 i = 0; i < sz.cy; i++)
		for(i32 j = 0; j < sz.cx; j++) {
			RichCell& c = cell[cl.y + i][cl.x + j];
			if(j < sz.cx - 1)
				c.hspan = 0;
			if(j) {
				const RichCell& sc = cell[cl.y + i][cl.x];
				c.format = sc.format;
				c.vspan = sc.vspan;
				c.ClearText(sc.text.GetFirstFormat(style), style);
			}
		}
	Normalize();
}

void sMatchRect(Rect& t, const Rect& s)
{
	if(t.left != s.left)
		t.left = Null;
	if(t.top != s.top)
		t.top = Null;
	if(t.right != s.right)
		t.right = Null;
	if(t.bottom != s.bottom)
		t.bottom = Null;
}

RichCell::Format RichTable::GetCellFormat(const Rect& sel) const
{
	RichCell::Format fmt;
	for(i32 i = sel.top; i <= sel.bottom; i++)
		for(i32 j = sel.left; j <= sel.right; j++)
			if(i == sel.top && j == sel.left)
				fmt = cell[i][j].format;
			else
			if(ci[i][j].valid) {
				const RichCell::Format& f = cell[i][j].format;
				sMatchRect(fmt.border, f.border);
				sMatchRect(fmt.margin, f.margin);
				if(fmt.align != f.align)
					fmt.align = Null;
				if(fmt.color != f.color)
					fmt.color = VoidColor;
				if(fmt.bordercolor != f.bordercolor)
					fmt.bordercolor = VoidColor;
			}
	return fmt;
}

void sSetRect(Rect& t, const Rect& s)
{
	if(!IsNull(s.left))
		t.left = s.left;
	if(!IsNull(s.top))
		t.top = s.top;
	if(!IsNull(s.right))
		t.right = s.right;
	if(!IsNull(s.bottom))
		t.bottom = s.bottom;
}

void  RichTable::SetCellFormat(const Rect& sel, const RichCell::Format& fmt,
                               bool setkeep, bool setround)
{
	for(i32 i = sel.top; i <= sel.bottom; i++)
		for(i32 j = sel.left; j <= sel.right; j++)
			if(ci[i][j].valid) {
				RichCell::Format& f = cell[i][j].format;
				sSetRect(f.border, fmt.border);
				sSetRect(f.margin, fmt.margin);
				if(!IsNull(fmt.align))
					f.align = fmt.align;
				if(fmt.color != VoidColor)
					f.color = fmt.color;
				if(fmt.bordercolor != VoidColor)
					f.bordercolor = fmt.bordercolor;
				if(!IsNull(fmt.minheight))
					f.minheight = fmt.minheight;
				if(setkeep)
					f.keep = fmt.keep;
				if(setround)
					f.round = fmt.round;
			}

	Normalize();
}

void  RichTable::Paste(Point pos, const RichTable& tab)
{
	for(i32 i = 0; i < tab.GetRows(); i++)
		for(i32 j = 0; j < min(GetColumns() - pos.x, tab.GetColumns()); j++)
			cell.At(i + pos.y, cell[cell.GetCount() - 1]).At(j + pos.x) <<= tab[i][j];
	Normalize();
}

void RichTable::AdjustSel(Rect& sel) const
{
again:
	for(i32 i = sel.top; i <= sel.bottom; i++)
		for(i32 j = sel.left; j <= sel.right; j++) {
			Point p = GetMasterCell(i, j);
			const RichCell& c = cell[p.y][p.x];
			Point pp = p + Point(c.hspan, c.vspan);
			if(p.x < sel.left) {
				sel.left = p.x;
				goto again;
			}
			if(pp.x > sel.right) {
				sel.right = pp.x;
				goto again;
			}
			if(p.y < sel.top) {
				sel.top = p.y;
				goto again;
			}
			if(pp.y > sel.bottom) {
				sel.bottom = pp.y;
				goto again;
			}
		}
}

RichTable::RichTable(const RichTable& src, i32)
{
	format = src.format;
	cell <<= src.cell;
	r_row = -1;
	Normalize();
}

RichTable::RichTable()
{
	clayout.py.page = -1;
	r_row = -1;
	Invalidate();
}

}
