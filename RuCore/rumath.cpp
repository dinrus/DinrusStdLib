#include "RuCore.h"

extern "C"{

    дво  кубкор(дво x){return cbrt(x);}
    плав   кубкорп(плав x){return cbrtf(x);}
    реал    кубкорр(реал x){return cbrtl(x);}

    дво  фабс(дво x){return fabs(x);}
    плав   фабсп(плав x){return fabsf(x);}
    реал    фабср(реал x){return fabsl(x);}

    дво  гипот(дво x, дво y){return hypot(x, y);}
    плав   гипотп(плав x, плав y){return hypotf(x, y);}
    реал    гипотр(реал x, реал y){return hypotl(x, y);}

    дво  степ(дво x, дво y){return pow(x, y);}
    плав   степп(плав x, плав y){return powf(x, y);}
    реал    степр(реал x, реал y){return powl(x, y);}

    дво  квкор(дво x){return sqrt(x);}
    плав   квкорп(плав x){return sqrtf(x);}
    реал    квкорр(реал x){return sqrtl(x);}

    дво  фцош(дво x){return erf(x);}
    плав   фцошп(плав x){return erff(x);}
    реал    фцошр(реал x){return erfl(x);}

    дво  фцошк(дво x){return erfc(x);}
    плав   фцошкп(плав x){return erfcf(x);}
    реал    фцошкр(реал x){return erfcl(x);}

    дво  лгамма(дво x){return lgamma(x);}
    плав   лгаммап(плав x){return lgammaf(x);}
    реал    лгаммар(реал x){return lgammal(x);}

    дво  тгамма(дво x){return tgamma(x);}
    плав   тгаммап(плав x){return tgammaf(x);}
    реал   тгаммар(реал x){return tgammal(x);}

    дво  вокругли(дво x){return ceil(x);}
    плав   вокруглип(плав x){return ceilf(x);}
    реал    вокруглир(реал x){return ceill(x);}

    дво  нокругли(дво x){return floor(x);}
    плав   нокруглип(плав x){return floorf(x);}
    реал    нокруглир(реал x){return floorl(x);}

    дво  ближцел(дво x){return nearbyint(x);}
    плав   ближцелп(плав x){return nearbyintf(x);}
    реал    ближцелр(реал x){return nearbyintl(x);}

    дво  ринт(дво x){return rint(x);}
    плав   ринтп(плав x){return rintf(x);}
    реал    ринтр(реал x){return rintl(x);}
    
    дво  округли(дво x){return std::round(x);}
    плав   округлип(плав x){return std::roundf(x);}
    реал    округлир(реал x){return std::roundl(x);}

    дво  транк(дво x){return trunc(x);}
    плав   транкп(плав x){return truncf(x);}
    реал    транкр(реал x){return truncl(x);}

    дво  фмод(дво x, дво y){return fmod(x, y);}
    плав   фмодп(плав x, плав y){return fmodf(x, y);}
    реал    фмодр(реал x, реал y){return fmodl(x, y);}

    дво  остаток(дво x, дво y){return remainder(x, y);}
    плав   остатокп(плав x, плав y){return remainderf(x, y);}
    реал    остатокр(реал x, реал y){return remainderl(x, y);}

    дво  ремкво(дво x, дво y, цел* quo){return remquo(x, y, quo);}
    плав   ремквоп(плав x, плав y, цел* quo){return remquof(x, y, quo);}
    реал    ремквор(реал x, реал y, цел* quo){return remquol(x, y, quo);}

    дво  копируйзнак(дво x, дво y){return copysign(x, y);}
    плав   копируйзнакп(плав x, плав y){return copysignf(x, y);}
    реал    копируйзнакр(реал x, реал y){return copysignl(x, y);}

    дво  нечисло(ткст tangp){return nan(tangp);}
    плав   нечислоп(ткст tangp){return nanf(tangp);}
    реал    нечислор(ткст tangp){return nanl(tangp);}

    дво  следза(дво x, дво y){return nextafter(x, y);}
    плав   следзап(плав x, плав y){return nextafterf(x, y);}
    реал    следзар(реал x, реал y){return nextafterl(x, y);}

    дво  следк(дво x, реал y){return nexttoward(x, y);}
    плав   следкп(плав x, реал y){return nexttowardf(x, y);}
    реал    следкд(реал x, реал y){return nexttowardl(x, y);}
    
  /* Комплексные числа из Ди требуют справки))) для Си
кдво какос(кдво z){return cacos(z);}
кплав  какосп(кплав z){return cacosf(z);}
креал   какоср(креал z){return cacosl(z);}

кдво касин(кдво z){return casin(z);}
кплав  касинп(кплав z){return casinf(z);}
креал   касинр(креал z){return casinl(z);}

кдво катан(кдво z){return catan(z);}
кплав  катанп(кплав z){return catanf(z);}
креал   катанр(креал z){return catanl(z);}

кдво ккос(кдво z){return ccos(z);}
кплав  ккосп(кплав z){return ccosf(z);}
креал   ккоср(креал z){return ccosl(z);}

кдво ксин(кдво z){return core.stdc.complex.csin(z);}
кплав  ксинп(кплав z){return csinf(z);}
креал   ксинр(креал z){return csinl(z);}

кдво ктан(кдво z){return ctan(z);}
кплав  ктанп(кплав z){return ctanf(z);}
креал   ктанр(креал z){return ctanl(z);}

кдво какосг(кдво z){return cacosh(z);}
кплав  какосгп(кплав z){return cacoshf(z);}
креал   какосгр(креал z){return cacoshl(z);}

кдво касинг(кдво z){return casinh(z);}
кплав  касингп(кплав z){return casinhf(z);}
креал   касингр(креал z){return casinhl(z);}

кдво катанг(кдво z){return catanh(z);}
кплав  катангп(кплав z){return catanhf(z);}
креал   катангр(креал z){return catanhl(z);}

кдво ккосг(кдво z){return ccosh(z);}
кплав  ккосгп(кплав z){return ccoshf(z);}
креал   ккосгр(креал z){return ccoshl(z);}

кдво ксинг(кдво z){return csinh(z);}
кплав  ксингп(кплав z){return csinhf(z);}
креал   ксингр(креал z){return csinhl(z);}

кдво ктанг(кдво z){return ctanh(z);}
кплав  ктангп(кплав z){return ctanhf(z);}
креал   ктангр(креал z){return ctanhl(z);}

кдво кэксп(кдво z){return cexp(z);}
кплав  кэкспп(кплав z){return cexpf(z);}
креал   кэкспр(креал z){return cexpl(z);}

кдво клог(кдво z){return clog(z);}
кплав  клогп(кплав z){return clogf(z);}
креал   клогр(креал z){return clogl(z);}

дво кабс(кдво z){return cabs(z);}
плав  кабсп(кплав z){return cabsf(z);}
реал  кабср(креал z){return cabsl(z);}

кдво кстеп(кдво x, кдво y){return cpow(x, y);}
кплав  кстепп(кплав x, кплав y){return cpowf(x, y);}
креал   кстепр(креал x, креал y){return cpowl(x, y);}

кдво кквкор(кдво z){return csqrt(z);}
кплав  кквкорп(кплав z){return csqrtf(z);}
креал   кквкорр(креал z){return csqrtl(z);}

 дво карг(кдво z){return core.stdc.complex.carg(z);}
 плав  каргп(кплав z){return cargf(z);}
 реал  каргр(креал z){return cargl(z);}

кдво кпроекц(кдво z){return core.stdc.complex.cproj(z);}
кплав  кпроекцп(кплав z){return cprojf(z);}
креал   кпроекцр(креал z){return cprojl(z);}
   */
    /*
    c_long  lrint(дво x){return lrint(x);}
    c_long  lrintf(плав x){return lrintf(x);}
    c_long  lrintl(реал x){return lrintl(x);}
*/
   // дол    llrint(дво x){return std::llrint(x);}
   // дол    llrintf(плав x){return std::llrintf(x);}
  //  дол    llrintl(реал x){return std::llrintl(x);}
  /*
    c_long  lround(дво x){return lround(x);}
    c_long  lroundf(плав x){return lroundf(x);}
    c_long  lroundl(реал x){return lroundl(x);}
*/

   // дол    llround(дво x){return llround(x);}
   // дол    llroundf(плав x){return llroundf(x);}
  //  дол    llroundl(реал x){return llroundl(x);}
 }