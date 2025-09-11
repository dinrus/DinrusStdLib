#include "CppBase.h"
#include "Internal.h"

namespace drx {

#define LTIMING(x)  // DTIMING(x)
#define LLOG(x)     // DLOG(x)

static std::atomic<i32> s_PPserial;
static VecMap<Txt, PPMacro>  sAllMacros; // Only MakePP can write to this
static ArrayMap<Txt, PPFile>    sPPfile; // Only MakePP can write to this

static VecMap<Txt, Time>     s_PathFileTime;
static StaticMutex                 s_PathFileTimeMutex;

static VecMap<Txt, Txt>   s_IncludePath;
static Txt                      s_Include_Path;
static StaticMutex                 s_IncludePathMutex;

static StaticMutex                 s_FlatPPMutex;
static ArrayMap<Txt, FlatPP>    s_FlatPP; // ArrayMap to allow read access

i32  NextPPSerial()
{
	return ++s_PPserial;
}

void SweepPPFiles(const Index<Txt>& keep)
{
	Index<i32> pp_segment_id;
	i32 unlinked_count = 0;
	for(i32 i = 0; i < sPPfile.GetCount(); i++)
		if(sPPfile.IsUnlinked(i))
			unlinked_count++;
		else
			if(keep.Find(sPPfile.GetKey(i)) < 0) {
				unlinked_count++;
				sPPfile.Unlink(i);
			}
			else {
				const PPFile& p = sPPfile[i];
				for(i32 j = 0; j < p.item.GetCount(); j++)
					pp_segment_id.FindAdd(p.item[j].segment_id);
			}
	if(unlinked_count > sPPfile.GetCount() / 2) {
		CleanPP();
		return;
	}
	unlinked_count = 0;
	for(i32 i = 0; i < sAllMacros.GetCount(); i++) {
		if(sAllMacros.IsUnlinked(i))
			unlinked_count++;
		else
		if(sAllMacros[i].segment_id && pp_segment_id.Find(sAllMacros[i].segment_id) < 0) {
			sAllMacros.Unlink(i);
			unlinked_count++;
		}
		if(unlinked_count > sAllMacros.GetCount() / 2) {
			CleanPP();
			return;
		}
	}
}

Txt GetSegmentFile(i32 segment_id)
{
	for(i32 i = 0; i < sPPfile.GetCount(); i++) {
		const Array<PPItem>& m = sPPfile[i].item;
		for(i32 j = 0; j < m.GetCount(); j++)
			if(m[j].type == PP_DEFINES && m[j].segment_id == segment_id)
				return sPPfile.GetKey(i);
	}
	return "<not found>";
}

PPMacro *FindPPMacro(const Txt& id, Index<i32>& segment_id, i32& segmenti)
{
	Index<i32> undef;
	PPMacro *r;
	i32 best;
	for(i32 pass = 0; pass < 2; pass++) {
		r = NULL;
		best = segmenti;
		i32 line = -1;
		i32 q = sAllMacros.Find(id);
		while(q >= 0) {
			PPMacro& m = sAllMacros[q];
			if(m.macro.IsUndef()) {
				if(pass == 0 && segment_id.Find(m.segment_id) >= 0)
					undef.FindAdd(m.segment_id); // cancel out undefined macro...
			}
			else
			if(pass == 0 || m.segment_id == 0 || undef.Find(m.undef_segment_id) < 0) {
				i32 si = m.segment_id == 0 ? INT_MAX : segment_id.Find(m.segment_id); // defs macros always override
				if(si > best || si >= 0 && si == best && m.line > line) {
					best = si;
					line = m.line;
					r = &m;
				}
			}
			q = sAllMacros.FindNext(q);
		}
		if(undef.GetCount() == 0)
			break;
	}
	segmenti = best;
	return r;
}

const CppMacro *FindMacro(const Txt& id, Index<i32>& segment_id, i32& segmenti)
{
	PPMacro *m = FindPPMacro(id, segment_id, segmenti);
	return m ? &m->macro : NULL;
}

void PPFile::CheckEndNamespace(Vec<i32>& namespace_block, i32 level, Md5Stream& md5)
{
	if(namespace_block.GetCount() && namespace_block.Top() == level) {
		namespace_block.Drop();
		item.Add().type = PP_NAMESPACE_END;
		md5.Put('.');
	}
}

void PPFile::Parse(Stream& in)
{
	LTIMING("PPFile::Parse");
	for(i32 i = 0; i < ppmacro.GetCount(); i++)
		sAllMacros.Unlink(ppmacro[i]);
	ppmacro.Clear();
	item.Clear();
	includes.Clear();
	bool was_using = false;
	bool was_namespace = false;
	i32  level = 0;
	bool incomment = false;
	bool do_pp = true;
	Vec<i32> namespace_block;
	bool next_segment = true;
	Index<i32> local_segments;
	keywords.Clear();
	i32 linei = 0;
	Md5Stream md5;
	i32 current_serial = 0;
	
	VecMap<Txt, PPMacro> local_macro; // gather all macros first to reduce locking
	
	while(!in.IsEof()) {
		Txt l = in.GetLine();
		tukk ll = l;
		while(*ll == ' ' || *ll == '\t')
			ll++;
		if(ll[0] == '/' && ll[1] == '/' && ll[2] == '$')
			do_pp = decode(ll[3], '+', true, '-', false, do_pp);
		while(*l.Last() == '\\' && !in.IsEof()) {
			l.Trim(l.GetLength() - 1);
			l.Cat(in.GetLine());
		}
		RemoveComments(l, incomment);
		try {
			CParser p(l);
			if(p.Char('#')) {
				if(do_pp) {
					if(p.Id("define")) {
						if(next_segment) {
							PPItem& m = item.Add();
							m.type = PP_DEFINES;
							m.segment_id = current_serial = NextPPSerial();
							next_segment = false;
							local_segments.Add(current_serial);
						}
						CppMacro def;
						Txt   id = def.Define(p.GetPtr());
						if(id.GetCount()) {
							PPMacro& l = local_macro.Add(id);
							l.segment_id = current_serial;
							l.line = linei;
							l.macro = def;
						/*
							PPMacro m;
							m.segment_id = current_serial;
							m.line = linei;
							m.macro = def;
							ppmacro.Add(sAllMacros.Put(id, m));
						*/
							md5.Put("#", 1);
							md5.Put(id);
							md5.Put(0);
							md5.Put(l.macro.md5, 16);
						}
					}
					else
					if(p.Id("undef")) {
						if(p.IsId()) {
							Txt id = p.ReadId();
							if(id.GetCount()) {
								md5.Put("#", 1);
								md5.Put(id);
								md5.Put(1);
								i32 q = local_macro.FindLast(id); // heuristic: only local undefs are allowed
								while(q >= 0) {
									PPMacro& um = local_macro[q];
									if(!um.macro.IsUndef()) { // found corresponding macro to undef
										PPItem& m = item.Add();
										m.type = PP_DEFINES;
										m.segment_id = current_serial = NextPPSerial();
										um.undef_segment_id = m.segment_id;
										next_segment = true;
										local_segments.Add(current_serial);
										PPMacro& l = local_macro.Add(id);
										l.segment_id = current_serial;
										l.line = linei;
										l.macro.SetUndef();
									}
									q = local_macro.FindPrev(q);
								}
							}
						/*
							i32 segmenti = -1;
							PPMacro *um = FindPPMacro(id, local_segments, segmenti);
							if(um && segmenti) {
								PPItem& m = item.Add();
								m.type = PP_DEFINES;
								m.segment_id = current_serial = NextPPSerial();
								um->undef_segment_id = m.segment_id;
								next_segment = true;
								local_segments.Add(current_serial);
								if(id.GetCount()) {
									PPMacro m;
									m.segment_id = current_serial;
									m.line = linei;
									m.macro.SetUndef();
									ppmacro.Add(sAllMacros.Put(id, m));
								}
							}
						*/
						}
					}
					else
					if(p.Id("include")) {
						PPItem& m = item.Add();
						next_segment = true;
						m.type = PP_INCLUDE;
						m.text = TrimBoth(p.GetPtr());
						if(IsNull(m.text))
							item.Drop();
						else
							includes.FindAdd(m.text);
						md5.Put('@');
						md5.Put(m.text);
					}
				}
			}
			else {
				while(!p.IsEof()) {
					if(was_namespace) {
						i32 type = was_using ? PP_USING : PP_NAMESPACE;
						Txt id;
						while(p.Char2(':', ':'))
							id = "::";
						if(p.IsId()) {
							id << p.ReadId();
							while(p.Char2(':', ':') && p.IsId())
								id << "::" << p.ReadId();
							if(!was_using)
								namespace_block.Add(level);
							if(!was_using || level == 0) {
								PPItem& m = item.Add();
								next_segment = true;
								m.type = type;
								m.text = id;
							}
							md5.Put('$');
							md5.Put(type);
							md5.Put(id);
						}
						was_namespace = was_using = false;
					}
					else
					if(p.Id("using"))
						was_using = true;
					else
					if(p.Id("namespace"))
						was_namespace = true;
					else {
						was_using = was_namespace = false;
						if(p.IsId()) {
							static const VecMap<Txt, Txt>& namespace_macro = GetNamespaceMacros();
							static const Index<Txt>& namespace_end_macro = GetNamespaceEndMacros();

							Txt id = p.ReadId();
							i32 q = namespace_macro.Find(id);
							if(q > 0) {
								PPItem& m = item.Add();
								next_segment = true;
								m.type = PP_NAMESPACE;
								m.text = namespace_macro[q];
								namespace_block.Add(level);
								level++;
								md5.Put('%');
								md5.Put(id);
							}
							else {
								q = namespace_end_macro.Find(id);
								if(q >= 0) {
									level--;
									CheckEndNamespace(namespace_block, level, md5);
								}
							}
							keywords.Add(id);
						}
						else
						if(p.Char('}')) {
							if(level > 0) {
								level--;
								CheckEndNamespace(namespace_block, level, md5);
							}
						}
						else
						if(p.Char('{'))
							level++;
						else
							p.SkipTerm();
					}
				}
			}
		}
		catch(...) {}
		linei++;
	}
	md5sum = md5.FinishTxt();
	Sort(keywords);
	Vec<i32> remove;
	i32 i = 0;
	while(i < keywords.GetCount()) { // Remove identical items
		i32 ii = i;
		i++;
		while(i < keywords.GetCount() && keywords[ii] == keywords[i])
			remove.Add(i++);
	}
	keywords.Remove(remove);
	INTERLOCKED { // this is the only place that is allowed to write to sAllMacros
		for(i32 i = 0; i < local_macro.GetCount(); i++)
			ppmacro.Add(sAllMacros.Put(local_macro.GetKey(i), local_macro[i]));
	}
}

void PPFile::Dump() const
{
	for(i32 i = 0; i < item.GetCount(); i++) {
		const PPItem& m = item[i];
		Txt ll;
		ll << decode(m.type, PP_DEFINES, "#defines ", PP_INCLUDE, "#include ",
		                     PP_USING, "using namespace ", PP_NAMESPACE, "namespace ",
		                     PP_NAMESPACE_END, "}", "");
		if(m.type == PP_DEFINES)
			ll << m.segment_id;
		else
			ll << m.text;
		if(m.type == PP_NAMESPACE)
			ll << " {";
		LOG(ll);
	}
	LOG("----- includes:");
	DUMPC(includes);
}

void InvalidatePPCache()
{
	{
		Mutex::Lock __(s_IncludePathMutex);
		s_IncludePath.Clear();
	}
	{
		Mutex::Lock __(s_FlatPPMutex);
		s_FlatPP.Clear();
	}
}

void PPSync(const Txt& include_path)
{
	LLOG("* PPSync");
	bool update = false;
	{
		Mutex::Lock __(s_IncludePathMutex);
		if(s_Include_Path != include_path) {
			s_IncludePath.Clear();
			s_Include_Path = include_path;
			update = true;
		}
	}
	if(update) {
		Mutex::Lock __(s_FlatPPMutex);
		s_FlatPP.Clear();
	}
}

void InvalidateFileTimeCache()
{
	Mutex::Lock __(s_PathFileTimeMutex);
	s_PathFileTime.Clear();
}

void InvalidateFileTimeCache(const Txt& path)
{
	LLOG("InvalidateFileTimeCache " << path);
	Mutex::Lock __(s_PathFileTimeMutex);
	s_PathFileTime.UnlinkKey(path);
}

Time GetFileTimeCached(const Txt& p)
{
	LTIMING("GetFileTimeCached");
	{
		Mutex::Lock __(s_PathFileTimeMutex);
		i32 q = s_PathFileTime.Find(p);
		if(q >= 0)
			return s_PathFileTime[q];
	}
	Time m = FileGetTime(p);
	{
		Mutex::Lock __(s_PathFileTimeMutex);
		s_PathFileTime.Put(p, m);
	}
	return m;
}

Txt GetIncludePath()
{
	Mutex::Lock __(s_IncludePathMutex);
	return s_Include_Path;
}

Txt GetIncludePath0(tukk s, tukk filedir)
{
	LTIMING("GetIncludePath0");
	while(IsSpace(*s))
		s++;
	i32 type = *s;
	if(type == '<' || type == '\"' || type == '?') {
		s++;
		Txt name;
		if(type == '<') type = '>';
		while(*s != '\r' && *s != '\n' && *s) {
			if(*s == type) {
				if(type == '\"') {
					Txt fn = NormalizeSourcePath(name, filedir);
					if(FileExists(fn))
						return fn;
				}
				Txt p = GetFileOnPath(name, GetIncludePath(), false);
				if(p.GetCount())
					return NormalizeSourcePath(p);
				return Null;
			}
			name.Cat(*s++);
		}
	}
	return Null;
}

Txt GetIncludePath(const Txt& s, const Txt& filedir)
{
	LTIMING("GetIncludePath");
	Mutex::Lock __(s_IncludePathMutex);
	Txt key;
	key << s << "#" << filedir;
	i32 q = s_IncludePath.Find(key);
	if(q >= 0)
		return s_IncludePath[q];
	LTIMING("GetIncludePath 2");
	Txt p = GetIncludePath0(s, filedir);
	s_IncludePath.Add(key, p);
	LLOG("GetIncludePath " << s << " " << filedir << ": " << p);
	return p;
}

void MakePP(const Index<Txt>& paths)
{
	LLOG("MakePP " << paths);
	Vec<Txt> todo;
	Vec<PPFile *> pp;
	for(i32 i = 0; i < paths.GetCount(); i++) {
		Txt path = paths[i];
		PPFile& f = sPPfile.GetPut(path);
		Time tm = GetFileTimeCached(path);
		if(f.filetime != tm) {
			f.filetime = tm;
			pp.Add(&f);
			todo.Add(path);
		}
	}
	CoFor(todo.GetCount(), [&](i32 i) {
		FileIn in(todo[i]);
		pp[i]->Parse(in);
	});
}

const PPFile& GetPPFile(tukk path)
{
	LTIMING("GetPPFile");
	LLOG("GetPPFile " << path);
	static PPFile zero;
	return sPPfile.Get(path, zero);
}

bool IsSameFile(const Txt& f1, const Txt& f2)
{
	return NormalizePath(f1) == NormalizePath(f2);
}

const FlatPP& GetFlatPPFile(tukk path, Index<Txt>& visited)
{
	LTIMING("GetFlatPPFile");
	LLOG("GetFlatPPFile " << path);
	Mutex::Lock __(s_FlatPPMutex);
	i32 q = s_FlatPP.Find(path);
	if(q >= 0) {
		LLOG("From cache");
		return s_FlatPP[q];
	}
	FlatPP& fp = s_FlatPP.Add(path);
	const PPFile& pp = GetPPFile(path);
	i32 n = visited.GetCount();
	visited.FindAdd(path);
	for(i32 i = 0; i < pp.item.GetCount(); i++) {
		const PPItem& m = pp.item[i];
		if(m.type == PP_INCLUDE) {
			Txt s = GetIncludePath(m.text, GetFileFolder(path));
			LLOG("#include " << m.text << " -> " << s);
			if(s.GetCount() && visited.Find(s) < 0) {
				visited.Add(s);
				const FlatPP& pp = GetFlatPPFile(s, visited);
				for(i32 i = 0; i < pp.segment_id.GetCount(); i++)
					fp.segment_id.FindAdd(pp.segment_id[i]);
				for(i32 i = 0; i < pp.usings.GetCount(); i++)
					fp.usings.FindAdd(pp.usings[i]);
			}
		}
		else
		if(m.type == PP_DEFINES)
			fp.segment_id.FindAdd(m.segment_id);
		else
		if(m.type == PP_USING)
			fp.usings.FindAdd(m.text);
	}
	visited.Trim(n);
	return fp;
}

const FlatPP& GetFlatPPFile(tukk path)
{
	Index<Txt> visited;
	visited.Add(path);
	return GetFlatPPFile(path, visited);
}

void GetAllMacros(Md5Stream& md5, const Txt& id, Index<i32>& segment_id)
{
	Vec< Tuple2<i32, i32> > pos;
	Vec<const CppMacro *> def;
	Txt r;
	i32 q = sAllMacros.Find(id);
	while(q >= 0) {
		const PPMacro& m = sAllMacros[q];
		i32 si = segment_id.Find(m.segment_id);
		if(si >= 0) {
			pos.Add(MakeTuple(si, m.line));
			def.Add(&m.macro);
		}
		q = sAllMacros.FindNext(q);
	}
	IndexSort(pos, def);
	i32 n = def.GetCount();
	if(n) {
		md5.Put(&n, sizeof(i32));
		md5.Put(id);
		for(i32 i = 0; i < n; i++)
			md5.Put(def[i]->md5, 16);
	}
}

static VecMap<Txt, Txt> s_namespace_macro;
static Index<Txt> s_namespace_end_macro;

static Txt sDefs;

void LoadPPConfig()
{
	for(i32 i = 0; i < sAllMacros.GetCount(); i++)
		if(sAllMacros[i].segment_id == 0 && !sAllMacros.IsUnlinked(i))
			sAllMacros.Unlink(i);

	s_namespace_macro.Clear();
	s_namespace_end_macro.Clear();

	TxtStream ss(sDefs);
	i32 linei = 0;
	while(!ss.IsEof()) {
		Txt l = ss.GetLine();
		try {
			CParser p(l);
			if(p.Char('#')) {
				if(p.Id("define")) {
					CppMacro def;
					Txt   id = def.Define(p.GetPtr());
					if(id.GetCount()) {
						PPMacro m;
						m.segment_id = 0;
						m.line = linei;
						m.macro = def;
						sAllMacros.Put(id, m);
						if(findarg(TrimBoth(def.body), "}", "};") >= 0)
							s_namespace_end_macro.Add(id);
						try {
							CParser p(def.body);
							if(p.Id("namespace") && p.IsId()) {
								Txt n = p.ReadId();
								if(p.Char('{') && p.IsEof())
									s_namespace_macro.Add(id, n);
							}
						}
						catch(CParser::Error) {}
					}
				}
			}
		}
		catch(CParser::Error) {}
		linei++;
	}
}

const VecMap<Txt, Txt>& GetNamespaceMacros()
{
	return s_namespace_macro;
}

const Index<Txt>& GetNamespaceEndMacros()
{
	return s_namespace_end_macro;
}

void SetPPDefs(const Txt& defs)
{
	sDefs = defs;
	LoadPPConfig();
}

void CleanPP()
{
	sAllMacros.Clear();
	sPPfile.Clear();
	s_PPserial = 0;
	LoadPPConfig();
}

void SerializePPFiles(Stream& s)
{
	i32 sPPserial = s_PPserial;
	s % sAllMacros % sPPfile % sPPserial;
	s_PPserial = sPPserial;
	if(s.IsLoading())
		LoadPPConfig();

#if 0
	if(s.IsLoading()) { _DBG_
		DDUMP(sPPfile.GetCount());
		DDUMP(sAllMacros.GetCount());
		DDUMP(sPPserial);
		
		Index<i32> psegment;
		for(i32 i = 0; i < sPPfile.GetCount(); i++) {
			const PPFile& p = sPPfile[i];
			for(i32 j = 0; j < p.item.GetCount(); j++)
				psegment.FindAdd(p.item[j].segment_id);
		}
		DDUMP(psegment.GetCount());
			
		i32 n = 0; _DBG_
		Index<i32> msegment;
		for(i32 i = 0; i < sAllMacros.GetCount(); i++) { _DBG_
			if(sAllMacros.IsUnlinked(i))
				n++;
			else
				msegment.FindAdd(sAllMacros[i].segment_id);
		}
		DLOG("UNLINKED " << n);
		DLOG("Segments " << msegment.GetCount());
	}
#endif
}

}
