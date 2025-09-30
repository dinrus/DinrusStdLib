#ifndef IDEEDITOR_H
#define IDEEDITOR_H

#include <drx/CtrlLib/CtrlLib.h>
#include <X/pcre/Pcre.h>

namespace drx {

#define  LAYOUTFILE <drx/CodeEditor/CodeEditor.lay>
#include <drx/CtrlCore/lay.h>

#define IMAGEVECTOR Vec
#define IMAGECLASS  CodeEditorImg
#define IMAGEFILE   <drx/CodeEditor/CodeEditor.iml>
#include <drx/Draw/iml_header.h>


void FindWildcardMenu(Callback1<tukk > cb, Point p, bool tablf, Ctrl *owner, bool regexp);

//Запмсь информации о строке.
struct LineInfoRecord {
	i32    lineno; //номер
	Txt    breakpoint; //останов
	i32    count; //счёт
	i32    error; //ошибка
	i32    firstedited; //редактирована впервые
	i32    edited; //редактирована

	LineInfoRecord() { error = 0; edited = 0; }
};

typedef Array<LineInfoRecord> LineInfo;

void ClearErrors(LineInfo& li);

struct LineInfoRemRecord : Moveable<LineInfoRemRecord> {
	i32    firstedited;
	i32    edited;
};

typedef Vec<LineInfoRemRecord> LineInfoRem;

//Пересчёт
void Renumber(LineInfo& lf);
//Удаление отладочных остановов
void ClearBreakpoints(LineInfo& lf);
//Валидация отладочных остановов
void ValidateBreakpoints(LineInfo& lf);

class CodeEditor;

//БрусРедактора:ЛевыйФрейм
class EditorBar : public FrameLeft<Ctrl> {
public:
//Отрисовка
	virtual void Paint(Draw& w);
//Двиг мыши
	virtual void MouseMove(Point p, dword flags);
//Выход мыши
	virtual void MouseLeave();
	virtual void LeftDown(Point p, dword flags);
//Двуклик левый
	virtual void LeftDouble(Point p, dword flags);
//Правая внизу
	virtual void RightDown(Point p, dword flags);
//Колёсико
	virtual void MouseWheel(Point p, i32 zdelta, dword keyflags);

private:
	struct LnInfo : Moveable<LnInfo> {
		i32    lineno = -1;
		Txt    breakpoint;
		i32    error;
		i32    firstedited;
		i32    edited;
		Image  icon;
		Txt    annotation;

		LnInfo() { lineno = -1; error = 0; firstedited = 0; edited = 0; }
	};

	Vec<LnInfo>      li;
	LineInfoRem      li_removed;

	CodeEditor       *editor;
	i32              ptrline[2];
	Image            ptrimg[2];
	bool             bingenabled;
	bool             hilite_if_endif;
	bool             line_numbers;
	i32              annotations;
	bool             ignored_next_edit;
	i32              next_age;
	i32              active_annotation;
	Vec<Color>       animate;
	Image            status_image;

	Txt&    PointBreak(i32& y);
	void    sPaintImage(Draw& w, i32 y, i32 fy, const Image& img);

	friend class CodeEditor;

public:
	Event<i32> WhenBreakpoint;//Событие:останов
	Event<>    WhenAnnotationMove;//Событие:Перемещение аннотации
	Event<>    WhenAnnotationClick;//Событие:Клик на аннотации
	Event<>    WhenAnnotationRightClick;//Событие:Правый клик на аннотации

//Вставить строки
	void InsertLines(i32 i, i32 count);
//Удалить строки
	void RemoveLines(i32 i, i32 count);
//Очистить строки
	void ClearLines();
//Промотка
	void Scroll()                          { Refresh(); }
//Синхронизация размера
	void SyncSize();
//Пересчёт
	void Renumber(i32 linecount);
//Удалить остановы
	void ClearBreakpoints();
//Валидировать остановы
	void ValidateBreakpoints();

//Получить останов по номеру строки
	Txt     GetBreakpoint(i32 ln);
//Установить останов по номеру, с текстом
	void    SetBreakpoint(i32 ln, const Txt& s);
//Установить, что строка редактирована
	void    SetEdited(i32 ln, i32 count = 1);
//Удалмть редактированые
	void    ClearEdited();
//Установить ошибку в строке.
	void    SetError(i32 ln, i32 err);
//Удалить ошибки
	void    ClearErrors(i32 ln);

//Установить редактор по указателю
	void SetEditor(CodeEditor *e)           { editor = e; }

//Получить инфу о строке
	LineInfo GetLineInfo() const;
//Установить инфу о строке
	void     SetLineInfo(const LineInfo& li, i32 total);
//Получить удаление
	LineInfoRem & GetLineInfoRem()                   { return li_removed; }
//Установить удаление
	void     SetLineInfoRem(LineInfoRem pick_ li)    { li_removed = pick(li); }

//Очистить оннотации
	void     ClearAnnotations();
//Установить аннотацию
	void     SetAnnotation(i32 line, const Image& img, const Txt& ann);
//Получить аннотацию
	Txt      GetAnnotation(i32 line) const;

