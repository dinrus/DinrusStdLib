topic "Класс InArray";
[i448;a25;kKO9;2 $$1,0#37138531426314131252341829483380:class]
[l288;2 $$2,2#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:item]
[l288;a4;*@5;1 $$6,6#70004532496200323422659154056402:requirement]
[l288;i1121;b17;O9;~~~.1408;2 $$7,0#10431211400427159095818037425705:param]
[i448;b42;O9;2 $$8,8#61672508125594000341940100500538:tparam]
[b42;2 $$9,9#13035079074754324216151401829390:normal]
[2 $$0,0#00000000000000000000000000000000:Default]
[{_}%RU-RU 
[ {{10000@(113.42.0) [s0; [*@7;4 Класс InArray]]}}&]
[s1;@(0.0.255)3%- &]
[s1;:noref:%- [@(0.0.255)3 template][3 _<][@(0.0.255)3 class][3 _][*@4;3 T][3 >]&]
[s1;:InArray`:`:class:%- [@(0.0.255) class]_[* InArray]_:_[@(0.0.255) public]_[*@3 MoveableAnd
DeepCopyOption]<_[_^InVector^ InVector]<[*@4 T]>_>_&]
[s2; InArray является Array`-флейвором [^topic`:`/`/Core`/src`/InVector`_ru`-ru^ I
nVector`'а]. В отличие от InVector, он не требует, 
чтобы элементы были [^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ M
oveable], и даже допускает сохранение 
элементов, [/ производных от] T.&]
[s2; В целом, любой метод, изменяющий число 
элементов в InVector`'е (плюс метод Shrink) 
повреждает все итераторы в InVector, но, 
в отличие от InVector, НЕ повреждает ссылки 
на элементы в нём.&]
[s2; InVector имеет дефолтную семантику пик`-трансфе
ра с опциональной глубокой копией. 
Он является [^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ Moveable].&]
[s0;i448;a25;kKO9;:noref:@(0.0.255)%- &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Методов]]}}&]
[s3;%- &]
[s5;:InArray`:`:Insert`(int`,T`*`):%- [*@4 T][@(0.0.255) `&]_[* Insert]([@(0.0.255) int]_[*@3 i
], [*@4 T]_`*[*@3 newt])&]
[s2; Вставляет новый элемент. Элемент 
определяется указателем на объект, 
созданный с помощью оператора new. 
InArray принимает на себя владение этим 
объектом. Этот вариант допускает 
использование InArray в качестве полиморфного 
контейнера, так как типом добавляемого 
элемента может быть либо T, либо производный 
от T. Конструктор не применяется. Повреждает 
итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Insert`(int`):%- [*@4 T][@(0.0.255) `&]_[* Insert]([@(0.0.255) int]_[*@3 i])&]
[s2; Вставляет дефолтно конструируемый 
элемент по индексу [%-*@3 i]. Повреждает 
итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Insert`(int`,const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Insert]([@(0.0.255) int
]_[*@3 i], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2; Вставляет копию [%-*@3 x] по индексу [%-*@3 i]. 
Повреждает итераторы.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:InArray`:`:InsertCreate`(int`,Args`&`&`.`.`.args`):%- [@(0.0.255) template]_
<[@(0.0.255) class]_[*@4 TT], [@(0.0.255) class...]_[*@4 Args]>_[*@4 TT][@(0.0.255) `&]_[* Inse
rtCreate]([@(0.0.255) int]_[*@3 i], [*@4 Args][@(0.0.255) `&`&...]_args)&]
[s2; Создаёт построенный на месте элемент 
 типа [%-*@4 TT] (с args в качестве параметров 
конструктора) и вставляет его по индексу 
[%-*@3 i].Повреждает итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:InsertN`(int`,int`):%- [@(0.0.255) void]_[* InsertN]([@(0.0.255) int]_[*@3 i],
 [@(0.0.255) int]_[*@3 count])&]
[s2; Вставляет число [%-*@3 count] дефолтно построенных
 элементов по индексу [%-*@3 i]. Повреждает 
итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Remove`(int`,int`):%- [@(0.0.255) void]_[* Remove]([@(0.0.255) int]_[*@3 i], 
[@(0.0.255) int]_[*@3 count]_`=_[@3 1])&]
[s2; Удаляет число [%-*@3 count] элементов, начиная 
с индекса [%-*@3 i]. Повреждает итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:operator`[`]`(int`)const:%- [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[* oper
ator`[`]]([@(0.0.255) int]_[*@3 i])_[@(0.0.255) const]&]
[s5;:InArray`:`:operator`[`]`(int`):%- [*@4 T][@(0.0.255) `&]_[* operator`[`]]([@(0.0.255) in
t]_[*@3 i])&]
[s2; Возвращает элемент по индексу [%-*@3 i].&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Add`(`):%- [*@4 T][@(0.0.255) `&]_[* Add]()&]
[s2; То же, что и Insert(GetCount()). Повреждает 
итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Add`(const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Add]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[*@3 x])&]
[s2; То же, что и Insert(GetCount(), [%-*@3 x]). Повреждает 
итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:AddN`(int`):%- [@(0.0.255) void]_[* AddN]([@(0.0.255) int]_[*@3 n])&]
[s2; То же, что и InsertN(GetCount(), [%-*@3 n]). Повреждает 
итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Add`(T`*`):%- [*@4 T][@(0.0.255) `&]_[* Add]([*@4 T]_`*[*@3 newt])&]
[s2; То же, что и Insert(GetCount(), [%-*@3 newt]). Повреждает 
итераторы.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:InArray`:`:Create`(Args`&`&`.`.`.args`):%- [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 TT], [@(0.0.255) class...]_[*@4 Args]>_[*@4 TT][@(0.0.255) `&]_[* Create]([*@4 Args][@(0.0.255) `&
`&...]_args)&]
[s2; То же, что и InsertCreate<[%-*@4 TT]>(GetCount(), args...). 
Повреждает итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:GetCount`(`)const:%- [@(0.0.255) int]_[* GetCount]()_[@(0.0.255) const]&]
[s2; Возвращает число элементов.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:IsEmpty`(`)const:%- [@(0.0.255) bool]_[* IsEmpty]()_[@(0.0.255) const]&]
[s2; То же, что и GetCount() `=`= 0.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Trim`(int`):%- [@(0.0.255) void]_[* Trim]([@(0.0.255) int]_[*@3 n])&]
[s2; То же, что и Remove(n, GetCount() `- n). Повреждает 
итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:SetCount`(int`):%- [@(0.0.255) void]_[* SetCount]([@(0.0.255) int]_[*@3 n])&]
[s2; Устанавливает число элементов в 
[%-*@3 n], либо удаляя лишние, либо используя 
AddN, чтобы расширить InArray. Повреждает 
итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Clear`(`):%- [@(0.0.255) void]_[* Clear]()&]
[s2; То же, что и Remove(0, GetCount()). Повреждает 
итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:At`(int`):%- [*@4 T][@(0.0.255) `&]_[* At]([@(0.0.255) int]_[*@3 i])&]
[s2; Если [%-*@3 i] >`= GetCount, выполняет SetCount(i 
`+ 1), чтобы элемент по индексу [%-*@3 i] 
гарантированно существовал. В любом 
случае возвращает ссылку на элемент 
по индексу [%-*@3 i]. Повреждает итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Shrink`(`):%- [@(0.0.255) void]_[* Shrink]()&]
[s2; Минимизирует память кучи, размещённую 
InVector`'ом. Повреждает итераторы.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Set`(int`,const T`&`,int`):%- [@(0.0.255) void]_[* Set]([@(0.0.255) int]_[*@3 i
], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x], [@(0.0.255) int]_[*@3 count])&]
[s2; Устанавливает значение числа [%-*@3 count] 
элементов, начиная с индекса [%-*@3 i], 
в [%-*@3 x].&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Set`(int`,const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Set]([@(0.0.255) int]_[*@3 i
], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2; Устанавливает значение элемента 
по индексу [%-*@3 i] в [%-*@3 x].&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Swap`(int`,int`):%- [@(0.0.255) void]_[* Swap]([@(0.0.255) int]_[*@3 i1], 
[@(0.0.255) int]_[*@3 i2])&]
[s2; Меняет местами элементы в позициях 
[%-*@3 i1] и [%-*@3 i2].&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Drop`(int`):%- [@(0.0.255) void]_[* Drop]([@(0.0.255) int]_[*@3 n]_`=_[@3 1])&]
[s2; Удаляет [%-*@3 n] элементов в конце InArray`'я.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Top`(`):%- [*@4 T][@(0.0.255) `&]_[* Top]()&]
[s5;:InArray`:`:Top`(`)const:%- [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[* Top]()_[@(0.0.255) c
onst]&]
[s2; Возвращает ссылку на последний элемент.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Pop`(`):%- [*@4 T]_[* Pop]()&]
[s2; Возвращает копию последнего элемента 
и удаляет его.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:FindUpperBound`(const T`&`,const L`&`)const:%- [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 L]>_[@(0.0.255) int]_[* FindUpperBound]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_
[*@3 val], [@(0.0.255) const]_[*@4 L][@(0.0.255) `&]_[*@3 less])_[@(0.0.255) const]&]
[s5;:InArray`:`:FindUpperBound`(const T`&`)const:%- [@(0.0.255) int]_[* FindUpperBound]([@(0.0.255) c
onst]_[*@4 T][@(0.0.255) `&]_[*@3 val])_[@(0.0.255) const]&]
[s2; Находит верхний предел для [%-*@3 val], 
используя [%-*@3 less] / StdLess<T> в качестве 
предиката сравнения. InArray должен сортировать
ся с использованием этого же предиката.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:FindLowerBound`(const T`&`,const L`&`)const:%- [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 L]>_[@(0.0.255) int]_[* FindLowerBound]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_
[*@3 val], [@(0.0.255) const]_[*@4 L][@(0.0.255) `&]_[*@3 less])_[@(0.0.255) const]&]
[s5;:InArray`:`:FindLowerBound`(const T`&`)const:%- [@(0.0.255) int]_[* FindLowerBound]([@(0.0.255) c
onst]_[*@4 T][@(0.0.255) `&]_[*@3 val])_[@(0.0.255) const]&]
[s2; Находит нипжний предел для [%-*@3 val], 
используя [%-*@3 less] / StdLess<T> в качестве 
предиката сравнения. InArray должен сортировать
ся с использованием этого же предиката.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:InsertUpperBound`(const T`&`,const L`&`):%- [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 L]>_[@(0.0.255) int]_[* InsertUpperBound]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[*@3 val], [@(0.0.255) const]_[*@4 L][@(0.0.255) `&]_[*@3 lss])&]
[s5;:InArray`:`:InsertUpperBound`(const T`&`):%- [@(0.0.255) int]_[* InsertUpperBound]([@(0.0.255) c
onst]_[*@4 T][@(0.0.255) `&]_[*@3 val])&]
[s2; Вставляет элемент в позиции, найденной 
посредством FindUpperBound (но вся операция 
оптимизируется относительно пары 
FindUpperBound/Insert). InArray должен сортироваться 
с использованием этого же предиката.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Find`(const T`&`,const L`&`)const:%- [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 L]>_[@(0.0.255) int]_[* Find]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 val], 
[@(0.0.255) const]_[*@4 L][@(0.0.255) `&]_[*@3 less])_[@(0.0.255) const]&]
[s5;:InArray`:`:Find`(const T`&`)const:%- [@(0.0.255) int]_[* Find]([@(0.0.255) const]_[*@4 T
][@(0.0.255) `&]_[*@3 val])_[@(0.0.255) const]&]
[s2; Находит позицию [%-*@3 val] в InVector, отсортированно
м с помощью [%-*@3 less] / StdLess<T>. Если не найдена, 
возвращает отрицательное значение.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Begin`(`)const:%- [_^InArray`:`:ConstIterator^ ConstIterator]_[* Begin]()
_[@(0.0.255) const]&]
[s5;:InArray`:`:End`(`)const:%- [_^InArray`:`:ConstIterator^ ConstIterator]_[* End]()_[@(0.0.255) c
onst]&]
[s5;:InArray`:`:GetIter`(int`)const:%- [_^InArray`:`:ConstIterator^ ConstIterator]_[* Get
Iter]([@(0.0.255) int]_[*@3 pos])_[@(0.0.255) const]&]
[s5;:InArray`:`:Begin`(`):%- [_^InArray`:`:Iterator^ Iterator]_[* Begin]()&]
[s5;:InArray`:`:End`(`):%- [_^InArray`:`:Iterator^ Iterator]_[* End]()&]
[s5;:InArray`:`:GetIter`(int`):%- [_^InArray`:`:Iterator^ Iterator]_[* GetIter]([@(0.0.255) i
nt]_[*@3 pos])&]
[s2; Возвращает константный/неконстантный 
итератор в позицию begin/end/[%-*@3 pos].&]
[s3; &]
[s4; &]
[s5;:InArray`:`:IsPicked`(`)const:%- [@(0.0.255) bool]_[* IsPicked]()_[@(0.0.255) const]&]
[s2; Возвращает константный/неконстантный 
итератор в позицию begin/end/[%-*@3 pos].&]
[s3; &]
[s4; &]
[s5;:InArray`:`:InArray`(`):%- [* InArray]()&]
[s2; Дефолтный конструктор.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:InArray`:`:InArray`(Upp`:`:InArray`&`&`):%- [* InArray]([* InArray][@(0.0.255) `&
`&]_[*@3 v])&]
[s2; Пик`-конструктор.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:InArray`:`:operator`=`(Upp`:`:InArray`&`&`):%- [_^Upp`:`:InArray^ InArray][@(0.0.255) `&
]_[* operator`=]([_^Upp`:`:InArray^ InArray][@(0.0.255) `&`&]_[*@3 v])&]
[s2; Пик`-присваивание.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:InArray`(const InArray`&`,int`):%- [* InArray]([@(0.0.255) const]_[* InArra
y][@(0.0.255) `&]_[*@3 v], [@(0.0.255) int])&]
[s2; Конструктор глубокой копии.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:InArray`:`:InArray`(std`:`:initializer`_list`<T`>`):%- [* InArray]([_^http`:`/`/en`.cppreference`.com`/w`/cpp`/utility`/initializer`_list^ s
td`::initializer`_list]<[*@4 T]>_[*@3 init])&]
[s2; Инициализация C`+`+ 11.&]
[s3; &]
[s4; &]
[s5;:InArray`:`:Swap`(InArray`&`):%- [@(0.0.255) void]_[* Swap]([_^InArray^ InArray][@(0.0.255) `&
]_[*@3 b])&]
[s2; Меняет местами InArray с [%-*@3 b].&]
[s0; ]]