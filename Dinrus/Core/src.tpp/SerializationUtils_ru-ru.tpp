topic "Утилиты сериализации";
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
[ {{10000@(113.42.0) [s0; [*@2;4 Утилиты сериализации]]}}&]
[s0;*@3;4 &]
[ {{10000t/25b/25@1 [s0;%- [* Список Функций]]}}&]
[s0;0%- &]
[s5;:Pack16`(Stream`&`,int`&`):%- [_^Stream^ Stream][@(0.0.255) `&]_[* Pack16]([_^Stream^ Str
eam][@(0.0.255) `&]_[*@3 s], [@(0.0.255) int`&]_[*@3 i])&]
[s2; Сериализует 32`-битные данные, оптимизируя 
их для 16`-битных значений. Если значение 
находится в диапазоне `-32767`-32767, оно 
сериализуется как 2 байта, иначе используетс
я 6 байтов.&]
[s7; [%-*C@3 s]-|Поток.&]
[s7; [%-*C@3 i]-|Сериализуемые данные.&]
[s7; [*/ Возвратное значение]-|s для сцепки.&]
[s3; &]
[s4;%- &]
[s5;:Pack16`(Stream`&`,int`&`,int`&`):%- [_^Stream^ Stream][@(0.0.255) `&]_[* Pack16]([_^Stream^ S
tream][@(0.0.255) `&]_[*@3 s], [@(0.0.255) int`&]_[*@3 i1], [@(0.0.255) int`&]_[*@3 i2])&]
[s5;:Pack16`(Stream`&`,int`&`,int`&`,int`&`):%- [_^Stream^ Stream][@(0.0.255) `&]_[* Pack16
]([_^Stream^ Stream][@(0.0.255) `&]_[*@3 s], [@(0.0.255) int`&]_[*@3 i1], 
[@(0.0.255) int`&]_[*@3 i2], [@(0.0.255) int`&]_[*@3 i3])&]
[s5;:Pack16`(Stream`&`,int`&`,int`&`,int`&`,int`&`):%- [_^Stream^ Stream][@(0.0.255) `&]_
[* Pack16]([_^Stream^ Stream][@(0.0.255) `&]_[*@3 s], [@(0.0.255) int`&]_[*@3 i1], 
[@(0.0.255) int`&]_[*@3 i2], [@(0.0.255) int`&]_[*@3 i3], [@(0.0.255) int`&]_[*@3 i4])&]
[s5;:Pack16`(Stream`&`,int`&`,int`&`,int`&`,int`&`,int`&`):%- [_^Stream^ Stream][@(0.0.255) `&
]_[* Pack16]([_^Stream^ Stream][@(0.0.255) `&]_[*@3 s], [@(0.0.255) int`&]_[*@3 i1], 
[@(0.0.255) int`&]_[*@3 i2], [@(0.0.255) int`&]_[*@3 i3], [@(0.0.255) int`&]_[*@3 i4], 
[@(0.0.255) int`&]_[*@3 i5])&]
[s2; Вызывает Pack16 для каждого индивидуального 
32`-битного целочисленного параметра.&]
[s3; &]
[s4;%- &]
[s5;:Load`(Callback1`<Stream`&`>`,Stream`&`):%- [@(0.0.255) bool]_[* Load]([_^Callback1^ Ca
llback1][@(0.0.255) <][_^Stream^ Stream][@(0.0.255) `&>]_[*@3 serialize], 
[_^Stream^ Stream][@(0.0.255) `&]_[*@3 stream])&]
[s2; Смарт`-реставрация сериализации. 
Сериализуемые данные представлены 
посредством обрвызова Callback. Вначале 
выполняется бэкап (резервирование) 
текущего состояния данных и сохранение 
в памяти. Затем данные загружаются 
из заданного потока. Если восстановление 
прошло успешно (не выведено исключения 
LoadingError), то Load возвращает true. Если 
выведено LoadingError, то будет впоймано 
Load, данные восстанавливаются из резервной 
копии и Load возвращает false.&]
[s7; [%-*C@3 serialize]-|Обрвызов функции сериализации.&]
[s7; [%-*C@3 stream]-|Поток.&]
[s7; [*/ Возвратное значение]-|true указывает 
на успех.&]
[s3; &]
[s4;%- &]
[s5;:Store`(Callback1`<Stream`&`>`,Stream`&`):%- [@(0.0.255) bool]_[* Store]([_^Callback1^ C
allback1][@(0.0.255) <][_^Stream^ Stream][@(0.0.255) `&>]_[*@3 serialize], 
[_^Stream^ Stream][@(0.0.255) `&]_[*@3 stream])&]
[s2; Сохранить сериализацию. Сериализуемые 
данные представлены обрвызовом Callback.&]
[s7; [%-*C@3 serialize]-|Обрвызов функции сериализации.&]
[s7; [%-*C@3 stream]-|Поток.&]
[s7; [*/ Возвратное значение]-|true, если данные 
успешно сохранены в поток.&]
[s3; &]
[s4;%- &]
[s5;:LoadFromFile`(Callback1`<Stream`&`>`,const char`*`):%- [@(0.0.255) bool]_[* LoadFrom
File]([_^Callback1^ Callback1][@(0.0.255) <]Stream[@(0.0.255) `&>]_[*@3 serialize], 
[@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 file]_`=_NULL)&]
[s2; Используя [^topic`:`/`/Core`/src`/SerializationUtils`_ru`-ru`#Load`(Callback1`<Stream`&`>`,Stream`&`)^ L
oad], данные восстанавливают из файла.&]
[s7; [%-*C@3 serialize]-|Обрвызов функции сериализации.&]
[s7; [%-*C@3 file]-|Имя файла.&]
[s7; [*/ Возвратное значение]-|true указывает 
на успех.&]
[s3; &]
[s4;%- &]
[s5;:StoreToFile`(Callback1`<Stream`&`>`,const char`*`):%- [@(0.0.255) bool]_[* StoreToFi
le]([_^Callback1^ Callback1][@(0.0.255) <]Stream[@(0.0.255) `&>]_[*@3 serialize], 
[@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 file]_`=_NULL)&]
[s2; Используя [^topic`:`/`/Core`/src`/SerializationUtils`_ru`-ru`#Store`(Callback1`<Stream`&`>`,Stream`&`)^ S
tore], данные сохраняются в файл.&]
[s7; [%-*C@3 serialize]-|Обрвызов функции сериализации.&]
[s7; [%-*C@3 file]-|Имя файла.&]
[s7; [*/ Возвратное значение]-|true указывает 
на успех.&]
[s3; &]
[s4;%- &]
[s5;:Load`(T`&`,Stream`&`):%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T][@(0.0.255) >]_
[@(0.0.255) bool]_[* Load]([*@4 T][@(0.0.255) `&]_[*@3 x], [_^Stream^ Stream][@(0.0.255) `&]_[*@3 s
])&]
[s2; Вызывает [^topic`:`/`/Core`/src`/SerializationUtils`_ru`-ru`#Load`(Callback1`<Stream`&`>`,Stream`&`)^ L
oad] обрвызовом(`&[@3 x], `&T`::Serialize), как обратный 
вызов сериализации. Другими словами, 
[@3 x] восстанавливается с помощью его 
метода Serialize.&]
[s7; [*C@4 T]-|Тип восстанавливаемого объекта.&]
[s7; [%-*C@3 x]-|Ссылка на объект.&]
[s7; [%-*C@3 s]-|Поток.&]
[s7; [*/ Возвратное значение]-|true указывает 
на успех.&]
[s3; &]
[s4;%- &]
[s5;:Store`(T`&`,Stream`&`):%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T][@(0.0.255) >
]_[@(0.0.255) bool]_[* Store]([*@4 T][@(0.0.255) `&]_[*@3 x], [_^Stream^ Stream][@(0.0.255) `&]_
[*@3 s])&]
[s2; Вызывает [^topic`:`/`/Core`/src`/SerializationUtils`_ru`-ru`#Store`(Callback1`<Stream`&`>`,Stream`&`)^ S
tore] обрвызовом(`&[@3 x], `&T`::Serialize), как обратный 
вызов сериализации. Другими словами, 
[@3 x] сохраняется с помощью его метода 
Serialize.&]
[s7; [*C@4 T]-|Тип восстанавливаемого объекта.&]
[s7; [%-*C@3 x]-|Ссылка на объект.&]
[s7; [%-*C@3 s]-|Поток.&]
[s7; [*/ Возвратное значение]-|true указывает 
на успех.&]
[s3; &]
[s4;%- &]
[s5;:LoadFromFile`(T`&`,const char`*`):%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T][@(0.0.255) >
]_[@(0.0.255) bool]_[* LoadFromFile]([*@4 T][@(0.0.255) `&]_[*@3 x], [@(0.0.255) const]_[@(0.0.255) c
har]_`*[*@3 name]_`=_NULL)&]
[s2; Вызывает [^topic`:`/`/Core`/src`/SerializationUtils`_ru`-ru`#LoadFromFile`(T`&`,const char`*`)^ L
oadFromFile] обрвызовом(`&[@3 x], `&T`::Serialize), как 
обратный вызов сериализации. Другими 
словами, [@3 x] восстанавливается из 
файла с помощью его метода Serialize.&]
[s7; [*C@4 T]-|Тип восстанавливаемого объекта.&]
[s7; [%-*C@3 x]-|Ссылка на объект.&]
[s7; [%-*C@3 name]-|Имя файла.&]
[s7; [*/ Возвратное значение]-|true указывает 
на успех.&]
[s3; &]
[s4;%- &]
[s5;:StoreToFile`(T`&`,const char`*`):%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T][@(0.0.255) >
]_[@(0.0.255) bool]_[* StoreToFile]([*@4 T][@(0.0.255) `&]_[*@3 x], [@(0.0.255) const]_[@(0.0.255) c
har]_`*[*@3 name]_`=_NULL)&]
[s2; Вызывает [^topic`:`/`/Core`/src`/SerializationUtils`_ru`-ru`#StoreToFile`(T`&`,const char`*`)^ S
toreToFile] обрвызовом(`&[@3 x], `&T`::Serialize), как 
обратный вызов сериализации. Другими 
словами, [@3 x] сохраняется в файл с помощью 
его метода Serialize.&]
[s7; [*C@4 T]-|Тип восстанавливаемого объекта.&]
[s7; [%-*C@3 x]-|Ссылка на объект.&]
[s7; [%-*C@3 name]-|Имя файла.&]
[s7; [*/ Возвратное значение]-|true указывает 
на успех.&]
[s3; &]
[s4;%- &]
[s5;:StoreAsString`(T`&`):%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T][@(0.0.255) >]_
[_^String^ String]_[* StoreAsString]([*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2; Сохраняет [@3 x], используя его метод 
Serialize и StringStream.&]
[s7; [*C@4 T]-|Тип восстанавливаемого объекта.&]
[s7; [%-*C@3 x]-|Ссылка на объект.&]
[s7; [*/ Возвратное значение]-|Строка, содержащая 
сериализуемый [@3 x].&]
[s3; &]
[s4;%- &]
[s5;:LoadFromString`(T`&`,const String`&`):%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T
][@(0.0.255) >]_[@(0.0.255) bool]_[* LoadFromString]([*@4 T][@(0.0.255) `&]_[*@3 x], 
[@(0.0.255) const]_[_^String^ String][@(0.0.255) `&]_[*@3 s])&]
[s2; Восстанавливает сериализованные 
данные из потока String (напр., ранее 
сохранённые посредством StoreAsString).&]
[s7; [*C@4 T]-|Тип восстанавливаемого объекта.&]
[s7; [%-*C@3 x]-|Ссылка на объект.&]
[s7; [%-*C@3 s]-|Строка, содержащая сериализованный 
[@3 x].&]
[s7; [*/ Возвратное значение]-|true указывает 
на успех.&]
[s0; &]
[s0; [*@3;4 Глобальная поддержка модульной 
сериализации]&]
[s0;# Модульная сериализация `- полезная 
опция для сохранения конфигурации 
приложений, состоящих из множества 
модулей. Она позволяет выполнять 
индивидуальное сохранение/загрузку 
данных из указанного глобального 
ключа конфигурации, а также сериализацию 
всех подобных данных единым потоком.&]
[s0;3 &]
[ {{10000t/25b/25@1 [s0;%- [* Список Функций]]}}&]
[s0;0%- &]
[s5;:RegisterGlobalConfig`(const char`*`):%- [@(0.0.255) void]_[* RegisterGlobalConfig]([@(0.0.255) c
onst]_[@(0.0.255) char]_`*[*@3 name])&]
[s7; Регестрирует имя как глобальный 
ключ конфигурации.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:RegisterGlobalSerialize`(const char`*`,Upp`:`:Event`<Upp`:`:Stream`&`>`):%- [@(0.0.255) v
oid]_[* RegisterGlobalSerialize]([@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 name], 
[_^Upp`:`:Event^ Event]<Stream[@(0.0.255) `&]>_[*@3 WhenSerialize])&]
[s2; Регестрирует имя как глобальный 
ключ конфигурации. [%-*@3 WhenSerialize] используктся 
напрямую для сериализации данных, 
в отличие от иных вариантов глобального 
ключа конфигурации, когда данные 
сохраняются/получаются обратно посредством
 LoadFromGlobal / StoreToGlobal.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:RegisterGlobalConfig`(const char`*`,Upp`:`:Event`<`>`):%- [@(0.0.255) void]_
[* RegisterGlobalConfig]([@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 name], 
[_^Upp`:`:Event^ Event]<>_[*@3 WhenFlush])&]
[s2; Регестрирует имя как глобальный 
ключ конфигурации. [%-*C@3 WhenFlush]  вызывается 
до сохранения всех конфигурационных 
ключей, выполняемого посредством 
SerializeGlobalConfigs, `- это полезно, когда 
StoreToGlobal нужно явно триггировать перед 
сохранением конфигурации.&]
[s3; &]
[s4;%- &]
[s5;:LoadFromGlobal`(T`&`,const char`*`):%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T
][@(0.0.255) >]_[@(0.0.255) bool]_[* LoadFromGlobal]([*@4 T][@(0.0.255) `&]_[*@3 x], 
[@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 name])&]
[s2; Загружает [%-*@3 x] из глобального ключа 
конфигурации [%-*@3 name], используя метод 
Serialize у T.&]
[s3; &]
[s4;%- &]
[s5;:StoreToGlobal`(T`&`,const char`*`):%- [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T
][@(0.0.255) >]_[@(0.0.255) void]_[* StoreToGlobal]([*@4 T][@(0.0.255) `&]_[*@3 x], 
[@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 name])&]
[s2; Сохраняет под глобальный ключ конфигурации
 [%-*@3 name], используя метод Serialize у T.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:LoadFromGlobal`(Upp`:`:Event`<Upp`:`:Stream`&`>`,const char`*`):%- [@(0.0.255) b
ool]_[* LoadFromGlobal]([_^Upp`:`:Event^ Event]<Stream[@(0.0.255) `&]>_[*@3 serialize], 
[@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 name])&]
[s2; Загружает [%-*@3 x] из глобального ключа 
конфигурации [%-*@3 name], используя [%-*@3 serialize] 
в качестве функции сериализации.&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:StoreToGlobal`(Upp`:`:Event`<Upp`:`:Stream`&`>`,const char`*`):%- [@(0.0.255) v
oid]_[* StoreToGlobal]([_^Upp`:`:Event^ Event]<Stream[@(0.0.255) `&]>_[*@3 x], 
[@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 name])&]
[s2; Сохраняет [@3 x] под глобальный ключ 
конфигурации [%-*@3 name], используя [%-*@3 serialize] 
в качестве функции сериализации.&]
[s3; &]
[s4;%- &]
[s5;:SerializeGlobalConfigs`(Stream`&`):%- [@(0.0.255) void]_[* SerializeGlobalConfigs]([_^Stream^ S
tream][@(0.0.255) `&]_[*@3 s])&]
[s2; Сериализует все зарегистрированные 
данные глобальной конфигурации из/в 
единый поток.&]
[s3; &]
[s0; ]]