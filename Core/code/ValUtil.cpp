#include "../Core.h"

namespace drx {

static Txt sAsTxt(const Vec<Val>& v);

#define LTIMING(x) // RTIMING(x)

struct Ref::ValRef : public RefManager {
	virtual i32   GetType()                            { return VALUE_V; }
	virtual Val GetVal(ukk ptr)            { return *(Val *) ptr; }
	virtual bool  IsNull(ukk ptr)              { return DRX::IsNull(*(Val *) ptr); }
	virtual void  SetVal(uk ptr, const Val& v)  { *(Val *) ptr = v; }
	virtual void  SetNull(uk ptr)                   { *(Val *) ptr = Null; }
};

Ref::Ref(Txt& s)  { ptr = &s; m = &Single< StdRef<Txt> >(); }
Ref::Ref(WTxt& s) { ptr = &s; m = &Single< StdRef<WTxt> >(); }
Ref::Ref(i32& i)     { ptr = &i; m = &Single< StdRef<i32> >(); }
Ref::Ref(z64& i)   { ptr = &i; m = &Single< StdRef<z64> >(); }
Ref::Ref(double& d)  { ptr = &d; m = &Single< StdRef<double> >(); }
Ref::Ref(bool& b)    { ptr = &b; m = &Single< StdRef<bool> >(); }
Ref::Ref(Date& d)    { ptr = &d; m = &Single< StdRef<Date> >(); }
Ref::Ref(Time& t)    { ptr = &t; m = &Single< StdRef<Time> >(); }
Ref::Ref(Val& v)   { ptr = &v; m = &Single< ValRef >(); }

// ----------------------------------

bool ValArray::Data::IsNull() const
{
	return data.IsEmpty();
}

void ValArray::Data::Serialize(Stream& s)
{
	s % data;
}

void ValArray::Data::Jsonize(JsonIO& jio)
{
	drx::Jsonize(jio, data);
}

void ValArray::Data::Xmlize(XmlIO& io)
{
	drx::Xmlize(io, data);
}

hash_t ValArray::Data::GetHashVal() const
{
	CombineHash w(data.GetCount());
	for(i32 i = 0; i < data.GetCount(); i++)
		w.Put(data[i].GetHashVal());
	return w;
}

bool ValArray::Data::IsEqual(const Val::Void *p)
{
	return ((Data *)p)->data == data;
}

i32 ValArray::Data::Compare(const Val::Void *p)
{
	return data.Compare(((Data *)p)->data);
}

bool ValArray::operator==(const ValArray& v) const
{
	return v.data->data == data->data;
}

i32 ValArray::Compare(const ValArray& b) const
{
	return data->data.Compare(b.data->data);
}

static Txt sAsTxt(const Vec<Val>& v)
{
	Txt s;
	s << "[";
	for(i32 i = 0; i < v.GetCount(); i++) {
		if(i) s << ", ";
		s << v[i];
	}
	s << "]";
	return s;
}

Txt ValArray::Data::AsTxt() const
{
	return sAsTxt(data);
}

Vec<Val> ValArray::VoidData;

Vec<Val>& ValArray::Create()
{
	data = new Data;
	return data->data;
}

Vec<Val>& ValArray::Clone() {
	if(data->GetRefCount() != 1) {
		Data *d = new Data;
		d->data = clone(data->data);
		data->Release();
		data = d;
	}
	return data->data;
}

void ValArray::Init0()
{
	data = &Single<NullData>();
	data->Retain();
}

ValArray::ValArray(const ValArray& v) {
	data = v.data;
	data->Retain();
}

ValArray::ValArray(ValArray&& v) {
	data = v.data;
	v.Init0();
}

ValArray::ValArray(Vec<Val>&& v)
{
	Create() = pick(v);
}

ValArray::ValArray(const Vec<Val>& v, i32 deep)
{
	Create() = clone(v);
}

ValArray::operator Val() const {
	data->Retain();
	return Val(data, VALUEARRAY_V);
}

ValArray::ValArray(const Val& src)
{
	if(!DRX::IsNull(src)) {
		if(src.Is<ValMap>()) {
			ValArray v = ValMap(src);
			data = v.data;
			data->Retain();
			return;
		}
		else {
			if(src.GetType() != VALUEARRAY_V)
				throw ValTypeError(Txt().Cat() << "неправильное преобразование значения: "
			                         << src.GetTypeName() << " -> ValArray",
			                         src, VALUEARRAY_V);
			data = (ValArray::Data *)src.GetVoidPtr();
		}
	}
	else
		data = &Single<NullData>();
	data->Retain();
}

void ValArray::Serialize(Stream& s) {
	if(s.IsLoading()) {
		data->Release();
		Create();
	}
	data->Serialize(s);
}

void ValArray::Jsonize(JsonIO& jio)
{
	if(jio.IsLoading()) {
		data->Release();
		Create();
	}
	data->Jsonize(jio);
}

void ValArray::Xmlize(XmlIO& xio)
{
	if(xio.IsLoading()) {
		data->Release();
		Create();
	}
	data->Xmlize(xio);
}

Txt ValArray::ToTxt() const
{
	return sAsTxt(Get());
}

ValArray::~ValArray() {
	ASSERT(data->GetRefCount() > 0);
	data->Release();
}

ValArray& ValArray::operator=(const ValArray& v) {
	v.data->Retain();
	data->Release();
	data = v.data;
	return *this;
}

void ValArray::SetCount(i32 n)
{
	Clone().SetCount(n);
}

void ValArray::SetCount(i32 n, const Val& v)
{
	Clone().SetCount(n, v);
}

void ValArray::Clear() {
	Clone().Clear();
}

void ValArray::Add(const Val& v) {
	Clone().Add(v);
}

void ValArray::Set(i32 i, const Val& v) {
#if !defined(_MSC_VER) || _MSC_VER > 1310
	ASSERT(i >= 0);
	Clone().At(i) = v;
#else
	throw 0;
#endif
}

Val& ValArray::At(i32 i)
{
	return Clone().At(i);
}

void ValArray::Remove(i32 i, i32 count)
{
	Clone().Remove(i, count);
}

void ValArray::Remove(const Vec<i32>& ii)
{
	Clone().Remove(ii);
}

void ValArray::Insert(i32 i, const ValArray& va)
{
	if(va.data == data) {
		ValArray va2 = va;
		Clone().Insert(i, va2.Get());
	}
	else
		Clone().Insert(i, va.Get());
}

const Val& ValArray::Get(i32 i) const {
	ASSERT(i >= 0 && i < GetCount());
	return data->data[i];
}

Val ValArray::GetAndClear(i32 i)
{
	ASSERT(i >= 0 && i < GetCount());
	Vec<Val>& x = Clone();
	Val v = x[i];
	x[i] = Val();
	return v;
}

Vec<Val> ValArray::Pick()
{
	Vec<Val>& x = Clone();
	Vec<Val> r = pick(x);
	x.Clear();
	return r;
}

template<>
Txt AsTxt(const ValArray& v) {
	return sAsTxt(v.Get());
}

bool ValMap::Data::IsNull() const {
	return this == &Single<ValMap::NullData>();
}

void ValMap::Data::Serialize(Stream& s) {
	s % key % value;
	if(key.GetCount() != value.GetCount())
		s.LoadError();
}

void ValMap::Data::Xmlize(XmlIO& xio)
{
	drx::Xmlize(xio, key);
	drx::Xmlize(xio, value);
}

void ValMap::Data::Jsonize(JsonIO& jio)
{
	if(jio.IsStoring()) {
		ValArray va;
		i32 n = min(value.GetCount(), key.GetCount());
		for(i32 i = 0; i < n; i++) {
			ValMap m;
			m.Add("key", StoreAsJsonVal(key[i]));
			m.Add("value", StoreAsJsonVal(value[i]));
			va.Add(m);
		}
		jio.Set(va);
	}
	else {
		Val va = jio.Get();
		key.Clear();
		value.Clear();
		for(i32 i = 0; i < va.GetCount(); i++) {
			Val k, v;
			LoadFromJsonVal(k, va[i]["key"]);
			LoadFromJsonVal(v, va[i]["value"]);
			key.Add(k);
			value.Add(v);
		}
	}
}

hash_t ValMap::Data::GetHashVal() const {
	CombineHash w(key.GetCount());
	for(i32 i = 0; i < key.GetCount(); i++)
		w.Put(key[i].GetHashVal());
	w.Put(value.GetHashVal());
	return w;
}

static bool sIsEqual(const Index<Val>& a, const Index<Val>& b)
{
	if(&a == &b) return true;
	if(a.GetCount() != b.GetCount()) return false;
	for(i32 i = 0; i < a.GetCount(); i++)
		if(a[i] != b[i]) return false;
	return true;
}

bool ValMap::Data::IsEqual(const Val::Void *p)
{
	return sIsEqual(((Data *)p)->key, key) && ((Data *)p)->value == value;
}

bool ValMap::operator==(const ValMap& v) const
{
	return sIsEqual(data->key, v.data->key) && data->value == v.data->value;
}

i32  ValMap::Data::Compare(const Val::Void *p)
{
	Data *b = (Data *)p;
	i32 n = min(key.GetCount(), b->key.GetCount());
	for(i32 i = 0; i < n; i++) {
		i32 q = SgnCompare(key[i], b->key[i]);
		if(q)
			return q;
		q = SgnCompare(value[i], b->value[i]);
		if(q)
			return q;
	}
	return SgnCompare(key.GetCount(), b->key.GetCount());
}

i32 ValMap::Compare(const ValMap& b) const
{
	return data->Compare((Val::Void *)b.data);
}

bool ValMap::IsSame(const ValMap& b) const
{
	for(i32 pass = 0; pass < 2; pass++) {
		const ValMap& m = pass ? *this : b;
		const ValMap& n = pass ? b : *this;
		for(i32 i = 0; i < m.GetCount(); i++)
			if(!n[m.GetKey(i)].IsSame(m.GetVal(i)))
				return false;
	}
	return true;
}

Txt ValMap::Data::AsTxt() const
{
	Txt s;
	s << "{ ";
	for(i32 i = 0; i < key.GetCount(); i++) {
		if(i) s << ", ";
		s << key[i] << ": " << value[i];
	}
	s << " }";
	return s;
}

ValMap::Data& ValMap::Create()
{
	data = new Data;
	return *data;
}

void ValMap::Clone(Data *&ptr)
{
	Data *d = new Data;
	d->key = clone(ptr->key);
	d->value = ptr->value;
	ptr->Release();
	ptr = d;
}

void ValMap::Init0()
{
	data = &Single<NullData>();
	data->Retain();
}

ValMap::ValMap(const ValMap& v)
{
	data = v.data;
	data->Retain();
}

ValMap::ValMap(Index<Val>&& k, Vec<Val>&& v)
{
	Data& d = Create();
	d.key = pick(k);
	d.value = ValArray(pick(v));
}

ValMap::ValMap(VecMap<Val, Val>&& m)
{
	Data& d = Create();
	d.key = m.PickKeys();
	d.value = ValArray(m.PickVals());
}

ValMap::ValMap(const Index<Val>& k, const Vec<Val>& v, i32 deep)
{
	Data& d = Create();
	d.key = clone(k);
	d.value = ValArray(v, 0);
}

ValMap::ValMap(const VecMap<Val, Val>& m, i32 deep)
{
	Data& d = Create();
	d.key = clone(m.GetKeys());
	d.value = ValArray(m.GetVals(), 0);
}

VecMap<Val, Val> ValMap::Pick()
{
	Data& d = UnShare();
	VecMap<Val, Val> m(d.key.PickKeys(), d.value.Pick());
	d.key.Clear();
	return m;
}

ValMap::operator Val() const {
	data->Retain();
	return Val(data, VALUEMAP_V);
}

void ValMap::FromArray(const ValArray& va)
{
	Init0();
	for(i32 i = 0; i < va.GetCount(); i++)
		Add(i, va[i]);
}

ValMap::ValMap(const Val& src)
{
	if(!IsNull(src)) {
		if(src.Is<ValArray>()) {
			FromArray(src);
			return;
		}
		else {
			if(src.GetType() != VALUEMAP_V)
				throw ValTypeError(Txt().Cat() << "Неправильное преобразование значения: "
			                         << src.GetTypeName() << " -> ValMap",
			                         src, VALUEMAP_V);
			data = (ValMap::Data *)src.GetVoidPtr();
		}
	}
	else
		data = &Single<NullData>();
	data->Retain();
}

void ValMap::Serialize(Stream& s) {
	if(s.IsLoading()) {
		data->Release();
		Create();
	}
	data->Serialize(s);
}

void ValMap::Jsonize(JsonIO& jio)
{
	if(jio.IsLoading()) {
		data->Release();
		Create();
	}
	data->Jsonize(jio);
}

void ValMap::Xmlize(XmlIO& xio)
{
	if(xio.IsLoading()) {
		data->Release();
		Create();
	}
	data->Xmlize(xio);
}

ValMap::~ValMap() {
	ASSERT(data->GetRefCount() > 0);
	data->Release();
}

ValMap& ValMap::operator=(const ValMap& v) {
	v.data->Retain();
	data->Release();
	data = v.data;
	return *this;
}

void ValMap::Clear() {
	data->Release();
	Init0();
}

void ValMap::Set(const Val& key, const Val& value)
{
	Data& d = UnShare();
	i32 i = d.key.Find(key);
	if(i >= 0)
		d.value.Set(i, value);
	else {
		d.key.Add(key);
		d.value.Add(value);
	}
}

void ValMap::SetAt(i32 i, const Val& v) {
	UnShare().value.Set(i, v);
}

void ValMap::SetKey(i32 i, const Val& k) {
	UnShare().key.Set(i, k);
}

i32 ValMap::RemoveKey(const Val& key)
{
	Data& d = UnShare();
	Vec<i32> rk;
	i32 q = d.key.Find(key);
	while(q >= 0) {
		rk.Add(q);
		q = d.key.FindNext(q);
	}
	if(rk.GetCount()) {
		Sort(rk);
		d.key.Remove(rk);
		d.value.Remove(rk);
	}
	return rk.GetCount();
}

void ValMap::Remove(i32 i)
{
	Data& d = UnShare();
	d.key.Remove(i);
	d.value.Remove(i);
}

Val ValMap::GetAndClear(const Val& key)
{
	Data& d = UnShare();
	i32 q = d.key.Find(key);
	return q < 0 ? ErrorVal() : d.value.GetAndClear(q);
}

// ----------------------------------

bool StdValPairOrder::operator()(const Val& k1, const Val& v1, const Val& k2, const Val& v2) const
{
	i32 q = StdValCompare(k1, k2, language);
	if(q) return q < 0;
	return StdValCompare(v1, v2, language);
}

bool FnValPairOrder::operator()(const Val& keya, const Val& valuea, const Val& keyb, const Val& valueb) const
{
	return (*fn)(keya, valuea, keyb, valueb) < 0;
}

i32 CompareTxts(const Val& a, const Val& b, const LangInfo& f)
{
	return f.Compare(WTxt(a), WTxt(b));
}

void Complex::Xmlize(XmlIO& xio)
{
	double r, i;
	r = real(); i = imag();
	xio.Attr("real", r).Attr("imag", i);
	*this = C(r, i);
}

void Complex::Jsonize(JsonIO& jio)
{
	double r, i;
	r = real(); i = imag();
	jio("real", r)("imag", i);
	*this = C(r, i);
}

void Complex::Serialize(Stream& s)
{
	double r, i;
	r = real(); i = imag();
	s % r % i;
	*this = C(r, i);
}

template <class T>
static void sReg(tukk name)
{
	if(FitsSvoVal<T>())
		Val::SvoRegister<T>(name);
	else
		Val::Register<T>(name);
}

INITBLOCK
{
	sReg<Point>("Point");
	sReg<Point64>("Point64");
	sReg<Pointf>("Pointf");
	sReg<Size>("Size");
	sReg<Size64>("Size64");
	sReg<Sizef>("Sizef");
	Val::Register<Rect>("Rect");
	Val::Register<Rect64>("Rect64");
	Val::Register<Rectf>("Rectf");
}

}