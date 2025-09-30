#include <drx/CtrlCore/CtrlCore.h>

#ifdef GUI_COCOA

NAMESPACE_UPP

#define LLOG(x)   //   LOG(x)
#define LOGTIMING 0

#ifdef _DEBUG
#define LOGMESSAGES 0
#endif

#define ELOGW(x)  // RLOG(GetSysTime() << ": " << x) // Only activate in MT!
#define ELOG(x)   // RLOG(GetSysTime() << ": " << x)

bool Ctrl::IsAlphaSupported()
{
	return false;
}

// Vec<Callback> Ctrl::hotkey;

i32 Ctrl::RegisterSystemHotKey(dword key, Function<void ()> cb)
{
/*	ASSERT(key >= K_DELTA);
	i32 q = hotkey.GetCount();
	for(i32 i = 0; i < hotkey.GetCount(); i++)
		if(!hotkey[i]) {
			q = i;
			break;
		}
	hotkey.At(q) = cb;
	dword mod = 0;
	if(key & K_ALT)
		mod |= MOD_ALT;
	if(key & K_SHIFT)
		mod |= MOD_SHIFT;
	if(key & K_CTRL)
		mod |= MOD_CONTROL;
	
	return RegisterHotKey(NULL, q, mod, key & 0xffff) ? q : -1;*/
	return -1;
}

void Ctrl::UnregisterSystemHotKey(i32 id)
{
/*	if(id >= 0 && id < hotkey.GetCount()) {
		UnregisterHotKey(NULL, id);
		hotkey[id].Clear();
	}*/
}

void Ctrl::WndShow(bool b)
{
	GuiLock __;
}

void Ctrl::WndUpdate()
{
	GuiLock __;
}

void Ctrl::SetAlpha(u8 alpha)
{
	GuiLock __;
}

void Ctrl::WndUpdate(const Rect& r)
{
	GuiLock __;

}

Vec<WTxt> SplitCmdLine__(tukk cmd)
{
	Vec<WTxt> out;
	while(*cmd)
		if((u8)*cmd <= ' ')
			cmd++;
		else if(*cmd == '\"') {
			WTxt quoted;
			while(*++cmd && (*cmd != '\"' || *++cmd == '\"'))
				quoted.Cat(FromSystemCharset(Txt(cmd, 1)).ToWTxt());
			out.Add(quoted);
		}
		else {
			tukk begin = cmd;
			while((u8)*cmd > ' ')
				cmd++;
			out.Add(Txt(begin, cmd).ToWTxt());
		}
	return out;
}

void Ctrl::SysEndLoop()
{
}

END_DRX_NAMESPACE

#endif
