topic "Структура Point";
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
[ {{10000@(113.42.0) [s0; [*@7;4 Структура Point`_]]}}&]
[s3; &]
[s1;:noref:%- [@(0.0.255)3 template][3 _<][@(0.0.255)3 class][3 _][*@4;3 T][@(0.0.255)3 >]&]
[s1;:Point`_`:`:struct:%- [@(0.0.255) struct]_[* Point`_]_:_[@(0.0.255) public]_[*@3 Moveable][@(0.0.255) <
]_[* Point`_][@(0.0.255) <][*@4 T][@(0.0.255) >]_>_&]
[s0; &]
[s0; [* Point`_] `- это генерная структура, описывающая 
двухмерный точечный объект. Горизонтальная 
координата представлена [*@(154.0.0) x][@(154.0.0) , 
]a вертикальная `- [*@(154.0.0) y]. Обе координаты 
типа [*@4 T].&]
[s3; &]
[s0; &]
[s0; Чтобы получить специализированную 
версию [* Point`_], используется одна из 
следующих:&]
[s0; &]
[s0; [* Point]&]
[s5;:Point`:`:typedef:%- [@(0.0.255) typedef]_[_^Point`_^ Point`_][@(0.0.255) <int>]_[* Point
]&]
[s2; Точка с [* int] координатами .&]
[s3; &]
[s4; &]
[s0;%- [* Point16]&]
[s5;:Point16`:`:typedef:%- [@(0.0.255) typedef]_[_^Point`_^ Point`_][@(0.0.255) <][_^int16^ i
nt16][@(0.0.255) >]_[* Point16]&]
[s2; Точка с [* int16][@(0.0.255)  ]координатами .&]
[s3; &]
[s4;*@(154.0.0) &]
[s0; [* Point64]&]
[s5;:Point64`:`:typedef:%- [@(0.0.255) typedef]_[_^Point`_^ Point`_][@(0.0.255) <][_^int64^ i
nt64][@(0.0.255) >]_[* Point64]&]
[s2; Точка с [* int64][@(0.0.255)  ]координатами .&]
[s3; &]
[s4;*@(154.0.0) &]
[s0;%- [* Pointf]&]
[s5;:Pointf`:`:typedef:%- [@(0.0.255) typedef]_[_^Point`_^ Point`_][@(0.0.255) <double>]_[* P
ointf]&]
[s2; Точка с [* double][@(0.0.255)  ]координатами .&]
[s3; &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0; [* Список Конструкторов]]}}&]
[s3;%- &]
[s5;:Point`_`:`:Point`_`(`):%- [* Point`_]()&]
[s2; Создаёт неинициализированную [%-* Point`_].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Point`_`(const Value`&`):%- [* Point`_]([@(0.0.255) const]_[_^Value^ Value][@(0.0.255) `&
]_[*@3 src])&]
[s2; Дефолтный копи`-конструктор.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Point`_`(T`,T`):%- [* Point`_]([*@4 T]_[*@3 x], [*@4 T]_[*@3 y])&]
[s2; Создаёт объект [%-* Point`_ ][%- и инициализует 
его горизонтальную координату в] 
[%-*@3 x][%- , а вертикальную в ][%-*@3 y].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Point`_`(const Point`_`<int`>`&`):%- [* Point`_]([@(0.0.255) const]_[* Poin
t`_][@(0.0.255) <int>`&]_[*@3 pt])&]
[s2; Создаёт объект [%-* Point`_ ][%- и инициализует 
его в] [%-* Point ][*@3 sz].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Point`_`(const Point`_`<short`>`&`):%- [* Point`_]([@(0.0.255) const]_[* Po
int`_][@(0.0.255) <short>`&]_[*@3 pt])&]
[s2; Создаёт объект [%-* Point`_ ][%- и инициализует 
его в] [%-* Point16 ][*@3 sz].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Point`_`(const Point`_`<double`>`&`):%- [* Point`_]([@(0.0.255) const]_[* P
oint`_][@(0.0.255) <double>`&]_[*@3 pt])&]
[s2; Создаёт объект [%-* Point`_ ][%- и инициализует 
его в] [%-* Pointf ][*@3 sz].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Point`_`(const Point`_`<int64`>`&`):%- [* Point`_]([@(0.0.255) const]_[* Po
int`_][@(0.0.255) <][_^int64^ int64][@(0.0.255) >`&]_[*@3 pt])&]
[s2; Создаёт объект [%-* Point`_ ][%- и инициализует 
его в] [%-* Point64 ][%-*@3 sz].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Point`_`(const Size`_`<T`>`&`):%- [* Point`_]([@(0.0.255) const]_[_^Size`_^ S
ize`_][@(0.0.255) <][*@4 T][@(0.0.255) >`&]_[*@3 sz])&]
[s2; Создаёт объект [%-* Point`_ ][%- и инициализует 
его в] [* Size] [%-*@3 sz].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Point`_`(const Nuller`&`):%- [* Point`_]([@(0.0.255) const]_[_^Nuller^ Null
er][@(0.0.255) `&])&]
[s2; Создаёт объект [%-* Point`_ ][%- и инициализует 
его в] [* Null].&]
[s3; &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Полей]]}}&]
[s3;%- &]
[s5;:Point`_`:`:x:%- [*@4 T]_[* x]&]
[s2; горизонтальная координата&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:y:%- [*@4 T]_[* y]&]
[s2; вертикальная координата&]
[s3; &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0; [* Method List]]}}&]
[s3;%- &]
[s5;:Point`_`:`:Clear`(`):%- [@(0.0.255) void]_[* Clear]()&]
[s2; Устанавливает обе координаты в 0.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:IsZero`(`)const:%- [@(0.0.255) bool]_[* IsZero]()_[@(0.0.255) const]&]
[s2; Возвращает [* true], если обе координаты 
равны 0, [* false] иначе.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:SetNull`(`):%- [@(0.0.255) void]_[* SetNull]()&]
[s2; Реинициализует объект в [* Null].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:IsNullInstance`(`)const:%- [@(0.0.255) bool]_[* IsNullInstance]()_[@(0.0.255) c
onst]&]
[s2; Проверяет, является ли точка [* Null].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Offset`(T`,T`):%- [@(0.0.255) void]_[* Offset]([*@4 T]_[*@3 dx], 
[*@4 T]_[*@3 dy])&]
[s2; Смещает координаты на [%-*@3 dx][%-*  и] [%-*@3 dy] 
соответственно.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:GetHashValue`(`)const:%- [@(0.0.255) unsigned]_[* GetHashValue]()_[@(0.0.255) c
onst]&]
[s2; Возвращает хэш`-значение этой точки.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:ToString`(`)const:%- [_^String^ String]_[* ToString]()_[@(0.0.255) const]&]
[s2; Возвращает строчное представление 
этой точки.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`+`=`(Point`_`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator`+
`=]([_^Point`_^ Point`_]_[*@3 p])&]
[s2; Увеличивает [* x] на горизонтальную 
координату [*@3 p][%- , а ][* y] на её вертикальную 
координату.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`+`=`(Point`_`:`:Sz`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* ope
rator`+`=]([_^Size`_^ Size`_][@(0.0.255) <][*@4 T][@(0.0.255) >]_[*@3 p])&]
[s2; Увеличивает [* x ]на горизонтальный 
размер [*@3 p], а [* y ]на её вертикальный 
размер.&]
[s0; &]
[s4;%- &]
[s5;:Point`_`:`:operator`+`=`(T`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator`+`=]([*@4 T
]_[*@3 t])&]
[s2; Увеличивает обе координаты на одинаковое 
значение [*@3 t].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`-`=`(Point`_`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator`-
`=]([_^Point`_^ Point`_]_[*@3 p])&]
[s2; Уменьшает [* x] на горизонтальную координату 
[*@3 p][%- , а ][* y] на её вертикальную координату.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`-`=`(Point`_`:`:Sz`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* ope
rator`-`=]([_^Size`_^ Size`_][@(0.0.255) <][*@4 T][@(0.0.255) >]_[*@3 p])&]
[s2; Уменьшает [* x ]на горизонтальный размер 
[*@3 p], а [* y ]на её вертикальный размер.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`-`=`(T`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator`-`=]([*@4 T
]_[*@3 t])&]
[s2; Уменьшает обе координаты на одинаковое 
значение [*@3 t].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`*`=`(Point`_`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator`*
`=]([_^Point`_^ Point`_]_[*@3 p])&]
[s2; Умножает [* x] на горизонтальную координату 
[*@3 p][%- , а ][* y] на её вертикальную координату.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`*`=`(Point`_`:`:Sz`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* ope
rator`*`=]([_^Size`_^ Size`_][@(0.0.255) <][*@4 T][@(0.0.255) >]_[*@3 p])&]
[s2; Умножает [* x ]на горизонтальный размер 
[*@3 p], а [* y ]на её вертикальный размер.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`*`=`(T`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator`*`=]([*@4 T
]_[*@3 t])&]
[s2; Умножает обе координаты на одинаковое 
значение [*@3 t].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`/`=`(Point`_`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator/
`=]([_^Point`_^ Point`_]_[*@3 p])&]
[s2; Делит [* x] на горизонтальную координату 
[*@3 p][%- , а ][* y] на её вертикальную координату.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`/`=`(Point`_`:`:Sz`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* ope
rator/`=]([_^Size`_^ Size`_][@(0.0.255) <][*@4 T][@(0.0.255) >]_[*@3 p])&]
[s2; Делит [* x ]на горизонтальный размер 
[*@3 p], а [* y ]на её вертикальный размер.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`/`=`(T`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator/`=]([*@4 T
]_[*@3 t])&]
[s2; Делит обе координаты на одинаковое 
значение [*@3 t].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`<`<`=`(int`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator<<`=
]([@(0.0.255) int]_[*@3 sh])&]
[s2; Сдвигает обе координаты влево на 
[*@3 sh].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`>`>`=`(int`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator>>`=
]([@(0.0.255) int]_[*@3 sh])&]
[s2; Сдвигает обе координаты вправо на 
[*@3 sh].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`+`+`(`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator`+`+]()&]
[s2; Инкрементирует обе координаты.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator`-`-`(`):%- [_^Point`_^ Point`_][@(0.0.255) `&]_[* operator`-`-]()&]
[s2; Декрементирует обе координаты.&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:operator Value`(`)const:%- [* operator_Value]()_[@(0.0.255) const]&]
[s2; Возвращает преобразование этой точки 
в [* Value].&]
[s3; &]
[s4;%- &]
[s5;:Point`_`:`:Serialize`(Stream`&`):%- [@(0.0.255) void]_[* Serialize]([_^Stream^ Stream][@(0.0.255) `&
]_[*@3 s])&]
[s2; Сериализует точку в поток [*@(141.42.0) s].&]
[s3; &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0; [* Френд`-список]]}}&]
[s3; &]
[s5;:`:`:Point`_`:`:operator`+`(`:`:Point`_`):%- friend_[^`:`:Point`_^ Point`_]_operato
r`+[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 p][@(64) )]&]
[s2; Возвращает неизменённую точку [%-*@3 s].&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:operator`-`(`:`:Point`_`):%- friend_[^`:`:Point`_^ Point`_]_operato
r`-[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 p][@(64) )]&]
[s2; Возвращает отрицательную точку [%-*@3 s].&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:operator`+`(`:`:Point`_`,`:`:Point`_`):%- friend_[^`:`:Point`_^ Poi
nt`_]_operator`+[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][^`:`:Point`_^@(64) Point`_][@(64) _][@3 b][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`+`(`:`:Point`_`,`:`:Point`_`:`:Sz`):%- friend_[^`:`:Point`_^ P
oint`_]_operator`+[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][^topic`:`/`/Core`/src`/Size`$en`-us^@(64) Size`_][@(64) <][@4 T][@(64) >_][@3 b][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`+`(`:`:Point`_`,T`):%- friend_[^`:`:Point`_^ Point`_]_oper
ator`+[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , T_][@3 t][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`+`(T`,`:`:Point`_`):%- friend_[^`:`:Point`_^ Point`_]_oper
ator`+[@(64) (T_][@3 t][@(64) , ][^`:`:Point`_^@(64) Point`_][@(64) _][@3 b][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`+`(`:`:Point`_`:`:Sz`,`:`:Point`_`):%- friend_[^topic`:`/`/Core`/src`/Size`$en`-us^ S
ize`_]<[@4 T][@(64) >_]operator`+[@(64) (][^topic`:`/`/Core`/src`/Size`$en`-us^@(64) Size
`_][@(64) <][@4 T][@(64) >_][@3 a][@(64) , ][^`:`:Point`_^@(64) Point`_][@(64) _][@3 b][@(64) )]&]
[s2; Возвращает итог сложения двух значений 
[* Point`_], [* Point`_ ]с [* Size`_ ]или [* Point`_ ]с единичным 
значением.&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:operator`-`(`:`:Point`_`,`:`:Point`_`):%- friend_[^topic`:`/`/Core`/src`/Size`$en`-us^ S
ize`_]<[@4 T][@(64) >_]operator`-[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][^`:`:Point`_^@(64) Point`_][@(64) _][@3 b][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`-`(`:`:Point`_`,`:`:Point`_`:`:Sz`):%- friend_[^`:`:Point`_^ P
oint`_]_operator`-[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][^topic`:`/`/Core`/src`/Size`$en`-us^@(64) Size`_][@(64) <][@4 T][@(64) >_][@3 b][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`-`(`:`:Point`_`,T`):%- friend_[^`:`:Point`_^ Point`_]_oper
ator`-[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , T_][@3 t][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`-`(`:`:Point`_`:`:Sz`,`:`:Point`_`):%- friend_[^topic`:`/`/Core`/src`/Size`$en`-us^ S
ize`_]<[@4 T][@(64) >_]operator`-[@(64) (][^topic`:`/`/Core`/src`/Size`$en`-us^@(64) Size
`_][@(64) <][@4 T][@(64) >_][@3 a][@(64) , ][^`:`:Point`_^@(64) Point`_][@(64) _][@3 b][@(64) )]&]
[s2; Возвращает итог вычитания двух значений 
[* Point`_], [* Point`_ ]с [* Size`_ ]или [* Point`_ ]с единичным 
значением.&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:operator`*`(`:`:Point`_`,`:`:Point`_`):%- friend_[^`:`:Point`_^ Poi
nt`_]_operator`*[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][^`:`:Point`_^@(64) Point`_][@(64) _][@3 b][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`*`(`:`:Point`_`,T`):%- friend_[^`:`:Point`_^ Point`_]_oper
ator`*[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , T_][@3 b][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`*`(T`,`:`:Point`_`):%- friend_[^`:`:Point`_^ Point`_]_oper
ator`*[@(64) (T_][@3 a][@(64) , ][^`:`:Point`_^@(64) Point`_][@(64) _][@3 b][@(64) )]&]
[s2; Возвращает итог умножения двух значений 
[* Point`_ ]или [* Point`_ ]с единичным значением.&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:operator`/`(`:`:Point`_`,`:`:Point`_`:`:Sz`):%- friend_[^`:`:Point`_^ P
oint`_]_operator/[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][^`:`:Point`_`:`:Sz^@(64) Sz][@(64) _][@3 b][@(64) )]&]
[s5;:`:`:Point`_`:`:operator`/`(`:`:Point`_`,T`):%- friend_[^`:`:Point`_^ Point`_]_oper
ator/[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , T_][@3 b][@(64) )]&]
[s2; Возвращает итог деления [* Point`_ ]на 
размер [* Size`_] или [* Point`_ ]с единичным 
значением.&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:operator`<`<`(`:`:Point`_`,int`):%- friend_[^`:`:Point`_^ Point`_]_
operator<<[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][@(0.0.255) int][@(64) _][@3 sh][@(64) )]&]
[s2; Возвращает итог левого сдвига [*@3 a] 
на [*@3 sh].&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:operator`>`>`(`:`:Point`_`,int`):%- friend_[^`:`:Point`_^ Point`_]_
operator>>[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][@(0.0.255) int][@(64) _][@3 sh][@(64) )]&]
[s2; Возвращает итог правого сдвига [*@3 a] 
на [*@3 sh].&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:operator`=`=`(`:`:Point`_`,`:`:Point`_`):%- friend_[@(0.0.255) bool
][@(64) _]operator`=`=[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][^`:`:Point`_^@(64) Point`_][@(64) _][@3 b][@(64) )]&]
[s2; Возвращает [* true], если [*@3 a ]и [*@3 b ]имеют 
равные координаты, [* false ]иначе.&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:operator`!`=`(`:`:Point`_`,`:`:Point`_`):%- friend_[@(0.0.255) bool
][@(64) _]operator!`=[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , 
][^`:`:Point`_^@(64) Point`_][@(64) _][@3 b][@(64) )]&]
[s2; Возвращает [* true], если [*@3 a ]и [*@3 b ]имеют 
неравные координаты , [* false ]иначе.&]
[s3; &]
[s4; &]
[s5;:`:`:Point`_`:`:min`(`:`:Point`_`,`:`:Point`_`):%- friend_[^`:`:Point`_^ Point`_]_m
in[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , ][^`:`:Point`_^@(64) Point`_][@(64) _
][@3 b][@(64) )]&]
[s2;%- [%RU-RU Возвращает ][%RU-RU* Point`_][%RU-RU  с координатами,
 равными минимуму соответствующих 
коодинат между ][*@3 a][@3  ]и[%RU-RU  ][*@3 b].&]
[s3;%- &]
[s4;%- &]
[s5;:`:`:Point`_`:`:max`(`:`:Point`_`,`:`:Point`_`):%- friend_[^`:`:Point`_^ Point`_]_m
ax[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , ][^`:`:Point`_^@(64) Point`_][@(64) _
][@3 b][@(64) )]&]
[s2;%- [%RU-RU Возвращает ][%RU-RU* Point`_][%RU-RU  с координатами,
 равными максимуму соответствующих 
коодинат между ][*@3 a][@3  ]и[%RU-RU  ][*@3 b].&]
[s3;%- &]
[s4; &]
[s5;:`:`:Point`_`:`:Nvl`(`:`:Point`_`,`:`:Point`_`):%- friend_[^`:`:Point`_^ Point`_]_N
vl[@(64) (][^`:`:Point`_^@(64) Point`_][@(64) _][@3 a][@(64) , ][^`:`:Point`_^@(64) Point`_][@(64) _
][@3 b][@(64) )]&]
[s2; Возвращает [%-*@3 b], если [%-*@3 a] равно Null, 
[%-*@3 a] иначе.&]
[s3; &]
[s0; ]]