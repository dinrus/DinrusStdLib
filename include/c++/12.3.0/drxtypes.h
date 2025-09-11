   /***************************************************
   БАЗОВЫЕ ТИПЫ ДЛЯ ФРЕЙМВОРКА DRX И КОМПИЛЯТОРА DRUX.
   
   Здесь собраны версии алиасов часто используемых типов, которые зачастую создают
    путаницу. Типы DinrusPro сводятся к упрощению до их указания по маске
   
                     (б)цХХ(Х),   где
                                б : "беззначное",
                                ц : "целое",
                                
    далее следует указание числа бит в типе (8 16 32 64 128 и т.д.),
    которое является  кратным числу 2. Два здесь берётся из того, что
   "сигнал либо есть", т.е. равен 1 (единице),
   либо
   "сигнала нет", т.е. равен 0 (нулю).

   Конечно, наш мир определённо имеет некий "досигнальный" уровень, но какие бы
   "кварки", "лептоны", "фотоны" не сочинялись, все они тотчас же становятся объектами
    "сигнального" уровня, поэтому играться в такие определения БЕССМЫСЛЕННО!

   Весь наш мир "энергетический", состоит из областей с давлением, и областей
   без давления, т.е. "сингулярных" или "нулярных", короче "единицевых" и "нолевых",
   а ещё вернее:
 
    СИГНАЛЬНЫХ со знаком "минус" (0), которые "приёмники",
                  так как разрежены, с низким давлением
    и
    СИГНАЛЬНЫХ со знаком "плюс" (1), которые "передатчики",
                   то бишь уплотнены и с высоким давлением.
                   
    Запомните - это ДАВЛЕНИЕ, которое именно и порождают СИГНАЛ, фиксируемый на приёмнике,
    а вовсе не БОГ!!!!!! Два сигнала о давлении, т.е. его НАПРАВЛЕННОСТИ,
    или, иначе говоря,- ПОЛЮСНОСТИ, ПОЛЯРНОСТИ...В общем, от этого, т.е. от "ЭНЕРГИИ"
    и возникает ДВИЖЕНИЕ, которое измеряется эталонами, такими как ВРЕМЯ,
    НАПРАВЛЕНИЕ, МАССА и прочее.
 
    (Кто там ещё "путешествует" по времени?!!! -
    Вот ещё один такой же риторический вопрос.)
 
    В МС (Метанаучном Синкретизме) это "Теория Энергонов" - она об этих самых элементарных
    досигнального уровня "элементах", из свойств которых происходит весь наш -
    сигнального уровня - МИР.

    Собственно, всё программирование также состоит в практическом использовании
    ТЕОРИИ ЭНЕРГОНОВ)))
    
    *****Виталий Кулич, 19 сент. 2024 г.*****
    
    *********************************************/
    

#ifndef _drx_types_h_
#define _drx_types_h_
#ifndef  SQLITE_CORE

#pragma once

#include <features.h>

#ifndef _LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER
#  pragma GCC system_header
#endif

#ifndef DINRUSPRO
#define MINIDINRUSPRO 1
#endif

#if defined __x86_64__ && !defined __ILP32__
# define WORDSIZE   64
#else
# define WORDSIZE   32
#define WORDSIZE32_SIZE_ULONG       0
#define WORDSIZE32_PTRDIFF_LONG     0
#endif

#ifdef __x86_64__
# define WORDSIZE_TIME64_COMPAT32   1
/* Как x86-64, так и x32 используют 64-битный системный интерфейс вызовов.  */
# define SYSCALL_WORDSIZE           64
#else
# define WORDSIZE_TIME64_COMPAT32   0
#endif

#ifndef MINIDINRUSPRO
# if WORDSIZE == 64
#  define INT64_C(c)    c ## L
#  define UINT64_C(c)   c ## UL
# else
#  define INT64_C(c)    c ## LL
#  define UINT64_C(c)   c ## ULL
# endif
#endif
/*********************************************
          Базовые типы РНЦП "Динрус"
          
/*
 * bool
 *      Булево значение, либо true, либо false.
 *
 * Если имеется stdbool.h, используется он, и там у bool размер 1. Это улучшает
 * вывод от компилятора и отладчика и совместимость со сторонними библиотеками.
 *
 * Что касается компиляторов C++, то у них есть совместимый встроенный тип bool.
 */
#ifndef __bool_defined
#ifndef __cplusplus
//Ниже даётся определение булева типа для языка Си.
#ifdef __clang__
//То есть нужно включить этот макрос вручную,
// через опцию -D USE_STDBOOL, если файл,
// указанный ниже, действительно "присутствует".
#include <stdbool.h>
#else
#ifndef bool
typedef unsigned char bool;
#endif

#ifndef true
#define true    ((bool) 1)
#endif

#ifndef false
#define false   ((bool) 0)
#endif

#endif
#endif
#define __bool_true_false_are_defined 1
  //Булев тип.
#define                     бул bool
#define __bool_defined           1
#endif
/*
* Важно понимать, что под знак отводится 1 бит из 8, 16, 32 или 64 и т.д. (!),
* то есть ёмкость* типа уменьшается; в беззначный вариант помещается больше
* вариантов кодирования, но указание знака (некоего направления)
* сводится на нет.
*
* Определение общеиспользуемых типов.
* Удобно тем, что "ц8" можно настроить на 1 байт (8 битов) на всех платформах.
* Так же и более важно то, что "ц32" можно настроить на 4 байта (32 бита)
* на всех платформах.
*/
// С точки зрения стандартного C++, таких типов 14:
//   * ЦЧ-типы
//         bool                         (1)
//         char, signed char, unsigned char, wchar_t            (4)
//         short, unsigned short                (2)
//         int, unsigned                        (2)
//         long, unsigned long                  (2)
//
//   * ПЗ-типы
//         float                        (1)
//         double                       (1)
//         long double                  (1)
//
// GNU C++ добавляет (если это поддерживается хостовой библиотекой Си)
//   * ЦЧ-типы
//         long long, unsigned long long            (2)
//
// что приводит нас к 16-ти фундаментальным типам данных в GNU C++.
/*
typedef bool          bool_t;//1
typedef char          char_t; //2
typedef signed char   byte_t; //3
typedef unsigned char ubyte_t; //4
typedef unsigned long ul_t;   //5
typedef signed long   l_t;   //6
typedef unsigned long long   ull_t;  //7
typedef signed long long     ll_t; //8
typedef int                  inti_t; //9
typedef unsigned              u_t; //10
typedef double            d_t;  //11
typedef float             f_t; //12
typedef long double       ld_t; //13
typedef short             sh_t; //14
typedef unsigned short    ush_t; //15

//Это нестандартные определения (псевдотипы)
#define ulli_t        unsigned long long int // = ull_t
#define lli_t         signed long long int // = ll_t
*/
 //ТИП Реал
#undef __real_t_defined
#ifndef __real_t_defined
typedef long double             real_t;
#define             реал        real_t
#define __real_t_defined          1
#endif

//Типы в 128 бит
#if defined __GNUC__ && defined __SIZEOF_INT128__
#define DRX_HAS_INT128      1 // Можно использовать 128-битные ЦЧ.

typedef unsigned __int128       u128;
typedef          __int128       i128;

#define                   бц128   u128
#define                    ц128   i128

#define МИНЦ128   ((i128)1 << 127)
#define МАКСЦ128   (~МИНЦ128)
#define МИНБЦ128  ((u128)0)
#define МАКСБЦ128  (~МИНБЦ128)
#define __int128_t_defined        1
//#warning На этой машине допускается использование типа i128 | u128 !
#endif

 
  //ТИП "Байт (8 бит) со знаком"
#undef __int8_t_defined
#ifndef __int8_t_defined
typedef  signed char              int8_t;    ///Соответствует int8_t
typedef  signed char                  i8;
 //Депрекировап!!! : #define                   int8   int8_t
  //Алиасы ЦЧ-типа i8 со знаком в 1 байт (8 битов).
