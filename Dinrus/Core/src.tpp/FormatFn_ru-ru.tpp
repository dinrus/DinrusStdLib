topic "Форматирование чисел";
[i448;a25;kKO9;2 $$1,0#37138531426314131252341829483380:class]
[l288;2 $$2,0#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:item]
[l288;a4;*@5;1 $$6,6#70004532496200323422659154056402:requirement]
[l288;i1121;b17;O9;~~~.1408;2 $$7,0#10431211400427159095818037425705:param]
[i448;b42;O9;2 $$8,8#61672508125594000341940100500538:tparam]
[b42;2 $$9,9#13035079074754324216151401829390:normal]
[2 $$0,0#00000000000000000000000000000000:Default]
[{_}%RU-RU 
[ {{10000@(113.42.0) [s0; [*@7;4 Форматирование чисел]]}}&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:FormatUnsigned`(Upp`:`:dword`):%- [_^Upp`:`:String^ String]_[* FormatUnsigned
]([_^Upp`:`:dword^ dword]_[*@3 w])&]
[s2; Возвращает аргумент как десятичную 
строку&]
[s3; &]
[s4; &]
[s5;:FormatInt`(int`):%- [_^String^ String]_[* FormatInt]([@(0.0.255) int]_[*@3 i])&]
[s2; Возвращает аргумент как десятичную 
строку. Если аргумент равен Null, строка 
пуста.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:FormatUInt64`(Upp`:`:uint64`):%- [_^Upp`:`:String^ String]_[* FormatUInt64]([_^Upp`:`:uint64^ u
int64]_[*@3 w])&]
[s2; Возвращает аргумент как десятичную 
строку&]
[s3; &]
[s4;%- &]
[s5;:FormatInt64`(int64`):%- [_^String^ String]_[* FormatInt64]([_^int64^ int64]_[*@3 a])&]
[s2; Возвращает аргумент как десятичную 
строку. Если аргумент равен Null, строка 
пуста.&]
[s3; &]
[s4; &]
[s5;:FormatIntBase`(int`,int`,int`,char`,int`,bool`):%- String_[* FormatIntBase]([@(0.0.255) i
nt]_[*@3 i], [@(0.0.255) int]_[*@3 radix], [@(0.0.255) int]_[*@3 width]_`=_[@3 0], 
[@(0.0.255) char]_[*@3 lpad]_`=_`'_`', [@(0.0.255) int]_[*@3 sign]_`=_[@3 0], 
[@(0.0.255) bool]_[*@3 upper]_`=_[@(0.0.255) false])&]
[s2; Форматирует целое, со знаком или 
без, [%-*@3 i,] при заданном корне [%-*@3 radix,] 
с левым padding`'ом на заданную ширину 
[%-*@3 width,] с заданным символом [%-*@3 lpad]. 
Если [*@3 sign ]равен `+1 `= всегда предпоставлять 
`'`+`'/`-, если 0 `= auto (только `'`-`'), если 
`-1, форматировать как беззначное. 
Если [%-*@3 upper] равно true, буквы для base 
> 10 в верхнем регистре. Если [%-*@3 i] равно 
Null, возвращает пустую String.&]
[s3; &]
[s3; &]
[s4;%- &]
[s5;:FormatIntDec`(int`,int`,char`,bool`):%- [_^String^ String]_[* FormatIntDec]([@(0.0.255) i
nt]_[*@3 i], [@(0.0.255) int]_[*@3 width], [@(0.0.255) char]_[*@3 lpad]_`=_`'_`', 
[@(0.0.255) bool]_[*@3 always`_sign]_`=_[@(0.0.255) false])&]
[s2; Форматирует десятичное целое со 
знаком [%-*@3 i, ]с левым padding`'ом до заданной 
ширины [%-*@3 width] символов (вывод более 
длинных не pad`'ируется), с заданным 
символом [%-*@3 lpad]. Если [%-*@3 always`_sign ]равно 
true, начинать положительные числа 
с `'`+`' (отрицательные числа всегда 
начинаются с `'`-`').&]
[s3; &]
[s4;%- &]
[s5;:FormatIntHex`(int`,int`,char`):%- [_^String^ String]_[* FormatIntHex]([@(0.0.255) int]_
[*@3 i], [@(0.0.255) int]_[*@3 width]_`=_[@3 8], [@(0.0.255) char]_[*@3 lpad]_`=_`'[@3 0]`')&]
[s2;~~~.1152; Форматирует беззначное шестнадцатирич
ное (корень 16) целое (integer) [%-*@3 i,] с левым 
padding`'ом до заданной ширины [%-*@3 width] 
символов (вывод более длинных не pad`'ируется),
 с заданным символом [%-*@3 lpad].&]
[s3; &]
[s4;%- &]
[s5;:FormatIntOct`(int`,int`,char`):%- [_^String^ String]_[* FormatIntOct]([@(0.0.255) int]_
[*@3 i], [@(0.0.255) int]_[*@3 width]_`=_[@3 12], [@(0.0.255) char]_[*@3 lpad]_`=_`'[@3 0]`')&]
[s2; Форматирует беззначное восмиричное 
(корень 8) целое [%-*@3 i,] с левым padding`'ом 
до заданной ширины [%-*@3 width] символов 
(вывод более длинных не pad`'ируется), 
с заданным символом [%-*@3 lpad].&]
[s3; &]
[s4;%- &]
[s5;:FormatIntRoman`(int`,bool`):%- [_^String^ String]_[* FormatIntRoman]([@(0.0.255) int]_
[*@3 i], [@(0.0.255) bool]_[*@3 upper]_`=_[@(0.0.255) false])&]
[s2; Форматирует значное целое [%-*@3 i] в 
римские числительные. Наибольший 
`"реализованный`" нумерал равен M (1000), 
поэтому при форматировании миллионов 
ожидается довольно длинная строка. 
Отрицательные числа представлены 
с подстановкой `[`-`], 0 или Null выводятся 
как Null String. Если [%-*@3 upper] равно true, используются
 заглавные `[true`] или строчные `[false`] 
буквы&]
[s3; &]
[s4;%- &]
[s5;:FormatIntAlpha`(int`,bool`):%- [_^String^ String]_[* FormatIntAlpha]([@(0.0.255) int]_
[*@3 i], [@(0.0.255) bool]_[*@3 upper]_`=_[@(0.0.255) true])&]
[s2; Форматирует значное целое [%-*@3 i]  в 
лексикографический индекс length`-first, 
т.е. нумерацию колонок excel (1 `= A,2 `= B.. 
 26 `= Z, 27 `= AA, 28 `= AB .. 52 `= AZ, 53 `= BA ... 78 `= 
BZ и т.д.). 0 или Null выводится как Null String. 
Только к отрицательным числам предпоставляе
тся `[`-`]. Если [%-*@3 upper] равен true, используется 
верхний регистр букв.&]
[s3; &]
[s4;%- &]
[s5;:Format64Hex`(uint64`):%- [_^String^ String]_[* Format64Hex]([_^uint64^ uint64]_[*@3 a])&]
[s2; Форматирует беззначное 16`-ричное 
[%-*@3 a] (корень 16) 64`-битное целое. Предположител
ьно очень быстр. Прописные буквы a`-f 
используются для цифр `[10`] `- `[15`].&]
[s3; &]
[s4;%- &]
[s5;:FormatIntHex`(const void`*`):%- [_^String^ String]_[* FormatIntHex]([@(0.0.255) const]_
[@(0.0.255) void]_`*[*@3 ptr])&]
[s2; Форматирует адрес указателя [%-*@3 ptr] 
как шестнадцатиричное (основание 
16) число, zero`-padded до числа цифр, подходящей 
для этой модели памяти (8 цифр на 32`-битных 
системах, 16 цифр на 64`-битных). Используется 
для логгирования и отладочных целей. 
Эквивалентно FormatHex.&]
[s3; &]
[s4;%- &]
[s5;:FormatHex`(const void`*`):%- [_^String^ String]_[* FormatHex]([@(0.0.255) const]_[@(0.0.255) v
oid]_`*[*@3 ptr])&]
[s2; То же, что и FormatIntHex.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:FormatDouble`(char`*`,double`,int`,Upp`:`:dword`):%- [@(0.0.255) char]_`*[* F
ormatDouble]([@(0.0.255) char]_`*[*@3 t], [@(0.0.255) double]_[*@3 x], 
[@(0.0.255) int]_[*@3 precision], [_^Upp`:`:dword^ dword]_[*@3 flags]_`=_FD`_TOLERANCE([@3 6
])`|FD`_MINIMAL`_EXP[@(0.0.255) `|]FD`_SPECIAL)&]
[s5;:Upp`:`:FormatDouble`(double`,int`,Upp`:`:dword`):%- [_^Upp`:`:String^ String]_[* For
matDouble]([@(0.0.255) double]_[*@3 x], [@(0.0.255) int]_[*@3 precision], 
[_^Upp`:`:dword^ dword]_[*@3 flags]_`=_FD`_TOLERANCE([@3 6])`|FD`_MINIMAL`_EXP[@(0.0.255) `|
]FD`_SPECIAL)&]
[s2; Форматирует число с плавающей точкой 
[%-*@3 d] в десятичной нотации, автоматически 
выбирая ординарную или научную (экспоненциа
льную) нотацию, в соответствии с флагом 
FD`_TOLERANCE.&]
[s2; Для первого варианта требуется буфер 
размером [%-*@3 precision] `+ 30 и он возвращает 
указатель за последним произведённым 
символом (оканчивающий `'`\0`'  НЕ приставляетс
я).&]
[s2; [%-*@3 precision] представляет собой число 
полноценных цифр.&]
[s2; [%-*@3 flags] может быть бинарной комбинацией.&]
[s0; &]
[ {{2691:7309<288;h1; [s0; FD`_SIGN]
:: [s0; [/ всегда впереди ставить знак (`+10)]]
:: [s0; FD`_MINUS0]
:: [s0; [/ выводить знак минуса для отрицательного 
нуля (`-0.0)]]
:: [s0; FD`_SIGN`_EXP]
:: [s0; [/ всегда перед экспонентой ставить 
знак (1e`+2)]]
:: [s0; FD`_CAP`_E]
:: [s0; [/ заглавная E для экспоненты (1E10)]]
:: [s0; FD`_ZEROS]
:: [s0; [/ сохранять нули в хвосте (1.25000)]]
:: [s0; FD`_MINIMAL`_EXP]
:: [s0; [/ исользовать минимальную экспоненту 
(1e5 вместо 1e`+05)]]
:: [s0; FD`_TOLERANCE(x)]
:: [s0; [/ число нулей, допустимых справа между 
десятичными точками и полноценными 
цифрами, до переключения в нотацию 
E]]
:: [s0; FD`_SPECIAL]
:: [s0; [/ разрешает выводить ][*/ nan][/  / ][*/ inf][/  (иначе 
такие числа выводятся пустыми)]]
:: [s0; FD`_FIX]
:: [s0; [/ всегда использовать фиксированную 
нотацию (переадресует FormatDouble к FormatF)]]
:: [s0; FD`_EXP]
:: [s0; [/ всегда использовать экспоненциальную 
нотацию (переадресует FormatDouble к FormatE)]]
:: [s0; FD`_SIGN`_SPACE]
:: [s0; [/ ставить впереди пробел вместо знака 
у положительных чисел]]
:: [s0; FD`_POINT]
:: [s0; [/ всегда добавлять десятичную точку]]}}&]
[s0; &]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:FormatDouble`(char`*`,double`):%- [@(0.0.255) char]_`*[* FormatDouble]([@(0.0.255) c
har]_`*[*@3 t], [@(0.0.255) double]_[*@3 x])&]
[s5;:FormatDouble`(double`):%- [_^String^ String]_[* FormatDouble]([@(0.0.255) double]_[*@3 a
])&]
[s2; То же, что и FormatDouble с флагами FD`_TOLERANCE(6)`|FD`_MINIMA
L`_EXP`|FD`_SPECIAL и точностью 15. Должно представлят
ь наиболее разумное форматирование 
для отображаемого вывода. Превый 
вариант требует буфер размером 32 
и возвращает указатель за последний 
произведённый символ (оканчивающий 
`'`\0`' НЕ приставляется).&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:FormatE`(char`*`,double`,int`,Upp`:`:dword`):%- [@(0.0.255) char]_`*[* Format
E]([@(0.0.255) char]_`*[*@3 t], [@(0.0.255) double]_[*@3 x], [@(0.0.255) int]_[*@3 precision],
 [_^Upp`:`:dword^ dword]_[*@3 flags])&]
[s5;:Upp`:`:FormatE`(double`,int`,Upp`:`:dword`):%- [_^Upp`:`:String^ String]_[* FormatE](
[@(0.0.255) double]_[*@3 x], [@(0.0.255) int]_[*@3 precision], [_^Upp`:`:dword^ dword]_[*@3 f
lags])&]
[s2; С дефолтными флагами [%-*@3 flags] предоставляет 
форматирование double, эквивалентное 
формату %e у printf. Первый вариант требует 
буфер размером [%-*@3 precision] `+ 30 и возвращает 
указатель за последний произведённый 
символ (оканчивающий `'`\0`' НЕ приставляется).&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:FormatF`(char`*`,double`,int`,Upp`:`:dword`):%- [@(0.0.255) char]_`*[* Format
F]([@(0.0.255) char]_`*[*@3 t], [@(0.0.255) double]_[*@3 x], [@(0.0.255) int]_[*@3 precision],
 [_^Upp`:`:dword^ dword]_[*@3 flags])&]
[s5;:Upp`:`:FormatF`(double`,int`,Upp`:`:dword`):%- [_^Upp`:`:String^ String]_[* FormatF](
[@(0.0.255) double]_[*@3 x], [@(0.0.255) int]_[*@3 precision], [_^Upp`:`:dword^ dword]_[*@3 f
lags])&]
[s2; С дефолтными флагами [%-*@3 flags] предоставляет 
форматирование double, эквивалентное 
формату %f у printf. Первый вариант требует 
буфер размером  [%-*@3 precision] `+ 400 и возвращает 
указатель за последний произведённый 
символ (оканчивающий `'`\0`' НЕ приставляется).&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:FormatG`(char`*`,double`,int`,Upp`:`:dword`):%- [@(0.0.255) char]_`*[* Format
G]([@(0.0.255) char]_`*[*@3 t], [@(0.0.255) double]_[*@3 x], [@(0.0.255) int]_[*@3 precision],
 [_^Upp`:`:dword^ dword]_[*@3 flags])&]
[s5;:Upp`:`:FormatG`(double`,int`,Upp`:`:dword`):%- [_^Upp`:`:String^ String]_[* FormatG](
[@(0.0.255) double]_[*@3 x], [@(0.0.255) int]_[*@3 precision], [_^Upp`:`:dword^ dword]_[*@3 f
lags])&]
[s2; С дефолтными флагами [%-*@3 flags] предоставляет 
форматирование double, эквивалентное 
формату %g у printf. Первый вариант требует 
буфер размером  [%-*@3 precision] `+ 30 и возвращает 
указатель за последний произведённый 
символ (оканчивающий `'`\0`' НЕ приставляется).&]
[s0; &]
[s4;%- &]
[s5;:FormatBool`(bool`):%- [_^String^ String]_[* FormatBool]([@(0.0.255) bool]_[*@3 a])&]
[s2; Форматирует булево [%-*@3 a] как одно 
из фиксированных слов `"true`" или `"false`" 
(несмотря на языковые установки).&]
[s3; &]
[s3; &]
[s4;%- &]
[s5;:FormatIntBase`(int`,int`,int`,char`,int`):%- [_^String^ String]_[* FormatIntBase]([@(0.0.255) i
nt]_[*@3 i], [@(0.0.255) int]_[*@3 base], [@(0.0.255) int]_[*@3 width]_`=_[@3 0], 
[@(0.0.255) char]_[*@3 lpad]_`=_`'_`', [@(0.0.255) int]_[*@3 sign]_`=_[@3 0])&]
[s2;%- Форматирует целое [*@3 i ]с заданной 
числовой основой [*@3 base], result [*@3 width]_charters 
wide with [*@3 lpad]_padding.&]
[s0; ]]