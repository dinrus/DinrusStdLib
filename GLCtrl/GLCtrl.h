#ifndef _GLCtrl_GLCtrl_h
#define _GLCtrl_GLCtrl_h

#include <drx/CtrlCore/CtrlCore.h>

#if defined(GUI_X11) || defined(GUI_GTK)
#define Time    XTime
#define Font    XFont
#define Display XDisplay
#define Picture XPicture
#define Status  i32
#endif

#define GLEW_STATIC

#include <X/glew/glew.h>

#ifdef TARGET_WIN32
#include <X/glew/wglew.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#if defined(GUI_X11) || defined(GUI_GTK)

#include <GL/glx.h>

#undef  Status
#undef  Picture
#undef  Time
#undef  Font
#undef  Display
#endif

namespace drx {

void InitializeGlew();


class GLCtrl : public Ctrl {
	typedef GLCtrl CLASSNAME;

public:
	Image  MouseEvent(i32 event, Point p, i32 zdelta, dword keyflags) override;
#ifdef TARGET_POSIX
	void   Paint(Draw& w) override;
#endif
	
private:
#ifdef TARGET_WIN32
	struct GLPane : DHCtrl {
		friend class GLCtrl;
		
		GLCtrl *ctrl;

		void DoGLPaint();
		
	public:
		GLPane() { NoWantFocus(); }
		
		virtual void    State(i32 reason);
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		virtual Image   MouseEvent(i32 event, Point p, i32 zdelta, dword keyflags);
		virtual void    Pen(Point p, const PenInfo& pen, dword keyflags);
		
		void Init();
		void Destroy();
		
		void ActivateContext();

		void ExecuteGL(HDC hdc, Event<> paint, bool swap_buffers);
		void ExecuteGL(Event<> paint, bool swap_buffers);
	};
#endif

	friend struct GLPane;

#ifdef TARGET_POSIX // we assume X11 or GTK
	u64 win = 0;
	bool visible;
	Rect position;

	void Create();
	void Sync();
	void Destroy();
	
	void State(i32 reason) override;
#else
	GLPane pane;
#endif

	static i32  depthSize;
	static i32  stencilSize;
	static bool doubleBuffering;
	static i32  numberOfSamples;

	static void MakeGLContext();
	void        DoGLPaint();

	static Size current_viewport; // because we need to set different viewports in drawing code

	Ptr<Ctrl> mouseTarget = NULL;

protected:
	// Called on paint events
	virtual void GLPaint() { WhenGLPaint(); }
	
	void Init();

public:
	Callback WhenGLPaint;

	static void SetDepthBits(i32 n)               { depthSize = n; }
	static void SetStencilBits(i32 n)             { stencilSize = n; }
	static void SetDoubleBuffering(bool b = true) { doubleBuffering = b; }
	static void SetMSAA(i32 n = 4)                { numberOfSamples = n; }
	
	static void CreateContext();
	
	static Size CurrentViewport()                 { return current_viewport; }
	static void SetCurrentViewport(); // intended to restore viewport after changing it in e.g. TextureDraw
	
	GLCtrl& RedirectMouse(Ctrl *target)           { mouseTarget = target; return *this; }
	
	void ExecuteGL(Event<> gl, bool swap_buffers = false);

	GLCtrl()                                      { Init(); }

#ifdef TARGET_WIN32
	void Refresh()                                { pane.Refresh(); }
#endif

	// deprecated (these settings are now static, as we have just single context)
	GLCtrl& DepthBits(i32 n)               { depthSize = n; return *this; }
	GLCtrl& StencilBits(i32 n)             { stencilSize = n; return *this; }
	GLCtrl& DoubleBuffering(bool b = true) { doubleBuffering = b; return *this; }
	GLCtrl& MSAA(i32 n = 4)                { numberOfSamples = n; return *this; }

	GLCtrl(i32  depthsize, i32  stencilsize = 8, bool doublebuffer = true,
	       bool multisamplebuffering = false, i32  numberofsamples = 0)
	{ Init(); DepthBits(depthsize).StencilBits(stencilsize).DoubleBuffering(doublebuffer).MSAA(numberofsamples); }

	// deprecated (fixed pipeline is so out of fashion...)
	void StdView();
};

}

#endif
