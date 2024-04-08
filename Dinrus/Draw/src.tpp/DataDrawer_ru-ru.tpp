topic "Класс DataDrawer";
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
[ {{10000@(113.42.0) [s0;%- [*@7;4 Класс DataDrawer]]}}&]
[s3;%- &]
[s1;:DataDrawer`:`:class:%- [@(0.0.255)3 class][3 _][*3 DataDrawer]&]
[s9; [*@(128.0.255) Этот абстрактный класс предоставляе
т возможность отображать двоичные 
даннные определённого типа в выводе 
Draw. Каждому типу двоичных данных присвоен 
id, который используется при вызове 
метода Draw`::DrawData, На этот id есть соответствующ
ий зарегистрированный (с помощью 
DataDrawer`::Register) класс DataDrawer. Draw`::DrawData создаёт 
экземпляр этого класса и использует 
его для отображения двоичных данных 
как один или более массивов пикселей 
ImageBuffer.]&]
[s9; [*@(128.0.255) Пакет Draw предопределяет DataDrawer, 
зарегистрированный с id `"image`_data`". Он 
представляет Image, закодированный 
двоично в любой кодировке, поддерживаемой 
включаемыми плагинами форматов изображений
, типа plugin/bmp или plugin/jpg].&]
[s3; &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Методов]]}}&]
[s3;%- &]
[s5;:DataDrawer`:`:Open`(const String`&`,int`,int`):%- [*@(0.0.255) virtual][*  
][*@(0.0.255) void][* _Open]([@(0.0.255) const]_[_^topic`:`/`/Core`/src`/String`$en`-us^ S
tring][@(0.0.255) `&]_[*@3 data], [@(0.0.255) int]_[*@3 cx], [@(0.0.255) int]_[*@3 cy])_`=_[@3 0
]&]
[s2; Вызывается для открытия [%-*@3 data] для 
обработки, размер вывода равен [%-*@3 cx], 
[%-*@3 cy].&]
[s3; &]
[s4; &]
[s5;:DataDrawer`:`:Render`(ImageBuffer`&`):%- [*@(0.0.255) virtual][*  
][*@(0.0.255) void][* _Render]([_^topic`:`/`/Draw`/src`/ImageBuffer`$en`-us^ ImageBuffe
r][@(0.0.255) `&]_[*@3 ib])_`=_[@3 0]&]
[s2; Вызывается для отображения части 
(или всего) результата в [%-*@3 ib]. Ширина 
[%-*@3 ib] такая же как задано в Open, высота 
соответствует тредуемой высоте отображаемо
го диапазона (band to render).&]
[s3; &]
[s4; &]
[s5;:DataDrawer`:`:`~DataDrawer`(`):%- [@(0.0.255) `~][* DataDrawer]()&]
[s2; Виртуальный деструктор.&]
[s3; &]
[s4; &]
[s5;:DataDrawer`:`:Create`(const String`&`):%- [*@(0.0.255) static][*  
][*_^topic`:`/`/Core`/src`/One`$en`-us^ One][* <][*_^DataDrawer^ DataDrawer][* >_Create(][*@(0.0.255) c
onst][* _][*_^topic`:`/`/Core`/src`/String`$en`-us^ String][*@(0.0.255) `&][* _][*@3 id][* )]&]
[s2; Создаёт произвольный DataDrawer с типом 
[%-*@3 id].&]
[s3; &]
[s4; &]
[s5;:DataDrawer`:`:Register`(const char`*`):%- [*@(0.0.255) static][*  
][*@(0.0.255) template][* _<][*@(0.0.255) class][* _][*@4 T][* >_][*@(0.0.255) void][* _Register(
][*@(0.0.255) const][* _][*@(0.0.255) char][* _`*][*@3 id][* )]&]
[s2; Этот метод регистрирует специфичный 
производный от DataDrawer класс, как отображатель
 для типа [%-*@3 id].&]
[s3; &]
[s0; ]]