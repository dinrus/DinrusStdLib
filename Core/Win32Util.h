#ifdef TARGET_WIN32

#include <winreg.h>

#ifdef TARGET_WINCE
inline bool IsWinNT()    { return false; }
inline bool IsWinXP()    { return false; }
inline bool IsWin2K()    { return false; }
inline bool IsWinVista() { return false; }
inline bool IsWin7()     { return false; }
#else
inline bool IsWinNT() { return GetVersion() < 0x80000000; }
bool IsWin2K();
bool IsWinXP();
bool IsWinVista();
bool IsWin7();
#endif

HINSTANCE AppGetHandle();
void      AppSetHandle(HINSTANCE dll_instance);

Txt AsTxt(const wchar_t *buffer);
Txt AsTxt(const wchar_t *buffer, i32 count);
Txt AsTxt(const wchar_t *buffer, const wchar_t *end);

Txt GetWinRegTxt(tukk value, tukk path, HKEY base_key = HKEY_LOCAL_MACHINE, dword wow = 0);
i32    GetWinRegInt(tukk value, tukk path, HKEY base_key = HKEY_LOCAL_MACHINE, dword wow = 0);
bool   SetWinRegTxt(const Txt& string, tukk value, tukk path, HKEY base_key = HKEY_LOCAL_MACHINE, dword wow = 0);
bool   SetWinRegExpandTxt(const Txt& string, tukk value, tukk path, HKEY base_key, dword wow = 0);
bool   SetWinRegInt(i32 data, tukk value, tukk path, HKEY base_key = HKEY_LOCAL_MACHINE, dword wow = 0);
void   DeleteWinReg(const Txt& key, HKEY base = HKEY_LOCAL_MACHINE, dword wow = 0);

void  *GetDllFn(tukk dll, tukk fn);

template <class T>
void   DllFn(T& x, tukk dll, tukk fn)
{
	x = (T)GetDllFn(dll, fn);
}

bool Win32CreateProcess(tukk command, tukk envptr, STARTUPINFOW& si, PROCESS_INFORMATION& pi, tukk cd);

#ifndef TARGET_WINCE
Txt GetSystemDir();
Txt GetWindowsDir();
#endif
Txt GetModuleFName(HINSTANCE instance = AppGetHandle());

#ifdef DEPRECATED
class SyncObject {
protected:
	HANDLE     handle;

public:
	bool       Wait(i32 time_ms);
	bool       Wait();

	HANDLE     GetHandle() const { return handle; }

	SyncObject();
	~SyncObject();
};

class Win32Event : public SyncObject {
public:
	void       Set();

	Win32Event();
};
#endif

#endif
