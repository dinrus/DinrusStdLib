#ifndef _DropGrid_DropGrid_h_
#define _DropGrid_DropGrid_h_

#include <drx/CtrlLib/CtrlLib.h>
#include <drx/GridCtrl/GridCtrl.h>

namespace drx {

class DropGrid : public Convert, public GridDisplay, public Ctrl
{
	public:
		enum
		{
			BTN_SELECT,
			BTN_LEFT,
			BTN_RIGHT,
			BTN_UP,
			BTN_DOWN,
			BTN_PLUS,
			BTN_CLEAN
		};

		class PopUpGrid : public GridCtrl
		{
			private:

				void CloseNoData();
				void CloseData();

			public:
				Callback WhenPopDown;
				Callback WhenClose;
				Callback WhenCloseData;
				Callback WhenCloseNoData;

				typedef PopUpGrid CLASSNAME;

				virtual void Deactivate();
				void PopUp(Ctrl *owner, const Rect &r);

				PopUpGrid();
		};

	protected:

		i32 key_col;
		i32 find_col;
		i32 value_col;
		Vec<i32> value_cols;
		PopUpGrid list;
		MultiButtonFrame drop;
		GridButton clear;

	private:
	
		i32 rowid;
		i32 trowid;
		Val value;
		bool change;

		i32 list_width;
		i32 list_height;
		i32 drop_lines;

		bool display_all:1;
		bool header:1;
		bool valuekey:1;
		bool notnull:1;
		bool display_columns:1;
		bool drop_enter:1;
		bool data_action:1;
		bool searching:1;
		bool must_change:1;
		bool null_action:1;
		bool clear_button:1;
		bool nodrop:1;

		GridDisplay *display;
		
		Txt must_change_str;

		void Change(i32 dir);
		void SearchCursor();
		void DoAction(i32 row, bool action = true, bool chg = true);
		Vec<Txt> MakeVector(i32 r) const;
		Val MakeVal(i32 r = -1, bool columns = true) const;
		void UpdateVal();
		Val Format0(const Val& q, i32 rowid) const;

	public:

		typedef DropGrid CLASSNAME;
		DropGrid();

		void Close();
		void CloseData();
		void CloseNoData();
		void Drop();

		DropGrid& Width(i32 w);
		DropGrid& Height(i32 h);
		DropGrid& SetKeyColumn(i32 n);
		DropGrid& SetFindColumn(i32 n);
		DropGrid& SetValColumn(i32 n);
		DropGrid& AddValColumn(i32 n);
		DropGrid& AddValColumns(i32 first = -1, i32 last = -1);
		DropGrid& DisplayAll(bool b = true);
		DropGrid& SetDropLines(i32 d);
		DropGrid& Header(bool b = true);
		DropGrid& NoHeader();
		DropGrid& Resizeable(bool b = true);
		DropGrid& ColorRows(bool b = true);
		DropGrid& NotNull(bool b = true);
		DropGrid& ValAsKey(bool b = true);
		DropGrid& SetDisplay(GridDisplay &d);
		DropGrid& DisplayColumns(bool b = true);
		DropGrid& DropEnter(bool b = true);
		DropGrid& DataAction(bool b = true);
		DropGrid& Searching(bool b = true);
		DropGrid& MustChange(bool b = true, tukk s = "");
		DropGrid& NullAction(bool b = true);
		DropGrid& ClearButton(bool b = true);
		DropGrid& NoDrop(bool b = true);
		DropGrid& SetData();
		DropGrid& SearchHideRows(bool b = true);

		GridCtrl::ItemRect& AddColumn(tukk name, i32 width = -1, bool idx = false);
		GridCtrl::ItemRect& AddColumn(Id id, tukk name, i32 width = -1, bool idx = false);
		GridCtrl::ItemRect& AddIndex(tukk name = "");
		GridCtrl::ItemRect& AddIndex(Id id);