	i32      GetLineNo(i32 lineno) const;
	i32      GetNoLine(i32 line) const;

	void     SetPtr(i32 line, const Image& img, i32 i);
	void     HidePtr();

//Активировать использование остановов
	void     EnableBreakpointing(bool b)     { bingenabled = b; }
//Подсвечивать ли #endif
	void     HiliteIfEndif(bool b)           { hilite_if_endif = b; Refresh(); }
//Показывать ли номера строк
	void     LineNumbers(bool b);
//Ширина аннотаций
	void     Annotations(i32 width);

	bool     IsHiliteIfEndif() const         { return hilite_if_endif; }
//Получить строку с активной аннотацией
	i32      GetActiveAnnotationLine() const { return active_annotation; }
//Установить анимацию
	void     SetAnimate(const Vec<Color>& a)   { if(a != animate) { animate = clone(a); Refresh(); } }
//Рисунок статуса
	void     StatusImage(const Image& m);

	EditorBar();
	virtual ~EditorBar();
};

//Позиция идентификатора
struct IdentPos {
	i32    begin;
	i32    end;
	Txt    ident;
};

Array<IdentPos> GetLineIdent(tukk line);
//Получить текст на строке
Vec<Point>   GetLineTxt(wtukk wline, bool& is_begin, bool& is_end);

inline i32  CharFilterCIdent(char i)  { return IsAlNum(i) || i == '_' ? i : 0; }
inline bool iscidl(wchar c)            { return iscid(c) || IsLetter(c); }
inline bool islbrkt(wchar c)           { return c == '{' || c == '[' || c == '('; }
inline bool isrbrkt(wchar c)           { return c == '}' || c == ']' || c == ')'; }
inline bool isbrkt(wchar c)            { return islbrkt(c) || isrbrkt(c); }

struct FindReplaceDlg : FrameBottom< WithIDEFindReplaceLayout<TopWindow> > {
	WTxt itext;
	bool    replacing;

	virtual bool Key(dword key, i32 count);
	void Setup(bool doreplace);
	void Sync();
	bool IsIncremental() const              { return incremental.IsEnabled() && incremental; }

	typedef FindReplaceDlg CLASSNAME;

	FindReplaceDlg();
};

#include "Syntax.h"
#include "CSyntax.h"
#include "DiffSyntax.h"
#include "TagSyntax.h"
#include "LogSyntax.h"
#include "PythonSyntax.h"

class CodeEditor : public LineEdit, public HighlightSetup
{
	friend class EditorBar;

public:
	virtual bool  Key(dword code, i32 count);
	virtual void  LeftDown(Point p, dword keyflags);
	virtual void  LeftDouble(Point p, dword keyflags);
	virtual void  LeftTriple(Point p, dword keyflags);
	virtual void  LeftRepeat(Point p, dword keyflags);
	virtual void  MouseMove(Point p, dword keyflags);
	virtual Image CursorImage(Point p, dword keyflags);
	virtual void  Serialize(Stream& s);
	virtual void  MouseLeave();
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyFlags);
	virtual void  Layout();

public:
	struct MouseTip {
		i32            pos;
		Val          value;
		const Display *display;
		Size           sz;
		bool           delayed = false;
		Color          background;
	};

protected:
	virtual void HighlightLine(i32 line, Vec<LineEdit::Highlight>& h, z64 pos);
	virtual void PreInsert(i32 pos, const WTxt& s);
	virtual void PostInsert(i32 pos, const WTxt& s);
	virtual void PreRemove(i32 pos, i32 size);
	virtual void PostRemove(i32 pos, i32 size);
	virtual void DirtyFrom(i32 line);
	virtual void SelectionChanged();

	virtual void ClearLines();
	virtual void InsertLines(i32 line, i32 count);
	virtual void RemoveLines(i32 line, i32 count);

