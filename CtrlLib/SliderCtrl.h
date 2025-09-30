class SliderCtrl : public Ctrl {
	i32           value;
	i32           min, max, step;
	bool          round_step;
	bool		  jump;

	i32           SliderToClient(i32 value) const;
	i32           ClientToSlider(i32 x) const;

	i32           HoVe(i32  x, i32  y) const;
	i32&          HoVeR(i32& x, i32& y) const;

	i32           Min() const     { return drx::min(min, max); };
	i32           Max() const     { return drx::max(min, max); };

public:
	typedef SliderCtrl CLASSNAME;

	Event<>       WhenSlideFinish;
	
	SliderCtrl();
	virtual ~SliderCtrl();

	virtual void  Paint(Draw& draw);
	virtual bool  Key(dword key, i32 repcnt);
	virtual void  LeftDown(Point pos, dword keyflags);
	virtual void  LeftRepeat(Point pos, dword keyflags);
	virtual void  LeftUp(Point pos, dword keyflags);
	virtual void  MouseMove(Point pos, dword keyflags);
	virtual void  GotFocus();
	virtual void  LostFocus();

	virtual void  SetData(const Val& value);
	virtual Val GetData() const;

	void          Inc();
	void          Dec();

	SliderCtrl&   MinMax(i32 _min, i32 _max);
	SliderCtrl&   Range(i32 max)                  { return MinMax(0, max); }
	i32           GetMin() const                  { return min; }
	i32           GetMax() const                  { return max; }

	bool          IsVert() const;
	SliderCtrl&   Jump(bool v = true)			  { jump = v; return *this; }

	SliderCtrl&   Step(i32 _step, bool _r = true) { step = _step; round_step = _r; return *this; }
	i32           GetStep() const                 { return step; }
	bool          IsRoundStep() const             { return round_step; }
};