#define                  ц8     i8
//typedef i8                    байт; ///Этот тип-алиас раздавлен))) См. ниже.
//typedef i8                    byte;
 #define __int8_t_defined        1
#endif

  //ТИП "Байт 8 бит) без знака"
#undef __uint8_t_defined
#ifndef __uint8_t_defined
typedef  unsigned char          uint8_t;      ///Соответствует uint8_t
typedef  unsigned char               u8;
//Депрекировап!!! : #define                   uint8 uint8_t
  //Алиасы ЦЧ-типа i8 без знака в 1 байт (8 битов).
#define                   бц8   u8
#define                   ббайт u8
#define                   ubyte u8 //склешился с типом byte...

 #define __uint8_t_defined        1
#endif

//Специальный тип в Си++, расширяющий тип u8.
//#undef __byte_defined
#ifndef __byte_defined

#ifdef __cplusplus

namespace std
{
  enum class byte : u8 {};
}//endns std

using byte = std::byte;

#else
   
    #define        byte u8
#endif
    #define      байт byte
 #define __byte_defined           1
 
#endif

  //ТИП "2 байта (16 бит) со знаком"
#undef __int16_t_defined
#ifndef __int16_t_defined
typedef  signed short int        int16_t;    ///Соответствует int16_t
typedef  signed short int            i16;
//Депрекировап!!! : #define                    int16 int16_t
  
  //Алиасы ЦЧ-типа i16 со знаком в 2 байта (16 битов).
#define                    ц16    i16
#define                    крат   i16

 #define __int16_t_defined       1
#endif

  //ТИП "2 байта (16 бит) без знака"
#undef __uint16_t_defined
#ifndef __uint16_t_defined
typedef  unsigned short int      uint16_t;     ///Соответствует uint16_t
typedef  unsigned short int           u16;
//Депрекировап!!! : #define                  uint16  uint16_t

  //Алиасы ЦЧ-типа i16 без знака в 2 байта (16 битов).
#define                   бц16   u16
#define                   бкрат  u16
#define                   word   u16
#define                   ushort u16
 #define __uint16_t_defined       1
#endif

  //ТИП "4 байта (32 бита) со знаком"
#undef __int32_t_defined
#ifndef __int32_t_defined
typedef  signed int               int32_t;      ///Соответствует int32_t
typedef  signed int                   i32;
//Депрекировап!!! : #define int32   int32_t
  //Алиасы ЦЧ-типа i32 со знаком в 4 байта (32 бита).
  //Он же просто int!)))
#define                     ц32   i32
#define                     цел   i32
#define __int32_t_defined        1
#endif

  //ТИП "4 байта (32 бита) без знака"
#undef __uint32_t_defined
#ifndef __uint32_t_defined
typedef  unsigned int            uint32_t;      ///Соответствует uint32_t
typedef  unsigned int                 u32;
//Депрекировап!!! : #define   uint32 uint32_t
  //Алиасы ЦЧ-типа ui32 со знаком в 4 байта (32 бита).
#define                    бц32   u32
#define                    бцел   u32
#define                    dword  u32
//Депрекировап!!! : #define                    uint  uint32_t
#define __uint32_t_defined       1
#endif

  //ТИП "8 байтов (64 бита) cо знаком"

#ifdef PLATFORM_WIN32
typedef signed __int64           i64;    ///Соответствует int64_t
#else
typedef  signed long int         i64;
#endif

  //ТИП "8 байтов (64 бита) без знака"
#ifdef PLATFORM_WIN32
typedef unsigned __int64         u64;  ///Соответствует uint64_t
#else
typedef  unsigned long int       u64;
#endif


/* Есть некоторые недоразумения с типами ц64 и ц128:
* Первый тип "провозглашается" как long int и в таком виде используется, даже когда
* WORDSIZE == 64. Но есть его другое определение, как long long int, и оно намекает на ц128,
* но представляет собой, опять-таки, ц64 (((
* Нужно определить макрос GNUC_EXTENSION, как алиас __extension__,
* чтобы компилятор использовал расширенное понимание типа... и не жаловался (такой совет).
* Говорят, что "long long" типы будут проходить даже в присутствии '-ansi -pedantic'.
*/

#if  __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#define GNUC_EXTENSION __extension__
#define DRX_EXENDED 1
#else
#define GNUC_EXTENSION
#define DRX_EXENDED 0
#endif

typedef signed long long int   z64;
typedef unsigned long long int zu64;
#define   ц64_2    z64
#define   бц64_2   zu64

#ifndef MINIDINRUSPRO
#if WORDSIZE == 64
typedef  signed long  int           int64_t; //= i64
typedef  unsigned long  int         uint64_t; //= u64
#else
typedef  signed long long  int      int64_t; //= z64
typedef  unsigned long long  int    uint64_t; //= zu64
#endif

//Депрекировап!!! : #define   int64  int64_t
//Депрекировап!!! : #define   uint64 uint64_t

  //Алиасы ЦЧ-типа ui64 без знака в 8 байтов (64 бита).
#define                    бц64   u64
#define                    бдол   u64
#define                    qword  u64
//Депрекировап!!! : #define    ulong  u64

  //Алиасы ЦЧ-типа i64 со знаком в 8 байтов (64 бита).
#define                     ц64   i64
#define                     дол   i64

#define __int64_t_defined        1
#define __uint64_t_defined       1
#endif


  //Алиасы ПЗ-типов (типов с плавающей запятой).
#define                   дво  double
#define                   плав float

  //Определение NULL.
#ifdef __cplusplus
    #define NULL nullptr
#else
     #define NULL 0
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
#ifndef __nullptr_t_defined
  typedef decltype(nullptr) nullptr_t;
  #define __nullptr_t_defined 1
#endif
#endif

//Определения константных базовых типов.

typedef const i8             i8k;
typedef const u8             u8k;
typedef const i16            i16k;
typedef const u16            u16k;
typedef const i32            i32k;
typedef const u32            u32k;
typedef const i64            i64k;
typedef const u64            u64k;
typedef const z64            z64k;
typedef const zu64           zu64k;

//Русские алиасы.

#define                  ц8к     i8k
#define                  бц8к    u8k
#define                  ц16к    i16k
#define                  бц16к   u16k
#define                  ц32к    i32k
#define                  бц32к   u32k
#define                  ц64к    i64k
#define                  бц64к   u64k
#define                  ц64_2к  z64k
#define                  бц64_2к zu64k

#ifndef MINIDINRUSPRO
///////////////////////////////////////////////////
// Смещение члена MEMBER в структуре типа TYPE.
#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)
#define _смещ_у offsetof

#ifndef __max_align_t_defined
#ifndef GNULIB_defined_max_align_t
// Типу, алиновка которого поддерживается в любом контексте и также
  // велика, как у любого стандартного типа, не требуется ее
   //спецификаторов (указателей).
   
typedef struct {
  z64  __max_align_ll __attribute__((__aligned__(__alignof__(z64))));
  real_t __max_align_ld __attribute__((__aligned__(__alignof__(real_t))));
  // _Float128 определён как базовый класс, поэтому max_align_t должен
    // быть достаточен для его алиновки. Этот код должен работать в C++,
    // поэтому используется здесь __float128; он доступен только на
    // некоторых архитектурах, но дополнительная алиновка нужна только на i386
    // для этого __float128.
#ifdef __i386__
  __float128 __max_align_f128 __attribute__((__aligned__(__alignof(__float128))));
#endif
} max_align_t;
#define _GCC_MAX_ALIGN_T 1
#define __max_align_t_defined 1
#endif
#endif

#define  т_макс_алин max_align_t
/////////////////////////////////////////////
#endif

#include <stdio.h>

  //Указатель на файл - для краткости - в отдельный тип
#define ФАЙЛ                    FILE
typedef FILE*                   fuk;
#define                    фук  fuk


  //Процедура
#define                    проц void

  //Указатель
typedef void*                   uk;
#define                    ук   uk

  //Константный указатель
typedef const void*             ukk;
#define                    укк  ukk

  // Нулевой указатель по-русски.
#define                    нук nullptr

