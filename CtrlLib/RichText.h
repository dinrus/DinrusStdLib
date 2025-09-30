class RichTextView : public Ctrl {
public:
	virtual void  Paint(Draw& w);
	virtual bool  Key(dword key, i32 count);
	virtual void  Layout();
	virtual Val GetData() const;
	virtual void  SetData(const Val& v);
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual Image CursorImage(Point p, dword keyflags);
	virtual void  LeftDown(Point p, dword keyflags);
	virtual void  MouseMove(Point p, dword keyflags);
	virtual void  LeftRepeat(Point p, dword keyflags);
	virtual void  LeftDouble(Point p, dword keyflags);
	virtual void  LeftTriple(Point p, dword keyflags);
	virtual void  RightDown(Point p, dword keyflags);
	virtual Txt GetSelectionData(const Txt& fmt) const;

private:
	Rect          margin;
	Color         background;
	Color         textcolor;
	Color         highlight_color = SYellow();
	Zoom          zoom;
	i32           cx;
	ScrollBar     sb;
	RichText      text;
	bool          sizetracking;
	bool          vcenter;
	bool          hldec;
	i32           highlight;
	i32           sell, selh;
	i32           cursor, anchor;
	bool          lazy;
	bool          shrink_oversized_objects;
	bool          icursor = true;
	bool          copy_with_tabs = false;

	void          EndSizeTracking();
	void          SetSb();
	void          Scroll();
	i32           GetPageCx(bool reduced = false) const;
	Point         GetTextPoint(Point p) const;
	i32           GetPointPos(Point p) const;
	Txt        GetLink(i32 pos, Point p) const;
	void          RefreshSel();
	void          RefreshRange(i32 a, i32 b);
	WTxt       GetSelText() const;
	i32           TopY() const;

protected:
	enum {
		TIMEID_ENDSIZETRACKING = Ctrl::TIMEID_COUNT,
		TIMEID_COUNT
	};

public:
	Event<const Txt&> WhenLink;
	Event<i32>           WhenMouseMove;
	Gate<i32, dword>     WhenMouseWheel;
	Event<>              WhenLeftClick;
	Gate<const Txt&>  WhenHighlight;

	void            Clear();
	void            Pick(RichText&& t);
	void            Pick(RichText&& txt, Zoom z);
	void            SetQTF(tukk qtf, Zoom z = Zoom(1, 1));
	const RichText& Get() const                               { return text; }
	Txt          GetQTF(u8 cs = CHARSET_UTF8) const      { return AsQTF(text, cs); }

	i32             GetCursor() const                         { return cursor; }

	i32             GetWidth() const                          { return text.GetWidth(); }
	i32             GetHeight(i32 cx) const                   { return text.GetHeight(Zoom(1, 1), cx); }
	i32             GetCy() const;
	i32             GetHeight() const /* obsolete */          { return GetHeight(GetSize().cx); }

	i32             GetSb() const                             { return sb; }
	void            SetSb(i32 i)                              { sb = i; }
	void            ScrollInto(i32 pos)                       { sb.ScrollInto(pos); }
	i32             GetSbTotal() const                        { return sb.GetTotal(); }
	Zoom            GetZoom() const;
	Rect            GetPage() const;

	bool            GotoLabel(Gate<const WTxt&> match, bool dohighlight = false, bool match_last = false);
	bool            GotoLabel(const Txt& lbl, bool highlight = false, bool match_last = false);
	void            ClearHighlight()                          { highlight = Null; Refresh(); }

	i32             GetLength() const                         { return text.GetLength(); }

	bool            IsSelection() const                       { return anchor != cursor; }
	void            ClearSelection();
	void            Copy();

	void            ScrollUp()                                { sb.PrevLine(); }
	void            ScrollDown()                              { sb.NextLine(); }
	void            ScrollPageUp()                            { sb.PrevPage(); }
	void            ScrollPageDown()                          { sb.NextPage(); }
	void            ScrollEnd()                               { sb.End(); }
	void            ScrollBegin()                             { sb.Begin(); }

	RichTextView&   PageWidth(i32 cx);
	RichTextView&   SetZoom(Zoom z);
	RichTextView&   Background(Color _color);
	RichTextView&   TextColor(Color _color);
	RichTextView&   Highlight(Color _color);
	RichTextView&   VCenter(bool b = true);
	RichTextView&   NoVCenter()                               { return VCenter(false); }
	RichTextView&   Margins(const Rect& m);
	RichTextView&   HMargins(i32 a);
	RichTextView&   VMargins(i32 a);
	RichTextView&   Margins(i32 a);
	RichTextView&   NoSb(bool b = true)                       { sb.AutoHide(!b); sb.Show(!b); return *this; }
	RichTextView&   AutoHideSb(bool b = true)                 { sb.AutoHide(b); return *this; }
	RichTextView&   HyperlinkDecoration(bool b = true)        { hldec = b; Refresh(); return *this; }
	RichTextView&   NoHyperlinkDecoration()                   { return HyperlinkDecoration(false); }
	RichTextView&   Lazy(bool b = true)                       { lazy = b; return *this; }
	RichTextView&   NoLazy()                                  { return Lazy(false); }
	RichTextView&   ShrinkOversizedObjects(bool b = true)     { shrink_oversized_objects = b; Refresh(); return *this; }
	RichTextView&   NoShrinkOversizedObjects()                { return ShrinkOversizedObjects(false); }
	RichTextView&   ICursor(bool b = true)                    { icursor = b; return *this; }
	RichTextView&   NoICursor()                               { return ICursor(false); }
	RichTextView&   CopyWithTabs(bool b = true)               { copy_with_tabs = b; return *this; }

	void            operator=(tukk qtf)                { SetQTF(qtf); }

	RichTextView&   SetScrollBarStyle(const ScrollBar::Style& s) { sb.SetStyle(s); return *this; }

	typedef RichTextView CLASSNAME;

	RichTextView();
	virtual ~RichTextView();
};

