#include "Geom.h"

namespace drx {

LinearSolver::LinearSolver(i32 count, double tolerance)
: rows(count)
, col1(count + 1)
, tolerance(tolerance)
{
	ASSERT(tolerance >= 0);
	matrix.SetCount(count * (count + 1), 0);
	left_rows.SetCount(rows);
	left_cols.SetCount(rows);
	for(i32 i = 0; i < rows; i++)
		left_rows[i] = left_cols[i] = i;
}

void LinearSolver::AddLSI(const double *bases, double value)
{
	double *row = matrix.Begin();
	const double *bi = bases;
	for(i32 i = 0; i < rows; i++) {
		const double *bj = bases;
		for(i32 j = 0; j < rows; j++)
			*row++ += *bi * *bj++;
		*row++ += *bi++ * value;
	}
}

Vec<double> LinearSolver::Solve()
{
	while(!left_rows.IsEmpty())
	{
		i32 er = -1, ec = -1;
		double best = tolerance;
		for(i32 pr = 0; pr < left_rows.GetCount(); pr++)
		{
			const double *p = Row(left_rows[pr]);
			for(i32 pc = 0; pc < left_cols.GetCount(); pc++)
			{
				double v = fabs(p[left_cols[pc]]);
				if(v > best)
				{
					best = v;
					ec = pc;
					er = pr;
				}
			}
		}

		if(er < 0 || best <= 0) // just to be sure
		{ // no more fixed variables
			for(i32 i = 0; i < left_rows.GetCount(); i++)
				if(fabs(Right(left_rows[i])) > tolerance)
					return Vec<double>(); // error
			break;
		}

		i32 cr = left_rows[er], cc = left_cols[ec];

		i32 p = left_rows.Pop();
		if(er < left_rows.GetCount()) left_rows[er] = p;
		p = left_cols.Pop();
		if(ec < left_cols.GetCount()) left_cols[ec] = p;

		const double *src = Row(cr);
		i32k *xb = left_cols.Begin(), *xe = left_cols.End();
		for(i32 i = 0; i < left_rows.GetCount(); i++)
		{
			double *dest = Row(left_rows[i]);
			double r = -dest[cc] / src[cc];
			for(i32k *xp = xb; xp < xe; xp++)
				dest[*xp] += src[*xp] * r;
			dest[rows] += src[rows] * r;
		}

		pivots.Add(Point(cc, cr));
	}

	Vec<double> result;
	result.SetCount(rows, Null);
	for(i32 r = pivots.GetCount(); --r >= 0;)
	{
		Point pivot = pivots[r];
		const double *row = Row(pivot.y);
//		i32 px = pivot.x;
		double out = row[rows]; // right side
		for(i32 s = r; ++s < pivots.GetCount();)
		{
			i32 c = pivots[s].x;
			out -= row[c] * result[c];
		}
		result[pivot.x] = out / row[pivot.x];
	}

	return result;
}

void LinearSolver::SelfTest()
{
	for(i32 i = 0; i < 1000; i++)
	{
		i32 ord = rand() % 10 + 1;
		Vec<double> res;
		while(res.GetCount() < ord)
			res.Add(rand());
		LinearSolver ls(ord);
		for(i32 r = 0; r < ord; r++)
		{
			double rs = 0;
			for(i32 c = 0; c < ord; c++)
				rs += res[c] * (ls(r, c) = rand());
			ls(r) = rs;
		}
		Vec<double> out = ls.Solve();
		if(!out.IsEmpty())
		{
			ASSERT(out.GetCount() == ord);
			for(i32 c = 0; c < out.GetCount(); c++)
				if(!IsNull(out[c]))
				{
					double d = out[c] - res[c];
					ASSERT(fabs(d) <= 1e-3);
				}
		}
	}
}

}
