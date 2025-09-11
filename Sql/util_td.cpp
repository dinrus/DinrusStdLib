#include "Sql.h"

namespace drx {

void td_scalar(SqlSet& set, const Txt& prefix, tukk x) {
	set.Cat(SqlId(prefix + x));
}

void td_array(SqlSet& set, const Txt& prefix, tukk x, i32 cnt) {
	Txt name = prefix + x;
	for(i32 i = 0; i < cnt; i++)
		set.Cat(SqlId(name + Format("%d", i)));
}

void td_var(SqlSet& set, const Txt& prefix, tukk x, SqlSet (*f)(const Txt&)) {
	set.Cat((*f)(prefix + x + '$'));
}

void td_shrink(Txt *array, i32 cnt) {
	Txt *lim = array + cnt;
	while(array < lim) {
		array->Shrink();
		array++;
	}
}

struct NfEqual : FieldOperator {
	ValArray va;
	
	virtual void Field(tukk name, Ref f) {
		va.Add(f);
	}
};

bool EqualFields(Fields a, Fields b)
{
	NfEqual fa, fb;
	a(fa);
	b(fb);
	return fa.table == fb.table && fa.va == fb.va;
}

struct NfAsTxt : FieldOperator {
	Txt text;
	
	virtual void Field(tukk name, Ref f) {
		text << "\n\t" << name << '=' << (Val)f;
	}
};


Txt AsTxt(Fields a)
{
	NfAsTxt x;
	a(x);
	return x.text;
}

}
