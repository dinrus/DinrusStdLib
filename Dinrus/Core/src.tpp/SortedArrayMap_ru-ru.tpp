topic "Класс SortedArrayMap";
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
[ {{10000@(113.42.0) [s0; [*@7;4 Класс SortedArrayMap]]}}&]
[s0;i448;a25;kKO9;@(0.0.255)%- &]
[s1;:noref:%- [@(0.0.255)3 template][3 _<][@(0.0.255)3 class][3 _][*@4;3 K][3 , 
][@(0.0.255)3 class][3 _][*@4;3 T][3 , ][@(0.0.255)3 class][3 _][*@4;3 Less][3 _`=_StdLess<][*@4;3 K
][3 >_>]&]
[s1;:SortedArrayMap`:`:class:%- [@(0.0.255) class]_[* SortedArrayMap]_:_[@(0.0.255) public]_
[*@3 MoveableAndDeepCopyOption]<[* SortedArrayMap]<[*@4 K], [*@4 T], 
[*@4 Less]>_>, [@(0.0.255) public]_[*@3 SortedAMap]<[*@4 K], [*@4 T], [*@4 Less], 
[_^Slaved`_InArray`_`_^ Slaved`_InArray`_`_]<[*@4 T]>_>_&]
[s2; Векторная разновидность [^topic`:`/`/Core`/src`/SortedAMap`_ru`-ru^ S
ortedAMap]. У неё дефолтная `"пик`"`-семантика 
трансфера с опционной глубокой копией. 
Он [^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ Moveable]. Ключи 
должны быть [^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ Moveable], 
но общих ограничений относительно 
значений нет,`- они могут быть даже 
производными от [%-*@4 T].&]
[s0;i448;a25;kKO9;@(0.0.255)%- &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Методов]]}}&]
[s3;%- &]
[s5;:SortedArrayMap`:`:Add`(const K`&`,const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Add]([@(0.0.255) c
onst]_[*@4 K][@(0.0.255) `&]_[*@3 k], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s5;:Upp`:`:SortedArrayMap`:`:Add`(const K`&`,T`&`&`):%- [*@4 T][@(0.0.255) `&]_[* Add]([@(0.0.255) c
onst]_[*@4 K][@(0.0.255) `&]_[*@3 k], [*@4 T][@(0.0.255) `&`&]_[*@3 x])&]
[s2; Вставляет копию пары ключ`-значение 
в самой верхней позиции ключей и возвращает 
ссылку на значение элемента.&]
[s3; &]
[s4; &]
[s5;:SortedArrayMap`:`:Add`(const K`&`):%- [*@4 T][@(0.0.255) `&]_[* Add]([@(0.0.255) const]_
[*@4 K][@(0.0.255) `&]_[*@3 k])&]
[s2; Вставляет ключ в самой верхней позиции 
ключей и возвращает ссылку на дефолтно 
сконструированное значение по тому 
же индексу.&]
[s3; &]
[s4; &]
[s5;:SortedArrayMap`:`:Add`(const K`&`,T`*`):%- [*@4 T][@(0.0.255) `&]_[* Add]([@(0.0.255) co
nst]_[*@4 K][@(0.0.255) `&]_[*@3 k], [*@4 T]_`*[*@3 newt])&]
[s2; Вставляет ключ в самой верхней позиции 
ключей и устанавливает в [%-*@3 newt] `- SortedArrayMap 
принимает во владение. Возвращает 
[%- `*][%-*@3 newt].&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:SortedArrayMap`:`:Create`(const K`&`,Args`&`&`.`.`.args`):%- [@(0.0.255) te
mplate]_<[@(0.0.255) class]_[*@4 TT], [@(0.0.255) class...]_[*@4 Args]>_[*@4 TT][@(0.0.255) `&
]_[* Create]([@(0.0.255) const]_[*@4 K][@(0.0.255) `&]_[*@3 k], [*@4 Args][@(0.0.255) `&`&...]_
args)&]
[s2; Вставляет ключ в самой верхней позиции 
ключей и создаёт новое значение типа 
[%-*@4 TT], используя args в качестве параметров 
конструктора. [%-*@4 TT] должен быть производным 
от [%-*@4 T]. Возвращает ссылку на вновь 
созданное значение.&]
[s3; &]
[s4; &]
[s5;:SortedArrayMap`:`:FindAdd`(const K`&`):%- [@(0.0.255) int]_[* FindAdd]([@(0.0.255) con
st]_[*@4 K][@(0.0.255) `&]_[*@3 k])&]
[s2; Если ключ [%-*@3 k] присутствует, возвращает 
индекс его самой низкой позиции. Если 
нет, вставляет его в самой верхней 
позиции и возвращает его индекс (значение 
элемента конструируется дефолтно).&]
[s3; &]
[s4; &]
[s5;:SortedArrayMap`:`:FindAdd`(const K`&`,const T`&`):%- [@(0.0.255) int]_[* FindAdd]([@(0.0.255) c
onst]_[*@4 K][@(0.0.255) `&]_[*@3 k], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 init])&]
[s2; Если ключ [%-*@3 k] присутствует, возвращает 
индекс его самой нижней (lower`-bound) позиции. 
Если нет, вставляет его в самой верхней 
(upper`-bound) позиции со значением элемента, 
копи`-сконструированного из [%-*@3 init], 
и возвращает его индекс.&]
[s3; &]
[s4; &]
[s5;:SortedArrayMap`:`:GetAdd`(const K`&`):%- [*@4 T][@(0.0.255) `&]_[* GetAdd]([@(0.0.255) c
onst]_[*@4 K][@(0.0.255) `&]_[*@3 k])&]
[s2; Если ключ [%-*@3 k] присутствует, возвращает 
ссылку на значение в его самой нижней 
позиции. Если нет, вставляет его в 
самой верхней позиции и возвращает 
ссылку на соответствующее значение 
(значение элемента конструируется 
дефолтно).&]
[s3; &]
[s4; &]
[s5;:SortedArrayMap`:`:GetAdd`(const K`&`,const T`&`):%- [*@4 T][@(0.0.255) `&]_[* GetAdd](
[@(0.0.255) const]_[*@4 K][@(0.0.255) `&]_[*@3 k], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x
])&]
[s2; Если ключ [%-*@3 k] присутствует, возвращает 
ссылку на значение в его самой нижней 
позиции. Если нет, вставляет его в 
самой верхней позиции и возвращает 
ссылку на соответствующее значение, 
которое копи`-конструируется из [%-*@3 x].&]
[s3; &]
[s4; &]
[s5;:SortedArrayMap`:`:Detach`(int`):%- [*@4 T]_`*[* Detach]([@(0.0.255) int]_[*@3 i])&]
[s2; Удаляет элемент по индексу [%-*@3 i] и 
возвращает указатель на значение. 
Этот указатель должен удаляться кодом`-клиен
том.&]
[s3; &]
[s4;%- &]
[s5;:SortedArrayMap`:`:operator`(`)`(const K`&`,const T`&`):%- [_^SortedArrayMap^ Sorte
dArrayMap][@(0.0.255) `&]_[* operator()]([@(0.0.255) const]_[*@4 K][@(0.0.255) `&]_[*@3 k], 
[@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 v])&]
[s2; То же, что и Add([%-*@3 k], [%-*@3 v]), возвращает 
`*this. Синтактический `"сахар`" для создания 
мапа.&]
[s3; &]
[s4; &]
[s5;:SortedArrayMap`:`:SortedArrayMap`(const SortedArrayMap`&`,int`):%- [* SortedArrayM
ap]([@(0.0.255) const]_[* SortedArrayMap][@(0.0.255) `&]_[*@3 s], [@(0.0.255) int])&]
[s2; Конструктор глубокой копии.&]
[s4;%- &]
[s5;:Upp`:`:SortedArrayMap`:`:SortedArrayMap`(std`:`:initializer`_list`<std`:`:pair`<K`,T`>`>`):%- [* S
ortedArrayMap]([_^http`:`/`/en`.cppreference`.com`/w`/cpp`/utility`/initializer`_list^ s
td`::initializer`_list]<[_^http`:`/`/en`.cppreference`.com`/w`/cpp`/utility`/pair^ s
td`::pair]<[*@4 K], [*@4 T]>>_[*@3 init])&]
[s2; Инициализация C`+`+ 11.&]
[s3; &]
[s0; ]]