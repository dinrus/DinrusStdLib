#include "../Core.h"

#ifdef TARGET_POSIX
#include <sys/mman.h>
#endif

namespace drx {

static i32 sMappingGranularity_()
{
#ifdef TARGET_WIN32
	static i32 mg = 0;
	if(!mg) {
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		mg = info.dwAllocationGranularity;
	}
#else
	static i32 mg = 4096;
#endif
	return mg;
}

FileMapping::FileMapping(tukk file_, bool delete_share_)
{
#ifdef TARGET_WIN32
	hfile = INVALID_HANDLE_VALUE;
	hmap = NULL;
#endif
#ifdef TARGET_POSIX
	hfile = -1;
	Zero(hfstat);
#endif
	base = rawbase = NULL;
	size = rawsize = 0;
	offset = rawoffset = 0;
	filesize = -1;
	write = false;
	if(file_)
		Open(file_, delete_share_);

}

bool FileMapping::Open(tukk file, bool delete_share)
{
	Close();
	write = false;
#ifdef TARGET_WIN32
	hfile = CreateFileW(ToSystemCharsetW(file), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(hfile == INVALID_HANDLE_VALUE)
		return false;
	filesize = ::GetFileSize(hfile, NULL);
	hmap = CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 0, NULL);
	if(!hmap) {
		Close();
		return false;
	}
#endif
#ifdef TARGET_POSIX
	hfile = open(ToSystemCharset(file), O_RDONLY);
	if(hfile == -1)
		return false;
	if(fstat(hfile, &hfstat) == -1) {
		Close();
		return false;
	}
	filesize = hfstat.st_size;
#endif
	return true;
}

bool FileMapping::Create(tukk file, z64 filesize_, bool delete_share)
{
	Close();
	write = true;
#ifdef TARGET_WIN32
	hfile = CreateFileW(ToSystemCharsetW(file), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | (delete_share ? FILE_SHARE_DELETE : 0),
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(hfile == INVALID_HANDLE_VALUE)
		return false;
	long lo = (dword)filesize_, hi = (dword)(filesize_ >> 32);
	hmap = CreateFileMapping(hfile, NULL, PAGE_READWRITE, hi, lo, NULL);
	if(!hmap) {
		Close();
		return false;
	}
#endif
#ifdef TARGET_POSIX
	hfile = open(ToSystemCharset(file), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(hfile == -1)
		return false;
#endif
	filesize = filesize_;
	return true;
}

bool FileMapping::Map(z64 mapoffset, size_t maplen)
{
	ASSERT(IsOpen());
	if(maplen == 0)
		return Unmap();
	mapoffset = minmax<z64>(mapoffset, 0, filesize);
	i32 gran = sMappingGranularity_();
	z64 rawoff = mapoffset & -gran;
	maplen = (size_t)min<z64>(maplen, filesize - mapoffset);
	size_t rawsz = (size_t)min<z64>((maplen + (size_t)(mapoffset - rawoff) + gran - 1) & -gran, filesize - rawoff);
	if(rawbase && (mapoffset < rawoffset || mapoffset + maplen > rawoffset + rawsize))
		Unmap();
	if(!rawbase) {
		rawoffset = rawoff;
		rawsize = rawsz;
#ifdef TARGET_WIN32
		rawbase = (u8 *)MapViewOfFile(hmap, write ? FILE_MAP_WRITE : FILE_MAP_READ,
			(dword)(rawoffset >> 32), (dword)(rawoffset >> 0), rawsize);
#else
		rawbase = (u8 *)mmap(0, rawsize,
			PROT_READ | (write ? PROT_WRITE : 0),
#ifdef TARGET_FREEBSD
			MAP_NOSYNC,
#else
			MAP_SHARED,
#endif
			hfile, rawoffset);
#endif
#ifdef TARGET_POSIX
		if(rawbase == (u8 *)~0)
#else
		if(!rawbase)
#endif
			return false;
	}
	offset = mapoffset;
	size = maplen;
	base = rawbase + (i32)(offset - rawoffset);
	return true;
}

bool FileMapping::Unmap()
{
	bool ok = true;
	if(rawbase) {
#ifdef TARGET_WIN32
		ok = !!UnmapViewOfFile(rawbase);
#endif
#ifdef TARGET_POSIX
		ok = (munmap((uk )rawbase, rawsize) == 0);
#endif
	}
	base = rawbase = NULL;
	size = 0;
	return ok;
}

bool FileMapping::Expand(z64 new_filesize)
{
	ASSERT(IsOpen());
	if(new_filesize > filesize) {
		if(!Unmap())
			return false;
#ifdef TARGET_WIN32
		if(!CloseHandle(hmap)) {
			hmap = NULL;
			return false;
		}
		hmap = NULL;
#endif
#ifdef TARGET_POSIX
		if(FTRUNCATE64_(hfile, new_filesize - filesize) != 0) {
			Close();
			return false;
		}
#endif
		filesize = new_filesize;
	}
	return true;
}

bool FileMapping::Close()
{
	bool ok = Unmap();
#ifdef TARGET_WIN32
	if(hmap) {
		if(!CloseHandle(hmap)) ok = false;
		hmap = NULL;
	}
	if(IsOpen()) {
		if(!CloseHandle(hfile)) ok = false;
		hfile = INVALID_HANDLE_VALUE;
	}
#endif
#ifdef TARGET_POSIX
	if(IsOpen()) {
		if(close(hfile) != 0) ok = false;
		Zero(hfstat);
		hfile = -1;
	}
#endif
	filesize = -1;
	offset = 0;
	size = 0;
	write = false;
	return ok;
}

Time FileMapping::GetTime() const
{
	ASSERT(IsOpen());
#ifdef TARGET_WIN32
	FileTime ft;
	GetFileTime(hfile, NULL, NULL, &ft);
	return ft;
#endif
#ifdef TARGET_POSIX
	return Time(hfstat.st_mtime);
#endif
}

Txt FileMapping::GetData(z64 offset, i32 len)
{
	if(IsOpen() && Map(offset, len))
		return Txt(base, len);
	else {
		NEVER();
		return Txt::GetVoid();
	}
}

}
