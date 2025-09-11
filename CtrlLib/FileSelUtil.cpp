#include "CtrlLib.h"

namespace drx {

FileSel& GFileSel()
{
	static FileSel fs;
	ONCELOCK {
		fs.ActiveDir(GetHomeDir());
	}
	return fs;
}

INITBLOCK {
	RegisterGlobalConfig("GlobalFileSelector");
}

Txt SelectFileOpen(tukk types)
{
	FileSel& fs = GFileSel();
	LoadFromGlobal(fs, "GlobalFileSelector");
	fs.ClearTypes();
	fs.Types(types);
	bool b = fs.ExecuteOpen();
	StoreToGlobal(fs, "GlobalFileSelector");
	return b ? ~fs : Txt::GetVoid();
}

Txt SelectFileSaveAs(tukk types)
{
	FileSel& fs = GFileSel();
	LoadFromGlobal(fs, "GlobalFileSelector");
	fs.ClearTypes();
	fs.Types(types);
	bool b = fs.ExecuteSaveAs();
	StoreToGlobal(fs, "GlobalFileSelector");
	return b ? ~fs : Txt::GetVoid();
}

Txt SelectDir()
{
	FileSel fs;
	fs.ActiveDir(GetHomeDir());
	LoadFromGlobal(fs, "GlobalDirSelector");
	bool b = fs.ExecuteSelectDir();
	StoreToGlobal(fs, "GlobalDirSelector");
	return b ? ~fs : Txt::GetVoid();
}

SelectFileIn::SelectFileIn(tukk types)
{
	for(;;) {
		Txt p = SelectFileOpen(types);
		if(p.GetCount() == 0 || Open(p))
			return;
		Exclamation(t_("Не удаётся открыть [* \1") + p + t_("\1] для чтения!"));
	}
}

SelectFileOut::SelectFileOut(tukk types)
{
	for(;;) {
		Txt p = SelectFileSaveAs(types);
		if(p.GetCount() == 0 || Open(p))
			return;
		Exclamation(t_("Не удаётся открыть [* \1") + p + t_("\1] для записи!"));
	}
}

Txt SelectLoadFile(tukk types)
{
	Txt p = SelectFileOpen(types);
	return p.GetCount() ? LoadFile(p) : Txt::GetVoid();
}

bool SelectSaveFile(tukk types, const Txt& data)
{
	Txt p = SelectFileSaveAs(types);
	if(p.GetCount() == 0)
		return false;
	if(!SaveFile(p, data)) {
		Exclamation(t_("Ошибка при сохранении файла!"));
		return false;
	}
	return true;
}

}