//Размеры типов определяются значно и беззначно.
//В английском варианте метится значное (signed),
//т.е. число со знаком + (плюс) или знаком - (минус).
//Знак несёт в себе информацию о направлении.
//

#if WORDSIZE == 64
typedef i64                         ptrdiff_t;
typedef u64                         size_t;
typedef i64                         ssize_t;
#else
#endif

#define  __ssize_t_defined      1
#define  __size_t_defined       1
#define  __ptrdiff_t_defined    1
#define razm                    ssize_t
#define brazm                   size_t
#define                    разм   razm
#define                    бразм  brazm
#define                    т_мера size_t

 //Тип беззначного ЦЧ-указателя
 #ifndef __uintptr_t_defined
typedef unsigned long          uintptr_t;
#define                uiuk    uintptr_t
#define                бцук    uintptr_t
#define __uintptr_t_defined 1
#endif

 //Тип значного ЦЧ-указателя
 #ifndef __intptr_t_defined
typedef long int              intptr_t;
#define               iuk     intptr_t
#define               цук     intptr_t
#define __intptr_t_defined 1
#endif

/*
  //Специальный тип в Си++, расширяющий тип size_t
#ifdef __cplusplus || not defined (_G_EXPECTED_CXX_STANDARD="98")
enum class align_val_t : size_t {};

  // Тривиальная структура nullopt_t.
struct nullopt_t {
  inline constexpr explicit nullopt_t() = default;
};

// nullopt, который можно использовать и возвращать.
inline constexpr nullopt_t nullopt{};
#endif
*/

  //Символ в формате ASCII.
#define                     сим char

  //Константный текстовый указатель
  //на тип char (сим).

typedef const char*             tukk;
#define                    тукк tukk
  //Неконстантный текстовый указатель
    //на тип char (сим).
typedef char*                   tuk;
#define                    тук  tuk

 //Двойной указатель на текст
 
#define   tuk2    tuk*
#define   tukk2   tukk*

/**
* В библиотеке Glib: gunichar:
*
*  Тип, вмещающий код любого символа UTF-32 или UCS-4, также
*  известный как Unicode code point (кодовая точка Юникода).
*
*  Если нужно произвести представление в UTF-8 для #gunichar,
*  используется g_ucs4_to_utf8(). Смотрите также g_utf8_to_ucs4()
*  для противоположного процесса.
*
*  Чтобы вывести/отсканировать значения этого типа в виде целого числа (ЦЧ),
*  используются %I32_MODIFIER и/или %UI32_FORMAT.
*
*  Нотация для выражения кодовой точки Unicode в бегущем тексте представляет
*  собой 16-ричное число с 4-6 цифрами и заглавными буквами, префиксированными
*  строкой "U+". Вводящие нули пропускаются, если в код-пойнте будет
*  менее 4-х 16-ричных цифр.
*  Например, "U+0041 ЛАТИНСКАЯ ЗАГЛАВНАЯ БУКВА A". Для вывода код-пойнта
*  в нотации U+, используется строка формата "U+\%04"I32_FORMAT"X".
*  Для сканирования используется строка формата "U+\%06"I32_FORMAT"X".
*
*  |[
*  gunichar c;
*  sscanf ("U+0041", "U+%06"I32_FORMAT"X", &amp;c)
*  g_print ("Read U+%04"I32_FORMAT"X", c);
*  ]|
*/

  //Символ Юникода.
#undef __wchar_t_defined
#undef __WCHAR_TYPE__
#define __WCHAR_TYPE__ i32  //Здесь получаем typedef int wchar_t;
#ifndef __cplusplus
typedef __WCHAR_TYPE__  wchar_t;
#endif
#define __wchar_t_defined 1


/* Целочисленный тип, не изменяемый дефолтными argument promotions, который
   может держать любое значение, соответствующее членам расширенного
   набора символов, а также как минимум одно значение, не соответствующее ему. */
#ifndef __wint_t_defined
typedef u32          wint_t;
#define __wint_t_defined 1
#endif

//И снова в загадочном <stddef.h> какая-то несуразица!
//Здесь нам приходится вместо wchar_t использовать wint_t !
//Более того, заглянув в <ncurses.h>, обнаруживаем, что
// wchar_t - это по-нашему u16.
// В самом UPP же он изначально был определён как u32 (что сходится с stddef.h),
// Но по логике, тип wint_t соотносим с i32, а wchar_t - c u32,
// а в реале - всё наоборот!
#if __wchar_t_defined
typedef __WCHAR_TYPE__   wchar;
#define         шим    wchar
#define         сим32  wchar
#define         char32 wchar
#define         wch    wchar
  //Текстовый указатель на тип wchar:
      // "ШирокоТекстовый УКазатель"
typedef wchar*        wtuk;
#define         штук  wtuk

  //Константный текстовый указатель на тип wchar.
typedef const wchar*  wtukk;
#define        штукк  wtukk
#endif

#if __wint_t_defined
typedef wint_t    wint;
#endif


/**
*  В библиотеке Glib: gunichar2:
*
*  Тип, вмещающий любой код-пойнт в кодировке UTF-16.
*  В UTF-16 также есть так называемые <firstterm>суррогатные пары</firstterm>,
*  используемые для кодирования символов, находящихся вне пределов BMP,
*  в виде пар из 16-битных чисел. Суроганые пары нельзя хранить в
*  в одном поле gunichar2, но все функции библиотеки GLib, принимающие
* массивы из gunichar2, корректно интерпретируют суррогатные пары.
*
*  Для вывода/сканирования значений этого типа в/из текста нужно преобразовать
*  в/из UTF-8, используя g_utf16_to_utf8()/g_utf8_to_utf16().
*
*  Для вывода/сканирования значений этого типа как ЦЧ ("integer"), используется
*  %I16_MODIFIER и/или %UI16_FORMAT.
*/

  //Символ в кодировке UTF-16
#ifdef PLATFORM_WIN32
typedef WCHAR      char16;
#else
typedef u16        char16;
#endif

# define   сим16 char16
/*
// Максимальная длина многобайтного символа в текущей локали.
#define MB_CUR_MAX  (__ctype_get_mb_cur_max ())
extern size_t __ctype_get_mb_cur_max (void);
*/
#ifndef MINIDINRUSPRO
// Стандарт posix требует от sa_family_t только, чтобы она была беззначной.
// linux man page для "address_families" перечисляет около 32 разных семейств
// адресов, намекая, что короткое 16-битное число может вместить их все.

    typedef u32     socklen_t;
         #define __socklen_t_defined 1
         
    typedef u16     sa_family_t;
    typedef u8      cc_t;
    
    typedef razm     clock_t;
        #define __clock_t_defined 1
    
    typedef i32      clockid_t;
    typedef u16     fexcept_t;
    typedef i32      sig_atomic_t;
    typedef u32     mode_t;
    # define __mode_t_defined 1
    typedef u32     pthread_key_t;

    typedef u32      tss_t;
    typedef u32      speed_t;
    typedef u32      tcflag_t;
                            
    typedef intptr_t  blkcnt_t;
    #define __blkcnt_t_defined 1
                             
    typedef intptr_t  blksize_t;
    #define __blksize_t_defined 1
                            
    typedef u64      dev_t;
    #define __dev_t_defined 1
    
    typedef u32      gid_t;
    #define __gid_t_defined 1
    
    typedef uintptr_t ino_t;
    #define __ino_t_defined 1
    
    typedef uintptr_t nlink_t;
    #define __nlink_t_defined 1
     
    typedef i64       off64_t;
    #define __off64_t_defined 1
    
    typedef i64       off_t;
        #define __off_t_defined 1
        
    typedef i32       pid_t;
        #define __pid_t_defined 1
        
    typedef u32      uid_t;
    #define __uid_t_defined 1
    
    typedef i64       suseconds_t;
    #define __suseconds_t_defined 1
    
/* Определение типа, используемого с 'error_t'. */

        typedef i32 error_t;
        #define __error_t_defined 1

//////////////////

/* Малые типы.  */

/* Со знаком.  */
typedef i8  int_least8_t;
typedef i16 int_least16_t;
typedef i32 int_least32_t;
typedef i64 int_least64_t;

