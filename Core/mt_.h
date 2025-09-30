// This for layouts, to avoid adding drx:: to each t_

#ifdef t_
#undef t_
#endif

#ifdef tt_
#undef tt_
#endif

#define t_(x)          drx::t_GetLngTxt(x)
#define tt_(x)         x
