class LinearSolver
{
public:
	LinearSolver(i32 count, double tolerance = 1e-10);

	double        *Row(i32 r)                        { return &matrix[r * col1]; }
	const double  *Row(i32 r) const                  { return &matrix[r * col1]; }
	double&        Left(i32 r, i32 c)                { return matrix[r * col1 + c]; }
	double         Left(i32 r, i32 c) const          { return matrix[r * col1 + c]; }
	double&        Right(i32 r)                      { return matrix[(r + 1) * col1 - 1]; }
	double         Right(i32 r) const                { return matrix[(r + 1) * col1 - 1]; }
	double&        operator () (i32 r, i32 c)        { return Left(r, c); }
	double         operator () (i32 r, i32 c) const  { return Left(r, c); }
	double&        operator () (i32 r)               { return Right(r); }
	double         operator () (i32 r) const         { return Right(r); }

	void           AddLSI(const double *bases, double value);

	Vec<double> Solve();

	static void    SelfTest();

private:
	i32            rows;
	i32            col1;
	Vec<double> matrix;
	Vec<Point>  pivots;
	double         tolerance;
	Vec<i32>    left_rows, left_cols;
};
