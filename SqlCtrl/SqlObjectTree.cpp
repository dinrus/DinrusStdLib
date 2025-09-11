#include "SqlCtrl.h"
#include "SqlDlg.h"

namespace drx {

class DlgSqlExport : public WithSqlExportLayout<TopWindow>
{
public:
	typedef DlgSqlExport CLASSNAME;
	DlgSqlExport();

	void    Run(Sql& cursor, Txt command, Txt tablename);
	void    Serialize(Stream& stream);

private:
	void    SyncUI();
	void    Toggle();

private:
	ArrayOption exp;
	enum {
		FMT_TEXT,
		FMT_SQL,
	};
	enum {
		DELIM_TAB,
		DELIM_SEMICOLON,
	};
	Txt recent_file;
};

void RunDlgSqlExport(Sql& cursor, Txt command, Txt tablename)
{
	DlgSqlExport().Run(cursor, command, tablename);
}

DlgSqlExport::DlgSqlExport()
{
	CtrlLayoutOKCancel(*this, "");
	columns.AddColumn(t_("Имя графы"));
	columns.AddColumn(t_("Тип"));
	columns.AddColumn(t_("Ширина"));
	HeaderCtrl::Column& hc = exp.AddColumn(columns, "").Margin(0).HeaderTab();
	hc.SetMargin(0).Fixed(18).SetAlign(ALIGN_CENTER).SetImage(CtrlImg::smallcheck());
	hc.WhenAction = OTVET(Toggle);
	format <<= FMT_TEXT;
	delimiters <<= DELIM_SEMICOLON;
	quote <<= true;
	format <<= OTVET(SyncUI);
}

void DlgSqlExport::SyncUI()
{
	i32 f = ~format;
	delimiters.Enable(f == FMT_TEXT);
	quote.Enable(f == FMT_TEXT);
	identity_insert.Enable(f == FMT_SQL);
	object_name.Enable(f == FMT_SQL);
}

void DlgSqlExport::Toggle()
{
	bool state = true;
	if(columns.IsCursor())
		state = !(bool)columns.Get(3);
	for(i32 i = 0; i < columns.GetCount(); i++)
		columns.Set(i, 3, state);
}

void DlgSqlExport::Run(Sql& cursor, Txt command, Txt tablename)
{
	Title(Nvl(tablename, t_("Запрос SQL")) + t_(" экспорт"));
	object_name <<= tablename;
	if(!cursor.Execute(command)) {
		Exclamation(NFormat(t_("Ошибка при выполнении [* \1%s\1]: \1%s"), command, cursor.GetLastError()));
		return;
	}
	for(i32 i = 0; i < cursor.GetColumns(); i++) {
		const SqlColumnInfo& sci = cursor.GetColumnInfo(i);
		Txt type;
		switch(sci.type) {
			case BOOL_V:
			case INT_V: type = t_("целое"); break;
			case DOUBLE_V: type = t_("реальное число"); break;
			case TXT_V:
			case WTXT_V: type = t_("ткст"); break;
			case DATE_V: type = t_("дата"); break;
			case TIME_V: type = t_("дата/время"); break;
			case /*ORA_BLOB_V*/-1: type = t_("БЛОБ"); break;
			case /*ORA_CLOB_V*/-2: type = t_("КЛОБ"); break;
			default: type = FormatInt(sci.type); break;
		}
		columns.Add(sci.name, sci.type, sci.width, 1);
	}
	static Txt cfg;
	LoadFromTxt(*this, cfg);
	SyncUI();
	while(TopWindow::Run() == IDOK)
		try {
			Txt out_table = ~object_name;
			Txt delim;
			switch((i32)~delimiters) {
				case DELIM_TAB: delim = "\t"; break;
				case DELIM_SEMICOLON: delim = ";"; break;
			}
			Vec<i32> out;
			Txt colstr;
			Txt title;
			for(i32 i = 0; i < columns.GetCount(); i++)
				if(columns.Get(i, 3)) {
					out.Add(i);
					Txt cname = cursor.GetColumnInfo(i).name;
					colstr << (i ? ", " : "") << cname;
					if(i) title << delim;
					title << cname;
				}
			if(out.IsEmpty()) {
				throw Exc(t_("Колонки не выбраны!"));
				continue;
			}
			Txt rowbegin, rowend;
			i32 fmt = ~format;
			FileSel fsel;
			Txt ext;
			switch(fmt) {
				case FMT_TEXT: {
					rowend = "";
					ext = ".txt";
					fsel.Type(t_("Текстовые файлы (*.txt)"), "*.txt");
					break;
				}
				case FMT_SQL: {
					if(identity_insert)
						rowbegin << "set identity_insert " << out_table << " on ";
					rowbegin << "insert into " << out_table << "(" << colstr << ") values (";
					rowend = ");";
					ext = ".sql";
					fsel.Type(t_("Сценарии SQL (*.sql)"), "*.sql");
					break;
				}
			}
			fsel.AllFilesType().DefaultExt(ext.Mid(1));
			if(!IsNull(recent_file))
				fsel <<= ForceExt(recent_file, ext);
			if(!fsel.ExecuteSaveAs(t_("Сохранить экспорт как")))
				continue;
			recent_file = ~fsel;
			FileOut fo;
			if(!fo.Open(recent_file)) {
				Exclamation(NFormat(t_("Ошибка при создании файла [* \1%s\1]."), recent_file));
				continue;
			}
			if(fmt == FMT_TEXT)
				fo.PutLine(title);
			Progress progress(t_("Экспортируется ряд %d"));
			while(cursor.Fetch()) {
				Txt script = rowbegin;
				for(i32 i = 0; i < out.GetCount(); i++) {
					Val v = cursor[out[i]];
					switch(fmt) {
						case FMT_TEXT: {
							if(i)
								script.Cat(delim);
							if(IsTxt(v) && quote) {
								Txt s = v;
								script << '\"';
								for(tukk p = s, *e = s.End(); p < e; p++)
									if(*p == '\"')
										script.Cat("\"\"");
									else
										script.Cat(*p);
								script << '\"';
							}
							else
								script << StdFormat(v);
							break;
						}
						case FMT_SQL: {
							if(i) script.Cat(", ");
							script << SqlCompile(cursor.GetDialect(), SqlFormat(v));
							break;
						}
					}
				}
				script << rowend;
				fo.PutLine(script);
/*
				if(autocommit && --left <= 0) {
					fo.PutLine("commit;");
					left = autocommit;
				}
*/
				if(progress.StepCanceled()) {
					Exclamation(t_("Экспорт оборван!"));
					return;
				}
			}
			fo.Close();
			if(fo.IsError())
				throw Exc(NFormat(t_("Ошибка при записи файла %s."), recent_file));
			break;
		}
		catch(Exc e) {
			ShowExc(e);
		}

	cfg = StoreAsTxt(*this);
}

void DlgSqlExport::Serialize(Stream& stream)
{
	i32 version = 1;
	stream / version % format % recent_file;
}

class SqlObjectTree : public TopWindow {
public:
	typedef SqlObjectTree CLASSNAME;
	SqlObjectTree(SqlSession& session);

