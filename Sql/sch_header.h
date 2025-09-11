// struct

#define CODETYPE(Table, b) \
struct S_##Table b { \
private: \
	S_##Table(i32) {} \
	static const S_info&        GetInfo(); \
	static const S_info        *info; \
\
public: \
	static drx::Txt          TableName; \
	static const drx::SqlSet&   ColumnSet()                           { return GetInfo().set; } \
	static drx::SqlSet          ColumnSet(const drx::Txt& prefix)  { return GetInfo().GetSet(prefix); } \
	static drx::SqlSet          Of(drx::SqlId table)                  { return GetInfo().GetOf(table); } \
	static const drx::Vec<drx::SqlId>& GetColumnIds()              { return GetInfo().ids; } \
	\
	void                        Clear(); \
	void                        FieldLayoutRaw(drx::FieldOperator& f, const drx::Txt& prefix = drx::Txt()); \
	void                        FieldLayout(drx::FieldOperator& f); \
	operator                    drx::Fields()                         { return callback(this, &S_##Table::FieldLayout); } \
	bool                        operator==(const S_##Table& x) const  { return drx::EqualFields(const_cast<S_##Table&>(*this), const_cast<S_##Table&>(x)); } \
	bool                        operator!=(const S_##Table& x) const  { return !operator==(x); } \
	drx::Txt                 ToTxt() const                      { return drx::AsTxt((drx::Fields)const_cast<S_##Table&>(*this)); } \
	\
	static i32                  GetCount()                            { return GetInfo().GetCount(); } \
	static SqlId                GetId(i32 i)                          { return GetInfo().GetId(i); } \
	static i32                  GetIndex(const drx::Txt& id)       { return GetInfo().GetIndex(id); } \
	static i32                  GetIndex(const drx::SqlId& id)        { return GetInfo().GetIndex(id); } \
	static i32                  GetWidth(i32 i)                       { return GetInfo().GetWidth(i); } \
	static i32                  GetWidth(const drx::SqlId& id)        { return GetInfo().GetWidth(id); } \
	\
	drx::Ref                    GetRef(i32 i)                         { return info->GetRef(this, i); } \
	drx::Ref                    GetRef(const drx::SqlId& id)          { return info->GetRef(this, id); } \
	drx::Val                  Get(const drx::SqlId& id) const       { return info->Get(this, id); } \
	drx::Val                  Get(i32 i) const                      { return info->Get(this, i); } \
	drx::ValMap               Get() const                           { return info->Get(this); } \
	void                        Set(i32 i, const drx::Val& v)       { return info->Set(this, i, v); } \
	void                        Set(const drx::SqlId& id, const drx::Val& v)  { return info->Set(this, id, v); } \
	void                        Set(const drx::ValMap& m)                     { return info->Set(this, m); } \
	\
	operator                    const S_info&() const                 { return *info; } \
	\
	S_##Table(); \
	S_##Table(const drx::ValMap& m);

#define TYPE(Table)                  CODETYPE(Table, __NIL)
#define TYPE_I(Table, b)             CODETYPE(Table, : public S_##b)
#define TWO_BASES(b1, b2)            : public S_##b1, public S_##b2
#define TYPE_II(Table, b1, b2)       CODETYPE(Table, TWO_BASES(b1, b2));
#define THREE_BASES(b1, b2, b3)      : public S_##b1, public S_##b2, public S_##b3
#define TYPE_III(Table, b1, b2, b3)  CODETYPE(Table, THREE_BASES(b1, b2, b3));

#define VAR(type, x)             S_##type x;

#define COLUMN(type, ctype, name, width, prec) \
enum { ADD_SCHEMA_PREFIX_CPP2(name,_WIDTH) = width, ADD_SCHEMA_PREFIX_CPP2(name,_PRECISION) = prec }; \
ctype ADD_SCHEMA_PREFIX_CPP(name); \
static drx::SqlId colid_##name;

#define COLUMN_ARRAY(type, ctype, name, width, prec, items) \
enum { ADD_SCHEMA_PREFIX_CPP2(name,_WIDTH) = width, ADD_SCHEMA_PREFIX_CPP2(name,_PRECISION) = prec }; \
ctype ADD_SCHEMA_PREFIX_CPP(name)[items]; \
static drx::SqlId colid_##name;
 
#define END_TYPE                 };

#define SQL_NAME(x)

#include SCHEMADIALECT

#undef CODETYPE
#undef TWO_BASES
#undef THREE_BASES

// SqlId

#define DOID(x) extern drx::SqlId ADD_SCHEMA_PREFIX_CPP(x);

#define SQL_NAME(x)

#include SCHEMADIALECT
