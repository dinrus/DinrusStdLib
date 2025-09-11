#include <drx/CtrlLib/CtrlLib.h>

namespace drx {

LineEdit *dlog = NULL;
i32 dlev = 0;

static i32 pos = 0;

void LogCon(tukk fmt, ...)
{
	char buffer[1024];
	va_list argptr;
	va_start(argptr, fmt);
	vsprintf(buffer, fmt, argptr);
	va_end(argptr);
	LOG(buffer);
}

void LogCon(i32 level, tukk fmt, ...)
{
	char buffer[1024];
	va_list argptr;
	va_start(argptr, fmt);
	vsprintf(buffer, fmt, argptr);
	va_end(argptr);
	LOG(buffer);
}

void LogGui(tukk fmt, ...)
{
	if(!dlog)
		return;

	pos = dlog->GetLength();
	char buffer[1024];
	va_list argptr;
	va_start(argptr, fmt);
	i32 l = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	dlog->Insert(pos, buffer);
	pos += l;
	dlog->Insert(pos, "\n");
	pos++;
	dlog->SetCursor(pos);
}

void LogGui(i32 level, tukk fmt, ...)
{
	if(!dlog || level < dlev)
		return;

	pos = dlog->GetLength();
	char buffer[1024];
	va_list argptr;
	va_start(argptr, fmt);
	i32 l = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	dlog->Insert(pos, buffer);
	pos += l;
	dlog->Insert(pos, "\n");
	pos++;
	dlog->SetCursor(pos);
}

}