		MultiButton::SubButton& AddButton(i32 type, const Callback &cb);
		MultiButton::SubButton& AddSelect(const Callback &cb);
		MultiButton::SubButton& AddPlus(const Callback &cb);
		MultiButton::SubButton& AddEdit(const Callback &cb);
		MultiButton::SubButton& AddClear();
		MultiButton::SubButton& AddText(tukk label, const Callback& cb);
		MultiButton::SubButton& GetButton(i32 n);
		
		i32 AddColumns(i32 cnt);

		void GoTop();

		i32 SetIndex(i32 n);
		i32 GetIndex() const;

		i32 GetCount() const;

		void Reset();
		void Clear();
		void Ready(bool b = true);
		void ClearVal();
		void DoClearVal();

		virtual Val GetData() const;
		virtual void SetData(const Val& v);

		Val GetVal() const;
		Val GetVal(i32 r) const;
		Val FindVal(const Val& v) const;
		Vec<Txt> FindVector(const Val& v) const;
		bool FindMove(const Val& v);
		Val GetKey() const;

		virtual bool Key(dword k, i32);
		virtual void Paint(Draw& draw);
		virtual void LeftDown(Point p, dword keyflags);
		virtual void GotFocus();
		virtual void LostFocus();
		virtual void Serialize(Stream& s);
		virtual bool Accept();
		virtual Size GetMinSize() const;
		virtual void State(i32 reason);

		void Paint0(Draw &w, i32 lm, i32 rm, i32 x, i32 y, i32 cx, i32 cy, const Val &val, dword style, Color &fg, Color &bg, Font &fnt, bool found = false, i32 fs = 0, i32 fe = 0);
		virtual void Paint(Draw &w, i32 x, i32 y, i32 cx, i32 cy, const Val &val, dword style, Color &fg, Color &bg, Font &fnt, bool found = false, i32 fs = 0, i32 fe = 0);

		Val Get(i32 c) const;
		Val Get(Id id) const;
		Val Get(i32 r, i32 c) const;
		Val Get(i32 r, Id id) const;
		Val GetPrev(i32 c) const;
		Val GetPrev(Id id) const;
		void  Set(i32 c, const Val& v);
		void  Set(Id id, const Val& v);
		void  Set(i32 r, i32 c, const Val& v);
		void  Set(i32 r, Id id, const Val& v);
		void  Set(i32 r, const Vec<Val> &v, i32 data_offset = 0, i32 column_offset = 0);
		void  Add(const Vec<Val> &v, i32 offset = 0, i32 count = -1, bool hidden = false);
		Txt GetTxt(Id id);

		Val& operator() (i32 r, i32 c);
		Val& operator() (i32 c);
		Val& operator() (Id id);
		Val& operator() (i32 r, Id id);

		GridCtrl::ItemRect& GetRow(i32 r);

		bool IsSelected();
		bool IsEmpty();
		bool IsChange();
		bool IsInit();
		
		void ClearChange();

		i32 Find(const Val& v, i32 col = 0, i32 opt = 0);
		i32 Find(const Val& v, Id id, i32 opt = 0);
		i32 GetCurrentRow() const;

		void CancelUpdate();

		GridCtrl& GetList() { return list; }

		virtual Val Format(const Val& q) const;
		
		Callback WhenLeftDown;
		Callback WhenDrop;

		GridCtrl::ItemRect& AddRow(i32 n = 1, i32 size = -1);
		DropGrid& Add() { AddRow(); return *this; }

		//$-DropCtrl& Add(const Val& [, const Val& ]...);
		#define  E__Add(I)      DropGrid& Add(__List##I(E__Val));
			__Expand(E__Add)
		#undef   E__Add
		//$+

		//$-GridCtrl::ItemRect& Add(const Val& [, const Val& ]...);
		#define  E__Add(I)      GridCtrl::ItemRect& AddRow(__List##I(E__Val));
			__Expand(E__Add)
		#undef   E__Add
		//$+

		DropGrid& AddSeparator(Color c);

};

}

#endif