/* Без знака.  */
typedef u8  uint_least8_t;
typedef u16 uint_least16_t;
typedef u32 uint_least32_t;
typedef u64 uint_least64_t;


/* Быстрые типы.  */

/* Со знаком.  */
typedef i8              int_fast8_t;
#if WORDSIZE == 64
typedef i64             int_fast16_t;
typedef i64             int_fast32_t;
typedef i64             int_fast64_t;
#else
typedef i32             int_fast16_t;
typedef i32             int_fast32_t;
#endif

#if WORDSIZE == 64 && DRX_EXTENDED == 1
__extension__ typedef z64  int_fast64_t;
#endif

/* Без знака.  */
typedef u8             uint_fast8_t;
#if WORDSIZE == 64
typedef u64            uint_fast16_t;
typedef u64            uint_fast32_t;
typedef u64            uint_fast64_t;
#else
typedef u32            uint_fast16_t;
typedef u32            uint_fast32_t;
#endif

#if WORDSIZE == 64 && DRX_EXTENDED == 1
__extension__ typedef zu64 uint_fast64_t;
#endif

/* Largest integral types.  */
#if WORDSIZE == 64 && DRX_EXTENDED == 0
typedef i64 intmax_t;
typedef u64 uintmax_t;
#else
__extension__ typedef z64  intmax_t;
__extension__ typedef zu64 uintmax_t;
#endif
#endif
/////////////////////////////////////////////////////

 //Неверно, неистино, отрицательно.
#define  нет false

 //Верно, истино, положительно.
#define  да true

  //Аналогичные типы, используемые в языке Си.
#define НЕТ 0
#define ДА  1

#define ЭКСП extern

#ifdef __cplusplus
//Определения для видов экспорта/импорта символов.
#if defined(PLATFORM_WIN32)
#define СИЭКСП __declspec(dllexport) extern "C"
#else
#define СИЭКСП extern "C"
#endif

#if defined(PLATFORM_WIN32)
#define СИИМП __declspec(dllimport) extern "C"
#else
#define СИИМП extern "C"
#endif

#if defined(PLATFORM_WIN32)
#define СИППЭКСП __declspec(dllexport) extern "C++"
#else
#define СИППЭКСП extern "C++"
#endif

#if defined(PLATFORM_WIN32)
#define СИППИМП __declspec(dllimport) extern "C++"
#else
#define СИППИМП extern "C++"
#endif

#define НАЧСИЭКСП    extern "C" {
#define НАЧСИППЭКСП  extern "C++" {
#define КОНСИЭКСП   }
#define КОНСИППЭКСП }
#else
#define СИЭКСП   extern
#define СИППЭКСП extern "C++"
#endif

#define _безвозвр    [[noreturn]]
#define _алин_как    alignas
#define _статассёрт  static_assert
#define _алин_у      alignof
#define _нителок     thread_local
#define _безискл     noexcept
#define _рестрикт    restrict

#if (defined(__arm__) || defined(_M_ARM))
    typedef __INTPTR_TYPE__ time_t;
#else
    typedef __INT64_TYPE__ time_t;
#endif

//Определения мин и макс констант для числовых типов
// с фиксированным размером.
#define МИНЦ8   ((i8)  0x80)
#define МАКСЦ8   ((i8)  0x7f)
#define МАКСБЦ8  ((u8) 0xff)

#define МИНЦ16  ((i16)  0x8000)
#define МАКСЦ16  ((i16)  0x7fff)
#define МАКСБЦ16 ((u16) 0xffff)

#define МИНЦ32  ((i32)  0x80000000)
#define МАКСЦ32  ((i32)  0x7fffffff)
#define МАКСБЦ32 ((u32) 0xffffffff)

#define МИНЦ64  ((i64) I64_CONSTANT(0x8000000000000000))
#define МАКСЦ64  I64_CONSTANT(0x7fffffffffffffff)
#define МАКСБЦ64 I64_CONSTANT(0xffffffffffffffffU)

///////////////////// C

//#ifdef __cplusplus
//#include <limits>
//#endif

#include <stdarg.h> // встроен в clang
typedef  va_list                спис_ва;

///////////////////////////////////////////////// C++
//Определения DinrusPro
//
#ifdef __cplusplus
#ifndef DINRUSPRO
 #include <string>
  #include <vector>
#endif

namespace std
{
  using ::i8;
  using ::i16;
  using ::i32;
  using ::i64;
  using ::z64;
  
  using ::u8;
  using ::u16;
  using ::u32;
  using ::u64;
  using ::zu64;
  
  using ::real_t;
  
#if __int128_t_defined
  using ::u128;
  using ::i128;
#endif

}//endns std

#define STxt            std::string
#define SWTxt           std::wstring
#define STxtStream      std::stringstream
#define SVector         std::vector
#define STxtbuf         std::stringbuf
#define STxtview        std::string_view

#include <typeinfo>
#endif
/////////////////////////////////////////////////////////////////////////
//Во избежание перехлёста с настоящей стандартной библиотекой,
//если её использование начато раньше использования drxstdlib.
#if defined(DRXSTDLIB) //&& !defined(_STDLIB_H) //#ifdef __drux__ ?
//#define _STDLIB_H 1

//Стандартная библиотека Си (<stdlib.h>)

/* Понятие об одной статической локали на категорию не совсем продуманное.
   Во многих приложениях  потребуется обрабатывать её данные, используя
   информацию из нескольких различных локалей.  Другая проблема с реализацией
   обработки интернационализации в стандартной библиотеке ISO C++.
   Существует поддержка этого иного набора функций, использующих данные локали,
   которые принимают дополнительный аргумент.

   Внимание: при том, что несколько интерфейсов *_l являются частью POSIX:2008,
   эти функции не являются.  */

/* Структура для реэнтрантных функций, использующих локаль. Это (почти что)
   тип opaque для программ на уровне пользователя.  */
   
//#ifndef _XLOCALE_H

typedef struct __locale_struct
{
  // Заметка: LC_ALL неполноценный индекс в этом массиве.
  struct __locale_data *__locales[13]; // 13 = __LC_LAST.
  // Для увеличения скорости этого решения добавлено несколько специальных членов.
    u64k* __ctype_b;
    i32k* __ctype_tolower;
    i32k* __ctype_toupper;
    // Заметка: LC_ALL неполноценный индекс в этом массиве.
    tukk __names[13];
};

//#endif
typedef  struct __locale_struct *__locale_t;
typedef __locale_t locale_t;
#define т_локаль locale_t

///////////////
// Регистрирует функцию, вызываемую при вызове `quick_exit'.
#ifdef __cplusplus
СИППЭКСП i32 at_quick_exit (void (*__func) (void))
     __THROW __asm ("at_quick_exit") __nonnull ((1));
#else
СИЭКСП i32 at_quick_exit (void (*__func) (void)) __THROW __nonnull ((1));
#endif

// Возвращается `div'.
#ifndef __div_t_defined
typedef struct
  {
    i32 quot;           // Коэффициент.
    i32 rem;            // Остаток.
  } div_t;
  
#define __div_t_defined 1
#endif

// Возвращается `ldiv'.
#ifndef __ldiv_t_defined
typedef struct
  {
    i64 quot;      // Коэффициент.
    i64 rem;        // Остаток.
  } ldiv_t;
  
#define __ldiv_t_defined   1
#endif

#ifndef __lldiv_t_defined
// Возвращается `lldiv'.
__extension__ typedef struct
  {
    z64 quot;     // Коэффициент.
    z64 rem;      // Коэффициент.
  } lldiv_t;
  
#define __lldiv_t_defined  1
#endif

// Наибольшее число, возвращаемое rand (аналогично INT_MAX).
#define RAND_MAX    2147483647

/* Эти макросы определены одинаково для всех машин.
   Изменения следует делать в `_exit'.  */
#define EXIT_FAILURE    1   // Статус выхода с ошибками.
#define EXIT_SUCCESS    0   // Статус успешного выхода.

