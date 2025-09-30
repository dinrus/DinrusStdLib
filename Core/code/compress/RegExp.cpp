//#include <drx/Core/Pcre.h>

namespace drx {


RegExp::RegExp(i32 options)
{
	Clear();
	SetOptions(options);
}

RegExp::RegExp(tukk  p, i32 options)
{
	Clear();
	SetOptions(options);
	SetPattern(p);
}

RegExp::RegExp(const Txt &p, i32 options)
{
	Clear();
	SetOptions(options);
	SetPattern(p);
}

RegExp::~RegExp()
{
	Clear(true);
}

void RegExp::Clear(bool freemem)
{
	if(freemem && study)
		pcre_free(study);
	if(freemem && cpattern)
		pcre_free(cpattern);

	first = false;
	cpattern = NULL;
	study = NULL;
	rc = 0;
	compile_options = 0;
	execute_options = 0;
	error_code = 0;
}

void RegExp::SetOptions(i32 options)
{
	compile_options = options & COMPILE_OPTIONS;
	execute_options = options & EXECUTE_OPTIONS;
}

void RegExp::SetPattern(tukk  p)
{
	pattern = p;
}

void RegExp::SetPattern(const Txt &p)
{
	pattern = p;
}

bool RegExp::Compile(bool recompile)
{
	if(cpattern)
	{
		if(recompile)
			pcre_free(cpattern);
		else
			return true;
	}
	cpattern = pcre_compile2(pattern, compile_options, &error_code, &error_string, &error_offset, NULL);
	return error_code == 0;
}

bool RegExp::Study(bool restudy)
{
	if(!cpattern)
		Compile();
	if(study){
		if(restudy)
			pcre_free(study);
		else
			return true;
	}
	study = pcre_study(cpattern, 0, &error_string);
	if(error_string != NULL)
		error_code = -1; // unfortunatelly, pcre_study doesn't return error codes...
	return error_code == 0;
}

i32 RegExp::Execute(const Txt &t, i32 offset)
{
	rc = pcre_exec(cpattern, study, t, t.GetLength(), offset, execute_options, pos, __countof(pos));
	if(rc <= 0)
		first = false;
	return rc;
}

bool RegExp::Match(const Txt &t, bool copy)
{
	if(copy)
		text = t;
	if(!Compile())
		return false;
	return Execute(t) > 0;
}

bool RegExp::FastMatch(const Txt &t)
{
	return Match(t, false);
}

bool RegExp::GlobalMatch(const Txt &t)
{
	if(!first)
	{
		first = true;
		return Match(t);
	}
	i32 offset = pos[1];
	if(pos[0] == pos[1])
	{
		if(pos[0] == t.GetLength())
		{
			first = false;
			return false;
		}
		execute_options |= PCRE_NOTEMPTY | PCRE_ANCHORED;
	}
	return Execute(t, offset) > 0;
}

Txt RegExp::operator[](i32k i)
{
	return GetTxt(i);
}

i32 RegExp::GetCount()
{
	return rc - 1;
}

Vec<Txt> RegExp::GetTxts()
{
	Vec<Txt> subs;
	for(i32 i = 0; i < GetCount(); i++)
		subs.Add(GetTxt(i));
	return subs;
}

Txt RegExp::GetTxt(i32 i)
{
	i = 2 * (i + 1);
	return text.Mid(pos[i], pos[i + 1] - pos[i]);
}

void RegExp::GetMatchPos(i32 i, i32& iPosStart, i32& iPosAfterEnd)
{
	i = 2 * (i + 1);
	iPosStart = pos[i];
	iPosAfterEnd = pos[i + 1];
}

i32 RegExp::GetOffset() const
{
	return rc > 0 ? pos[0] : -1;
}

i32 RegExp::GetLength() const
{
	return rc > 0 ? pos[1] - pos[0] : -1;
}

i32 RegExp::GetSubOffset(i32 i) const
{
	ASSERT(i < rc - 1);
	return pos[2 * i + 2];
}

i32 RegExp::GetSubLength(i32 i) const
{
	return pos[2 * i + 3] - pos[2 * i + 2];
}

bool RegExp::ReplacePos(Txt& t, i32 p, i32 q, const Txt& r)
{
	if(p>=0 && q<=t.GetLength() && q>p){

		t.Remove(p, q-p);
		if(!r.IsEmpty()) t.Insert(p, r);

		return true;
	}

	return false;
}

Vec<Txt> RegExp::Make_rv(const Txt& r)
{
	RegExp reg("\\(((\r|\n|.)*?)\\)");

	Vec<Txt> rv;

	while(reg.GlobalMatch(r)){
		rv.Add(reg.GetTxt(0));
	}

	return rv;
}

Vec<Txt> RegExp::ResolveBackRef(const Vec<Txt>& rv)
{
	Vec<Txt> new_rv;

	i32 rv_count = rv.GetCount();
	i32 match_count = GetCount();

	RegExp reg("\\\\(\\d+)");

	Txt log;

	if(match_count>0 && rv_count>0) {

		// deep copy rv vector.
		new_rv<<=rv;

		// loop throug number of replace string
		// run reg regexp on each to find if they
		// have any "match referance" ie /1 /2 /3 etc
		for(i32 i=0; i<rv_count; i++){

			while(reg.GlobalMatch(new_rv[i])){

				// for each matched "match ref"
				for(i32 j=0, jMx=reg.GetCount(); j< jMx; j++){

					Txt p;

					// convert the ref to number
					i32 m=StrIntVal(reg.GetTxt(j));

					// make "find" string litaral
					p<<"\\"<<m;

					// reduce 1 from back ref to be used as index.
					m--;

					// doing actual replacement.
					if(m<match_count) new_rv[i].Replace(p, GetTxt(m));
				}
			}
		}
	}

	return new_rv;
}



i32 RegExp::Replace0(Txt& t, const Vec<Txt>& rv, i32k& rv_count, i32& offset)
{

	i32 x=0, y=0;
	i32 count=GetCount();


	for(i32 i=0; i<count; i++){

		GetMatchPos(i, x, y);

		if(i<rv_count) {

			ReplacePos(t, x+offset, y+offset, rv[i]);

			offset = offset + rv[i].GetLength() - (y-x);

		}
		else{

			ReplacePos(t, x+offset, y+offset, "");

			offset = offset - (y-x);
		}
	}

	return count;
}

i32 RegExp::Replace(Txt& t, const Vec<Txt>& rv, bool backref)
{
	Txt t_copy(t);
	i32 count=0;
	i32 pos_offset=0;
	i32 rv_count=rv.GetCount();
	Vec<Txt> new_rv;

	new_rv.Shrink();
	first=false;

	Match(t_copy);

	if(backref){

		 new_rv.Clear();
		 new_rv = ResolveBackRef(rv);
		 count  = Replace0(t, new_rv, rv_count, pos_offset);

	}
	else count = Replace0(t, rv, rv_count, pos_offset);

	return count;
}

i32 RegExp::Replace(Txt& t, const Txt& r, bool backref)
{
	Vec<Txt> rv(Make_rv(r));
	return Replace(t, rv, backref);
}


i32 RegExp::Replace(Txt& t, Event<Vec<Txt>&> cbr)
{
	Txt t_copy(t);
	i32 count=0;
	i32 pos_offset=0;
	i32 rv_count=0;
	Vec<Txt> rv;

	first=false;

	Match(t_copy);

	rv=GetTxts();

	cbr(rv);

	rv_count=rv.GetCount();

	Replace0(t, rv, rv_count, pos_offset);

	return count;

}

i32 RegExp::ReplaceGlobal(Txt& t, const Vec<Txt>& rv, bool backref)
{
	Txt t_copy(t);
	i32 count=0;
	i32 pos_offset=0;
	i32 rv_count=rv.GetCount();
	Vec<Txt> new_rv;

	new_rv.Shrink();
	first=false;

	while(GlobalMatch(t_copy)){

			if(backref){

				 new_rv.Clear();
				 new_rv = ResolveBackRef(rv);
				 count += Replace0(t, new_rv, rv_count, pos_offset);

			}
			else count += Replace0(t, rv, rv_count, pos_offset);

	}

	return count;

}

i32 RegExp::ReplaceGlobal(Txt& t, const Txt& r, bool backref)
{
	Vec<Txt> rv(Make_rv(r));
	return ReplaceGlobal(t, rv, backref);
}

i32 RegExp::ReplaceGlobal(Txt& t, Event<Vec<Txt>&> cbr)
{
	Txt t_copy(t);
	i32 count=0;
	i32 pos_offset=0;
	i32 rv_count=0;
	Vec<Txt> rv;

	first = false;
	while(GlobalMatch(t_copy)) {
		rv.Clear();
		rv=GetTxts();
		cbr(rv);
		rv_count=rv.GetCount();
		count += Replace0(t, rv, rv_count, pos_offset);
	}

	return count;

}

}
