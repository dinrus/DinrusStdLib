topic "Класс InVector";
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
[ {{10000@(113.42.0) [s0; [*@7;4 Класс InVector]]}}&]
[s1;%- [@(0.0.255)3 template][3 _<][@(0.0.255)3 class][3 _][*@4;3 T][3 >]&]
[s1;:InVector`:`:class:%- [@(0.0.255) class]_[* InVector]_:_[@(0.0.255) public]_[*@3 MoveableA
ndDeepCopyOption]<_[* InVector]<[*@4 T]>_>_&]
[s2; InVector `- это контейнер случайного доступа 
(с оператором`[`]), у которого очень 
быстрое время вставки большого числа 
элементов. Комплексность вставок 
O(n) относительно сложно оценить, но 
она должна составлять log(n) для любого 
реалистичного числа элементов. Сложность 
повторного получения индекса равна 
log(n), но, благадаря поточным кэшам, 
класс быстр при сканировании простых 
диапазонов с использованием увеличения/умен
ьшения единичного индекса в единчном 
контейнере и быстр для любых сканирований, 
основанных на итераторах.&]
[s2; Как правило, любой метод, изменяющий 
число элементов в InVector (плюс метод 
Shrink) повреждает все итераторы в InVector 
и ссылки на элементы в нём.&]
[s2; InVector имеет дефолтную семантику пик`-трансфе
ра с опциональной глубокой копией. 
Он [^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ Moveable].&]
[s2; InVector требует, чтобы элементы были 
[^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ Moveable].&]
[s0;i448;a25;kKO9;@(0.0.255)%- &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Методов]]}}&]
[s3;%- &]
[s5;:InVector`:`:Insert`(int`):%- [*@4 T][@(0.0.255) `&]_[* Insert]([@(0.0.255) int]_[*@3 i])&]
[s2; Вставляет дефолтно сконструированный 
элемент по [%-*@3 i]. Повреждает итераторы 
и ссылки на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Insert`(int`,const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Insert]([@(0.0.255) in
t]_[*@3 i], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2; Вставляет копию [%-*@3 x] по [%-*@3 i]. Повреждает 
итераторы и ссылки на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:InsertN`(int`,int`):%- [@(0.0.255) void]_[* InsertN]([@(0.0.255) int]_[*@3 i
], [@(0.0.255) int]_[*@3 count])&]
[s2; Вставляет [%-*@3 count] дефолтно сконструированны
х элементов по [%-*@3 i]. Повреждает итераторы 
и ссылки на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Remove`(int`,int`):%- [@(0.0.255) void]_[* Remove]([@(0.0.255) int]_[*@3 i],
 [@(0.0.255) int]_[*@3 count]_`=_[@3 1])&]
