Val  ParseJSON(CParser& p);
Val  ParseJSON(tukk s);

inline Txt AsJSON(i32 i)             { return IsNull(i) ? "null" : AsTxt(i); }
inline Txt AsJSON(double n)          { return IsNull(n) ? "null" : AsTxt(n); }
inline Txt AsJSON(bool b)            { return b ? "true" : "false"; }
inline Txt AsJSON(const Txt& s)   { return AsCTxt(s, INT_MAX, NULL, ASCTXT_JSON); }
inline Txt AsJSON(const WTxt& s)  { return AsCTxt(s.ToTxt(), INT_MAX, NULL, ASCTXT_JSON); }
inline Txt AsJSON(tukk s)     { return AsCTxt(s, INT_MAX, NULL, ASCTXT_JSON); }
Txt AsJSON(Time tm);
Txt AsJSON(Date dt);

Txt AsJSON(const Val& v, const Txt& indent, bool pretty);
Txt AsJSON(const Val& v, bool pretty = false);

class JsonArray;

class Json {
	Txt text;

public:
	Json& CatRaw(tukk key, const Txt& val);

	Txt ToTxt() const                                     { return "{" + text + "}"; }
	Txt operator~() const                                    { return ToTxt(); }
	operator Txt() const                                     { return ToTxt(); }

	operator bool() const                                       { return text.GetCount(); }
	
	Json& operator()(tukk key, const Val& value)       { return CatRaw(key, AsJSON(value)); }
	Json& operator()(tukk key, i32 i)                    { return CatRaw(key, AsJSON(i)); }
	Json& operator()(tukk key, double n)                 { return CatRaw(key, AsJSON(n)); }
	Json& operator()(tukk key, bool b)                   { return CatRaw(key, AsJSON(b)); }
	Json& operator()(tukk key, Date d)                   { return CatRaw(key, AsJSON(d)); }
	Json& operator()(tukk key, Time t)                   { return CatRaw(key, AsJSON(t)); }
	Json& operator()(tukk key, const Txt& s)          { return CatRaw(key, AsJSON(s)); }
	Json& operator()(tukk key, const WTxt& s)         { return CatRaw(key, AsJSON(s)); }
	Json& operator()(tukk key, tukk s)            { return CatRaw(key, AsJSON(s)); }
	Json& operator()(tukk key, const Json& object)       { return CatRaw(key, ~object); }
	Json& operator()(tukk key, const JsonArray& array);
		
	Json() {}
	Json(tukk key, const Val& value)                   { CatRaw(key, AsJSON(value)); }
	Json(tukk key, i32 i)                                { CatRaw(key, AsJSON(i)); }
	Json(tukk key, double n)                             { CatRaw(key, AsJSON(n)); }
	Json(tukk key, bool b)                               { CatRaw(key, AsJSON(b)); }
	Json(tukk key, Date d)                               { CatRaw(key, AsJSON(d)); }
	Json(tukk key, Time t)                               { CatRaw(key, AsJSON(t)); }
	Json(tukk key, const Txt& s)                      { CatRaw(key, AsJSON(s)); }
	Json(tukk key, const WTxt& s)                     { CatRaw(key, AsJSON(s)); }
	Json(tukk key, tukk s)                        { CatRaw(key, AsJSON(s)); }
	Json(tukk key, const Json& object)                   { CatRaw(key, ~object); }
	Json(tukk key, const JsonArray& array)               { operator()(key, array); }
};

class JsonArray {
	Txt text;

public:
	JsonArray& CatRaw(const Txt& val);

	Txt ToTxt() const                                     { return "[" + text + "]"; }
	Txt operator~() const                                    { return ToTxt(); }
	operator Txt() const                                     { return ToTxt(); }
	
	operator bool() const                                       { return text.GetCount(); }
	
