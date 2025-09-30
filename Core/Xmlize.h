class XmlIO;

template <class T>
void XmlAttrLoad(T& var, const Txt& text)
{
	var.XmlAttrLoad(text);
}

template <class T>
Txt XmlAttrStore(const T& var)
{
	return var.XmlAttrStore();
}

class XmlIO {
	XmlNode& node;
	bool     loading;
	Val    userdata;

public:
	bool IsLoading() const            { return loading; }
	bool IsStoring() const            { return !loading; }

	XmlNode& Node()                   { return node; }
	const XmlNode& Node() const       { return node; }

	XmlNode *operator->()             { return &node; }

	Txt GetAttr(tukk id)                    { return node.Attr(id); }
	void   SetAttr(tukk id, const Txt& val) { node.SetAttr(id, val); }

	template <class T> XmlIO operator()(tukk tag, T& var);
	template <class T> XmlIO List(tukk tag, tukk itemtag, T& var);
	template <class T, class X> XmlIO Var(tukk tag, T& var, X var_xmlize);
	template <class T, class X> XmlIO Array(tukk tag, T& var, X item_xmlize, tukk itemtag = "item");

	template <class T, class D> XmlIO operator()(tukk tag, T& var, const D& def);
	template <class T, class D> XmlIO List(tukk tag, tukk itemtag, T& var, const D& def);

	template <class T> XmlIO Attr(tukk id, T& var) {
		if(IsLoading())
			XmlAttrLoad(var, node.Attr(id));
		else
			node.SetAttr(id, XmlAttrStore(var));
		return *this;
	}

	template <class T, class D> XmlIO Attr(tukk id, T& var, const D& def) {
		if(IsLoading())
		    if(IsNull(node.Attr(id)))
				var = def;
		    else
				XmlAttrLoad(var, node.Attr(id));
		else
		if(var != def)
			node.SetAttr(id, XmlAttrStore(var));
		return *this;
	}

	XmlIO At(i32 i)                                    { XmlIO m(node.At(i), IsLoading(), userdata); return m; }
	XmlIO Add()                                        { XmlIO m(node.Add(), IsLoading(), userdata); return m; }
	XmlIO Add(tukk id)                          { XmlIO m(node.Add(id), IsLoading(), userdata); return m; }
	XmlIO GetAdd(tukk id)                       { XmlIO m(node.GetAdd(id), IsLoading(), userdata); return m; }
	
	void  SetUserData(const Val& v)                  { userdata = v; }
	Val GetUserData() const                          { return userdata; }

	XmlIO(XmlNode& xml, bool loading, const Val& userdata) : node(xml), loading(loading), userdata(userdata) {}
	XmlIO(XmlNode& xml, bool loading) : node(xml), loading(loading) {}
	XmlIO(XmlIO xml, tukk tag) : node(xml.node.GetAdd(tag)), loading(xml.loading), userdata(xml.userdata) {}
};

template<> inline void XmlAttrLoad(Txt& var, const Txt& text) { var = text; }
template<> inline Txt XmlAttrStore(const Txt& var)            { return var; }

template<> void XmlAttrLoad(WTxt& var, const Txt& text);
template<> Txt XmlAttrStore(const WTxt& var);
template<> void XmlAttrLoad(i32& var, const Txt& text);
template<> Txt XmlAttrStore(i32k& var);
template<> void XmlAttrLoad(dword& var, const Txt& text);
template<> Txt XmlAttrStore(const dword& var);
template<> void XmlAttrLoad(double& var, const Txt& text);
template<> Txt XmlAttrStore(const double& var);
template<> void XmlAttrLoad(bool& var, const Txt& text);
template<> Txt XmlAttrStore(const bool& var);
template <> void XmlAttrLoad(i16& var, const Txt& text);
template <> Txt XmlAttrStore(const i16& var);
template <> void XmlAttrLoad(z64& var, const Txt& text);
template <> Txt XmlAttrStore(const z64& var);
template <> void XmlAttrLoad(u8& var, const Txt& text);
template <> Txt XmlAttrStore(u8k& var);
template <> void XmlAttrLoad(Date& var, const Txt& text);
template <> Txt XmlAttrStore(const Date& var);
template <> void XmlAttrLoad(Time& var, const Txt& text);
template <> Txt XmlAttrStore(const Time& var);

template<> void Xmlize(XmlIO& xml, Txt& var);
template<> void Xmlize(XmlIO& xml, WTxt& var);
template<> void Xmlize(XmlIO& xml, i32& var);
template<> void Xmlize(XmlIO& xml, dword& var);
template<> void Xmlize(XmlIO& xml, double& var);
template<> void Xmlize(XmlIO& xml, bool& var);
template<> void Xmlize(XmlIO& xml, Date& var);
template<> void Xmlize(XmlIO& xml, Time& var);
template<> void Xmlize(XmlIO& xml, i16& var);
template<> void Xmlize(XmlIO& xml, z64& var);
template<> void Xmlize(XmlIO& xml, u8& var);

