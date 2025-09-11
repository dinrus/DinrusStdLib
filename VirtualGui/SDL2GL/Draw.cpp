#include "SDL2GL.h"

namespace drx {

SystemDraw& SDL2GUI::BeginDraw()
{
	gldraw.Init(GetSize(), (zu64)glcontext);
	sysdraw.SetTarget(&gldraw);
	return sysdraw;
}

void SDL2GUI::CommitDraw()
{
	gldraw.Finish();
	SDL_GL_SwapWindow(win);
}

};