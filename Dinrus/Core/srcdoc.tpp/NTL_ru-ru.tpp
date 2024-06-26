topic "Обзор контейнеров U++ - NTL";
[l288;i704;a17;O9;~~~.992;2 $$1,0#10431211400427159095818037425705:param]
[a83;*R6 $$2,5#31310162474203024125188417583966:caption]
[H4;b83;*4 $$3,5#07864147445237544204411237157677:title]
[b167;a42;C2 $$4,6#40027414424643823182269349404212:item]
[b42;a42;ph2 $$5,5#45413000475342174754091244180557:text]
[l288;a17;2 $$6,6#27521748481378242620020725143825:desc]
[l321;t246;C@5;1 $$7,7#20902679421464641399138805415013:code]
[b2503;2 $$8,0#65142375456100023862071332075487:separator]
[*@(0.0.255)2 $$9,0#83433469410354161042741608181528:base]
[t4167;C2 $$10,0#37138531426314131251341829483380:class]
[l288;a17;*1 $$11,11#70004532496200323422659154056402:requirement]
[i417;b42;a42;O9;~~~.416;2 $$12,12#10566046415157235020018451313112:tparam]
[b167;C2 $$13,13#92430459443460461911108080531343:item1]
[i288;a42;O9;C2 $$14,14#77422149456609303542238260500223:item2]
[*@2$(0.128.128)2 $$15,15#34511555403152284025741354420178:NewsDate]
[l321;*C$7;2 $$16,16#03451589433145915344929335295360:result]
[l321;b83;a83;*C$7;2 $$17,17#07531550463529505371228428965313:result`-line]
[l160;t4167;*C+117 $$18,5#88603949442205825958800053222425:package`-title]
[2 $$0,0#00000000000000000000000000000000:Default]
[{_}%RU-RU 
[s2; [@5 Обзор контейнеров U`+`+ `- NTL]&]
[s3; Содержание&]
[s0; &]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#1^ 1. Введение]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#2^ 2. Случайный 
доступ]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#3^ 3. Требования]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#4^ 4. Flavors]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#5^ 5. Контейнеры 
как возвратные значения]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#6^ 6. Базовые контейнеры 
случайного доступа]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#7^ 7. Двунаправленные 
контейнеры]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#8^ 8. Index]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#9^ 9. Maps]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#10^ 10. InVector и Sorted 
maps]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#11^ 11. One]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#12^ 12. Any]&]
[s0; [^topic`:`/`/Core`/srcdoc`/NTL`_ru`-ru`#13^ 13. Buffer]&]
[s0; &]
[s3;:1: [@(28.127.0) 1. Введение]&]
[s5; [* NTL] (Non`-standard Template library) `- это библиотека 
шаблонов контейнеров и алгоритмов. 
Она разработана с целью решения некоторых 
проблем, встречающихся в текущей 
стандартной библиотеке C`+`+ STL.&]
[s5; Примеры кода найдутся в [^topic`:`/`/Core`/srcdoc`/Tutorial`_ru`-ru^ У
чебнике по Core].&]
[s3;:2: [@(28.127.0) 2. Случайный доступ]&]
[s5; У каждого единичного шаблона контейнера 
NTL, который может хранить более одного 
элемента, случайный доступ. Этот случайный 
доступ так же быстр, как доступ с использован
ием итераторов. Для итерации элементов 
можно использовать индексы, а не итераторы, 
и мы рекомендуем такой подход.&]
[s3;:3: [@(28.127.0) 3. Требования]&]
[s5; Важная фича NTL `- требования к сохраняемым 
элементам. В отличие от STL, у которой 
есть единое [/ copy`-constructible] и [/ assignable] требование 
для всей библиотеки, NTL выставляет 
требования на `"по контейнерной`" и 
даже `"по методной`" основе. Таким образом 
NTL разрешает [/ прямое] сохранение [/ любого] 
типа элементов. NTL предоставляет два 
или даже три флейвора для каждого 
базового ADT типа контейнера, относящихся 
к требованиям к элементам.&]
[s3;:4: [@(28.127.0) 4. Флейворы]&]
[s5; [*/ Vector] `- флейвор контейнеров, требующий, 
чтобы типы были [*/^topic`:`/`/Core`/srcdoc`/Moveable`_ru`-ru^ moveabl
e (перемещаемыми)] и имели либо [*/^topic`:`/`/Core`/srcdoc`/pick`_`_ru`-ru^ d
eep copy constructor (конструктор глубокой 
копии)] и оператор, либо [*/^topic`:`/`/Core`/srcdoc`/pick`_`_ru`-ru^ p
ick constructor (пик`-конструктор)] и оператор 
или дефолтный конструктор. Контейнеры 
этого флейвора имеют в имени слово 
[* Vector], за исключением [* Index], который 
также из этого флейвора. У этого флейвора 
наилучшие рантаймные характеристики.&]
[s5; [*/ Array] flavor of containers has [*/ no][/  ]common requirements 
for elements, but is less efficient. You can use it to store 
even polymorphic types. Type of stored elements does not even 
have to be defined at the point of instantiation, so you can 
use Array flavor to create recursive data structures. Another 
feature of this flavor is that references to elements are never 
invalidated. Containers of this flavor have [* Array] in their 
name.&]
[s3;:5: [@(28.127.0) 5. Контейнеры как возвратные 
значения]&]
[s5; NTL allows containers to be used as return values, in single 
constant time operation. It is allowed by defining specific [*/^topic`:`/`/Core`/srcdoc`/pick`_`_ru`-ru^ t
ransfer semantics][/ .]&]
[s3;:6: [@(28.127.0) 6. Базовые контейнеры случайного 
доступа]&]
[s5; This kind of containers allows adding/removing elements at the 
end of the sequence in amortized constant time. As these containers 
are basic building blocks for NTL, their names are the same as 
names for all three flavors. [* Vector] flavor allows direct access 
to underlying C vector of elements. [* Vector] and [* Array] flavors 
allow removing and inserting of elements at any position (with 
complexity based on number of elements moved).&]
[s3;:7: [@(28.127.0) 7. Двунаправленные контейнеры]&]
[s5; This kind of containers allows adding/removing elements to/from 
both beginning and end of sequence in constant amortized time. 
It is well suited for FIFO operations. [* BiVector] and [* BiArray] 
are two flavors of bidirectional containers.&]
[s3;:8: [@(28.127.0) 8. Индекс]&]
[s5; Basically, this kind of container provides random access sequence 
that allows adding elements at the end of sequence in constant 
amortized time (much like basic random access containers) with 
one unique feature: it allows fast retrieval of position of element 
with specific value. Hashing is used for this operation. Indexes 
represent ideal building`-blocks for any associative operations. 
[* Index] and [* ArrayIndex ]are two flavors of indexes.&]
[s5; As both index flavors are built on top of according basic random 
access containers, they provide read`-only access or even pick 
operations to this container. That means you can supply elements 
to [* Index] by picking [* Vector ]in [* Index] constructor or get 
[* Vector] of elements of [* Index] in low constant time pick operation. 
Same is true for [* Array] and [* ArrayIndex.]&]
[s5; Also, as most of functionality added to basic random access 
containers is same for both [* Index] and [* ArrayIndex], most of 
operations are shared in common template class [* AIndex], which 
represents index container without concrete flavor. [* Index] and 
[* ArrayIndex] are derived from this [* AIndex], adding few operations 
specific for their flavors.&]
[s3;:9: [@(28.127.0) 9. Мапы]&]
[s5; Basically, maps are just simple compositions of Index and basic 
random access container to store values, thus getting classical 
map design. In find operation, position of key in Index is retrieved 
and value from basic access container at the same position is 
returned. This design also allows accessing both keys and values 
using indices. &]
[s5; In case of maps, keys are always stored in [* Index], that means 
that flavor is relative only to type of values (and keys must 
be moveable). [* VectorMap], [* ArrayMap] and [* SegtorMap] are flavors 
of maps. As with [* Index], the common operations of this flavor 
are implemented in template base class [* AMap]. Also, picking 
operations for any part of maps are available.&]
[s3;:10: [@(28.127.0) 10. InVector и Сортированные Мапы]&]
[s5; NTL provides random access containers with fast insertion at 
arbitrary position, which scale well to milions of items. These 
containers then provide basis for `'sorted maps`' that are using 
binary search. Sorted maps are useful when range`-search is required.&]
[s3;:11: [@(28.127.0) 11. One]&]
[s5; [* One] is a container that can hold one or none elements of the 
specified type or a type derived from it. Its functionally rather 
close to standard library auto`_ptr but with improvements in 
direction of NTL, like transfer semantics, moveable concept (yes, 
you [/ can] have Vector< One<T> >) and others. Also it is important 
from conceptual view, because it is it is treated like a container 
rather than a pointer.&]
[s3;:12: [@(28.127.0) 12. Any]&]
[s5; Any is a container that can store none or single element of 
[/ any] type. Container provides Is<T> method to determine the 
type of stored element and Create<T> method to create the element 
in container.&]
[s3;:13: [@(28.127.0) 13. Buffer]&]
[s0; Buffer is a simple random access container without growing properties.&]
[s0; ]]