	virtual void NewScrollPos();

	virtual Txt  GetPasteText();

	TimeCallback delayed;

	EditorBar   bar;
	Vec<i32> line2;

	struct SyntaxPos {
		i32    line;
		Txt data;

		void Clear() { line = 0; data.Clear(); }
	};

	SyntaxPos   syntax_cache[6];

//	EditorSyntax rm_ins;

	char    rmb;
	z64     highlight_bracket_pos0;
	z64     highlight_bracket_pos;
	bool    bracket_flash;
	z64     bracket_start;

	bool    barline;
	double  stat_edit_time;
	Time    last_key_time;

	bool    auto_enclose;
	bool    mark_lines;
	bool    check_edited;
	bool    persistent_find_replace;
	bool    do_ff_restore_pos;
	bool    withfindreplace;
	bool    wordwrap;

	i32     ff_start_pos;

	FindReplaceDlg findreplace;

	enum {
		WILDANY = 16,
		WILDONE,
		WILDSPACE,
		WILDNUMBER,
		WILDID,
	};

	struct Found {
		i32     type;
		WTxt    text;
	};

	Array<Found> foundwild;
	WTxt   foundtext;
	bool   foundsel;
	bool   found, notfoundfw, notfoundbk;
	z64    foundpos;
	i32    foundsize;

	enum { SEL_CHARS, SEL_WORDS, SEL_LINES };
	i32    selkind;

	WTxt illuminated;

	Txt  iwc;

	Txt highlight;

	i32    spellcheck_comments = 0;
	bool   wordwrap_comments = true;

	struct Tip : Ctrl {
		Val v;
		Color background;
		const Display *d;

		virtual void Paint(Draw& w);

		Tip();
	};

	bool  delayed_tip = false;
	Point delayed_pos = Null;
	Tip   tip;
	i32   tippos = Null;

	i32   replacei;

	bool          search_canceled;
	i32           search_time0;
	One<Progress> search_progress;

	Txt        refresh_info; // serialized next line syntax context to detect the need of full Refresh

	Vec<Point> errors; // ошибки в текущем файле (при компиляции)

	struct ScrollBarItems : Ctrl {
		ScrollBar& sb;
		CodeEditor& editor;

		void Paint(Draw& w);

		Vec<Tuple<i32, Image, Color>> pos;

		ScrollBarItems(ScrollBar& sb, CodeEditor& e);
	};

	ScrollBarItems sbi;


	struct HlSt;

	bool   MouseSelSpecial(Point p, dword flags);
	void   InitFindReplace();
	void   CancelBracketHighlight(z64& pos);
	void   FindPrevNext(bool prev);
	void   CheckBrackets();
	void   OpenNormalFindReplace0(bool replace);
	void   OpenNormalFindReplace(bool replace);
	void   FindReplaceAddHistory();
	void   FindWildcard();
	void   ReplaceWildcard();
	void   InsertWildcard(tukk s);
	void   IncrementalFind();
	void   NotFound();
	void   NoFindError();
	void   CheckSyntaxRefresh(z64 pos, const WTxt& text);

	void   SetFound(i32 fi, i32 type, const WTxt& text);

	i32    Match(wtukk f, wtukk s, i32 line, bool we, bool icase, i32 fi = 0);
	WTxt   GetWild(i32 type, i32& i);
	WTxt   GetReplaceText();

	bool   InsertRS(i32 chr, i32 count = 1);

	void   IndentInsert(i32 chr, i32 count);

	void   ForwardWhenBreakpoint(i32 i);

	bool   ToggleSimpleComment(i32 &start_line, i32 &end_line, bool usestars = true);
	void   ToggleLineComments(bool usestars = false);
	void   ToggleStarComments();
	void   Enclose(tukk c1, tukk c2, i32 l = -1, i32 h = -1);
	void   Make(Event<Txt&> op);
	void   TabsOrSpaces(Txt& out, bool maketabs);
	void   LineEnds(Txt& out);

	enum {
		TIMEID_PERIODIC = Ctrl::TIMEID_COUNT,
		TIMEID_COUNT,
	};

	void   Periodic();

	void   StartSearchProgress(z64 l, z64 h);
	bool   SearchProgress(i32 line);
	bool   SearchCanceled();
	void   EndSearchProgress();

