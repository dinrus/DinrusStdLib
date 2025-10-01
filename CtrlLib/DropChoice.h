void DropEdge_Write(Val);

class PopUpTable : public ArrayCtrl { // deprecated, replaced with PopUpList
public:
	virtual void LeftUp(Point p, dword keyflags);
	virtual bool Key(dword key, i32);

protected:
	void PopupDeactivate();
	void PopupCancelMode();

	struct Popup : Ctrl {
		PopUpTable *table;

		virtual void Deactivate() { table->PopupDeactivate(); }
		virtual void CancelMode() { table->PopupCancelMode(); }
	};

	i32          droplines;
	i32          inpopup;
	bool         open;
	One<Popup>   popup;

	void         DoClose();

public:
	void         PopUp(Ctrl *owner, i32 x, i32 top, i32 bottom, i32 width);
	void         PopUp(Ctrl *owner, i32 width);
	void         PopUp(Ctrl *owner);

	Event<>      WhenCancel;
	Event<>      WhenSelect;

	PopUpTable&  SetDropLines(i32 _droplines)          { droplines = _droplines; return *this; }

	void         Normal();

	PopUpTable();
	virtual ~PopUpTable();
};

class PopUpList {
protected:
	void PopupDeactivate();
	void PopupCancelMode();

	struct PopupArrayCtrl : ArrayCtrl {
		PopUpList *list;

		virtual void LeftUp(Point p, dword keyflags);
		virtual bool Key(dword key, i32);
	};

	struct Popup : Ctrl {
		PopUpList     *list;
		PopupArrayCtrl ac;
		bool           closing = false;

		virtual void Deactivate() { if(!closing) list->PopupDeactivate(); }
		virtual void CancelMode() { if(!closing) list->PopupCancelMode(); }

		Popup(PopUpList *list);
	};

	Vec<Val>           items;
	Vec<word>            lineinfo;
	Vec<const Display *> linedisplay;
	One<Popup>              popup;
	const ScrollBar::Style *sb_style = nullptr;
	const Display          *display;
	const Convert          *convert;
	i32                     linecy;
	i32                     cursor = -1;
	i16                   droplines;
	i16                   inpopup:15;
	bool                    permanent:1;

	void          DoSelect();
	void          DoCancel();
	void          DoClose();

	friend struct Popup;

public:
	Event<>      WhenCancel;
	Event<>      WhenSelect;

	void         PopUp(Ctrl *owner, i32 x, i32 top, i32 bottom, i32 width);
	void         PopUp(Ctrl *owner, i32 width);
	void         PopUp(Ctrl *owner);
	
	ArrayCtrl&   Permanent();

	void         Clear();
	void         SetCount(i32 n);
	void         Add(const Val& v);
	void         AddSeparator();
	void         Remove(i32 i);
	void         Insert(i32 i, const Val& v);

	void         SetCursor(i32 i);
	i32          GetCursor() const;

	i32          GetCount() const                              { return items.GetCount(); }
	void         Set(i32 i, const Val& v);
	Val        Get(i32 i) const                              { return items[i]; }
	i32          Find(const Val& v) const;
	void         SetScrollBarStyle(const ScrollBar::Style& s);
	void         SetLineCy(i32 cy);
	i32          GetLineCy() const                             { return linecy; }
	void         SetLineCy(i32 ii, i32 cy);
	i32          GetLineCy(i32 ii) const;
	bool         Key(i32 c);
	bool         IsLineEnabled(i32 ii) const;

	void           SetDisplay(const Display& d);
	const Display& GetDisplay() const                          { return *display; }

	void           SetDisplay(i32 i, const Display& d);
	const Display& GetDisplay(i32 i) const;

	void           SetConvert(const Convert& c);

	PopUpList&   SetDropLines(i32 _droplines)                  { droplines = _droplines; return *this; }

	PopUpList();
	virtual ~PopUpList();
};

class DropList : public MultiButton, public Convert {
public:
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual bool  Key(dword key, i32);
	virtual void  SetData(const Val& data);
	virtual Val GetData() const;
	virtual void  DropPush();

