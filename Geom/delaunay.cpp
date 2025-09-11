#include "Geom.h"

namespace drx {

#define LLOG(x) // DLOG(x)

inline
void Delaunay::Link(i32 ta, i32 ia, i32 tb, i32 ib)
{
	LLOG("Link " << ta << "[" << ia << "] to " << tb << "[" << ib << "]"); \
	triangles[ta].SetNext(ia, tb, ib);
	triangles[tb].SetNext(ib, ta, ia);
}

void Delaunay::Build(const Array<Pointf>& p, double e)
{
	epsilon = e;
	epsilon2 = e * e;
	points <<= p;
	order = GetSortOrder(points, [](const Pointf& a, const Pointf& b) { return a.x < b.x || (a.x == b.x && a.y < b.y); });
	tihull = -1;
	i32 npoints = p.GetCount();

	LLOG("Delaunay(" << npoints << " points): " << points);

	triangles.Clear();
	if(order.IsEmpty())
		return;
	const Pointf *p0 = &points[order[0]];
	i32 xi = 0;
	do
		if(++xi >= points.GetCount())
			return;
	while(IsNear(*p0, points[order[xi]]));

	// pass 1: create pair of improper triangles
	CreatePair(order[0], order[xi]);
	while(++xi < npoints)
		AddHull(order[xi]);
	
#if 0
	RLOG("//Delaunay: " << triangles.GetCount() << " triangles");
	for(i32 td = 0; td < triangles.GetCount(); td++)
	{
		const Triangle& t = triangles[td];
		RLOG(NFormat("[%d] = [%d, %d, %d] (%4v)",
			td, t[0], t[1], t[2],
			t.IsProper() ? (At(t, 1) - At(t, 0)) % (At(t, 2) - At(t, 1)) : double(Null))
			<< NFormat(" -> [%d & %d, %d & %d, %d & %d]",
			t.Next(0), t.NextIndex(0),
			t.Next(1), t.NextIndex(1),
			t.Next(2), t.NextIndex(2)));
	}
	RLOG("");
#endif

// #define SANITY
#ifdef SANITY
	i32 sanity = 0;
#endif

	i32 clean = 0;
	do
	{
	#ifdef SANITY
		ASSERT(sanity++ < 1000);
	#endif
		LLOG("--------------- Cleaning from " << clean);
		i32 old_clean = clean;
		clean = triangles.GetCount();
		for(i32 i = clean; --i >= old_clean;)
			if(triangles[i].IsProper())
			{
				Triangle& t = triangles[i];
				for(i32 x = 0; x < 3; x++)
				{
					i32 j = t.Next(x);
					Triangle& u = triangles[j];
					if(u.IsProper())
					{
						i32 x1 = x + 1, x2 = x + 2;
						if(x1 >= 3) x1 -= 3;
						if(x2 >= 3) x2 -= 3;
						Pointf A = At(t, x);
						Pointf B = At(t, x1);
						Pointf C = At(t, x2);
						i32 y = t.NextIndex(x);
						Pointf D = At(u, y);
						double t1 = (A - C) ^ (B - A);
						double t2 = (B - C) % (B - A);
						double u1 = (D - C) ^ (B - D);
						double u2 = (B - C) % (B - D);
						if(t1 * u2 - t2 * u1 < -epsilon)
						{ // not locally Delaunay, flip
							i32 y1 = y + 1, y2 = y + 2;
							if(y1 >= 3) y1 -= 3;
							if(y2 >= 3) y2 -= 3;

							LLOG("Delaunay flip (" << i << " / " << x << ", " << j << " / " << y << ")");
							LLOG(i << ": " << t[x] << " - " << A << ", " << t[x1] << " - " << B << ", " << t[x2] << " - " << C);
							LLOG(j << ": " << u[y] << " - " << D << ", " << u[y1] << " - " << At(u, y1) << ", " << u[y2] << " - " << At(u, y2));
							LLOG("t1 = " << t1 << ", t2 = " << t2 << ", t = " << t1 / t2);
							LLOG("u1 = " << u1 << ", u2 = " << u2 << ", u = " << u1 / u2);

							Triangle ot = t;
							Triangle ou = u;
							ASSERT(ot[x1] == ou[y2] && ot[x2] == ou[y1]);
							t.Set(ot[x1], ou[y], ot[x]);
							u.Set(ou[y1], ot[x], ou[y]);
							Link(i, 0, j, 0);
							Link(i, 1, ot.Next(x2), ot.NextIndex(x2));
							Link(i, 2, ou.Next(y1), ou.NextIndex(y1));
							Link(j, 1, ou.Next(y2), ou.NextIndex(y2));
							Link(j, 2, ot.Next(x1), ot.NextIndex(x1));
							clean = i;

							LLOG("After flip: [" << i << "] = " << t[x] << ", " << t[x1] << ", " << t[y2]
								<< "; [" << j << "] = " << u[y] << ", " << u[y1] << ", " << u[y2]);
						}
					}
				}
			}
	}
	while(clean < triangles.GetCount());
}

void Delaunay::CreatePair(i32 i, i32 j)
{
	LLOG("CreatePair(" << i << ": " << points[i] << ", " << j << ": " << points[j] << ")");

	i32 ia = triangles.GetCount(), ib = ia + 1;
	triangles.Add().Set(-1, i, j);
	triangles.Add().Set(-1, j, i);
	Link(ia, 0, ib, 0);
	Link(ia, 1, ib, 2);
	Link(ia, 2, ib, 1);

	tihull = ia;
}

void Delaunay::AddHull(i32 i)
{
	LLOG("AddHull(" << i << ": " << points[i] << ")");

	ASSERT(tihull >= 0);
	Pointf newpt = points[i];
	i32 hi = tihull;
	i32 vf = -1, vl = -1;
	bool was_out = true, fix_out = false;
	i32 im = -1;
	double nd2 = 1e300;
	do
	{
		const Triangle& t = triangles[hi];
		Pointf t1 = At(t, 1), t2 = At(t, 2), tm = (t1 + t2) * 0.5;
		double d2 = Squared(t1 - newpt);
		if(d2 <= epsilon2)
		{
			LLOG("-> too close to " << t[1] << ": " << t1);
			return; // too close
		}
		if(d2 < nd2)
		{
			im = hi;
			nd2 = d2;
		}
		if((t2 - t1) % (newpt - tm) > epsilon2)
		{
			LLOG("IN[" << hi << "], was_out = " << was_out);
			if(was_out)
				vf = hi;
			if(!fix_out)
				vl = hi;
			was_out = false;
		}
		else
		{
			LLOG("OUT[" << hi << "], was_out = " << was_out);
			was_out = true;
			if(vl >= 0)
				fix_out = true;
		}
		hi = t.Next(1);
	}
	while(hi != tihull);
	if(vf < 0)
	{ // collinear, extend fan
		Triangle& tm = triangles[im];
		i32 in = tm.Next(2);
//		Triangle& tn = triangles[in];

		i32 j = tm[1];

		i32 ia = triangles.GetCount(), ib = ia + 1;
		LLOG("collinear -> connect " << im << " -> " << ia << " -> " << ib << " -> " << in);
		triangles.Add().Set(-1, i, j);
		triangles.Add().Set(-1, j, i);
		Link(ia, 0, ib, 0);
		Link(ia, 2, ib, 1);
		Link(ia, 1, im, 2);
		Link(ib, 2, in, 1);
	}
	else
	{
		Triangle& tf = triangles[vf];
		Triangle& tl = triangles[vl];
		i32 xfn = tf.Next(2), xln = tl.Next(1);

		i32 xf = triangles.GetCount(), xl = xf + 1;
		LLOG("adding vertex " << i << ": " << points[i] << " to hull between " << vf << " and " << vl);
		triangles.Add().Set(-1, tf[1], i);
		triangles.Add().Set(-1, i, tl[2]);

		tihull = xf;
		tf[0] = i;
		for(i32 f = vf; f != vl; triangles[f = triangles[f].Next(1)][0] = i)
			;

		Link(xf, 0, vf, 2);
		Link(xl, 0, vl, 1);

		Link(xf, 2, xfn, 1);
		Link(xl, 1, xln, 2);

		Link(xf, 1, xl, 2);
	}
}

}
