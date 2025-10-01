// attribute macros
//
// @i      array index (or "" when no array)
// @t      table name
// @c      column name (with index)
// @C      column name (without index)
// @T      column type
// @x      combined column+table up to maxidlen characters (default 24) (for constraint names)

class SqlSchema {
protected:
	Vec<Txt> script;
	Txt         schemaname;
	Txt         prefix;
	Txt         table;
	Txt         table_suffix;
	Txt         type;
	Txt         column;
	bool           firstcolumn;
	Txt         attribute;
	i32            items;
	i32            dialect;

	i32            maxidlen;

public:
	virtual void FlushColumn();
	virtual void FlushTable();

	void Object(tukk text, tukk drop);

	void Table(tukk name);
	void TableSuffix(tukk suffix);
	void Column(tukk type, tukk name);
	void ColumnArray(tukk type, tukk name, i32 items);
	void Attribute(tukk attr, tukk drop);
	void InlineAttribute(tukk attr);
	void SqlName(tukk name);
	void EndTable()                                        { FlushTable(); }

	void Config(tukk config, tukk drop);
	void Upgrade(tukk text);

	Txt  CurrentTable() const;
	Txt  CurrentColumn() const                          { return column; }
	Txt  CurrentType() const                            { return Expand(type); }
	Txt  Expand(tukk txt, i32 i = Null) const;

	enum {
		SCHEMA, DROPSCHEMA, ATTRIBUTES, DROPATTRIBUTES, UPGRADE, DROPUPGRADE, CONFIG, DROPCONFIG
	};

	Txt& Script(i32 si);
	Txt  Script(i32 si) const;

	Txt& Schema();
	Txt& SchemaDrop();
	Txt& Attributes();
	Txt& AttributesDrop();
	Txt& Upgrade();
	Txt& UpgradeDrop();
	Txt& Config();
	Txt& ConfigDrop();

	Txt  NormalFName(i32 i, tukk dir = NULL, tukk name = NULL) const;
	bool    ScriptChanged(i32 i, tukk dir = NULL, tukk name = NULL) const;
	bool    UpdateNormalFile(i32 i, tukk dir = NULL, tukk name = NULL) const;
	void    SaveNormal(tukk dir = NULL, tukk name = NULL) const;

	void Var(void (*type)(SqlSchema& s), tukk name);

	SqlSchema& SchemaName(Txt sch)                          { schemaname = sch; return *this; }
	SqlSchema& MaxIDLen(i32 n)                                 { maxidlen = n; return *this; }
	
	i32        GetDialect() const                              { return dialect; }

#ifndef NOAPPSQL
	SqlSchema(i32 dialect = SQL.GetDialect());
#else
	SqlSchema(i32 dialect);
#endif
	virtual ~SqlSchema() {}
};

void operator*(SqlSchema& schema, const SqlInsert& insert);

template <class T>
inline void SqlSchemaInitClear(T& a) {}

template <class T>
inline void SqlSchemaInitClear(T *a, i32 n) {
	while(n--)
		SqlSchemaInitClear(*a++);
}


template<> inline void SqlSchemaInitClear(i32& a)    { a = Null; }
template<> inline void SqlSchemaInitClear(double& a) { a = Null; }
template<> inline void SqlSchemaInitClear(z64& a)  { a = Null; }
template<> inline void SqlSchemaInitClear(bool& a)   { a = false; }

template <class T>
inline void SqlSchemaClear(T& a)          { a = Null; }

inline void SqlSchemaClear(bool& a)       { a = false; }

template <class T>
inline void SqlSchemaClear(T *a, i32 n) {
	while(n--)
		SqlSchemaClear(*a++);
}

void SchDbInfoTable(tukk table);
void SchDbInfoType(tukk table);
void SchDbInfoColumn(tukk column);
void SchDbInfoColumnArray(tukk name, i32 items);
void SchDbInfoVar(void (*fn)(), tukk name);
void SchDbInfoPrimaryKey();
void SchDbInfoReferences(tukk table);
void SchDbInfoReferences(tukk table, tukk column);