[s2; Удаляет [%-*@3 count] элементов по [%-*@3 i]. 
Повреждает итераторы и ссылки на 
элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:operator`[`]`(int`)const:%- [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[* ope
rator`[`]]([@(0.0.255) int]_[*@3 i])_[@(0.0.255) const]&]
[s5;:InVector`:`:operator`[`]`(int`):%- [*@4 T][@(0.0.255) `&]_[* operator`[`]]([@(0.0.255) i
nt]_[*@3 i])&]
[s2; Возвращает ссылку на элемент по [%-*@3 i].&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Add`(`):%- [*@4 T][@(0.0.255) `&]_[* Add]()&]
[s2; То же, что и Insert(GetCount()). Повреждает 
итераторы и ссылки на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Add`(const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Add]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[*@3 x])&]
[s2; То же, что и Insert(GetCount(), [%-*@3 x]). Повреждает 
итераторы и ссылки на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:AddN`(int`):%- [@(0.0.255) void]_[* AddN]([@(0.0.255) int]_[*@3 n])&]
[s2; То же, что и InsertN(GetCount(), [%-*@3 n]). Повреждает 
итераторы и ссылки на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:GetCount`(`)const:%- [@(0.0.255) int]_[* GetCount]()_[@(0.0.255) const]&]
[s2; Возвращает число элементов в InVector.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:IsEmpty`(`)const:%- [@(0.0.255) bool]_[* IsEmpty]()_[@(0.0.255) const]&]
[s2; То же, что и GetCount() `=`= 0.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Trim`(int`):%- [@(0.0.255) void]_[* Trim]([@(0.0.255) int]_[*@3 n])&]
[s2; То же, что и Remove(n, GetCount() `- n). Повреждает 
итераторы и ссылки на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:SetCount`(int`):%- [@(0.0.255) void]_[* SetCount]([@(0.0.255) int]_[*@3 n])&]
[s2; Устанавливает число элементов в 
[%-*@3 n], либо удаляя лишние элементы, 
либо используя AddN, чтобы расширить 
InVector. Повреждает итераторы и ссылки 
на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Clear`(`):%- [@(0.0.255) void]_[* Clear]()&]
[s2; То же, что и Remove(0, GetCount()). Повреждает 
итераторы и ссылки на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:At`(int`):%- [*@4 T][@(0.0.255) `&]_[* At]([@(0.0.255) int]_[*@3 i])&]
[s2; Если [%-*@3 i] >`= GetCount, выполняет SetCount(i 
`+ 1), чтобы элемент по [%-*@3 i] гарантированно 
существовал. В любом случае возвращает 
ссылку на элемент по [%-*@3 i]. Повреждает 
итераторы и ссылки на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Shrink`(`):%- [@(0.0.255) void]_[* Shrink]()&]
[s2; Минимизирует память кучи, размещённую 
InVector`'ом. Повреждает итераторы и ссылки 
на элементы.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Set`(int`,const T`&`,int`):%- [@(0.0.255) void]_[* Set]([@(0.0.255) int]_[*@3 i
], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x], [@(0.0.255) int]_[*@3 count])&]
[s2; Устанавливает значение [%-*@3 count] элементов, 
начиная с [%-*@3 i], в [%-*@3 x].&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Set`(int`,const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Set]([@(0.0.255) int]_[*@3 i
], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2; Устанавливает значение элемента 
по [%-*@3 i] в [%-*@3 x].&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Swap`(int`,int`):%- [@(0.0.255) void]_[* Swap]([@(0.0.255) int]_[*@3 i1], 
[@(0.0.255) int]_[*@3 i2])&]
[s2; Меняет местами элементы в позициях 
[%-*@3 i1] и [%-*@3 i2].&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Drop`(int`):%- [@(0.0.255) void]_[* Drop]([@(0.0.255) int]_[*@3 n]_`=_[@3 1])&]
[s2; Удаляет [%-*@3 n] элементов в конце InVector`'а.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Top`(`):%- [*@4 T][@(0.0.255) `&]_[* Top]()&]
[s5;:InVector`:`:Top`(`)const:%- [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[* Top]()_[@(0.0.255) c
onst]&]
[s2; Возвращает ссылку на последний элемент.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Pop`(`):%- [*@4 T]_[* Pop]()&]
[s2; Возвращает копию последнего элемента 
и удаляет его.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:FindUpperBound`(const T`&`,const L`&`)const:%- [@(0.0.255) template]_<
[@(0.0.255) class]_[*@4 L]>_[@(0.0.255) int]_[* FindUpperBound]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[*@3 val], [@(0.0.255) const]_[*@4 L][@(0.0.255) `&]_[*@3 less])_[@(0.0.255) const]&]
[s5;:InVector`:`:FindUpperBound`(const T`&`)const:%- [@(0.0.255) int]_[* FindUpperBound](
[@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 val])_[@(0.0.255) const]&]
[s2; Находит верхнюю границу для [%-*@3 val] 
посредством [%-*@3 less] / StdLess<T> в качестве 
предиката сравнения. InVector должен 
сортироваться с помощью такого же 
предиката.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:FindLowerBound`(const T`&`,const L`&`)const:%- [@(0.0.255) template]_<
[@(0.0.255) class]_[*@4 L]>_[@(0.0.255) int]_[* FindLowerBound]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[*@3 val], [@(0.0.255) const]_[*@4 L][@(0.0.255) `&]_[*@3 less])_[@(0.0.255) const]&]
[s5;:InVector`:`:FindLowerBound`(const T`&`)const:%- [@(0.0.255) int]_[* FindLowerBound](
[@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 val])_[@(0.0.255) const]&]
[s2; Находит нижнюю границу для [%-*@3 val] 
посредством [%-*@3 less] / StdLess<T> в качестве 
предиката сравнения. InVector должен 
сортироваться с помощью такого же 
предиката.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:InsertUpperBound`(const T`&`,const L`&`):%- [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 L]>_[@(0.0.255) int]_[* InsertUpperBound]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[*@3 val], [@(0.0.255) const]_[*@4 L][@(0.0.255) `&]_[*@3 less])&]
[s5;:InVector`:`:InsertUpperBound`(const T`&`):%- [@(0.0.255) int]_[* InsertUpperBound]([@(0.0.255) c
onst]_[*@4 T][@(0.0.255) `&]_[*@3 val])&]
[s2; Вставляет элемент в позиции, найденной 
посредством FindUpperBound (но вся операция 
оптимизируется относительно пары 
FindUpperBound/Insert). InVector должен сортироваться 
с помощью такого же предиката.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Find`(const T`&`,const L`&`)const:%- [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 L]>_[@(0.0.255) int]_[* Find]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 val], 
[@(0.0.255) const]_[*@4 L][@(0.0.255) `&]_[*@3 less])_[@(0.0.255) const]&]
[s5;:InVector`:`:Find`(const T`&`)const:%- [@(0.0.255) int]_[* Find]([@(0.0.255) const]_[*@4 T
][@(0.0.255) `&]_[*@3 val])_[@(0.0.255) const]&]
[s2; Нахрдит позицию [%-*@3 val] в InVector, отсортирванным
 посредством [%-*@3 less] / StdLess<T>. Если не 
найдена, возвращает отрицательное 
число.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Begin`(`)const:%- [_^InVector`:`:ConstIterator^ ConstIterator]_[* Begin](
)_[@(0.0.255) const]&]
[s5;:InVector`:`:End`(`)const:%- [_^InVector`:`:ConstIterator^ ConstIterator]_[* End]()_[@(0.0.255) c
onst]&]
[s5;:InVector`:`:GetIter`(int`)const:%- [_^InVector`:`:ConstIterator^ ConstIterator]_[* G
etIter]([@(0.0.255) int]_[*@3 pos])_[@(0.0.255) const]&]
[s5;:InVector`:`:Begin`(`):%- [_^InVector`:`:Iterator^ Iterator]_[* Begin]()&]
[s5;:InVector`:`:End`(`):%- [_^InVector`:`:Iterator^ Iterator]_[* End]()&]
[s5;:InVector`:`:GetIter`(int`):%- [_^InVector`:`:Iterator^ Iterator]_[* GetIter]([@(0.0.255) i
nt]_[*@3 pos])&]
[s2; Возвращает константный/неконстантный 
итератор в begin/end/[%-*@3 pos].&]
[s3; &]
[s4; &]
[s5;:InVector`:`:InVector`(`):%- [* InVector]()&]
[s2; Конструирует пустой InVector.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:InVector`(const InVector`&`,int`):%- [* InVector]([@(0.0.255) const]_[* In
Vector][@(0.0.255) `&]_[*@3 v], [@(0.0.255) int])&]
[s2; Опционный конструктор глубокой копии.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:InVector`:`:InVector`(std`:`:initializer`_list`<T`>`):%- [* InVector]([_^http`:`/`/en`.cppreference`.com`/w`/cpp`/utility`/initializer`_list^ s
td`::initializer`_list]<[*@4 T]>_[*@3 init])&]
[s2; Инициализация C`+`+ 11.&]
[s3; &]
[s4; &]
[s5;:InVector`:`:Swap`(InVector`&`):%- [@(0.0.255) void]_[* Swap]([_^InVector^ InVector][@(0.0.255) `&
]_[*@3 b])&]
[s2; Поменять местами с другим InVector.&]
[s3; &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0; [* Примеры с кэшированием InVector]]}}&]
[s3;%- &]
[s0; Следующие примеры демонстрируют, 
что подразумевает `"простое сканирование`", 
использующее кэш для ускорения повторного 
получения индекса, в сравнении с более 
сложными безкэшевыми сканированиями:&]
[s0; &]
[s0; [4 Кэшированные случаи]&]
[s0; &]
[ {{10000 [s0; [C int m `= 0;]&]
[s0; [C for(int i `= 0; i < x.GetCount(); i`+`+)]&]
[s0; [C -|m `+`= x`[i`];]&]
[s0; [C for(int i `= x.GetCount(); `-`-i >`= 0;)]&]
[s0; [C -|m `+`= x`[i`];]]}}&]
[s0; &]
[s0; &]
[ {{10000 [s0; [C InVector<int> y;]&]
[s0; [C y <<`= x;]&]
[s0; [C int m `= 0;]&]
[s0; [C for(int i `= 0; i < x.GetCount(); i`+`+)]&]
[s0; [C -|m `+`= x`[i`];]&]
[s0; [C for(int i `= y.GetCount(); `-`-i >`= 0;)]&]
[s0; [C -|m `+`= y`[i`];]]}}&]
[s0; &]
[s0; &]
[s0; &]
[s0; [4 Безкэшевые случаи (здесь почти в 
8 раз медленнее)]&]
[s0; &]
[ {{10000 [s0; [C int m `= 0;]&]
[s0; [C for(int i `= 0; i < x.GetCount(); i`+`+)]&]
[s0; [C -|m `+`= x`[i`] `+ x`[x.GetCount() `- i `- 1`];]]}}&]
[s0; &]
[ {{10000 [s0; [C InVector<int> y;]&]
[s0; [C y <<`= x;]&]
[s0; [C int m `= 0;]&]
[s0; [C for(int i `= 0; i < x.GetCount(); i`+`+)]&]
[s0; [C -|m `+`= x`[i`] `+ y`[i`];]]}}&]
[s0; ]]