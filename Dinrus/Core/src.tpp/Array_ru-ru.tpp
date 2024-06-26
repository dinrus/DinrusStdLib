topic "Класс Array (Массив)";
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
[ {{10000@(113.42.0) [s0; [*@7;4 Класс Array]]}}&]
[s3;%- &]
[s1;:noref:%- [@(0.0.255)3 template][3 _<][@(0.0.255)3 class][3 _][*@4;3 T][@(0.0.255)3 >]&]
[s1;:Array`:`:class:%- [@(0.0.255) class]_[* Array]_:_[@(0.0.255) public]_[*@3 MoveableAndDeep
CopyOption][@(0.0.255) <]_[* Array][@(0.0.255) <][*@4 T][@(0.0.255) >]_>_&]
[s8; [*@4 T]-|Тип или класс`-основа элементов, 
сохраняемых в Array. Общие требования 
к T отсутствуют.&]
[s9; Наиболее универсальная форма базового 
контейнера случайного доступа. Его 
черты возникают из того факта, что 
, как правило, он реализуется с применением 
непрямого (скрытого) контейнера с 
указателями на T, такого как, например, 
Vector<T`*>. Тем не менее, он поддерживает 
общие концепты, такие как владение 
содержимыми элементами, доступ по 
ссылке (а не по указателю) к элементам, 
и проч.&]
[s9; Он предоставляет практически все 
операции над Vector`'ом, с той же семантикой, 
и почти любой Vector можно прямо заменить 
на Array. С другой стороны, им предоставляются 
некоторые специальные операции, невозможные
 для Vector`'а, и, что самое важное, он 
никогда не повреждает ссылки (то есть 
ссылки и указатели C`+`+) на элементы 
(однако, к сожалению, им часто повреждаются 
обходчики).&]
[s9; Array также может использоваться для 
сохранения полиморфных элементов: 
сохранённые элементы могут производиться 
от T. Чтобы сохранять такие элементы, 
нужно передать указатель на элемент, 
предварительно созданный на куче. 
Как прежде, Array принимает `"во владение`" 
такие элементы (т.е. имеет возможность, 
например, удалять их в любое время). 
Можно также использовать этот метод 
для создания Array`'я элементов, у которого 
нет ни pick, ни КГК (конструктора глубокой 
копии), ни дефолтного конструктора.&]
[s9; Есть также операции, повзоляющие 
откреплять элемент от Array`'я, удалять 
его, но не руша  Эта операция возвращает 
указатель на такой элемент, и Array `"сдаёт`" 
`"права собственности`".&]
[s9; Недостаток Array`'я в сравнении с Vector`'ом 
в производительности (performance):  большинство 
операций значительно медленнее, чем 
с Vector`'ами (по фактору до 8(`-ми раз), 
что зависит от скорости системных 
методов  malloc/free). По`-прежднему, некоторые 
операции могут быть быстрее, например, 
операции вставки (Insert) и удаления 
(Remove) обычно перемещают намного меньше 
памяти.&]
[s9; Что касается памяти, для элементов 
с небольшим размером, потребление 
памяти Array`'ем значительно выше, чем 
у Vector`'а. По мере роста элементов в 
размере, Array начинает становиться 
эффективнее Vector`'а.&]
[s9; Итераторы к Array удовлетворяют всем 
требованиям стандартной библиотеки 
C`+`+ для обходчиков (итераторов) случайного 
доступа (random access), плюс то, что они 
позволяют делать присваивание (assignment) 
(и конструкцию копии) , и ещё `- тестировать 
на указатель 0 (то есть NULL).&]
[s9; Как любой иной NTL`-контейнер, Array является 
[*/^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ moveable][*/  (`"движимым`") 
]типом с `"семантикой трансфера`" ( 
transfer semantics), именуемой `" [*/^topic`:`/`/Core`/srcdoc`/pick`_`_ru`-ru^ p
ick и опциональная глубокая копия (ОГК)] 
`".&]
[s3; &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0; [* Детали Конструктора]]}}&]
[s3;%- &]
[s5;:Array`:`:Array`(`):%- [* Array]()&]
[s2; Дефолтный конструктор. Конструирует 
пустой Array.&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:Array`(int`):%- [@(0.0.255) explicit]_[* Array]([@(0.0.255) int]_[*@3 n])
&]
[s2; Создаёт Array из [%-*@3 n] дефолтно конструируемых 
элементов.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:Array`(int`,const T`&`):%- [@(0.0.255) explicit]_[* Array]([@(0.0.255) i
nt]_[*@3 n], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 init])&]
[s2; Создаёт Array из [%-*@3 n] элементов, `"копи`-сконстр
уированных`" как [%-*@3 init].&]
[s3; &]
[s4;%- &]
[s5;:Array`:`:Array`(pick`_ Array`&`):%- [* Array]([@(0.128.128) pick`_]_[* Array][@(0.0.255) `&
]_[*@3 v])&]
[s2; Pick`-конструктор. `"Трансферит`" исходный 
Array за `"низкое константное`" время, 
но рушит его при пикинге. &]
[s7; [*@3 v]-|Исходный Array.&]
[s3; &]
[s4; &]
[s5;:Array`:`:Array`(const Array`&`,int`):%- [* Array]([@(0.0.255) const]_[* Array][@(0.0.255) `&
]_[*@3 v], [@(0.0.255) int])&]
[s2; Опциональный конструктор глубокой 
копии (ОКГК).&]
[s6; Требуется, чтобы у T был конструктор 
глубокой копии (КГК) или опциональный 
КГК (ОКГК), если в массиве находятся 
только объекты типа T.&]
[s6; Требуется полиморфная глубокая копия 
(ПГК), если в Array также находятся объекты, 
производного то Т, типа.&]
[s7; [*@3 v]-|Исходный Array&]
[s3; &]
[s4; &]
[s5;:Array`:`:`~Array`(`):%- [@(0.0.255) `~][* Array]()&]
[s2; Деструктор. Вызывает деструктор 
(`"разрушитель`") для каждого (отдельного) 
элемента в Array.&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:Array`(std`:`:initializer`_list`<T`>`):%- [* Array]([_^http`:`/`/en`.cppreference`.com`/w`/cpp`/utility`/initializer`_list^ s
td`::initializer`_list]<[*@4 T]>_[*@3 init])&]
[s2; C`+`+11 инициализация.&]
[s0; &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Членов]]}}&]
[s4;H0;%- &]
[s5;:Array`:`:Add`(`):%- [*@4 T][@(0.0.255) `&]_[* Add]()&]
[s2; Добавляет дефолтно сконструированный 
элемент в Array.&]
[s6; Требуется, чтобы у T был дефолтный 
конструктор.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*/ Возвратное значение]-|Ссылка на `"вновь 
добавленный дефолтно сконструированный`" 
элемент.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Add`(const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Add]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[*@3 x])&]
[s2; Добавляет новый элемент с указанным 
значением в Array.&]
[s6; Требуется, чтобы у T был дефолтный 
конструктор.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 x]-|Значение, копируемое во вновь 
созданный элемент.&]
[s7; [*/ Возвратное значение]-|Ссылка на новый 
элемент в Array`'е.&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:Add`(T`&`&`):%- [*@4 T][@(0.0.255) `&]_[* Add]([*@4 T][@(0.0.255) `&`&]_[*@3 x
])&]
[s2; Добавляет новый элемент в этот  Array 
и `"пикует`" в него значение параметра 
x.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Add`(T`*`):%- [*@4 T][@(0.0.255) `&]_[* Add]([*@4 T]_`*[*@3 newt])&]
[s2; Добавляет новый элемент в этот Array. 
Элемент задаётся указателем на объект, 
созданный с применением оператора 
new. Array принимает права владения этим 
объектом. Этот вариант позволяет 
использовать Array как полиморфный 
контейнер, так как тип добавляемого 
элемента может быть либо T, либо производный 
от T. Никакого конмтруктора не применяется. 
&]
[s7; [*C@3 newt]-|Добавляемый объект.&]
[s7; [*/ Возвратное значение]-|Ссылка на новый 
элемент  (то есть [* `*newt]).&]
[s0;*%- &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:Add`(Upp`:`:One`<T`>`&`&`):%- [*@4 T][@(0.0.255) `&]_[* Add]([_^Upp`:`:One^ O
ne]<[*@4 T]>`&`&_[*@3 one])&]
[s2; Создаёт новый элемент в Array`'е, перемещая 
контент [%-*@3 one] в него. [%-*@3 one] должен 
содержать значение, иначе поведение 
неопределённое.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:Create`(Args`&`&`.`.`.args`):%- [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 TT], [@(0.0.255) class...]_[*@4 Args]>_[*@4 TT][@(0.0.255) `&]_[* Create]([*@4 Args][@(0.0.255) `&
`&...]_[*@3 args])&]
[s2; Создаёт новый элемент в Array`'е. [%-*@3 args] 
передаются конструктору.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:operator`[`]`(int`)const:%- [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[* operat
or`[`]]([@(0.0.255) int]_[*@3 i])_[@(0.0.255) const]&]
[s2; Возвращает ссылку на элемент по указанной 
позиции.&]
[s7; [*C@3 i]-|Позиция элемента.&]
[s7; [*/ Возвратное значение]-|Константная 
ссылка на элемент.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:operator`[`]`(int`):%- [*@4 T][@(0.0.255) `&]_[* operator`[`]]([@(0.0.255) int]_
[*@3 i])&]
[s2; Возвращает ссылку на элемент по указанной 
позиции.&]
[s7; [*C@3 i]-|Позиция элемента.&]
[s7; [*/ Возвратное значение]-|Ссылка на элемент.&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:Get`(int`,const T`&`)const:%- [@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[* Get]([@(0.0.255) int]_[*@3 i], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 def])_[@(0.0.255) c
onst]&]
[s5;:Upp`:`:Array`:`:Get`(int`,T`&`):%- [*@4 T][@(0.0.255) `&]_[* Get]([@(0.0.255) int]_[*@3 i],
 [*@4 T][@(0.0.255) `&]_[*@3 def])&]
