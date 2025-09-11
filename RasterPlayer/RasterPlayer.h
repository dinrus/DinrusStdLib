#ifndef _RasterPlayer_RasterPlayer_h_
#define _RasterPlayer_RasterPlayer_h_

#include <drx/CtrlLib/CtrlLib.h>

namespace drx {

class RasterPlayer : public Ctrl {
private:
    virtual void Paint(Draw& w);
    bool IsKilled();

    drx::Array<Image> images;
    drx::Array<i32> delays;
    i32 ind;
    Color background;
    double speed;
    bool mt;

    TimeStop tTime;
    double tFrame_ms;

public:
    RasterPlayer();
    virtual ~RasterPlayer() noexcept;

    bool Load(const Txt &fileName);
    bool LoadBuffer(const Txt &buffer);

    void Play();
    void Stop();
    bool IsRunning()    {return running;}
    void NextFrame();
    inline void NextPage() {NextFrame();};
    RasterPlayer& SetBackground(Color c)    {background = c; Refresh(); return *this;}
    RasterPlayer& SetSpeed(double s = 1)    {speed = s; Refresh(); return *this;}
    RasterPlayer& SetMT(bool _mt = false);

    Event<> WhenShown;

    i32 GetPageCount()  {return images.GetCount();};
    i32 GetFrameCount() {return images.GetCount();};
    i32 GetPage()       {return ind;};
    void SetPage(i32 i) {ind = minmax(i, 0, images.GetCount());};

#ifdef _MULTITHREADED
    friend void RasterPlayerThread(RasterPlayer *animatedClip);
#endif
    void TimerFun();

protected:
    volatile Atomic running, kill;
};

}

#endif
