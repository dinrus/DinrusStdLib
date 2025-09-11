#include "Sql.h"

namespace drx {

void S_info_maker::Field(tukk name, Ref f, bool *b)
{
	if(b) f = Ref(*b);
	S_info::Column& c = info.column.Add(name);
	c.offset = (u8 *)f.GetVoidPtr() - (u8 *)s;
	c.manager = f.GetManager();
	c.width = 0;
}

void S_info_maker::Width(i32 width)
{
	info.column.Top().width = width;
}

Ref S_info::GetRef(ukk s, i32 i) const
{
	return Ref((u8 *)s + column[i].offset, column[i].manager);
}

i32 S_info::GetWidth(const SqlId& id) const
{
	i32 q = column.Find(~id);
	return q >= 0 ? GetWidth(q) : 0;
}

Ref S_info::GetRef(ukk s, const SqlId& id) const
{
	i32 q = column.Find(~id);
	return q >= 0 ? GetRef(s, q) : Ref();
}

Val S_info::Get(ukk s, const SqlId& id) const
{
	return ~GetRef(s, id);
}

Val S_info::Get(ukk s, i32 i) const
{
	return ~GetRef(s, i);
}

ValMap S_info::Get(ukk s) const
{
	ValMap m;
	for(i32 i = 0; i < column.GetCount(); i++)
		m.Add(column.GetKey(i), GetRef(s, i));
	return m;
}

void S_info::Set(ukk s, i32 i, const Val& v) const
{
	Ref f = GetRef(s, i);
	if(f.Is<bool>() && IsTxt(v)) {
		Txt h = v;
		f = !(h == "0" || IsNull(h));
	}
	else
		f = v;
}

void S_info::Set(ukk s, const SqlId& id, const Val& v) const
{
	i32 q = column.Find(~id);
	if(q >= 0)
		Set(s, q, v);
}

void S_info::Set(ukk s, const ValMap& m) const
{
	for(i32 i = 0; i < m.GetCount(); i++) {
		Val v = m.GetKey(i);
		if(IsTxt(v))
			Set(s, (Txt)v, m.GetVal(i));
	}
}

SqlSet S_info::GetSet(const Txt& prefix) const
{
	SqlSet set;
	for(i32 i = 0; i < column.GetCount(); i++)
		set << SqlId(prefix + column.GetKey(i));
	return set;
}

SqlSet S_info::GetOf(const SqlId& table) const
{
	SqlSet set;
	for(i32 i = 0; i < ids.GetCount(); i++)
		set << SqlId(ids[i].Of(table));
	return set;
}

void S_info::Init()
{
	column.Shrink();
	ids.SetCount(column.GetCount());
	for(i32 i = 0; i < column.GetCount(); i++) {
		SqlId id(column.GetKey(i));
		ids[i] = id;
		set << id;
	}
}

}