[s2; Если индекс [%-*@3 i] полноценен (`"валиден`") 
(он >`= 0 и < GetCount()), то возвращается ссылка 
на элемент по [%-*@3 i], иначе возвращается 
[%-*@3 def].&]
[s3; &]
[s4;%- &]
[s5;:Array`:`:GetCount`(`)const:%- [@(0.0.255) int]_[* GetCount]()_[@(0.0.255) const]&]
[s2; Возвращает число элементов в этом 
Array.&]
[s7; [*/ Возвратное значение]-|Действительное 
число элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:IsEmpty`(`)const:%- [@(0.0.255) bool]_[* IsEmpty]()_[@(0.0.255) const]&]
[s2; Тестирует, пуст ли Array. То же, что и 
GetCount() `=`= 0.&]
[s7; [*/ Возвратное значение]-|верно, если 
Array пустой, ложно в ином случае.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Trim`(int`):%- [@(0.0.255) void]_[* Trim]([@(0.0.255) int]_[*@3 n])&]
[s2; Снижает число элементов в этом Array`'е 
до указанного. Требуемое число должно 
быть меньше или равно действительному 
числу элементов в этом Array`'е.&]
[s7; [*C@3 n]-|Требуемое число элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:SetCount`(int`):%- [@(0.0.255) void]_[* SetCount]([@(0.0.255) int]_[*@3 n])&]
[s2; Изменяет счёт элементов в этом Array`'е 
до обозначенного значения. Если требуемое 
число элементов больше, чем действительное 
число, то вновь добавленные элементы 
конструируются по дефолтному методу. 
Если ёмкость этого Array`'я должна быть 
увеличена, то новая ёмкость будет 
точно соответствовать требуемому 
числу элементов (в отличие от [* SetCountR]).&]
[s6; Требуется, чтобы у T был дефолтный 
конструктор.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 n]-|Требуемое число элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:SetCount`(int`,const T`&`):%- [@(0.0.255) void]_[* SetCount]([@(0.0.255) int]_
[*@3 n], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 init])&]
[s2; Изменяет счёт элементов в этом Array`'е 
до обозначенного значения. Если требуемое 
число элементов больше, чем действительное 
число, то вновь добавленные элементы 
инициализируются на заданное значение 
с помощью копи`-конструктора (КГК). 
Если ёмкость этого Array`'я должна быть 
увеличена, то новая ёмкость будет 
точно соответствовать требуемому 
числу элементов (в отличие от [* SetCountR]).&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 n]-|Требуемое число элементов.&]
[s7; [*C@3 init]-|Значение инициализации вновь 
добавленных элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:SetCountR`(int`):%- [@(0.0.255) void]_[* SetCountR]([@(0.0.255) int]_[*@3 n])&]
[s2; Изменяет счёт элементов в этом Array`'е 
до обозначенного значения. Если требуемое 
число элементов больше чем действительное 
число, вновь добавленные элементы 
конструируются по дефолтному методу. 
Если ёмкость этого Array`'я должна быть 
увеличена, новая ёмкость будет больше, 
чем требуемое число элементов (в отличие 
от [* SetCount]) , что позволяет добавлять 
другие элементы, не увеличивая её.&]
[s6; Требуется, чтобы у T был дефолтный 
конструктор.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 n]-|Требуемое число элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:SetCountR`(int`,const T`&`):%- [@(0.0.255) void]_[* SetCountR]([@(0.0.255) in
t]_[*@3 n], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 init])&]
[s2; Изменяет счёт элементов в этом Array`'е 
до обозначенного значения. Если требуемое 
число элементов больше чем действительное 
число, вновь добавленные элементы 
инициализируются на заданное значение 
с помощью копи`-конструктора (КГК). 
Если ёмкость этого Array`'я должна быть 
увеличена, то новая ёмкость будет 
больше, чем требуемое число элементов 
(в отличие от [* SetCount]) , что позволяет 
добавлять другие элементы, не увеличивая 
её.&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 n]-|Требуемое число элементов.&]
[s7; [*C@3 init]-|Значение инициализации вновь 
добавленных элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Clear`(`):%- [@(0.0.255) void]_[* Clear]()&]
[s2; Удаляет все элементы из этого Array`'я. 
Ёмкость также очищается до нуля (обнуляется)
.&]
[s6; Повреждает итераторы к Array.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:At`(int`):%- [*@4 T][@(0.0.255) `&]_[* At]([@(0.0.255) int]_[*@3 i])&]
[s2; Если заданная позиция ниже, чем число 
элементов в этом Array`'е ([* i < GetCount()]), 
то возвращается ссылка на элемент 
по указанной позиции.Иначе число 
элементов в массиве увеличивается 
на [* i `+ 1]. Вновь добавленные элементы 
конструируются дефолтно. Если ёмкость 
этого Array`'я должна быть увеличена, 
то новая ёмкость будет больше, чем 
требуемое число элементов, что позволяет 
добавлять другие элементы, не увеличивая 
её.&]
[s6; Требуется, чтобы у T был дефолтный 
конструктор.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция требуемого элемента.&]
[s7; [*/ Возвратное значение]-|Ссылка на требуемый 
элемент.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:At`(int`,const T`&`):%- [*@4 T][@(0.0.255) `&]_[* At]([@(0.0.255) int]_[*@3 i], 
[@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2; Если заданная позиция ниже, чем число 
элементов в этом Array`'е ([* i < GetCount()]), 
то возвращается ссылка на элемент 
по указанной позиции.Иначе число 
элементов в массиве увеличивается 
на [* i `+ 1]. Вновь добавленные элементы 
конструируются копированием из [* x]. 
Если ёмкость этого Array`'я должна быть 
увеличена, то новая ёмкость будет 
больше, чем требуемое число элементов, 
что позволяет добавлять другие элементы, 
не увеличивая её.&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция требуемого элемента.&]
[s7; [*C@3 x]-|Инициализационное значение вновь 
добавляемых элементов.&]
[s7; [*/ Возвратное значение]-|Ссылка на требуемый 
элемент.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Shrink`(`):%- [@(0.0.255) void]_[* Shrink]()&]
[s2; Минимизирует потребление памяти 
этим Array`'ем путём уменьшения ёмкости 
до числа элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Reserve`(int`):%- [@(0.0.255) void]_[* Reserve]([@(0.0.255) int]_[*@3 xtra])&]
[s2; Резервирует ёмкость. Если требуемая 
ёмкость больше, чем текущая, она увеличивает
ся на необходимое значение.&]
[s7; [*C@3 xtra]-|Необходимая ёмкость.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:GetAlloc`(`)const:%- [@(0.0.255) int]_[* GetAlloc]()_[@(0.0.255) const]&]
[s2; Возвращает текущую ёмкость этого 
Array`'я.&]
[s7; [*/ Возвратное значение]-| Ёмкость этого 
Array`'я.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Set`(int`,const T`&`,int`):%- [@(0.0.255) void]_[* Set]([@(0.0.255) int]_[*@3 i
], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x], [@(0.0.255) int]_[*@3 count])&]
[s2; Устанавливает требуемое число элементов, 
начиная с позиции [* i] и до указанного 
значения. Если необходимое число 
элементов превосходит существующие 
элементы, тогда в этот Array будут добавлены 
новые элементы.&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Начальная позиция.&]
[s7; [*C@3 x]-|Значение.&]
[s7; [*C@3 count]-|Число элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Set`(int`,const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Set]([@(0.0.255) int]_[*@3 i],
 [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s5;:Upp`:`:Array`:`:Set`(int`,T`&`&`):%- [*@4 T][@(0.0.255) `&]_[* Set]([@(0.0.255) int]_[*@3 i
], [*@4 T][@(0.0.255) `&`&]_[*@3 x])&]
[s2; Устанавливает элемент по [%-*@3 i] в [%-*@3 x] 
(при необходимости добавляя дефолтно 
конструируемые элементы) и возвращает 
ссылку на этот элемент.&]
[s3; &]
[s4;%- &]
[s5;:Array`:`:Remove`(int`,int`):%- [@(0.0.255) void]_[* Remove]([@(0.0.255) int]_[*@3 i], 
[@(0.0.255) int]_[*@3 count]_`=_[@3 1])&]
[s2; Удаляет требуемое число элементов.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция.&]
[s7; [*C@3 count]-|Число удаляемых элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Remove`(const int`*`,int`):%- [@(0.0.255) void]_[* Remove]([@(0.0.255) const]_
[@(0.0.255) int]_`*[*@3 sorted`_list], [@(0.0.255) int]_[*@3 n])&]
[s2; Удаляет число элементов из Array`'я. 
Время операции почти не зависит от 
числа элементов.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 sorted`_list]-|Указатель на Array удаляемых 
позиций. Он должен быть отсортирован 
от наименьшего к наибольшему значению.&]
[s7; [*C@3 n]-|Число удаляемых элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Remove`(const Vector`<int`>`&`):%- [@(0.0.255) void]_[* Remove]([@(0.0.255) c
onst]_[_^Vector^ Vector][@(0.0.255) <int>`&]_[*@3 sorted`_list])&]
[s2; Удаляет число элементов из этого 
Array`'я. То же, что и [* Remove(sorted`_list, sorted`_list.GetCount())].&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 sorted`_list]-|Отсортированный Vector удаляемых 
позиций.&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:RemoveIf`(Condition`):%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 C
ondition]>_[@(0.0.255) void]_[* RemoveIf]([*@4 Condition]_[*@3 c])&]
[s2; Удаляет элементы, при удовлетворении 
условия [%-*@3 c]. Условие `- это вызываемый 
(callable) (обычно, лямбда), который принимает 
(int i) как параметр и возвращает либо 
верно, либо неверно.&]
[s6; Повреждает итераторы к Array.&]
[s3; &]
[s4;%- &]
[s5;:Array`:`:InsertN`(int`,int`):%- [@(0.0.255) void]_[* InsertN]([@(0.0.255) int]_[*@3 i], 
[@(0.0.255) int]_[*@3 count]_`=_[@3 1])&]
[s2; Вставляет заданное число дефолтно 
конструируемых элементов по указанной 
позиции.&]
[s6; Требуется, чтобы у T был дефолтный 
конструктор.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция.&]
[s7; [*C@3 count]-|Число вставляемых элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Insert`(int`):%- [*@4 T][@(0.0.255) `&]_[* Insert]([@(0.0.255) int]_[*@3 i])&]
[s2; Вставляет один дефолтно конструируемый 
элемент по указанной позиции.&]
[s6; Требуется, чтобы у T был дефолтный 
конструктор.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Insert`(int`,const T`&`,int`):%- [@(0.0.255) void]_[* Insert]([@(0.0.255) int
]_[*@3 i], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x], [@(0.0.255) int]_[*@3 count])&]
[s2; Вставляет указанное число элементов, 
устанавливая их в указанное значение.&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция.&]
[s7; [*C@3 x]-|Значение вставляемых элементов.&]
[s7; [*C@3 count]-|Число вставляемых элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Insert`(int`,const T`&`):%- [*@4 T][@(0.0.255) `&]_[* Insert]([@(0.0.255) int]_
[*@3 i], [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2; Вставляет элемент по [%-*@3 i], устанавливая 
его в [%-*@3 x] и возвращает ссылку на негоt.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:InsertPick`(int`,T`&`&`):%- [*@4 T][@(0.0.255) `&]_[* InsertPick]([@(0.0.255) i
nt]_[*@3 i], [*@4 T][@(0.0.255) `&`&]_[*@3 x])&]
[s2; Вставляет элемент по [%-*@3 i], `"пикует`" 
контент [%-*@3 x] и возвращает ссылку на 
него.&]
[s3; &]
[s4;%- &]
[s5;:Array`:`:Insert`(int`,const Array`&`):%- [@(0.0.255) void]_[* Insert]([@(0.0.255) int]_
[*@3 i], [@(0.0.255) const]_[_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ Arr
ay][@(0.0.255) `&]_[*@3 x])&]
[s2; Вставляет все элементы из другого 
Array`'я.&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция.&]
[s7; [*C@3 x]-|Исходный Array.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Insert`(int`,const Array`&`,int`,int`):%- [@(0.0.255) void]_[* Insert]([@(0.0.255) i
nt]_[*@3 i], [@(0.0.255) const]_[_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ A
rray][@(0.0.255) `&]_[*@3 x], [@(0.0.255) int]_[*@3 offset], [@(0.0.255) int]_[*@3 count])&]
[s2; Вставляет диапазон элементов из 
другого Array`'я..&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция вставки.&]
[s7; [*C@3 x]-|Исходный Array.&]
[s7; [*C@3 offset]-|Позиция первого вставляемого 
элемента в исходном Array`'е.&]
[s7; [*C@3 count]-|Число вставляемых элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:Insert`(int`,std`:`:initializer`_list`<T`>`):%- [@(0.0.255) void]_
[* Insert]([@(0.0.255) int]_[*@3 i], [_^http`:`/`/en`.cppreference`.com`/w`/cpp`/utility`/initializer`_list^ s
td`::initializer`_list]<[*@4 T]>_[*@3 init])&]
[s2; Вставляет список инициализаторов 
в стиле C`+`+11.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:Append`(std`:`:initializer`_list`<T`>`):%- [@(0.0.255) void]_[* Appe
nd]([_^http`:`/`/en`.cppreference`.com`/w`/cpp`/utility`/initializer`_list^ std`::in
itializer`_list]<[*@4 T]>_[*@3 init])&]
[s2; Приставляет списов инициализаторов 
в стиле C`+`+11.&]
[s3; &]
[s4;%- &]
[s5;:Array`:`:Append`(const Array`&`):%- [@(0.0.255) void]_[* Append]([@(0.0.255) const]_[_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ A
rray][@(0.0.255) `&]_[*@3 x])&]
[s2; Приставляет все элементы исходного 
Array`'я..&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 x]-|Исходный Array.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Append`(const Array`&`,int`,int`):%- [@(0.0.255) void]_[* Append]([@(0.0.255) c
onst]_[_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ Array][@(0.0.255) `&]_[*@3 x
], [@(0.0.255) int]_[*@3 o], [@(0.0.255) int]_[*@3 c])&]
[s2; Приставляет диапазон элементов из 
исходного Array`'я..&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 x]-|Исходный Array.&]
[s7; [*C@3 o]-|Позиция первого вставляемого 
элемента в исходном Array`'е.&]
[s7; [*C@3 c]-|Число вставляемых элементов.&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:InsertPick`(int`,Upp`:`:Array`&`&`):%- [@(0.0.255) void]_[* InsertPi
ck]([@(0.0.255) int]_[*@3 i], [_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ A
rray][@(0.0.255) `&`&]_[*@3 x])&]
[s2; Вставляет исходный Array по указанной 
позиции, используя семантику `"пик`-трансфера
`". Работает быстрее, чем вставка глубокой 
копии, КГК для T не используется, но 
исходный Array рушится при пикинге.&]
[s3;%- &]
[s4;%- &]
[s5;:Upp`:`:Array`:`:AppendPick`(Upp`:`:Array`&`&`):%- [@(0.0.255) void]_[* AppendPick]([_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ A
rray][@(0.0.255) `&`&]_[*@3 x])&]
[s2; Приставляет исходный Array, используя 
семантику `"пик`-трансфера`". Работает 
быстрее, чем вставка глубокой копии, 
КГК для T не используется, но исходный 
Array рушится при пикинге.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Swap`(int`,int`):%- [@(0.0.255) void]_[* Swap]([@(0.0.255) int]_[*@3 i1], 
[@(0.0.255) int]_[*@3 i2])&]
[s2; Обменивается элементами, не используя 
КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i1]-|Позиция первого элемента.&]
[s7; [*C@3 i2]-|Позиция второго элемента.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Move`(int`,int`):%- [@(0.0.255) void]_[* Move]([@(0.0.255) int]_[*@3 i1], 
[@(0.0.255) int]_[*@3 i2])&]
[s2; Удаляет элемент в позиции [* i1] и вставляет 
его в [* i2], КГК не используется.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i1]-|Позиция удаляемого элемента.&]
[s7; [*C@3 i2]-|Целевая позиция.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Detach`(int`):%- [*@4 T]_`*[* Detach]([@(0.0.255) int]_[*@3 i])&]
[s2; Удаляет элемент в позиции [* i], `"сбрасывая`" 
с себя `"права собственности`". Клиент 
отвечает за удаление этого элемента.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция удаляемого элемента.&]
[s7; [*/ Возвратное значение]-|Указатель 
на элемент, размещённый в куче.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Swap`(int`,T`*`):%- [*@4 T]_`*[* Swap]([@(0.0.255) int]_[*@3 i], 
[*@4 T]_`*[*@3 newt])&]
[s2; Устанавливает элемент по [%-*@3 i] в [%-*@3 newt].и 
возвращает указатель на исходный 
элемент. Клиент отвечает за удаление 
исходного элемента.&]
[s3; &]
[s4;%- &]
[s5;:Array`:`:Set`(int`,T`*`):%- [*@4 T][@(0.0.255) `&]_[* Set]([@(0.0.255) int]_[*@3 i], 
[*@4 T]_`*[*@3 newt])&]
[s2; Заменяет элемент по указнной позиции 
на элемент, ранее созданный на куче. 
Array принимает права собственности 
на этот элемент.&]
[s7; [*C@3 i]-|Позиция.&]
[s7; [*C@3 newt]-|Новый элемент, размещённый 
на куче.&]
[s7; [*/ Возвратное значение]-|Ссылка на новый 
элемент.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Insert`(int`,T`*`):%- [@(0.0.255) void]_[* Insert]([@(0.0.255) int]_[*@3 i], 
[*@4 T]_`*[*@3 newt])&]
[s2; Вставляет элемент, ранее размещённый 
на куче по указанной позиции. Array принимает 
правообладание элементом.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 i]-|Позиция вставки.&]
[s7; [*C@3 newt]-|Новый элемент, размещённый 
на куче.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Drop`(int`):%- [@(0.0.255) void]_[* Drop]([@(0.0.255) int]_[*@3 n]_`=_[@3 1])&]
[s2; `"Роняет`" число последних элементов 
в этом Array`'е (то же, что и Trim(GetCount() `- 
n)).&]
[s7; [C@3 n][* -|]Число элементов&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Top`(`):%- [*@4 T][@(0.0.255) `&]_[* Top]()&]
[s2; Возвращает ссылку на последний элемент 
в этом Array`'е.&]
[s7; [*/ Возвратное значение]-|Ссылка на последний 
элемент в этом Array`'е.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Top`(`)const:%- [@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[* Top]()_[@(0.0.255) c
onst]&]
[s2; Возвращает константную ссылку на 
последний элемент в этом Array`'е.&]
[s7; [*/ Возвратное значение]-|Ссылка на последний 
элемент в этом Array`'е.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:PopDetach`(`):%- [*@4 T]_`*[* PopDetach]()&]
[s2; `"Роняет`" последний элемент в этом 
Array`'е, `"отказываясь`" от правообладания 
(то же, что и Detach(GetCount() `- 1)). Клиент 
отвечает за удаление элемента.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*/ Возвратное значение]-|Указатель 
на элемент на куче.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Swap`(Array`&`):%- [@(0.0.255) void]_[* Swap]([_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ A
rray][@(0.0.255) `&]_[*@3 b])&]
[s2; Обменивается контентом Array`'я с другим 
массивом за операцию константного 
времени.&]
[s7; [*C@3 b]-|Целевой массив.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:operator`<`<`(const T`&`):%- [_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ A
rray][@(0.0.255) `&]_[* operator<<]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s5;:Upp`:`:Array`:`:operator`<`<`(T`&`&`):%- [_^Upp`:`:Array^ Array][@(0.0.255) `&]_[* ope
rator<<]([*@4 T][@(0.0.255) `&`&]_[*@3 x])&]
[s2; Оператор, заменяющий метод [* Add]. Возвращая 
ссылку на этот Array, позволяет добавить 
в одном выражении несколько элементов, 
тем самым, например, позволяя конструировать
 временный Array как часть выражения, 
типа Foo((Array<int>() << 1 << 2 << 4)).&]
[s6; Требуется, чтобы у T был КГК.&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 x]-|Значение, трансферизуемое во 
вновь созданный элемент.&]
[s7; [*/ Возвратное значение]-|Ссылка на этот 
Array (`*this).&]
[s3; &]
[s4;%- &]
[s5;:Array`:`:operator`<`<`(T`*`):%- [_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ A
rray][@(0.0.255) `&]_[* operator<<]([*@4 T]_`*[*@3 newt])&]
[s2; Оператор, заменяющий [* void Add(T `*x)]. Возвращая 
ссылку на этот Array, позволяет добавить 
в одном выражении несколько элементов, 
тем самым, например, позволяя конструировать
 временный Array как часть выражения, 
типа  Foo((Array<Bar>() << new Bar << new DerivedFromBar)).&]
[s6; Повреждает итераторы к Array.&]
[s7; [*C@3 newt]-|Добавляемый объект.&]
[s7; [*/ Возвратное значение]-|Ссылка на этот 
Array (`*this).&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Serialize`(Stream`&`):%- [@(0.0.255) void]_[* Serialize]([_^topic`:`/`/Core`/src`/Stream`$en`-us`#Stream`:`:class^ S
tream][@(0.0.255) `&]_[*@3 s])&]
[s2; Сериализует контент этого Array`'я в/из 
потока Stream. Работает только при условии, 
что NTL используется как часть UPP. Не 
работает с полиморфными Array`'ями (в 
которых хранятся объекты, производные 
от T).&]
[s6; Требуется, чтобы у T была определена 
операция сериализации.&]
[s7; [*C@3 s]-|Целевой/исходный поток.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:operator`=`(pick`_ Array`&`):%- [@(0.0.255) void]_[* operator`=]([@(0.128.128) p
ick`_]_[_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ Array][@(0.0.255) `&]_
[*@3 v])&]
[s2; Pick`-оператор. `"Трансферит`" исходный 
Array за малое константное время, но 
рушит его при пикинге.&]
[s7; [*C@3 v]-|Исходный Array.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:ValueType`:`:typedef:%- [@(0.0.255) typedef]_T_[* ValueType]&]
[s2; Typedef T для использования в шаблонных 
алгоритмах.&]
[s3;%- &]
[s4;%- &]
[s1;:Array`:`:Iterator`:`:class:%- [@(0.0.255) class]_[* Iterator]_:_[@(0.0.255) public]_[*@3 C
onstIterator]&]
[s2; Тип обходчика.&]
[s3;%- &]
[s4;%- &]
[s1;:Upp`:`:Array`:`:ConstIterator`:`:class:%- [@(0.0.255) class]_[* ConstIterator]&]
[s2; Константный тип обходчика.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Begin`(`):%- [_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:Iterator`:`:class^ I
terator]_[* Begin]()&]
[s2; Возвращает неконстантный обходчик 
к первому элементу в этом Array`'е. &]
[s7; [*/ Возвратное значение]-|Обходчик.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:End`(`):%- [_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:Iterator`:`:class^ I
terator]_[* End]()&]
[s2; Возвращает неконстантный обходчик 
к позиции сразу за последним элементом 
в Array`'е.&]
[s7; [*/ Возвратное значение]-|Обходчик.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:GetIter`(int`):%- [_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:Iterator`:`:class^ I
terator]_[* GetIter]([@(0.0.255) int]_[*@3 pos])&]
[s2; Возвращает неконстантный обходчик 
к элементу по указанной позиции. То 
же, что и [* Begin() `+ pos]. Выгода этого метода 
в том, что [* pos] является диапазоном, 
проверяемым в режиме отладки.&]
[s7; [*C@3 pos]-|Требуемая позиция.&]
[s7; [*/ Возвратное значение]-|Обходчик.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:Begin`(`)const:%- [_^topic`:`/`/Core`/src`/Array`$en`-us`#Upp`:`:Array`:`:ConstIterator`:`:class^ C
onstIterator]_[* Begin]()_[@(0.0.255) const]&]
[s2; Возвращает константный обходчик 
к первому элементу в Array`'е. &]
[s7; [*/ Возвратное значение]-|Обходчик.&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:End`(`)const:%- [_^topic`:`/`/Core`/src`/Array`$en`-us`#Upp`:`:Array`:`:ConstIterator`:`:class^ C
onstIterator]_[* End]()_[@(0.0.255) const]&]
[s2; Возвращает константный обходчик 
к позиции сразу за последним элементом 
в Array`'е.&]
[s7; [*/ Возвратное значение]-|Обходчик.-|&]
[s3;%- &]
[s4;%- &]
[s5;:Array`:`:GetIter`(int`)const:%- [_^topic`:`/`/Core`/src`/Array`$en`-us`#Upp`:`:Array`:`:ConstIterator`:`:class^ C
onstIterator]_[* GetIter]([@(0.0.255) int]_[*@3 pos])_[@(0.0.255) const]&]
[s2; Возвращает константный обходчик 
к элементу по указанной позиции. То 
же, что и [* Begin() `+ pos]. Выгода этого метода 
в том, что [* pos] является диапазоном, 
проверяемым в режиме отладки.&]
[s7; [*C@3 pos]-|Требуемая позиция.&]
[s7; [*/ Возвратное значение]-|Обходчик.&]
[s3; &]
[s4;%- &]
[s5;%- friend_[@(0.0.255) void][@(64) _]Swap[@(64) (][_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ A
rray][@(64) `&_][@3 a][@(64) , ][_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:class^ A
rray][@(64) `&_][@3 b][@(64) )]&]
[s2; Специализация генерного [* Swap] для 
Array. Обменивает массивы за простую, 
постоянную во времени, операцию.&]
[s7; [*C@3 a]-|Первый обмениваемый Array.&]
[s7; [*C@3 b]-|Второй обмениваемый Array.&]
[s4;%- &]
[s5;%- friend_[@(0.0.255) void][@(64) _]IterSwap[@(64) (][_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:Iterator`:`:class^ I
terator][@(64) _][@3 a][@(64) , ][_^topic`:`/`/Core`/src`/Array`$en`-us`#Array`:`:Iterator`:`:class^ I
terator][@(64) _][@3 b][@(64) )]&]
[s2; Специализация генерного [* IterSwap] для 
Array. Обменивает элементы в Array без 
всяких требований к T.&]
[s7; [*C@3 a]-|Обходчик к первому элементу.&]
[s7; [*C@3 b]-|Обходчик ко второму элементу.&]
[s3; &]
[s0; ]]