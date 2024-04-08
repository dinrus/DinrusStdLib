topic "Класс Color";
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
[ {{10000@(113.42.0) [s0; [*@7;4 Класс ][*@(229)4 Color (Цвет)]]}}&]
[s3;%- &]
[s1;:Color`:`:class:%- [@(0.0.255)3 class][3 _][*3 Color][3 _:_][@(0.0.255)3 private][3 _][*@3;3 Ass
ignValueTypeNo][@(0.0.255)3 <][*3 Color][3 , COLOR`_V, ][_^Moveable^3 Moveable][@(0.0.255)3 <
][*3 Color][@(0.0.255)3 >][3 _>_]&]
[s9; Тип [* Color] представляет цвет RGB (КЗС). 
Все его компоненты находятся в диапазоне 
от 0 до 255. Значение Null для Color определено. 
Color совместим с Value как тип Rich`-Value (поддержива
ется сравнение значений, сериализация 
и проч.).&]
[s3; &]
[s0;%- &]
[ {{10000F(128)G(128)@1 [s0; [* Детали Конструктора]]}}&]
[s3; &]
[s5;:Color`:`:Color`(`):%- [* Color]()&]
[s2; Дефолтный конструктор. Оставляет 
значения компонентов неинициализованными.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:Color`(int`,int`,int`):%- [* Color]([@(0.0.255) int]_[*@3 r], 
[@(0.0.255) int]_[*@3 g], [@(0.0.255) int]_[*@3 b])&]
[s2; Конструктор.&]
[s7; [%-*C@3 r]-|Красный канал.&]
[s7; [%-*C@3 g]-|Зелёный канал.&]
[s7; [%-*C@3 b]-|Синий канал.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:Color`(const Nuller`&`):%- [* Color]([@(0.0.255) const]_[_^Nuller^ Nuller][@(0.0.255) `&
])&]
[s2; Конструктор Null.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:Color`(const Value`&`):%- [* Color]([@(0.0.255) const]_[_^Value^ Value][@(0.0.255) `&
]_[*@3 q])&]
[s2; Преобразование из Value.&]
[s7; [%-*@3 q]-|Значение.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:Color`(Color`(`*`)`(`)`):%- [* Color]([* Color]_(`*[*@3 fn])())&]
[s6;%- `[депрекирован`]&]
[s2; Этот конструктор  позволяет использовать 
указатель на функцию, возвращающую 
Colors, вместо Colors во многих местах. Глобальные 
функции, возвращающие Color, а не глобальные 
константы цвета, требуются на многих 
платформах, поскольку при использовании 
динамических библиотек глобальные 
переменные запрещены.&]
[s7; [%-*@3 fn]-|Указатель на функцию, возвращающую 
Color.&]
[s0; &]
[s4;%- &]
[s5;:Color`:`:Color`(RGBA`):%- [* Color]([_^RGBA^ RGBA]_[*@3 rgba])&]
[s2; Преобразование из структуры RGBA [%-*@3 rgba].&]
[s3; &]
[s0;%- &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Членов]]}}&]
[s3; &]
[s5;:Color`:`:operator Value`(`)const:%- [* operator_Value]()_[@(0.0.255) const]&]
[s2; Преобразование в Value.&]
[s7; [*/ Возвратное значение]-|Значение.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:GetRaw`(`)const:%- [_^dword^ dword]_[* GetRaw]()_[@(0.0.255) const]&]
[s7; [*/ Возвратное значение]-|Платформо`-специфичны
й dword, представляющий этот цвет.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:GetR`(`)const:%- [@(0.0.255) int]_[* GetR]()_[@(0.0.255) const]&]
[s7; [*/ Возвратное значение]-|Красная составляющая
.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:GetG`(`)const:%- [@(0.0.255) int]_[* GetG]()_[@(0.0.255) const]&]
[s7; [*/ Возвратное значение]-|Зелёная составляющая
.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:GetB`(`)const:%- [@(0.0.255) int]_[* GetB]()_[@(0.0.255) const]&]
[s7; [*/ Возвратное значение]-|Синяя составляющая.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:SetNull`(`):%- [@(0.0.255) void]_[* SetNull]()&]
[s2; Устанавливает этот Color в Null.&]
[s3;%- &]
[s4;%- &]
[s5;:Color`:`:IsNullInstance`(`)const:%- [@(0.0.255) bool]_[* IsNullInstance]()_[@(0.0.255) c
onst]&]
[s7; [*/ Возвратное значение]-|True, если Color 
равен Null. Используется для реализации 
IsNull для Color.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:GetHashValue`(`)const:%- [@(0.0.255) unsigned]_[* GetHashValue]()_[@(0.0.255) c
onst]&]
[s7; [*/ Возвратное значение]-|Хэш`-значение 
для Color.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:operator`=`=`(Color`)const:%- [@(0.0.255) bool]_[* operator`=`=]([_^Color^ Co
lor]_[*@3 c])_[@(0.0.255) const]&]
[s7; [%-*C@3 c]-|Сравниваемый цвет.&]
[s7; [*/ Возвратное значение]-|True, если Color 
равен [@3 c].&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:operator`!`=`(Color`)const:%- [@(0.0.255) bool]_[* operator!`=]([_^Color^ Col
or]_[*@3 c])_[@(0.0.255) const]&]
[s7; [%-*C@3 c]-|Сравниваемый цвет.&]
[s7; [*/ Возвратное значение]-|True,если Color 
не равен [@3 c].&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:Serialize`(Stream`&`):%- [@(0.0.255) void]_[* Serialize]([_^Stream^ Stream][@(0.0.255) `&
]_[*@3 s])&]
[s2; Сериализует этот цвет в поток [^topic`:`/`/Core`/src`/Stream`_ru`-ru`#Stream`:`:class^ S
tream] [%-*@3 s].&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:FromRaw`(dword`):%- [@(0.0.255) static] [_^Color^ Color]_[* FromRaw]([_^dword^ d
word]_[*@3 co])&]
[s2; Создаёт Color, основанный на платформо`-специф
ичном `"сыром`" значении dword (бцел).&]
[s7; [%-*C@3 co]-|Сырое бцел`-значение.&]
[s7; [*/ Возвратное значение]-|Color.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:Color`:`:Special`(int`):%- [@(0.0.255) static] [_^Upp`:`:Color^ Color]_[* Speci
al]([@(0.0.255) int]_[*@3 n])&]
[s2; Создаёт особый цвет [%-*@3 n]. Спеццвета 
определены приложением, наподобии 
цвета `"escape`" (`"обманки`") для тех ситуаций, 
когда в Цвете нужно сохранить нечто 
иное, например, индекс какой`-то цветовой 
палитры.&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:Color`:`:GetSpecial`(`)const:%- [@(0.0.255) int]_[* GetSpecial]()_[@(0.0.255) c
onst]&]
[s2; Возвращает значение спеццвета или 
`-1, если Color не спец.&]
[s3;%- &]
[s4;%- &]
[s5;:Color`:`:operator COLORREF`(`)const:%- [* operator_COLORREF]()_[@(0.0.255) const]&]
[s6;%- Специфично для Win32.&]
[s2; Возвращает COLORREF для Color.&]
[s7; [*/ Возвратное значение]-|COLORREF.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:FromCR`(COLORREF`):%- [@(0.0.255) static] [_^Color^ Color]_[* FromCR](COLORRE
F_[*@3 cr])&]
[s6;%- Специфично для Win32.&]
[s2; Создаёт Color из COLORREF.&]
[s7; [%-*C@3 cr]-|COLORREF.&]
[s7; [*/ Возвратное значение]-|Color.&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:operator dword`(`)const:%- [* operator_dword]()_[@(0.0.255) const]&]
[s6;%- Недоступно на Win32&]
[s7; [*/ Возвратное значение]-|[^topic`:`/`/Core`/src`/Color`$en`-us`#Color`:`:GetRaw`(`)const^ G
etRaw]().&]
[s3; &]
[s4;%- &]
[s5;:Color`:`:operator RGBA`(`)const:%- [* operator_RGBA]()_[@(0.0.255) const]&]
[s2; Возвращает структуру RGBA, содержащую 
цветовую информацию.&]
[s3;%- &]
[s0; &]
[s0; &]
[s0;%- &]
[ {{10000@(113.42.0) [s0; [*@(229)4 Утилитные Функции]]}}&]
[s3;%- &]
[s5;:GetRValue`(dword`):%- [@(0.0.255) int]_[* GetRValue]([_^dword^ dword]_[*@3 c])&]
[s2; Возвращает красную составляющую 
из значения, специфичного для платформы.&]
[s7; [%-*C@3 c]-|Платформно`-специфичное значение.&]
[s7; [*/ Возвратное значение]-|Красная составляющая
.&]
[s3; &]
[s4;%- &]
[s5;:GetGValue`(dword`):%- [@(0.0.255) int]_[* GetGValue]([_^dword^ dword]_[*@3 c])&]
[s2; Возвращает зелёную составляющую 
из значения, специфичного для платформы.&]
[s7; [%-*C@3 c]-|Платформно`-специфичное значение&]
[s7; [*/ Возвратное значение]-|Зелёная составляющая
.&]
[s3; &]
[s4;%- &]
[s5;:GetBValue`(dword`):%- [@(0.0.255) int]_[* GetBValue]([_^dword^ dword]_[*@3 c])&]
[s2; Возвращает синюю составляющую из 
значения, специфичного для платформы.&]
[s7; [%-*C@3 c]-|Платформно`-специфичное значение&]
[s7; [*/ Возвратное значение]-|Синяя составляющая.&]
[s3; &]
[s4;%- &]
[s5;:RGB`(byte`,byte`,byte`):%- [_^dword^ dword]_[* RGB]([_^byte^ byte]_[*@3 r], 
[_^byte^ byte]_[*@3 g], [_^byte^ byte]_[*@3 b])&]
[s2; Вычисляет платформно`-специфичное 
значение из составляющих.&]
[s7; [%-*C@3 r]-|Красная составляющая.&]
[s7; [%-*C@3 g]-|Зелёная составляющая.&]
[s7; [%-*C@3 b]-|Синяя составляющая.&]
[s7; [*/ Возвратное значение]-|Платформно`-специфичн
ое значение&]
[s3; &]
[s4;%- &]
[s5;:GetHashValue`(Color`):%- [@(0.0.255) unsigned]_[* GetHashValue]([_^Color^ Color]_[*@3 c])
&]
[s2; Возвращает хэш`-значение для цвета.&]
[s7; [%-*C@3 c]-|Color.&]
[s7; [*/ Возвратное значение]-|Хэш`-значение&]
[s3; &]
[s4;%- &]
[s5;:Nvl`(Color`,Color`):%- [_^Color^ Color]_[* Nvl]([_^Color^ Color]_[*@3 a], 
[_^Color^ Color]_[*@3 b])&]
[s2; Возвращает первый параметр, если 
он не Null, иначе второй.&]
[s7; [%-*C@3 a]-|Color.&]
[s7; [%-*C@3 b]-|Color.&]
[s7; [*/ Возвратное значение]-|a, если не Null, 
иначе b.&]
[s3; &]
[s4;%- &]
[s5;:AsString`(const Color`&`):%- [@(0.0.255) template]_<>_[_^String^ String]_[* AsString](
[@(0.0.255) const]_[_^Color^ Color][@(0.0.255) `&]_[*@3 c])&]
[s2; Преобразует Color в текстовый формат.&]
[s7; [%-*C@3 c]-|Color.&]
[s7; [*/ Возвратное значение]-|Текстуальная 
форма.&]
[s3; &]
[s4;%- &]
[s5;:RGBtoHSV`(double`,double`,double`,double`&`,double`&`,double`&`):%- [@(0.0.255) vo
id]_[* RGBtoHSV]([@(0.0.255) double]_[*@3 r], [@(0.0.255) double]_[*@3 g], 
[@(0.0.255) double]_[*@3 b], [@(0.0.255) double`&]_[*@3 h], [@(0.0.255) double`&]_[*@3 s], 
[@(0.0.255) double`&]_[*@3 v])&]
[s2; Трансформирует цветовой триплет 
`[R,G,B`] (каждая составляющая предположительно
 находится в диапазоне `[0..1`]) в триплет 
`[H,S,V`] (опять же, все составляющие в 
диапазоне `[0..1`]). Это эффективно трансформиру
ет куб RGB в конус, у которого выбор 
цвета более интуитивно соответствует 
нашему зрительному восприятию ([/ hue] 
соответствует оттенку цвета (tint of 
the color), [/ saturation] `- насыщенности цветового 
оттенка (the richness of the color hue), а [/ value] соответству
ет светимости (luminance) или яркости (brightness). 
&]
[s2; &]
[s2; Математическая формула, используемая 
для преобразования между этими двумя 
цветовыми пространствами, такая:&]
[s2; &]
[s2; v `= max(r, g, b)&]
[s2; s `= 1 `- min(r, g, b) / v&]
[s2; h `= (x `- min(r, g, b)) / (max(r, g, b) `- min(r, g, b))&]
[s2; &]
[s2; где [/ x] `- цветовая составляющая, не 
являющаяся ни минимальной, ни максимальной. 
Для этого есть шесть комбинаций, в 
каждой из которых чётное принимает 
h, а в каждом нечётном 1`-h. Эти шесть 
комбинаций соответствуют шести точкам 
на цветовом круге (колесе), формирующим 
основание конуса HSV, `- чистые красный, 
жёлтый, зелёный, cyan, синий и magenta. Диапазоны 
этих цветовых составляющих вдоль 
окружности следующие (заглавные буквы 
обозначают максимальное значение 
составляющей, а маленькие `- минимальное): 
Rgb `-> RGb `-> rGb `-> rGB `-> rgB `-> RgB.&]
[s2; &]
[s2; Простой способ визуализации трансформа 
HSV, основанного на кубе RGB, таков: вообразите 
себе некую деформацию куба, где верхушка 
конуса соответствует чёрной вершине 
(vertex) куба RBG, противоположная (белая) 
вершина куба RGB соответствует центру 
базового круга конуса HSV, а остальные 
шесть вершин куба RGB проецируются 
на окружность основания конуса, как 
поясняется в предыдущем параграфе.&]
[s3; &]
[s4;%- &]
[s5;:HSVtoRGB`(double`,double`,double`,double`&`,double`&`,double`&`):%- [@(0.0.255) vo
id]_[* HSVtoRGB]([@(0.0.255) double]_[*@3 h], [@(0.0.255) double]_[*@3 s], 
[@(0.0.255) double]_[*@3 v], [@(0.0.255) double`&]_[*@3 r], [@(0.0.255) double`&]_[*@3 g], 
[@(0.0.255) double`&]_[*@3 b])&]
[s2; Трансформирует триплет HSV (у которого 
каждая компонента предположительно 
находится в интервале `[0..1`]) в триплет 
RGB (опять`-таки, с диапазонами составляющих 
`[0..1`]). Это проецирует конус HSV (как 
объяснено в предыдущем разделе) обратно 
в куб RGB.&]
[s3; &]
[s4;%- &]
[s5;:HsvColorf`(double`,double`,double`):%- [_^Color^ Color]_[* HsvColorf]([@(0.0.255) doub
le]_[*@3 h], [@(0.0.255) double]_[*@3 s], [@(0.0.255) double]_[*@3 v])&]
[s2; Преобразует триплет цвета HSV в объект 
[* Color]. Предполагается, что составляющие 
H, S и V находятся в диапазоне `[0..1`].&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:CMYKtoRGB`(double`,double`,double`,double`,double`&`,double`&`,double`&`):%- [@(0.0.255) v
oid]_[* CMYKtoRGB]([@(0.0.255) double]_[*@3 c], [@(0.0.255) double]_[*@3 m], 
[@(0.0.255) double]_[*@3 y], [@(0.0.255) double]_[*@3 k], [@(0.0.255) double`&]_[*@3 r], 
[@(0.0.255) double`&]_[*@3 g], [@(0.0.255) double`&]_[*@3 b])&]
[s2; Преобразует определение цвета CMYK 
в RGB (все составляющие в диапазоне 
`[0..1`]).&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:RGBtoCMYK`(double`,double`,double`,double`&`,double`&`,double`&`,double`&`):%- [@(0.0.255) v
oid]_[* RGBtoCMYK]([@(0.0.255) double]_[*@3 r], [@(0.0.255) double]_[*@3 g], 
[@(0.0.255) double]_[*@3 b], [@(0.0.255) double`&]_[*@3 c], [@(0.0.255) double`&]_[*@3 m], 
[@(0.0.255) double`&]_[*@3 y], [@(0.0.255) double`&]_[*@3 k])&]
[s2; Преобразует определение цвета RGB 
в CMYK (все составляющие в диапазоне 
`[0..1`]).&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:CmykColorf`(double`,double`,double`,double`):%- [_^Upp`:`:Color^ Color]_[* Cm
ykColorf]([@(0.0.255) double]_[*@3 c], [@(0.0.255) double]_[*@3 m], [@(0.0.255) double]_[*@3 y
], [@(0.0.255) double]_[*@3 k])&]
[s2; Преобразует определение цвета CMYK 
в RGB (все составляющие в диапазоне 
`[0..1`]).&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:CmyColorf`(double`,double`,double`):%- [_^Upp`:`:Color^ Color]_[* CmyColorf](
[@(0.0.255) double]_[*@3 c], [@(0.0.255) double]_[*@3 m], [@(0.0.255) double]_[*@3 y])&]
[s2; Преобразует определение цвета CMY 
в RGB (все составляющие в диапазоне 
`[0..1`]).&]
[s3; &]
[s4;%- &]
[s5;:Blend`(Color`,Color`,int`):%- [_^Color^ Color]_[* Blend]([_^Color^ Color]_[*@3 c1], 
[_^Color^ Color]_[*@3 c2], [@(0.0.255) int]_[*@3 alpha]_`=_[@3 128])&]
[s2; Смешивает два цвета.&]
[s7; [%-*C@3 c1]-|Первый цвет.&]
[s7; [%-*C@3 c2]-|Второй цвет.&]
[s7; [%-*C@3 alpha]-|Фактор смешивания в диапазоне 
0..255.&]
[s7; [*/ Возвратное значение]-|Смешанный 
цвет `- (255 `- alpha) / 255.0 `* c1 `+ alpha / 255.0 `* 
c2.&]
[s3; &]
[s4;%- &]
[s5;:ColorToHtml`(Color`):%- [_^String^ String]_[* ColorToHtml]([_^Color^ Color]_[*@3 color])
&]
[s2; Преобразует Color в текстуальный формат, 
используемый в HTML (в шестнадцатиричную 
форму, типа #ffffff  для белого цвета).&]
[s7; [%-*C@3 color]-|Color.&]
[s7; [*/ Возвратное значение]-|Текст HTML.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:ColorFromText`(const char`*`):%- [_^Upp`:`:Color^ Color]_[* ColorFromText]([@(0.0.255) c
onst]_[@(0.0.255) char]_`*[*@3 s])&]
[s2; Преобразует текст в Color. Допустимые 
форматы `- 3 десятичных числа в диапазоне 
0`-255, представляющие цвета r, g, b и разделённые
 либо `',`', либо `';`', либо `'.`', либо `' 
`', либо же текст должен содержать 
ровно 6 шестнадцатиричных цифр, представляющ
их значения RRGGBB.&]
[s3; &]
[s4;%- &]
[s5;:GrayColor`(int`):%- [_^Color^ Color]_[* GrayColor]([@(0.0.255) int]_[*@3 a]_`=_[@3 128])&]
[s2; Возвращает серый цвет с интенсивностью 
[%-*@3 a].&]
[s3; &]
[s4;%- &]
[s5;:Grayscale`(const Color`&`):%- [@(0.0.255) int]_[* Grayscale]([@(0.0.255) const]_[_^Color^ C
olor][@(0.0.255) `&]_[*@3 c])&]
[s2; Преобразует цвет [%-*@3 c][%-  в grayscale (оттенок 
серого)].&]
[s3; &]
[s4;%- &]
[s5;:IsDark`(Color`):%- [@(0.0.255) bool]_[* IsDark]([_^Color^ Color]_[*@3 c])&]
[s2; True, если уровень оттенка серого [%-*@3 c] 
ниже 80.&]
[s3; &]
[s4;%- &]
[s5;:IsLight`(Color`):%- [@(0.0.255) bool]_[* IsLight]([_^Color^ Color]_[*@3 c])&]
[s2; True, если уровень оттенка серого [%-*@3 c] 
выше 175 (255 `- 80).&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:DarkTheme`(Upp`:`:Color`):%- [_^Upp`:`:Color^ Color]_[* DarkTheme]([_^Upp`:`:Color^ C
olor]_[*@3 c])&]
[s2; `'Dark Theme conversion`'. Преобразует тёмные 
цвета в светлые, и наоборот, сохраняя 
цветовой оттенок.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:DarkThemeCached`(Upp`:`:Color`):%- [_^Upp`:`:Color^ Color]_[* DarkThemeCached
]([_^Upp`:`:Color^ Color]_[*@3 c])&]
[s2; Кэшированный вариант DarkTheme, кэширует 
до 4`-х повторяющихся цветов.&]
[s3; &]
[s4;%- &]
[s5;:StraightColor`(RGBA`):%- [_^Color^ Color]_[* StraightColor]([_^RGBA^ RGBA]_[*@3 rgba])&]
[s2; Преобразует структуру RGBA [%-*@3 rgba] в 
[* Color].&]
[s3; &]
[s0;3 &]
[ {{10000@(113.42.0) [s0; [*@(229)4 Константы цвета]]}}&]
[s3; &]
[s9; Предопределённые цвета представлены 
функциями, возвращающими предопределённое 
значение цвета.&]
[s3; &]
[s0; &]
[s5;:Black`(`):%- [_^Color^ Color]_[* Black]()&]
[s2; Чёрный.&]
[s3;%- &]
[s4;%- &]
[s5;:Gray`(`):%- [_^Color^ Color]_[* Gray]()&]
[s2; Серый.&]
[s3;%- &]
[s4;%- &]
[s5;:LtGray`(`):%- [_^Color^ Color]_[* LtGray]()&]
[s2; Светло серый.&]
[s3;%- &]
[s4;%- &]
[s5;:WhiteGray`(`):%- [_^Color^ Color]_[* WhiteGray]()&]
[s2; Бело`-серый.&]
[s3;%- &]
[s4;%- &]
[s5;:White`(`):%- [_^Color^ Color]_[* White]()&]
[s2; Белый.&]
[s3;%- &]
[s4;%- &]
[s5;:Red`(`):%- [_^Color^ Color]_[* Red]()&]
[s2; Красный.&]
[s3;%- &]
[s4;%- &]
[s5;:Green`(`):%- [_^Color^ Color]_[* Green]()&]
[s2; Зелёный.&]
[s3;%- &]
[s4;%- &]
[s5;:Brown`(`):%- [_^Color^ Color]_[* Brown]()&]
[s2; Коричневый.&]
[s3;%- &]
[s4;%- &]
[s5;:Blue`(`):%- [_^Color^ Color]_[* Blue]()&]
[s2; Синий.&]
[s3;%- &]
[s4;%- &]
[s5;:Magenta`(`):%- [_^Color^ Color]_[* Magenta]()&]
[s2; Magenta.&]
[s3;%- &]
[s4;%- &]
[s5;:Cyan`(`):%- [_^Color^ Color]_[* Cyan]()&]
[s2; Cyan.&]
[s3;%- &]
[s4;%- &]
[s5;:Yellow`(`):%- [_^Color^ Color]_[* Yellow]()&]
[s2; Жёлтый.&]
[s3; &]
[s4;%- &]
[s5;:LtRed`(`):%- [_^Color^ Color]_[* LtRed]()&]
[s2; Светло`-красный (розовый).&]
[s3;%- &]
[s4;%- &]
[s5;:LtGreen`(`):%- [_^Color^ Color]_[* LtGreen]()&]
[s2; Светло`-зелёный.&]
[s3;%- &]
[s4;%- &]
[s5;:LtYellow`(`):%- [_^Color^ Color]_[* LtYellow]()&]
[s2; Светло`-жёлтый.&]
[s3;%- &]
[s4;%- &]
[s5;:LtBlue`(`):%- [_^Color^ Color]_[* LtBlue]()&]
[s2; Светло`-синий (голубой).&]
[s3;%- &]
[s4;%- &]
[s5;:LtMagenta`(`):%- [_^Color^ Color]_[* LtMagenta]()&]
[s2; Светлая magenta.&]
[s3;%- &]
[s4;%- &]
[s5;:LtCyan`(`):%- [_^Color^ Color]_[* LtCyan]()&]
[s2; Светлый cyan.&]
[s3;%- &]
[s0; ]]