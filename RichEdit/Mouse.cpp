#include "RichEdit.h"

namespace drx {

Size RichEdit::GetPhysicalSize(const RichObj& obj)
{
	if(ignore_physical_size)
		return 600 * obj.GetPixelSize() / 96;
	return obj.GetPhysicalSize();
}

void RichEdit::CancelMode()
{
	tabmove.table = 0;
	selclick = false;
	dropcaret.Clear();
}

void RichEdit::MouseWheel(Point p, i32 zdelta, dword keyflags)
{
	if(keyflags == K_CTRL) {
		if(IsNull(floating_zoom))
			ZoomView(sgn(zdelta));
		else {
			floating_zoom = minmax(floating_zoom + zdelta / 480.0, 0.5, 10.0);
			RefreshLayoutDeep();
		}
	}
	else
		sb.Wheel(zdelta);
}

RichHotPos RichEdit::GetHotPos(Point p)
{
	i32 x;
	PageY py;
	GetPagePoint(p, py, x);
	return text.GetHotPos(x, py, 4 / GetZoom(), pagesz);
}

void RichEdit::SetObjectPercent(i32 p)
{
	if(objectpos >= 0) {
		RichObj obj = GetObject();
		Size sz = GetPhysicalSize(obj) * p / 100;
		if(sz.cx > 0 && sz.cy > 0) {
			obj.SetSize(sz);
			obj.KeepRatio(true);
			ReplaceObject(obj);
		}
	}
}

void RichEdit::SetObjectYDelta(i32 pt)
{
	if(objectpos >= 0) {
		RichObj obj = GetObject();
		obj.SetYDelta(pt * 25 / 3);
		ReplaceObject(obj);
	}
}

void RichEdit::SetObjectPos(i32 pos)
{
	Rect r = GetObjectRect(cursor);
	Rect rr = r.Offseted(GetTextRect().left, -sb);
	objectrect = GetObjectRect(pos);
	objectpos = cursor;
	PlaceCaret();
	Refresh(rr);
	ReadFormat();
}

void RichEdit::LeftDown(Point p, dword flags)
{
	useraction = true;
	NextUndo();
	SetFocus();
	selclick = false;
	tabmove = GetHotPos(p);
	if(tabmove.table && tabmove.column >= -2) {
		SaveTableFormat(tabmove.table);
		SetCapture();
		Move(text.GetCellPos(tabmove.table, 0, max(tabmove.column, 0)).pos);
		return;
	}
	i32 c = GetHotSpot(p);
	if(c >= 0 && objectpos >= 0) {
		i32 pos = objectpos;
		RectTracker tracker(*this);
		RichObj obj = text.GetRichPos(pos).object;
		tracker.MinSize(Size(16, 16))
		       .MaxSize(GetZoom() * pagesz)
		       .Animation()
		       .Dashed()
		       .KeepRatio(obj.IsKeepRatio());
		i32 tx, ty;
		switch(c) {
		case 1:
			tracker.SetCursorImage(Image::SizeVert());
			tx = ALIGN_CENTER; ty = ALIGN_BOTTOM;
			break;
		case 2:
			tracker.SetCursorImage(Image::SizeHorz());
			tx = ALIGN_RIGHT; ty = ALIGN_CENTER;
			break;
		default:
			tracker.SetCursorImage(Image::SizeBottomRight());
			tx = ALIGN_RIGHT; ty = ALIGN_RIGHT;
			break;
		}
		double zoom = GetZoom().AsDouble();
		Size sz = obj.GetSize();
		sz.cx = i32(zoom * sz.cx + 0.5);
		sz.cy = i32(zoom * sz.cy + 0.5);
		sz = tracker.Track(Rect(objectrect.Offseted(GetTextRect().left, -sb).TopLeft(), sz), tx, ty).Size();
		sz.cx = i32(sz.cx / zoom + 0.5);
		sz.cy = i32(sz.cy / zoom + 0.5);
		obj.SetSize(sz);
		ReplaceObject(obj);
	}
	else {
		c = GetMousePos(p);
		if(c >= 0) {
			if(InSelection(c)) {
				selclick = true;
				return;
			}
			Move(c, flags & K_SHIFT);
			mpos = c;
			SetCapture();
			if(cursorp.object && GetObjectRect(cursor).Offseted(GetTextRect().left, -sb).Contains(p))
				SetObjectPos(cursor);
		}
	}
}

void RichEdit::LeftUp(Point p, dword flags)
{
	useraction = true;
	NextUndo();
	i32 c = GetMousePos(p);
	i32 d = c;
	if(!HasCapture() && InSelection(d) && selclick) {
		CancelSelection();
		Move(c);
	}
	selclick = false;
}

void RichEdit::MouseMove(Point p, dword flags)
{
	useraction = true;
	if(HasCapture() && (flags & K_MOUSELEFT)) {
		if(tabmove.table && tabmove.column >= 0) {
			RichTable::Format fmt = text.GetTableFormat(tabmove.table);
			if(tabmove.column >= fmt.column.GetCount() - 1)
				return;
			i32 sum = Sum(fmt.column);
			i32 nl = 0;
			for(i32 i = 0; i < tabmove.column; i++)
				nl += fmt.column[i];
			i32 xn = fmt.column[tabmove.column] + fmt.column[tabmove.column + 1];
			i32 xl = tabmove.left + tabmove.cx * nl / sum + 12;
			i32 xh = tabmove.left + tabmove.cx * (nl + xn) / sum - 12;
			if(xl >= xh)
				return;
			i32 xx = minmax(GetSnapX(p.x) - tabmove.delta, xl, xh) - tabmove.left;
			fmt.column[tabmove.column] = xx * sum / tabmove.cx - nl;
			fmt.column[tabmove.column + 1] = xn - fmt.column[tabmove.column];
			text.SetTableFormat(tabmove.table, fmt);
			Finish();
		}
		else
		if(tabmove.table && tabmove.column == RICHHOT_LM) {
			RichTable::Format fmt = text.GetTableFormat(tabmove.table);
			fmt.rm = clamp(fmt.rm, 0, tabmove.textcx - fmt.lm - 120);
			fmt.lm = clamp(GetSnapX(p.x) - tabmove.textleft, 0, max(tabmove.textcx - fmt.rm - 120, 0));
			text.SetTableFormat(tabmove.table, fmt);
			Finish();
		}
		else
		if(tabmove.table && tabmove.column == RICHHOT_RM) {
			RichTable::Format fmt = text.GetTableFormat(tabmove.table);
			fmt.lm = clamp(fmt.lm, 0, max(tabmove.textcx - fmt.rm - 120, 0));
			fmt.rm = minmax(tabmove.textcx - GetSnapX(p.x) + tabmove.textleft, 0, tabmove.textcx - fmt.lm - 120);
			text.SetTableFormat(tabmove.table, fmt);
			Finish();
		}
		else {
			PageY py = GetPageY(p.y + sb);
			i32 c;
			if(py > text.GetHeight(pagesz))
				c = GetLength() + 1;
			else
				c = GetNearestPos(GetX(p.x), py);
			if(c != mpos) {
				mpos = -1;
				Move(c, true);
			}
		}
	}
}

static bool IsObjectPercent(Sizef percent, i32 p)
{
	return fabs(percent.cx - p) < 1 && fabs(percent.cy - p) < 1;
}

static bool IsObjectDelta(i32 delta, i32 d)
{
	return d * 25 / 3 == delta;
}

void RichEdit::StdBar(Bar& menu)
{
	i32 l, h;
	Id field;
	Txt fieldparam;
	Txt ofieldparam;
	RichObj object;
	if(GetSelection(l, h)) {
		CutTool(menu);
		CopyTool(menu);
		PasteTool(menu);
	}
	else {
		if(objectpos >= 0) {
			bar_object = GetObject();
			if(!bar_object) return;
			bar_object.Menu(menu, context);
			if(!menu.IsEmpty())
				menu.Separator();
			Size sz = GetPhysicalSize(bar_object);
			Sizef percent = bar_object.GetSize();
			percent = 100.0 * percent / Sizef(sz);
			bool b = sz.cx || sz.cy;
			menu.Add(t_("Позиция объекта.."), OTVET(AdjustObjectSize));
			menu.Separator();
			menu.Add(b, "20 %", OTVET1(SetObjectPercent, 20)).Check(IsObjectPercent(percent, 20));
			menu.Add(b, "30 %", OTVET1(SetObjectPercent, 30)).Check(IsObjectPercent(percent, 30));
			menu.Add(b, "40 %", OTVET1(SetObjectPercent, 40)).Check(IsObjectPercent(percent, 40));
			menu.Add(b, "50 %", OTVET1(SetObjectPercent, 50)).Check(IsObjectPercent(percent, 50));
			menu.Add(b, "60 %", OTVET1(SetObjectPercent, 60)).Check(IsObjectPercent(percent, 60));
			menu.Add(b, "70 %", OTVET1(SetObjectPercent, 70)).Check(IsObjectPercent(percent, 70));
			menu.Add(b, "80 %", OTVET1(SetObjectPercent, 80)).Check(IsObjectPercent(percent, 80));
			menu.Add(b, "90 %", OTVET1(SetObjectPercent, 90)).Check(IsObjectPercent(percent, 90));
			menu.Add(b, "100 %", OTVET1(SetObjectPercent, 100)).Check(IsObjectPercent(percent, 100));
			menu.Break();
			i32 delta = bar_object.GetYDelta();
			menu.Add(t_("3 pt выше"), OTVET1(SetObjectYDelta, -3)).Check(IsObjectDelta(delta, -3));
			menu.Add(t_("2 pt выше"), OTVET1(SetObjectYDelta, -2)).Check(IsObjectDelta(delta, -2));
			menu.Add(t_("1 pt выше"), OTVET1(SetObjectYDelta, -1)).Check(IsObjectDelta(delta, -1));
			menu.Add(t_("Базовая линия"), OTVET1(SetObjectYDelta, 0)).Check(IsObjectDelta(delta, 0));
			menu.Add(t_("1 pt ниже"), OTVET1(SetObjectYDelta, 1)).Check(IsObjectDelta(delta, 1));
			menu.Add(t_("2 pt ниже"), OTVET1(SetObjectYDelta, 2)).Check(IsObjectDelta(delta, 2));
			menu.Add(t_("3 pt ниже"), OTVET1(SetObjectYDelta, 3)).Check(IsObjectDelta(delta, 3));
			menu.Separator();
			CopyTool(menu);
			CutTool(menu);
		}
		else {
			RichPos p = cursorp;
			field = p.field;
			bar_fieldparam = p.fieldparam;
			RichPara::FieldType *ft = RichPara::fieldtype().Get(field, NULL);
			if(ft) {
				ft->Menu(menu, &bar_fieldparam);
				if(!menu.IsEmpty())
					menu.Separator();
				CopyTool(menu);
				CutTool(menu);
			}
			else {
				WTxt w = GetWordAtCursor();
				if(!w.IsEmpty() && !SpellWord(w, w.GetLength(),
				                              fixedlang ? fixedlang : formatinfo.language)) {
					if(true) {
						Vec<Txt> h = SpellerFindCloseWords(fixedlang ? fixedlang : formatinfo.language, w.ToTxt(), 10);
						for(Txt s : h)
							menu.Add(s, [=, this] {
								i32 pos, count;
								GetWordAtCursorPos(pos, count);
								if(count) {
									Remove(pos, count);
									WTxt h = s.ToWTxt();
									Insert(pos, AsRichText(h, formatinfo));
									Move(pos + h.GetCount());
									Finish();
								}
							});
					}
					menu.Add(t_("добавить в словарь пользователя"), OTVET(AddUserDict));
					menu.Separator();
				}
				PasteTool(menu);
				PastePlainTextTool(menu);
				ObjectTool(menu);
			}
		}
		LoadImageTool(menu);
	}
}

void RichEdit::RightDown(Point p, dword flags)
{
	useraction = true;
	NextUndo();
	MenuBar menu;
	i32 l, h;
	Rect ocr = GetCaretRect();
	i32 fieldpos = -1;
	Id field;
	Txt ofieldparam;
	RichObj o;
	bar_object.Clear();
	bar_fieldparam = Null;
	if(!GetSelection(l, h)) {
		LeftDown(p, flags);
		if(objectpos >= 0)
			o = bar_object = GetObject();
		else {
			RichPos p = cursorp;
			field = p.field;
			bar_fieldparam = p.fieldparam;
			RichPara::FieldType *ft = RichPara::fieldtype().Get(field, NULL);
			if(ft) {
				ofieldparam = bar_fieldparam;
				fieldpos = cursor;
			}
		}
	}
	WhenBar(menu);
	Rect r = GetCaretRect();
	Refresh(r);
	Refresh(ocr);
	paintcarect = true;
	menu.Execute();
	paintcarect = false;
	Refresh(r);
	if(bar_object && o && o.GetSerialId() != bar_object.GetSerialId())
		ReplaceObject(bar_object);
	if(fieldpos >= 0 && bar_fieldparam != ofieldparam) {
		RichText::FormatInfo f = text.GetFormatInfo(fieldpos, 1);
		Remove(fieldpos, 1);
		RichPara p;
		p.Cat(field, bar_fieldparam, f);
		RichText clip;
		clip.Cat(p);
		Insert(fieldpos, clip, false);
		Finish();
	}
	bar_object.Clear();
	bar_fieldparam = Null;
}

void RichEdit::LeftDouble(Point p, dword flags)
{
	NextUndo();
	i32 c = GetMousePos(p);
	if(c >= 0) {
		if(objectpos == c) {
			RichObj object = GetObject();
			if(!object) return;
			RichObj o = object;
			o.DefaultAction(context);
			if(object.GetSerialId() != o.GetSerialId())
				ReplaceObject(o);
		}
		else {
			RichPos rp = cursorp;
			RichPara::FieldType *ft = RichPara::fieldtype().Get(rp.field, NULL);
			if(ft) {
				i32 fieldpos = cursor;
				ft->DefaultAction(&rp.fieldparam);
				RichText::FormatInfo f = text.GetFormatInfo(fieldpos, 1);
				Remove(fieldpos, 1);
				RichPara p;
				p.Cat(rp.field, rp.fieldparam, f);
				RichText clip;
				clip.Cat(p);
				Insert(fieldpos, clip, false);
				Finish();
			}
			else {
				z64 l, h;
				if(GetWordSelection(c, l, h))
					SetSelection((i32)l, (i32)h);
			}
		}
	}
}

void RichEdit::LeftTriple(Point p, dword flags)
{
	NextUndo();
	i32 c = GetMousePos(p);
	if(c >= 0 && c != objectpos) {
		RichPos rp = text.GetRichPos(c);
		Select(c - rp.posinpara, rp.paralen + 1);
	}
}

Image RichEdit::CursorImage(Point p, dword flags)
{
	if(tablesel)
		return Image::Arrow();

	switch(GetHotSpot(p)) {
	case 0:
		return Image::SizeBottomRight();
	case 1:
		return Image::SizeVert();
	case 2:
		return Image::SizeHorz();
	default:
		if(text.GetRichPos(GetMousePos(p)).object) {
			return Image::Arrow();
		}
		else
		if(HasCapture() && tabmove.table && tabmove.column >= -2)
			return Image::SizeHorz();
		else {
			RichHotPos hp = GetHotPos(p);
			if(hp.table > 0)
				return Image::SizeHorz();
			else {
				i32 c = GetMousePos(p);
				return InSelection(c) && !HasCapture() ? Image::Arrow() : Image::IBeam();
			}
		}
	}
	return Image::Arrow();
}

void RichEdit::LeftRepeat(Point p, dword flags)
{
	NextUndo();
	if(HasCapture() && (flags & K_MOUSELEFT)) {
		if(p.y < 0)
			MoveUpDown(-1, true);
		if(p.y > GetSize().cy)
			MoveUpDown(1, true);
	}
}

}