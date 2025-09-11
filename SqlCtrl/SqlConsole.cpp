#include "SqlCtrl.h"
#include "SqlDlg.h"
#include <drx/Report/Report.h>
#include <drx/CodeEditor/CodeEditor.h>

namespace drx {

#define IMAGEFILE <drx/SqlCtrl/SqlCtrl.iml>
#define IMAGECLASS SqlConsoleImg
#include <drx/Draw/iml.h>

static bool scmpw(u8k *s, tukk w) {
    for(;;) {
        if(*w == '\0')
            return *s == ' ' || *s == '\t';
        if(ToUpper(*s++) != ToUpper(*w++))
            return false;
    }
}

static i32k MAX_NESTING_LEVEL = 10;
static i32k PROGRESS_RANGE = 1000000000;

bool SqlRunScript(i32 dialect, Stream& script_stream,
    Gate<Txt> executor, Gate<i32, i32> progress,
    Index<Txt>& script_file_names, i32 progress_min, i32 progress_max)
{
    i32 line_number = 1;
    for(i32 c; (c = script_stream.Term()) >= 0;) {
        if(c <= ' ') {
            if(c == '\n')
                line_number++;
            script_stream.Get();
        }
        else if(c == '@') {
            script_stream.Get();
            z64 start = script_stream.GetPos() - 1;
            bool rel = false;
            if(script_stream.Term() == '@') {
                script_stream.Get();
                rel = true;
            }
            TxtBuf subscript_fn;
            while((c = script_stream.Get()) >= 0 && c != '\n')
                subscript_fn.Cat((char)c);
            i32 e = subscript_fn.GetLength();
            while(e > 0 && subscript_fn[e - 1] <= ' ')
                e--;
            Txt fn(~subscript_fn, e);
            fn = NormalizePath(fn, GetFileDir(script_file_names[rel ? script_file_names.GetCount() - 1 : 0]));
            Txt norm = fn;
#if !TARGET_PATH_HAS_CASE
            norm = ToLower(norm);
#endif
            if(script_file_names.Find(norm) >= 0)
                throw Exc(NFormat(t_("циркулярное включение сценария %s в %s:%d"),
                    fn, script_file_names.Top(), line_number));
            if(script_file_names.GetCount() >= MAX_NESTING_LEVEL)
                throw Exc(NFormat(t_("слишком глубокое гнездование сценария (%d уровней)"), script_file_names.GetCount()));
            z64 end = script_stream.GetPos();

            i32 new_min = progress_min + (i32)((progress_max - progress_min) * start / script_stream.GetSize());
            i32 new_max = progress_min + (i32)((progress_max - progress_min) * end / script_stream.GetSize());

            FileIn fi;
            if(!fi.Open(fn))
                throw Exc(NFormat(t_("ошибка при открытии файла сценария '%s'"), fn));

            script_file_names.Add(norm);
            if(!SqlRunScript(dialect, fi, executor, progress, script_file_names, new_min, new_max))
                return false;
            script_file_names.Drop();
        }
        else {
            TxtBuf statement;
            bool exec = false;
            bool body = false;
            bool chr = false;

            while(!script_stream.IsEof() && !exec) {
                Txt line = script_stream.GetLine();
                line_number++;

                i32 l = line.GetLength();
                if(l && line[0] == '/' && !body && !chr)
                    exec = true;
                else
                if(l && line[0] == '.' && body && !chr)
                    body = false;
                else {
                    if(statement.GetLength() && !chr)
                        statement.Cat(' ');
                    bool spc = true;
                    bool create = false;
                    for(u8k *s = (u8k *)(tukk )line; *s; s++) {
                        if(s[0] == '-' && s[1] == '-' && !chr) break;
                        if(*s == '\'') chr = !chr;
                        if(!chr && spc && scmpw(s, "create"))
                            create = true;
                        if(!chr && spc && (scmpw(s, "begin") || scmpw(s, "declare") ||
                           create && (scmpw(s, "procedure") || scmpw(s, "function") || scmpw(s, "trigger"))
                        ))
                            body = true;
                        if(*s == ';' && !chr && !body) {
                            exec = true;
                            break;
                        }
                        if(*s > ' ' || chr) {
                            statement.Cat(*s);
                            spc = false;
                        }
                        else
                        if(*s == '\t' || *s == ' ') {
                            if(!spc) statement.Cat(' ');
                            spc = true;
                        }
                        else
                            spc = false;
                    }
                    if(chr) statement.Cat("\r\n");
                }
            }
            if(progress((i32)(progress_min + script_stream.GetPos()
                * (progress_max - progress_min) / script_stream.GetSize()), PROGRESS_RANGE))
                throw AbortExc();
            if(!executor(statement))
                return false;
        }
    }
    return true;
}

bool SqlRunScript(i32 dialect, Stream& script_stream, const Txt& file_name,
                  Gate<Txt> executor, Gate<i32, i32> progress)
{
    Index<Txt> script_file_names;
    Txt fn = NormalizePath(file_name);
#if !TARGET_PASH_HAS_CASE
    fn = ToLower(fn);
#endif
    script_file_names.Add(fn);
    return SqlRunScript(dialect, script_stream, executor, progress, script_file_names, 0, PROGRESS_RANGE);
}

void RunDlgSqlExport(Sql& cursor, Txt command, Txt tablename);

class SqlConsole : public TopWindow {
public:
    virtual bool Key(dword key, i32 count);
    virtual void Serialize(Stream& s);

