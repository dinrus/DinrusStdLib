struct BoundsPainter : public NilPainter {
	virtual void   MoveOp(const Pointf& p, bool rel);
	virtual void   LineOp(const Pointf& p, bool rel);
	virtual void   QuadraticOp(const Pointf& p1, const Pointf& p, bool rel);
	virtual void   QuadraticOp(const Pointf& p, bool rel);
	virtual void   CubicOp(const Pointf& p1, const Pointf& p2, const Pointf& p, bool rel);
	virtual void   CubicOp(const Pointf& p2, const Pointf& p, bool rel);
	virtual void   ArcOp(const Pointf& c, const Pointf& r, double angle, double sweep, bool rel);
	virtual void   SvgArcOp(const Pointf& r, double xangle, bool large, bool sweep,
	                        const Pointf& p, bool rel);
	virtual void   CloseOp();
	virtual void   DivOp();
	
	virtual void   ClipOp();

	virtual void   TextOp(const Pointf& p, wtukk text, Font fnt, i32 n = -1,
	                      const double *dx = NULL);
	virtual void   CharacterOp(const Pointf& p, i32 ch, Font fnt);

	virtual void   TransformOp(const Xform2D& m);
	virtual void   BeginOp();
	virtual void   EndOp();

	
	void Finish(double width = 0);

	Painter& sw;
	Rectf    boundingbox;
	Pointf   current, qcontrol, ccontrol;

	Array<Xform2D> mtx;
	Rectf      svg_boundingbox;
	NilPainter nil;

	void  Bounds(Pointf p);

	void  Quadratic(const Pointf& p1, const Pointf& p);
	void  Cubic(const Pointf& p1, const Pointf& p2, const Pointf& p);

	void  New();
	const Rectf& Get() { return boundingbox; }
	
	bool  compute_svg_boundingbox;

	BoundsPainter(Painter& sw) : sw(sw) { New(); mtx.Add(); svg_boundingbox = Null; compute_svg_boundingbox = false; }
};

struct SvgParser {
	Painter& sw;

	void ParseSVG(tukk svg, tukk folder);

	struct Stop : Moveable<Stop> {
		RGBA   color;
		double offset;
	};
	
	struct Gradient {
		bool   resolved;
		bool   radial;
		Pointf a, b, c, f;
		double r;
		i32    style;
		bool   user_space; // СДЕЛАТЬ: Cascade!
		Txt transform;

		Vec<Stop> stop;
		
		Txt href;
	};
	
	ArrayMap<Txt, Gradient> gradient;
	
	struct State {
		double opacity;
	
		i32    fill_gradient;
		Color  fill;
		double fill_opacity;
		
		i32    stroke_gradient;
		Color  stroke;
		double stroke_width;
		double stroke_opacity;
		
		Txt    dash_array;
		double dash_offset;
		
		i32    text_anchor; // 0 left, 1 middle, 2 right
		
		Font   font;
		
		const XmlNode *n;
	};
	
	const XmlNode *current = NULL;
	Array<State>   state;
	bool           closed;
	Pointf         prev;
	Xform2D        lastTransform;
	BoundsPainter  bp;
	VecMap<Txt, const XmlNode*> idmap;
	VecMap<Txt, Txt> classes;

	void Reset();

	static Color GetTextColor(const Txt& color);
	static Color GetColor(const Txt& text);
	
	void    ProcessVal(const Txt& key, const Txt& value);
	void    Style(tukk style);
	Xform2D Transform(tukk transform);
	
	//Внимание: Функция ниже вынужденно переименована из Txt в AsTxt!
	Txt    AsTxt(tukk id)                { return current ? current->Attr(id) : Txt(); }
	double Dbl(tukk id, double def = 0)  { return Nvl(StrDbl(AsTxt(id)), def); }
	
	void StartElement(const XmlNode& n);
	void EndElement();
	void StrokeFinishElement();
	void FinishElement();
	void DoGradient(i32 gi, bool stroke);
	void Poly(const XmlNode& n, bool line);
	void Items(const XmlNode& n, i32 depth);
	void Element(const XmlNode& n, i32 depth, bool dosymbols = false);
	void ParseGradient(const XmlNode& n, bool radial);
	void ResolveGradient(i32 i);
	void MapIds(const XmlNode& n);

	bool   ReadBool(CParser& p);
	double ReadDouble(CParser& p);
	Pointf ReadPoint0(CParser& p, bool rel);
	Pointf ReadPoint(CParser& p, bool rel);
	void   Path(tukk s);
	
	bool Parse(tukk xml);

	Event<Txt, Txt&> resloader;
	
	SvgParser(Painter& sw);
};