	Txt    GetRefreshInfo(i32 pos);

public:
	Event<>            WhenSelection;
	Gate<MouseTip&>    WhenTip;
	Event<>            WhenLeftDown;
	Event<z64>         WhenCtrlClick;
	Event<>            WhenAnnotationMove;
	Event<>            WhenAnnotationClick;
	Event<>            WhenAnnotationRightClick;
	Event<>            WhenOpenFindReplace;
	Event<Txt&>        WhenPaste;
	Event<>            WhenUpdate;
	Event<i32>         WhenBreakpoint;

	Event<const Vec<Tuple<z64, i32>>&> WhenFindAll;

	FrameTop<Button>    topsbbutton;
	FrameTop<Button>    topsbbutton1;

	virtual bool DelayedTip(MouseTip& tip);

	static dword find_next_key;
	static dword find_prev_key;
	static dword replace_key;

	void   Clear();

	void   Highlight(const Txt& h);
	Txt    GetHighlight() const       { return highlight; }

	void   EscapeFindReplace();
	void   CloseFindReplace();
	void   FindReplace(bool pick_selection, bool pick_text, bool replace);
	void   FindAll();
	bool   FindFrom(z64 pos, bool back, bool block);
	bool   RegExpFind(z64 pos, bool block);
	bool   Find(bool back, bool block);
	bool   Find(bool back, bool blockreplace, bool replace);
	void   FindNext();
	void   FindPrev();
	bool   GetTxtRange(z64 cursor, z64& b, z64& e) const;
	bool   GetTxtRange(z64& b, z64& e) const { return GetTxtRange(GetCursor64(), b, e); }
	bool   FindTxt(bool back);
	bool   FindLangTxt(bool back);
	void   Replace();
	void   ReplaceAll(bool rest);
	i32    BlockReplace();

	void   MakeTabsOrSpaces(bool tabs);
	void   MakeLineEnds();

	void   CopyWord();
	void   SwapChars();
	void   DuplicateLine();
	void   Put(i32 chr);
	void   FinishPut();

	void   SerializeFind(Stream& s);
	bool   IsFindOpen() const                       { return findreplace.IsOpen(); }
	void   FindClose()                              { CloseFindReplace(); }

	void   Goto();

	void   DoFind();
	void   DoFindBack();

//	void    FindWord(bool back);
	WTxt    GetI();
	void    SetI(Ctrl *edit);
	void    PutI(WithDropChoice<EditTxt>& edit);

	void   MoveNextWord(bool sel);
	void   MovePrevWord(bool sel);
	void   MoveNextBrk(bool sel);
	void   MovePrevBrk(bool sel);

	Txt    GetWord(z64 pos);
	Txt    GetWord();

	bool   GetWordPos(z64 pos, z64& l, z64& h);

	void   DeleteWord();
	void   DeleteWordBack();
	void   SetLineSelection(i32 l, i32 h);
	bool   GetLineSelection(i32& l, i32& h);
	void   TabRight();
	void   TabLeft();
	void   SwapUpDown(bool up);

	void    CheckEdited(bool e = true)                { check_edited = e; }
	bool    GetCheckEdited()                          { return check_edited; }

	void    EditorBarLayout()                         { bar.SyncSize(); }

	LineInfo GetLineInfo() const                      { return bar.GetLineInfo(); }
	void     SetLineInfo(const LineInfo& lf);
	LineInfoRem GetLineInfoRem()                      { return LineInfoRem(bar.GetLineInfoRem(), 0); }
	void     SetLineInfoRem(LineInfoRem pick_  lf)    { bar.SetLineInfoRem(LineInfoRem(lf, 0)); }
	double   GetStatEditTime() const                  { return stat_edit_time; }
	void     Renumber()                               { bar.Renumber(GetLineCount()); }
	void     ClearBreakpoints()                       { bar.ClearBreakpoints(); }
	void     ValidateBreakpoints()                    { bar.ValidateBreakpoints(); }
	i32      GetLineNo(i32 line) const                { return bar.GetLineNo(line); }
	i32      GetNoLine(i32 line) const                { return bar.GetNoLine(line); }
	void     SetPtr(i32 line, const Image& img, i32 i){ bar.SetPtr(line, img, i); }
	void     HidePtr()                                { bar.HidePtr(); }
	Txt      GetBreakpoint(i32 line)                  { return bar.GetBreakpoint(line); }
	void     SetBreakpoint(i32 line, const Txt& b) { bar.SetBreakpoint(line, b); }
	void     SetError(i32 line, i32 err)              { bar.SetError(line, err); }
	void     ClearErrors(i32 line = -1)               { bar.ClearErrors(line); }
	void     ClearEdited()                            { bar.ClearEdited(); }
	i32		 GetUndoCount()                           { return undo.GetCount(); }
	void     GotoLine(i32 line);
	void     EnableBreakpointing()                    { bar.EnableBreakpointing(true); }
	void     DisableBreakpointing()                   { bar.EnableBreakpointing(false); }
	void     Renumber2();
	i32      GetLine2(i32 i) const;
	void     ReformatComment();

// СДЕЛАТЬ: Syntax: Remove
	void     HiliteScope(u8 b)                      { EditorSyntax::hilite_scope = b; Refresh(); }
	void     HiliteBracket(u8 b)                    { EditorSyntax::hilite_bracket = b; Refresh(); }
	void     HiliteIfDef(u8 b)                      { EditorSyntax::hilite_ifdef = b; Refresh(); }
	void     HiliteIfEndif(bool b)                    { bar.HiliteIfEndif(b); }

