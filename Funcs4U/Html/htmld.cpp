// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <drx/Core/Core.h>
#include "htmld.h"


namespace drx {

Txt ToHtmlD(tukk s)
{
	if(*s == '`')
		return Txt(++s);

	Txt result;
	while(*s) {
		if(*s == 31)
			result.Cat("&nbsp;");
		else
		if(*s == '<')
			result.Cat("&lt;");
		else
		if(*s == '>')
			result.Cat("&gt;");
		else
		if(*s == '&')
			result.Cat("&amp;");
		else
		if(*s == '\"')
			result.Cat("&quot;");
		else
		if((u8)*s < ' ')
			result.Cat(NFormat("&#%d;", (u8)*s));
		else
			result.Cat(*s);
		s++;
	}
	return result;
}

/*
HtmlsD& HtmlsD::Qtf(tukk qtf) {
	HtmlDocOut html;
	html.Qtf(qtf);
	Cat(html.Get());
	return *this;
}
*/

HtmlsD& HtmlsD::Text(tukk s)                  { Cat(ToHtmlD(s)); return *this; }
HtmlsD& HtmlsD::Quote(tukk s)                 { Cat('\"' + ToHtmlD(s) + '\"'); return *this; }
HtmlsD& HtmlsD::Color(class Color color)             { Quote(ColorToHtml(color)); return *this; }
HtmlsD& HtmlsD::Percent(double d)                    { return Quote(DRX::Format("%g%%", d)); }

HtmlsD& HtmlsD::Color(tukk nm, class Color c) {
	if(!DRX::IsNull(c))
		Attr(nm);
	Cat('=');
	return Color(c);
}

HtmlsD& HtmlsD::Percent(tukk nm, double d)    { Attr(nm); Cat('='); return Percent(d); }

HtmlsD& HtmlsD::Attr(tukk a)                  { Cat(' '); Cat(a); return *this; }
HtmlsD& HtmlsD::Attr(tukk a, tukk s)   { Attr(a); Cat('='); return Quote(s); }
HtmlsD& HtmlsD::Attr(tukk a, Txt s)        { return Attr(a, ~s); }
HtmlsD& HtmlsD::Attr(tukk a, i32 i)           { return Attr(a, DRX::Format("%d", i)); }
HtmlsD& HtmlsD::Attr(tukk a, double d)        { return Attr(a, DRX::Format("%g", d)); }
HtmlsD& HtmlsD::Attr(tukk a, Date d)          { return Attr(a, DRX::Format(d)); }
HtmlsD& HtmlsD::Attr(tukk a, Val v)         { return Attr(a, StdFormat(v)); }

HtmlsD& HtmlsD::Attp(tukk a, double v)        { return v >= 0 ? Attr(a, v) : Percent(a, -v); }

HtmlsD& HtmlsD::Atth(tukk a, tukk h)
{
	Attr(a);
	Cat('=');
	Cat('\"');
	Cat(h);
	Cat('\"');
	return *this;
}

HtmlsD& HtmlsD::Tag(tukk tag)                 { Cat('<'); Cat(tag); return *this; }
HtmlsD& HtmlsD::Gat()                                { Cat('>'); return *this; }

HtmlsD& HtmlsD::Nbsp()                               { Cat("&nbsp;"); return *this; }
HtmlsD& HtmlsD::Br()                                 { Cat("<BR>", 4); return *this; }
HtmlsD& HtmlsD::Hr()                                 { Cat("<HR>", 4); return *this; }
HtmlsD& HtmlsD::Lf()                                 { Cat('\n'); return *this; }

HtmlsD& HtmlsD::Cat(const HtmlTagD& tag)
{
	Cat(tag.GetTag());
	Cat(tag.GetEndTag());
	return *this;
}

HtmlTagD& HtmlTagD::Type(tukk s)       { return Attr("TYPE", s); }

HtmlTagD& HtmlTagD::Name(tukk s)       { return Attr("NAME", s); }
HtmlTagD& HtmlTagD::Name(Txt id)           { return Attr("NAME", ~id); }

HtmlTagD& HtmlTagD::Data(tukk v)       { return Attr("VALUE", v); }
HtmlTagD& HtmlTagD::Data(Txt v)            { return Attr("VALUE", v); }
HtmlTagD& HtmlTagD::Data(double v)            { return Attr("VALUE", v); }
HtmlTagD& HtmlTagD::Data(Val v)             { return Attr("VALUE", v); }

HtmlTagD& HtmlTagD::ReadOnly()                { return Attr("READONLY"); }
HtmlTagD& HtmlTagD::Multiple()                { return Attr("MULTIPLE"); }
HtmlTagD& HtmlTagD::Selected()                { return Attr("SELECTED"); }

HtmlTagD& HtmlTagD::Align(Alignment align)
{
	switch(align) {
		case ALIGN_CENTER: return Attr("ALIGN", "CENTER");
		case ALIGN_RIGHT:  return Attr("ALIGN", "RIGHT");
		default:           return Attr("ALIGN", "LEFT");
	}
}

HtmlTagD& HtmlTagD::Left()                    { return Align(ALIGN_LEFT); }
HtmlTagD& HtmlTagD::Right()                   { return Align(ALIGN_RIGHT); }
HtmlTagD& HtmlTagD::Center()                  { return Align(ALIGN_CENTER); }

HtmlTagD& HtmlTagD::VAlign(Alignment align)
{
	switch(align) {
		case ALIGN_CENTER: return Attr("VALIGN", "MIDDLE");
		case ALIGN_BOTTOM: return Attr("VALIGN", "BOTTOM");
		default:           return Attr("VALIGN", "TOP");
	}
}

HtmlTagD& HtmlTagD::Top()                     { return VAlign(ALIGN_TOP); }
HtmlTagD& HtmlTagD::VCenter()                 { return VAlign(ALIGN_CENTER); }
HtmlTagD& HtmlTagD::Bottom()                  { return VAlign(ALIGN_BOTTOM); }

HtmlTagD& HtmlTagD::HotKey(char c)            { return c ? Attr("ACCESSKEY", Txt(c, 1)) : *this; }
HtmlTagD& HtmlTagD::Size(i32 c)               { return Attr("SIZE", c); }
HtmlTagD& HtmlTagD::Size(i32 cx, i32 cy)      { return Attr("SIZE", Format("%d, %d", cx, cy)); }

HtmlTagD& HtmlTagD::Color(class Color color)      { return Color("COLOR", color); }
HtmlTagD& HtmlTagD::BorderColor(class Color col)  { return Color("BORDERCOLOR", col); }
HtmlTagD& HtmlTagD::BgColor(class Color color)    { return Color("BGCOLOR", color); }

HtmlTagD& HtmlTagD::Face(tukk s)       { return Attr("FACE", s); }
HtmlTagD& HtmlTagD::Border(i32 i)             { return Attr("BORDER", i); }
HtmlTagD& HtmlTagD::CellSpacing(i32 i)        { return Attr("CELLSPACING", i); }
HtmlTagD& HtmlTagD::CellPadding(i32 i)        { return Attr("CELLPADDING", i); }

HtmlTagD& HtmlTagD::Width(double w)           { return Attp("WIDTH", w); }
HtmlTagD& HtmlTagD::Height(double h)          { return Attp("HEIGHT", h); }

HtmlTagD& HtmlTagD::RowSpan(i32 i)            { return Attr("ROWSPAN", i); }
HtmlTagD& HtmlTagD::ColSpan(i32 i)            { return Attr("COLSPAN", i); }

HtmlTagD& HtmlTagD::Class(tukk s)      { return Attr("CLASS", s); }
HtmlTagD& HtmlTagD::DataFld(tukk s)    { return Attr("DATAFLD", s); }
HtmlTagD& HtmlTagD::DataSrc(tukk s)    { return Attr("DATASRC", s); }
HtmlTagD& HtmlTagD::Href(tukk s)       { return Atth("HREF", s); }
HtmlTagD& HtmlTagD::HtmlId(tukk s)     { return Attr("ID", s); }
HtmlTagD& HtmlTagD::Lang(tukk s)       { return Attr("LANG", s); }
HtmlTagD& HtmlTagD::Language(tukk s)   { return Attr("LANGUAGE", s); }
HtmlTagD& HtmlTagD::JavaScript()              { return Language("JAVASCRIPT"); }
HtmlTagD& HtmlTagD::VbScript()                { return Language("VBSCRIPT"); }
HtmlTagD& HtmlTagD::Methods(tukk s)    { return Attr("METHODS", s); }
HtmlTagD& HtmlTagD::Rel(tukk s)        { return Attr("REL", s); }
HtmlTagD& HtmlTagD::Rev(tukk s)        { return Attr("REV", s); }
HtmlTagD& HtmlTagD::Style(tukk s)      { return Attr("STYLE", s); }
HtmlTagD& HtmlTagD::TabIndex(i32 i)           { return Attr("TABINDEX", i); }
HtmlTagD& HtmlTagD::Target(tukk s)     { return Attr("TARGET", s); }
HtmlTagD& HtmlTagD::TargetBlank()             { return Target("_blank"); }
HtmlTagD& HtmlTagD::TargetParent()            { return Target("_parent"); }
HtmlTagD& HtmlTagD::TargetSelf()              { return Target("_self"); }
HtmlTagD& HtmlTagD::TargetTop()               { return Target("_top"); }
HtmlTagD& HtmlTagD::Title(tukk s)      { return Attr("TITLE", s); }
HtmlTagD& HtmlTagD::Urn(tukk s)        { return Attr("URN", s); }
HtmlTagD& HtmlTagD::Alt(tukk s)        { return Attr("ALT", s); }
HtmlTagD& HtmlTagD::Coords(tukk s)     { return Attr("COORDS", s); }
HtmlTagD& HtmlTagD::NoHref()                  { return Attr("NOHREF"); }
HtmlTagD& HtmlTagD::Shape(tukk s)      { return Attr("SHAPE", s); }
HtmlTagD& HtmlTagD::ShapeCirc()               { return Shape("CIRC"); }
HtmlTagD& HtmlTagD::ShapePoly()               { return Shape("POLY"); }
HtmlTagD& HtmlTagD::ShapeRect()               { return Shape("RECT"); }
HtmlTagD& HtmlTagD::Balance(double v)         { return Attr("BALANCE", v); }
HtmlTagD& HtmlTagD::Loop(i32 i)               { return Attr("LOOP", i); }
HtmlTagD& HtmlTagD::Volume(double v)          { return Attr("VOLUME", v); }
HtmlTagD& HtmlTagD::Src(tukk s)        { return Attr("SRC", s); }
HtmlTagD& HtmlTagD::Alink(class Color c)      { return Color("ALINK", c); }
HtmlTagD& HtmlTagD::Vlink(class Color c)      { return Color("VLINK", c); }
HtmlTagD& HtmlTagD::Link(class Color c)       { return Color("LINK", c); }
HtmlTagD& HtmlTagD::Background(tukk s) { return Attr("BACKGROUND", s); }
HtmlTagD& HtmlTagD::OnClick(tukk s)    { return Atth("onClick", s); }

HtmlsD HtmlTagD::GetTag() const {
	return IsNull(tag) ? ~tag : ~tag + ">";
}

HtmlsD HtmlTagD::GetEndTag() const {
	return end;
}

void  HtmlTagD::Combine(const HtmlTagD& tag2)
{
	if(tag2.IsEmpty()) return;
	if(!IsNull(tag)) {
		tag = tag + ">" + tag2.tag;
		end = tag2.end + end;
	}
	else {
		tag = tag2.tag;
		end = tag2.end;
	}
}

HtmlsD HtmlTagD::ApplyTo(Txt s) const
{
	HtmlsD h;
	h.Cat(GetTag());
	h.Cat(s);
	h.Cat(GetEndTag());
	return h;
}

HtmlsD HtmlTagD::ApplyTo(tukk s) const
{
	HtmlsD h;
	h.Cat(GetTag());
	h.Cat(s);
	h.Cat(GetEndTag());
	return h;
}

HtmlTagD& HtmlTagD::SingleTag(tukk s)
{
	tag = "<";
	tag.Cat(s);
	return *this;
}

HtmlTagD& HtmlTagD::PairTag(tukk s)
{
	SingleTag(s);
	end = "</";
	end.Cat(s);
	end.Cat('>');
	end.Cat('\n');
	return *this;
}

HtmlTagD::HtmlTagD(tukk tag) {
	PairTag(tag);
}

HtmlTagD HtmlSingleTagD(tukk tag) {
	return HtmlTagD().SingleTag(tag);
}

HtmlTagD HtmlInputD(tukk type, tukk name)
{
	HtmlTagD tag = HtmlSingleTagD("INPUT").Type(type);
	if(name && *name)
		tag.Name(name);
	return tag;
}

HtmlTagD HtmlEditD(Txt name)
{
	return HtmlInputD("TEXT", name);
}

HtmlTagD HtmlEditD(Txt name, i32 size, i32 maxlength, tukk defaultVal, bool password)
{
	HtmlTagD tag = HtmlInputD(password ? "PASSWORD" : "TEXT", name)
		.Size(size).Attr("MAXLENGTH", maxlength);
	if(defaultVal && *defaultVal)
		tag.Data(defaultVal);
	return tag;
}

HtmlTagD HtmlHiddenD(Txt name, Val val) {
	return HtmlInputD("HIDDEN").Name(name).Data(val);
}

HtmlTagD HtmlHiddenD(tukk name, Val val) {
	return HtmlInputD("HIDDEN").Name(name).Data(val);
}

HtmlTagD HtmlHiddenD(Val val) {
	return HtmlHiddenD("STATE", val);
}

HtmlTagD HtmlSubmitD(tukk s) {
	return HtmlInputD("SUBMIT").Data(s);
}

HtmlTagD HtmlSelectD(Txt name) {
	return HtmlTagD("SELECT").Name(name);
}

HtmlsD HtmlOptionD(Val val, tukk text, bool selected)
{
	HtmlTagD option("OPTION");
	option.Data(val);
	if(selected)
		option.Selected();
	return option / text;
}

HtmlTagD HtmlTableD() {
	return HtmlTagD("TABLE");
}

HtmlTagD HtmlRowD() {
	return HtmlTagD("TR");
}

HtmlTagD HtmlCellD() {
	return HtmlTagD("TD");
}

/*
HtmlTagD HtmlFont(Font font) {
	HtmlTagD tag;
	if(font.IsBold())
		tag /= HtmlTagD("B");
	if(font.IsItalic())
		tag /= HtmlTagD("I");
	if(font.IsUnderline())
		tag /= HtmlTagD("U");
	if(font.IsStrikeout())
		tag /= HtmlTagD("S");
	HtmlTagD fnt("FONT");
	fnt.Face(FontFaceToHtml(font));
	fnt.Size(FontSizeToHtml(font));
	return tag / fnt;
}
*/

HtmlTagD HtmlLinkD(tukk link)
{
	return HtmlTagD("A").Href(link);
}

HtmlsD HtmlBlockD(HtmlsD html, double width, double left, double top, double right, double bottom,
				Alignment align)
{
	HtmlsD h;
	if(top)
		h = HtmlRowD() / HtmlCellD().Height(top) / "";
	if(left || right || align != ALIGN_LEFT)
		h << HtmlRowD() / (
				HtmlCellD().Width(left) +
				HtmlCellD().Align(align).Width(width) / html +
				HtmlCellD().Width(right)
			 );
	if(bottom)
		h << HtmlRowD() / HtmlCellD().Height(bottom) / "";
	return !IsNull(h) ? HtmlTableD().Border(0).Width(-100).CellSpacing(0).CellPadding(0) / h
		              : html;
}

HtmlTagD HtmlHeaderD(tukk title, Txt css, tukk other)
{
	Txt h =
		"HTML>\r\n"
	    "<HEAD>\t\n"
	    "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf-8\">\t\n"
	    "<META NAME=\"Generator\" CONTENT=\"U++ HTML Package\">\t\n"
	    "<TITLE>" + Txt(title) + "</TITLE>\r\n"
	;
	if(!IsNull(css))
		h << "<STYLE TYPE=\"text/css\"><!--\r\n"
		  << css << "\r\n-->\r\n</STYLE>\r\n";
	if(other)
		h << other;
	h << "</HEAD";
	return HtmlSingleTagD(h) / HtmlTagD("BODY");
}

HtmlsD operator+(const HtmlsD& a, const HtmlsD& b)
{
	return ~a + ~b;
}

HtmlsD operator+(const HtmlsD& s, const HtmlTagD& tag)
{
	HtmlsD h;
	h.Cat(s);
	h.Cat(tag);
	return h;
}

HtmlsD operator+(const HtmlTagD& tag, const HtmlsD& s)
{
	HtmlsD h;
	h.Cat(tag);
	h.Cat(s);
	return h;
}

HtmlsD operator+(const HtmlsD& a, tukk b)
{
	HtmlsD h;
	h.Cat(a);
	h.Cat(b);
	return h;
}

HtmlsD operator+(tukk a, const HtmlsD& b)
{
	HtmlsD h;
	h.Cat(a);
	h.Cat(b);
	return h;
}

HtmlsD operator+(const HtmlTagD& tag1, const HtmlTagD& tag2)
{
	HtmlsD h;
	h.Cat(tag1);
	h.Cat(tag2);
	return h;
}

HtmlsD operator+(const Txt& s, const HtmlTagD& tag)
{
	HtmlsD h;
	h.Cat(s);
	h.Cat(tag);
	return h;
}

HtmlsD operator+(const HtmlTagD& tag, const Txt& s)
{
	HtmlsD h;
	h.Cat(tag);
	h.Cat(s);
	return h;
}

HtmlsD operator+(tukk s, const HtmlTagD& tag) {
	HtmlsD h;
	h.Cat(s);
	h.Cat(tag);
	return h;
}

HtmlsD operator+(const HtmlTagD& tag, tukk s)
{
	HtmlsD h;
	h.Cat(tag);
	h.Cat(s);
	return h;
}

HtmlTagD operator/(const HtmlTagD& t1, const HtmlTagD& t2)
{
	HtmlTagD t = t1;
	t.Combine(t2);
	return t;
}

HtmlTagD Div(const HtmlTagD& t1, const HtmlTagD& t2)
{
	HtmlTagD t = t1;
	t.Combine(t2);
	return t;
}

HtmlTagD& operator/=(HtmlTagD& tag, const HtmlTagD& s)
{
	tag.Combine(s);
	return tag;
}

HtmlTagD operator % (const HtmlTagD& t1, const HtmlTagD& t2)
{
	if(IsNull(t1.Tag()))
		return t2;
	else if(IsNull(t2.Tag()))
		return t1;
	else
	{
		HtmlTagD t;
		t.Tag() = t1.Tag() + ">\n" + t2.Tag();
		t.End() = t2.End() + "\n" + t1.End();
		return t;
	}
}

HtmlsD operator % (const HtmlTagD& tag, tukk s)
{
	HtmlsD h;
	h.Cat(tag.GetTag());
	h.Lf();
	h.Cat(s);
	h.Lf();
	h.Cat(tag.GetEndTag());
//	h.Lf();
	return h;
}

HtmlTagD HtmlFontColorD(Color c)   { return !IsNull(c) ? HtmlTagD("FONT").Color(c) : HtmlTagD(); }
HtmlTagD HtmlFontSizeD(i32 s)      { return HtmlTagD("FONT").Attr("SIZE", s); }

HtmlTagD HtmlLinkD(tukk link, tukk target)
{
	HtmlTagD tag("A");
	tag.Href(link);
	if(target && *target)
		tag.Target(target);
	return tag;
}

HtmlTagD HtmlImgD(Txt src, Txt alt)
{
	return HtmlSingleTagD("IMG").Src(src).Alt(alt);
}

HtmlsD HtmlTextAreaD(Txt name, Size size, tukk defaultVal)
{
	return HtmlTagD("TEXTAREA").Name(name).Attr("ROWS", size.cy).Attr("COLS", size.cx)
		.Attr("WRAP", "virtual")
		/ ToHtmlD(defaultVal ? defaultVal : "");
}

HtmlTagD HtmlButtonD(Txt name, tukk text)
{
	return HtmlInputD("SUBMIT", name).Data(text);
}

HtmlTagD HtmlWarnD(Color color)
{
	return HtmlItalicD() / HtmlFontColorD(color);
}

HtmlTagD HtmlPackedTableD()
{
	return HtmlTableD().Border(0).CellSpacing(0).CellPadding(0);
}

HtmlTagD HtmlFormD(Txt action, bool multipart, Txt method)
{
	HtmlTagD tag = HtmlTagD("FORM").Attr("ACTION", action).Attr("METHOD", method);
	if(multipart)
		tag.Attr("ENCTYPE", "multipart/form-data");
	return tag;
}

HtmlsD HtmlUploadD(Txt name)
{
	return HtmlInputD("FILE", name);
}

HtmlsD HtmlCheckD(Txt name, bool on)
{
	HtmlTagD tag = HtmlInputD("CHECKBOX", name).Data("1");
	if(on)
		tag.Attr("CHECKED");
	return tag;
}

HtmlTagD HtmlTCellD()
{
	return HtmlCellD().Top();
}

HtmlTagD HtmlRowCellD()
{
	return HtmlRowD() % HtmlCellD();
}

HtmlTagD HtmlRowTCellD()
{
	return HtmlRowD() % HtmlTCellD();
}

HtmlsD HtmlMenuD(bool active, tukk text, tukk url, i32 wd)
{
	HtmlTagD table = HtmlTableD().CellSpacing(0).Border(2)
		.BorderColor(Black).BgColor(active ? Yellow : WhiteGray);
	if(wd)
		table.Width(wd);
	return table % HtmlRowD() % HtmlCellD().Center()
		/ HtmlLinkD(url) / HtmlFontColorD(Black) / (active ? HtmlBoldD() / text : HtmlsD(text));
}

HtmlTagD HtmlDialogD(i32 width)
{
	return HtmlTableD().Width(width).CellSpacing(0).CellPadding(1).Border(1).BgColor(WhiteGray)
		% HtmlRowD() % HtmlCellD()
		% HtmlTableD().Width(-100).CellSpacing(4).CellPadding(0).Border(0).BgColor(WhiteGray);
}

HtmlsD HtmlHeadD(Txt title, tukk charset)
{
	HtmlsD head;
	head << HtmlTagD("TITLE") / title << "\n"
		 << HtmlSingleTagD("META")
			.Attr("HTTP-EQUIV", "Content-Type")
			.Attr("CONTENT", Txt("text/html; charset=") + charset);
	return HtmlTagD("HEAD") % ~head + "\n\n";
}

HtmlsD HtmlPageD(Txt title, const HtmlsD& body, Color bgcolor, tukk charset)
{
	return HtmlTagD("HTML") % ~(HtmlHeadD(title, charset) + HtmlTagD("BODY").BgColor(bgcolor) % ~body);
}

HtmlsD HtmlTitlePageD(Txt title, const HtmlsD& body, Color bgcolor, tukk charset)
{
	return HtmlPageD(title, HtmlTagD("H1") / title + "\n\n" + body, bgcolor);
}

HtmlTagD HtmlLineD()
{
	return HtmlRowD() / HtmlCellD();
}

HtmlTagD HtmlParaD()
{
	return HtmlTagD("p");
}

HtmlTagD HtmlCourierD(i32 size)
{
	return HtmlTagD("span")
	       .Attr("style", Sprintf("font-size: %dpt; font-family: monospace;", size));
}

HtmlTagD HtmlArialD(i32 size)
{
	return HtmlTagD("span")
	       .Attr("style", Sprintf("font-size: %dpt; font-family: sans-serif;", size));
}

HtmlTagD HtmlRomanD(i32 size)
{
	return HtmlTagD("span")
	       .Attr("style", Sprintf("font-size: %dpt; font-family: serif;", size));
}

HtmlTagD HtmlCourierD()
{
	return HtmlTagD("span").Attr("style", "font-family: monospace;");
}

HtmlTagD HtmlArialD()
{
	return HtmlTagD("span").Attr("style", "font-family: sans-serif;");
}

HtmlTagD HtmlRomanD()
{
	return HtmlTagD("span").Attr("style", "font-family: serif;");
}

HtmlTagD HtmlFntSizeD(i32 size)
{
	return HtmlTagD("span").Attr("style", Sprintf("font-size: %dpt", size));
}

}