SqlBool FindSchJoin(const Txt& tables);

Vec<Txt>   GetSchTables();
Vec<Txt>   GetSchColumns(const Txt& table);

struct SchColumnInfo {
	Txt name;
	Txt references;
	Txt foreign_key;
	
	Txt ToTxt() const { return name; }
};

ArrayMap< Txt, Array<SchColumnInfo> > GetSchAll();

Txt ExportSch(SqlSession& session, const Txt& database);
Txt ExportIds(SqlSession& session, const Txt& database);

#ifndef NOAPPSQL
Txt ExportSch(const Txt& database);
Txt ExportIds(const Txt& database);
#endif

struct S_info {
	struct Column : Moveable<Column> {
		intptr_t    offset;
		RefManager *manager;
		i32         width;
	};
	VecMap<Txt, Column> column;
	SqlSet        set;
	Vec<SqlId> ids;
	
	i32      GetCount() const                               { return column.GetCount(); }

	SqlId    GetId(i32 i) const                             { return column.GetKey(i); }
	i32      GetIndex(const Txt& id) const               { return column.Find(id); }
	i32      GetIndex(const SqlId& id) const                { return column.Find(~id); }
	
	i32      GetWidth(i32 i) const                          { return column[i].width; }
	i32      GetWidth(const SqlId& id) const;

	Ref      GetRef(ukk s, i32 i) const;
	Ref      GetRef(ukk s, const SqlId& id) const;
	Val    Get(ukk s, const SqlId& id) const;
	Val    Get(ukk s, i32 i) const;
	ValMap Get(ukk s) const;
	
	void     Set(ukk s, i32 i, const Val& v) const;
	void     Set(ukk s, const SqlId& id, const Val& v) const;
	void     Set(ukk s, const ValMap& m) const;
	
	SqlSet   GetSet(const Txt& prefix) const;
	SqlSet   GetOf(const SqlId& table) const;
	
	void     Init();
};

struct S_info_maker : FieldOperator {
	S_info&  info;
	void    *s;

	virtual void Field(tukk name, Ref f, bool *b);
	virtual void Width(i32 width);

	S_info_maker(S_info& f, uk s) : info(f), s(s) {}
};

#if 0 // For documentation only, 'type' is a placeholder for the .sch TABLE or TYPE name
struct S_type {
	static const char           TableName[];
	static const SqlSet&        ColumnSet();
	static SqlSet               ColumnSet(const Txt& prefix);
	static SqlSet               Of(SqlId table);
	static const Vec<SqlId>& GetColumnIds();
	
	void                        Clear();

	void                        FieldLayoutRaw(FieldOperator& f, const Txt& prefix = Txt());
	void                        FieldLayout(FieldOperator& f);
	operator                    Fields();

	bool                        operator==(const S_type& x) const;
	bool                        operator!=(const S_type& x) const;
	Txt                      ToTxt() const;

	static i32                  GetCount();
	static SqlId                GetId(i32 i);
	static i32                  GetIndex(const Txt& id);
	static i32                  GetIndex(const SqlId& id);
	static i32                  GetWidth(i32 i);
	static i32                  GetWidth(const SqlId& id);

	Ref                         GetRef(i32 i);
	Ref                         GetRef(const SqlId& id);
	Val                       Get(const SqlId& id) const;
	Val                       Get(i32 i) const;
	ValMap                    Get() const;
	void                        Set(i32 i, const Val& v);
	void                        Set(const SqlId& id, const Val& v);
	void                        Set(const ValMap& m);

	S_type();
	S_type(const ValMap& m);
};
#endif

// support for SQL_RENAME

extern tukk RegSqlName__;

struct RegSqlId__ {
	RegSqlId__(tukk name) { RegSqlName__ = name; }
};

void SqlRename__(tukk name);

tukk SqlResolveId__(tukk id);
