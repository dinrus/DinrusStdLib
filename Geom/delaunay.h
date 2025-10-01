class Delaunay
{
public:
	Delaunay() : tihull(-1) {}
	Delaunay(const Array<Pointf>& points, double epsilon = 1e-6) { Build(points, epsilon); }

	void                          Build(const Array<Pointf>& points, double epsilon = 1e-6);

public:
	struct Triangle // anticlockwise
	{
		bool IsProper() const                       { return vertex[0] >= 0; }
		i32  operator [] (i32 i) const              { return vertex[i]; }
		i32& operator [] (i32 i)                    { return vertex[i]; }

		i32  Next(i32 i) const                      { return nextindex[i] >> 2; }
		i32  NextIndex(i32 i) const                 { return nextindex[i] & 3; }

		void Set(i32 a, i32 b, i32 c)               { vertex[0] = a; vertex[1] = b; vertex[2] = c; }
		void SetNext(i32 i, i32 next, i32 index)    { ASSERT(index >= 0 && index <= 2); nextindex[i] = next * 4 + index; }

		i32  vertex[3];    // [0] = -1 => infinity
		i32  nextindex[3]; // neighbour[i] is opposite to vertex[i]; bit 0 & 1 = my index in neighbour's neighbour list
	};

	i32                           GetCount() const                             { return triangles.GetCount(); }
	const Triangle&               operator [] (i32 i) const                    { return triangles[i]; }
	Pointf                        At(i32 i) const                              { return points[i]; }
	Pointf                        At(const Triangle& t, i32 i) const           { return points[t[i]]; }
	i32                           GetHullIndex() const                         { return tihull; }

private:
	bool                          IsNear(const Pointf& a, const Pointf& b) const { return fabs(a.x - b.x) <= epsilon && fabs(a.y - b.y) <= epsilon; }
	void                          CreatePair(i32 i, i32 j);
	void                          AddHull(i32 i);
	void                          Link(i32 ta, i32 ia, i32 tb, i32 ib);

private:
	Array<Pointf>                 points;
	Vec<i32>                   order;
	Array<Triangle>               triangles;
	double                        epsilon;
	double                        epsilon2;
	i32                           tihull;
};