class RichTextCtrl : public RichTextView {
public:
	virtual void  SetData(const Val& v);

public:
	void            SetQTF(tukk qtf, Zoom z = GetRichTextStdScreenZoom()) { RichTextView::SetQTF(qtf, z); }
	void            operator=(tukk qtf)                                   { SetQTF(qtf); }
	RichTextCtrl();
};

i32 Prompt(i32 dontshowagain, tukk opt_id,
           Event<const Txt&> WhenLink, i32 beep,
           tukk title, const Image& iconbmp, tukk qtf, bool okcancel,
           tukk button1, tukk button2, tukk button3,
		   i32 cx,
		   Image im1, Image im2, Image im3);

i32 Prompt(Event<const Txt&> WhenLink,
           tukk title, const Image& iconbmp, tukk qtf, bool okcancel,
           tukk button1, tukk button2, tukk button3,
		   i32 cx,
		   Image im1, Image im2, Image im3);

i32 Prompt(Event<const Txt&> WhenLink,
           tukk title, const Image& icon, tukk qtf, bool okcancel,
           tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		   i32 cx = 0);
i32 Prompt(tukk title, const Image& icon, tukk qtf, bool okcancel,
           tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		   i32 cx = 0);
i32 Prompt(tukk title, const Image& icon, tukk qtf,
           tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		   i32 cx = 0);

enum { BEEP_NONE, BEEP_INFORMATION, BEEP_EXCLAMATION, BEEP_QUESTION, BEEP_ERROR };

i32 Prompt(Event<const Txt&> WhenLink, i32 beep,
           tukk title, const Image& iconbmp, tukk qtf, bool okcancel,
           tukk button1, tukk button2, tukk button3,
		   i32 cx,
		   Image im1, Image im2, Image im3);

i32 Prompt(Event<const Txt&> WhenLink, i32 beep,
           tukk title, const Image& icon, tukk qtf, bool okcancel,
           tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		   i32 cx = 0);
i32 Prompt(i32 beep, tukk title, const Image& icon, tukk qtf, bool okcancel,
           tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		   i32 cx = 0);
i32 Prompt(i32 beep, tukk title, const Image& icon, tukk qtf,
           tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		   i32 cx = 0);

i32 PromptOpt(tukk opt_id, i32 beep, Event<const Txt&> WhenLink,
              tukk title, const Image& icon, tukk qtf, bool okcancel,
              tukk button1, tukk button2, tukk button3,
		      i32 cx, Image im1, Image im2, Image im3);

i32 PromptOpt(tukk opt_id, i32 beep,
              tukk title, const Image& icon, tukk qtf, bool okcancel,
              tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		      i32 cx = 0);

i32 PromptOpt(tukk opt_id, i32 beep,
              tukk title, const Image& icon, tukk qtf,
              tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		      i32 cx = 0);