	void        Run();

private:
	enum OBJTYPE {
		OBJ_NULL,
		OBJ_SCHEMA,
		OBJ_SCHEMA_OBJECTS,
		OBJ_TABLES,
		OBJ_VIEWS,
		OBJ_SEQUENCES,
		OBJ_TABLE_COLUMNS,
		OBJ_SEQUENCE,
		OBJ_COLUMN,
		OBJ_PRIMARY_KEY,
		OBJ_ROWID,
	};
	struct Item {
		Item(OBJTYPE type = OBJ_NULL, Txt schema = Null, Txt object = Null)
		: type(type), schema(schema), object(object) {}
		Item(const Val& v) { if(IsTypeRaw<Item>(v)) *this = ValTo<Item>(v); }
		operator Val() const { return RawToVal(*this); }

		i32 type;
		Txt schema;
		Txt object;
	};

	void        Open(i32 node);
	void        OpenSchema(i32 node);
	void        OpenTables(i32 node, const Item& item);
	void        OpenViews(i32 node, const Item& item);
	void        OpenSequences(i32 node, const Item& item);
	void        OpenTableColumns(i32 node, const Item& item);

	void        ToolLocal(Bar& bar);
	void        TableExport(Txt table_name);

private:
	TreeCtrl    schema;
	SqlSession& session;
};

void SQLObjectTree(SqlSession& session) { SqlObjectTree(session).Run(); }

SqlObjectTree::SqlObjectTree(SqlSession& sess)
: session(sess)
{
	Title(t_("Дерево объектов SQL"));
	Sizeable().MaximizeBox();
	Add(schema.SizePos());
	schema.WhenOpen = OTVET(Open);
	schema.SetRoot(CtrlImg::Computer(), Item(OBJ_SCHEMA), t_("Схемы"));
	schema.WhenBar = OTVET(ToolLocal);
}

void SqlObjectTree::Run() {
	TopWindow::Run();
}

void SqlObjectTree::Open(i32 node)
{
	const Item& item = ValTo<Item>(schema[node]);
	switch(item.type) {
		case OBJ_SCHEMA:         OpenSchema(node); break;
		case OBJ_TABLES:         OpenTables(node, item); break;
		case OBJ_VIEWS:          OpenViews(node, item); break;
		case OBJ_SEQUENCES:      OpenSequences(node, item); break;
		case OBJ_TABLE_COLUMNS:  OpenTableColumns(node, item); break;
	}
}

void SqlObjectTree::OpenSchema(i32 node) {
	try {
		schema.RemoveChildren(node);
		Vec<Txt> schemas = session.EnumDatabases();
		Sort(schemas);
		for(i32 i = 0; i < schemas.GetCount(); i++) {
			Txt sname = schemas[i];
			i32 snode = schema.Add(node, CtrlImg::Dir(), Item(OBJ_SCHEMA_OBJECTS, sname), sname, true);
			schema.Add(snode, CtrlImg::Dir(), Item(OBJ_TABLES, sname), t_("Таблицы"), true);
			schema.Add(snode, CtrlImg::Dir(), Item(OBJ_VIEWS, sname), t_("Обзоры"), true);
			schema.Add(snode, CtrlImg::Dir(), Item(OBJ_SEQUENCES, sname), t_("Последовательности"), true);
		}
	}
	catch(Exc e) {
		ShowExc(e);
	}
}

void SqlObjectTree::OpenTables(i32 node, const Item& item)
{
	try {
		schema.RemoveChildren(node);
		Vec<Txt> tables = session.EnumTables(item.schema);
		Sort(tables);
		for(i32 i = 0; i < tables.GetCount(); i++)
			schema.Add(node, CtrlImg::File(),
				Item(OBJ_TABLE_COLUMNS, item.schema, tables[i]),
				tables[i], true);
	} catch(Exc e) { ShowExc(e); }
}

void SqlObjectTree::OpenViews(i32 node, const Item& item) {
	try {
		schema.RemoveChildren(node);
		Vec<Txt> views = session.EnumViews(item.schema);
		Sort(views);
		for(i32 i = 0; i < views.GetCount(); i++)
			schema.Add(node, CtrlImg::File(),
				Item(OBJ_TABLE_COLUMNS, item.schema, views[i]),
				views[i], true);
	} catch(Exc e) { ShowExc(e); }
}

void SqlObjectTree::OpenSequences(i32 node, const Item& item) {
	try {
		schema.RemoveChildren(node);
		Vec<Txt> sequences = session.EnumSequences(item.schema);
		Sort(sequences);
		for(i32 i = 0; i < sequences.GetCount(); i++)
			schema.Add(node, CtrlImg::File(),
				Item(OBJ_SEQUENCE, item.schema, sequences[i]),
				sequences[i], true);
	} catch(Exc e) { ShowExc(e); }
}

void SqlObjectTree::OpenTableColumns(i32 node, const Item& item)
{
	try {
		schema.RemoveChildren(node);
		Vec<SqlColumnInfo> columns = session.EnumColumns(item.schema, item.object);
		for(i32 i = 0; i < columns.GetCount(); i++)
			schema.Add(node, CtrlImg::Hd(), Item(OBJ_COLUMN, item.schema, item.object), (Val)columns[i].name);
		Vec<Txt> pk = session.EnumPrimaryKey(item.schema, item.object);
		if(!pk.IsEmpty()) {
			Txt pklist;
			for(i32 i = 0; i < pk.GetCount(); i++)
				pklist << (i ? "; " : t_("Первичный ключ: ")) << pk[i];
			schema.Add(node, Null, Item(OBJ_PRIMARY_KEY, item.schema, item.object), (Val)pklist);
		}
		Txt rowid = session.EnumRowID(item.schema, item.object);
		if(!IsNull(rowid))
			schema.Add(node, Null, Item(OBJ_ROWID, item.schema, item.object), Val("RowID: " + rowid));
	} catch(Exc e) { ShowExc(e); }
}

void SqlObjectTree::ToolLocal(Bar& bar)
{
	if(schema.IsCursor()) {
		const Item& item = ValTo<Item>(~schema);
		switch(item.type) {
			case OBJ_TABLE_COLUMNS: {
				bar.Add(t_("Экспорт"), OTVET1(TableExport, item.schema + "." + item.object));
			}
		}
	}
}

void SqlObjectTree::TableExport(Txt table_name)
{
	Txt schema, table = table_name;
	i32 f = table.Find('.');
	if(f >= 0) {
		schema = table.Left(f);
		table = table.Mid(f + 1);
	}
	Vec<SqlColumnInfo> info = session.EnumColumns(schema, table);
	Sql cursor(session);
	RunDlgSqlExport(cursor, "select * from " + schema + "." + table, table);
}

}
