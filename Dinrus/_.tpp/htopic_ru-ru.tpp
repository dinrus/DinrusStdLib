topic "Класс SortedIndex";
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
[ {{10000@(113.42.0) [s0; [*@7;4 Класс SortedIndex]]}}&]
[s0;3%- &]
[s1;:noref:%- [@(0.0.255)3 template][3 _<][@(0.0.255)3 class][3 _][*@4;3 T][3 , 
][@(0.0.255)3 class][3 _][*@4;3 Less][3 _`=_StdLess<][*@4;3 T][3 >_>]&]
[s1;:SortedIndex`:`:class:%- [@(0.0.255) class]_[* SortedIndex]_:_[@(0.0.255) private]_[*@3 Mo
veableAndDeepCopyOption]<_[* SortedIndex]<[*@4 T], [*@4 Less]>_>_&]
[s2; SortedIndex `- это (вектроподобный) контейнер 
случайного доступа, который всегда 
содержит свои элементы в отсортированном 
виде, используя предикат [%-*@4 Less]. SortedIndex 
`- это относительно простой адаптер 
для [^topic`:`/`/Core`/src`/InVector`_ru`-ru^ InVector], который 
из сображений о производительности 
используется как ссылка.&]
[s2; [%-*@4 T] должен быть типа [*/^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ m
oveable].Как любой иной контейнер NTL, AIndex 
типа [*/^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ moveable 
] с семантикой трансфера [*/^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ п
ик и опционная глубокая копия].&]
[s0;3%- &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Методов]]}}&]
[s3;%- &]
[s5;:SortedIndex`:`:Add`(const T`&`):%- [@(0.0.255) int]_[* Add]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[*@3 x])&]
[s2; Вставляет новый элемент [%-*@3 x] в самую 
верхнюю позицию и возвращает его 
индекс.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:FindAdd`(const T`&`):%- [@(0.0.255) int]_[* FindAdd]([@(0.0.255) const]_
[*@4 T][@(0.0.255) `&]_[*@3 key])&]
[s2; Если [%-*@3 key] уже есть в SortedIndex, возвращает 
его индекс. Если нет, вставляет его 
и возвращает его индекс.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:operator`<`<`(const T`&`):%- [_^SortedIndex^ SortedIndex][@(0.0.255) `&
]_[* operator<<]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2; То же, что и Add([%-*@3 x]).&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:FindLowerBound`(const T`&`)const:%- [@(0.0.255) int]_[* FindLowerBoun
d]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])_[@(0.0.255) const]&]
[s2; Находит самый нижний (lower`-bound) индекс 
[%-*@3 x].&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:FindUpperBound`(const T`&`)const:%- [@(0.0.255) int]_[* FindUpperBoun
d]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])_[@(0.0.255) const]&]
[s2; Находит самый верхний (upper`-bound) индекс 
[%-*@3 x].&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Find`(const T`&`)const:%- [@(0.0.255) int]_[* Find]([@(0.0.255) const]_
[*@4 T][@(0.0.255) `&]_[*@3 x])_[@(0.0.255) const]&]
[s2; Возвращает минимальный индекс элемента, 
равного [%-*@3 x], либо отрицательное число, 
если он не найден.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:FindNext`(int`)const:%- [@(0.0.255) int]_[* FindNext]([@(0.0.255) int]_
[*@3 i])_[@(0.0.255) const]&]
[s2; Если элемент по [%-*@3 i] `+ 1 равен элементу 
в [%-*@3 i] , возвращает [%-*@3 i] `+ 1, иначе возвращает 
отрицательное число.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:FindLast`(const T`&`)const:%- [@(0.0.255) int]_[* FindLast]([@(0.0.255) c
onst]_[*@4 T][@(0.0.255) `&]_[*@3 x])_[@(0.0.255) const]&]
[s2; Находит максимальный индекс, равный 
[%-*@3 x], или отрицательное число, если 
такого нет.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:FindPrev`(int`)const:%- [@(0.0.255) int]_[* FindPrev]([@(0.0.255) int]_
[*@3 i])_[@(0.0.255) const]&]
[s2; Если элемент по [%-*@3 i] `- 1 равен элементу 
по [%-*@3 i] , возвращает [%-*@3 i] `- 1, иначе 
`- отрицательное число.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Remove`(int`):%- [@(0.0.255) void]_[* Remove]([@(0.0.255) int]_[*@3 i])&]
[s2; Удаляет элемент по индексу [%-*@3 i].&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Remove`(int`,int`):%- [@(0.0.255) void]_[* Remove]([@(0.0.255) int]_[*@3 i
], [@(0.0.255) int]_[*@3 count])&]
[s2; Удаляет [%-*@3 count] элементов, начиная 
с индекса [%-*@3 i].&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:RemoveKey`(const T`&`):%- [@(0.0.255) int]_[* RemoveKey]([@(0.0.255) co
nst]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2; Удаляет все элементы, равные [%-*@3 x].&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:operator`[`]`(int`)const:%- [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[* o
perator`[`]]([@(0.0.255) int]_[*@3 i])_[@(0.0.255) const]&]
[s2; Возвращает элемент по индексу [%-*@3 i].&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:GetCount`(`)const:%- [@(0.0.255) int]_[* GetCount]()_[@(0.0.255) const]&]
[s2; Возвращает число элементов.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:IsEmpty`(`)const:%- [@(0.0.255) bool]_[* IsEmpty]()_[@(0.0.255) const]&]
[s2; То же, что и GetCount() `=`= 0.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Clear`(`):%- [@(0.0.255) void]_[* Clear]()&]
[s2; Удаляет все элементы.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Trim`(int`):%- [@(0.0.255) void]_[* Trim]([@(0.0.255) int]_[*@3 n])&]
[s2; Удаляет все элементы, начиная с индекса 
[%-*@3 n].&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Drop`(int`):%- [@(0.0.255) void]_[* Drop]([@(0.0.255) int]_[*@3 n]_`=_[@3 1
])&]
[s2; Удаляет последние [%-*@3 n] элементов.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Top`(`)const:%- [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[* Top]()_[@(0.0.255) c
onst]&]
[s2; Возвращает последний элемент.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Shrink`(`):%- [@(0.0.255) void]_[* Shrink]()&]
[s2; Роняет резерв аллокации (уменьшает 
отпечаток памяти).&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Begin`(`)const:%- [_^SortedIndex`:`:ConstIterator^ ConstIterator]_[* B
egin]()_[@(0.0.255) const]&]
[s2; Возвращает итератов в начало SortedIndex.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:End`(`)const:%- [_^SortedIndex`:`:ConstIterator^ ConstIterator]_[* En
d]()_[@(0.0.255) const]&]
[s2; Возвращает итератор в конец SortedIndex.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:GetIter`(int`)const:%- [_^SortedIndex`:`:ConstIterator^ ConstIterat
or]_[* GetIter]([@(0.0.255) int]_[*@3 pos])_[@(0.0.255) const]&]
[s2; Возвращает итератор в индекс [%-*@3 pos] 
.&]
[s3; &]
[s4;%- &]
[s5;:SortedIndex`:`:GetKeys`(`)const:%- [@(0.0.255) const]_[_^InVector^ InVector]<[*@4 T]>`&
_[* GetKeys]()_[@(0.0.255) const]&]
[s2; Возвращает ссылку на InVector ключей.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:SortedIndex`(const SortedIndex`&`,int`):%- [* SortedIndex]([@(0.0.255) c
onst]_[* SortedIndex][@(0.0.255) `&]_[*@3 s], [@(0.0.255) int])&]
[s2; Конструктор глубокой копии.&]
[s3; &]
[s4; &]
[s5;:SortedIndex`:`:Swap`(SortedIndex`&`):%- [@(0.0.255) void]_[* Swap]([_^SortedIndex^ Sor
tedIndex][@(0.0.255) `&]_[*@3 a])&]
[s2; Меняет местами контенты двух контейнеров 
SortedIndex.&]
[s0; ]]