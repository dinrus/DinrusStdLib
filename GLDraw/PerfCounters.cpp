#include "GLDraw.h"

namespace drx {

i32 sDrawCounter;
i32 sElementCounter;
i32 sTextureCounter;
i32 sProgramCounter;
i32 sTesselateCounter;

void GLClearCounters()
{
	sElementCounter = sTextureCounter = sProgramCounter = sDrawCounter = sTesselateCounter = 0;
}

i32 GLElementCounter()
{
	return sElementCounter;
}

i32 GLTextureCounter()
{
	return sTextureCounter;
}

i32 GLProgramCounter()
{
	return sProgramCounter;
}

i32 GLDrawCounter()
{
	return sDrawCounter;
}

i32 GLTesselateCounter()
{
	return sTesselateCounter;
}

};