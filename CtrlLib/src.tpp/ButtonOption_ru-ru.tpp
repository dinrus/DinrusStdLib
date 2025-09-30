topic "Класс ButtonOption  (ОпцияКнопка)";
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
[ {{10000@3 [s0; [*@(229)4 Класс ButtonOption (ОпцияКнопка)]]}}&]
[s3; &]
[s1;:ButtonOption`:`:class:%- [@(0.0.255)3 class][3 _][*3 ButtonOption][3 _:_][@(0.0.255)3 publi
c][3 _][*@3;3 Ctrl]&]
[s2; &]
[s2; 
@@image:225&125
(AyQAFAAAAOz/AAAAAHic3ZXLSsNAFIbzGD6IK0HwFQTBnSC4ciMIuqkgLopuRKS2sZvEpYJuFEEXihIFxWskoBWhaJterPdbilnUHw4OQ2YysSUrh5/hJJx/vjkHctJo/Nv1UL26dCz7ZOv4YEMt5CAT+S1YiIUY6htKdnQPt7X3h6mzZ3RgZLrs5ryvlxYsxAIaLsg6tG+LFWntd251/8iBEfK/PWYZS60mjZ2ZpVNRk+bueHqdWegcVIrrARTZ7dn0HK7KLABJKbyAIwtjoV5U5Pu+pml++DKMDDKZBRVpv4s/n39EdWThWQiQU697/B7Q8mKWZ4knS9+EsT4/3kl0WwrYHi/r7fWZxGIKaI9kSVsaxnp6rJGksZolBmpW7b7M74HHGFmVUgFp2EkU87vICjTt7z10C3m1pHVFSmRhMtxcX6hlmvM8C9+OsXam1tSCFWDh08YIcs731ErpeldvglkwE1a2bbUm9E2y0MJwo6mFEYTJgEaJQkUADSYyEM1DsgCH6qStQ0UAMQuxMLFLxRyMuKpiaON6cGFi05xv1kIs/FwQM7RiIQeZ9P9q1hKZGe/6AX9k/30=)
&]
[s2; &]
[s9; Простой Ctrl с двумя состояниями: подобен 
Option, но с внешним видом кнопки Button. 
Нажатое состояние персистентно (до 
следующего клика) и представляет 
собой true. Значение ButtonOption может быть 
либо 1, либо 0.&]
[s0; &]
[s0;%- [%RU-RU/ Производный от][%RU-RU  ][^topic`:`/`/CtrlCore`/src`/Ctrl`_ru`-ru`#Ctrl`:`:class^ C
trl]&]
[s3; &]
[s0;%- &]
[ {{10000F(128)G(128)@1 [s0; [* Список Публичных Методов]]}}&]
[s3; &]
[s5;:ButtonOption`:`:SetImage`(const Image`&`):%- [_^ButtonOption^ ButtonOption][@(0.0.255) `&
]_[* SetImage]([@(0.0.255) const]_[_^Image^ Image][@(0.0.255) `&]_[*@3 img])&]
[s2; Устанавливает изображение [%-*C@3 img], 
отображаемое в ButtonOption. Возвращает 
`*this.&]
[s3; &]
[s4;%- &]
[s5;:ButtonOption`:`:SetImage`(const Image`&`,const Image`&`):%- [_^ButtonOption^ Butto
nOption][@(0.0.255) `&]_[* SetImage]([@(0.0.255) const]_[_^Image^ Image][@(0.0.255) `&]_[*@3 m
], [@(0.0.255) const]_[_^Image^ Image][@(0.0.255) `&]_[*@3 m1])&]
[s2; Устанавливает изображение [%-*C@3 m], 
отображаемое для состояния `"false`" 
и [%-*C@3 m1], для состояния `"true`".&]
[s3; &]
[s4;%- &]
[s5;:ButtonOption`:`:operator`=`(const Image`&`):%- [@(0.0.255) void]_[* operator`=]([@(0.0.255) c
onst]_[_^Image^ Image][@(0.0.255) `&]_[*@3 img])&]
[s2; Эквивалентно SetImage([*C@3 img]).&]
[s3; &]
[s4;%- &]
[s5;:ButtonOption`:`:Set`(bool`):%- [@(0.0.255) void]_[* Set]([@(0.0.255) bool]_[*@3 b])&]
[s2; Устанавливает ButtonOption в [%-*C@3 b].&]
[s3; &]
[s4;%- &]
[s5;:ButtonOption`:`:Get`(`)const:%- [@(0.0.255) bool]_[* Get]()_[@(0.0.255) const]&]
[s2; Возвращает состояние ButtonOption.&]
[s3; &]
[s4;%- &]
[s5;:ButtonOption`:`:operator`=`(bool`):%- [@(0.0.255) void]_[* operator`=]([@(0.0.255) boo
l]_[*@3 b])&]
[s2; Эквивалентно Set([*C@3 b]).&]
[s3; &]
[s4;%- &]
[s5;:ButtonOption`:`:operator bool`(`)const:%- [* operator_bool]()_[@(0.0.255) const]&]
[s2; Эквивалентно Get().&]
[s3; &]
[s4;%- &]
[s5;:ButtonOption`:`:SetLabel`(const String`&`):%- [_^ButtonOption^ ButtonOption][@(0.0.255) `&
]_[* SetLabel]([@(0.0.255) const]_[_^String^ String][@(0.0.255) `&]_[*@3 text])&]
[s2; Устанавливает ярлык кнопки в [%-*@3 text].&]
[s3; &]
[s4;%- &]
[s5;:ButtonOption`:`:GetLabel`(`)const:%- [_^String^ String]_[* GetLabel]()_[@(0.0.255) con
st]&]
[s2; Возвращает  ярлык кнопки.&]
[s3;%- &]
[s4;%- &]
[s5;:ButtonOption`:`:StyleDefault`(`):%- [@(0.0.255) static] [@(0.0.255) const]_[_^ButtonOption`:`:Style^ S
tyle][@(0.0.255) `&]_[* StyleDefault]()&]
[s2; Возвращает дефолтный стиль, используемый 
для черчения `"нормальной`" ButtonOption.&]
[s0; &]
[s4;%- &]
[s5;:ButtonOption`:`:StyleFlat`(`):%- [@(0.0.255) static] [@(0.0.255) const]_[_^ButtonOption`:`:Style^ S
tyle][@(0.0.255) `&]_[* StyleFlat]()&]
[s2; Возвращает дефолтный стиль, используемый 
для черчения `"плоской`" ButtonOption.&]
[s3;%- &]
[s4;%- &]
[s5;:ButtonOption`:`:SetStyle`(const ButtonOption`:`:Style`&`):%- [_^ButtonOption^ Butt
onOption][@(0.0.255) `&]_[* SetStyle]([@(0.0.255) const]_[_^ButtonOption`:`:Style^ Style][@(0.0.255) `&
]_[*@3 s])&]
[s2; Устанавливает стиль кнопки в [%-*@3 s]. 
Возвращает  `*this для сцепления.&]
[s3; &]
[s4;%- &]
[s5;:ButtonOption`:`:AutoStyle`(`):%- [_^ButtonOption^ ButtonOption][@(0.0.255) `&]_[* Auto
Style]()&]
[s2; Очищает всю информацию о стиле, возвращая 
к виду по умолчанию. Возвращает `*this 
для сцепления.&]
[s3;%- &]
[s0; &]
[s0;%- &]
[s0;%- &]
[ {{10000@3 [s0; [*@(229)4 Структура ButtonOption`::Style]]}}&]
[s3; &]
[s1;:ButtonOption`:`:Style`:`:struct:%- [@(0.0.255)3 struct][3 _][*3 Style][3 _:_][@(0.0.255)3 p
ublic][3 _][*@3;3 ChStyle][@(0.0.255)3 <][*3 Style][@(0.0.255)3 >][3 _]&]
[s9; Эта структура определяет внешний 
вид (look) ButtonOption.&]
[s3;%- &]
[s0; ]]