// Максимальная длина многобайтного символа в текущей локали.
#define MB_CUR_MAX  (__ctype_get_mb_cur_max ())
СИЭКСП size_t __ctype_get_mb_cur_max (void) __THROW __wur;

// Преобразовывает текст в ПЗ-число.
СИЭКСП double atof (tukk __nptr) __THROW __attribute_pure__ __nonnull ((1)) __wur;
// Преобразовывает текст в 32-битное ЦЧ.
СИЭКСП i32 atoi (tukk __nptr) __THROW __attribute_pure__ __nonnull ((1)) __wur;
// Преобразовывает текст в 64-битное ЦЧ.
СИЭКСП i64 atol (tukk __nptr) __THROW __attribute_pure__ __nonnull ((1)) __wur;

// Преобразовывает текст в long long integer.
__extension__ СИЭКСП z64 atoll (tukk __nptr)
     __THROW __attribute_pure__ __nonnull ((1)) __wur;

// Преобразовывает текст в ПЗ-число.
СИЭКСП double strtod (tukk __restrict __nptr,
              tuk* __restrict __endptr) __THROW __nonnull ((1));

// То же для ПЗ-типов с размерами `float' и `long double'.
СИЭКСП float strtof (tukk __restrict __nptr,
             tuk* __restrict __endptr) __THROW __nonnull ((1));

СИЭКСП real_t strtold (tukk __restrict __nptr,
                tuk* __restrict __endptr) __THROW __nonnull ((1));

// Преобразовывает текст в i64.
СИЭКСП i64 strtol (tukk __restrict __nptr,
            tuk* __restrict __endptr, i32 __base)  __THROW __nonnull ((1));
/// Преобразовывает текст в u64.
СИЭКСП u64 strtoul (tukk __restrict __nptr,
                  tuk* __restrict __endptr, i32 __base) __THROW __nonnull ((1));

// Преобразовывает текст в quadword integer.
__extension__
СИЭКСП z64 strtoq (tukk __restrict __nptr,
                 tuk* __restrict __endptr, i32 __base) __THROW __nonnull ((1));
// Преобразовывает текст в unsigned quadword integer.
__extension__
СИЭКСП zu64 strtouq (tukk __restrict __nptr,
                       tuk* __restrict __endptr, i32 __base)  __THROW __nonnull ((1));

// Преобразовывает текст в quadword integer.
__extension__
СИЭКСП z64 strtoll (tukk __restrict __nptr,
                  tuk* __restrict __endptr, i32 __base) __THROW __nonnull ((1));
// Преобразовывает текст в unsigned quadword integer.
__extension__
СИЭКСП zu64 strtoull (tukk __restrict __nptr,
                    tuk* __restrict __endptr, i32 __base) __THROW __nonnull ((1));

/* Специальная версия тех же функций, что выше, принимающих локаль
   в качестве дополнительного параметра.  */
СИЭКСП i64 strtol_l (tukk __restrict __nptr,
              tuk* __restrict __endptr, i32 __base,
              __locale_t __loc) __THROW __nonnull ((1, 4));

СИЭКСП u64 strtoul_l (tukk __restrict __nptr,
                    tuk* __restrict __endptr,
                    i32 __base, __locale_t __loc) __THROW __nonnull ((1, 4));

__extension__
СИЭКСП z64 strtoll_l (tukk __restrict __nptr,
                tuk* __restrict __endptr, i32 __base,
                __locale_t __loc) __THROW __nonnull ((1, 4));

__extension__
СИЭКСП zu64 strtoull_l (tukk __restrict __nptr,
                      tuk* __restrict __endptr,
                      i32 __base, __locale_t __loc) __THROW __nonnull ((1, 4));

СИЭКСП double strtod_l (tukk __restrict __nptr,
            tuk* __restrict __endptr, __locale_t __loc)  __THROW __nonnull ((1, 3));

СИЭКСП float strtof_l (tukk __restrict __nptr,
               tuk* __restrict __endptr, __locale_t __loc) __THROW __nonnull ((1, 3));

СИЭКСП real_t strtold_l (tukk __restrict __nptr,
                  tuk* __restrict __endptr,
                  __locale_t __loc)  __THROW __nonnull ((1, 3));

__extension__ __extern_inline z64 __NTH (atoll (tukk __nptr))
{
  return strtoll (__nptr, (tuk* ) NULL, 10);
}

/* Преобразовывает N в base 64, используя цифры "./0-9A-Za-z", least-significant
   digit first.  Возвращает указатель на статическое хранилище, который переопределяется
   при следующем вызове.  */
СИЭКСП tuk l64a (i64 __n) __THROW __wur;

// Читает число из строки S в base 64, как выше.
СИЭКСП i64 a64l (tukk __s) __THROW __attribute_pure__ __nonnull ((1)) __wur;

/* Функции `random', `srandom', `initstate' и `setstate' из BSD Unices.
   Функции `rand' и `srand' требуются по стандарту ANSI.
   Предоставлены оба интерфейса к одному и тому же генератору случайных чисел (ГСЧ).
   Возвращается случайное число типа i64 в диапазоне от 0 до RAND_MAX включительно.  */
СИЭКСП i64 random (void) __THROW;

// Заправляет в ГСЧ определённое число.
СИЭКСП void srandom (u32 __seed) __THROW;

/* Инициализует ГСЧ с использованием буфера состояния STATEBUF,
   длиной STATELEN, и заправляет в него семя SEED. Оптимальные длины равны 8, 16,
   32, 64, 128 и 256, чем больше, тем лучше; значения менее 8 вызовут ошибку,
   а более 256 будут округлены вниз.  */
СИЭКСП tuk initstate (u32 __seed, tuk __statebuf,
            size_t __statelen) __THROW __nonnull ((2));

/* Переключает ГСЧ на буфер состояния STATEBUF, который прежде этого
   долен быть иницмализован посредством `initstate'.  */
СИЭКСП tuk setstate (tuk __statebuf) __THROW __nonnull ((1));

/* Реэнтрантные версии функций семейства `random'.
   Все эти функции используют следующую структуру данных для хранения
   состояния вместо глобальных переменных состояния.  */

struct random_data
  {
    i32* fptr;      // Указатель на перед.
    i32* rptr;      // Указатель на зад.
    i32* state;     // Массив значений состояния.
    i32 rand_type;      // Тип ГСЧ.
    i32 rand_deg;       // Степень ГСЧ.
    i32 rand_sep;       // Расстояние между передом и задом.
    i32* end_ptr;       // Указатель за таблицу состояний.
  };

СИЭКСП i32 random_r (struct random_data *__restrict __buf,
             i32* __restrict __result) __THROW __nonnull ((1, 2));

СИЭКСП i32 srandom_r (u32 __seed, struct random_data *__buf)  __THROW __nonnull ((2));

СИЭКСП i32 initstate_r (u32 __seed, tuk __restrict __statebuf,
            size_t __statelen,
            struct random_data* __restrict __buf)  __THROW __nonnull ((2, 4));

СИЭКСП i32 setstate_r (tuk __restrict __statebuf,
               struct random_data* __restrict __buf) __THROW __nonnull ((1, 2));

// Возвращает ЦЧ от 0 до RAND_MAX включительно.
СИЭКСП i32 rand (void) __THROW;
// Заправляет в ГСЧ данное число.
СИЭКСП void srand (u32 __seed) __THROW;

// Реэнтрантный интерфейс в соответствии с POSIX.1.
СИЭКСП i32 rand_r (u32 *__seed) __THROW;

// Функции ГСЧ в стиле System V в 48 бит.

// Возвращает неотрицательное, двойной точности ПЗ-значение в [0.0,1.0).
СИЭКСП double drand48 (void) __THROW;
СИЭКСП double erand48 (u16 __xsubi[3]) __THROW __nonnull ((1));

/* Возвращает неотрицательный ц64 в [0,2^31).  */
СИЭКСП i64 lrand48 (void) __THROW;
СИЭКСП i64 nrand48 (u16 __xsubi[3]) __THROW __nonnull ((1));