    void ListMenu(Bar& bar);
    void ListPrintRow();
    void ListPrintList();
    void ListExport();

protected:
    Sql                        cursor;
    Txt                     lastquery;
    Splitter                   lires, vsplit;
    ArrayCtrl                  list;
    StaticRect                 rec_err;
    DocEdit                    errortext;
    ArrayCtrl                  record;
    ArrayCtrl                  trace;
    CodeEditor                 command;
    Button                     execute;
    Button                     schema;
    Button                     csv;
    ParentCtrl                 command_pane;
//  CallbackSet                hide;
    Vec<i32>                cw;
    Vec<bool>               visible;
    Vec<bool>               lob;
    Txt                     LastDir;
    Label                      info1, info2;

    enum {
        NORMAL,
        RERUN,
        SCRIPT,
        QUIET,
    };

    void    ColSize();
    void    Hide(i32 i);
    void    Record();
    void    Execute(i32 type = NORMAL);
    void    TraceToCommand();
    void    TraceToExecute();
    void    ListToCommand(ArrayCtrl *list);
    void    SaveTrace();
    void    RunScript(bool quiet);
    void    TraceMenu(Bar& menu);
    void    ObjectTree() { SQLObjectTree(cursor.GetSession()); }
    void    Csv();

    class Exec;
    friend class Exec;
    class Exec : public StatementExecutor {
    public:
        typedef Exec CLASSNAME;
        Exec(bool quiet) : quiet(quiet) {}
        SqlConsole *me;
        bool quiet;
        virtual bool Execute(const Txt& stmt) {
            me->command <<= stmt; me->Sync(); me->Execute(quiet ? QUIET : SCRIPT); return true;
        }
        bool GateExec(Txt stmt) { return Execute(stmt); }
        operator Gate<Txt> ()   { return OTVET(GateExec); }

    };

    void ViewRecord();

public:
    typedef SqlConsole CLASSNAME;

    void    Perform();

    void    AddInfo(Ctrl& tgt, Label& info, tukk txt);

