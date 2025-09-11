#include "CppBase.h"
#include "Internal.h"

namespace drx {

#define LLOG(x)
#define LTIMING(x)  DTIMING(x)

void CppItem::Serialize(Stream& s)
{
	s % kind % access
	  % item % name % natural % at % tparam % param % pname
	  % tname % ctname % type % ptype % virt % filetype % file % line % impl
	  % using_namespaces;
}

struct CmpItem {
	bool operator()(const CppItem& a, const Txt& b) const
	{
		return a.qitem < b;
	}
};

i32 FindItem(const Array<CppItem>& x, const Txt& qitem)
{
	for(i32 i = 0; i < x.GetCount(); i++)
		if(x[i].qitem == qitem)
			return i;
	return -1;
}

i32 FindNext(const Array<CppItem>& x, i32 i)
{
	if(i >= x.GetCount())
		return i;
	Txt q = x[i].qitem;
	while(i < x.GetCount() && x[i].qitem == q)
		i++;
	return i;
}

i32 GetCount(const Array<CppItem>& x, i32 i)
{
	return FindNext(x, i) - i;
}

i32 FindName(const Array<CppItem>& x, const Txt& name, i32 i)
{
	while(i < x.GetCount()) {
		if(x[i].name == name)
			return i;
		i++;
	}
	return -1;
}

bool CppBase::IsType(i32 i) const
{
	return GetKey(i).GetCount();
}

void CppBase::Dump(Stream& s)
{
	for(i32 i = 0; i < GetCount(); i++) {
		s << Nvl(GetKey(i), "<<GLOBALS>>") << "\n";
		const Array<CppItem>& m = (*this)[i];
		for(i32 j = 0; j < m.GetCount(); j++)
			s << '\t' << m[j] << "\n";
	}
}

void CppBase::Sweep(const Index<i32>& file, bool keep)
{
	Vec<i32> remove;
	for(i32 ni = 0; ni < GetCount(); ni++) {
		Array<CppItem>& n = (*this)[ni];
		Vec<i32> nr;
		for(i32 i = 0; i < n.GetCount(); i++)
			if((file.Find(n[i].file) < 0) == keep)
				nr.Add(i);
		if(nr.GetCount() == n.GetCount())
			remove.Add(ni); // remove whole array (later)
		else
			n.Remove(nr); // only remove some items
	}
	Remove(remove);
}

void CppBase::Append(CppBase&& base)
{
	for(i32 i = 0; i < base.GetCount(); i++)
		GetAdd(base.GetKey(i)).Append(pick(base[i]));
}

void CppBase::RemoveFile(i32 filei)
{
	Index<i32> h;
	h.Add(filei);
	RemoveFiles(h);
}


}
