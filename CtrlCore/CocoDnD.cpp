#include <drx/CtrlCore/CtrlCore.h>

#ifdef TARGET_COCOA

NAMESPACE_UPP

#define LLOG(x) // LOG(x)

// --------------------------------------------------------------------------------------------

Image MakeDragImage(const Image& arrow, Image sample);

Image MakeDragImage(const Image& arrow, const Image& arrow98, Image sample)
{
	return MakeDragImage(arrow, sample);
}

END_DRX_NAMESPACE

#endif
	