    SqlConsole(SqlSession& session);
};

void SqlConsole::Execute(i32 type) {
    list.Reset();
    list.HeaderObject().Absolute().Moving();
    visible.Clear();
    lob.Clear();
    record.Clear();
    Txt s = ~command;
    while(*s.Last() == ';')
        s.Trim(s.GetLength() - 1);
    i32 ms0 = msecs();
    cursor.ClearError();
    lastquery = s;
    Txt ttl = s;
    ttl.Replace("\t", " ");
    ttl.Replace("\n", " ");
    ttl.Replace("\r", "");
    info1.Remove();
    info2.Remove();
    if(!cursor.Execute(s)) {
    error:
        record.Hide();
        errortext.Show();
        list.AddColumn(t_("Ошибка"));
        Txt err = cursor.GetLastError();
        errortext <<= err;
        list.Add(err);
        trace.Add(s, err, "");
        trace.GoEnd();
        Title((ttl + " - " + err).ToWTxt());
        return;
    }
    if(type == QUIET)
        return;
    bool onecol = cursor.GetColumns() == 1;
    record.Show(!onecol);
    errortext.Show(onecol);
    i32 ms1 = msecs();
    cw.SetCount(cursor.GetColumns());
    visible.SetCount(cw.GetCount(), true);
    i32 margins;
    for(i32 i = 0; i < cursor.GetColumns(); i++) {
        const SqlColumnInfo& ci = cursor.GetColumnInfo(i);
        Txt n = ToLower(ci.name);
        list.AddColumn(n);
        list.HeaderTab(i).WhenAction = OTVET1(Hide, i);
        margins = HorzLayoutZoom(2) + 2 * list.HeaderTab(i).GetMargin();
        cw[i] = GetTextSize(n, StdFont()).cx + margins;
        record.Add(n, Null);
        lob.Add(ci.type == -1 || ci.type == -2); // !! BLOB / CLOB hack
    }
    Progress pi;
    pi.SetText(t_("Фетчировано %d строк(а)"));
    while(cursor.Fetch()) {
        Vec<Val> row = cursor.GetRow();
        for(i32 i = 0; i < cursor.GetColumns(); i++)
        {
            if(lob[i])
            {
                Txt temp;
                cursor.GetColumn(i, temp);
                row[i] = temp;
            }
            cw[i] = max(cw[i], GetTextSize(StdFormat(row[i]), StdFont()).cx + margins);
            cw[i] = min(cw[i], list.GetSize().cx / 3);
        }
        list.Add(row);
        if(pi.StepCanceled()) break;
    }
    if(cw.GetCount() && cursor.WasError()) {
        list.Reset();
        goto error;
    }
    visible.SetCount(list.GetColumnCount(), true);
    ColSize();
    if(list.GetCount() > 0)
        list.SetCursor(0);
    Title(NFormat(t_("%s (%d рядов)"), ttl, list.GetCount()));
    Txt rrows = Format(t_("%d рядов"), max(list.GetCount(), cursor.GetRowsProcessed()));
    Txt rms = Format(t_("%d мс"), ms1 - ms0);
    if(type == RERUN && trace.IsCursor()) {
        trace.Set(1, rrows);
        trace.Set(2, rms);
    }
    else {
        trace.Add(s, rrows, rms);
        trace.GoEnd();
    }
    command.Remove(0, command.GetLength());
    command.SetSelection(0, 0);
}

void SqlConsole::ColSize() {
    i32 maxw = list.GetSize().cx;
    i32 wx = 0;
    for(i32 i = 0; i < list.GetColumnCount(); i++)
        if(visible[i]) {
            i32 w = min(maxw, cw[i]);
            wx += w;
            list.HeaderObject().SetTabRatio(i, w);
            list.HeaderObject().ShowTab(i);
        }
        else
            list.HeaderObject().HideTab(i);
}

void SqlConsole::Hide(i32 i) {
    if(i < visible.GetCount())
        visible[i] = false;
    ColSize();
}

void SqlConsole::Record() {
    if(list.GetIndexCount() == 1)
        errortext <<= StdFormat(list.Get(0));
    for(i32 i = 0; i < list.GetIndexCount(); i++)
        record.Set(i, 1, list.IsCursor() ? list.Get(i) : Val());
}

bool SqlConsole::Key(dword key, i32 count) {
    switch(key) {
    case K_F5:
        Execute();
        return true;
    case K_CTRL_R:
        RunScript(false);
        return true;
    case K_CTRL_Q:
        RunScript(true);
        return true;
    case K_CTRL_S:
        SaveTrace();
        return true;
    }
    return TopWindow::Key(key, count);
}

void SqlConsole::Serialize(Stream& s) {
    i32 version = 0;
    s / version;
    s.Magic();
    Rect r = GetRect();
    s % r;
    SetRect(r);
    vsplit.Serialize(s);
    record.SerializeHeader(s);
    lires.Serialize(s);
    trace.SerializeHeader(s);
    if(s.IsLoading())
        trace.Clear();
    Vec<ValArray> ar;
    for(i32 i = 0; i < trace.GetCount(); i++)
        ar.Add(trace.GetArray(i));
    s % ar;
    if(s.IsLoading()) {
        for(i32 i = 0; i < ar.GetCount(); i++)
            trace.SetArray(i, ar[i]);
        trace.GoEnd();
    }
    if(version >= 1)
        s % LastDir;
    s.Magic();
}

void SqlConsole::Perform() {
    const char cfg[] = "SqlConsole.cfg";
    LoadFromFile(*this, cfg);
    Title(t_("Коммандер SQL"));
    Icon(SqlConsoleImg::database_edit(), SqlConsoleImg::SqlConsoleIconLarge());
    Sizeable();
    Zoomable();
    ActiveFocus(command);
    Run();
    cursor.ClearError();
    StoreToFile(*this, cfg);
}

void SqlConsole::TraceToCommand() {
    if(trace.IsCursor()) {
        command.SetData(trace.Get(0));
        command.SetCursor(command.GetLength());
    }
}

void SqlConsole::ListToCommand(ArrayCtrl *l)
{
    i32 c = l->GetClickColumn();
    if(GetCtrl() && l->IsCursor() && c >= 0 && c < l->GetColumnCount()) {
        command.Paste(AsTxt(l->Get(c)).ToWTxt());
        command.SetFocus();
    }
}

void SqlConsole::TraceToExecute() {
    Execute(RERUN);
}

void SqlConsole::SaveTrace() {
    FileSel fsel;
    fsel.ActiveDir(LastDir);
    fsel.DefaultExt("sql");
    fsel.Type(t_("Сценарии SQL (*.sql)"), "*.sql");
    fsel.AllFilesType();
    if(!fsel.ExecuteSaveAs(t_("Сохранить трассировку как"))) return;
    FileOut out(~fsel);
    if(!out) return;
    LastDir = GetFileDir(~fsel);
    for(i32 i = 0; i < trace.GetCount(); i++) {
        out.Put((Txt)trace.Get(i, 0));
        out.Put(";\n");
    }
}

void SqlConsole::RunScript(bool quiet) {
//  DRX::RunScript runscript = cursor.GetSession().GetRunScript();
//  if(!runscript) {
//      Exclamation(t_("Database connection doesn't support running scripts."));
//      return;
//  }
    FileSel fsel;
    fsel.ActiveDir(LastDir);
    fsel.DefaultExt("sql");
    fsel.Type(t_("Сценарии SQL (*.sql)"), "*.sql");
    fsel.AllFilesType();
    if(!fsel.ExecuteOpen(t_("Выполнить сценарий"))) return;
    Exec exec(quiet);
    exec.me = this;
    LastDir = GetFileDir(~fsel);
    Progress progress(t_("Выполняется сценарий"));
    FileIn fi;
    if(!fi.Open(~fsel)) {
        Exclamation(NFormat(t_("Не удаётся открыть файл [* \1%s\1]."), ~fsel));
        return;
    }
    try {
        SqlRunScript(cursor.GetDialect(), fi, ~fsel, exec, progress);
    }
    catch(Exc e) {
        ShowExc(e);
    }
}

void SqlConsole::TraceMenu(Bar& menu) {
    menu.Add(t_("Сохранить как сценарий.."), OTVET(SaveTrace)).Key(K_CTRL_S);
    menu.Add(t_("Выполнить сценарий.."), OTVET1(RunScript, false)).Key(K_CTRL_R);
    menu.Add(t_("Выполнить сценарий втихую.."), OTVET1(RunScript, true)).Key(K_CTRL_Q);
}

void SqlConsole::ListMenu(Bar& bar)
{
    bar.Add(t_("Печать записи"), OTVET(ListPrintRow));
    bar.Add(t_("Печать списка"), OTVET(ListPrintList));
    bar.Add(t_("Экспорт..."), OTVET(ListExport));
}

void SqlConsole::Csv()
{
    SelectSaveFile("Csv\t*.csv", list.AsCsv());
}

void SqlConsole::ListPrintRow()
{
    Txt qtf;
    qtf << "[A1 ";
    if(!IsNull(lastquery))
        qtf << "[4* \1" << lastquery << "\1]&&";
    for(i32 i = 0; i < record.GetCount(); i++) {
        qtf << (i ? "--" : "++") << "::10@(240.240.240) \1" << StdFormat(record.Get(i, 0)) << "\1"
            "||::30@(255.255.255) \1" << StdFormat(record.Get(i, 1)) << "\1";
    }
    qtf << "++";
    Report report;
    report << qtf;
    DRX::Perform(report);
}

void SqlConsole::ListPrintList()
{
    Txt qtf;
    qtf << "[A1 ";
    if(!IsNull(lastquery))
        qtf << "[4* \1" << lastquery << "\1]&&";
    for(i32 i = 0; i < record.GetCount(); i++)
        qtf << (i ? "||" : "++") << "::@(240.240.240) [* \1"
        << StdFormat(record.Get(i, 0)) << "\1]";
    for(i32 i = 0; i < list.GetCount(); i++)
        for(i32 j = 0; j < list.GetIndexCount(); j++)
            qtf << (j ? "||" : "--") << "::@(255.255.255) \1" << StdFormat(list.Get(i, j)) << '\1';
    qtf << "++";
    Report report;
    report << qtf;
    DRX::Perform(report);
}

void SqlConsole::ListExport()
{
    RunDlgSqlExport(cursor, lastquery, Null);
}

struct SqlValViewDlg : public WithSqlValViewLayout<TopWindow> {
    typedef SqlValViewDlg CLASSNAME;

