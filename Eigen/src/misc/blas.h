#ifndef BLAS_H
#define BLAS_H

#ifdef __cplusplus
extern "C"
{
#endif

#define BLASFUNC(FUNC) FUNC##_

#ifdef __WIN64__
typedef z64 BLASLONG;
typedef unsigned z64 BLASULONG;
#else
typedef long BLASLONG;
typedef unsigned long BLASULONG;
#endif

i32    BLASFUNC(xerbla)(const char *, i32 *info, i32);

float  BLASFUNC(sdot)  (i32 *, float  *, i32 *, float  *, i32 *);
float  BLASFUNC(sdsdot)(i32 *, float  *,        float  *, i32 *, float  *, i32 *);

double BLASFUNC(dsdot) (i32 *, float  *, i32 *, float  *, i32 *);
double BLASFUNC(ddot)  (i32 *, double *, i32 *, double *, i32 *);
double BLASFUNC(qdot)  (i32 *, double *, i32 *, double *, i32 *);

i32  BLASFUNC(cdotuw)  (i32 *, float  *, i32 *, float  *, i32 *, float*);
i32  BLASFUNC(cdotcw)  (i32 *, float  *, i32 *, float  *, i32 *, float*);
i32  BLASFUNC(zdotuw)  (i32 *, double  *, i32 *, double  *, i32 *, double*);
i32  BLASFUNC(zdotcw)  (i32 *, double  *, i32 *, double  *, i32 *, double*);

i32    BLASFUNC(saxpy) (i32k *, const float  *, const float  *, i32k *, float  *, i32k *);
i32    BLASFUNC(daxpy) (i32k *, const double *, const double *, i32k *, double *, i32k *);
i32    BLASFUNC(qaxpy) (i32k *, const double *, const double *, i32k *, double *, i32k *);
i32    BLASFUNC(caxpy) (i32k *, const float  *, const float  *, i32k *, float  *, i32k *);
i32    BLASFUNC(zaxpy) (i32k *, const double *, const double *, i32k *, double *, i32k *);
i32    BLASFUNC(xaxpy) (i32k *, const double *, const double *, i32k *, double *, i32k *);
i32    BLASFUNC(caxpyc)(i32k *, const float  *, const float  *, i32k *, float  *, i32k *);
i32    BLASFUNC(zaxpyc)(i32k *, const double *, const double *, i32k *, double *, i32k *);
i32    BLASFUNC(xaxpyc)(i32k *, const double *, const double *, i32k *, double *, i32k *);

i32    BLASFUNC(scopy) (i32 *, float  *, i32 *, float  *, i32 *);
i32    BLASFUNC(dcopy) (i32 *, double *, i32 *, double *, i32 *);
i32    BLASFUNC(qcopy) (i32 *, double *, i32 *, double *, i32 *);
i32    BLASFUNC(ccopy) (i32 *, float  *, i32 *, float  *, i32 *);
i32    BLASFUNC(zcopy) (i32 *, double *, i32 *, double *, i32 *);
i32    BLASFUNC(xcopy) (i32 *, double *, i32 *, double *, i32 *);

i32    BLASFUNC(sswap) (i32 *, float  *, i32 *, float  *, i32 *);
i32    BLASFUNC(dswap) (i32 *, double *, i32 *, double *, i32 *);
i32    BLASFUNC(qswap) (i32 *, double *, i32 *, double *, i32 *);
i32    BLASFUNC(cswap) (i32 *, float  *, i32 *, float  *, i32 *);
i32    BLASFUNC(zswap) (i32 *, double *, i32 *, double *, i32 *);
i32    BLASFUNC(xswap) (i32 *, double *, i32 *, double *, i32 *);

float  BLASFUNC(sasum) (i32 *, float  *, i32 *);
float  BLASFUNC(scasum)(i32 *, float  *, i32 *);
double BLASFUNC(dasum) (i32 *, double *, i32 *);
double BLASFUNC(qasum) (i32 *, double *, i32 *);
double BLASFUNC(dzasum)(i32 *, double *, i32 *);
double BLASFUNC(qxasum)(i32 *, double *, i32 *);

i32    BLASFUNC(isamax)(i32 *, float  *, i32 *);
i32    BLASFUNC(idamax)(i32 *, double *, i32 *);
i32    BLASFUNC(iqamax)(i32 *, double *, i32 *);
i32    BLASFUNC(icamax)(i32 *, float  *, i32 *);
i32    BLASFUNC(izamax)(i32 *, double *, i32 *);
i32    BLASFUNC(ixamax)(i32 *, double *, i32 *);

i32    BLASFUNC(ismax) (i32 *, float  *, i32 *);
i32    BLASFUNC(idmax) (i32 *, double *, i32 *);
i32    BLASFUNC(iqmax) (i32 *, double *, i32 *);
i32    BLASFUNC(icmax) (i32 *, float  *, i32 *);
i32    BLASFUNC(izmax) (i32 *, double *, i32 *);
i32    BLASFUNC(ixmax) (i32 *, double *, i32 *);

i32    BLASFUNC(isamin)(i32 *, float  *, i32 *);
i32    BLASFUNC(idamin)(i32 *, double *, i32 *);
i32    BLASFUNC(iqamin)(i32 *, double *, i32 *);
i32    BLASFUNC(icamin)(i32 *, float  *, i32 *);
i32    BLASFUNC(izamin)(i32 *, double *, i32 *);
i32    BLASFUNC(ixamin)(i32 *, double *, i32 *);

i32    BLASFUNC(ismin)(i32 *, float  *, i32 *);
i32    BLASFUNC(idmin)(i32 *, double *, i32 *);
i32    BLASFUNC(iqmin)(i32 *, double *, i32 *);
i32    BLASFUNC(icmin)(i32 *, float  *, i32 *);
i32    BLASFUNC(izmin)(i32 *, double *, i32 *);
i32    BLASFUNC(ixmin)(i32 *, double *, i32 *);

float  BLASFUNC(samax) (i32 *, float  *, i32 *);
double BLASFUNC(damax) (i32 *, double *, i32 *);
double BLASFUNC(qamax) (i32 *, double *, i32 *);
float  BLASFUNC(scamax)(i32 *, float  *, i32 *);
double BLASFUNC(dzamax)(i32 *, double *, i32 *);
double BLASFUNC(qxamax)(i32 *, double *, i32 *);

float  BLASFUNC(samin) (i32 *, float  *, i32 *);
double BLASFUNC(damin) (i32 *, double *, i32 *);
double BLASFUNC(qamin) (i32 *, double *, i32 *);
float  BLASFUNC(scamin)(i32 *, float  *, i32 *);
double BLASFUNC(dzamin)(i32 *, double *, i32 *);
double BLASFUNC(qxamin)(i32 *, double *, i32 *);

float  BLASFUNC(smax)  (i32 *, float  *, i32 *);
double BLASFUNC(dmax)  (i32 *, double *, i32 *);
double BLASFUNC(qmax)  (i32 *, double *, i32 *);
float  BLASFUNC(scmax) (i32 *, float  *, i32 *);
double BLASFUNC(dzmax) (i32 *, double *, i32 *);
double BLASFUNC(qxmax) (i32 *, double *, i32 *);

float  BLASFUNC(smin)  (i32 *, float  *, i32 *);
double BLASFUNC(dmin)  (i32 *, double *, i32 *);
double BLASFUNC(qmin)  (i32 *, double *, i32 *);
float  BLASFUNC(scmin) (i32 *, float  *, i32 *);
double BLASFUNC(dzmin) (i32 *, double *, i32 *);
double BLASFUNC(qxmin) (i32 *, double *, i32 *);

i32    BLASFUNC(sscal) (i32 *,  float  *, float  *, i32 *);
i32    BLASFUNC(dscal) (i32 *,  double *, double *, i32 *);
i32    BLASFUNC(qscal) (i32 *,  double *, double *, i32 *);
i32    BLASFUNC(cscal) (i32 *,  float  *, float  *, i32 *);
i32    BLASFUNC(zscal) (i32 *,  double *, double *, i32 *);
i32    BLASFUNC(xscal) (i32 *,  double *, double *, i32 *);
i32    BLASFUNC(csscal)(i32 *,  float  *, float  *, i32 *);
i32    BLASFUNC(zdscal)(i32 *,  double *, double *, i32 *);
i32    BLASFUNC(xqscal)(i32 *,  double *, double *, i32 *);

float  BLASFUNC(snrm2) (i32 *, float  *, i32 *);
float  BLASFUNC(scnrm2)(i32 *, float  *, i32 *);

double BLASFUNC(dnrm2) (i32 *, double *, i32 *);
double BLASFUNC(qnrm2) (i32 *, double *, i32 *);
double BLASFUNC(dznrm2)(i32 *, double *, i32 *);
double BLASFUNC(qxnrm2)(i32 *, double *, i32 *);

i32    BLASFUNC(srot)  (i32 *, float  *, i32 *, float  *, i32 *, float  *, float  *);
i32    BLASFUNC(drot)  (i32 *, double *, i32 *, double *, i32 *, double *, double *);
i32    BLASFUNC(qrot)  (i32 *, double *, i32 *, double *, i32 *, double *, double *);
i32    BLASFUNC(csrot) (i32 *, float  *, i32 *, float  *, i32 *, float  *, float  *);
i32    BLASFUNC(zdrot) (i32 *, double *, i32 *, double *, i32 *, double *, double *);
i32    BLASFUNC(xqrot) (i32 *, double *, i32 *, double *, i32 *, double *, double *);

i32    BLASFUNC(srotg) (float  *, float  *, float  *, float  *);
i32    BLASFUNC(drotg) (double *, double *, double *, double *);
i32    BLASFUNC(qrotg) (double *, double *, double *, double *);
i32    BLASFUNC(crotg) (float  *, float  *, float  *, float  *);
i32    BLASFUNC(zrotg) (double *, double *, double *, double *);
i32    BLASFUNC(xrotg) (double *, double *, double *, double *);

i32    BLASFUNC(srotmg)(float  *, float  *, float  *, float  *, float  *);
i32    BLASFUNC(drotmg)(double *, double *, double *, double *, double *);

i32    BLASFUNC(srotm) (i32 *, float  *, i32 *, float  *, i32 *, float  *);
i32    BLASFUNC(drotm) (i32 *, double *, i32 *, double *, i32 *, double *);
i32    BLASFUNC(qrotm) (i32 *, double *, i32 *, double *, i32 *, double *);

/* Level 2 routines */

i32 BLASFUNC(sger)(i32 *,    i32 *, float *,  float *, i32 *,
		   float *,  i32 *, float *,  i32 *);
i32 BLASFUNC(dger)(i32 *,    i32 *, double *, double *, i32 *,
		   double *, i32 *, double *, i32 *);
i32 BLASFUNC(qger)(i32 *,    i32 *, double *, double *, i32 *,
		   double *, i32 *, double *, i32 *);
i32 BLASFUNC(cgeru)(i32 *,    i32 *, float *,  float *, i32 *,
		    float *,  i32 *, float *,  i32 *);
i32 BLASFUNC(cgerc)(i32 *,    i32 *, float *,  float *, i32 *,
		    float *,  i32 *, float *,  i32 *);
i32 BLASFUNC(zgeru)(i32 *,    i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, i32 *);
i32 BLASFUNC(zgerc)(i32 *,    i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, i32 *);
i32 BLASFUNC(xgeru)(i32 *,    i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, i32 *);
i32 BLASFUNC(xgerc)(i32 *,    i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, i32 *);

i32 BLASFUNC(sgemv)(const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(dgemv)(const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(qgemv)(const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(cgemv)(const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zgemv)(const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xgemv)(const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);

i32 BLASFUNC(strsv) (const char *, const char *, const char *, i32k *, const float  *, i32k *, float  *, i32k *);
i32 BLASFUNC(dtrsv) (const char *, const char *, const char *, i32k *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(qtrsv) (const char *, const char *, const char *, i32k *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(ctrsv) (const char *, const char *, const char *, i32k *, const float  *, i32k *, float  *, i32k *);
i32 BLASFUNC(ztrsv) (const char *, const char *, const char *, i32k *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(xtrsv) (const char *, const char *, const char *, i32k *, const double *, i32k *, double *, i32k *);

i32 BLASFUNC(stpsv) (char *, char *, char *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(dtpsv) (char *, char *, char *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(qtpsv) (char *, char *, char *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(ctpsv) (char *, char *, char *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(ztpsv) (char *, char *, char *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xtpsv) (char *, char *, char *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(strmv) (const char *, const char *, const char *, i32k *, const float  *, i32k *, float  *, i32k *);
i32 BLASFUNC(dtrmv) (const char *, const char *, const char *, i32k *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(qtrmv) (const char *, const char *, const char *, i32k *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(ctrmv) (const char *, const char *, const char *, i32k *, const float  *, i32k *, float  *, i32k *);
i32 BLASFUNC(ztrmv) (const char *, const char *, const char *, i32k *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(xtrmv) (const char *, const char *, const char *, i32k *, const double *, i32k *, double *, i32k *);

i32 BLASFUNC(stpmv) (char *, char *, char *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(dtpmv) (char *, char *, char *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(qtpmv) (char *, char *, char *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(ctpmv) (char *, char *, char *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(ztpmv) (char *, char *, char *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xtpmv) (char *, char *, char *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(stbmv) (char *, char *, char *, i32 *, i32 *, float  *, i32 *, float  *, i32 *);
i32 BLASFUNC(dtbmv) (char *, char *, char *, i32 *, i32 *, double *, i32 *, double *, i32 *);
i32 BLASFUNC(qtbmv) (char *, char *, char *, i32 *, i32 *, double *, i32 *, double *, i32 *);
i32 BLASFUNC(ctbmv) (char *, char *, char *, i32 *, i32 *, float  *, i32 *, float  *, i32 *);
i32 BLASFUNC(ztbmv) (char *, char *, char *, i32 *, i32 *, double *, i32 *, double *, i32 *);
i32 BLASFUNC(xtbmv) (char *, char *, char *, i32 *, i32 *, double *, i32 *, double *, i32 *);

i32 BLASFUNC(stbsv) (char *, char *, char *, i32 *, i32 *, float  *, i32 *, float  *, i32 *);
i32 BLASFUNC(dtbsv) (char *, char *, char *, i32 *, i32 *, double *, i32 *, double *, i32 *);
i32 BLASFUNC(qtbsv) (char *, char *, char *, i32 *, i32 *, double *, i32 *, double *, i32 *);
i32 BLASFUNC(ctbsv) (char *, char *, char *, i32 *, i32 *, float  *, i32 *, float  *, i32 *);
i32 BLASFUNC(ztbsv) (char *, char *, char *, i32 *, i32 *, double *, i32 *, double *, i32 *);
i32 BLASFUNC(xtbsv) (char *, char *, char *, i32 *, i32 *, double *, i32 *, double *, i32 *);

i32 BLASFUNC(ssymv) (const char *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(dsymv) (const char *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(qsymv) (const char *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);

i32 BLASFUNC(sspmv) (char *, i32 *, float  *, float *,
		     float  *, i32 *, float *, float *, i32 *);
i32 BLASFUNC(dspmv) (char *, i32 *, double  *, double *,
		     double  *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(qspmv) (char *, i32 *, double  *, double *,
		     double  *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(ssyr) (const char *, i32k *, const float   *, const float  *, i32k *, float  *, i32k *);
i32 BLASFUNC(dsyr) (const char *, i32k *, const double  *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(qsyr) (const char *, i32k *, const double  *, const double *, i32k *, double *, i32k *);

i32 BLASFUNC(ssyr2) (const char *, i32k *, const float   *, const float  *, i32k *, const float  *, i32k *, float  *, i32k *);
i32 BLASFUNC(dsyr2) (const char *, i32k *, const double  *, const double *, i32k *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(qsyr2) (const char *, i32k *, const double  *, const double *, i32k *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(csyr2) (const char *, i32k *, const float   *, const float  *, i32k *, const float  *, i32k *, float  *, i32k *);
i32 BLASFUNC(zsyr2) (const char *, i32k *, const double  *, const double *, i32k *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(xsyr2) (const char *, i32k *, const double  *, const double *, i32k *, const double *, i32k *, double *, i32k *);

i32 BLASFUNC(sspr) (char *, i32 *, float   *, float  *, i32 *,
		    float  *);
i32 BLASFUNC(dspr) (char *, i32 *, double  *, double *, i32 *,
		    double *);
i32 BLASFUNC(qspr) (char *, i32 *, double  *, double *, i32 *,
		    double *);

i32 BLASFUNC(sspr2) (char *, i32 *, float   *,
		     float  *, i32 *, float  *, i32 *, float  *);
i32 BLASFUNC(dspr2) (char *, i32 *, double  *,
		     double *, i32 *, double *, i32 *, double *);
i32 BLASFUNC(qspr2) (char *, i32 *, double  *,
		     double *, i32 *, double *, i32 *, double *);
i32 BLASFUNC(cspr2) (char *, i32 *, float   *,
		     float  *, i32 *, float  *, i32 *, float  *);
i32 BLASFUNC(zspr2) (char *, i32 *, double  *,
		     double *, i32 *, double *, i32 *, double *);
i32 BLASFUNC(xspr2) (char *, i32 *, double  *,
		     double *, i32 *, double *, i32 *, double *);

i32 BLASFUNC(cher) (char *, i32 *, float   *, float  *, i32 *,
		    float  *, i32 *);
i32 BLASFUNC(zher) (char *, i32 *, double  *, double *, i32 *,
		    double *, i32 *);
i32 BLASFUNC(xher) (char *, i32 *, double  *, double *, i32 *,
		    double *, i32 *);

i32 BLASFUNC(chpr) (char *, i32 *, float   *, float  *, i32 *, float  *);
i32 BLASFUNC(zhpr) (char *, i32 *, double  *, double *, i32 *, double *);
i32 BLASFUNC(xhpr) (char *, i32 *, double  *, double *, i32 *, double *);

i32 BLASFUNC(cher2) (char *, i32 *, float   *,
		     float  *, i32 *, float  *, i32 *, float  *, i32 *);
i32 BLASFUNC(zher2) (char *, i32 *, double  *,
		     double *, i32 *, double *, i32 *, double *, i32 *);
i32 BLASFUNC(xher2) (char *, i32 *, double  *,
		     double *, i32 *, double *, i32 *, double *, i32 *);

i32 BLASFUNC(chpr2) (char *, i32 *, float   *,
		     float  *, i32 *, float  *, i32 *, float  *);
i32 BLASFUNC(zhpr2) (char *, i32 *, double  *,
		     double *, i32 *, double *, i32 *, double *);
i32 BLASFUNC(xhpr2) (char *, i32 *, double  *,
		     double *, i32 *, double *, i32 *, double *);

i32 BLASFUNC(chemv) (const char *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zhemv) (const char *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xhemv) (const char *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);

i32 BLASFUNC(chpmv) (char *, i32 *, float  *, float *,
		     float  *, i32 *, float *, float *, i32 *);
i32 BLASFUNC(zhpmv) (char *, i32 *, double  *, double *,
		     double  *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xhpmv) (char *, i32 *, double  *, double *,
		     double  *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(snorm)(char *, i32 *, i32 *, float  *, i32 *);
i32 BLASFUNC(dnorm)(char *, i32 *, i32 *, double *, i32 *);
i32 BLASFUNC(cnorm)(char *, i32 *, i32 *, float  *, i32 *);
i32 BLASFUNC(znorm)(char *, i32 *, i32 *, double *, i32 *);

i32 BLASFUNC(sgbmv)(char *, i32 *, i32 *, i32 *, i32 *, float  *, float  *, i32 *,
		    float  *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(dgbmv)(char *, i32 *, i32 *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(qgbmv)(char *, i32 *, i32 *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(cgbmv)(char *, i32 *, i32 *, i32 *, i32 *, float  *, float  *, i32 *,
		    float  *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(zgbmv)(char *, i32 *, i32 *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xgbmv)(char *, i32 *, i32 *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(ssbmv)(char *, i32 *, i32 *, float  *, float  *, i32 *,
		    float  *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(dsbmv)(char *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(qsbmv)(char *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(csbmv)(char *, i32 *, i32 *, float  *, float  *, i32 *,
		    float  *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(zsbmv)(char *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xsbmv)(char *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(chbmv)(char *, i32 *, i32 *, float  *, float  *, i32 *,
		    float  *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(zhbmv)(char *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xhbmv)(char *, i32 *, i32 *, double *, double *, i32 *,
		    double *, i32 *, double *, double *, i32 *);

/* Level 3 routines */

i32 BLASFUNC(sgemm)(const char *, const char *, i32k *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(dgemm)(const char *, const char *, i32k *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(qgemm)(const char *, const char *, i32k *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(cgemm)(const char *, const char *, i32k *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zgemm)(const char *, const char *, i32k *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xgemm)(const char *, const char *, i32k *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);

i32 BLASFUNC(cgemm3m)(char *, char *, i32 *, i32 *, i32 *, float *,
	   float  *, i32 *, float  *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(zgemm3m)(char *, char *, i32 *, i32 *, i32 *, double *,
	   double *, i32 *, double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xgemm3m)(char *, char *, i32 *, i32 *, i32 *, double *,
	   double *, i32 *, double *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(sge2mm)(char *, char *, char *, i32 *, i32 *,
		     float *, float  *, i32 *, float  *, i32 *,
		     float *, float  *, i32 *);
i32 BLASFUNC(dge2mm)(char *, char *, char *, i32 *, i32 *,
		     double *, double  *, i32 *, double  *, i32 *,
		     double *, double  *, i32 *);
i32 BLASFUNC(cge2mm)(char *, char *, char *, i32 *, i32 *,
		     float *, float  *, i32 *, float  *, i32 *,
		     float *, float  *, i32 *);
i32 BLASFUNC(zge2mm)(char *, char *, char *, i32 *, i32 *,
		     double *, double  *, i32 *, double  *, i32 *,
		     double *, double  *, i32 *);

i32 BLASFUNC(strsm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const float *,  const float *,  i32k *, float *,  i32k *);
i32 BLASFUNC(dtrsm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(qtrsm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(ctrsm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const float *,  const float *,  i32k *, float *,  i32k *);
i32 BLASFUNC(ztrsm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(xtrsm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, double *, i32k *);

i32 BLASFUNC(strmm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const float *,  const float *,  i32k *, float *,  i32k *);
i32 BLASFUNC(dtrmm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(qtrmm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(ctrmm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const float *,  const float *,  i32k *, float *,  i32k *);
i32 BLASFUNC(ztrmm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, double *, i32k *);
i32 BLASFUNC(xtrmm)(const char *, const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, double *, i32k *);

i32 BLASFUNC(ssymm)(const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(dsymm)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(qsymm)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(csymm)(const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zsymm)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xsymm)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);

i32 BLASFUNC(csymm3m)(char *, char *, i32 *, i32 *, float  *, float  *, i32 *, float  *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(zsymm3m)(char *, char *, i32 *, i32 *, double *, double *, i32 *, double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xsymm3m)(char *, char *, i32 *, i32 *, double *, double *, i32 *, double *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(ssyrk)(const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(dsyrk)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(qsyrk)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(csyrk)(const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zsyrk)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xsyrk)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, double *, i32k *);

i32 BLASFUNC(ssyr2k)(const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(dsyr2k)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double*, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(qsyr2k)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double*, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(csyr2k)(const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zsyr2k)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double*, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xsyr2k)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double*, i32k *, const double *, double *, i32k *);

i32 BLASFUNC(chemm)(const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zhemm)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xhemm)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);

i32 BLASFUNC(chemm3m)(char *, char *, i32 *, i32 *, float  *, float  *, i32 *,
	   float  *, i32 *, float  *, float  *, i32 *);
i32 BLASFUNC(zhemm3m)(char *, char *, i32 *, i32 *, double *, double *, i32 *,
	   double *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xhemm3m)(char *, char *, i32 *, i32 *, double *, double *, i32 *,
	   double *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(cherk)(const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zherk)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xherk)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, double *, i32k *);

i32 BLASFUNC(cher2k)(const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zher2k)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xher2k)(const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(cher2m)(const char *, const char *, const char *, i32k *, i32k *, const float  *, const float  *, i32k *, const float *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zher2m)(const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double*, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xher2m)(const char *, const char *, const char *, i32k *, i32k *, const double *, const double *, i32k *, const double*, i32k *, const double *, double *, i32k *);


#ifdef __cplusplus
}
#endif

#endif
