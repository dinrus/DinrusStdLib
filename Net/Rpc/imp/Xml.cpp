#include <drx/Core/Core.h>
#include <drx/Net/Rpc/Rpc.h>

namespace drx {

Val ParseXmlRpcVal(XmlParser& p)
{
	Val r;
	p.PassTag("value");
	if(p.Tag("i32") || p.Tag("i4")) {
		Txt s = p.ReadText();
		CParser p(s);
		if(!p.IsInt())
			throw XmlError("ожидалось целое");
		r = p.ReadInt();
	}
	else
	if(p.Tag("i8")) {
		Txt s = p.ReadText();
		CParser p(s);
		if(!p.IsInt())
			throw XmlError("ожидалось целое");
		r = p.ReadInt64();
	}
	else
	if(p.Tag("boolean")) {
		i32 n = StrInt(p.ReadText());
		if(n != 0 && n != 1)
			throw XmlError("ожидалось булево");
		r = (bool)n;
	}
	else
	if(p.Tag("double")) {
		Txt s = p.ReadText();
		CParser p(s);
		if(!p.IsDouble())
			throw XmlError("ожидалось дво");
		r = p.ReadDouble();
	}
	else
	if(p.Tag("string") || p.Tag("base64"))
		r = p.ReadText();
	else
	if(p.TagE("nil")) {
		p.PassEnd();
		return r;
	}
	else
	if(p.Tag("dateTime.iso8601")) {
		Txt s = TrimBoth(p.ReadText());
		// 19980717T14:08:55
		// 01234567890123456
		if(s.GetCount() < 17 || s[8] != 'T' || s[11] != ':' || s[14] != ':') // <17 to allow zone ignored zone info
			throw XmlError("неверный фомат даты/времени");
		Time tm;
		tm.year = atoi(s.Mid(0, 4));
		tm.month = atoi(s.Mid(4, 2));
		tm.day = atoi(s.Mid(6, 2));
		tm.hour = atoi(s.Mid(9, 2));
		tm.minute = atoi(s.Mid(12, 2));
		tm.second = atoi(s.Mid(15, 2));
		r = tm;
	}
	else
	if(p.Tag("array")) {
		ValArray va;
		p.PassTag("data");
		while(!p.End())
			va.Add(ParseXmlRpcVal(p));
		r = va;
	}
	else
	if(p.Tag("struct")) {
		ValMap vm;
		while(p.Tag("member")) {
			p.PassTag("name");
			Txt name = p.ReadText();
			p.PassEnd(); // name
			vm.Add((Val)name, ParseXmlRpcVal(p));
			p.PassEnd(); // member
		}
		r = vm;
	}
	else
		throw XmlError("значение неизвестного типа");
	p.PassEnd();
	p.PassEnd();
	return r;
}

Val ParseXmlRpcParam(XmlParser& p)
{
	p.PassTag("param");
	Val v = ParseXmlRpcVal(p);
	p.PassEnd();
	return v;
	}

ValArray ParseXmlRpcParams(XmlParser& p)
{
	ValArray va;
	if(p.Tag("params"))
		while(!p.End())
			va.Add(ParseXmlRpcParam(p));
	return va;
}

Txt FormatXmlRpcVal(const Val& _v, bool supports_i8)
{
	Txt r;
	Val v = _v;
	if(v.GetType() == INT64_V && !supports_i8) {
		z64 x = v;
		if((i32)x == x)
			v = (i32)x;
	}
	if(IsNull(v) && !IsTxt(v) && !IsValArray(v))
		r = XmlTag("nil")();
	else
	if(v.GetType() == INT64_V && supports_i8)
		r = XmlTag("i8")(AsTxt((z64)v));
	else
	if(v.GetType() == INT_V)
		r = XmlTag("i32")(Format("%d", (i32)v));
	else
	if(v.GetType() == BOOL_V)
		r = XmlTag("boolean")(AsTxt((i32)(bool)v));
	else
	if(IsNumber(v))
		r = XmlTag("double")(Format("%.16g", (double)v));
	else
	if(IsDateTime(v)) {
		Time t = v;
		r = XmlTag("dateTime.iso8601")
					(Format("%04.4d%02.2d%02.2d`T%02.2d`:%02.2d`:%02.2d",
					        t.year, t.month, t.day, t.hour, t.minute, t.second));
	}
	else
	if(v.GetType() == VALUEMAP_V) {
		r = "<struct>";
		ValMap vm = v;
		const Index<Val>& k = vm.GetKeys();
		ValArray va = vm.GetVals();
		for(i32 i = 0; i < k.GetCount(); i++)
			r << XmlTag("member")(XmlTag("name")(k[i]) + FormatXmlRpcVal(va[i], supports_i8));
		r << "</struct>";
	}
	else
	if(v.GetType() == VALUEARRAY_V) {
		r = "<array><data>";
		ValArray va = v;
		for(i32 i = 0; i < va.GetCount(); i++)
			r << FormatXmlRpcVal(va[i], supports_i8);
		r << "</data></array>";
	}
	else
	if(v.Is<RawJsonText>())
		r = XmlTag("string").Text(v.To<RawJsonText>().json);
	else
		r = XmlTag("string").Text(v);
	return XmlTag("value")(r);
}

Txt FormatXmlRpcParam(const Val& param, bool supports_i8)
{
	return XmlTag("param")(FormatXmlRpcVal(param, supports_i8));
}

Txt FormatXmlRpcParams(const ValArray& params, bool supports_i8)
{
	Txt r;
	r = "<params>";
	for(i32 i = 0; i < params.GetCount(); i++)
		r << FormatXmlRpcParam(params[i], supports_i8);
	r << "</params>";
	return r;
}

Txt FormatXmlRpcError(i32 code, tukk text)
{
	Txt r;
	r << XmlHeader() <<
		"<methodResponse>"
		  "<fault>"
		    "<value>"
		      "<struct>"
		        "<member>"
		          "<name>faultCode</name>"
		          "<value><i32>" << code << "</i32></value>"
		        "</member>"
		        "<member>"
		          "<name>faultTxt</name>"
		          "<value><string>" << DeXml(text) << "</string></value>"
		        "</member>"
		      "</struct>"
		    "</value>"
		  "</fault>"
		"</methodResponse>"
	;
	return r;
}

}
