#ifndef _SDL2GUI_SDL2GUI_h
#define _SDL2GUI_SDL2GUI_h

#include <drx/CtrlLib/CtrlLib.h>
#include <drx/GLDraw/GLDraw.h>
//#include <drx/VirtualGui/VirtualGui.h>

#ifdef TARGET_POSIX
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

namespace drx {

struct SDL2GUI : VirtualGui {
	virtual dword       GetOptions();
	virtual Size        GetSize();
	virtual dword       GetMouseButtons();
	virtual dword       GetModKeys();
	virtual bool        IsMouseIn();
	virtual bool        ProcessEvent(bool *quit);
	virtual void        WaitEvent(i32 ms);
	virtual bool        IsWaitingEvent();
	virtual void        WakeUpGuiThread();
	virtual void        SetMouseCursor(const Image& image);
	virtual SystemDraw& BeginDraw();
	virtual void        CommitDraw();

	virtual void        Quit();
	virtual void        HandleSDLEvent(SDL_Event* event);

	SDL_Window   *win;
	SDL_GLContext glcontext;
	z64         serial;
	GLDraw        gldraw;
	SystemDraw    sysdraw;

	void Attach(SDL_Window *win, SDL_GLContext glcontext);
	void Detach();

	bool Create(const Rect& rect, tukk title);
	void Destroy();

	SDL2GUI();
	~SDL2GUI();
};

};

#endif