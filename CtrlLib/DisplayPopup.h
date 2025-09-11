class DisplayPopup : public Pte<DisplayPopup> {
private:
	struct Pop : Pte<Pop> {
		struct PopCtrl : public Ctrl {
			virtual void  Paint(Draw& w);
			
			struct Pop *p;
		};
		
		Ptr<Ctrl>      ctrl;
		Rect           item;
	
		Val          value;
		Color          paper, ink;
		dword          style;
		const Display *display;
		i32            margin;
		bool           usedisplaystdsize = false;
	
		PopCtrl   view;
		PopCtrl   frame;
	
		Callback WhenClose;
	
		void Set(Ctrl *ctrl, const Rect& item, const Val& v, const Display *display,
		         Color ink, Color paper, dword style, i32 margin = 0);
		void Sync();

		static Vec<Pop *>& all();
	
		Pop();
		~Pop();
	};
	
	One<Pop>     popup;
	bool         usedisplaystdsize = false;

	static bool StateHook(Ctrl *, i32 reason);
	static bool MouseHook(Ctrl *, bool, i32, Point, i32, dword);
	static void SyncAll();
	
	typedef DisplayPopup CLASSNAME;

public:
	void Set(Ctrl *ctrl, const Rect& item, const Val& v, const Display *display,
	         Color ink, Color paper, dword style, i32 margin = 0);
	void Cancel();
	bool IsOpen();
	bool HasMouse();
	void UseDisplayStdSize();

	DisplayPopup();
	~DisplayPopup();
};
