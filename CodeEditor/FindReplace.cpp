#include "CodeEditor.h"

namespace drx {

void CodeEditor::InitFindReplace()
{
	findreplace.find.AddButton().SetMonoImage(CtrlImg::smallright()).Tip("Уайлдкард")
		<<= OTVET(FindWildcard);
	findreplace.replace.AddButton().SetMonoImage(CtrlImg::smallright()).Tip("Уайлдкард")
		<<= OTVET(ReplaceWildcard);
	PutI(findreplace.find);
	PutI(findreplace.replace);
	findreplace.amend <<= OTVET(Replace);
	findreplace.amend_all <<= OTVET1(ReplaceAll, false);
	findreplace.amend_rest <<= OTVET1(ReplaceAll, true);
	findreplace.prev <<= OTVET(DoFindBack);
	findreplace.replacing = false;
	found = notfoundfw = notfoundbk = foundsel = false;
	persistent_find_replace = false;
	findreplace.find <<= findreplace.wholeword <<= findreplace.wildcards
	                 <<= findreplace.incremental <<= findreplace.regexp
	                 <<= findreplace.ignorecase <<= OTVET(IncrementalFind);
	ff_start_pos = -1;
	findreplace.find_all << [=, this] { FindAll(); };
}

FindReplaceDlg::FindReplaceDlg()
{
	find.NoUpDownKeys();
	samecase <<= true;
	close.Cancel();
	prev.SetImage(CtrlImg::SmallUp());
	amend.SetImage(CodeEditorImg::Replace());
	amend_all.SetImage(CodeEditorImg::ReplaceAll());
	amend_rest.SetImage(CodeEditorImg::ReplaceRest());
	find_all.SetImage(CodeEditorImg::FindAll());
	incremental <<= true;
	mode <<= OTVET(Sync);
	mode.Hide();
	Sync();
	
	find.NullText("Найти");
	replace.NullText("Заменить");
}

void FindReplaceDlg::Sync()
{
	samecase.Enable(ignorecase);
	bool b = !regexp;
	wildcards.Enable(b);
	prev.Enable(b);
	ignorecase.Enable(b);
	wholeword.Enable(b);
	incremental_from_cursor.Enable(IsIncremental());
	b = !mode.IsVisible() || ~mode == 0;
	replace.Enable(b);
}

dword CodeEditor::find_next_key = K_F3;
dword CodeEditor::find_prev_key = K_SHIFT_F3;
dword CodeEditor::replace_key = K_CTRL_R;

bool FindReplaceDlg::Key(dword key, i32 cnt) {
	if(key == K_CTRL_I) {
		if(find.HasFocus()) {
			find <<= itext;
			return true;
		}
		if(replace.HasFocus()) {
			replace <<= itext;
			return true;
		}
	}
	if(key == K_ENTER) {
		next.WhenAction();
		return true;
	}
	if(findarg(key, K_TAB, K_SHIFT_TAB) >= 0 && replace.IsShown()) {
		if(find.HasFocus())
			replace.SetFocus();
		else
			find.SetFocus();
		return true;
	}
	if(key == K_ESCAPE) {
		close.WhenAction();
		return true;
	}
	return TopWindow::Key(key, cnt);
}

void FindReplaceDlg::Setup(bool doreplace)
{
	CtrlLayout(*this);
	close.SetImage(CodeEditorImg::Cancel());
	close.Tip("Закрыть (Esc)");
	close.Normal();
	close.SetLabel("");
	next.SetImage(CtrlImg::SmallDown());
	next.Tip("Найти далее (" + GetKeyDesc(CodeEditor::find_next_key) + ")");
	next.Normal();
	next.SetLabel("");
	prev.Tip("Найти ранее (" + GetKeyDesc(CodeEditor::find_prev_key) + ")");
	amend.Tip("Заменить (" + GetKeyDesc(CodeEditor::replace_key) + ")");
	find_all.Show();
	find_all.Tip("Найти все");
	amend.Disable();
	replacing = doreplace;
	replace.Show(replacing);
	amend.Show(replacing);
	amend_all.Show(replacing);
	amend_rest.Show(replacing);
	Height(doreplace ? GetLayoutSize().cy : replace.GetRect().top);
	SetFrame(TopSeparatorFrame());
	Sync();
}

void CodeEditor::SetFound(i32 fi, i32 type, const WTxt& text)
{
	Found& f = foundwild.At(fi);
	f.type = type;
	f.text = text;
}

i32 CodeEditor::Match(wtukk f, wtukk s, i32 line, bool we, bool ignorecase, i32 fi)
{
	wtukk b = s;
	i32 n = 0;
	WTxt ln;
	while(*f) {
		if(*f == WILDANY) {
			f++;
			WTxt wild;
			for(;;) {
				i32 nn = Match(f, s, line, we, ignorecase, fi + 1);
				if(nn >= 0) {
					SetFound(fi, WILDANY, wild);
					return i32(s - b) + n + nn;
				}
				wild.Cat(*s ? *s : '\n');
				if(!*s++) return -1;
			}
			return -1;
		}
		else
		if(*f == WILDONE) {
			if(!*s) return -1;
			SetFound(fi++, WILDONE, WTxt(*s, 1));
			s++;
		}
		else
		if(*f == WILDSPACE) {
			wtukk wb = s;
			if(*s != ' ' && *s != '\t') return -1;
			s++;
			while(*s == ' ' || *s == '\t')
				s++;
			SetFound(fi++, WILDSPACE, WTxt(wb, s));
		}
		else
		if(*f == WILDNUMBER) {
			wtukk wb = s;
			if(*s < '0' || *s > '9') return -1;
			s++;
			while(*s >= '0' && *s <= '9')
				s++;
			SetFound(fi++, WILDNUMBER, WTxt(wb, s));
		}
		else
		if(*f == WILDID) {
			wtukk wb = s;
			if(!iscib(*s)) return -1;
			s++;
			while(iscidl(*s)) s++;
			SetFound(fi++, WILDID, WTxt(wb, s));
		}
		else
		if(*f == '\n') {
			if(*s != '\0' || ++line >= GetLineCount()) return -1;
			n += i32(s - b) + 1;
			ln = GetWLine(line);
			s = b = ln;
		}
		else {
			if(ignorecase ? ToUpper(*s) != ToUpper(*f) : *s != *f) return -1;
			s++;
		}
		f++;
	}
	return we && iscidl(*s) ? -1 : i32(s - b) + n;
}

bool CodeEditor::Find(bool back, bool block)
{
	foundsel = true;
	if(notfoundfw) MoveTextBegin();
	if(notfoundbk) MoveTextEnd();
	foundsel = false;
	z64 cursor, pos;
	if(found)
		GetSelection(pos, cursor);
	else
		GetSelection(cursor, pos);
	pos = cursor;
	bool b = FindFrom(pos, back, block);
	findreplace.amend.Enable(b);
	return b;
}

bool CodeEditor::RegExpFind(z64 pos, bool block)
{
	RegExp regex((Txt)~findreplace.find);
	
	i32 line = GetLinePos64(pos);
	Txt ln = ToUtf8(GetWLine(line).Mid(LimitSize(pos)));
	for(;;) {
		if(regex.Match(ln)) {
			for(i32 i = 0; i < regex.GetCount(); i++)
				SetFound(i, WILDANY, regex.GetTxt(i).ToWTxt());
			i32 off = regex.GetOffset();
			i32 len = Utf32Len(~ln + off, regex.GetLength());
			pos = GetPos64(line, Utf32Len(~ln, off) + (i32)pos);
			foundtext = GetW(pos, len);
			if(!block) {
				foundsel = true;
				SetSelection(pos, pos + len);
				foundsel = false;
				CenterCursor();
			}
			foundpos = pos;
			foundsize = len;
			found = true;
			return true;
		}
		if(++line >= GetLineCount()) {
			WaitView(line);
			if(line >= GetLineCount())
				return false;
		}
		ln = GetUtf8Line(line);
		pos = 0;
		if(!SearchProgress(line))
			return false;
	}
}

void CodeEditor::FindAll()
{
	Vec<Tuple<z64, i32>> found;
	foundpos = 0;
	while(FindFrom(foundpos, false, true)) {
		found.Add(MakeTuple(foundpos, foundsize));
		foundpos += foundsize;
		if(found.GetCount() >= 10000) {
			Exclamation("Слишком много совпадений, будут показаны только первые 10000.");
			break;
		}
	}
	WhenFindAll(found);
}

bool CodeEditor::FindFrom(z64 pos, bool back, bool block)
{
	notfoundbk = notfoundfw = false;
	if(findreplace.regexp) {
		if(RegExpFind(pos, block))
			return true;
		if(back)
			notfoundbk = true;
		else
			notfoundfw = true;
		return false;
	}
	WTxt text = ~findreplace.find;
	WTxt ft;
	wtukk s = text;
	while(*s) {
		i32 c = *s++;
		if(c == '\\') {
			c = *s++;
			if(c == '\0') break;
			if(c == 'n')
				ft.Cat('\n');
			else
			if(c == 't')
				ft.Cat('\t');
			else
			if(c >= ' ')
				ft.Cat(c);
		}
		else
		if(c >= ' ') {
			if(findreplace.wildcards)
				ft.Cat(c == '*' ? WILDANY :
					   c == '?' ? WILDONE :
					   c == '%' ? WILDSPACE :
					   c == '#' ? WILDNUMBER :
					   c == '$' ? WILDID :
								  c
				);
			else
				ft.Cat(c);
		}
	}
	bool wb = findreplace.wholeword ? iscidl(*ft) : false;
	bool we = findreplace.wholeword ? iscidl(*ft.Last()) : false;
	if(ft.IsEmpty()) return false;
	foundwild.Clear();
	i32 line = GetLinePos64(pos);
	WTxt ln = GetWLine(line);
	wtukk l = ln;
	s = l + pos;
	bool ignorecase = findreplace.ignorecase;
	for(;;) {
		for(;;) {
			if(!wb || (s == l || !iscidl(s[-1]))) {
				i32 n = Match(ft, s, line, we, ignorecase);
				if(n >= 0) {
					z64 pos = GetPos64(line, i32(s - l));
					foundtext = GetW(pos, n);
					if(!back || pos + n < cursor) {
						if(!block) {
							foundsel = true;
							SetSelection(pos, pos + n);
							foundsel = false;
							CenterCursor();
						}
						foundpos = pos;
						foundsize = n;
						found = true;
						return true;
					}
				}
			}
			if(back) {
				if(s-- == l) break;
			}
			else
				if(!*s++) break;
		}
		if(back) {
			if(--line < 0) break;
			ln = GetWLine(line);
			l = ln;
			s = ln.End();
		}
		else {
			if(++line >= GetLineCount()) {
				WaitView(line);
				if(line >= GetLineCount())
					break;
			}
			ln = GetWLine(line);
			l = s = ln;
		}
		if(!SearchProgress(back ? GetLineCount() - line : line))
			break;
	}
	if(back)
		notfoundbk = true;
	else
		notfoundfw = true;
	return false;
}

void CodeEditor::FindReplaceAddHistory()
{
	if(!IsNull(findreplace.find))
		findreplace.find.AddHistory();
	if(!IsNull(findreplace.replace))
		findreplace.replace.AddHistory();
}

void CodeEditor::NoFindError()
{
	findreplace.find.Error(false);
}

void CodeEditor::NotFound()
{
	findreplace.find.Error();
	if(!findreplace.IsIncremental())
		SetFocus();
	findreplace.amend.Disable();
}

bool CodeEditor::Find(bool back, bool blockreplace, bool replace)
{
	NoFindError();
	if(Find(back, blockreplace)) {
		if(!blockreplace) {
			if(!findreplace.IsOpen())
				OpenNormalFindReplace(replace);
			if(!findreplace.IsIncremental())
				SetFocus();
		}
		return true;
	}
	else {
		NotFound();
		return false;
	}
}

WTxt CodeEditor::GetWild(i32 type, i32& i)
{
	for(;;) {
		if(i >= foundwild.GetCount()) break;
		Found& f = foundwild[i++];
		if(f.type == type) return f.text;
	}
	for(i32 j = 0; j < foundwild.GetCount(); j++) {
		Found& f = foundwild[j++];
		if(f.type == type) return f.text;
	}
	return WTxt();
}

WTxt CodeEditor::GetReplaceText()
{
	WTxt rs = ~findreplace.replace;
	bool wildcards = findreplace.wildcards;
	bool samecase = findreplace.ignorecase && findreplace.samecase;

	i32 anyi = 0, onei = 0, spacei = 0, numberi = 0, idi = 0;
	WTxt rt;
	wtukk s = rs;
	while(*s) {
		i32 c = *s++;
		if(c == '\\') {
			c = *s++;
			if(c == '\0') break;
			if(c == 'n')
				rt.Cat('\n');
			else
			if(c == 't')
				rt.Cat('\t');
			else
			if(c >= ' ')
				rt.Cat(c);
		}
		else
		if(c >= ' ') {
			if(wildcards) {
				WTxt w;
				if(c == '*')
					w = GetWild(WILDANY, anyi);
				else
				if(c == '?')
					w = GetWild(WILDONE, onei);
				else
				if(c == '%')
					w = GetWild(WILDSPACE, spacei);
				else
				if(c == '#')
					w = GetWild(WILDNUMBER, numberi);
				else
				if(c == '$')
					w = GetWild(WILDID, idi);
				else
				if(c == '@') {
					c = *s++;
					if(c == '\0') break;
					if(c == '@') {
						rt << AsTxt(replacei).ToWTxt();
						continue;
					}
					if(c == '#') {
						rt << AsTxt(replacei + 1).ToWTxt();
						continue;
					}
					if(c >= '1' && c <= '9') {
						c -= '1';
						w = c < foundwild.GetCount() ? foundwild[c].text : WTxt();
					}
					else {
						rt.Cat('@');
						if(c >= ' ' && c < 255) rt.Cat(c);
						continue;
					}
				}
				else {
					rt.Cat(c);
					continue;
				}
				if(*s == '+') {
					w = ToUpper(w);
					s++;
				}
				else
				if(*s == '-') {
					w = ToLower(w);
					s++;
				}
				else
				if(*s == '!') {
					w = InitCaps(w);
					s++;
				}
				rt.Cat(w);
			}
			else
				rt.Cat(c);
		}
	}
	if(samecase) {
		if(foundtext.GetCount() && rt.GetCount()) {
			if(IsUpper((wchar) foundtext[0]))
				rt.Set(0, ToUpper((wchar)rt[0]));
			if(IsLower((wchar)foundtext[0]))
				rt.Set(0, ToLower((wchar)rt[0]));
		}
		if(foundtext.GetCount() > 1) {
			if(IsUpper((wchar)foundtext[1]))
				for(i32 i = 1; i < rt.GetCount(); i++)
					rt.Set(i, ToUpper((wchar)rt[i]));
			if(IsLower((wchar)foundtext[1]))
				for(i32 i = 1; i < rt.GetCount(); i++)
					rt.Set(i, ToLower((wchar)rt[i]));
		}
	}
	replacei++;
	return rt;
}

void CodeEditor::Replace()
{
	if(!findreplace.replacing)
		return;
	NextUndo();
	FindReplaceAddHistory();
	if(!found) return;
	bool h = persistent_find_replace;
	persistent_find_replace = true; // avoid closing of findreplace by selection change
	if(RemoveSelection()) {
		Paste(GetReplaceText());
		Find(false, false, true);
	}
	persistent_find_replace = h;
}

i32 CodeEditor::BlockReplace()
{
	NextUndo();
	Refresh(); // Setting full-refresh here avoids Pre/Post Remove/Insert costs
	i32 l, h;
	if(!GetSelection32(l, h)) return 0;
	PlaceCaret(l);
	i32 count = 0;
	foundpos = l;
	Index<i32> ln;
	StartSearchProgress(l, h);
	while(foundpos < GetLength() && FindFrom(foundpos, false, true) && foundpos + foundsize <= h) {
		CachePos(foundpos);
		if(~findreplace.mode == 0) {
			Remove((i32)foundpos, foundsize);
			WTxt rt = GetReplaceText();
			Insert((i32)foundpos, rt);
			foundpos += rt.GetCount();
			if(foundsize + rt.GetCount() == 0 && foundpos < GetLength())
				foundpos++;
			h = h - foundsize + rt.GetCount();
			count++;
		}
		else {
			ln.FindAdd(GetLine(foundpos));
			foundpos += foundsize;
		}
	}
	if(SearchCanceled())
		return count;
	EndSearchProgress();
	Progress pi("Удаление строк");
	if(~findreplace.mode != 0) {
		ClearSelection();
		i32 ll = GetLine(l);
		i32 lh = GetLine(h);
		if(GetPos64(lh) == h)
			lh--;
		bool mm = ~findreplace.mode == 1;
		Txt replace;
		pi.SetTotal(lh - ll + 1);
		for(i32 i = ll; i <= lh; i++) {
			pi.Set(i, lh - ll + 1);
			if(pi.Canceled())
				return 0;
			if((ln.Find(i) >= 0) == mm) {
				replace << GetUtf8Line(i) << "\n";
				count++;
			}
		}
		i32 pos = GetPos32(ll);
		Remove(pos, GetPos32(GetLine(h)) - pos);
		SetSelection(pos, pos + Insert(pos, replace));
	}
	else
		SetSelection(l, h);
	return count;
}

void CodeEditor::OpenNormalFindReplace0(bool replace)
{
	findreplace.incremental.Enable(GetLength64() < 2000000);
	findreplace.Setup(replace);
	findreplace.find_all.Show(!replace && WhenFindAll);
	findreplace.itext = GetI();
	findreplace.prev.Show();
	findreplace.next <<= OTVET(DoFind);
	findreplace.close <<= OTVET(EscapeFindReplace);
	if(!findreplace.IsOpen())
		InsertFrame(FindFrame(sb), findreplace);
	WhenOpenFindReplace();
	findreplace.find.SetFocus();
}

void CodeEditor::OpenNormalFindReplace(bool replace)
{
	OpenNormalFindReplace0(replace);
//	if(!findreplace.incremental_from_cursor)
//		IncrementalFind();
}

void CodeEditor::FindReplace(bool pick_selection, bool pick_text, bool replace)
{
	if(IsReadOnly())
		replace = false;
	
	if(findreplace.IsOpen())
		CloseFindReplace();

	ff_start_pos = GetCursor32();

	replacei = 0;
	WTxt find_text;
	i32 find_pos = -1;
	
	findreplace.Setup(replace);
	
	if(pick_text || pick_selection)
	{
		if(IsSelection()) {
			i32 l, h;
			GetSelection32(l, h);
			if(h - l < 100) {
				find_text = GetSelectionW();
				if(find_text.Find('\n') >= 0)
					find_text.Clear();
			}
		}
		else
		if(pick_text) {
			i32 l, h;
			l = h = GetCursor32();
			while(l > 0 && CharFilterCIdent(GetChar(l - 1)))
				l--;
			while(h < GetLength64() && CharFilterCIdent(GetChar(h)))
				h++;
			find_text = Get(l, h - l).ToWTxt();
			find_pos = h;
		}
		if(find_text.GetCount())
			findreplace.find <<= find_text;
	}
	if(IsSelection() && replace) {
		findreplace.itext = GetI();
	//	SetLayout_BlockReplaceLayout(findreplace);
		findreplace.SetRect(WithBlockReplaceLayout<EmptyClass>::GetLayoutSize());
		findreplace.Title(t_("Замена в выделении"));
		//findreplace.find_all.Hide();
		findreplace.amend.Hide();
	//	findreplace.amend_all.Hide();
		findreplace.amend_rest.Hide();
		findreplace.prev.Hide();
		findreplace.next.Ok() <<= findreplace.Breaker(IDOK);
		findreplace.close.SetLabel("Закрыть");
		findreplace.close.Cancel() <<= findreplace.Breaker(IDCANCEL);
		//findreplace.close.SetImage(Null);
		findreplace.close.Tip("Закрыть");
		//findreplace.next.SetImage(Null);
		findreplace.next.SetLabel("Следщ");
		findreplace.next.Tip("К следующему");
		findreplace.mode.Show();
		findreplace.mode <<= 0;
		findreplace.Sync();
		if(findreplace.Execute() == IDOK)
			BlockReplace();
		findreplace.mode.Hide();
	}
	else {
		if(find_pos >= 0)
			SetCursor(find_pos);
		OpenNormalFindReplace(replace);
		findreplace.find.SetFocus();
	}
}

void CodeEditor::ReplaceAll(bool rest)
{
	i32 l, h;
	GetSelection32(l, h);
	i32 c = min(l, h);
	findreplace.mode <<= 0;
	SetSelection(rest * c, GetLength64());
	BlockReplace();
	SetCursor(c);
}

void CodeEditor::InsertWildcard(tukk s)
{
	iwc = s;
}

void FindWildcardMenu(Callback1<tukk > cb, Point p, bool tablf, Ctrl *owner, bool regexp)
{
	MenuBar menu;
	if(regexp) {
		menu.Add(t_("Один или более пробелов"), callback1(cb, " +"));
		menu.Add(t_("Один или более любых символов"), callback1(cb, ".+"));
		menu.Add(t_("Слово"), callback1(cb, "\\w+"));
		menu.Add(t_("Число"), callback1(cb, "\\d+"));
		menu.Add(t_("Любой символ"), callback1(cb, "."));
		if(tablf) {
			menu.Separator();
			menu.Add(t_("Таб"), callback1(cb, "\\t"));
		}
	}
	else {
		menu.Add(t_("Один или более пробелов"), callback1(cb, "%"));
		menu.Add(t_("Один или более любых символов"), callback1(cb, "*"));
		menu.Add(t_("Идентификатор C++"), callback1(cb, "$"));
		menu.Add(t_("Число"), callback1(cb, "#"));
		menu.Add(t_("Любой символ"), callback1(cb, "?"));
		if(tablf) {
			menu.Separator();
			menu.Add(t_("Таб"), callback1(cb, "\\t"));
			menu.Add(t_("Лайнфид"), callback1(cb, "\\n"));
		}
	}
	menu.Execute(owner, p);
}

void CodeEditor::FindWildcard()
{
	i32 l, h;
	findreplace.find.GetSelection(l, h);
	iwc.Clear();
	FindWildcardMenu(OTVET(InsertWildcard), findreplace.find.GetPushScreenRect().TopRight(), true,
	                 &findreplace, findreplace.regexp);
	if(iwc.GetCount()) {
		if(!findreplace.regexp)
			findreplace.wildcards = true;
		findreplace.find.SetFocus();
		findreplace.find.SetSelection(l, h);
		findreplace.find.RemoveSelection();
		findreplace.find.Insert(iwc);
	}
}

void CodeEditor::ReplaceWildcard()
{
	MenuBar menu;
	Txt ptxt;
	if(findreplace.regexp)
		ptxt = t_("Сверен подобразец %d");
	else {
		menu.Add(t_("Совпадающие пробелы"), OTVET1(InsertWildcard, "%"));
		menu.Add(t_("Совадаюшие 1/неск. любых символов"), OTVET1(InsertWildcard, "*"));
		menu.Add(t_("Совпадающие идентификаторы C++"), OTVET1(InsertWildcard, "$"));
		menu.Add(t_("Совпадающее число"), OTVET1(InsertWildcard, "#"));
		menu.Add(t_("Совпадающий любой символ"), OTVET1(InsertWildcard, "?"));
		ptxt = t_("Совпадающий шаблон %d");
	}
	menu.Add(t_("Индекс замены на базе 0"), OTVET1(InsertWildcard, "@@"));
	menu.Add(t_("Индекс замены на базе 1"), OTVET1(InsertWildcard, "@#"));
	menu.Separator();
	for(i32 i = 1; i <= 9; i++)
		menu.Add(Format(ptxt, i), OTVET1(InsertWildcard, "@"+AsTxt(i)));
	menu.Separator();
	menu.Add(t_("Взаг"), OTVET1(InsertWildcard, "+"));
	menu.Add(t_("Впроп"), OTVET1(InsertWildcard, "-"));
	menu.Add(t_("Озаглавь"), OTVET1(InsertWildcard, "!"));
	menu.Separator();
	menu.Add(t_("Таб"), OTVET1(InsertWildcard, "\\t"));
	menu.Add(t_("Лайнфид"), OTVET1(InsertWildcard, "\\n"));
	i32 l, h;
	findreplace.replace.GetSelection(l, h);
	iwc.Clear();
	menu.Execute(&findreplace, findreplace.replace.GetPushScreenRect().TopRight());
	if(iwc.GetCount()) {
		if(!findreplace.regexp)
			findreplace.wildcards = true;
		findreplace.replace.SetFocus();
		findreplace.replace.SetSelection(l, h);
		findreplace.replace.RemoveSelection();
		findreplace.replace.Insert(iwc);
	}
}

void CodeEditor::CloseFindReplace()
{
	if(findreplace.IsOpen()) {
		FindReplaceAddHistory();
		RemoveFrame(findreplace);
	}
}

void CodeEditor::EscapeFindReplace()
{
	CloseFindReplace();
	if(ff_start_pos >= 0 && ff_start_pos < GetLength64() && findreplace.IsIncremental() && do_ff_restore_pos) {
		SetCursor(ff_start_pos);
		ff_start_pos = -1;
	}
}

void CodeEditor::IncrementalFind()
{
	NoFindError();
	findreplace.Sync();
	if(!findreplace.IsIncremental() || findreplace.GetTopCtrl() == &findreplace) // || we are block replace
		return;
	bool b = FindFrom(ff_start_pos >= 0 && ff_start_pos < GetLength64()
	                  && findreplace.incremental_from_cursor ? ff_start_pos : 0, false, false);
	findreplace.amend.Enable(b);
	if(!b)
		NotFound();
}

void CodeEditor::DoFind()
{
	Find(false, false);
}

void CodeEditor::DoFindBack()
{
	Find(true, false);
}

void CodeEditor::SerializeFind(Stream& s)
{
	i32 version = 2;
	s / version;
	s % findreplace.find;
	findreplace.find.SerializeList(s);
	s % findreplace.wholeword % findreplace.ignorecase % findreplace.wildcards;
	if(version >= 0)
		s % findreplace.samecase;
	s % findreplace.replace;
	if(version >= 1)
		s % findreplace.incremental;
	if(version >= 2)
		s % findreplace.regexp;
	findreplace.replace.SerializeList(s);
}

Txt ReadList(WithDropChoice<EditTxt>& e)
{
	TxtStream ss;
	e.SerializeList(ss);
	return ss;
}

void WriteList(WithDropChoice<EditTxt>& e, const Txt& data)
{
	TxtStream ss(data);
	e.SerializeList(ss);
}

CodeEditor::FindReplaceData CodeEditor::GetFindReplaceData()
{
	FindReplaceData r;
	r.find = ~findreplace.find;
	r.replace = ~findreplace.replace;
	r.wholeword = ~findreplace.wholeword;
	r.ignorecase = ~findreplace.ignorecase;
	r.wildcards = ~findreplace.wildcards;
	r.samecase = ~findreplace.samecase;
	r.regexp = ~findreplace.regexp;
	r.find_list = ReadList(findreplace.find);
	r.replace_list = ReadList(findreplace.replace);
	return r;
}

void CodeEditor::SetFindReplaceData(const FindReplaceData& r)
{
	findreplace.find <<= r.find;
	findreplace.replace <<= r.replace;
	findreplace.wholeword <<= r.wholeword;
	findreplace.ignorecase <<= r.ignorecase;
	findreplace.wildcards <<= r.wildcards;
	findreplace.samecase <<= r.samecase;
	findreplace.regexp <<= r.regexp;
	findreplace.mode <<= 0;
	WriteList(findreplace.find, r.find_list);
	WriteList(findreplace.replace, r.replace_list);
}

void CodeEditor::FindPrevNext(bool prev)
{
	StartSearchProgress(-1, -1);
	if(!findreplace.IsOpen()) {
		WTxt find_text;
		if(IsSelection()) {
			i32 l, h;
			GetSelection32(l, h);
			if(h - l < 100) {
				find_text = GetSelectionW();
				if(find_text.Find('\n') >= 0)
					find_text.Clear();
			}
		}
		if(find_text.GetCount())
			findreplace.find <<= find_text;
		OpenNormalFindReplace0(false);
	}
	if(Find(prev, false))
		NoFindError();
	else
		NotFound();
	EndSearchProgress();
}

void CodeEditor::FindNext()
{
	FindPrevNext(false);
}

void CodeEditor::FindPrev()
{
	FindPrevNext(true);
}

void CodeEditor::StartSearchProgress(z64, z64)
{
	search_canceled = false;
	search_progress.Create();
	search_progress->SetText("Сканируется файл");
	search_time0 = msecs();
}

bool CodeEditor::SearchProgress(i32 line)
{
	if(search_progress && !search_canceled && msecs(search_time0) > 20) {
		search_time0 = msecs();
		search_progress->Create();
		search_canceled = IsView() ? search_progress->SetCanceled(i32(GetPos64(line) >> 8), i32(GetViewSize() >> 8))
		                           : search_progress->SetCanceled(line, GetLineCount());
	}
	return !search_canceled;
}

bool CodeEditor::SearchCanceled()
{
	return search_canceled;
}

void CodeEditor::EndSearchProgress()
{
	search_progress.Clear();
	search_canceled = false;
}

}