void XmlizeLangAttr(XmlIO& xml, i32& lang, tukk id = "lang");
void XmlizeLang(XmlIO& xml, tukk tag, i32& lang, tukk id = "id");

template <class T>
void Xmlize(XmlIO& xml, T& var)
{
	var.Xmlize(xml);
}

template <class T>
void Xmlize(XmlIO& xml, tukk itemtag, T& var)
{
	var.Xmlize(xml, itemtag);
}

template <class T, class X>
void XmlizeContainer(XmlIO& xml, tukk tag, T& data, X item_xmlize);

template<class T>
void XmlizeContainer(XmlIO& xml, tukk tag, T& data);

template<class K, class V, class T>
void XmlizeMap(XmlIO& xml, tukk keytag, tukk valuetag, T& data);

template<class K, class V, class T>
void XmlizeSortedMap(XmlIO& xml, tukk keytag, tukk valuetag, T& data);

template<class K, class T>
void XmlizeIndex(XmlIO& xml, tukk keytag, T& data);

template<class T>
void XmlizeStore(XmlIO& xml, const T& data)
{
	ASSERT(xml.IsStoring());
	Xmlize(xml, const_cast<T&>(data));
}

template <class T>
struct ParamHelper__ {
	T&   data;
	void Invoke(XmlIO xml) {
		Xmlize(xml, data);
	}

	ParamHelper__(T& data) : data(data) {}
};

Txt DoStoreAsXML(Event<XmlIO> xmlize, tukk name);
bool   DoLoadFromXML(Event<XmlIO> xmlize, const Txt& xml);
bool   DoTryLoadFromXML(Event<XmlIO> xmlize, const Txt& xml);

template <class T>
Txt StoreAsXML(const T& data, tukk name = NULL)
{
	ParamHelper__<T> p(const_cast<T &>(data));
	return DoStoreAsXML([&](XmlIO io) { Xmlize(io, const_cast<T &>(data)); }, name);
}

template <class T>
bool LoadFromXML(T& data, const Txt& xml)
{
	ParamHelper__<T> p(data);
	return DoLoadFromXML(callback(&p, &ParamHelper__<T>::Invoke), xml);
}

template <class T>
bool TryLoadFromXML(T& data, const Txt& xml)
{
	ParamHelper__<T> p(data);
	return DoTryLoadFromXML(callback(&p, &ParamHelper__<T>::Invoke), xml);
}

bool StoreAsXMLFile(Event<XmlIO> xmlize, tukk name = NULL, tukk file = NULL);
bool LoadFromXMLFile(Event<XmlIO> xmlize, tukk file = NULL);
bool TryLoadFromXMLFile(Event<XmlIO> xmlize, tukk file = NULL);

template <class T>
bool StoreAsXMLFile(T& data, tukk name = NULL, tukk file = NULL)
{
	ParamHelper__<T> p(data);
	return StoreAsXMLFile(callback(&p, &ParamHelper__<T>::Invoke), name, file);
}

template <class T>
bool LoadFromXMLFile(T& data, tukk file = NULL)
{
	ParamHelper__<T> p(data);
	return LoadFromXMLFile(callback(&p, &ParamHelper__<T>::Invoke), file);
}

template <class T>
bool TryLoadFromXMLFile(T& data, tukk file = NULL)
{
	ParamHelper__<T> p(data);
	return TryLoadFromXMLFile(callback(&p, &ParamHelper__<T>::Invoke), file);
}

template <class T>
void XmlizeBySerialize(XmlIO& xio, T& x)
{
	Txt h;
	if(xio.IsStoring())
		h = HexTxt(StoreAsTxt(x));
	xio.Attr("data", h);
	if(xio.IsLoading())
		try {
			LoadFromTxt(x, ScanHexTxt(h));
		}
		catch(LoadingError) {
			throw XmlError("ошибка xmlize по serialize");
		}
}

void  StoreJsonVal(XmlIO& xio, const Val& v);
Val LoadJsonVal(const XmlNode& n);

template <class T>
void XmlizeByJsonize(XmlIO& xio, T& x)
{
	if(xio.IsStoring())
		StoreJsonVal(xio, StoreAsJsonVal(x));
	else {
		try {
			LoadFromJsonVal(x, LoadJsonVal(xio.Node()));
		}
		catch(JsonizeError e) {
			throw XmlError("xmlize by jsonize error: " + e);
		}
	}
}

#include "Xmlize.hpp"