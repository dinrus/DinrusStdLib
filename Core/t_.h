#ifdef t_
#undef t_
#endif

#ifdef tt_
#undef tt_
#endif

#ifndef t_h
#define t_h

inline tukk t_(tukk s)  { return t_GetLngTxt(s); }
inline tukk tt_(tukk s) { return s; }

#endif