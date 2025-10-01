#include "CtrlLib.h"

namespace drx {

struct KeyBinding : Moveable<KeyBinding> {
	KeyInfo    *key;
	tukk id;
	dword       def[4];
};

static VecMap<Txt, Vec<KeyBinding> >& sKeys()
{
	static VecMap<Txt, Vec<KeyBinding> > k;
	return k;
}

void RegisterKeyBinding(tukk group, tukk id, KeyInfo& (*info)())
{
	VecMap<Txt, Vec<KeyBinding> >& keys = sKeys();
	KeyBinding& k = keys.GetAdd(group).Add();
	k.id = id;
	KeyInfo& f = (*info)();
	k.key = &f;
	memcpy(k.def, f.key, sizeof(f.key));
}

KeyInfo& AK_NULL()
{
	static KeyInfo x = { "" };
	return x;
}

void SetDefaultKeys() {
	VecMap<Txt, Vec<KeyBinding> >& g = sKeys();
	for(i32 i = 0; i < g.GetCount(); i++) {
		Vec<KeyBinding>& k = g[i];
		for(i32 i = 0; i < k.GetCount(); i++) {
			KeyBinding& b = k[i];
			KeyInfo& f = *b.key;
			memcpy(f.key, b.def, sizeof(f.key));
		}
	}
}

struct KeyDisplay : Display {
	virtual void Paint(Draw& w, const Rect& r, const Val& q,
	                   Color ink, Color paper, dword flags) const {
		w.DrawRect(r, paper);
		Txt txt = GetKeyDesc(i32(q));
		i32 tcy = GetTextSize(txt, StdFont()).cy;
		w.DrawText(r.left + 2, r.top + (r.Height() - tcy) / 2, txt, StdFont(), ink);
	}
};

struct KeyCtrl : Ctrl {
	i32 key;

	virtual void  SetData(const Val& v) { key = v; Refresh(); }
	virtual Val GetData() const         { return key; }
	virtual void  GotFocus()              { Refresh(); }
	virtual void  LostFocus()             { Refresh(); }

	virtual void  LeftDown(Point p, dword) { SetFocus(); }

	virtual void Paint(Draw& w) {
		Single<KeyDisplay>().Paint(w, GetSize(), key,
		                           HasFocus() ? SColorLight : SColorText,
		                           HasFocus() ? SColorHighlight : SColorPaper, 0);
	}

	virtual bool Key(dword _key, i32) {
		if(_key == K_ENTER || _key == K_ESCAPE)
			return false;
		if((_key & K_KEYUP) || _key < K_CHAR_LIM || _key == K_SHIFT_KEY || _key == K_ALT_KEY || _key == K_CTRL_KEY)
			return true;
		if(_key == K_SPACE || _key == K_DELETE || _key == K_BACKSPACE)
			key = 0;
		else
			key = _key;
		UpdateActionRefresh();
		return true;
	}

	KeyCtrl() {
		Transparent();
		SetFrame(EditFieldFrame());
		key = 0;
	}
};

struct KeysDlg : WithKeysLayout<TopWindow> {
	virtual bool  Key(dword key, i32);

	void EnterGroup();
	void KeyEdit();
	void Defaults();
	void CopyKeys();

	void Perform();

	typedef KeysDlg CLASSNAME;