	JsonArray& operator<<(const Val& value)                   { return CatRaw(AsJSON(value)); }
	JsonArray& operator<<(i32 i)                                { return CatRaw(AsJSON(i)); }
	JsonArray& operator<<(double n)                             { return CatRaw(AsJSON(n)); }
	JsonArray& operator<<(bool b)                               { return CatRaw(AsJSON(b)); }
	JsonArray& operator<<(Date d)                               { return CatRaw(AsJSON(d)); }
	JsonArray& operator<<(Time t)                               { return CatRaw(AsJSON(t)); }
	JsonArray& operator<<(const Txt& s)                      { return CatRaw(AsJSON(s)); }
	JsonArray& operator<<(const WTxt& s)                     { return CatRaw(AsJSON(s)); }
	JsonArray& operator<<(tukk s)                        { return CatRaw(AsJSON(s)); }
	JsonArray& operator<<(const Json& object)                   { return CatRaw(~object); }
	JsonArray& operator<<(const JsonArray& array)               { return CatRaw(~array); }
		
	JsonArray() {}
};

inline Json& Json::operator()(tukk key, const JsonArray& array)
{
	return CatRaw(key, array);
}

class JsonIO {
	const Val   *src;
	One<ValMap>  map;
	Val          tgt;

public:
	bool IsLoading() const                       { return src; }
	bool IsStoring() const                       { return !src; }
	
	const Val& Get() const                     { ASSERT(IsLoading()); return *src; }
	void         Set(const Val& v)             { ASSERT(IsStoring() && !map); tgt = v; }
	
	Val        Get(tukk key)            { ASSERT(IsLoading()); return (*src)[key]; }
	void         Set(tukk key, const Val& v);

	void         Put(Val& v)                   { ASSERT(IsStoring()); if(map) v = *map; else v = tgt; }
	Val        GetResult() const               { ASSERT(IsStoring()); return map ? Val(*map) : tgt; }

	template <class T>
	JsonIO& operator()(tukk key, T& value);

	template <class T>
	JsonIO& operator()(tukk key, T& value, const T& defvalue);

	template <class T>
	JsonIO& List(tukk key, tukk , T& var) { return operator()(key, var); }

	template <class T, class X>
	JsonIO& Var(tukk key, T& value, X item_jsonize);

	template <class T, class X>
	JsonIO& Array(tukk key, T& value, X item_jsonize, tukk  = NULL);
	
	JsonIO(const Val& src) : src(&src)         {}
	JsonIO()                                     { src = NULL; }
};

struct JsonizeError : Exc {
	JsonizeError(const Txt& s) : Exc(s) {}
};

template <class T>
void Jsonize(JsonIO& io, T& var)
{
	var.Jsonize(io);
}

template <class T>
JsonIO& JsonIO::operator()(tukk key, T& value)
{
	if(IsLoading()) {
		const Val& v = (*src)[key];
		if(!v.IsVoid()) {
			JsonIO jio(v);
			Jsonize(jio, value);
		}
	}
	else {
		ASSERT(tgt.IsVoid());
		if(!map)
			map.Create();
		JsonIO jio;
		Jsonize(jio, value);
		if(jio.map)
			map->Add(key, *jio.map);
		else
			map->Add(key, jio.tgt);
	}
	return *this;
}

template <class T, class X>
JsonIO& JsonIO::Var(tukk key, T& value, X jsonize)
{
	if(IsLoading()) {
		const Val& v = (*src)[key];
		if(!v.IsVoid()) {
			JsonIO jio(v);
			jsonize(jio, value);
		}
	}
	else {
		ASSERT(tgt.IsVoid());
		if(!map)
			map.Create();
		JsonIO jio;
		jsonize(jio, value);
		if(jio.map)
			map->Add(key, *jio.map);
		else
			map->Add(key, jio.tgt);
	}
	return *this;
}


template <class T, class X>
void JsonizeArray(JsonIO& io, T& array, X item_jsonize)
{
	if(io.IsLoading()) {
		const Val& va = io.Get();
		array.SetCount(va.GetCount());
		for(i32 i = 0; i < va.GetCount(); i++) {
			JsonIO jio(va[i]);
			item_jsonize(jio, array[i]);
		}
	}
	else {
		Vec<Val> va;
		va.SetCount(array.GetCount());
		for(i32 i = 0; i < array.GetCount(); i++) {
			JsonIO jio;
			item_jsonize(jio, array[i]);
			jio.Put(va[i]);
		}
		io.Set(ValArray(pick(va)));
	}
}