	virtual Val Format(const Val& q) const;

private:
	PopUpList          list;
	Index<Val>       key;
	Val              value;
	const Convert     *valueconvert;
	const Display     *valuedisplay;
	i16              dropwidth;
	bool               displayall:1;
	bool               dropfocus:1;
	bool               notnull:1;
	bool               alwaysdrop:1;
	bool               usewheel:1;

	void          Select();
	void          Cancel();
	void          Change(i32 q);
	void          EnableDrop(bool b = true)         { MainButton().Enable(b || alwaysdrop); }
	void          Sync();

	typedef       DropList CLASSNAME;

public:
	typedef MultiButton::Style Style;

	Event<>       WhenDrop;

	DropList&     Add(const Val& key, const Val& value, bool enable = true);
	DropList&     Add(const Val& value)         { return Add(value, value); }
	DropList&     Add(std::initializer_list<std::pair<Val, Val>> init);

	void          Remove(i32 i);
	void          ClearList();
	void          Clear();

	DropList&     AddSeparator();

	void          Drop();

	const Val& operator=(const Val& v)        { SetData(v); return v; }
	operator Val() const                        { return GetData(); }

	void          SetIndex(i32 i)                 { SetData(GetKey(i)); }
	i32           GetIndex() const                { return FindKey(value); }
	void          GoBegin()                       { if(GetCount()) SetIndex(0); }
	void          GoEnd()                         { if(GetCount()) SetIndex(GetCount() - 1); }
	void          GoPrev()                        { Change(-1); }
	void          GoNext()                        { Change(1); }

	bool          HasKey(const Val& k) const    { return FindKey(k) >= 0; }
	i32           FindKey(const Val& k) const;
	i32           Find(const Val& k) const      { return FindKey(k); }
	i32           FindVal(const Val& v) const { return list.Find(v); }

	i32           GetCount() const                { return key.GetCount(); }
	void          Trim(i32 n);
	const Val&  GetKey(i32 i) const             { return key[i]; }

	Val         GetVal(i32 i) const           { return list.Get(i); }
	Val         GetVal() const;
	void          SetVal(i32 i, const Val& v);
	void          SetVal(const Val& v);
	Val         operator[](i32 i) const         { return GetVal(i); }

	void          Adjust();
	void          Adjust(const Val& k);

	ArrayCtrl&    ListObject()                          { return list.Permanent(); }

	DropList&     SetDropLines(i32 d)                   { list.SetDropLines(d); return *this; }
	DropList&     SetValConvert(const Convert& cv);
	DropList&     SetConvert(const Convert& cv);
	DropList&     SetDisplay(i32 i, const Display& d);
	DropList&     SetDisplay(const Display& d);
	DropList&     SetLineCy(i32 i, i32 lcy)             { list.SetLineCy(i, lcy); return *this; }
	DropList&     SetLineCy(i32 lcy)                    { list.SetLineCy(lcy); return *this; }
	DropList&     SetDisplay(const Display& d, i32 lcy);
	DropList&     ValDisplay(const Display& d);
	DropList&     DisplayAll(bool b = true)             { displayall = b; return *this; }
	DropList&     DropFocus(bool b = true)              { dropfocus = b; return *this; }
	DropList&     NoDropFocus()                         { return DropFocus(false); }
	DropList&     AlwaysDrop(bool e = true);
	DropList&     SetStyle(const Style& s)              { MultiButton::SetStyle(s); return *this; }
	DropList&     NotNull(bool b = true)                { notnull = b; return *this; }
	DropList&     DropWidth(i32 w)                      { dropwidth = w; return *this; }
	DropList&     DropWidthZ(i32 w)                     { dropwidth = HorzLayoutZoom(w); return *this; }
	DropList&     Wheel(bool b = true)                  { usewheel = b; return *this; }
	DropList&     NoWheel()                             { return Wheel(false); }

	DropList&     SetScrollBarStyle(const ScrollBar::Style& s) { list.SetScrollBarStyle(s); return *this; }

	DropList();
	virtual ~DropList();
};

void Append(DropList& list, const VecMap<Val, Val>& values);
void Append(DropList& list, const VecMap<i32, Txt>& values);
void Append(MapConvert& convert, const VecMap<Val, Val>& values);
void Append(MapConvert& convert, const VecMap<i32, Txt>& values);
void Append(DropList& list, const MapConvert& convert);

