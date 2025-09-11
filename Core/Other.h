template <class T, class... Args>
T& Single(Args... args) {
	static T o(args...);
	return o;
}

template <class T> // Workaround for GCC bug - specialization needed...
T& Single() {
	static T o;
	return o;
}

i32 RegisterTypeNo__(tukk type);

template <class T>
i32 StaticTypeNo() {
	static i32 typeno = RegisterTypeNo__(typeid(T).name());
	return typeno;
}

class Any : Moveable<Any> {
	struct BaseData {
		i32      typeno;

		virtual ~BaseData() {}
	};

	template <class T>
	struct Data : BaseData {
		T        data;

		template <class... Args>
		Data(Args&&... args) : data(std::forward<Args>(args)...) { typeno = StaticTypeNo<T>(); }
	};

	BaseData *ptr;

	void Chk() const                              { ASSERT(ptr != (uk )1); }
	void Pick(Any&& s)                            { ptr = s.ptr; const_cast<Any&>(s).ptr = NULL; }

public:
	template <class T, class... Args> T& Create(Args&&... args) { Clear(); Data<T> *x = new Data<T>(std::forward<Args>(args)...); ptr = x; return x->data; }
	template <class T> bool Is() const            { return ptr && ptr->typeno == StaticTypeNo<T>(); }
	template <class T> T& Get()                   { ASSERT(Is<T>()); Chk(); return ((Data<T>*)ptr)->data; }
	template <class T> const T& Get() const       { ASSERT(Is<T>()); Chk(); return ((Data<T>*)ptr)->data; }

	void Clear()                                  { if(ptr) delete ptr; ptr = NULL; }

	bool IsEmpty() const                          { return ptr == NULL; }

	void operator=(Any&& s)                       { if(this != &s) { Clear(); Pick(pick(s)); } }
	Any(Any&& s)                                  { Pick(pick(s)); }

	Any(const Any& s) = delete;
	void operator=(const Any& s) = delete;

	Any()                                         { ptr = NULL; }
	~Any()                                        { Clear(); }
};

class Bits : Moveable<Bits> {
	i32         alloc;
	dword      *bp;

	void Expand(i32 q);
	void Realloc(i32 nalloc);
	i32  GetLast() const;

public:
	void   Clear();
	void   Set(i32 i, bool b = true) { ASSERT(i >= 0 && alloc >= 0); i32 q = i >> 5; if(q >= alloc) Expand(q);
	                                   i &= 31; bp[q] = (bp[q] & ~(1 << i)) | (b << i); }
	bool   Get(i32 i) const        { ASSERT(i >= 0 && alloc >= 0); i32 q = i >> 5;
	                                 return q < alloc ? bp[q] & (1 << (i & 31)) : false; }
	bool   operator[](i32 i) const { return Get(i); }

	void   Set(i32 i, dword bits, i32 count);
	dword  Get(i32 i, i32 count);
	void   Set64(i32 i, zu64 bits, i32 count);
	zu64   Get64(i32 i, i32 count);

	void   SetN(i32 i, bool b, i32 count);
	void   SetN(i32 i, i32 count)         { SetN(i, true, count); }

	void   Reserve(i32 nbits);
	void   Shrink();

	Txt    ToTxt() const;

	dword       *CreateRaw(i32 n_dwords);
	const dword *Raw(i32& n_dwords) const { n_dwords = alloc; return bp; }
	dword       *Raw(i32& n_dwords)       { n_dwords = alloc; return bp; }

	void         Serialize(Stream& s);

	Bits()                                { bp = NULL; alloc = 0; }
	~Bits()                               { Clear(); }

	Bits(Bits&& b)                        { alloc = b.alloc; bp = b.bp; b.bp = NULL; }
	void operator=(Bits&& b)              { if(this != &b) { Clear(); alloc = b.alloc; bp = b.bp; b.bp = NULL; } }

	Bits(const Bits&) = delete;
	void operator=(const Bits&) = delete;
};

class PackedData {
	uk ptr = nullptr;

	template <class T>
	T Get(i32 ii, T def) const;

public:
	void   SetRawPtr(uk p)                   { ptr = p; }
	void  *GetRawPtr() const                    { return ptr; }

	void   SetData(i32 ii, ukk data, i32 datalen);

	template <class F>
	bool   GetData(i32 ii, F out) const;

	void   SetNull(i32 ii)                      { SetData(ii, NULL, 0); }

	void   SetTxt(i32 ii, tukk s)     { SetData(ii, s, (i32)strlen(s)); }
	void   SetTxt(i32 ii, const Txt& s)   { SetData(ii, s, s.GetCount()); }
	Txt    GetTxt(i32 ii) const              { Txt r; GetData(ii, [&](tukk s, i32 n) { r = Txt(s, n); }); return r; }

	void   SetInt(i32 ii, i32 val)              { SetData(ii, &val, sizeof(i32)); }
	i32    GetInt(i32 ii, i32 def) const        { return Get<i32>(ii, def); }

	void   SetDword(i32 ii, dword val)          { SetData(ii, &val, sizeof(dword)); }
	i32    GetDword(i32 ii, dword def) const    { return Get<dword>(ii, def); }

