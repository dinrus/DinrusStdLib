#include "RichText.h"

namespace drx {

RichTable::Format RichText::GetTableFormat(i32 table) const
{
	Mutex::Lock __(mutex);
	return GetConstTable(table).GetFormat();
}

void RichText::SetTableFormat(i32 table, const RichTable::Format& fmt)
{
	RichTable& tab = GetUpdateTable(table);
	tab.SetFormat(fmt);
	tab.Normalize();
}

i32  RichText::SetTable(i32 pos, const RichTable& table)
{
	RefreshAll();
	RichPos p = GetRichPos(pos);
	i32 bpos = pos;
	RichTxt& txt = GetUpdateText(pos);
	i32 pi = txt.FindPart(pos);
	ASSERT(pos == 0 && txt.GetPartLength(pi) == 0 && txt.IsPara(pi));
	RichTable pt(table, 1);
	txt.SetPick(pi, pick(pt));
	return GetRichPos(bpos).table;
}

RichTable RichText::CopyTable(i32 table) const
{
	Mutex::Lock __(mutex);
	RichTable tab(GetConstTable(table), 1);
	return tab;
}

void RichText::ReplaceTable(i32 table, const RichTable& tab)
{
	(GetUpdateTable(table) <<= tab).Normalize();
	RefreshAll();
}

void      RichText::DestroyTable(i32 table)
{
	i32 pi;
	RichTxt& txt = GetTableUpdateText(table, style, pi);
	RichPara p;
	txt.Set(pi, p, style);
	RefreshAll();
}

void RichText::RemoveParaSpecial(i32 table, bool before)
{
	i32 pi;
	RichTxt& txt = GetTableUpdateText(table, style, pi);
	if(before)
		pi--;
	else
		pi++;
	ASSERT(txt.IsPara(pi) && txt.GetPartLength(pi) == 0);
	txt.part.Remove(pi);
	RefreshAll();
}

bool RichText::CanRemoveParaSpecial(i32 table, bool before)
{
	i32 pi;
	if(!table)
		return false;
	RichTxt& txt = GetTableUpdateText(table, style, pi);
	if(before)
		pi--;
	else
		pi++;
	return pi >= 0 && pi < txt.GetPartCount() && txt.IsPara(pi) && txt.GetPartLength(pi) == 0;
}

bool RichText::ShouldInsertParaSpecial(i32 table, bool before)
{
	i32 pi;
	RichTxt& txt = GetTableUpdateText(table, style, pi);
	if(!before)
		pi++;
	return pi == 0 || pi >= txt.GetPartCount() || txt.IsTable(pi);
}


void RichText::InsertParaSpecial(i32 table, bool before, const RichPara::Format& fmt)
{
	i32 pi;
	RichTxt& txt = GetTableUpdateText(table, style, pi);
	if(!before)
		pi++;
	txt.RefreshAll();
	txt.part.Insert(pi);
	RichPara p;
	p.format = fmt;
	txt.Set(pi, p, style);
	RefreshAll();
}

RichTable RichText::CopyTable(i32 table, const Rect& sel) const
{
	Mutex::Lock __(mutex);
	return GetConstTable(table).Copy(sel);
}

Point RichText::GetMasterCell(i32 table, i32 row, i32 column)
{
	return GetConstTable(table).GetMasterCell(row, column);
}

void  RichText::PasteTable(i32 table, Point pos, const RichTable& tab)
{
	GetUpdateTable(table).Paste(pos, tab);
	RefreshAll();
}

void  RichText::InsertTableRow(i32 table, i32 row)
{
	GetUpdateTable(table).InsertRow(row, style);
	RefreshAll();
}

void  RichText::RemoveTableRow(i32 table, i32 row)
{
	GetUpdateTable(table).RemoveRow(row);
	RefreshAll();
}

void  RichText::InsertTableColumn(i32 table, i32 column)
{
	GetUpdateTable(table).InsertColumn(column, style);
	RefreshAll();
}

void  RichText::RemoveTableColumn(i32 table, i32 column)
{
	GetUpdateTable(table).RemoveColumn(column);
	RefreshAll();
}

void  RichText::SplitCell(i32 table, Point cell, Size sz)
{
	GetUpdateTable(table).SplitCell(cell, sz, style);
	RefreshAll();
}

void  RichText::JoinCell(i32 table, const Rect& sel)
{
	RichTable& tab = GetUpdateTable(table);
	RichCell& cell = tab[sel.top][sel.left];
	cell.vspan = sel.bottom - sel.top;
	cell.hspan = sel.right - sel.left;
	for(i32 i = sel.top; i <= sel.bottom; i++)
		for(i32 j = sel.left; j <= sel.right; j++) {
			if(tab(i, j) && (i != sel.top || j != sel.left)) {
				RichTxt& t = tab[i][j].text;
				for(i32 pi = 0; pi < t.GetPartCount(); pi++)
					if(t.IsTable(pi))
						cell.text.CatPick(pick(t.part[pi].Get<RichTable>()));
					else
					if(pi < t.GetPartCount() - 1 || t.GetPartLength(pi))
						cell.text.Cat(t.Get(pi, style), style);
				tab[i][j].ClearText();
			}
		}
	tab.Normalize();
	RefreshAll();
}

RichCell::Format RichText::GetCellFormat(i32 table, const Rect& sel) const
{
	Mutex::Lock __(mutex);
	return GetConstTable(table).GetCellFormat(sel);
}

void RichText::SetCellFormat(i32 table, const Rect& sel, const RichCell::Format& fmt, bool setkeep, bool setround)
{
	GetUpdateTable(table).SetCellFormat(sel, fmt, setkeep, setround);
	RefreshAll();
}

void RichText::ClearTable(i32 table, const Rect& sel)
{
	RichTable& tab = GetUpdateTable(table);
	for(i32 i = sel.top; i <= sel.bottom; i++)
		for(i32 j = sel.left; j <= sel.right; j++) {
			if(tab(i, j)) {
				tab.InvalidateRefresh(i, j);
				tab[i][j].ClearText(tab[i][j].text.GetFirstFormat(style), style);
			}
		}
	tab.Normalize();
	RefreshAll();
}

RichText::FormatInfo RichText::GetTableFormatInfo(i32 table, const Rect& sel) const
{
	Mutex::Lock __(mutex);
	const RichTable& tab = GetConstTable(table);
	bool first = true;
	FormatInfo fi;
	for(i32 i = sel.top; i <= sel.bottom; i++)
		for(i32 j = sel.left; j <= sel.right; j++) {
			if(tab(i, j)) {
				const RichTxt& txt = tab[i][j].text;
				txt.CombineFormat(fi, 0, txt.GetPartCount(), first, style);
			}
		}
	return fi;
}

void       RichText::ApplyTableFormatInfo(i32 table, const Rect& sel, const RichText::FormatInfo& fi)
{
	RichTable& tab = GetUpdateTable(table);
	for(i32 i = sel.top; i <= sel.bottom; i++)
		for(i32 j = sel.left; j <= sel.right; j++) {
			if(tab(i, j)) {
				tab.InvalidateRefresh(i, j);
				RichTxt& txt = tab[i][j].text;
				txt.ApplyFormat(fi, 0, txt.GetPartCount(), style);
			}
		}
	tab.Normalize();
	RefreshAll();
}

}