void operator*=(DropList& list, const VecMap<Val, Val>& values);
void operator*=(DropList& list, const VecMap<i32, Txt>& values);
void operator*=(MapConvert& convert, const VecMap<Val, Val>& values);
void operator*=(MapConvert& convert, const VecMap<i32, Txt>& values);
void operator*=(DropList& list, const MapConvert& convert);

class DropChoice : public MultiButtonFrame {
public:
	virtual void       Serialize(Stream& s); //empty

protected:
	PopUpList          list;
	Ctrl              *owner;
	bool               appending : 1;
	bool               dropfocus : 1;
	bool               always_drop : 1;
	bool               hide_drop : 1;
	bool               updownkeys : 1;
	bool               rodrop : 1;

	void        Select();
	void        Drop();
	void        EnableDrop(bool b);
	void        PseudoPush();

	i32         dropwidth;

	typedef DropChoice CLASSNAME;

public:
	Event<>     WhenDrop;
	Event<>     WhenSelect;

	bool        DoKey(dword key);
	void        DoWheel(i32 zdelta);

	void        Clear();
	void        Add(const Val& data);
	i32         Find(const Val& data) const         { return list.Find(data); }
	void        FindAdd(const Val& data);
	void        Set(i32 i, const Val& data)         { list.Set(i, data); }
	void        Remove(i32 i);
	void        SerializeList(Stream& s);

	i32         GetCount() const                      { return list.GetCount(); }
	Val       Get(i32 i) const                      { return list.Get(i); }

	void        AddHistory(const Val& data, i32 max = 12);

	void        AddTo(Ctrl& _owner);
	bool        IsActive() const                      { return IsOpen(); }

	Val       Get() const;
	i32         GetIndex() const;

	DropChoice& SetDisplay(i32 i, const Display& d)   { list.SetDisplay(i, d); return *this; }
	DropChoice& SetDisplay(const Display& d)          { list.SetDisplay(d); return *this; }
	DropChoice& SetLineCy(i32 lcy)                    { list.SetLineCy(lcy); return *this; }
	DropChoice& SetDisplay(const Display& d, i32 lcy) { SetDisplay(d); SetLineCy(lcy); return *this; }
	DropChoice& SetConvert(const Convert& d)          { list.SetConvert(d); return *this; }
	DropChoice& SetDropLines(i32 n)                   { list.SetDropLines(n); return *this; }
	DropChoice& Appending()                           { appending = true; return *this; }
	DropChoice& AlwaysDrop(bool e = true);
	DropChoice& HideDrop(bool e = true)               { hide_drop = e; AlwaysDrop(always_drop); return *this; }
	DropChoice& RdOnlyDrop(bool e = true)             { rodrop = e; return *this; }
	DropChoice& NoDropFocus()                         { dropfocus = false; return *this; }

	DropChoice& DropWidth(i32 w)                      { dropwidth = w; return *this; }
	DropChoice& DropWidthZ(i32 w)                     { dropwidth = HorzLayoutZoom(w); return *this; }
	DropChoice& UpDownKeys(bool b = true)             { updownkeys = b; return *this; }

	DropChoice& SetScrollBarStyle(const ScrollBar::Style& s) { list.SetScrollBarStyle(s); return *this; }

	DropChoice();

	static bool DataSelect(Ctrl& owner, DropChoice& drop, const Txt& appends);
};

template <class T>
class WithDropChoice : public T {
public:
	virtual bool   Key(dword key, i32 repcnt);
	virtual void   MouseEnter(Point p, dword keyflags);
	virtual void   MouseLeave();
	virtual void   MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual void   GotFocus();
	virtual void   LostFocus();

protected:
	DropChoice      select;
	Txt          appends;
	bool            withwheel;

	void            DoWhenSelect();
	void            DoWhenDrop()                          { WhenDrop(); }

public:
	Event<>         WhenDrop;
	Event<>         WhenSelect;

	void            ClearList()                           { select.Clear(); }
	void            AddList(const Val& data)            { select.Add(data); }
	void            FindAddList(const Val& data)        { select.FindAdd(data); }
	i32             Find(const Val& data) const         { return select.Find(data); }
	void            Set(i32 i, const Val& data)         { select.Set(i, data); }
	void            Remove(i32 i)                         { select.Remove(i); }
	void            SerializeList(Stream& s)              { select.SerializeList(s); }

