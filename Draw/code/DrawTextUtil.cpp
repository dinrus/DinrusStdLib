#include <drx/Draw/Draw.h>

namespace drx {

void DrawTextEllipsis(Draw& w, i32 x, i32 y, i32 cx, wtukk text, tukk ellipsis,
				      Font font, Color ink, i32 n)
{
	if(n < 0) n = strlen__(text);
	FontInfo f = font.Info();
	tukk s;
	i32 dtl = 0;
	i32 el = 0;
	for(s = ellipsis; *s; s++) {
		dtl += f[*s];
		el++;
	}
	i32 l = 0;
	i32 i;
	for(i = 0; i < n; i++) {
		l += f[(u8) text[i]];
		if(l > cx) {
			while(l + dtl > cx && i > 0) {
				l -= f[text[i]];
				i--;
			}
			i++;
			break;
		}
	}
	w.DrawText(x, y, text, font, ink, i);
	if(i < n)
		w.DrawText(x + l, y, ellipsis, font, ink, el);
}

void DrawTextEllipsis(Draw& w, i32 x, i32 y, i32 cx, tukk text, tukk ellipsis,
				      Font font, Color ink, i32 n)
{
	return DrawTextEllipsis(w, x, y, cx, WTxt(text), ellipsis, font, ink, n);
}

Size GetTLTextSize(wtukk text, Font font)
{
	Size sz(0, 0);
	i32 cy = font.GetCy();
	wtukk s = text;
	wtukk t = s;
	for(;;) {
		if(*s == '\n' || *s == '\0') {
			i32 a = 0;
			wtukk q = t;
			while(q < s) {
				while(q < s && *q < ' ') {
					if(*q == '\t')
						a = (a + 2 * cy) / (2 * cy) * (2 * cy);
					q++;
				}
				t = q;
				while(q < s && *q >= ' ')
					q++;
				a += GetTextSize(t, font, (i32) (q - t)).cx;
			}
			t = s + 1;
			sz.cy += cy;
			sz.cx = max(sz.cx, a);
		}
		if(*s++ == '\0') break;
	}
	return sz;
}

i32 GetTLTextHeight(wtukk s, Font font)
{
	return GetTLTextSize(s, font).cy;
}

void DrawTLText(Draw& draw, i32 x, i32 y, i32 cx, wtukk text,
                Font font, Color ink, i32 accesskey) {
	i32 cy = font.GetCy();
	wtukk s = text;
	wtukk t = s;
	i32 apos = HIWORD(accesskey);
	i32 akey = LOWORD(accesskey);
	for(;;) {
		if(*s == '\n' || *s == '\0') {
			i32 a = x;
			wtukk q = t;
			wtukk start = NULL;
			while(q < s) {
				while(q < s && *q < ' ') {
					if(*q == '\t')
						a = (a - x + 2 * cy) / (2 * cy) * (2 * cy) + x;
					q++;
				}
				t = q;
				bool ak = false;
				start = q;
				while(q < s && *q >= ' ') {
					if(akey && (i32)ToUpper(ToAscii(*q)) == akey && (apos == 0 || q - start + 1 == apos)) {
						ak = true;
						akey = 0;
						break;
					}
					q++;
				}
				start = NULL;
				draw.DrawText(a, y, t, font, ink, (i32)(q - t));
				a += GetTextSize(t, font, (i32)(q - t)).cx;
				if(ak) {
					draw.DrawText(a, y, q, font().Underline(), ink, 1);
					a += GetTextSize(q, font().Underline(), 1).cx;
					q++;
				}
			}
			t = s + 1;
			y += cy;
		}
		if(*s++ == '\0') break;
	}
}

}
