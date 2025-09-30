#ifndef LAPACK_H
#define LAPACK_H

#include "blas.h"

#ifdef __cplusplus
extern "C"
{
#endif

i32 BLASFUNC(csymv) (const char *, i32k *, const float  *, const float  *, i32k *, const float  *, i32k *, const float  *, float  *, i32k *);
i32 BLASFUNC(zsymv) (const char *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);
i32 BLASFUNC(xsymv) (const char *, i32k *, const double *, const double *, i32k *, const double *, i32k *, const double *, double *, i32k *);


i32 BLASFUNC(cspmv) (char *, i32 *, float  *, float *,
         float  *, i32 *, float *, float *, i32 *);
i32 BLASFUNC(zspmv) (char *, i32 *, double  *, double *,
         double  *, i32 *, double *, double *, i32 *);
i32 BLASFUNC(xspmv) (char *, i32 *, double  *, double *,
         double  *, i32 *, double *, double *, i32 *);

i32 BLASFUNC(csyr) (char *, i32 *, float   *, float  *, i32 *,
        float  *, i32 *);
i32 BLASFUNC(zsyr) (char *, i32 *, double  *, double *, i32 *,
        double *, i32 *);
i32 BLASFUNC(xsyr) (char *, i32 *, double  *, double *, i32 *,
        double *, i32 *);

i32 BLASFUNC(cspr) (char *, i32 *, float   *, float  *, i32 *,
        float  *);
i32 BLASFUNC(zspr) (char *, i32 *, double  *, double *, i32 *,
        double *);
i32 BLASFUNC(xspr) (char *, i32 *, double  *, double *, i32 *,
        double *);

i32 BLASFUNC(sgemt)(char *, i32 *, i32 *, float  *, float  *, i32 *,
        float  *, i32 *);
i32 BLASFUNC(dgemt)(char *, i32 *, i32 *, double *, double *, i32 *,
        double *, i32 *);
i32 BLASFUNC(cgemt)(char *, i32 *, i32 *, float  *, float  *, i32 *,
        float  *, i32 *);
i32 BLASFUNC(zgemt)(char *, i32 *, i32 *, double *, double *, i32 *,
        double *, i32 *);

i32 BLASFUNC(sgema)(char *, char *, i32 *, i32 *, float  *,
        float  *, i32 *, float *, float  *, i32 *, float *, i32 *);
i32 BLASFUNC(dgema)(char *, char *, i32 *, i32 *, double *,
        double *, i32 *, double*, double *, i32 *, double*, i32 *);
i32 BLASFUNC(cgema)(char *, char *, i32 *, i32 *, float  *,
        float  *, i32 *, float *, float  *, i32 *, float *, i32 *);
i32 BLASFUNC(zgema)(char *, char *, i32 *, i32 *, double *,
        double *, i32 *, double*, double *, i32 *, double*, i32 *);

i32 BLASFUNC(sgems)(char *, char *, i32 *, i32 *, float  *,
        float  *, i32 *, float *, float  *, i32 *, float *, i32 *);
i32 BLASFUNC(dgems)(char *, char *, i32 *, i32 *, double *,
        double *, i32 *, double*, double *, i32 *, double*, i32 *);
i32 BLASFUNC(cgems)(char *, char *, i32 *, i32 *, float  *,
        float  *, i32 *, float *, float  *, i32 *, float *, i32 *);
i32 BLASFUNC(zgems)(char *, char *, i32 *, i32 *, double *,
        double *, i32 *, double*, double *, i32 *, double*, i32 *);

i32 BLASFUNC(sgetf2)(i32 *, i32 *, float  *, i32 *, i32 *, i32 *);
i32 BLASFUNC(dgetf2)(i32 *, i32 *, double *, i32 *, i32 *, i32 *);
i32 BLASFUNC(qgetf2)(i32 *, i32 *, double *, i32 *, i32 *, i32 *);
i32 BLASFUNC(cgetf2)(i32 *, i32 *, float  *, i32 *, i32 *, i32 *);
i32 BLASFUNC(zgetf2)(i32 *, i32 *, double *, i32 *, i32 *, i32 *);
i32 BLASFUNC(xgetf2)(i32 *, i32 *, double *, i32 *, i32 *, i32 *);

i32 BLASFUNC(sgetrf)(i32 *, i32 *, float  *, i32 *, i32 *, i32 *);
i32 BLASFUNC(dgetrf)(i32 *, i32 *, double *, i32 *, i32 *, i32 *);
i32 BLASFUNC(qgetrf)(i32 *, i32 *, double *, i32 *, i32 *, i32 *);
i32 BLASFUNC(cgetrf)(i32 *, i32 *, float  *, i32 *, i32 *, i32 *);
i32 BLASFUNC(zgetrf)(i32 *, i32 *, double *, i32 *, i32 *, i32 *);
i32 BLASFUNC(xgetrf)(i32 *, i32 *, double *, i32 *, i32 *, i32 *);

i32 BLASFUNC(slaswp)(i32 *, float  *, i32 *, i32 *, i32 *, i32 *, i32 *);
i32 BLASFUNC(dlaswp)(i32 *, double *, i32 *, i32 *, i32 *, i32 *, i32 *);
i32 BLASFUNC(qlaswp)(i32 *, double *, i32 *, i32 *, i32 *, i32 *, i32 *);
i32 BLASFUNC(claswp)(i32 *, float  *, i32 *, i32 *, i32 *, i32 *, i32 *);
i32 BLASFUNC(zlaswp)(i32 *, double *, i32 *, i32 *, i32 *, i32 *, i32 *);
i32 BLASFUNC(xlaswp)(i32 *, double *, i32 *, i32 *, i32 *, i32 *, i32 *);

i32 BLASFUNC(sgetrs)(char *, i32 *, i32 *, float  *, i32 *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(dgetrs)(char *, i32 *, i32 *, double *, i32 *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(qgetrs)(char *, i32 *, i32 *, double *, i32 *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(cgetrs)(char *, i32 *, i32 *, float  *, i32 *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(zgetrs)(char *, i32 *, i32 *, double *, i32 *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(xgetrs)(char *, i32 *, i32 *, double *, i32 *, i32 *, double *, i32 *, i32 *);

i32 BLASFUNC(sgesv)(i32 *, i32 *, float  *, i32 *, i32 *, float *, i32 *, i32 *);
i32 BLASFUNC(dgesv)(i32 *, i32 *, double *, i32 *, i32 *, double*, i32 *, i32 *);
i32 BLASFUNC(qgesv)(i32 *, i32 *, double *, i32 *, i32 *, double*, i32 *, i32 *);
i32 BLASFUNC(cgesv)(i32 *, i32 *, float  *, i32 *, i32 *, float *, i32 *, i32 *);
i32 BLASFUNC(zgesv)(i32 *, i32 *, double *, i32 *, i32 *, double*, i32 *, i32 *);
i32 BLASFUNC(xgesv)(i32 *, i32 *, double *, i32 *, i32 *, double*, i32 *, i32 *);

i32 BLASFUNC(spotf2)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(dpotf2)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(qpotf2)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(cpotf2)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(zpotf2)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(xpotf2)(char *, i32 *, double *, i32 *, i32 *);

i32 BLASFUNC(spotrf)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(dpotrf)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(qpotrf)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(cpotrf)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(zpotrf)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(xpotrf)(char *, i32 *, double *, i32 *, i32 *);

i32 BLASFUNC(slauu2)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(dlauu2)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(qlauu2)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(clauu2)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(zlauu2)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(xlauu2)(char *, i32 *, double *, i32 *, i32 *);

i32 BLASFUNC(slauum)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(dlauum)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(qlauum)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(clauum)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(zlauum)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(xlauum)(char *, i32 *, double *, i32 *, i32 *);

i32 BLASFUNC(strti2)(char *, char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(dtrti2)(char *, char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(qtrti2)(char *, char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(ctrti2)(char *, char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(ztrti2)(char *, char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(xtrti2)(char *, char *, i32 *, double *, i32 *, i32 *);

i32 BLASFUNC(strtri)(char *, char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(dtrtri)(char *, char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(qtrtri)(char *, char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(ctrtri)(char *, char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(ztrtri)(char *, char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(xtrtri)(char *, char *, i32 *, double *, i32 *, i32 *);

i32 BLASFUNC(spotri)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(dpotri)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(qpotri)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(cpotri)(char *, i32 *, float  *, i32 *, i32 *);
i32 BLASFUNC(zpotri)(char *, i32 *, double *, i32 *, i32 *);
i32 BLASFUNC(xpotri)(char *, i32 *, double *, i32 *, i32 *);

#ifdef __cplusplus
}
#endif

#endif