i32 PromptOpt1(tukk opt_id, i32 beep, Event<const Txt&> WhenLink,
               tukk title, const Image& icon, tukk qtf, bool okcancel,
               tukk button1, tukk button2, tukk button3,
		       i32 cx, Image im1, Image im2, Image im3);

i32 PromptOpt1(tukk opt_id, i32 beep,
               tukk title, const Image& icon, tukk qtf, bool okcancel,
               tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		       i32 cx = 0);

i32 PromptOpt1(tukk opt_id, i32 beep,
               tukk title, const Image& icon, tukk qtf,
               tukk button1, tukk button2 = NULL, tukk button3 = NULL,
		       i32 cx = 0);

void PromptOK(tukk qtf);
i32  PromptOKCancel(tukk qtf);
i32  PromptOKCancelAll(tukk qtf);
i32  PromptYesNo(tukk qtf);
i32  PromptYesNoCancel(tukk qtf);
i32  PromptYesNoAll(tukk qtf);
i32  PromptRetryCancel(tukk qtf);
i32  PromptAbortRetry(tukk qtf);
i32  PromptAbortRetryIgnore(tukk qtf);
i32  PromptSaveDontSaveCancel(tukk qtf);
void ShowExc(const Exc& exc);

void Exclamation(tukk qtf);

void ErrorOK(tukk qtf);
i32  ErrorOKCancel(tukk qtf);
i32  ErrorYesNo(tukk qtf);
i32  ErrorYesNoCancel(tukk qtf);
i32  ErrorYesNoAll(tukk qtf);
i32  ErrorRetryCancel(tukk qtf);
i32  ErrorAbortRetry(tukk qtf);
i32  ErrorAbortRetryIgnore(tukk qtf);

void PromptOKOpt(tukk qtf, tukk opt_id = NULL);
i32  PromptOKCancelOpt(tukk qtf, tukk opt_id = NULL);
i32  PromptOKCancelAllOpt(tukk qtf, tukk opt_id = NULL);
i32  PromptYesNoOpt(tukk qtf, tukk opt_id = NULL);
i32  PromptYesNoCancelOpt(tukk qtf, tukk opt_id = NULL);
i32  PromptYesNoAllOpt(tukk qtf, tukk opt_id = NULL);
i32  PromptRetryCancelOpt(tukk qtf, tukk opt_id = NULL);
i32  PromptAbortRetryOpt(tukk qtf, tukk opt_id = NULL);
i32  PromptAbortRetryIgnoreOpt(tukk qtf, tukk opt_id = NULL);
i32  PromptSaveDontSaveCancelOpt(tukk qtf, tukk opt_id = NULL);

void ExclamationOpt(tukk qtf, tukk opt_id = NULL);

void ErrorOKOpt(tukk qtf, tukk opt_id = NULL);
i32  ErrorOKCancelOpt(tukk qtf, tukk opt_id = NULL);
i32  ErrorYesNoOpt(tukk qtf, tukk opt_id = NULL);
i32  ErrorYesNoCancelOpt(tukk qtf, tukk opt_id = NULL);
i32  ErrorYesNoAllOpt(tukk qtf, tukk opt_id = NULL);
i32  ErrorRetryCancelOpt(tukk qtf, tukk opt_id = NULL);
i32  ErrorAbortRetryOpt(tukk qtf, tukk opt_id = NULL);
i32  ErrorAbortRetryIgnoreOpt(tukk qtf, tukk opt_id = NULL);
void ShowExcOpt(const Exc& exc, tukk opt_id);

void PromptOKOpt1(tukk qtf, tukk opt_id = NULL);
i32  PromptOKCancelOpt1(tukk qtf, tukk opt_id = NULL);
i32  PromptOKCancelAllOpt1(tukk qtf, tukk opt_id = NULL);
i32  PromptYesNoOpt1(tukk qtf, tukk opt_id = NULL);
i32  PromptYesNoCancelOpt1(tukk qtf, tukk opt_id = NULL);
i32  PromptYesNoAllOpt1(tukk qtf, tukk opt_id = NULL);
i32  PromptRetryCancelOpt1(tukk qtf, tukk opt_id = NULL);
i32  PromptAbortRetryOpt1(tukk qtf, tukk opt_id = NULL);
i32  PromptAbortRetryIgnoreOpt1(tukk qtf, tukk opt_id = NULL);
i32  PromptSaveDontSaveCancelOpt1(tukk qtf, tukk opt_id = NULL);