	void   SetInt64(i32 ii, z64 val)          { SetData(ii, &val, sizeof(z64)); }
	z64    GetInt64(i32 ii, z64 def) const    { return Get<z64>(ii, def); }

	void   SetPtr(i32 ii, uk val)            { SetData(ii, &val, sizeof(uk )); }
	void  *GetPtr(i32 ii) const                 { return Get<uk >(ii, nullptr); }

	void   Clear();

	Vec<Txt>    Unpack() const;
	size_t      GetPackedSize() const;
	Txt         GetPacked() const           { return Txt((tukk )ptr, (i32)GetPackedSize()); }

	PackedData() {}
	PackedData(const PackedData&) = delete;
	~PackedData();
};

template <class T, i32 N = 1>
struct Link {
	T *link_prev[N];
	T *link_next[N];

protected:
	void LPN(i32 i)                      { link_prev[i]->link_next[i] = link_next[i]->link_prev[i] = (T *)this; }

public:
	NOUBSAN	T *GetPtr()                  { return (T *) this; }
	const T *GetPtr() const              { return (const T *) this; }
	T       *GetNext(i32 i = 0)          { return link_next[i]; }
	T       *GetPrev(i32 i = 0)          { return link_prev[i]; }
	const T *GetNext(i32 i = 0) const    { return link_next[i]; }
	const T *GetPrev(i32 i = 0) const    { return link_prev[i]; }

	NOUBSAN	void LinkSelf(i32 i = 0)     { link_next[i] = link_prev[i] = (T *)this; }
	void LinkSelfAll()                   { for(i32 i = 0; i < N; i++) LinkSelf(i); }
	void Unlink(i32 i = 0)               { link_next[i]->link_prev[i] = link_prev[i]; link_prev[i]->link_next[i] = link_next[i];
	                                       LinkSelf(i); }
	void UnlinkAll()                     { for(i32 i = 0; i < N; i++) Unlink(i); }
	NOUBSAN	void LinkBefore(Link *n, i32 i = 0)  { link_next[i] = (T *)n; link_prev[i] = link_next[i]->link_prev[i]; LPN(i); }
	NOUBSAN	void LinkAfter(Link *p, i32 i = 0)   { link_prev[i] = (T *)p; link_next[i] = link_prev[i]->link_next[i]; LPN(i); }

	T   *InsertNext(i32 i = 0)           { T *x = new T; x->LinkAfter(this, i); return x; }
	T   *InsertPrev(i32 i = 0)           { T *x = new T; x->LinkBefore(this, i); return x; }

	void DeleteList(i32 i = 0)           { while(link_next[i] != GetPtr()) delete link_next[i]; }

	bool InList(i32 i = 0) const         { return link_next[i] != GetPtr(); }
	bool IsEmpty(i32 i = 0) const        { return !InList(i); }

	Link()                               { LinkSelfAll(); }
	~Link()                              { UnlinkAll(); }

private:
	Link(const Link&);
	void operator=(const Link&);

public:
#ifdef _DEBUG
	void Dump() {
		for(T *t = GetNext(); t != this; t = t->GetNext())
			LOG(t);
		LOG("-------------------------------------");
	}
#endif
};

template <class T, i32 N = 1>
class LinkOwner : public Link<T, N> {
public:
	~LinkOwner()                         { Link<T, N>::DeleteList(); }
};

template <class T, class K = Txt>
class LRUCache {
public:
	struct Maker {
		virtual K      Key() const = 0;
		virtual i32    Make(T& object) const = 0;
		virtual ~Maker() {}
	};

private:
	struct Item : Moveable<Item> {
		i32    prev, next;
		i32    size;
		One<T> data;
		bool   flag;
	};

	struct Key : Moveable<Key> {
		K            key;
		Txt       type;

		bool operator==(const Key& b) const { return key == b.key && type == b.type; }
		hash_t GetHashVal() const { return CombineHash(key, type); }
	};

	Index<Key>   key;
	Vec<Item> data;
	i32  head;

	i32  size;
	i32  count;

	i32  foundsize;
	i32  newsize;
	bool flag = false;

	i32k InternalSize = 3 * (sizeof(Item) + sizeof(Key) + 24) / 2;

	void Unlink(i32 i);
	void LinkHead(i32 i);

public:
	i32  GetSize() const            { return size; }
	i32  GetCount() const           { return count; }

	template <class P> void AdjustSize(P getsize);

	T&       GetLRU();
	const K& GetLRUKey();
	void     DropLRU();
	void     Shrink(i32 maxsize, i32 maxcount = 30000);
	void     ShrinkCount(i32 maxcount = 30000)          { Shrink(INT_MAX, maxcount); }

	template <class P> i32  Remove(P predicate);
	template <class P> bool RemoveOne(P predicate);

	template <class B, class A>
	T&   Get(const Maker& m, B before_make, A after_make);
	T&   Get(const Maker& m) { return Get(m, []{}, []{}); }

	void Clear();

	void ClearCounters();
	i32  GetFoundSize() const       { return foundsize; }
	i32  GetNewSize() const         { return newsize; }

	LRUCache() { head = -1; size = 0; count = 0; ClearCounters(); }
};

#include "Other.hpp"