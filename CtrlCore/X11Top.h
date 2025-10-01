protected:
	virtual void    EventProc(XWindow& w, XEvent *event);
private:
	XSizeHints *size_hints;
	XWMHints   *wm_hints;
	XClassHint *class_hint;
	Size        xminsize, xmaxsize;
	bool        topmost;
	
	Buffer<u64>  PreperIcon(const Image& icon, i32& len);
	
	void                   CenterRect(Ctrl *owner);
	void                   DefSyncTitle();
	void                   EndIgnoreTakeFocus();

	void                   SyncState();

	Image                  invert;
	WTxt                title2;

	static  Rect           windowFrameMargin;