	i32             GetCount() const                      { return select.GetCount(); }
	Val           Get(i32 i) const                      { return select.Get(i); }

	void            AddHistory(i32 max = 12)              { select.AddHistory(this->GetData(), max); }

	MultiButton::SubButton& AddButton()                   { return select.AddButton(); }
	i32                     GetButtonCount() const        { return select.GetButtonCount(); }
	MultiButton::SubButton& GetButton(i32 i)              { return select.GetButton(i); }
	Rect                    GetPushScreenRect() const     { return select.GetPushScreenRect(); }

	const MultiButton::Style& StyleDefault()              { return select.StyleFrame(); }
	WithDropChoice& SetStyle(const MultiButton::Style& s) { select.SetStyle(s); return *this; }

	WithDropChoice& Dropping(bool b = true)               { select.MainButton().Show(b); return *this; }
	WithDropChoice& NoDropping()                          { return Dropping(false); }
	WithDropChoice& NoDropFocus()                         { select.NoDropFocus(); return *this; }
	WithDropChoice& Appending(const Txt& s = ", ")     { appends = s; select.Appending(); return *this; }
	WithDropChoice& SetDropLines(i32 n)                   { select.SetDropLines(n); return *this; }
	WithDropChoice& SetDisplay(i32 i, const Display& d)   { select.SetDisplay(i, d); return *this; }
	WithDropChoice& SetDisplay(const Display& d)          { select.SetDisplay(d); return *this; }
	WithDropChoice& SetLineCy(i32 lcy)                    { select.SetLineCy(lcy); return *this; }
	WithDropChoice& SetDisplay(const Display& d, i32 lcy) { select.SetDisplay(d, lcy); return *this; }
	WithDropChoice& SetConvert(const Convert& d)          { select.SetConvert(d); return *this; }
	WithDropChoice& AlwaysDrop(bool b = true)             { select.AlwaysDrop(b); return *this; }
	WithDropChoice& HideDrop(bool b = true)               { select.HideDrop(b); return *this; }
	WithDropChoice& RdOnlyDrop(bool b = true)             { select.RdOnlyDrop(b); return *this; }
	WithDropChoice& WithWheel(bool b = true)              { withwheel = b; return *this; }
	WithDropChoice& NoWithWheel()                         { return WithWheel(false); }
	WithDropChoice& DropWidth(i32 w)                      { select.DropWidth(w); return *this; }
	WithDropChoice& DropWidthZ(i32 w)                     { select.DropWidthZ(w); return *this; }
	WithDropChoice& UpDownKeys(bool b = true)             { select.UpDownKeys(b); return *this; }
	WithDropChoice& NoUpDownKeys()                        { return UpDownKeys(false); }

	WithDropChoice();
};

template <class T>
WithDropChoice<T>::WithDropChoice() {
	select.WhenDrop = callback(this, &WithDropChoice::DoWhenDrop);
	select.WhenSelect = callback(this, &WithDropChoice::DoWhenSelect);
	appends = Txt::GetVoid();
	withwheel = true;
	SetStyle(StyleDefault());
	select.AddTo(*this);
}

template <class T>
bool WithDropChoice<T>::Key(dword key, i32 repcnt) {
	return select.DoKey(key) || T::Key(key, repcnt);
}

template <class T>
void WithDropChoice<T>::MouseWheel(Point p, i32 zdelta, dword keyflags)
{
	if(withwheel)
		select.DoWheel(zdelta);
}

template <class T>
void WithDropChoice<T>::MouseEnter(Point p, dword keyflags)
{
	select.Refresh();
	T::MouseEnter(p, keyflags);
}

template <class T>
void WithDropChoice<T>::MouseLeave()
{
	select.Refresh();
	T::MouseLeave();
}

template <class T>
void WithDropChoice<T>::GotFocus()
{
	select.Refresh();
	T::GotFocus();
}

template <class T>
void WithDropChoice<T>::LostFocus()
{
	select.Refresh();
	T::LostFocus();
}

template <class T>
void WithDropChoice<T>::DoWhenSelect() {
	if(DropChoice::DataSelect(*this, select, appends)) {
		this->SetFocus();
		WhenSelect();
	}
}