template <class T, class X> JsonIO& JsonIO::Array(tukk key, T& value, X item_jsonize, tukk )
{
	if(IsLoading()) {
		const Val& v = (*src)[key];
		if(!v.IsVoid()) {
			JsonIO jio(v);
			JsonizeArray(jio, value, item_jsonize);
		}
	}
	else {
		ASSERT(tgt.IsVoid());
		if(!map)
			map.Create();
		JsonIO jio;
		JsonizeArray(jio, value, item_jsonize);
		if(jio.map)
			map->Add(key, *jio.map);
		else
			map->Add(key, jio.tgt);
	}
	return *this;
}

template <class T>
JsonIO& JsonIO::operator()(tukk key, T& value, const T& defvalue)
{
	if(IsLoading()) {
		const Val& v = (*src)[key];
		if(v.IsVoid())
			value = defvalue;
		else {
			JsonIO jio(v);
			Jsonize(jio, value);
		}
	}
	else {
		ASSERT(tgt.IsVoid());
		if(!map)
			map.Create();
		JsonIO jio;
		Jsonize(jio, value);
		if(jio.map)
			map->Add(key, *jio.map);
		else
			map->Add(key, jio.tgt);
	}
	return *this;
}

template <class T>
Val StoreAsJsonVal(const T& var)
{
	JsonIO io;
	Jsonize(io, const_cast<T&>(var));
	return io.GetResult();
}

template <class T>
void LoadFromJsonVal(T& var, const Val& x)
{
	JsonIO io(x);
	Jsonize(io, var);
}

template <class T>
Txt StoreAsJson(const T& var, bool pretty = false)
{
	return AsJSON(StoreAsJsonVal(var), pretty);
}

template <class T>
bool LoadFromJson(T& var, tukk json)
{
	try {
		Val jv = ParseJSON(json);
		if(jv.IsError())
			return false;
		LoadFromJsonVal(var, jv);
	}
	catch(ValTypeError) {
		return false;
	}
	catch(JsonizeError) {
		return false;
	}
	return true;
}

Txt sJsonFile(tukk file);

template <class T>
bool StoreAsJsonFile(const T& var, tukk file = NULL, bool pretty = false)
{
	return SaveFile(sJsonFile(file), StoreAsJson(var, pretty));;
}

template <class T>
bool LoadFromJsonFile(T& var, tukk file = NULL)
{
	return LoadFromJson(var, LoadFile(sJsonFile(file)));
}

template<> void Jsonize(JsonIO& io, i32& var);
template<> void Jsonize(JsonIO& io, u8& var);
template<> void Jsonize(JsonIO& io, i16& var);
template<> void Jsonize(JsonIO& io, z64& var);
template<> void Jsonize(JsonIO& io, double& var);
template<> void Jsonize(JsonIO& io, bool& var);
template<> void Jsonize(JsonIO& io, Txt& var);
template<> void Jsonize(JsonIO& io, WTxt& var);
template<> void Jsonize(JsonIO& io, Date& var);
template<> void Jsonize(JsonIO& io, Time& var);

template <class T>
void JsonizeArray(JsonIO& io, T& array)
{
	JsonizeArray(io, array, [](JsonIO& io, ValTypeOf<T>& item) { Jsonize(io, item); });
}

template <class T, class K, class V>
void JsonizeMap(JsonIO& io, T& map, tukk keyid, tukk valueid)
{
	if(io.IsLoading()) {
		map.Clear();
		const Val& va = io.Get();
		map.Reserve(va.GetCount());
		for(i32 i = 0; i < va.GetCount(); i++) {
			K key;
			V value;
			LoadFromJsonVal(key, va[i][keyid]);
			LoadFromJsonVal(value, va[i][valueid]);
			map.Add(key, pick(value));
		}
	}
	else {
		Vec<Val> va;
		va.SetCount(map.GetCount());
		for(i32 i = 0; i < map.GetCount(); i++)
			if(!map.IsUnlinked(i)) {
				ValMap item;
				item.Add(keyid, StoreAsJsonVal(map.GetKey(i)));
				item.Add(valueid, StoreAsJsonVal(map[i]));
				va[i] = item;
			}
		io.Set(ValArray(pick(va)));
	}
}

