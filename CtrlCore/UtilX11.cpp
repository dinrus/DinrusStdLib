#include "CtrlCore.h"

#ifdef GUI_X11

namespace drx {

Vec<Rect> RectRgn(const Rect& r)
{
	Vec<Rect> q;
	q.Add(r);
	return q;
}

Vec<Rect> Intersect(const Vec<Rect>& r1, const Vec<Rect>& r2)
{
	Vec<Rect> q;
	for(i32 i = 0; i < r1.GetCount(); i++)
		for(i32 j = 0; j < r2.GetCount(); j++) {
			Rect r = r1[i] & r2[j];
			if(!r.IsEmpty())
				q.Add(r);
		}
	return q;
}

Vec<Rect> Subtract(const Vec<Rect>& r1, const Vec<Rect>& r2)
{
	Vec<Rect> q;
	bool dummy;
	q <<= r1;
	for(i32 i = 0; i < r2.GetCount(); i++)
		q = Subtract(q, r2[i], dummy);
	return q;
}

Vec<Rect> Xor(const Vec<Rect>& r1, const Vec<Rect>& r2)
{
	Vec<Rect> is = Intersect(r1, r2);
	Vec<Rect> q = Subtract(r1, is);
	q.Append(Subtract(r2, is));
	return q;
}

}

#endif
