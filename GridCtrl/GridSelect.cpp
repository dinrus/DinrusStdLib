#include "GridCtrl.h"

namespace drx {

void GridCtrl::Select(i32 n, i32 cnt /* = 1*/)
{
	SelectCount(n + fixed_rows, cnt, true);
}

void GridCtrl::SelectCount(i32 i, i32 cnt, bool sel)
{
	if(cnt <= 0)
		return;
	SelectRange(Point(fixed_cols, i), Point(total_cols - 1, i + cnt - 1), sel);
}

void GridCtrl::SelectRange(i32 from, i32 to, bool sel)
{
	SelectRange(Point(fixed_cols, from), Point(total_cols - 1, to), sel);
}

void GridCtrl::ShiftSelect(i32 from, i32 to)
{
	ShiftSelect(Point(fixed_cols, from), Point(total_cols - 1, to));
}

void GridCtrl::SelectRange(Point from, Point to, bool sel /* = true*/, bool fullrow /* = false*/)
{
	Point f, t;

	if(fullrow)
	{
		from.x = fixed_cols;
		to.x = total_cols - 1;
	}

	if(from.y < to.y)
	{
		f = from;
		t = to;
	}
	else
	{
		f = to;
		t = from;
	}

	i32 ymin = min(f.y, t.y);
	i32 ymax = max(f.y, t.y);

	i32 xmin = f.x;
	i32 xmax = t.x;
	
	if(ymin < 0 || xmin < 0)
		return;

	if(xmin > xmax)
	{
		i32 t = xmin;
		xmin = xmax;
		xmax = t;
	}

	for(i32 i = ymin; i <= ymax; i++)
	{
		ItemRect &ir = vitems[i];
		i32 yid = ir.id;

		bool is_row_selected = false;
		bool do_refresh = false;

		for(i32 j = fixed_cols; j < total_cols; j++)
		{
			i32 xid = hitems[j].id;
			Item &it = items[yid][xid];

			if(j >= xmin && j <= xmax)
			{
				if(it.IsSelect() != sel)
				{
					it.Select(sel);
					do_refresh = true;
				}
				if(sel)
				{
					is_row_selected = true;
					selected_items++;
				}
				else
					selected_items--;
			}
			else if(it.IsSelect())
				is_row_selected = true;
		}

		if(!ir.IsSelect())
		{
			if(is_row_selected)
				selected_rows++;
		}
		else if(!is_row_selected)
			selected_rows--;

		ir.Select(is_row_selected);

		if(do_refresh)
			RefreshRow(i, false, false);

	}
}

void GridCtrl::SelectInverse(i32 from, i32 to)
{
	i32 nfrom = min(from, to);
	i32 nto = max(from, to);

	for(i32 i = nfrom ; i <= nto; i++)
	{
		vitems[i].Select(!vitems[i].IsSelect());
		if(vitems[i].IsSelect())
			selected_rows++;
		else
			selected_rows--;
		RefreshRow(i, 0);
	}
}

void GridCtrl::ShiftSelect(Point from, Point to)
{
	Point f, t;

	if(from.y < to.y)
	{
		f = from;
		t = to;
	}
	else
	{
		f = to;
		t = from;
	}

	if(select_row)
	{
		f.x = fixed_cols;
		t.x = total_cols;
	}

	i32 ymin = f.y;
	i32 ymax = t.y;

	i32 xmin = f.x;
	i32 xmax = t.x;

	if(ymin == ymax && xmin > xmax)
	{
		i32 t = xmin;
		xmin = xmax;
		xmax = t;
	}

	selected_rows = 0;
	selected_items = 0;

	for(i32 i = fixed_rows; i < total_rows; i++)
	{
		ItemRect &ir = vitems[i];
		i32 yid = ir.id;

		bool is_row_selected = false;
		bool do_refresh = false;

		if((i >= ymin && i <= ymax))
		{
			for(i32 j = fixed_cols; j < total_cols; j++)
			{
				i32 xid = hitems[j].id;

				bool s = true;
				if(i == ymin && ymin == ymax)
					s = (j >= xmin && j <= xmax);
				else if(i == ymin) s = (j >= xmin);
				else if(i == ymax) s = (j <= xmax);

				if(items[yid][xid].IsSelect() != s)
				{
					items[yid][xid].Select(s);
					do_refresh = true;
				}
				if(s)
				{
					is_row_selected = true;
					selected_items++;
				}
			}
		}
		else
		{
			for(i32 j = fixed_cols; j < total_cols; j++)
				if(items[yid][j].IsSelect())
				{
					items[yid][j].Select(false);
					do_refresh = true;
				}
		}

		if(is_row_selected)
			selected_rows++;

		ir.Select(is_row_selected);

		if(do_refresh)
			RefreshRow(i, false, false);
	}
}

void GridCtrl::SelectRange(const Rect& r, bool sel /* = true*/, bool fullrow /* = false*/)
{
	Rect rr = r.Normalized();
	SelectRange(rr.TopLeft(), rr.BottomRight(), sel, fullrow);
}

bool GridCtrl::IsSelected(i32 n, bool relative)
{
	//i32 id = vitems[n + (relative ? fixed_rows : 0)].id;
	i32 id = n + (relative ? fixed_rows : 0);
	return vitems[id].IsSelect() || vitems[id].IsCursor();
}

bool GridCtrl::IsSelect(i32 n, i32 m, bool relative)
{
	i32 r = relative ? fixed_rows + n : n;
	i32 c = relative ? fixed_cols + m : m;
	Item &it = GetItem(r, c);
	return it.IsSelect();
}

bool GridCtrl::IsSelected(i32 n, i32 m, bool relative)
{
	i32 r = relative ? fixed_rows + n : n;
	i32 c = relative ? fixed_cols + m : m;
	Item &it = GetItem(r, c);
	return it.IsSelect() || it.IsCursor();
}

bool GridCtrl::IsSelected()
{
	return IsSelected(rowidx, false);
}

void GridCtrl::ClearSelection()
{
	LG(0, "Cleared %d", selected_rows);
	if(selected_rows > 0)
	{
		for(i32 i = fixed_rows; i < total_rows; i++)
		{
			vitems[i].Select(0);
			for(i32 j = fixed_cols; j < total_cols; j++)
				items[i][j].Select(0);
		}

		anchor = Null;

		Refresh();
		selected_rows = 0;
		selected_items = 0;
	}
}

};