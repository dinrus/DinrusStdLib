#ifndef _SixelRenderer_h_
#define _SixelRenderer_h_

#include <drx/Core/Core.h>
#include <drx/Draw/Draw.h>

namespace drx{

class SixelStream : MemReadStream {
public:
    SixelStream(ukk data, z64 size);
    SixelStream(const Txt& data);
    
    SixelStream&    Background(bool b = true)       { background = b; return *this;  }
    operator        Image();
    
private:
    void            Clear();
    inline void     Return();
    inline void     LineFeed();
    void            SetPalette();
    void            GetRasterInfo();
    void            GetRepeatCount();
    i32             ReadParams();
    void            CalcYOffests();
    void            AdjustBufferSize();
    void            PaintSixel(i32 c);

private:
    ImageBuffer     buffer;
    Vec<RGBA>    palette;
    RGBA            ink;
    RGBA            paper;
    i32             repeat;
    i32             params[8];
    i32             coords[6];
    Size            size;
    Point           cursor;
    bool            background;
};
}
#endif