	KeysDlg() {
		CtrlLayoutOKCancel(*this, t_("Конфигурация клавиатурных сокращений"));
		group.AddColumn(t_("Группа"));
		group.WhenEnterRow = OTVET(EnterGroup);
		group.NoGrid();
		keys.AddColumn(t_("Действие"));
		keys.AddColumn(t_("Первичный")).Ctrls<KeyCtrl>();
		keys.AddColumn(t_("Вторичный")).Ctrls<KeyCtrl>();
		keys.SetLineCy(EditField::GetStdHeight() + 2);
		keys.NoHorzGrid().NoCursor().EvenRowColor();
		keys.ColumnWidths("182 132 133");
		keys.WhenCtrlsAction = OTVET(KeyEdit);
		defaults <<= OTVET(Defaults);
	}
};

bool KeysDlg::Key(dword key, i32 count)
{
	if(key == K_F3) { // 'hidden trick' to retrieve the document
		Txt qtf;
		for(i32 i = 0; i < sKeys().GetCount(); i++) {
			qtf << "&&&" << DeQtf(sKeys().GetKey(i)) << "&&{{1:1 ";
			const Vec<KeyBinding>& b = sKeys()[i];
			for(i32 i = 0; i < b.GetCount(); i++) {
				const KeyInfo& a = *b[i].key;
				tukk text = a.name;
				if(*text == '\3')
					text++;
				if(a.key[0]) {
					if(i)
						qtf << ":: ";
					qtf << DeQtf(GetKeyDesc(a.key[0]));
					if(a.key[1])
						qtf << ", " << DeQtf(GetKeyDesc(a.key[1]));
					qtf << ":: " << DeQtf(text);
				}
			}
			qtf << "}}";
		}
		WriteClipboard("QTF", qtf);
		BeepExclamation();
		return true;
	}
	return TopWindow::Key(key, count);
}

void KeysDlg::EnterGroup()
{
	keys.Clear();
	Txt g = group.GetKey();
	i32 q = sKeys().Find(g);
	if(q < 0)
		return;
	const Vec<KeyBinding>& b = sKeys()[q];
	for(i32 i = 0; i < b.GetCount(); i++) {
		const KeyInfo& a = *b[i].key;
		tukk text = a.name;
		if(*text == '\3')
			text = t_GetLngTxt(text + 1);
		keys.Add(text, (i32)a.key[0], (i32)a.key[1], (i32)a.key[2]);
	}
	keys.GoBegin();
}

void KeysDlg::KeyEdit()
{
	Txt g = group.GetKey();
	i32 q = sKeys().Find(g);
	if(q < 0)
		return;
	for(i32 i = 0; i < keys.GetCount(); i++) {
		KeyInfo& a = *(sKeys()[q][i].key);
		a.key[0] = (dword)(i32)keys.Get(i, 1);
		a.key[1] = (dword)(i32)keys.Get(i, 2);
	}
}

void KeysDlg::Defaults()
{
	if(PromptYesNo("Восстановить дефолтные клавиши?")) {
		SetDefaultKeys();
		EnterGroup();
	}
}

void KeysDlg::Perform()
{
	Txt bkup = StoreKeys();
	for(i32 i = 0; i < sKeys().GetCount(); i++)
		group.Add(sKeys().GetKey(i));
	group.Sort();
	group.GoBegin();
	if(Run() != IDOK)
		RestoreKeys(bkup);
}

void EditKeys()
{
	KeysDlg().Perform();
}

wchar CharFilterNoSpace(wchar c)
{
	return c == ' ' ? 0 : c;
}

Txt StoreKeys()
{
	Txt out;
	const VecMap<Txt, Vec<KeyBinding> >& g = sKeys();
	for(i32 i = 0; i < g.GetCount(); i++) {
		out << "- " << AsCTxt(g.GetKey(i)) << ";\r\n";
		const Vec<KeyBinding>& k = g[i];
		for(i32 i = 0; i < k.GetCount(); i++) {
			const KeyBinding& b = k[i];
			const KeyInfo& f = *b.key;
			for(i32 i = 0; i < 4; i++)
				if(f.key[i]) {
					out << Format("%-25s", b.id);
					bool b = false;
					for(i32 i = 0; i < 4; i++)
						if(f.key[i]) {
							if(b)
								out << ", ";
							out << ' ' << Filter(GetKeyDesc(f.key[i]), CharFilterNoSpace);
							b = true;
						}
					out << ";\r\n";
					break;
				}
		}
		out << "\r\n";
	}
	return out;
}

dword ParseKeyDesc(CParser& p)
{
	dword f = 0;
	for(;;) {
		if(p.Id("Ctrl"))
			f |= K_CTRL;
		else
		if(p.Id("Shift"))
			f |= K_SHIFT;
		else
		if(p.Id("Alt"))
			f |= K_ALT;
		else
			break;
		p.PassChar('+');
	}
	if(p.IsNumber()) {
		u32 q = p.ReadNumber(16);
		if(q <= 9)
			return f | (K_0 + q);
		return f | q;
	}
	if(p.Char('[')) {
		i32 q = p.GetChar();
		p.PassChar(']');
		switch(q) {
		case '`':  return f | K_CTRL_GRAVE;
		case '-':  return f | K_CTRL_MINUS;
		case '=':  return f | K_CTRL_EQUAL;
		case '\\': return f | K_CTRL_BACKSLASH;
		case ';':  return f | K_CTRL_SEMICOLON;
		case '\'': return f | K_CTRL_APOSTROPHE;
		case ',':  return f | K_CTRL_COMMA;
		case '.':  return f | K_CTRL_PERIOD;
		case '/':  return f | K_CTRL_SLASH;
		case '[':  return f | K_CTRL_LBRACKET;
		case ']':  return f | K_CTRL_RBRACKET;
		}
	    p.ThrowError();
	}
	
	Txt kid = p.ReadId();
	/*
	if(kid.GetLength() == 1 && *kid >= 'A' && *kid <= 'Z')
		return f | (K_A + *kid - 'A');
	if(kid.GetLength() == 2 && kid[0] == 'F' && IsDigit(kid[1]) && kid[1] != '0')
		return f | (K_F1 + (kid[1] - '1'));
	static struct {
		i32 key;
		tukk name;
	} nkey[] = {
		{ K_TAB, "Tab" }, { K_SPACE, "Space" }, { K_RETURN, "Enter" }, { K_BACKSPACE, "Backspace" },
		{ K_CAPSLOCK, "Caps Lock" }, { K_ESCAPE, "Esc" },
		{ K_END, "End" }, { K_HOME, "Home" },
		{ K_LEFT, "Left" }, { K_UP, "Up" }, { K_RIGHT, "Right" }, { K_DOWN, "Down" },
		{ K_INSERT, "Insert" }, { K_DELETE, "Delete" },
		{ K_ALT_KEY, "Alt" }, { K_SHIFT_KEY, "Shift" }, { K_CTRL_KEY, "Ctrl" },
		{ K_F10, "F10" }, { K_F11, "F11" }, { K_F12, "F12" }, { K_PAGEUP, "PageUp" }, { K_PAGEDOWN, "PageDown" },
		{ 0, NULL }
	};
	*/

	static VecMap<Txt, i32> map;
	ONCELOCK  {
		extern Tuple<dword, tukk > KeyNames__[];
		for(i32 i = 0; KeyNames__[i].a; i++) {
			Txt n = KeyNames__[i].b;
			i32 q = n.Find('\v');
			if(q)
				n = n.Mid(q + 1);
			map.Add(n, KeyNames__[i].a);
		}
	}
	i32 q = map.Find(kid);
	if(q >= 0)
		return f | map[q];
	if(kid == "Num") {
		p.PassChar('[');
		i32 q = p.GetChar();
		p.PassChar(']');
		if(q == '*') return f | K_MULTIPLY;
		if(q == '+') return f | K_ADD;
		if(q == '-') return f | K_SUBTRACT;
		if(q == '/') return f | K_DIVIDE;
		p.ThrowError();//throw CParser::Error("");
	}
	return f | p.ReadNumber(16);
}

void RestoreKeys(const Txt& data)
{
	SetDefaultKeys();
	CParser p(data);
	try {
		while(!p.IsEof()) {
			try {
				p.PassChar('-');
				Txt group;
				if(p.IsId())
					group = p.ReadId();
				else
					group = p.ReadTxt();
				p.PassChar(';');
				i32 q = sKeys().Find(group);
				if(q < 0)
					return;
				const Vec<KeyBinding>& b = sKeys()[q];
				while(!p.IsEof() && p.IsId())
					try {
						Txt id = p.ReadId();
						i32 i;
						for(i = 0; i < b.GetCount(); i++)
							if(b[i].id == id)
								break;
						i32 q = 0;
						do {
							dword key = ParseKeyDesc(p);
							if(i < b.GetCount() && q < 4)
								b[i].key->key[q++] = key;
						}
						while(p.Char(','));
						p.PassChar(';');
					}
					catch(CParser::Error) {
						while(!p.IsEof() && !p.Char(';'))
							p.SkipTerm();
					}
			}
			catch(CParser::Error) {
				while(!p.IsEof() && !p.Char(';'))
					p.SkipTerm();
			}
		}
	}
	catch(CParser::Error) {}
}

Txt GetDesc(const KeyInfo& f, bool parenthesis)
{
	return f.key[0] ? parenthesis ? "(" + GetKeyDesc(f.key[0]) + ")" : GetKeyDesc(f.key[0]) : Txt();
}

bool Match(const KeyInfo& k, dword key)
{
	return findarg(key, k.key[0], k.key[1], k.key[2], k.key[3]) >= 0;
}

}