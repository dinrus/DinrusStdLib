#include "CppBase.h"
#include "Internal.h"

namespace drx {

#define LLOG(x)
#define LTIMING(x)  // DTIMING(x)

ScopeInfo::ScopeInfo(const CppBase& base, i32 scopei)
	: scopei(scopei), base(base)
{
	LTIMING("ScopeInfo(const CppBase& base, i32 scopei)");
	Init();
}

ScopeInfo::ScopeInfo(i32 scopei, const CppBase& base)
	: scopei(scopei), base(base)
{
	LTIMING("ScopeInfo(i32 scopei, const CppBase& base)");
	Init();
}

ScopeInfo::ScopeInfo(const CppBase& base, const Txt& scope)
	: scopei(base.Find(scope)), base(base)
{
	LTIMING("ScopeInfo(const CppBase& base, const Txt& scope)");
	Init();
}

ScopeInfo::ScopeInfo(const ScopeInfo& f)
	: base(f.base)
{
	LTIMING("ScopeInfo copy contructor");
	scopes <<= f.scopes;
	bvalid = nvalid = false;
	scopei = f.scopei;
}

void ScopeInfo::Init()
{
	bvalid = nvalid = false;
}

void ScopeInfo::Bases(i32 i, Vec<i32>& g)
{ // recursively retrieve all base classes
	if(base.IsType(i)) {
		const Array<CppItem>& n = base[i];
		for(i32 i = 0; i < n.GetCount(); i++) {
			const CppItem& im = n[i];
			if(im.IsType()) {
				tukk q = im.qptype;
				tukk b = q;
				for(;;) {
					if(*q == ';' || *q == '\0') {
						if(b < q) {
							Txt h = Txt(b, q);
							i32 q = h.Find('<');
							if(q >= 0)
								h.Trim(q);
							h = TrimBoth(h);
							i32 nq = base.Find(h);
							if(nq >= 0)
								g.Add(nq);
						}
						if(*q == '\0')
							break;
						q++;
						b = q;
					}
					else
						q++;
				}
			}
		}
	}
}

const Vec<Txt>& ScopeInfo::GetBases()
{
	LTIMING("GetBases");
	if(!bvalid) {
		bvalid = true;
		baselist.Clear();
		if(scopei < 0)
			return baselist;
		Vec<i32> b;
		Index<i32> bi;
		Bases(scopei, b);
		while(b.GetCount()) {
			Vec<i32> bb;
			for(i32 i = 0; i < b.GetCount(); i++) {
				i32 q = b[i];
				if(bi.Find(q) < 0) {
					bi.Add(q);
					Bases(b[i], bb);
				}
			}
			b = pick(bb);
		}
		for(i32 i = 0; i < bi.GetCount(); i++)
			baselist.Add(base.GetKey(bi[i]) + "::");
	}
	return baselist;
}

const Vec<Txt>& ScopeInfo::GetScopes(const Txt& usings_)
{
	LTIMING("GetScopes");
	if(!nvalid || usings != usings_) {
		usings = usings_;
		nvalid = true;
		scopes.Clear();
		if(scopei >= 0) {
			Txt nn = base.GetKey(scopei);
			while(nn.GetCount()) {
				if(nn[0] == ':' && nn.GetCount() == 2) {
					scopes.Add(nn);
					return scopes;
				}
				scopes.Add(nn + "::");
				i32 q = nn.ReverseFind(':');
				nn.Trim(max(0, q - 1));
			}
		}
		scopes.Add("");
		Vec<Txt> h = Split(usings, ';');
		for(i32 i = 0; i < h.GetCount(); i++)
			scopes.Add(h[i] + "::");
	}
	return scopes;
}

}
