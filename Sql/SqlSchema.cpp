#include "Sql.h"

namespace drx {

void SqlSchema::FlushColumn() {
	if(column.IsEmpty()) return;
	for(i32 i = items ? 0 : Null; items ? i < items : IsNull(i); i++) {
		Txt cn = Format("\t%-20s ", ~Expand("@c", i)) + CurrentType();
		if(dialect == SQLITE3)
			cn = Format(" %s ", ~Expand("@c", i)) + CurrentType();
		Txt attr = Expand(attribute, i);
		Txt cd = cn + attr;
		if(firstcolumn) {
			Schema() << Expand("create table @t (\n");
			SchemaDrop() << Expand("drop table @t;\n");
			if(dialect == SQLITE3)
				Upgrade() << Expand("create table @t ( ") << cd << " )" << table_suffix << ";\n";
			else
				Upgrade() << Expand("create table @t (\n") << cd << "\n)" << table_suffix << ";\n\n";
		}
		else
		{
			if(dialect == MSSQL) {
				Upgrade() << Expand("alter table @t add ") << cn << ";\n";
				Upgrade() << Expand("alter table @t alter column ") << cd << ";\n";
			}
			else
			if(dialect == MY_SQL) {
				Upgrade() << Expand("alter table @t add ") << cn << ";\n";
				Upgrade() << Expand("alter table @t modify ") << cd << ";\n";
			}
			else
			if (dialect == SQLITE3 || dialect == MY_SQL)
				Upgrade() << Expand("alter table @t add ") << cd << ";\n";
			else
			if (dialect == PGSQL)
				Upgrade() << Expand("alter table @t add \n") << cd << "\n;\n\n";
			else
				Upgrade() << Expand("alter table @t add (\n") << cd << "\n);\n\n";
			Schema() << ",\n";
		}
		Schema() << cd;
		firstcolumn = false;
	}
	column.Clear();
}

void SqlSchema::FlushTable() {
	FlushColumn();
	if(!table.IsEmpty())
		Schema() << "\n)" << table_suffix << ";\n\n";
	table.Clear();
}

void SqlSchema::Object(tukk text, tukk drop) {
	FlushTable();
	Schema() << text << '\n';
	SchemaDrop() << drop << '\n';
}

void SqlSchema::Upgrade(tukk text) {
	Upgrade() << text << '\n';
}

void SqlSchema::Table(tukk name) {
	FlushTable();
	table = name;
	table_suffix = Null;
	Attributes() << '\n';
	AttributesDrop() << '\n';
	firstcolumn = true;
}

void SqlSchema::TableSuffix(tukk suffix)
{
	table_suffix << suffix;
}

void SqlSchema::Column(tukk _type, tukk name) {
	FlushColumn();

	items = 0;
	type = _type;
	column = prefix + name;
	attribute.Clear();
}

void SqlSchema::SqlName(tukk name)
{
	(firstcolumn ? table : column) = prefix + name;
}

void SqlSchema::ColumnArray(tukk type, tukk name, i32 _items) {
	Column(type, name);
	items = _items;
}

void SqlSchema::Attribute(tukk attr, tukk drop) {
	for(i32 i = items ? 0 : Null; items ? i < items : IsNull(i); i++) {
		Attributes() << Expand(attr, i) << '\n';
		AttributesDrop() << Expand(drop, i) << '\n';
	}
}

void SqlSchema::InlineAttribute(tukk attr) {
	attribute << (*attr == ',' ? "" : " ") << Expand(attr);
}

void SqlSchema::Config(tukk config, tukk drop) {
	Config() << config;
	ConfigDrop() << drop;
}

Txt SqlSchema::CurrentTable() const
{
	return IsNull(schemaname) ? table : schemaname + '.' + table;
}

Txt SqlSchema::Expand(tukk txt, i32 i) const
{
	Txt n, r;
	if(!IsNull(i))
		n = Format("%d", i);
	Txt c = CurrentColumn() + n;
	Txt v = table + '$' + c;
	v = v.Mid(max(0, min(v.GetLength(), v.GetLength() - maxidlen)));
	while(*txt) {
		if(*txt == '@' && txt[1]) {
			switch(*++txt) {
			case 'i': r.Cat(n); break;
			case 't': r.Cat(CurrentTable()); break;
			case 'u': r.Cat(table); break;
			case 'c': r.Cat(c); break;
			case 'C': r.Cat(CurrentColumn()); break;
			case 'T': r.Cat(CurrentType()); break;
			case 'x': r.Cat(v); break;
			case 's': if(!IsNull(schemaname)) { r.Cat(schemaname); r.Cat('.'); } break;
			default:  r.Cat(*txt); break;
			}
		}
		else
			r.Cat(*txt);
		txt++;
	}
	return r;
}

void SqlSchema::Var(void (*type)(SqlSchema& s), tukk name) {
	Txt p = prefix;
	prefix << name << '$';
	(*type)(*this);
	prefix = p;
}

Txt& SqlSchema::Script(i32 si)   { return script.At(si); }
Txt  SqlSchema::Script(i32 si) const  { return si < script.GetCount() ? script[si] : Txt(); }
Txt& SqlSchema::Schema()         { return Script(SCHEMA); }
Txt& SqlSchema::SchemaDrop()     { return Script(DROPSCHEMA); }
Txt& SqlSchema::Attributes()     { return Script(ATTRIBUTES); }
Txt& SqlSchema::AttributesDrop() { return Script(DROPATTRIBUTES); }
Txt& SqlSchema::Upgrade()        { return Script(UPGRADE); }
Txt& SqlSchema::UpgradeDrop()    { return Script(DROPUPGRADE); }
Txt& SqlSchema::Config()         { return Script(CONFIG); }
Txt& SqlSchema::ConfigDrop()     { return Script(DROPCONFIG); }

Txt SqlSchema::NormalFName(i32 i, tukk dir, tukk name) const {
	ASSERT(i >= 0 && i <= DROPCONFIG);
	Txt n;
	if(name)
		n = name;
	else
#ifdef TARGET_WIN32
		n = ForceExt(GetFName(GetExeFilePath()), ".sql");
#endif
#ifdef TARGET_POSIX
		n = "script.sql";
#endif
	tukk pfx[] = {
		"S_", "SD_", "A_", "AD_", "U_", "UD_", "C_", "CD_"
	};
	return dir ? AppendFName(dir, pfx[i] + n) : ConfigFile(pfx[i] + n);
}

bool SqlSchema::ScriptChanged(i32 i, tukk dir, tukk name) const {
	Txt fn = NormalFName(i, dir, name);
	return LoadFile(fn) != Script(i);
}

bool SqlSchema::UpdateNormalFile(i32 i, tukk dir, tukk name) const {
	Txt fn = NormalFName(i, dir, name);
	if(LoadFile(fn) != Script(i)) {
		DeleteFile(fn);
		SaveFile(fn, Script(i));
		return true;
	}
	return false;
}

void SqlSchema::SaveNormal(tukk dir, tukk name) const {
	for(i32 i = SCHEMA; i <= DROPCONFIG; i++)
		UpdateNormalFile(i, dir, name);
}

SqlSchema::SqlSchema(i32 dialect_)
{
	dialect = dialect_;
	maxidlen = 63;
	if(dialect == MSSQL)
		maxidlen = 128;
	if(dialect == ORACLE)
		maxidlen = 24;
}

void operator*(SqlSchema& schema, const SqlInsert& insert) {
	schema.Config() << SqlStatement(insert).Get(schema.GetDialect()) << ";\n";
	schema.ConfigDrop() << SqlStatement(Delete(insert.GetTable())
		                                .Where(insert.GetKeyColumn() == insert.GetKeyVal()))
		                   .Get(schema.GetDialect())
	                    << ";\n";
}

VecMap<Txt, Txt>& sSqlRename()
{
	static VecMap<Txt, Txt> x;
	return x;
}

tukk RegSqlName__;

void SqlRename__(tukk name)
{
	static auto& rename = sSqlRename();
	if(RegSqlName__)
		rename.Add(RegSqlName__, name);
	RegSqlName__ = NULL;
}

tukk SqlResolveId__(tukk id)
{
	static auto& rename = sSqlRename();
	i32 q;
	if(rename.GetCount() && (q = rename.Find(id)) >= 0)
		return rename[q];
	return id; // cannot be conditional expression as we are returning tukk  !
}

};