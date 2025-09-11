#include "RichEdit.h"

namespace drx {

void RichEdit::SpellerAdd(const WTxt& w, i32 lang)
{
	drx::SpellerAdd(w, lang);
}

i32 RichEdit::fixedlang;

Bits RichEdit::SpellParagraph(const RichPara& para)
{
	i32 len = para.GetLength();
	Buffer<wchar> text(len);
	Buffer<i32> lang(len);
	wtuk s = text;
	i32 *g = lang;
	for(i32 i = 0; i < para.GetCount(); i++) {
		const RichPara::Part& p = para[i];
		if(p.IsText()) {
			i32 l = p.text.GetLength();
			memcpy(s, p.text, l * sizeof(wchar));
			Fill(g, g + l, fixedlang ? fixedlang : p.format.language);
			s += l;
			g += l;
		}
		else {
			*s++ = 127;
			*g++ = 0;
		}
	}
	Bits e;
	s = text;
	wtuk end = text + len;
	while(s < end) {
		if(IsLetter(*s)) {
			wtukk q = s;
			while(s < end && IsLetter(*s) || s + 1 < end && *s == '\'' && IsLetter(s[1]))
				s++;
			if(!SpellWord(q, i32(s - q), lang[q - text]))
				e.SetN(i32(q - text), i32(s - q));
		}
		else
			s++;
	}
	return e;
}

}