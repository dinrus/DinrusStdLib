private:
	friend struct MMCtrl;
	friend struct MMImp;

	static bool                local_dnd_copy;
	static Ptr<Ctrl>           lastActive;
	
	friend void CocoInit(i32 argc, tukk *argv, tukk *envptr);
	
protected:
	virtual void MMClose() {}

	void   Create(Ctrl *owner, dword style, bool active);
	static void SetNSAppImage(const Image& img);
	static void SyncAppIcon();
	static void ResetCocoaMouse();
	static void DoCancelPreedit();
	static i32  GetCaretBlinkTime()               { return 500; }

public:
	static void      EndSession()              {}
	static bool      IsEndSession()            { return false; }
	
	uk GetNSWindow() const;
	uk GetNSView() const;
	bool   IsCocoActive() const;
	
	void   RegisterCocoaDropFormats();

	static Rect GetScreenArea(Point pt);
