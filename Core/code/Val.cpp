#include "../Core.h"

namespace drx {

const Nuller Null;

#define LTIMING(x) // RTIMING(x)

ValTypeError::ValTypeError(const Txt& text, const Val& src, i32 target)
:	Exc(text), src(src), target(target) {}

hash_t Val::GetOtherHashVal() const {
	if(IsNull())
		return 0;
	u8 st = data.GetSt();
	if(st == REF)
		return ptr()->GetHashVal();
	return svo[st]->GetHashVal(&data);
}

void Val::RefRelease()
{
	ASSERT(IsRef()); // Check that svo type is not registered as Ref
	ptr()->Release();
}

void Val::RefRetain()
{
	ptr()->Retain();
}

Val& Val::operator=(const Val& v) {
	if(this == &v) return *this;
	Val h = v; // Make copy a 'v' can be reference to ValMap/Array contained element
	FreeRef();   // e.g. json = json["foo"]
	data = h.data;
	if(IsRef())
		ptr()->Retain();
	return *this;
}

void Val::SetLarge(const Val& v)
{
	if(v.IsRef()) {
		data.SetSmall(v.data);
		RefRetain();
	}
	else
		data.LSet(v.data);
}

dword Val::GetType() const
{
	if(IsTxt())
		return TXT_V;
	u8 st = data.GetSt();
	return st == REF ? GetRefType() : st == VOIDV ? VOID_V : st;
}

bool Val::IsNull() const
{
	if(IsTxt())
		return data.GetCount() == 0;
	i32 st = data.GetSt();
	if(st == VOIDV)
		return true;
	if(st == REF)
		return ptr()->IsNull();
	return svo[st]->IsNull(&data);
}

bool Val::IsPolyEqual(const Val& v) const
{
	i32 st = data.GetSpecial();
	if(st == REF)
		return ptr()->IsPolyEqual(v);
	if(svo[st] && svo[st]->IsPolyEqual(&data, v))
		return true;
	return IsNull() && v.IsNull();
}

bool Val::operator==(const Val& v) const {
	if(IsTxt() && v.IsTxt())
		return data == v.data;
	if(GetType() != v.GetType()) {
		if(IsPolyEqual(v) || v.IsPolyEqual(*this))
			return true;
	}
	else {
		i32 st = data.GetSpecial();
		if(st == REF) {
			if(ptr()->IsEqual(v.ptr()))
				return true;
		}
		else
		if(st != VOIDV) {
			if(svo[st]->IsEqual(&data, &v.data))
				return true;
		}
	}
	return IsNull() && v.IsNull();
}

i32 Val::PolyCompare(const Val& v) const
{
	i32 st = data.GetSpecial();
	if(st == REF)
		return ptr()->PolyCompare(v);
	if(st != VOIDV)
		return svo[st]->PolyCompare(&data, v);
	return 0;
}

i32 Val::Compare2(const Val& v) const
{
	if(IsTxt() && v.IsTxt())
		return SgnCompare(data, v.data);
	dword stw = data.GetStW();
	if(stw == v.data.GetStW()) {
		if(stw == Txt::StW(INT64_V))
			return SgnCompare(GetSmallRaw<z64>(), v.GetSmallRaw<z64>());
		if(stw == Txt::StW(DATE_V))
			return SgnCompare(GetSmallRaw<Date>(), v.GetSmallRaw<Date>());
		if(stw == Txt::StW(TIME_V))
			return SgnCompare(GetSmallRaw<Time>(), v.GetSmallRaw<Time>());
	}
	bool a = IsNull();
	bool b = v.IsNull();
	if(a || b)
		return SgnCompare(b, a);
	i32 st = data.GetSpecial();
	if(GetType() == v.GetType()) {
		if(st == REF)
			return ptr()->Compare(v.ptr());
		if(st != VOIDV)
			return svo[st]->Compare(&data, &v.data);
	}
	if(st != VOIDV) {
		i32 q = PolyCompare(v);
		if(q) return q;
		return -v.PolyCompare(*this);
	}
	return 0;
}

bool Val::IsSame(const Val& b) const
{
	const Val& a = *this;
	if(a.Is<ValMap>() && b.Is<ValMap>())
		return ValMap(a).IsSame(ValMap(b));
	else
	if(a.Is<ValArray>() && b.Is<ValArray>()) {
		if(a.GetCount() != b.GetCount())
			return false;
		for(i32 i = 0; i < a.GetCount(); i++)
			if(!a[i].IsSame(b[i]))
				return false;
		return true;
	}
	else
		return a == b;
}

Val::Val(const WTxt& s) { InitRef(new RichValRep<WTxt>(s), WTXT_V); Magic(); }

Val::operator WTxt() const
{
	if(IsNull()) return Null;
	return GetType() == WTXT_V ? To<WTxt>() : ((Txt)(*this)).ToWTxt();
}

Date Val::GetOtherDate() const
{
	if(IsNull()) return Null;
	return GetSmall<Time>();
}

Time Val::GetOtherTime() const
{
	if(IsNull()) return Null;
	return ToTime(GetSmall<Date>());
}

Txt Val::GetOtherTxt() const
{
	if(IsNull()) return Null;
	if(Is<Txt>())
		return To<Txt>();
	return To<WTxt>().ToTxt();
}

i32 Val::GetOtherInt() const
{
	if(IsNull()) return Null;
	return data.IsSpecial(BOOL_V) ? (i32)GetSmall<bool>() :
	       data.IsSpecial(INT64_V) ? (i32)GetSmall<z64>() :
	       (i32)GetSmall<double>();
}

z64 Val::GetOtherInt64() const
{
	if(IsNull()) return Null;
	return data.IsSpecial(BOOL_V) ? (z64)GetSmall<bool>() :
	       data.IsSpecial(INT_V) ? (z64)GetSmall<i32>() :
	       (z64)GetSmall<double>();
}

double Val::GetOtherDouble() const
{
	if(IsNull()) return Null;
	return data.IsSpecial(BOOL_V) ? (double)GetSmall<bool>() :
	       data.IsSpecial(INT_V) ? (double)GetSmall<i32>() :
	       (double)GetSmall<z64>();
}

bool Val::GetOtherBool() const
{
	if(IsNull()) return Null;
	return data.IsSpecial(DOUBLE_V) ? (bool)GetSmall<double>() :
	       data.IsSpecial(INT_V) ? (bool)GetSmall<i32>() :
	       (bool)GetSmall<z64>();
}

VecMap<dword, Val::Void *(*)()>& Val::Typemap()
{
	static VecMap<dword, Val::Void *(*)()> x;
	return x;
}

Index<Txt>& Val::NameNdx()
{
	static Index<Txt> x;
	return x;
}

Index<dword>& Val::TypeNdx()
{
	static Index<dword> x;
	return x;
}

void Val::AddName(dword type, tukk name)
{
	NameNdx().Add(name);
	TypeNdx().Add(type);
}

i32 Val::GetType(tukk name)
{
	i32 q = NameNdx().Find(name);
	if(q < 0)
		return Null;
	return TypeNdx()[q];
}

Txt Val::GetName(dword type)
{
	i32 q = TypeNdx().Find(type);
	if(q < 0)
		return Null;
	return NameNdx()[q];
}

SVO_FN(s_Txt, Txt);
SVO_FN(s_int, i32);
SVO_FN(s_double, double);
SVO_FN(s_int64, z64);
SVO_FN(s_bool, bool);
SVO_FN(s_date, Date);
SVO_FN(s_time, Time);

struct SvoVoidFn {
	static bool       IsNull(ukk p)                      { return true; }
	static void       Serialize(uk p, Stream& s)              {}
	static void       Xmlize(uk p, XmlIO& xio)               {}
	static void       Jsonize(uk p, JsonIO& jio)             {}
	static hash_t     GetHashVal(ukk p)                { return 0; }
	static bool       IsEqual(ukk p1, ukk p2)    { return true; }
	static bool       IsPolyEqual(ukk p, const Val& v) { return false; }
	static Txt     AsTxt(ukk p)                    { return Txt(); }
};

static Val::Sval s_void = {
	SvoVoidFn::IsNull, SvoVoidFn::Serialize,SvoVoidFn::Xmlize, SvoVoidFn::Jsonize,
	SvoVoidFn::GetHashVal, SvoVoidFn::IsEqual,
	SvoVoidFn::IsPolyEqual, SvoVoidFn::AsTxt
};

Val::Sval *Val::svo[256] = {
	&s_Txt, // TXT_V
	&s_int, // INT_V

	&s_double, //DOUBLE_V  = 2;
	&s_void, //VOIDV_V  = 3;
	&s_date, //DATE_V    = 4;
	&s_time, //TIME_V    = 5;

	NULL, //ERROR_V   = 6;

	NULL, //VALUE_V   = 7;

	NULL, //WTXT_V = 8;

	NULL, //VALUEARRAY_V = 9;

	&s_int64, //INT64_V  = 10;
	&s_bool, //BOOL_V   = 11;

	NULL, //VALUEMAP_V   = 12;
};

Val::Void *ValArrayDataCreate()
{
	return new ValArray::Data;
}

Val::Void *ValMapDataCreate()
{
	return new ValMap::Data;
}

void Val::RegisterStd()
{
	ONCELOCK {
		Val::Register<WTxt>("WTxt");
		Val::Register<Complex>("Complex");
		Val::Register(VALUEARRAY_V, ValArrayDataCreate, "ValArray");
		Val::Register(VALUEMAP_V, ValMapDataCreate, "ValMap");
		Val::AddName(TXT_V, "Txt");
		Val::AddName(INT_V, "i32");
		Val::AddName(DOUBLE_V, "double");
		Val::AddName(VOID_V, "void");
		Val::AddName(DATE_V, "Date");
		Val::AddName(TIME_V, "Time");
		Val::AddName(INT64_V, "z64");
		Val::AddName(BOOL_V, "bool");
		Val::AddName(ERROR_V, "error");
	};
}

void ValRegisterHelper()
{
	Val::RegisterStd();
}

INITBLOCK {
	ValRegisterHelper();
}

void Val::Serialize(Stream& s) {
	RegisterStd();
	dword type;
	if(s.IsLoading()) {
		s / type;
		if(type >= 0x8000000)
			s.LoadError();
		Free();
		i32 st = type == VOID_V ? VOIDV : type == TXT_V ? TXT : type;
		if(st == TXT)
			s % data;
		else
		if(st >= 0 && st < 255 && svo[st]) {
			data.SetSpecial((u8)type);
			svo[st]->Serialize(&data, s);
		}
		else {
			typedef Void* (*vp)();
			vp *cr = Typemap().FindPtr(type);
			if(cr) {
				Void *p = (**cr)();
				p->Serialize(s);
				InitRef(p, type);
			}
			else {
				Free();
				data.SetSpecial(3);
				if(type != VOID_V && type != ERROR_V)
					s.LoadError();
			}
		}
	}
	else {
		type = GetType();
		ASSERT(type < 0x8000000); // only Vals with assigned real type ID can be serialized
		s / type;
		i32 st = data.GetSpecial();
		ASSERT_(!type || type == ERROR_V || type == UNKNOWN_V || st == TXT ||
		        (IsRef() ? Typemap().Find(type) >= 0 : st < 255 && svo[st]),
		        GetName() + " не требуется для сериализации");
		if(st == VOIDV)
			return;
		if(st == TXT)
			s % data;
		else
		if(IsRef())
			ptr()->Serialize(s);
		else
			svo[st]->Serialize(&data, s);
	}
}

static Txt s_binary("serialized_binary");

void Val::Xmlize(XmlIO& xio)
{
	RegisterStd();
	if(xio.IsStoring()) {
		if(IsError()) {
			Val v = Null;
			v.Xmlize(xio);
			return;
		}
		dword type = GetType();
		Txt name = GetName(type);
		if(name.GetCount() == 0) {
			xio.SetAttr("type", s_binary);
			Txt s = HexTxt(StoreAsTxt(*this));
			drx::Xmlize(xio, s);
		}
		else {
			xio.SetAttr("type", name);
			i32 st = data.GetSpecial();
			ASSERT_(!type || type == ERROR_V || type == UNKNOWN_V || st == TXT ||
			        (IsRef() ? Typemap().Find(type) >= 0 : st < 255 && svo[st]),
			        GetName() + " не зарегистрирован для xml-изации");
			if(st == VOIDV)
				return;
			if(st == TXT)
				drx::Xmlize(xio, data);
			else
			if(IsRef())
				ptr()->Xmlize(xio);
			else
				svo[st]->Xmlize(&data, xio);
		}
	}
	else {
		Txt name = xio.GetAttr("type");
		if(drx::IsNull(name))
			*this = Val();
		else
		if(name == s_binary) {
			Txt s;
			drx::Xmlize(xio, s);
			try {
				LoadFromTxt(*this, ScanHexTxt(s));
			}
			catch(LoadingError) {
				throw XmlError("Ошибка при xml-изации сериализованного бинарника");
			}
		}
		else {
			i32 type = GetType(name);
			if(drx::IsNull(type))
				throw XmlError("Неправильный тип значения");
			Free();
			i32 st = (dword)type == VOID_V ? VOIDV : (dword)type == TXT_V ? TXT : type;
			if(st == TXT)
				drx::Xmlize(xio, data);
			else
			if(st < 255 && svo[st]) {
				data.SetSpecial((u8)type);
				svo[st]->Xmlize(&data, xio);
			}
			else {
				typedef Void* (*vp)();
				vp *cr = Typemap().FindPtr(type);
				if(cr) {
					Void *p = (**cr)();
					p->Xmlize(xio);
					InitRef(p, type);
				}
				else
					throw XmlError("Неправильный тип значения");
			}
		}
	}
}

void Val::Jsonize(JsonIO& jio)
{
	RegisterStd();
	if(jio.IsStoring()) {
		if(IsNull())
			jio.Set(Null);
		else {
			dword type = GetType();
			Txt name = GetName(type);
			if(name.GetCount() == 0) {
				Txt s = HexTxt(StoreAsTxt(*this));
				jio("type", s_binary)
				   ("value", s);
			}
			else {
				i32 st = data.GetSpecial();
				ASSERT_(!type || type == ERROR_V || type == UNKNOWN_V || st == TXT ||
				        (IsRef() ? Typemap().Find(type) >= 0 : st < 255 && svo[st]),
				        GetName() + " не зарегистрирован для json-изации");
				if(st == VOIDV)
					return;
				JsonIO hio;
				if(st == TXT) {
					Txt h = data;
					drx::Jsonize(hio, h);
				}
				else {
					if(IsRef())
						ptr()->Jsonize(hio);
					else
						svo[st]->Jsonize(&data, hio);
				}
				ValMap m;
				m.Add("type", name);
				m.Add("value", hio.GetResult());
				jio.Set(m);
			}
		}
	}
	else {
		Val g = jio.Get();
		if(g.IsNull())
			*this = Null;
		else {
			Txt name = g["type"];
			Val  val = g["value"];
			if(name == s_binary) {
				if(!drx::IsTxt(val))
					throw JsonizeError("Ошибка сериализованного бинарника");
				Txt s = val;
				try {
					LoadFromTxt(*this, ScanHexTxt(s));
				}
				catch(LoadingError) {
					throw JsonizeError("Ошибка сериализованного бинарника");
				}
			}
			else {
				i32 type = GetType(name);
				if(drx::IsNull(type))
					throw JsonizeError("Неверный тип значения");
				Free();
				i32 st = (dword)type == VOID_V ? VOIDV : (dword)type == TXT_V ? TXT : type;
				if(st == TXT) {
					if(!drx::IsTxt(val))
						throw JsonizeError("Ошибка в serialized_binary");
					data = val;
				}
				else {
					JsonIO hio(val);
					if(st < 255 && svo[st]) {
						data.SetSpecial((u8)type);
						svo[st]->Jsonize(&data, hio);
					}
					else {
						typedef Void* (*vp)();
						vp *cr = Typemap().FindPtr(type);
						if(cr) {
							Void *p = (**cr)();
							p->Jsonize(hio);
							InitRef(p, type);
						}
						else
							throw JsonizeError("Неверный тип значения");
					}
				}
			}
		}
	}
}

void Val::Register(dword w, Void* (*c)(), tukk name) {
#ifdef flagCHECKINIT
	RLOG("Register valuetype " << w);
#endif
	AssertST(); // all needs to be registered at file level scope
	ASSERT(w != UNKNOWN_V);
	ASSERT(w < 0x8000000);
	CHECK(Typemap().GetAdd(w, c) == c);
	AddName(w, name);
}

Txt  Val::ToTxt() const {
	if(IsNull())
		return Null;
	if(IsTxt())
		return data;
	if(IsRef())
		return ptr()->AsTxt();
	i32 st = data.GetSpecial();
	return svo[st]->AsTxt(&data);
}

i32 Val::GetCount() const
{
	if(IsRef()) {
		dword t = GetRefType();
		if(t == VALUEARRAY_V)
			return ((ValArray::Data *)ptr())->data.GetCount();
		if(t == VALUEMAP_V)
			return ((ValMap::Data *)ptr())->value.GetCount();
	}
	return 0;
}

const Val& Val::operator[](i32 i) const
{
	if(IsRef()) {
		dword t = GetRefType();
		if(t == VALUEARRAY_V)
			return ((ValArray::Data *)ptr())->data[i];
		if(t == VALUEMAP_V)
			return ((ValMap::Data *)ptr())->value[i];
	}
	return ErrorVal();
}

const Vec<Val>& Val::GetVA() const
{
	if(IsRef()) {
		if(Is<ValArray>())
			return ((ValArray::Data *)ptr())->data;
		if(Is<ValMap>())
			return ((ValMap::Data *)ptr())->value.data->data;
	}
	return ValArray::VoidData;
}

force_inline
Vec<Val>& Val::UnShareArray()
{
	ValArray::Data *data = (ValArray::Data *)ptr();
	if(data->GetRefCount() != 1) {
		ValArray::Data *d = new ValArray::Data;
		d->data = clone(data->data);
		data->Release();
		ptr() = d;
		data = d;
	}
	return data->data;
}

Val& Val::At(i32 i)
{
	if(IsNull())
		*this = ValArray();
	ASSERT(i >= 0 && IsRef());
	dword t = GetRefType();
	if(t == VALUEMAP_V) {
		ValArray& va = ValMap::UnShare((ValMap::Data*&)ptr()).value;
		ASSERT(i < va.GetCount());
		return va.At(i);
	}
	ASSERT(t == VALUEARRAY_V);
	return UnShareArray().At(i);
}

void Val::Add(const Val& s)
{
	if(IsNull()) {
		if(IsRef()) RefRelease();
		ValArray::Data *d = new ValArray::Data;
		d->data.Add(s);
		InitRef(d, VALUEARRAY_V);
		Magic();
		return;
	}
	ASSERT(IsRef() && GetRefType() == VALUEARRAY_V);
	UnShareArray().Add(s);
}

const Val& Val::operator[](const Txt& key) const
{
	if(IsRef() && GetRefType() == VALUEMAP_V)
		return ((ValMap::Data *)ptr())->Get(key);
	return ErrorVal();
}

Val& Val::GetAdd(const Val& key)
{
	if(IsNull()) {
		if(IsRef()) RefRelease();
		ValMap::Data *d = new ValMap::Data;
		Val& h = d->GetAdd(key);
		InitRef(d, VALUEMAP_V);
		Magic();
		return h;
	}
	if(GetType() == VALUEARRAY_V) {
		ValMap m = *this;
		*this = m;
	}
	ASSERT(GetType() == VALUEMAP_V);
	return ValMap::UnShare((ValMap::Data*&)ptr()).GetAdd(key);
}

Val& Val::operator()(const Txt& key)
{
	return GetAdd(key);
}

Val& Val::operator()(tukk key)
{
	return GetAdd(key);
}

Val& Val::operator()(const Id& key)
{
	return GetAdd(~key);
}

Txt Val::GetName() const
{
	if(IsRef()) {
		Void *p = ptr(); // supress CLANG warning
		return typeid(*p).name();
	}
	if(IsTxt())
		return "Txt";
	static Tuple<u8, tukk > tp[] = {
		{ (u8)INT_V, "i32" },
		{ (u8)DOUBLE_V, "double" },
		{ (u8)VOIDV, "void" },
		{ (u8)DATE_V, "Date" },
		{ (u8)TIME_V, "Time" },
		{ (u8)INT64_V, "z64" },
		{ (u8)BOOL_V, "bool" },
	};
	Tuple<u8, tukk > *x = FindTuple(tp, __countof(tp), data.GetSpecial());
	return x ? Txt(x->b) : AsTxt(GetType());
}

class ValErrorCls : public RichValRep<Txt> {
public:
	virtual dword      GetType() const             { return ERROR_V; }
	virtual bool       IsNull() const              { return true; }
	virtual void       Serialize(Stream& s)        {}
	virtual Txt     AsTxt() const            { return "<error: \'" + v + "\'>"; }

	ValErrorCls(const Txt& s) : RichValRep<Txt>(s)  {}
};

Val ErrorVal(const Txt& s) {
	return Val(new ValErrorCls(s), ERROR_V);
}

Val ErrorVal(tukk s) {
	return ErrorVal(Txt(s));
}

const Val& ErrorVal() {
	static Val v = ErrorVal(Txt());
	return v;
}

Txt GetErrorText(const Val& v) {
	ASSERT(IsError(v));
	return ((RichValRep<Txt> *)v.GetVoidPtr())->Get();
}

}