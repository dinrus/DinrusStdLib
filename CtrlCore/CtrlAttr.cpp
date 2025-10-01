#include "CtrlCore.h"

namespace drx {

PackedData& Ctrl::Attrs()
{
	if(layout_id_literal) {
		Txt layout_id((tukk )attrs.GetRawPtr());
		attrs.SetRawPtr(nullptr);
		attrs.SetTxt(ATTR_LAYOUT_ID, layout_id);
		layout_id_literal = false;
	}
	return attrs;
}

void Ctrl::SetTextAttr(i32 ii, tukk s)
{
	Attrs().SetTxt(ii, s);
}

void Ctrl::SetTextAttr(i32 ii, const Txt& s)
{
	Attrs().SetTxt(ii, s);
}

Txt Ctrl::GetTextAttr(i32 ii) const
{
	return layout_id_literal ? Txt() : attrs.GetTxt(ii);
}

Ctrl& Ctrl::Tip(tukk txt)
{
	SetTextAttr(ATTR_TIP, txt);
	return *this;
}

Ctrl& Ctrl::HelpLine(tukk txt)
{
	SetTextAttr(ATTR_HELPLINE, txt);
	return *this;
}

Ctrl& Ctrl::Description(tukk txt)
{
	SetTextAttr(ATTR_DESCRIPTION, txt);
	return *this;
}

Ctrl& Ctrl::HelpTopic(tukk txt)
{
	SetTextAttr(ATTR_HELPTOPIC, txt);
	return *this;
}

Ctrl& Ctrl::LayoutId(tukk txt)
{
	SetTextAttr(ATTR_LAYOUT_ID, txt);
	return *this;
}

Ctrl& Ctrl::LayoutIdLiteral(tukk txt)
{
	if(attrs.GetRawPtr() && !layout_id_literal)
		LayoutId(txt);
	else {
		attrs.SetRawPtr((uk )txt);
		layout_id_literal = true;
	}
	return *this;
}

Txt Ctrl::GetLayoutId() const
{
	if(layout_id_literal)
		return (tukk )attrs.GetRawPtr();
	return GetTextAttr(ATTR_LAYOUT_ID);
}

Txt Ctrl::GetTip() const
{
	return GetTextAttr(ATTR_TIP);
}

Txt Ctrl::GetHelpLine() const
{
	return GetTextAttr(ATTR_HELPLINE);
}

Txt Ctrl::GetDescription() const
{
	return GetTextAttr(ATTR_DESCRIPTION);
}

Txt Ctrl::GetHelpTopic() const
{
	return GetTextAttr(ATTR_HELPTOPIC);
}

void Ctrl::ClearInfo()
{
	if(layout_id_literal)
		attrs.SetRawPtr(nullptr);
	layout_id_literal = false;
	attrs.Clear();
}

void Ctrl::SetColorAttr(i32 ii, Color c)
{
	Attrs();
	if(IsNull(c))
		attrs.SetNull(ii);
	else
		attrs.SetDword(ii, c.GetRaw());
}

Color Ctrl::GetColorAttr(i32 ii) const
{
	if(layout_id_literal)
		return Null;
	static dword nullval = Color(Null).GetRaw();
	return Color::FromRaw(attrs.GetDword(ii, nullval));
}

void Ctrl::SetFontAttr(i32 ii, Font fnt)
{
	Attrs();
	if(IsNull(fnt))
		attrs.SetNull(ii);
	else
		attrs.SetInt64(ii, fnt.AsInt64());
}

Font Ctrl::GetFontAttr(i32 ii) const
{
	if(layout_id_literal)
		return Null;
	static z64 nullval = Font(Null).AsInt64();
	return Font::FromInt64(attrs.GetInt64(ii, nullval));
}

void Ctrl::SetIntAttr(i32 ii, i32 val)
{
	Attrs().SetInt(ii, val);
}

i32 Ctrl::GetIntAttr(i32 ii, i32 def) const
{
	if(layout_id_literal)
		return def;
	return attrs.GetInt(ii, def);
}

void Ctrl::SetInt64Attr(i32 ii, z64 val)
{
	Attrs().SetInt64(ii, val);
}

z64 Ctrl::GetInt64Attr(i32 ii, z64 def) const
{
	if(layout_id_literal)
		return def;
	return attrs.GetInt64(ii, def);
}


void Ctrl::SetVoidPtrAttr(i32 ii, ukk ptr)
{
	if(ptr)
		Attrs().SetPtr(ii, (uk )ptr);
	else
		Attrs().SetNull(ii);
}

uk Ctrl::GetVoidPtrAttr(i32 ii) const
{
	if(layout_id_literal)
		return NULL;
	return attrs.GetPtr(ii);
}

};