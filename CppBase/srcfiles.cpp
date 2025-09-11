#include "CppBase.h"
#include "Internal.h"

namespace drx {

#define LTIMING(x) // DTIMING(x)
#define LLOG(x) // DLOG(x)

static VecMap<Txt, Txt> sSrcFile;
static Index<zu64>             sIncludes;

Txt NormalizeSourcePath(const Txt& path, const Txt& currdir)
{
	LTIMING("NormalizeSourcePath");
#ifdef TARGET_WIN32
	return ToLower(NormalizePath(path, currdir));
#else
	return NormalizePath(path, currdir);
#endif
}

Txt NormalizeSourcePath(const Txt& path)
{
	return NormalizeSourcePath(path, GetCurDir());
}

void ClearSources()
{
	sSrcFile.Clear();
	sIncludes.Clear();
}

const Index<Txt>& GetAllSources()
{
	return sSrcFile.GetIndex();
}

const VecMap<Txt, Txt>& GetAllSourceMasters()
{
	return sSrcFile;
}

void GatherSources(const Txt& master_path, const Txt& path_, Vec<i32>& parents)
{
	Txt path = NormalizeSourcePath(path_);
	LLOG("--- GatherSources " << master_path << " " << path);
	if(sSrcFile.Find(path) >= 0)
		return;
	i32 ii = sSrcFile.GetCount();
	for(i32 i = 0; i < parents.GetCount(); i++)
		sIncludes.Add(MAKEQWORD(parents[i], ii));
	sSrcFile.Add(path, master_path);
	parents.Add(ii);
	const PPFile& f = GetPPFile(path);
	Index<Txt> todo;
	for(Txt inc : f.includes) {
		Txt p = GetIncludePath(inc, GetFileFolder(path));
		if(p.GetCount())
			todo.FindAdd(p);
	}
	MakePP(todo); // parse PP files in parallel to accelerate things...
	for(Txt p : todo)
		GatherSources(master_path, p, parents);
	parents.Drop();
}

void GatherSources(const Txt& path)
{
	LTIMING("GatherSources");
	Vec<i32> parents;
	LLOG("=== GatherSources " << path);
	MakePP({ NormalizeSourcePath(path) });
	GatherSources(NormalizeSourcePath(path), path, parents);
}

Txt GetMasterFile(const Txt& file)
{
	return sSrcFile.Get(file, Null);
}

bool   IncludesFile(const Txt& parent_path, const Txt& header_path)
{
	LTIMING("IncludesFile");
	i32 pi = sSrcFile.Find(parent_path);
	i32 i = sSrcFile.Find(header_path);
	return pi >= 0 && i >= 0 && sIncludes.Find(MAKEQWORD(pi, i)) >= 0;
}

}