template <class T, class K, class V>
void JsonizeSortedMap(JsonIO& io, T& map, tukk keyid, tukk valueid)
{
	if(io.IsLoading()) {
		map.Clear();
		const Val& va = io.Get();
		for(i32 i = 0; i < va.GetCount(); i++) {
			K key;
			V value;
			LoadFromJsonVal(key, va[i][keyid]);
			LoadFromJsonVal(value, va[i][valueid]);
			map.Add(key, pick(value));
		}
	}
	else {
		Vec<Val> va;
		va.SetCount(map.GetCount());
		for(i32 i = 0; i < map.GetCount(); i++) {
			ValMap item;
			item.Add(keyid, StoreAsJsonVal(map.GetKey(i)));
			item.Add(valueid, StoreAsJsonVal(map[i]));
			va[i] = item;
		}
		io.Set(ValArray(pick(va)));
	}
}

template <class T, class K, class V>
void JsonizeTxtMap(JsonIO& io, T& map)
{
	if(io.IsLoading()) {
		map.Clear();
		const ValMap& va = io.Get();
		map.Reserve(va.GetCount());
		for(i32 i = 0; i < va.GetCount(); i++) {
			V value;
			Txt key = va.GetKey(i);
			LoadFromJsonVal(key, va.GetKey(i));
			LoadFromJsonVal(value, va.GetVal(i));
			map.Add(key, pick(value));
		}
	}
	else {
		Index<Val>  index;
		Vec<Val> values;
		index.Reserve(map.GetCount());
		values.Reserve(map.GetCount());
		for (i32 i=0; i<map.GetCount(); ++i)
		{
			index.Add(StoreAsJsonVal(map.GetKey(i)));
			values.Add(StoreAsJsonVal(map[i]));
		}
		ValMap vm(pick(index), pick(values));
		io.Set(vm);
	}
}

template <class K, class V>
void TxtMap(JsonIO& io, VecMap<K, V>& map)
{
	JsonizeTxtMap<VecMap<K, V>, K, V>(io, map);
}

template <class K, class V>
void TxtMap(JsonIO& io, ArrayMap<K, V>& map)
{
	JsonizeTxtMap<ArrayMap<K, V>, K, V>(io, map);
}

template <class T, class V>
void JsonizeIndex(JsonIO& io, T& index)
{
	if(io.IsLoading()) {
		const Val& va = io.Get();
		index.Reserve(va.GetCount());
		for(i32 i = 0; i < va.GetCount(); i++) {
			V v;
			LoadFromJsonVal(v, va[i]);
			index.Add(pick(v));
		}
	}
	else {
		Vec<Val> va;
		for(i32 i = 0; i < index.GetCount(); i++)
			if(!index.IsUnlinked(i))
				va.Add(StoreAsJsonVal(index[i]));
		io.Set(ValArray(pick(va)));
	}
}

template <class T>
void JsonizeBySerialize(JsonIO& jio, T& x)
{
	Txt h;
	if(jio.IsStoring())
	   h = HexTxt(StoreAsTxt(x));
	jio("data", h);
	if(jio.IsLoading())
		try {
			LoadFromTxt(x, ScanHexTxt(h));
		}
		catch(LoadingError) {
			throw JsonizeError("jsonize by serialize error");
		}
}

template <class IZE>
struct LambdaIzeVar {
	IZE& ize;

	void Jsonize(JsonIO& io) { ize(io); }
	void Xmlize(XmlIO& io) { ize(io); }
	
	LambdaIzeVar(IZE& ize) : ize(ize) {}
};

template <class IO, class IZE>
void LambdaIze(IO& io, tukk id, IZE ize)
{
	LambdaIzeVar<IZE> var(ize);
	io(id, var);
}
