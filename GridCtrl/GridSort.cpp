#include "GridCtrl.h"

namespace drx {

/*
bool GridCtrl::operator<(const Vec<Val>& a, const Vec<Val>& b)
{
	for(i32 i = 0; i < SortOrder.GetCount(); i++)
	{
		if(sortMode)
			return (StdValCompare((*a.items)[a.id][sortCol].val, (*b.items)[b.id][sortCol].val, 0) < 0);
		else
			return a.id < b.id;

	}
}
*/

void GridCtrl::GSort(i32 scol)
{
	i32 col;

	i32 cnt = sortOrder.GetCount();

	if(cnt == 0)
		return;

	if(scol < 0)
		scol = cnt - 1;

	col = sortOrder[scol];

	//i32 order = 1;
	i32 order = hitems[col].sortmode;

	/* dla trybu multisort nie mozna w ten sposob
	   sortowac dla pierwszej kolumny */

	if(cnt > 1 && scol != 0)
	{
		i32 match = 1;
		i32 is = fixed_rows;

		for(i32 i = fixed_rows + 1; i < total_rows; i++)
		{
			i32 n = vitems[i].id;
			i32 m = vitems[is].id;

			bool found = true;
			for(i32 j = 0; j < scol; j++)
			{
				i32 k = sortOrder[j];
				if(items[n][k].val != items[m][k].val)
				{
					found = false;
					break;
				}
			}

			if(found)
			{
				match++;
				continue;
			}
			else
			{
				if(match > 1)
					GSort(col, order, is, match);

				match = 1;
				is = i;
			}
		}
		if(match > 1)
			GSort(col, order, is, match);
	}
	else
		GSort(col, order, fixed_rows);

}

void GridCtrl::GSort(i32 col, i32 order, i32 from, i32 count)
{
	if(count == 0)
		return;

	ItemRect::sortCol = col;
	ItemRect::sortMode = (order != 0);

	VItems::Iterator its, ite;

	its = vitems.Begin() + from;

	if(count < 0)
		ite = vitems.End();
	else
		ite = its + count;

	if(order < 2)
		drx::Sort(SubRange(its, ite).Write(), StdLess<ItemRect>());
	else
		drx::Sort(SubRange(its, ite).Write(), StdGreater<ItemRect>());
}


void GridCtrl::Multisort()
{
	GSort(-1, 0, fixed_rows);
	for(i32 i = 0; i < sortOrder.GetCount(); i++)
		GSort(i);
}

i32 GridCtrl::InMultisort(i32 col)
{
	for(i32 i = 0; i < sortOrder.GetCount(); i++)
		if(col == sortOrder[i])
			return i;

	return -1;
}

void GridCtrl::ClearMultisort(i32 n)
{
	for(i32 i = n; i < sortOrder.GetCount(); i++)
	{
		i32 c = GetIdCol(sortOrder[i], true);
		hitems[c].sortmode = 0;
		hitems[c].sortcol = 0;
	}
	sortOrder.Clear();
}

bool GridCtrl::IsSorted()
{
	return sortOrder.GetCount() > 0;
}

void GridCtrl::MarkSort(i32 col, i32 sort_mode, bool refresh)
{
	i32 mcol = InMultisort(col);

	sortCol = col;
	hitems[col].sortmode = sort_mode;
	
	if(mcol < 0)
	{
		sortOrder.Add(col);
		mcol = sortOrder.GetCount() - 1;
	}

	hitems[col].sortcol = mcol;
	
	if(refresh)
		RefreshTop();
}

void GridCtrl::MarkSort(i32 col, i32 sort_mode)
{
	MarkSort(fixed_cols + col, sort_mode, true);
}

void GridCtrl::MarkSort(Id id, i32 sort_mode)
{
	MarkSort(aliases.Get(id), sort_mode, true);
}

GridCtrl& GridCtrl::Sort(i32 sort_col, i32 sort_mode, bool multisort, bool repaint)
{
	i32 col = GetIdCol(sort_col + fixed_cols);
	if(col < 0)
		return *this;

	if(!multisort)
		ClearMultisort();
	
	MarkSort(col, sort_mode, false);
	GSort();

	UpdateCursor();

	if(repaint)
		Repaint(false, true);
	return *this;
}

GridCtrl& GridCtrl::Sort(Id id, i32 sort_mode, bool multisort, bool repaint)
{
	return Sort(aliases.Get(id) - fixed_cols, sort_mode, multisort, repaint);
}

GridCtrl& GridCtrl::MultiSort(i32 sort_col, i32 sort_mode)
{
	return Sort(sort_col, sort_mode, true);
}

GridCtrl& GridCtrl::MultiSort(Id id, i32 sort_mode)
{
	return Sort(aliases.Get(id) - fixed_cols, sort_mode, true);
}

void GridCtrl::ClearSort()
{
	Sort(0, SORT_ID);
}

void GridCtrl::ReSort()
{
	Multisort();
	Repaint(false, true);
}

Vec<Id> GridCtrl::GetSortOrderId() const
{
	Vec<Id> v;
	for(i32 i = 0; i < sortOrder.GetCount(); i++)
		v.Add(aliases.GetKey(sortOrder[i]));
	return v;
}

Vec<GridCtrl::SortOrder> GridCtrl::GetSortOrder() const
{
	Vec<SortOrder> v;

	for(i32 i = 0; i < sortOrder.GetCount(); i++)
	{
		i32 c = sortOrder[i];
		SortOrder& s = v.Add();
		s.id = c;
		s.name = aliases.GetKey(c);
		s.ascending = hitems[c].IsSortAsc();
		s.descending = hitems[c].IsSortDsc();
	}

	return v;
}

}