/* Возвращает ц64 со знаком в [-2^31,2^31).  */
СИЭКСП i64 mrand48 (void) __THROW;
СИЭКСП i64 jrand48 (u16 __xsubi[3]) __THROW __nonnull ((1));

// Заправляет ГСЧ.
СИЭКСП void srand48 (i64 __seedval) __THROW;
СИЭКСП u16* seed48 (u16 __seed16v[3]) __THROW __nonnull ((1));
СИЭКСП void lcong48 (u16 __param[7]) __THROW __nonnull ((1));

/* Структура данных для коммуникации с потокобезопасными версиями. Этот
   тип рассматривается как opaque (скрытый, непрозрачный). Он экспортируется
   только потому, что пользователям надо разместить объекты этого типа.  */
struct drand48_data
  {
    u16 __x[3];  // Текущее состояние.
    u16 __old_x[3]; // Прежднее состояние.
    u16 __c; /* Additive const. in congruential formula.  */
    u16 __init;  /* Flag for initializing.  */
    zu64 __a; /* Factor in congruential formula.  */
  };

// Возвращает неотрицательное, двойной точности ПЗ-значение в [0.0,1.0).
СИЭКСП i32 drand48_r (struct drand48_data *__restrict __buffer,
              double *__restrict __result) __THROW __nonnull ((1, 2));
СИЭКСП i32 erand48_r (u16 __xsubi[3],
              struct drand48_data *__restrict __buffer,
              double *__restrict __result) __THROW __nonnull ((1, 2));

// Возвращает неотрицательное ц64 в [0,2^31).
СИЭКСП i32 lrand48_r (struct drand48_data *__restrict __buffer,
              i64 *__restrict __result)  __THROW __nonnull ((1, 2));
СИЭКСП i32 nrand48_r (u16 __xsubi[3],
              struct drand48_data *__restrict __buffer,
              i64 *__restrict __result) __THROW __nonnull ((1, 2));

// Возвращает ц64 со знаком в [-2^31,2^31).
СИЭКСП i32 mrand48_r (struct drand48_data *__restrict __buffer,
              i64 *__restrict __result)  __THROW __nonnull ((1, 2));
СИЭКСП i32 jrand48_r (u16 __xsubi[3],
              struct drand48_data *__restrict __buffer,
              i64 *__restrict __result)  __THROW __nonnull ((1, 2));

// Заправляет ГСЧ.
СИЭКСП i32 srand48_r (i64 __seedval, struct drand48_data *__buffer)  __THROW __nonnull ((2));
СИЭКСП i32 seed48_r (u16 __seed16v[3], struct drand48_data *__buffer) __THROW __nonnull ((1, 2));
СИЭКСП i32 lcong48_r (u16 __param[7], struct drand48_data *__buffer) __THROW __nonnull ((1, 2));

// Аллоцирует в памяти SIZE байтов.
СИЭКСП uk malloc (size_t __size) __THROW __attribute_malloc__ __wur;
// Аллоцирует элементы NMEMB, каждый размером SIZE байтов, все инициализованные в 0.
СИЭКСП uk calloc (size_t __nmemb, size_t __size)  __THROW __attribute_malloc__ __wur;

/* Реаллоцирует ранее аллоцированный блок
   в PTR, делая новый блок длиной в SIZE байтов.
 __attribute_malloc__ не используется, т.к.,если realloc возвратит
   тот же указатель, что был ему передан, нужно разрешить алиасинг
   между объектами, на которые указывает старый и новый указатели.  */
СИЭКСП uk realloc (uk __ptr, size_t __size)  __THROW __attribute_warn_unused_result__;
// Освобождает блок, аллоцированный `malloc', `realloc' или `calloc'.
СИЭКСП void free (uk __ptr) __THROW;

// Освобождает блок. Алиас для `free'.  (Sun Unices).
СИЭКСП void cfree (uk __ptr) __THROW;
 
////////////////////////// <alloca.h>
// Аллоцирует блок, который освобождается при выходе вызывающей функции.
СИЭКСП uk alloca (size_t __size) __THROW;

#ifdef	__GNUC__
# define alloca(size)	__builtin_alloca (size)
#endif
///////////////////////////
// Аллоцирует SIZE байтов на границе страницы. Хранилище нельзя освободить.
СИЭКСП uk valloc (size_t __size) __THROW __attribute_malloc__ __wur;

// Аллоцирует память в SIZE байтов с алиновкой ALIGNMENT.
СИЭКСП i32 posix_memalign (uk* __memptr, size_t __alignment, size_t __size)
     __THROW __nonnull ((1)) __wur;

// ISO C вариант алинованной аллокации.
СИЭКСП uk aligned_alloc (size_t __alignment, size_t __size)
     __THROW __wur __attribute__ ((__malloc__, __alloc_size__ (2)));

// Прерывает выполнение и генерирует дамп ядра (core-dump).
СИЭКСП void abort (void) __THROW __attribute__ ((__noreturn__));

// Регистрирует функцию, выполняемую при вызове `exit'.
СИЭКСП i32 atexit (void (*__func) (void)) __THROW __nonnull ((1));

/* Регистрирует функцию, вызываемую со статусом,
   заданным `exit', и с указанным аргументом.  */
СИЭКСП i32 on_exit (void (*__func) (i32 __status, uk __arg), uk __arg) __THROW __nonnull ((1));

/* Вызывает все функции, зарегистрированные посредством `atexit' и `on_exit',
   в порядке, обратном порядку их регистрации,
   выполняет зачистку stdio, и терминирует выполнение программы со статусом STATUS.  */
СИЭКСП void exit (i32 __status) __THROW __attribute__ ((__noreturn__));

/* Вызывает все функции, зарегистрированные посредством `at_quick_exit',
    в порядке, обратном порядку их регистрации,
   и терминирует выполнение программы со статусом STATUS.  */
СИЭКСП void quick_exit (i32 __status) __THROW __attribute__ ((__noreturn__));

/* Терминирует программу со статусом STATUS, не вызывая никакой из
   функций, зарегистрированных посредством `atexit' или `on_exit'.  */
СИЭКСП void _Exit (i32 __status) __THROW __attribute__ ((__noreturn__));

// Возвращает значение переменной среды NAME, либо NULL, если её не существует.
СИЭКСП tuk getenv (tukk __name) __THROW __nonnull ((1)) __wur;

/* Подобна предыдущей функции, но возвращает NULL, если программа
   выполняется с активированными SUID или SGID.  */
СИЭКСП tuk secure_getenv (tukk __name)  __THROW __nonnull ((1)) __wur;

/* Поместить STRING, который в форме "ИМЯ=ЗНАЧЕНИЕ", в среду.
   Если нет `=', то NAME будет удалено из среды.  */
СИЭКСП i32 putenv (tuk __string) __THROW __nonnull ((1));

/* Устанавливает NAME в VALUE в среде.
   Если REPLACE не ноль, то существующее знвчение будет переопределено. */
СИЭКСП i32 setenv (tukk __name, tukk __value, i32 __replace) __THROW __nonnull ((2));

// Удаляет из среды переменную NAME.
СИЭКСП i32 unsetenv (tukk __name) __THROW __nonnull ((1));

/* `clearenv' планировали добавить в POSIX.1, но, вероятно, забыли.
   Тем не менее, стандарт POSIX.9 (привязки POSIX для Fortran 77)
   без этой функции не может обойтись.  */
СИЭКСП i32 clearenv (void) __THROW;

/* Генерирует уникальное имя временного файла из шаблона TEMPLATE.
   Последние шесть символов TEMPLATE должны быть "XXXXXX";
   они заменяются текстом, делающим имя файла уникальным.
   Всегда возвращает TEMPLATE, как уникальное имя временного файла,
   либо нулевую строку, если не удалось получить уникальное имя.  */
СИЭКСП tuk mktemp (tuk __template) __THROW __nonnull ((1));