    Txt value;

    void Sync();
    void Save();

    SqlValViewDlg();
};

void SqlValViewDlg::Sync()
{
    if(~format) {
        TxtStream ss;
        HexDumpData(ss, ~value, value.GetLength(), false, 1000000);
        text <<= ss.GetResult();
    }
    else
        text <<= value;
}

void SqlValViewDlg::Save()
{
    SelectSaveFile("Файл\t*.*", value);
}

SqlValViewDlg::SqlValViewDlg()
{
    CtrlLayout(*this, "");
    text.SetFont(Monospace(GetStdFont().GetHeight()));
    format <<= OTVET(Sync);
    save <<= OTVET(Save);
    format = 0;
}

void SqlViewVal(const Txt& title, const Txt& value)
{
    SqlValViewDlg dlg;
    dlg.Title(title);
    dlg.value = value;
    dlg.Sync();
    dlg.Execute();
}

void SqlConsole::ViewRecord()
{
    if(!record.IsCursor())
        return;
    SqlViewVal(record.Get(0), AsTxt(record.Get(1)));
}

void SqlConsole::AddInfo(Ctrl& tgt, Label& info, tukk txt)
{
    info = txt;
    tgt.Add(info.SizePos());
//  tgt.Add(info.BottomPos(0, GetStdFontCy()).RightPos(0, GetTextSize(txt, StdFont()).cx));
}

SqlConsole::SqlConsole(SqlSession& session)
: cursor(session)
{
    i32 ecy = EditField::GetStdHeight();
    lires.Horz(list, rec_err);
    rec_err << record.SizePos() << errortext.SizePos();
    errortext.SetReadOnly();
    errortext.SetFont(Courier(12));
    errortext.Hide();
    vsplit.Vert(lires, trace);
    vsplit.Add(command_pane);
    vsplit.SetPos(6500);
    vsplit.SetPos(8500, 1);
    lires.SetPos(7000);
    record.AddColumn(t_("Графа"), 5);
    record.AddColumn(t_("Значение"), 10);
    record.WhenLeftDouble = OTVET(ViewRecord);
    record.WhenLeftClick = OTVET1(ListToCommand, &record);
    AddInfo(record, info1, "\1[g= Используйте [@B Ctrl+Click], чтобы копировать данные в SQL&[@B DoubleClick] для детального обзора.");
    trace.AddColumn(t_("Команда"), 8);
    trace.AddColumn(t_("Результат"), 1);
    trace.AddColumn(t_("Продолжительность"), 1);
    trace.WhenLeftClick = OTVET(TraceToCommand);
    trace.WhenLeftDouble = OTVET(TraceToExecute);
    trace.WhenBar = OTVET(TraceMenu);
    trace.NoWantFocus();
    list.WhenSel = OTVET(Record);
    list.WhenLeftClick = OTVET1(ListToCommand, &list);
    list.WhenBar = OTVET(ListMenu);
    list.HeaderObject().Absolute();
    AddInfo(list, info2, "\1[g= Используйте [@B Ctrl+Click], чтобы копировать данные в SQL.");
    Add(vsplit.SizePos());
    command.SetFont(Courier(GetStdFontCy()));
    command_pane.Add(command.VSizePos().HSizePos(0, HorzLayoutZoom(100)));
    ecy = max(24, ecy);
    command_pane.Add(execute.TopPos(0, ecy).RightPos(4, HorzLayoutZoom(90)));
    command_pane.Add(schema.TopPos(ecy + 4, ecy).RightPos(4, HorzLayoutZoom(90)));
    command_pane.Add(csv.TopPos(2 * ecy + 4, ecy).RightPos(4, HorzLayoutZoom(90)));
    command.Highlight("sql");
    schema.SetLabel(t_("&Схема"));
    schema <<= OTVET(ObjectTree);
    schema.SetImage(SqlConsoleImg::bricks());
    execute <<= OTVET1(Execute, NORMAL);
    execute.SetImage(SqlConsoleImg::lightning());
    execute.SetLabel(t_("Выполнить (F5)"));
    csv <<= OTVET(Csv);
    csv.SetLabel(t_("Экспорт.."));
    csv.SetImage(SqlConsoleImg::database_save());
    ActiveFocus(command);
}

void SQLCommander(SqlSession& session) {
    SqlConsole con(session);
    con.Perform();
}

bool IsSqlConsoleActive__()
{
    return dynamic_cast<SqlConsole *>(Ctrl::GetActiveWindow());
}

}
