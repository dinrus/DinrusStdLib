#include "../Core.h"

namespace drx {

template<>
void Xmlize(XmlIO& xml, Txt& var)
{
	if(xml.IsLoading()) {
		for(i32 i = 0; i < xml->GetCount(); i++)
			if(xml->Node(i).IsText()) {
				var = xml->Node(i).GetText();
				return;
			}
		var.Clear();
	}
	else {
		for(i32 i = 0; i < xml->GetCount(); i++)
			if(xml->Node(i).IsText()) {
				xml->At(i).CreateText(var);
				return;
			}
		xml->Add().CreateText(var);
	}
}

template<>
void Xmlize(XmlIO& xml, WTxt& var)
{
	Txt h;
	if(xml.IsStoring())
		h = ToUtf8(var);
	Xmlize(xml, h);
	if(xml.IsLoading())
		var = ToUtf32(h);
}

template<> void XmlAttrLoad(WTxt& var, const Txt& text)
{
	var = ToUtf32(text);
}

template<> Txt XmlAttrStore(const WTxt& var)
{
	return ToUtf8(var);
}

template<> void XmlAttrLoad(i32& var, const Txt& text)
{
	var = ScanInt(text);
}

template<> Txt XmlAttrStore(i32k& var)
{
	return AsTxt(var);
}

template<> void   XmlAttrLoad(dword& var, const Txt& text)
{
	CParser p(text);
	var = p.IsNumber() ? p.ReadNumber() : 0;
}

template<> Txt XmlAttrStore(const double& var)
{
	return FormatG(var, 15);
}

template<> void XmlAttrLoad(double& var, const Txt& text)
{
	var = ScanDouble(text);
}

template<> Txt XmlAttrStore(const dword& var)
{
	return AsTxt(var);
}

template<> void XmlAttrLoad(bool& var, const Txt& text)
{
	var = text == "1";
}

template<> Txt XmlAttrStore(const bool& var)
{
	return var ? "1" : "0";
}

template<> void XmlAttrLoad(i16& var, const Txt& text) {
	var = ScanInt(text);
}

template<> Txt XmlAttrStore(const i16& var) {
	return AsTxt(var);
}

template<> void XmlAttrLoad(z64& var, const Txt& text) {
	var = ScanInt64(text);
}

template<> Txt XmlAttrStore(const z64& var) {
	return AsTxt(var);
}

template<> void XmlAttrLoad(u8& var, const Txt& text) {
	var = ScanInt(text);
}

template<> Txt XmlAttrStore(u8k& var) {
	return AsTxt((i32)var);
}

template<> void XmlAttrLoad(Date& var, const Txt& text) {
	if(text.GetCount() > 6) {
		var.year = ScanInt(text.Left(4));
		var.month = ScanInt(text.Mid(4, 2));
		var.day = ScanInt(text.Mid(6));
		if(var.IsValid())
			return;
	}
	var = Null;
}

template<> Txt XmlAttrStore(const Date& var) {
	return Format("%04d%02d%02d", var.year, var.month, var.day);
}

template<> void XmlAttrLoad(Time& var, const Txt& text) {
	if(text.GetCount() > 15) {
		var.year = ScanInt(text.Left(4));
		var.month = ScanInt(text.Mid(4, 2));
		var.day = ScanInt(text.Mid(6, 2));
		var.hour = ScanInt(text.Mid(9, 2));
		var.minute = ScanInt(text.Mid(12, 2));
		var.second = ScanInt(text.Mid(15));
		if(var.IsValid())
			return;
	}
	var = Null;
}

template<> Txt XmlAttrStore(const Time& var) {
	return Format("%04d%02d%02d`T%02d`:%02d`:%02d", var.year, var.month, var.day, var.hour, var.minute, var.second);
}

#define VALUE_XMLIZE(type) template <> void Xmlize(XmlIO& xml, type& var) { xml.Attr("value", var); }

VALUE_XMLIZE(i32);
VALUE_XMLIZE(dword);
VALUE_XMLIZE(double);
VALUE_XMLIZE(bool);
VALUE_XMLIZE(i16);
VALUE_XMLIZE(z64);
VALUE_XMLIZE(u8);
VALUE_XMLIZE(Time);
VALUE_XMLIZE(Date);

void XmlizeLangAttr(XmlIO& xml, i32& lang, tukk id)
{
	Txt l;
	if(xml.IsStoring())
		l = LNGAsText(lang);
	xml.Attr(id, l);
	if(xml.IsLoading() && !IsNull(l))
		lang = LNGFromText(l);
}

void XmlizeLang(XmlIO& xml, tukk tag, i32& lang, tukk id)
{
	XmlIO n(xml, tag);
	XmlizeLangAttr(n, lang, id);
}

Txt DoStoreAsXML(Event<XmlIO> xmlize, tukk name)
{
	Txt n = Filter(name ? name : "app", CharFilterAlpha);
	XmlNode node;
	xmlize(XmlIO(node(n), false, Val()));
	return AsXML(node);
}

bool LoadFromXML0(Event<XmlIO> xmlize, const Txt& xml)
{
	XmlNode node = ParseXML(xml);
	if(node.GetCount() == 0)
		return false;
	for(i32 i = 0; i < node.GetCount(); i++)
		if(node.Node(i).IsTag()) {
			Val dummy;
			xmlize(XmlIO(node.At(i), true, dummy));
			break;
		}
	return true;
}

bool DoLoadFromXML(Event<XmlIO> xmlize, const Txt& xml)
{
	try {
		return LoadFromXML0(xmlize, xml);
	}
	catch(ValTypeError) {}
	catch(XmlError) {}
	return false;
}

bool DoTryLoadFromXML(Event<XmlIO> xmlize, const Txt& xml)
{
	return LoadFromXML0(xmlize, xml);
}

static Txt sXMLFile(tukk file)
{
	return file ? Txt(file) : ConfigFile(GetExeTitle() + ".xml");
}

bool StoreAsXMLFile(Event<XmlIO> xmlize, tukk name, tukk file)
{
	return SaveFile(sXMLFile(file), DoStoreAsXML(xmlize, name ? (Txt)name : GetExeTitle()));
}

bool LoadFromXMLFile(Event<XmlIO> xmlize, tukk file)
{
	return DoLoadFromXML(xmlize, LoadFile(sXMLFile(file)));
}

bool TryLoadFromXMLFile(Event<XmlIO> xmlize, tukk file)
{
	return DoTryLoadFromXML(xmlize, LoadFile(sXMLFile(file)));
}

void StoreJsonVal(XmlIO& xio, const Val& v)
{
	if(v.GetType() == VALUEMAP_V) {
		ValMap m = v;
		ValArray va = m.GetVals();
		for(i32 i = 0; i < m.GetCount(); i++) {
			Val h = m.GetVal(i);
			XmlIO io = xio.Add((Txt)m.GetKey(i));
			StoreJsonVal(io, h);
		}
		return;
	}
	else
	if(v.GetType() == VALUEARRAY_V) {
		ValArray va = v;
		for(i32 i = 0; i < va.GetCount(); i++) {
			XmlIO io = xio.Add("item");
			Val h = va[i];
			StoreJsonVal(io, h);
		}
	}
	else
	if(v.GetType() == BOOL_V) {
		bool b = v;
		Xmlize(xio, b);
	}
	else
	if(IsNumber(v)) {
		double h = v;
		Xmlize(xio, h);
		return;
	}
	else
	if(IsTxt(v)) {
		Txt h = v;
		Xmlize(xio, h);
	}
	else
	if(v.Is<Date>()) {
		Date h = v;
		Xmlize(xio, h);
	}
	else
	if(v.Is<Time>()) {
		Time h = v;
		Xmlize(xio, h);
	}
	else
		NEVER();
}

Val LoadJsonVal(const XmlNode& n)
{
	Txt h = n.Attr("value");
	if(h.GetCount())
		return ScanDouble(h);
	ValMap m;
	Txt text;
	for(i32 i = 0; i < n.GetCount(); i++)
		if(n[i].IsTag())
			m.Add(n[i].GetTag(), LoadJsonVal(n[i]));
		else
			return n[i].GetText();
	return m;
}

}