/* Генерирует уникальное имя временного файла из шаблона TEMPLATE.
   Последние шесть символов TEMPLATE должны быть "XXXXXX";
   они заменяются текстом, делающим имя файла уникальным.
   Возвращает файловый дескриптор, открытый на файле для чтения и записи,
   либо -1, если ему не удвлось создать файл с уникальным именем.

   Эта функция может выступать точкой отмены, поэтому она не помечена
   посредством __THROW.  */
#ifndef __USE_FILE_OFFSET64
СИЭКСП i32 mkstemp (tuk __template) __nonnull ((1)) __wur;
#else
#ifdef __REDIRECT
СИЭКСП i32 __REDIRECT (mkstemp, (tuk __template), mkstemp64) __nonnull ((1)) __wur;
#else
#define mkstemp mkstemp64
#endif
#endif

#ifdef __USE_LARGEFILE64
СИЭКСП i32 mkstemp64 (tuk __template) __nonnull ((1)) __wur;
#endif

/* Подобна mkstemp, но у шаблона может иметься суффикс после
   XXXXXX. Длина суффикса задаётся вторым параметром.

   Эта функция может выступать точкой отмены, поэтому она не помечена
   посредством __THROW.  */
#ifndef __USE_FILE_OFFSET64
СИЭКСП i32 mkstemps (tuk __template, i32 __suffixlen) __nonnull ((1)) __wur;
#else
#ifdef __REDIRECT
СИЭКСП i32 __REDIRECT (mkstemps, (tuk __template, i32 __suffixlen),
                                                             mkstemps64) __nonnull ((1)) __wur;
#else
#define mkstemps mkstemps64
#endif
#endif

#ifdef __USE_LARGEFILE64
СИЭКСП i32 mkstemps64 (tuk __template, i32 __suffixlen) __nonnull ((1)) __wur;
#endif

/* Создать уникальную временную директорию из шаблона TEMPLATE.
   Последними шестью символами TEMPLATE должны быть "XXXXXX";
   они заменяются строкой, делающей имя директори уникальным.
   Возвращает TEMPLATE, либо нулевой указатель, если уникальное имя не получено.
   Директория создаётся в режиме 700.  */
СИЭКСП tuk mkdtemp (tuk __template) __THROW __nonnull ((1)) __wur;

/* Генерирует уникальное имя временного файла из шаблона TEMPLATE, подобно
   mkstemp. Но вызывающая функция может передавать дополнительные флаги,
   используемые при вызове open для создания файла.

   Эта функция может выступать точкой отмены, поэтому она не помечена
   посредством __THROW.  */
#ifndef __USE_FILE_OFFSET64
СИЭКСП i32 mkostemp (tuk __template, i32 __flags) __nonnull ((1)) __wur;
#else
#ifdef __REDIRECT
СИЭКСП i32 __REDIRECT (mkostemp, (tuk __template, i32 __flags), mkostemp64) __nonnull ((1)) __wur;
#else
#define mkostemp mkostemp64
#endif
#endif

#ifdef __USE_LARGEFILE64
СИЭКСП i32 mkostemp64 (tuk __template, i32 __flags) __nonnull ((1)) __wur;
#endif

/* Подобна mkostemp, но у шаблона может иметься суффикс после
   XXXXXX. Длина суффикса задаётся вторым параметром.

   Эта функция может выступать точкой отмены, поэтому она не помечена
   посредством __THROW.  */
#ifndef __USE_FILE_OFFSET64
СИЭКСП i32 mkostemps (tuk __template, i32 __suffixlen, i32 __flags) __nonnull ((1)) __wur;
#else
#ifdef __REDIRECT
СИЭКСП i32 __REDIRECT (mkostemps, (tuk __template, i32 __suffixlen,
                                           i32 __flags), mkostemps64) __nonnull ((1)) __wur;
#else
#define mkostemps mkostemps64
#endif
#endif

#ifdef __USE_LARGEFILE64
СИЭКСП i32 mkostemps64 (tuk __template, i32 __suffixlen, i32 __flags) __nonnull ((1)) __wur;
#endif

/* Выполняет заданную строку как команду оболочке.

   Эта функция может выступать точкой отмены, поэтому она не помечена
   посредством __THROW.  */
СИЭКСП i32 system (tukk __command) __wur;

/* Возвращает текст, аллоцированный посредством malloc, с каноническим
   абсолютным именем существующего файла.  */
СИЭКСП tuk canonicalize_file_name (tukk __name) __THROW __nonnull ((1)) __wur;

/* Возвращает каноническое абсолютное имя файла NAME. Если RESOLVED равно
   null, итог аллоцируется посредством malloc; иначе, если каноническое имя
   составляет PATH_MAX или более символов, возвратит null с `errno', установленным в
   ENAMETOOLONG; Если имя помещается в менее чем PATH_MAX символов,
   возвращает  в RESOLVED имя.  */
СИЭКСП tuk realpath (tukk __restrict __name, tuk __restrict __resolved) __THROW __wur;

// Сокращённое имя для типа функций сравнения.
#ifndef __COMPAR_FN_T
#define __COMPAR_FN_T
typedef i32 (*__compar_fn_t) (ukk , ukk );
typedef __compar_fn_t comparison_fn_t;
#endif

typedef i32 (*__compar_d_fn_t) (ukk, ukk, uk );

/* Выполняет двомчный поиск KEY в BASE, состоящей из NMEMB элементов,
   размером в SIZE байтов каждый, используя COMPAR для проведения сравнения.  */
СИЭКСП uk bsearch (ukk __key, ukk __base,
             size_t __nmemb, size_t __size, __compar_fn_t __compar) __nonnull ((1, 2, 5)) __wur;

/* Сортирует NMEMB элементов из BASE, размером в SIZE байтов каждый,
   используя COMPAR для проведения сравнения.  */
СИЭКСП void qsort (uk __base, size_t __nmemb, size_t __size,
                                                    __compar_fn_t __compar) __nonnull ((1, 4));

СИЭКСП void qsort_r (uk __base, size_t __nmemb, size_t __size,
                                        __compar_d_fn_t __compar, uk __arg) __nonnull ((1, 4));

// Возвращает абсолютное значение X.
СИЭКСП i32 abs (i32 __x) __THROW __attribute__ ((__const__)) __wur;
СИЭКСП i64 labs (i64 __x) __THROW __attribute__ ((__const__)) __wur;
__extension__ СИЭКСП z64 llabs (z64 __x) __THROW __attribute__ ((__const__)) __wur;

/* Возвращает  `div_t', `ldiv_t' или `lldiv_t' представление
   значения NUMER over DENOM.
   GCC may have built-ins for these someday.  */
СИЭКСП div_t div (i32 __numer, i32 __denom)  __THROW __attribute__ ((__const__)) __wur;
СИЭКСП ldiv_t ldiv (i64 __numer, i64 __denom) __THROW __attribute__ ((__const__)) __wur;
__extension__ СИЭКСП lldiv_t lldiv (z64 __numer, z64 __denom)
                                                     __THROW __attribute__ ((__const__)) __wur;

/* Преобразовывает в текст ПЗ-числа. Возвращёные значения валидны только
   до другого вызова этой же функции.  */

/* Преобразовывает VALUE в текст с NDIGIT цифр и возвращает указатель на
   него.  В *DECPT указывается позиция десятичного символа, а в *SIGN -
   знак этого числа.  */
СИЭКСП tuk ecvt (double __value, i32 __ndigit, i32* __restrict __decpt,
                                      i32* __restrict __sign) __THROW __nonnull ((3, 4)) __wur;

/*  Преобразовывает VALUE в текст, округляя его до NDIGIT деятичных цифр. В *DECPT
   указывается позиция десятичного символа, а в *SIGN -
   знак этого числа.  */
СИЭКСП tuk fcvt (double __value, i32 __ndigit, i32* __restrict __decpt,
                                      i32* __restrict __sign) __THROW __nonnull ((3, 4)) __wur;

/* По возможности преобразует VALUE в текст с NDIGIT значащих цифр.
   Иначе используется экспоненциальное представление. итоговый текст
   записывается в BUF.  */
СИЭКСП tuk gcvt (double __value, i32 __ndigit, tuk __buf) __THROW __nonnull ((3)) __wur;