void ExclamationOpt1(tukk qtf, tukk opt_id = NULL);

void ErrorOKOpt1(tukk qtf, tukk opt_id = NULL);
i32  ErrorOKCancelOpt1(tukk qtf, tukk opt_id = NULL);
i32  ErrorYesNoOpt1(tukk qtf, tukk opt_id = NULL);
i32  ErrorYesNoCancelOpt1(tukk qtf, tukk opt_id = NULL);
i32  ErrorYesNoAllOpt1(tukk qtf, tukk opt_id = NULL);
i32  ErrorRetryCancelOpt1(tukk qtf, tukk opt_id = NULL);
i32  ErrorAbortRetryOpt1(tukk qtf, tukk opt_id = NULL);
i32  ErrorAbortRetryIgnoreOpt1(tukk qtf, tukk opt_id = NULL);
void ShowExcOpt1(const Exc& exc, tukk opt_id);

Image YesButtonImage();
Image NoButtonImage();
Image AbortButtonImage();
Image RetryButtonImage();

void YesButtonImage_Write(Image m);
void NoButtonImage_Write(Image m);
void AbortButtonImage_Write(Image m);
void RetryButtonImage_Write(Image m);

void ClearPromptOptHistory();
void ClearPromptOptHistory(Gate<Txt> filter);
void SerializePromptOptHistory(Stream& s);

typedef
i32 (*RedirectPromptFn)(Event<const Txt&> WhenLink,
                        tukk title, const Image& iconbmp, tukk qtf, bool okcancel,
                        tukk button1, tukk button2, tukk button3,
                        i32 cx, Image im1, Image im2, Image im3);

void RedirectPrompts(RedirectPromptFn r);

class HelpWindow : public TopWindow {
public:
	bool Key(dword key, i32) override;

private:
	RichTextView   view;
	TreeCtrl       tree;
	Splitter       tree_view;
	ToolBar        toolbar;
	Zoom           zoom;

	struct Pos : Moveable<Pos> {
		Txt link;
		i32    scy;
	};

	Vec<Pos>    back;
	Vec<Pos>    forward;
	Index<Txt>  tree_ndx;
	Txt         topic;
	Txt         label;
	Txt         current_link;
	i32            doing_goto = 0;

	bool GoTo0(const Txt& link);
	void Back();
	void Forward();
	void FontSize();
	void Tools(Bar& bar);
	void TreeCursor();
	void SetZoom();
	void Print();
	Pos  GetPos();
	Vec<i32> ScPositions(const Vec<i32>& p);
	void Ids(i32 pid, Vec<i32>& r);
	Vec<i32> Ids();
	bool PrevNext(i32 d, bool allowempty);

public:
	virtual Topic AcquireTopic(const Txt& topic);
	virtual void  FinishText(RichText& text);
	virtual void  BarEx(Bar& bar);

	Gate<const WTxt&, const WTxt&> WhenMatchLabel;

	bool GoTo(const Txt& link);

	void SetBar();
	void Serialize(Stream& s) override;

	void ClearTree();
	i32  AddTree(i32 parent, const Image& img, const Txt& topic, const Txt& title);
	void SortTree(i32 id, i32 (*cmp)(const Val& k1, const Val& v1,
	                                 const Val& k2, const Val& v2));
	void SortTree(i32 id, i32 (*cmp)(const Val& v1, const Val& v2));
	void SortTree(i32 id);
	void FinishTree();
	void OpenDeep(i32 id = 0);
	void CurrentOrHome();
	void ClearCurrentLink()                       { current_link.Clear(); }
	bool Up(const Vec<i32>& poslist);
	bool Down(const Vec<i32>& poslist);
	bool Next(bool allowempty = false);
	bool Prev(bool allowempty = false);

	Txt GetCurrent() const                     { return topic; }
	Txt GetCurrentLabel() const                { return label; }
	Txt GetCurrentLink() const                 { return current_link; }

	typedef HelpWindow CLASSNAME;

	HelpWindow();
};

#ifndef TARGET_PDA

void Print(Draw& w, const RichText& text, const Rect& page, const Vec<i32>& pagelist);
void Print(Draw& w, const RichText& text, const Rect& page);
bool Print(const RichText& text, const Rect& page, i32 currentpage, tukk name = NULL);

#endif