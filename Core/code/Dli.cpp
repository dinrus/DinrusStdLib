#include "../Core.h"

#ifdef TARGET_WIN32
typedef HMODULE DLLHANDLE;
#else
typedef void   *DLLHANDLE;
#endif

#define LLOG(x) // RLOG(x)

#ifdef TARGET_WIN32

#include <winnt.h>

namespace drx {

class PeFile
{
public:
	PeFile(tukk data = 0) { Open(data); }

	bool                        Open(tukk data);

	i32                         GetExportCount() const;
	const char                 *GetExport(i32 index) const;
	const char                 *FindExportRaw(tukk name, bool case_sensitive = true) const;

	i32                         GetSectionIndex(tukk name) const;

private:
	const char                 *data;
	const IMAGE_NT_HEADERS     *headers;
	const IMAGE_SECTION_HEADER *sections;
	const IMAGE_EXPORT_DIRECTORY *exports;
};

bool PeFile::Open(tukk _data)
{
	data = _data;
	if(!data || data[0] != 'M' || data[1] != 'Z')
		return false;
	i32 pe = Peek32le(data + 0x3C);
	if(IsBadReadPtr(data + pe, sizeof(IMAGE_NT_HEADERS)))
		return false;
	if(memcmp(data + pe, "PE\0\0", 4))
		return false;
	headers = (const IMAGE_NT_HEADERS *)(data + pe);
	sections = (const IMAGE_SECTION_HEADER *)(headers + 1);
	exports = (const IMAGE_EXPORT_DIRECTORY *)(data
		+ headers->OptionalHeader.DataDir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	return true;
}

i32 PeFile::GetSectionIndex(tukk name) const
{
	for(i32 i = 0, n = headers->FileHeader.NumberOfSections; i < n; i++)
		if(!strcmp((tukk )sections[i].Name, name))
			return i;
	return -1;
}

i32 PeFile::GetExportCount() const
{
	return exports ? exports->NumberOfNames : 0;
}

tukk PeFile::GetExport(i32 index) const
{
	if(!exports || index < 0 || index >= (i32) exports->NumberOfNames)
		return 0;
	return data + ((const dword *)(data + (dword)exports->AddressOfNames))[index];
}

static bool EqualMem(tukk a, tukk b, i32 len, bool case_sensitive)
{
	if(case_sensitive)
		return !memcmp(a, b, len);
	else
		return !MemICmp(a, b, len);
}

tukk PeFile::FindExportRaw(tukk name, bool case_sensitive) const
{
	if(!exports || !name || !*name)
		return 0;
	i32 len = (i32)strlen(name);
	const dword *pnames = (const dword *)(data + (dword)exports->AddressOfNames);
	for(i32 i = 0; i < (i32) exports->NumberOfNames; i++) {
		tukk exp = data + pnames[i];
		i32 elen = (i32)strlen(exp);
		if(elen < len)
			continue;
		if(elen == len && EqualMem(exp, name, len, case_sensitive))
			return exp;
		if(EqualMem(exp, name, len, case_sensitive) && exp[len] == '@')
			return exp;
		if(exp[0] == '_' && EqualMem(exp + 1, name, len, case_sensitive)
			&& (exp[len + 1] == '@' || exp[len + 1] == 0))
			return exp;
	}
	return 0;
}

}

HMODULE CheckDll__(tukk fn, tukk const *names, DRX::Vec<uk >& plist)
{
	HMODULE hmod = LoadLibrary(fn);
	
	if(!hmod)
		return 0;

	DRX::PeFile pe;
	if(!pe.Open((tukk )hmod)) {
		FreeLibrary(hmod);
		return 0;
	}

	i32 missing = 0;
	for(tukk const *p = names; *p; p++) {
		tukk exp = *p;
		bool optional = (*exp == '?');
		if(optional) exp++;
		tukk name = pe.FindExportRaw(exp);
		uk proc = 0;
		if(!name || !(proc = (uk )GetProcAddress(hmod, name)))
			if(!optional) {
				if(!missing) {
					LLOG(fn << " missing exports:");
				}
				LLOG(exp);
				missing++;
			}
		plist.Add(proc);
	}
	if(missing) {
		LLOG(missing << " total");
		FreeLibrary(hmod);
		return 0;
	}
	return hmod;
}

void FreeDll__(HMODULE hmod)
{
	FreeLibrary(hmod);
}

#endif

#ifdef TARGET_POSIX

#include <dlfcn.h>

uk CheckDll__(tukk fn, tukk const *names, DRX::Vec<uk >& plist)
{
	drx::MemoryIgnoreLeaksBlock __;
	uk hmod = dlopen(fn, RTLD_LAZY | RTLD_GLOBAL);
	if(!hmod)
		return 0;

	i32 missing = 0;
	for(tukk const *p = names; *p; p++) {
		tukk exp = *p;
		bool optional = (*exp == '?');
		if(optional) exp++;
		uk proc = dlsym(hmod, exp);
		if(!proc && !optional) {
			if(!missing) {
				LOG(fn << " missing exports:");
			}
			LOG(exp);
		}
		plist.Add(proc);
	}

	if(missing) {
		LOG(missing << " missing symbols total");
		dlclose(hmod);
		return 0;
	}

	return hmod;
}

void FreeDll__(uk hmod)
{
	if(hmod)
		dlclose(hmod);
}

#endif//TARGET_POSIX

DLLHANDLE LoadDll__(DRX::Txt& inoutfn, tukk const *names, uk const *procs)
{
	tukk fn = inoutfn;
	while(*fn) {
		tukk b = fn;
		while(*fn && *fn != ';'
#ifndef TARGET_WIN32
			&& *fn != ':'
#endif
		)
			fn++;
		DRX::Txt libname(b, fn);
		if(*fn)
			fn++;
		DRX::Vec<uk> plist;
		if(DLLHANDLE hmod = CheckDll__(libname, names, plist)) {
			for(i32 i = 0; i < plist.GetCount(); i++)
				*(uk *)*procs++ = plist[i];
			inoutfn = libname;
			return hmod;
		}
	}
	return 0;
}
