#include "../Core.h"

namespace drx {

#ifdef TARGET_WIN32

static HINSTANCE app_instance;

HINSTANCE AppGetHandle()
{
	if(!app_instance)
		app_instance = GetModuleHandle(NULL);
	return app_instance;
}

void AppSetHandle(HINSTANCE dll_instance) { app_instance = dll_instance; }

#ifndef TARGET_WINCE
bool IsWin2K()
{
	OSVERSIONINFO of;
	of.dwOSVersionInfoSize = sizeof(of);
	GetVersionEx(&of);
	return of.dwMajorVersion >= 5;
}

bool IsWinXP()
{
	OSVERSIONINFO of;
	of.dwOSVersionInfoSize = sizeof(of);
	GetVersionEx(&of);
	return of.dwMajorVersion > 5 || of.dwMajorVersion == 5 && of.dwMinorVersion >= 1;
}

bool IsWinVista()
{
	OSVERSIONINFO of;
	of.dwOSVersionInfoSize = sizeof(of);
	GetVersionEx(&of);
	return of.dwMajorVersion >= 6;
}

bool IsWin7()
{
	OSVERSIONINFO of;
	of.dwOSVersionInfoSize = sizeof(of);
	GetVersionEx(&of);
	return of.dwMajorVersion >= 6 && of.dwMinorVersion >= 1;
}
#endif

Txt AsTxt(const wchar_t *buffer) {
	if(!buffer)
		return Null;
	return AsTxt(buffer, (i32)wcslen(buffer));
}

Txt AsTxt(const wchar_t *buffer, i32 count) { // Convert with code page...
	if(!buffer)
		return Null;
	TxtBuf temp(count);
	for(tuk p = temp, *e = p + count; p < e;)
		*p++ = (char)*buffer++;
	return Txt(temp);
}

Txt AsTxt(const wchar_t *buffer, const wchar_t *end) {
	if(!buffer)
		return Null;
	return AsTxt(buffer, (i32)(end - buffer));
}

Txt GetWinRegTxt(tukk value, tukk path, HKEY base_key, dword wow) {
	HKEY key = 0;
	if(RegOpenKeyEx(base_key, path, 0, KEY_READ|wow, &key) != ERROR_SUCCESS)
		return Txt::GetVoid();
	dword type, len;
	if(RegQueryValEx(key, value, 0, &type, NULL, &len) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return Txt::GetVoid();
	}
	TxtBuf raw_len(len);
	if(RegQueryValEx(key, value, 0, 0, (u8 *)~raw_len, &len) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return Txt::GetVoid();
	}
	if(len > 0 && (type == REG_SZ || type == REG_EXPAND_SZ))
		len--;
	raw_len.SetLength(len);
	RegCloseKey(key);
	return Txt(raw_len);
}

i32 GetWinRegInt(tukk value, tukk path, HKEY base_key, dword wow) {
	HKEY key = 0;
	if(RegOpenKeyEx(base_key, path, 0, KEY_READ|wow, &key) != ERROR_SUCCESS)
		return Null;
	i32 data;
	dword type, length = sizeof(data);
	if(RegQueryValEx(key, value, 0, &type, (u8 *)&data, &length) != ERROR_SUCCESS)
		data = Null;
	RegCloseKey(key);
	return data;
}

bool SetWinRegTxt(const Txt& string, tukk value, tukk path, HKEY base_key, dword wow) {
	HKEY key = 0;
	if(RegCreateKeyEx(base_key, path, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS|wow, NULL, &key, NULL) != ERROR_SUCCESS)
		return false;
	bool ok = (RegSetValEx(key, value, 0,	REG_SZ, string, string.GetLength() + 1) == ERROR_SUCCESS);
	RegCloseKey(key);
	return ok;
}

bool SetWinRegExpandTxt(const Txt& string, tukk value, tukk path, HKEY base_key, dword wow) {
	HKEY key = 0;
	if(RegCreateKeyEx(base_key, path, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS|wow, NULL, &key, NULL) != ERROR_SUCCESS)
		return false;
	bool ok = (RegSetValEx(key, value, 0,	REG_EXPAND_SZ, string, string.GetLength() + 1) == ERROR_SUCCESS);
	RegCloseKey(key);
	return ok;
}

bool SetWinRegInt(i32 data, tukk value, tukk path, HKEY base_key, dword wow)
{
	HKEY key = 0;
	if(RegCreateKeyEx(base_key, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS|wow, NULL, &key, NULL) != ERROR_SUCCESS)
		return false;
	bool ok = (RegSetValEx(key, value, 0, REG_DWORD, (u8k *)&data, sizeof(data)) == ERROR_SUCCESS);
	RegCloseKey(key);
	return ok;
}

void DeleteWinReg(const Txt& key, HKEY base, dword wow) {
	HKEY hkey;
	if(RegOpenKeyEx(base, key, 0, KEY_READ|wow, &hkey) != ERROR_SUCCESS)
		return;
	Vec<Txt> subkeys;
	char temp[_MAX_PATH];
	dword len;
	for(dword dw = 0; len = sizeof(temp), RegEnumKeyEx(hkey, dw, temp, &len, 0, 0, 0, 0) == ERROR_SUCCESS; dw++)
		subkeys.Add(temp);
	RegCloseKey(hkey);
	while(!subkeys.IsEmpty())
		DeleteWinReg(key + '\\' + subkeys.Pop(), base);

	static LONG (WINAPI *RegDeleteKeyEx)(HKEY, LPCTSTR, REGSAM, DWORD);
	DllFn(RegDeleteKeyEx, "Advapi32.dll", "RegDeleteKeyExA");

	if(wow && RegDeleteKeyEx)
		RegDeleteKeyEx(base, key, wow, 0);
	else
		RegDeleteKey(base, key);
}

Txt GetSystemDir() {
	WCHAR temp[MAX_PATH];
	*temp = 0;
	::GetSystemDirW(temp, sizeof(temp));
	return FromSystemCharsetW(temp);
}

Txt GetWindowsDir() {
	WCHAR temp[MAX_PATH];
	*temp = 0;
	GetWindowsDirW(temp, sizeof(temp));
	return FromSystemCharsetW(temp);
}

uk GetDllFn(tukk dll, tukk fn)
{
	if(HMODULE hDLL = LoadLibrary(dll))
		return (uk )GetProcAddress(hDLL, fn);
	return NULL;
}

Txt GetModuleFName(HINSTANCE instance) {
	WCHAR h[_MAX_PATH];
	GetModuleFNameW(instance, h, _MAX_PATH);
	return FromSystemCharsetW(h);
}

#ifdef DEPRECATED
bool SyncObject::Wait(i32 ms)
{
	return WaitForSingleObject(handle, ms);
}

bool SyncObject::Wait()
{
	return Wait(INFINITE);
}

SyncObject::SyncObject()
{
	handle = NULL;
}

SyncObject::~SyncObject()
{
	if(handle) CloseHandle(handle);
}

Win32Event::Win32Event()
{
	handle = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void Win32Event::Set()
{
	SetEvent(handle);
}
#endif

#endif

}