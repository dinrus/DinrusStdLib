#include "RuCore.h"

extern "C"{
	
	дво  акос(дво x){return std::acos(x);}
    плав   акосп(плав x){return acosf(x);}
    реал    акоср(реал x){return acosl(x);}

    дво  асин(дво x){return std::asin(x);}
    плав   асинп(плав x){return asinf(x);}
    реал    асинр(реал x){return asinl(x);}

    дво  атан(дво x){return std::atan(x);}
    плав   атанп(плав x){return atanf(x);}
    реал    атанр(реал x){return atanl(x);}

    дво  атан2(дво y, дво x){return std::atan2(y, x);}
    плав   атан2п(плав y, плав x){return atan2f(y, x);}
    реал    атан2р(реал y, реал x){return atan2l(y, x);}

    дво  кос(дво x){return std::cos(x);}
    плав   косп(плав x){return cosf(x);}
    реал    коср(реал x){return cosl(x);}

    дво  син(дво x){return std::sin(x);}
    плав   синп(плав x){return sinf(x);}
    реал    синр(реал x){return sinl(x);}

    дво  тан(дво x){return std::tan(x);}
    плав   танп(плав x){return tanf(x);}
    реал    танр(реал x){return tanl(x);}

    дво  акосг(дво x){return std::acosh(x);}
    плав   акосгп(плав x){return acoshf(x);}
    реал    акосгр(реал x){return acoshl(x);}

    дво  асинг(дво x){return std::asinh(x);}
    плав   асингп(плав x){return asinhf(x);}
    реал    асингр(реал x){return asinhl(x);}

    дво  атанг(дво x){return std::atanh(x);}
    плав   атангп(плав x){return atanhf(x);}
    реал    атангр(реал x){return atanhl(x);}

    дво  косг(дво x){return std::cosh(x);}
    плав   косгп(плав x){return coshf(x);}
    реал    косгр(реал x){return coshl(x);}

    дво  синг(дво x){return std::sinh(x);}
    плав   сингп(плав x){return sinhf(x);}
    реал    сингр(реал x){return sinhl(x);}

    дво  танг(дво x){return std::tanh(x);}
    плав   тангп(плав x){return tanhf(x);}
    реал    тангр(реал x){return tanhl(x);}

    дво  эксп(дво x){return std::exp(x);}
    плав   экспп(плав x){return expf(x);}
    реал    экспр(реал x){return expl(x);}

    дво  эксп2(дво x){return std::exp2(x);}
    плав   эксп2п(плав x){return std::exp2f(x);}
    реал    эксп2р(реал x){return std::exp2l(x);}

    дво  экспм1(дво x){return std::expm1(x);}
    плав  экспм1п(плав x){return std::expm1f(x);}
    реал    экспм1р(реал x){return std::expm1l(x);}

    дво  фрэксп(дво знач, цел* exp){return std::frexp(знач, exp);}//
    плав   фрэкспп(плав знач, цел* exp){return frexpf(знач, exp);}//
    реал   фрэкспр(реал знач, цел* exp){return frexpl(знач, exp);}//!!!!!!!!!!!!!!!!!!

    цел     илогб(дво x){return ilogb(x);}
    цел     илогбп(плав x){return ilogbf(x);}
    цел     илогбр(реал x){return ilogbl(x);}

    дво  лдэксп(дво x, цел exp){return ldexp(x, exp);}
    плав   лдэкспп(плав x, цел exp){return ldexpf(x, exp);}
    реал    лдэкспр(реал x, цел exp){return ldexpl(x, exp);}

    дво  лог(дво x){return log(x);}
    плав   логп(плав x){return logf(x);}
    реал    логр(реал x){return logl(x);}

    дво  лог10(дво x){return log10(x);}
    плав   лог10п(плав x){return log10f(x);}
    реал    лог10р(реал x){return log10l(x);}

    дво  лог1п(дво x){return log1p(x);}
    плав   лог1пп(плав x){return log1pf(x);}
    реал    лог1пр(реал x){return log1pl(x);}

    дво  лог2(дво x){return log2(x);}
    плав   лог2п(плав x){return log2f(x);}
    реал    лог2р(реал x){return log2l(x);}

    дво  логб(дво x){return logb(x);}
    плав   логбп(плав x){return logbf(x);}
    реал    логбр(реал x){return logbl(x);}

    дво  модф(дво значение, дво* цук){return modf(значение, цук);}
    плав   модфп(плав значение, плав* цук){return modff(значение, цук);}
    реал    модфр(реал значение, реал *цук){return modfl(значение, цук);}

    дво  скалбн(дво x, цел n){return scalbn(x, n);}
    плав   скалбнп(плав x, цел n){return scalbnf(x, n);}
    реал    скалбнр(реал x, цел n){return scalbnl(x, n);}
}