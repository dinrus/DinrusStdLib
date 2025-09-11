#include "../Core.h"

namespace drx {

// iscale: computes x * y / z.

i32 iscale(i32 x, i32 y, i32 z)
{
#if CPU_32 && COMPILER_MSC
	__asm {
		mov		eax, [x]
		imul	[y]
		idiv	[z]
	}
#else
	z64 res = x;
	res *= y;
	res /= z;
	return (i32)res;
#endif
}

// iscalefloor: computes x * y / z, rounded towards -infty.

i32 iscalefloor(i32 x, i32 y, i32 z)
{
#if CPU_32 && COMPILER_MSC
	__asm {
		mov		eax, [x]
		imul	[y]
		idiv	[z]
		and		edx, edx
		jge		__1
		dec		eax
	__1:
	}
#else
	z64 res = x;
	z64 mulres = res * y;
	res = mulres / z;
	if(res * z != mulres)
		res--;
	return (i32)res;
#endif
}

// iscaleceil: computes x * y / z, rounded towards +infty.

i32 iscaleceil(i32 x, i32 y, i32 z)
{
#if CPU_32 && COMPILER_MSC
	__asm {
		mov		eax, [x]
		imul	[y]
		idiv	[z]
		and		edx, edx
		jle		__1
		inc		eax
	__1:
	}
#else
	z64 res = x;
	z64 mulres = res * y;
	res = mulres / z;
	if(res * z != mulres)
		res++;
	return (i32)res;
#endif
}

#ifdef COMPILER_MSC
#pragma warning(default: 4035)
#endif

// idivfloor: returns x / y, truncated towards -infinity.

i32 idivfloor(i32 x, i32 y)
{
	return x / y - (x % y < 0);
}

// idivceil: returns x / y, truncated towards +infinity.

i32 idivceil(i32 x, i32 y)
{
	return x / y + (x % y > 0);
}

// itimesfloor: moves x to nearest lower multiple of y.

i32 itimesfloor(i32 x, i32 y)
{
	return y * idivfloor(x, y);
}

// itimesceil: moves x to nearest higher multiple of y.

i32 itimesceil(i32 x, i32 y)
{
	return y * idivceil(x, y);
}

// fround: converts double to nearest integer. Checks integer limits.

i32 fround(double value)
{
	return value >= INT_MAX ? INT_MAX : value <= INT_MIN ? INT_MIN
		: (i32)floor(value + 0.5);
}

// ffloor: converts double to integer rounding towards -infinity. Checks integer limits.

i32 ffloor(double value)
{
	return value >= INT_MAX ? INT_MAX : value <= INT_MIN ? INT_MIN
		: (i32)floor(value);
}

// fceil: converts double to integer rounding towards +infinity. Checks integer limits.

i32 fceil(double value)
{
	return value >= INT_MAX ? INT_MAX : value <= INT_MIN ? INT_MIN
		: (i32)ceil(value);
}

// fround: converts double to nearest integer. Checks integer limits.

z64 fround64(double value)
{
	return value >= (double)INT64_MAX ? INT64_MAX : value <= (double)INT64_MIN ? INT64_MIN
		: (z64)floor(value + 0.5);
}

// ffloor: converts double to integer rounding towards -infinity. Checks integer limits.

z64 ffloor64(double value)
{
	return value >= (double)INT64_MAX ? INT64_MAX : value <= (double)INT64_MIN ? INT64_MIN
		: (z64)floor(value);
}

// fceil: converts double to integer rounding towards +infinity. Checks integer limits.

z64 fceil64(double value)
{
	return value >= (double)INT64_MAX ? INT64_MAX : value <= (double)INT64_MIN ? INT64_MIN
		: (z64)ceil(value);
}

// asTxt: converts double numbers to strings.

Txt AsTxt(double x, i32 digits)
{
	return FormatG(x, digits);
}

// modulo: working version of math function `fmod'.

double modulo(double x, double y)
{
	return y ? (x - y * floor(x / y)) : x;
}

i32 ilog10(double d)
{
	if(IsNull(d) || d == 0)
		return Null;
	return fceil(log10(fabs(d)));
}

double ipow10(i32 i)
{
	static const double pow_tbl[] =
	{
		1e00,
		1e01, 1e02, 1e03, 1e04, 1e05, 1e06, 1e07, 1e08, 1e09, 1e10,
		1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20,
		1e21, 1e22, 1e23, 1e24, 1e25, 1e26, 1e27, 1e28, 1e29, 1e30,
		1e31, 1e32, 1e33, 1e34, 1e35, 1e36, 1e37, 1e38, 1e39, 1e40,
		1e41, 1e42, 1e43, 1e44, 1e45, 1e46, 1e47, 1e48, 1e49, 1e50,
	};
	if(i >= 0 && i < __countof(pow_tbl))
		return pow_tbl[i];
	else if(i < 0 && i > -__countof(pow_tbl))
		return 1 / pow_tbl[-i];
	return pow(10.0, (double)i);
}

double normalize(double d, i32& exp)
{
	if(IsNull(d) || d == 0)
	{
		exp = Null;
		return d;
	}
	bool sign = (d < 0);
	if(sign) d = -d;
	exp = minmax<i32>(ilog10(d), -308, +308); // 8-u8 double!
	d /= ipow10(exp);
	if(d >= 10) { d /= 10; exp++; }
	if(d < 1)   { d *= 10; exp--; }
	return sign ? -d : d;
}

double roundr(double d, i32 digits)
{
	i32 i = ilog10(d);
	if(IsNull(i))
		return d;
	double fac = ipow10(i - digits);
	return floor(d / fac + 0.5) * fac;
}

double floorr(double d, i32 digits)
{
	i32 i = ilog10(d);
	if(IsNull(i))
		return d;
	double fac = ipow10(i - digits);
	return floor(d / fac) * fac;
}

double ceilr(double d, i32 digits)
{
	i32 i = ilog10(d);
	if(IsNull(i))
		return d;
	double fac = ipow10(i - digits);
	return ceil(d / fac) * fac;
}

i32 SolveQuadraticEquation(double a, double b, double c, double *r)
{
	if(a == 0) {
		if(b == 0)
			return 0;
		r[0] = r[1] = -c / b;
		return 1;
    }
	double d = b * b - 4 * a * c;
	if (d < 0)
		return 0;
	a *= 2;
	if (d == 0) {
		r[0] = r[1] = -b / a;
		return 1;
	}
	d = sqrt(d);
	if(b == 0) {
		r[0] = d / a;
		r[1] = -r[0];
    }
	else {
		r[0] = (-b + d) / a;
		r[1] = (-b - d) / a;
    }
	return 2;
}

}