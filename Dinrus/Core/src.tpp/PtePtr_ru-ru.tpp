topic "Классы Ptr и Pte";
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
[ {{10000@(113.42.0) [s0; [*@7;4 Классы Ptr и Pte]]}}&]
[s9; Шаблоны классов Ptr и Pte предоставляют 
систему смарт`-указателей, которые 
очищаются (им присваивается NULL) при 
разрушение указываемого ими объекта. 
Это делает их полезными в ситуациях, 
когда срок жизни объекта точно неопределяем
.&]
[s9; В то время как шаблон класса [* Ptr ]предоставля
ет смарт`-указатели, [* Pte ]`- класс, добавляющий 
необходимый функционал указываемым 
им объектам. [* Ptr ]может указывать только 
на объекты типа, производного от [* Pte]. 
Чтобы класс стал `"[* Ptr]`-able`", нужно добавить 
[* Pte ]как один из классов`-основ, с производним 
классом, в качестве его параметра, 
напр.:&]
[s0; [C+75 struct Foo : Pte<Foo> `{....]&]
[s3; &]
[s0; &]
[ {{10000@(113.42.0) [s0; [*@7;4 Класс Pte]]}}&]
[s3;%- &]
[s1;:noref:%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T][@(0.0.255) >]&]
[s1;:Pte`:`:class:%- [@(0.0.255) class]_[* Pte]_:_[@(0.0.255) public]_[*@3 PteBase]&]
[s0; &]
[s0; Этот шаблонный класс реализует функционал, 
необходимый в указываемом объекте.&]
[s0; [*C@4 T]-|Тип указываемого объекта.&]
[s0; &]
[s0; [/ Производный от] [^topic`:`/`/Core`/src`/PtePtr`_ru`-ru`#Ptr`:`:class^ P
teBase]&]
[s3; &]
[s0;%- &]
[ {{10000@(113.42.0) [s0; [*@7;4 Класс Ptr]]}}&]
[s3;%- &]
[s1;:noref:%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T][@(0.0.255) >]&]
[s1;:Ptr`:`:class:%- [@(0.0.255) class]_[* Ptr]_:_[@(0.0.255) public]_[*@3 PtrBase], 
[@(0.0.255) private]_[*@3 Moveable][@(0.0.255) <]_[* Ptr][@(0.0.255) <][*@4 T][@(0.0.255) >]_>_
&]
[s0;%- &]
[s0; Указатель на специфичный тип.&]
[s0; [*C@4 T]-|Тип указываемого объекта. Класс 
T должен быть производным от Pte<T>.&]
[s0; &]
[s0; [/ Производный от] [^topic`:`/`/Core`/src`/PtePtr`_ru`-ru`#Ptr`:`:class^ P
trBase]&]
[s3; &]
[s1;%- &]
[ {{10000F(128)G(128)@1 [s0; [* Детали Конструктора]]}}&]
[s3;%- &]
[s5;:Ptr`:`:Ptr`(`):%- [* Ptr]()&]
[s2; Дефолтный конструктор.&]
[s3; &]
[s4;%- &]
[s5;:Ptr`:`:Ptr`(T`*`):%- [* Ptr]([*@4 T]_`*[*@3 ptr])&]
[s2; Конструирует Ptr, указывающий на заданный 
объект [%-*@3 ptr].&]
[s3; &]
[s4;%- &]
[s5;:Ptr`:`:Ptr`(const Ptr`&`):%- [* Ptr]([@(0.0.255) const]_[* Ptr][@(0.0.255) `&]_[*@3 ptr])&]
[s2; Конструирует Ptr, указывающий на тот 
же объект, что и другой Ptr [%-*@3 ptr].&]
[s3; &]
[s1;%- &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Методов]]}}&]
[s3; &]
[s5;:Ptr`:`:operator`-`>`(`)const:%- [*@4 T]_`*[* operator`->]()_[@(0.0.255) const]&]
[s2; Возвращает указатель C`+`+ на указываемый 
объект или NULL, если Ptr не указывает 
ни на какой объект.&]
[s3;%- &]
[s4;%- &]
[s5;:Ptr`:`:operator`~`(`)const:%- [*@4 T]_`*[* operator`~]()_[@(0.0.255) const]&]
[s2; Возвращает указатель C`+`+ на указываемый 
объект или NULL, если Ptr не указывает 
ни на какой объект.&]
[s3;%- &]
[s4;%- &]
[s5;:Ptr`:`:operator T`*`(`)const:%- [* operator_T`*]()_[@(0.0.255) const]&]
[s2; Возвращает указатель C`+`+ на указываемый 
объект или NULL, если Ptr не указывает 
ни на какой объект.&]
[s3;%- &]
[s4;%- &]
[s5;:Ptr`:`:operator`=`(T`*`):%- [_^Ptr^ Ptr][@(0.0.255) `&]_[* operator`=]([*@4 T]_`*[*@3 ptr])
&]
[s2; Присваивает новый указатель. Возвращает 
`*this.&]
[s3;%- &]
[s4;%- &]
[s5;:Ptr`:`:operator`=`(const Ptr`&`):%- [_^Ptr^ Ptr][@(0.0.255) `&]_[* operator`=]([@(0.0.255) c
onst]_[_^Ptr^ Ptr][@(0.0.255) `&]_[*@3 ptr])&]
[s2; Присваивает другой Ptr. Возвращает 
`*this.&]
[s3; &]
[s4;%- &]
[s5;:Ptr`:`:ToString`(`)const:%- [_^String^ String]_[* ToString]()_[@(0.0.255) const]&]
[s2; Преобразует всю информацию в string 
для диагностических целей&]
[s3; &]
[s0;i448;a25;kKO9;@(0.0.255)%- &]
[ {{10000F(128)G(128)@1 [s0; [* Глобальные Операторы]]}}&]
[s3; &]
[s5;K%- friend_[@(0.0.255) bool][@(64) _]operator`=`=[@(64) (][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) P
tr][@(64) `&_][@3 a][@(64) , ][@(0.0.255) const][@(64) _T_`*][@3 b][@(64) )]&]
[s5;K%- friend_[@(0.0.255) bool][@(64) _]operator`=`=[@(64) (][@(0.0.255) const][@(64) _T_`*][@3 a
][@(64) , ][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) Ptr][@(64) `&_][@3 b][@(64) )]&]
[s5;K%- friend_[@(0.0.255) bool][@(64) _]operator`=`=[@(64) (][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) P
tr][@(64) `&_][@3 a][@(64) , ][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) Ptr][@(64) `&_][@3 b][@(64) )
]&]
[s5;K%- friend_[@(0.0.255) bool][@(64) _]operator`=`=[@(64) (][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) P
tr][@(64) `&_][@3 a][@(64) , T_`*][@3 b][@(64) )]&]
[s5;K%- friend_[@(0.0.255) bool][@(64) _]operator`=`=[@(64) (T_`*][@3 a][@(64) , 
][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) Ptr][@(64) `&_][@3 b][@(64) )]&]
[s5;K%- friend_[@(0.0.255) bool][@(64) _]operator!`=[@(64) (][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) P
tr][@(64) `&_][@3 a][@(64) , ][@(0.0.255) const][@(64) _T_`*][@3 b][@(64) )]&]
[s5;K%- friend_[@(0.0.255) bool][@(64) _]operator!`=[@(64) (][@(0.0.255) const][@(64) _T_`*][@3 a
][@(64) , ][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) Ptr][@(64) `&_][@3 b][@(64) )]&]
[s5;K%- friend_[@(0.0.255) bool][@(64) _]operator!`=[@(64) (][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) P
tr][@(64) `&_][@3 a][@(64) , ][@(0.0.255) const][@(64) _][^`:`:Ptr^@(64) Ptr][@(64) `&_][@3 b][@(64) )
]&]
[s2; Операторы сравнения.&]
[s3; &]
[s0; ]]