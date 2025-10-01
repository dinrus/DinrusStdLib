#include "CppBase.h"
#include "Internal.h"

namespace drx {

Txt GetStdDefs()
{
	static const char ns[] = "_STD_BEGIN:std;_C_STD_BEGIN:std;_STDEXT_BEGIN:stdext;NAMESPACE_UPP:drx";
	static const char endns[] = "_STD_END;_STDEXT_END;_C_STD_END;END_DRX_NAMESPACE";
	static const char ignore[] = "__declspec(...);__cdecl;__attribute__(...);__stdcall;__forceinline;"
                                 "__out;__in;__inout;__deref_in;__deref_inout;__deref_out;"
                                 "__pragma(...);__pascal;_far;_pascal;_cdecl;"
                                 "__AuToQuOtE;__xin;__xout;__export;"
                                 "__clrcall;__alignof(...);"
                                 
                                 "__asm__(...);__asm(...);__restrict;__inline;__typeof;"
                                 "_GLIBCXX_VISIBILITY(...);_GLIBCXX_BEGIN_NAMESPACE_VERSION;"
                                 "_GLIBCXX_END_NAMESPACE_VERSION;_GLIBCXX_CONSTEXPR;"
                                 "_GLIBCXX_HAS_NESTED_TYPE(...);"
                                 "__inline__;"
                                 "_GLIBCXX_CONST;"
                                 "_GLIBCXX_ABI_TAG_CXX11;"
                                 "__attribute(...);"
                                 "__glibcxx_class_requires(...);"
                                 "__glibcxx_class_requires2(...);"
                                 "__glibcxx_class_requires3(...);"
                                 "__glibcxx_class_requires4(...);"
                                 "__glibcxx_class_requires5(...);"
	;
	
	Txt defs;
	Vec<Txt> h = Split(ns, ';');
	for(i32 i = 0; i < h.GetCount(); i++) {
		Txt id, n;
		SplitTo(h[i], ':', id, n);
		ValMap m;
		m.Add(id, n);
		defs << "#define " << id << " namespace " << n << " {\n";
	}
	h = Split(endns, ';');
	for(i32 i = 0; i < h.GetCount(); i++)
		defs << "#define " << h[i] << " };\r\n";
	h = Split(ignore, ';');
	for(i32 i = 0; i < h.GetCount(); i++)
		defs << "#define " << h[i] << "\n";
	defs << "#define NULL NULL\n";
	defs << "#define DrawText DrawText\n"; // DrawTextA/DrawTextW fiasco...
	defs << "#define __typeof__ decltype\n";
	return defs;
}

};