	void     ThousandsSeparator(bool b)               { thousands_separator = b; Refresh(); }
	void     IndentSpaces(bool is)                    { indent_spaces = is; }
	void     IndentAmount(i32 ia)                     { indent_amount = ia; }
	void     NoParenthesisIndent(bool b)              { no_parenthesis_indent = b; }

	void     SpellcheckComments(i32 lang)             { spellcheck_comments = lang; Refresh(); }
	i32      GetSpellcheckComments() const            { return spellcheck_comments; }
	void     WordwrapComments(bool b)                 { wordwrap_comments = b; }
	bool     IsWordwrapComments() const               { return wordwrap_comments; }

	void     NoFindReplace()                          { withfindreplace = false; }

	void     LineNumbers(bool b)                      { bar.LineNumbers(b); }
	void     MarkLines(bool b)                        { mark_lines = b; }
	bool     GetMarkLines()                           { return mark_lines; }
	void     AutoEnclose(bool b)                      { auto_enclose = b; }
	void     BarLine(bool b)                          { barline = b; }
	void     WordWrap(bool b)                         { wordwrap = b; }

	void     PersistentFindReplace(bool b = true)     { persistent_find_replace = b; }
	bool     IsPersistentFindReplace() const          { return persistent_find_replace; }

	void     FindReplaceRestorePos(bool b = true)     { do_ff_restore_pos = b; }
	bool     IsFindReplaceRestorePos() const          { return do_ff_restore_pos; }

	void     Annotations(i32 width)                   { bar.Annotations(width); }
	void     ClearAnnotations()                       { bar.ClearAnnotations(); }
	void     SetAnnotation(i32 i, const Image& icon, const Txt& a) { bar.SetAnnotation(i, icon, a); }
	Txt      GetAnnotation(i32 i) const               { return bar.GetAnnotation(i); }
	i32      GetActiveAnnotationLine() const          { return bar.GetActiveAnnotationLine(); }
	Size     GetBarSize() const                       { return bar.GetSize(); }
	void     HideBar()                                { bar.Hide(); }
	void     AnimateBar(const Vec<Color>& a)       { bar.SetAnimate(a); }

	void     Errors(Vec<Point>&& errs);

	void     Illuminate(const WTxt& text);
	WTxt     GetIlluminated() const                   { return illuminated; }

	void     Zoom(i32 d);

	void     StatusImage(const Image& m)              { bar.StatusImage(m); }

	One<EditorSyntax> GetSyntax(i32 line);
	bool IsCursorBracket(z64 pos) const;
	bool IsMatchingBracket(z64 pos) const;

// СДЕЛАТЬ: Do we really need this ?
	Vec<IfState> GetIfStack(i32 line)              { return GetSyntax(line)->PickIfStack(); }

	struct FindReplaceData {
		Txt    find, replace;
		Txt    find_list, replace_list;
		bool   wholeword, wildcards, ignorecase, samecase, regexp;
	};

	FindReplaceData GetFindReplaceData();
	void            SetFindReplaceData(const FindReplaceData& d);

	void     SyncTip();
	void     CloseTip()                               { if(tip.IsOpen()) tip.Close(); tip.d = NULL;  }

	typedef CodeEditor CLASSNAME;

	CodeEditor();
	virtual ~CodeEditor();

	static void InitKeywords();
};

Txt    ReadList(WithDropChoice<EditTxt>& e);
void   WriteList(WithDropChoice<EditTxt>& e, const Txt& data);

}

#endif