/* Long double versions of above functions.  */
СИЭКСП tuk qecvt (real_t __value, i32 __ndigit,
              i32* __restrict __decpt, i32* __restrict __sign) __THROW __nonnull ((3, 4)) __wur;
СИЭКСП tuk qfcvt (real_t __value, i32 __ndigit,
            i32* __restrict __decpt, i32* __restrict __sign)   __THROW __nonnull ((3, 4)) __wur;
СИЭКСП tuk qgcvt (real_t __value, i32 __ndigit, tuk __buf)   __THROW __nonnull ((3)) __wur;

/* Реэнтрантные версии функций, представленных выше, предоставляющие
   свои собственные буферы.  */
СИЭКСП i32 ecvt_r (double __value, i32 __ndigit, i32* __restrict __decpt,
                                i32* __restrict __sign, tuk __restrict __buf,
                                                    size_t __len) __THROW __nonnull ((3, 4, 5));
СИЭКСП i32 fcvt_r (double __value, i32 __ndigit, i32* __restrict __decpt,
                                i32* __restrict __sign, tuk __restrict __buf,
                                                    size_t __len) __THROW __nonnull ((3, 4, 5));

СИЭКСП i32 qecvt_r (real_t __value, i32 __ndigit,
                     i32* __restrict __decpt, i32* __restrict __sign,
                              tuk __restrict __buf, size_t __len) __THROW __nonnull ((3, 4, 5));
СИЭКСП i32 qfcvt_r (real_t __value, i32 __ndigit,
                     i32* __restrict __decpt, i32* __restrict __sign,
                              tuk __restrict __buf, size_t __len) __THROW __nonnull ((3, 4, 5));

// Возвращает длину многобайтного символа в S, который не длинее N.
СИЭКСП i32 mblen (tukk __s, size_t __n) __THROW __wur;

/* Возвращает длину указанного многобайтного символа, помещая его `wchar_t'
   представление в *PWC.  */
СИЭКСП i32 mbtowc (wtuk __restrict __pwc, tukk __restrict __s, size_t __n) __THROW __wur;
           
// Помещает многобайтный символ, представленный WCHAR из S, возвращая его длину.
СИЭКСП i32 wctomb (tuk __s, wchar_t __wchar) __THROW __wur;

// Преобразует многобайтный текст в широкосимвольный.
СИЭКСП size_t mbstowcs (wtuk __restrict  __pwcs, tukk __restrict __s, size_t __n) __THROW;
            
// Преобразует широкосимвольный текст в многобайтный.
СИЭКСП size_t wcstombs (tuk __restrict __s,
                                       wtukk __restrict __pwcs, size_t __n)  __THROW;

/* Определяет, соответствует ли текстовое значение RESPONSE подтверждающему
   или отрицающему респонс-выражению, заданному категорией LC_MESSAGES в
   текущей локали программы. Возвращает 1 при подтвердительном, 0 при
   отрицательном и -1 при несовпадении.  */
СИЭКСП i32 rpmatch (tukk __response) __THROW __nonnull ((1)) __wur;

/* Parse comma separated suboption from *OPTIONP and match against
   strings in TOKENS.  If found return index and set *VALUEP to
   optional value introduced by an equal sign.  If the suboption is
   not part of TOKENS return in *VALUEP beginning of unknown
   suboption.  On exit *OPTIONP is set to the beginning of the next
   token or at the terminating NUL character.  */
СИЭКСП i32 getsubopt (tuk* __restrict __optionp, tuk const *__restrict __tokens,
                                tuk* __restrict __valuep) __THROW __nonnull ((1, 2, 3)) __wur;

/* Setup DES tables according KEY.  */
СИЭКСП void setkey (tukk __key) __THROW __nonnull ((1));

/* X/Open pseudo terminal handling.  */

/* Возвращает master pseudo-terminal handle.  */
СИЭКСП i32 posix_openpt (i32 __oflag) __wur;

/* The next four functions all take a master pseudo-tty fd and
   perform an operation on the associated slave:  */

/* Chown the slave to the calling user.  */
СИЭКСП i32 grantpt (i32 __fd) __THROW;

/* Release an internal lock so the slave can be opened.
   Call after grantpt().  */
СИЭКСП i32 unlockpt (i32 __fd) __THROW;

/* Возвращает the pathname of the pseudo terminal slave assoicated with
   the master FD is open on, or NULL on errors.
   The returned storage is good until the next call to this function.  */
СИЭКСП tuk ptsname (i32 __fd) __THROW __wur;

/* Store at most BUFLEN characters of the pathname of the slave pseudo
   terminal associated with the master FD is open on in BUF.
   Возвращает 0 on success, otherwise an error number.  */
СИЭКСП i32 ptsname_r (i32 __fd, tuk __buf, size_t __buflen) __THROW __nonnull ((2));

/* Open a master pseudo terminal and return its file descriptor.  */
СИЭКСП i32 getpt (void);

/* Put the 1 minute, 5 minute and 15 minute load averages into the first
   NELEM elements of LOADAVG.  Возвращает the number written (never more than
   three, but may be less than NELEM), or -1 if an error occurred.  */
СИЭКСП i32 getloadavg (double __loadavg[], i32 __nelem)
     __THROW __nonnull ((1));
///////////////////////////////////////////////////
       #include <sys/types.h>

СИЭКСП int getentropy(void *buffer, size_t length);
СИЭКСП void explicit_bzero(void *s, size_t n)__THROW __nonnull ((1))
    __fortified_attr_access (__write_only__, 1, 2);
СИЭКСП ssize_t copy_file_range(i32 fd_in, off_t *off_in,
                               i32 fd_out, off_t *off_out,
                               size_t len, u32 flags);
СИЭКСП pid_t gettid(void)__THROW;
СИЭКСП int close_range(unsigned int first, unsigned int last, int flags)__THROW;
СИЭКСП int close_from(unsigned int first);

/////////////////////////////////////////////////
#ifdef __cplusplus

#define __CORRECT_ISO_CPP_STDLIB_H_PROTO 1

#include <bits/std_abs.h>

extern "C++"
{
namespace std
{
  using ::div_t;
  using ::ldiv_t;
  using ::abort;
  using ::aligned_alloc;
  using ::atexit;
  using ::at_quick_exit;
  using ::atof;
  using ::atoi;
  using ::atol;
  using ::bsearch;
  using ::calloc;
  using ::div;
  using ::exit;
  using ::free;
  using ::getenv;
  using ::abs;
  using ::labs;
  using ::llabs;
  using ::ldiv;
  using ::malloc;
  using ::mblen;
  using ::mbstowcs;
  using ::mbtowc;
  using ::qsort;
  using ::quick_exit;
  using ::rand;
  using ::realloc;
  using ::srand;
  using ::strtod;
  using ::strtol;
  using ::strtoul;
  using ::system;
  using ::wcstombs;
  using ::wctomb;

  inline ldiv_t  div(long __i, long __j) { return ldiv(__i, __j); }
} // namespace

namespace __gnu_cxx
{
  using ::lldiv_t;
  using ::_Exit;
  using ::llabs;

  inline lldiv_t  div(long long __n, long long __d)
  { lldiv_t __q; __q.quot = __n / __d; __q.rem = __n % __d; return __q; }

  using ::lldiv;
  using ::atoll;
  using ::strtoll;
  using ::strtoull;
  using ::strtof;
  using ::strtold;

} // namespace __gnu_cxx

namespace std
{
  using ::__gnu_cxx::lldiv_t;
  using ::__gnu_cxx::_Exit;
  using ::__gnu_cxx::llabs;
  using ::__gnu_cxx::div;
  using ::__gnu_cxx::lldiv;
  using ::__gnu_cxx::atoll;
  using ::__gnu_cxx::strtof;
  using ::__gnu_cxx::strtoll;
  using ::__gnu_cxx::strtoull;
  using ::__gnu_cxx::strtold;
} // namespace std
}//extern C++

//Нужно согласовать этот заголовок с нашими типами!!!
#include <cstdlib>
#endif
#endif
#endif
#endif
