#include "CtrlLib.h"

namespace drx {

#ifdef GUI_WIN
#ifndef TARGET_WINCE

#define LLOG(x)

static Txt s_updatedir;
static Txt s_updater;

void UpdateSetDir(tukk path)
{
	s_updatedir = path;
}

void UpdateSetUpdater(tukk exename)
{
	s_updater = exename;
}

Txt UpdateGetDir()
{
	return Nvl(s_updatedir, GetIniKey("UPDATE"));
}

Time FileTimeToTime(const FILETIME& time) {
	SYSTEMTIME t;
	FileTimeToSystemTime(&time, &t);
	return Time(t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
}

bool NoUpdate(tukk dsf, tukk srf, i32& len) {
	FindFile dst(dsf);
	FindFile src(srf);
	if(!src) return true;
	len = (i32)src.GetLength();
	if(!dst) return false;
	return abs(FileTimeToTime(dst.GetLastWriteTime()) - FileTimeToTime(src.GetLastWriteTime()))
		    < 100 && len == dst.GetLength();
}

void UpdateFile(Txt dst, Txt src)
{
	if(src.IsEmpty()) return;
	Txt filename = GetFName(dst);
	i32 len;
	if(NoUpdate(dst, src, len)) return;
	Txt tmp = dst + ".tmp";
	Progress pi;
	pi.Title((t_("Обновление ") + filename).ToWTxt());
	pi.SetText(t_("Обновляется ") + filename);
//	pi.Open();
	FileIn in(src);
	if(!in)
	{
		Exclamation(Format(t_("Ошибка при открытии файла [* \1%s\1]."), src));
		return;
	}
	FileDelete(tmp);
	FileOut out(tmp);
	dword n = 0;
	char buffer[2048];
	if(!out)
	{
		Exclamation(Format(t_("Ошибка при создании файла [* \1%s\1]."), tmp));
		return;
	}
	for(;;)
	{
		dword i = in.Get(buffer, 2048);
		if(i == 0) break;
		out.Put(buffer, i);
		n += i;
		pi.Set(n, len);
		if(pi.Canceled() && PromptYesNo(t_("Прерывание обновления возможно вызовет проблемы при следующем запустке приложения.\nПродолжить?")))
		{
			out.Close();
			FileDelete(tmp);
			return;
		}
	}
	out.SetTime(in.GetTime());
	out.Close();
	if(out.IsError())
	{
		Exclamation(Format(t_("Ошибка при записи файла [* \1%s\1]."), tmp));
		FileDelete(tmp);
		return;
	}
	Txt old = dst + ".old";
	pi.SetText(Format(t_("Переписывается %s"), dst));
	pi.SetTotal(10000);
	i32 start = msecs();
	for(;;)
	{
		SetFileAttributes(dst, 0);
		FileDelete(old);
		FileMove(dst, old);
		if(FileMove(tmp, dst))
			return;
		if(pi.SetPosCanceled(msecs(start) % 10000) && PromptYesNo(t_("Прерывание обновления возможно вызовет проблемы при следующем запустке приложения.\nПродолжить?")))
			return;
		Sleep(500);
	}
}

void UpdateFile(tukk filename)
{
	Txt dst = GetExeDirFile(filename);
	Txt src = UpdateGetDir();
	if(IsNull(src))
		return;
	UpdateFile(dst, AppendFName(src, filename));
}

static Txt FixArg(Txt s)
{
	if(s.Find(' ') < 0 && s.Find('\"') < 0)
		return s;
	Txt ucmd;
	ucmd << '\"';
	for(tukk p = s; *p; p++)
		if(*p == '\"')
			ucmd << "\"\"";
		else
			ucmd << *p;
	ucmd << '\"';
	return ucmd;
}

void SelfUpdate() {
	char dst[512];
	UpdateFile(Nvl(s_updater, Txt("UPDATER.EXE")));
	::GetModuleFName(NULL, dst, 512);
	{
		Txt src = UpdateGetDir();
		if(src.IsEmpty()) return;
		src = AppendFName(src, GetFNamePos(dst));
		i32 dummy;
		if(NoUpdate(dst, src, dummy)) return;
		Txt commandline;
		const Vec<Txt>& cmd = CommandLine();
		for(i32 i = 0; i < cmd.GetCount(); i++) {
			commandline += ' ';
			commandline += FixArg(cmd[i]);
		}
		if(WinExec(GetExeDirFile("updater.exe") + " " + GetFName(dst) + commandline, SW_SHOWNORMAL) <= 31)
			return;
	}
	exit(0);
}

bool SelfUpdateSelf()
{
	const Vec<Txt>& cmdline = CommandLine();
	Txt exe = GetExeFilePath();

	if(cmdline.GetCount() >= 2 && cmdline[0] == "-update")
	{ // updater
		Txt exec = FixArg(cmdline[1]);
		UpdateFile(exec, exe);
		for(i32 i = 2; i < cmdline.GetCount(); i++)
			exec << ' ' << FixArg(cmdline[i]);
		WinExec(exec, SW_SHOWNORMAL);
		return true;
	}

	Txt src = UpdateGetDir();
	if(src.IsEmpty()) return false;
	src = AppendFName(src, GetFNamePos(exe));
	i32 dummy;
	if(NoUpdate(exe, src, dummy)) return false;

	Txt commandline;
	commandline << src << " -update " << FixArg(exe);
	for(i32 i = 0; i < cmdline.GetCount(); i++)
		commandline << ' ' << FixArg(cmdline[i]);
	LLOG("SelfUpdateSelf (running updater): " << commandline);
	return WinExec(commandline, SW_SHOWNORMAL) >= 32;
}

#endif
#endif

}