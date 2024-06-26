topic "Класс JsonIO и фреймворк Jsonize";
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
[{_} 
[ {{10000@(113.42.0) [s0;%RU-RU [*@7;4 Класс JsonIO и фреймворк Jsonize]]}}&]
[s3; &]
[s1;:JsonIO`:`:class: [@(0.0.255)3 class][3 _][*3 JsonIO]&]
[s2;%RU-RU JsonIO представляет собой единичное 
значение JSON в фреймворке Jsonize. Оно 
используется в качестве параметра 
для глобальной функции Jsonize. Глобальная 
функция Jsonize имеет дефолтное шаблонное 
определение, которое, в свою очередь, 
вызывает метод Jsonize объекта. Однако, 
когда невозможно использовать этот 
метод в реализации (напр., из`-за поддерживаем
ых примитивных типов), может быть 
использована глобальная шаблонная 
специализация Jsonize.&]
[s2;%RU-RU &]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%RU-RU [* Список Публичных Методов]]}}&]
[s3;%RU-RU &]
[s5;:JsonIO`:`:IsLoading`(`)const: [@(0.0.255) bool]_[* IsLoading]()_[@(0.0.255) const]&]
[s2;%RU-RU Возвращает true при получение данных 
из JSON.&]
[s3; &]
[s4; &]
[s5;:JsonIO`:`:IsStoring`(`)const: [@(0.0.255) bool]_[* IsStoring]()_[@(0.0.255) const]&]
[s2;%RU-RU Возвращает true при сохранении 
данных в JSON.&]
[s3; &]
[s4; &]
[s5;:JsonIO`:`:Get`(`)const: [@(0.0.255) const]_[_^Value^ Value][@(0.0.255) `&]_[* Get]()_[@(0.0.255) c
onst]&]
[s2;%RU-RU Возвращает значение узла JSON при 
загрузке.&]
[s3; &]
[s4; &]
[s5;:JsonIO`:`:Set`(const Value`&`): [@(0.0.255) void]_[* Set]([@(0.0.255) const]_[_^Value^ V
alue][@(0.0.255) `&]_[*@3 v])&]
[s2;%RU-RU Устанавливает значение узла JSON 
при сохранении.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:JsonIO`:`:Get`(const char`*`): [_^Upp`:`:Value^ Value]_[* Get]([@(0.0.255) cons
t]_[@(0.0.255) char]_`*[*@3 key])&]
[s2;%RU-RU Возвращает значение подузла JSON 
[%-*@3 key] при загрузке.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:JsonIO`:`:Set`(const char`*`,const Upp`:`:Value`&`): [@(0.0.255) void]_[* Set
]([@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 key], [@(0.0.255) const]_[_^Upp`:`:Value^ Val
ue][@(0.0.255) `&]_[*@3 v])&]
[s2;%RU-RU Устанавливает значение подузла 
JSON [%-*@3 key] апи сохранении.&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:JsonIO`:`:Put`(Value`&`): [@(0.0.255) void]_[* Put]([_^Value^ Value][@(0.0.255) `&]_[*@3 v
])&]
[s2;%RU-RU Помещает данные узла JSON в [%-*@3 v] 
при сохранении `- вызывается при завершении 
`"джейсонизации`".&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:JsonIO`:`:GetResult`(`)const: [_^Value^ Value]_[* GetResult]()_[@(0.0.255) const]&]
[s2;%RU-RU Возвращает данные узла JSON в [%-*@3 v] 
при сохранении `- вызывается при завершении 
`"джейсонизации`"..&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:JsonIO`:`:operator`(`)`(const char`*`,T`&`): [@(0.0.255) template]_<[@(0.0.255) clas
s]_[*@4 T]>_[_^JsonIO^ JsonIO][@(0.0.255) `&]_[* operator()]([@(0.0.255) const]_[@(0.0.255) c
har]_`*[*@3 key], [*@4 T][@(0.0.255) `&]_[*@3 value])&]
[s2;%RU-RU Устанавливает JsonIO, представляющее 
объект JSON (если этого ещё нет), создаёт 
член [%-*@3 key] и джейсонирует [%-*@3 value] в 
него (вызывая глобальную функцию 
Jsonize, общая форма которой вызывает 
метод Jsonize).&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:JsonIO`:`:operator`(`)`(const char`*`,T`&`,const T`&`): [@(0.0.255) templat
e]_<[@(0.0.255) class]_[*@4 T]>_[_^Upp`:`:JsonIO^ JsonIO][@(0.0.255) `&]_[* operator()]([@(0.0.255) c
onst]_[@(0.0.255) char]_`*[*@3 key], [*@4 T][@(0.0.255) `&]_[*@3 value], 
[@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 defvalue])&]
[s2;%RU-RU Устанавливает JsonIO, представляющее 
объект JSON (если этого ещё нет), создаёт 
член [%-*@3 key] и джейсонирует [%-*@3 value] в 
него (вызывая глобальную функцию 
Jsonize, общая форма которой вызывает 
метод Jsonize). Если при загрузке [%-*@3 key] 
отсутствует в исходных данных, [%-*@3 defvalue] 
присваивается [%-*@3 value].&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:JsonIO`:`:List`(const char`*`,const char`*`,T`&`): [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 T]>_[_^Upp`:`:JsonIO^ JsonIO][@(0.0.255) `&]_[* List]([@(0.0.255) const]_[@(0.0.255) c
har]_`*[*@3 key], [@(0.0.255) const]_[@(0.0.255) char]_`*, [*@4 T][@(0.0.255) `&]_[*@3 var])&]
[s2;%RU-RU То же, что и operator()([%-*@3 key], [%-*@3 var]). 
Предоставляет совместимость с Xmlize 
для общих шаблонных процедур, предоставляющ
их как код Jsonize, так и Xmlize.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:JsonIO`:`:Var`(const char`*`,T`&`,X`): [@(0.0.255) template]_<[@(0.0.255) cla
ss]_[*@4 T], [@(0.0.255) class]_[*@4 X]>_[_^Upp`:`:JsonIO^ JsonIO][@(0.0.255) `&]_[* Var]([@(0.0.255) c
onst]_[@(0.0.255) char]_`*[*@3 key], [*@4 T][@(0.0.255) `&]_[*@3 value], 
[*@4 X]_[*@3 item`_jsonize])&]
[s2;%RU-RU Подобно оператору()([%-*@3 key], [%-*@3 value]), 
но, вместо использования глобальной 
фунции Jsonize, применяет [%-*@3 item`_jsonize], 
чтобы определить структуру.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:JsonIO`:`:Array`(const char`*`,T`&`,X`,const char`*`): [@(0.0.255) template
]_<[@(0.0.255) class]_[*@4 T], [@(0.0.255) class]_[*@4 X]>_[_^Upp`:`:JsonIO^ JsonIO][@(0.0.255) `&
]_[* Array]([@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 key], [*@4 T][@(0.0.255) `&]_[*@3 value
], [*@4 X]_[*@3 item`_jsonize], [@(0.0.255) const]_[@(0.0.255) char]_`*_`=_NULL)&]
[s2;%RU-RU Джейсонирует контейнер массива 
(должен иметь методы GetCount, Add и оператор`[`]), 
используя [%-*@3 item`_jsonize] для определения 
структуры элементов. Последний параметр 
игнорируется и включён только для 
обеспечения совместимости с кодом 
Xmlize для общих шаблонных процедур, 
предоставляющих как код Jsonize, так 
и Xmlize.&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:JsonIO`:`:JsonIO`(const Value`&`): [* JsonIO]([@(0.0.255) const]_[_^Value^ Value][@(0.0.255) `&
]_[*@3 src])&]
[s2;%RU-RU Устанавливает JsonIO для получения 
данных из источника [%-*@3 src], который 
должен иметь совместимое с Json значение.&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:JsonIO`:`:JsonIO`(`): [* JsonIO]()&]
[s2;%RU-RU Создаёт JsonIO для сохранения данных.&]
[s3; &]
[s0; &]
[ {{10000@(113.42.0) [s0;%RU-RU [*@7;4  Функции Store/Load из Jsonize]]}}&]
[s3; &]
[s5;:StoreAsJsonValue`(const T`&`): [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T]>_[_^Value^ V
alue]_[* StoreAsJsonValue]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 var])&]
[s2;%RU-RU Преобразует [%-*@3 var] в совместимое 
с JSON значение (такое, которое может 
непосредственно использоваться функцией 
AsJson).&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:LoadFromJsonValue`(T`&`,const Value`&`): [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T
]>_[@(0.0.255) void]_[* LoadFromJsonValue]([*@4 T][@(0.0.255) `&]_[*@3 var], 
[@(0.0.255) const]_[_^Value^ Value][@(0.0.255) `&]_[*@3 x])&]
[s2;%RU-RU Получает переменную из совместимого 
с JSON значения. При ошибке может выводить 
JsonizeError.&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:StoreAsJson`(const T`&`,bool`): [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T]>_[_^String^ S
tring]_[* StoreAsJson]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&]_[*@3 var], 
[@(0.0.255) bool]_[*@3 pretty]_`=_[@(0.0.255) false])&]
[s2;%RU-RU Преобразует [%-*@3 var] в текст JSON. Если 
[%-*@3 pretty] равно true, добавляет пробелы 
для лучшего вида, в противном случае 
результат максимально компактен.&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:LoadFromJson`(T`&`,const char`*`): [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T]>_
[@(0.0.255) bool]_[* LoadFromJson]([*@4 T][@(0.0.255) `&]_[*@3 var], [@(0.0.255) const]_[@(0.0.255) c
har]_`*[*@3 json])&]
[s2;%RU-RU Получает [%-*@3 var] из текста [%-*@3 json]. 
Не выводит JsonizeError, возвращает false 
в случае ошибки.&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:StoreAsJsonFile`(const T`&`,const char`*`,bool`): [@(0.0.255) template]_<[@(0.0.255) c
lass]_[*@4 T]>_[@(0.0.255) bool]_[* StoreAsJsonFile]([@(0.0.255) const]_[*@4 T][@(0.0.255) `&
]_[*@3 var], [@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 file]_`=_NULL, 
[@(0.0.255) bool]_[*@3 pretty]_`=_[@(0.0.255) false])&]
[s2;%RU-RU Преобразует [%-*@3 var] в файл JSON.Если 
[%-*@3 pretty] равно true, добавляет пробелы 
для лучшего вида, в противном случае 
результат максимально компактен. 
Если [%-*@3 file] равно NULL, ConfigFile(GetExeTitle() 
`+ `".json`") используется как файловый 
путь.&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:LoadFromJsonFile`(T`&`,const char`*`): [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T
]>_[@(0.0.255) bool]_[* LoadFromJsonFile]([*@4 T][@(0.0.255) `&]_[*@3 var], 
[@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 file]_`=_NULL)&]
[s2;%RU-RU Получает [%-*@3 var] из файла [%-*@3 json]. 
Не выводит JsonizeError, возвращает false 
в случае ошибки. Если [%-*@3 file] равно 
NULL, ConfigFile(GetExeTitle() `+ `".json`") используется 
как файловый путь.&]
[s3; &]
[s0;i448;a25;kKO9;@(0.0.255) &]
[ {{10000@(113.42.0) [s0;%RU-RU [*@7;4 Стандартные определения шаблона 
Jsonize]]}}&]
[s3; &]
[s5;:Jsonize`(JsonIO`&`,int`&`): [@(0.0.255) template]_<>_[@(0.0.255) void]_[* Jsonize]([_^JsonIO^ J
sonIO][@(0.0.255) `&]_[*@3 io], [@(0.0.255) int`&]_[*@3 var])&]
[s5;:Upp`:`:Jsonize`(Upp`:`:JsonIO`&`,Upp`:`:byte`&`): [@(0.0.255) template]_<>_[@(0.0.255) v
oid]_[* Jsonize]([_^Upp`:`:JsonIO^ JsonIO][@(0.0.255) `&]_[*@3 io], [_^Upp`:`:byte^ byte][@(0.0.255) `&
]_[*@3 var])&]
[s5;:Jsonize`(JsonIO`&`,int16`&`): [@(0.0.255) template]_<>_[@(0.0.255) void]_[* Jsonize]([_^JsonIO^ J
sonIO][@(0.0.255) `&]_[*@3 io], [_^int16^ int16][@(0.0.255) `&]_[*@3 var])&]
[s5;:Jsonize`(JsonIO`&`,int64`&`): [@(0.0.255) template]_<>_[@(0.0.255) void]_[* Jsonize]([_^JsonIO^ J
sonIO][@(0.0.255) `&]_[*@3 io], [_^int64^ int64][@(0.0.255) `&]_[*@3 var])&]
[s5;:Jsonize`(JsonIO`&`,double`&`): [@(0.0.255) template]_<>_[@(0.0.255) void]_[* Jsonize](
[_^JsonIO^ JsonIO][@(0.0.255) `&]_[*@3 io], [@(0.0.255) double`&]_[*@3 var])&]
[s5;:Jsonize`(JsonIO`&`,bool`&`): [@(0.0.255) template]_<>_[@(0.0.255) void]_[* Jsonize]([_^JsonIO^ J
sonIO][@(0.0.255) `&]_[*@3 io], [@(0.0.255) bool`&]_[*@3 var])&]
[s5;:Jsonize`(JsonIO`&`,String`&`): [@(0.0.255) template]_<>_[@(0.0.255) void]_[* Jsonize](
[_^JsonIO^ JsonIO][@(0.0.255) `&]_[*@3 io], [_^String^ String][@(0.0.255) `&]_[*@3 var])&]
[s5;:Jsonize`(JsonIO`&`,WString`&`): [@(0.0.255) template]_<>_[@(0.0.255) void]_[* Jsonize](
[_^JsonIO^ JsonIO][@(0.0.255) `&]_[*@3 io], [_^WString^ WString][@(0.0.255) `&]_[*@3 var])&]
[s5;:Jsonize`(JsonIO`&`,Date`&`): [@(0.0.255) template]_<>_[@(0.0.255) void]_[* Jsonize]([_^JsonIO^ J
sonIO][@(0.0.255) `&]_[*@3 io], [_^Date^ Date][@(0.0.255) `&]_[*@3 var])&]
[s5;:Jsonize`(JsonIO`&`,Time`&`): [@(0.0.255) template]_<>_[@(0.0.255) void]_[* Jsonize]([_^JsonIO^ J
sonIO][@(0.0.255) `&]_[*@3 io], [_^Time^ Time][@(0.0.255) `&]_[*@3 var])&]
[s2; Предлагают определение шаблона Jsonize 
для поддержки типа данных [*@3 var].&]
[s3; &]
[s0; &]
[ {{10000@(113.42.0) [s0;%RU-RU [*@7;4 Особые варианты Jsonize]]}}&]
[s3; &]
[s5;:StringMap`(JsonIO`&`,VectorMap`<K`,V`>`&`): [@(0.0.255) template]_<[@(0.0.255) class
]_[*@4 K], [@(0.0.255) class]_[*@4 V]>_[@(0.0.255) void]_[* StringMap]([_^JsonIO^ JsonIO][@(0.0.255) `&
]_[*@3 io], [_^VectorMap^ VectorMap]<[*@4 K], [*@4 V], [*@4 H]>`&_[*@3 map])&]
[s5;:StringMap`(JsonIO`&`,ArrayMap`<K`,V`>`&`): [@(0.0.255) template]_<[@(0.0.255) class]_
[*@4 K], [@(0.0.255) class]_[*@4 V]>_[@(0.0.255) void]_[* StringMap]([_^JsonIO^ JsonIO][@(0.0.255) `&
]_[*@3 io], [_^ArrayMap^ ArrayMap]<[*@4 K], [*@4 V], [*@4 H]>`&_[*@3 map])&]
[s2;%RU-RU Дефолтный Jsonize для Map`'ов должен 
кодировать любой тип как ключ, что 
приводит к некоторым неестественным 
результатам, когда ключ [%-*@4 K] является 
String или WString. Альтернативный вариант 
[%-* StringMap ]кодирует мап как объект JSON 
с ключами, равноценными ключам мапа.&]
[s3;%RU-RU &]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:JsonizeBySerialize`(JsonIO`&`,T`&`): [@(0.0.255) template]_<[@(0.0.255) class]_[*@4 T]>
_[@(0.0.255) void]_[* JsonizeBySerialize]([_^JsonIO^ JsonIO][@(0.0.255) `&]_[*@3 jio], 
[*@4 T][@(0.0.255) `&]_[*@3 x])&]
[s2;%RU-RU Кодирует/получает Json, бинарно 
сериализуя объект и джейсонируя строку 
16`-ричных данных.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:LambdaIze`(IO`&`,const char`*`,IZE`): [@(0.0.255) template]_<[@(0.0.255) clas
s]_[*@4 IO], [@(0.0.255) class]_[*@4 IZE]>_[@(0.0.255) void]_[* LambdaIze]([*@4 IO][@(0.0.255) `&
]_[*@3 io], [@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 id], [*@4 IZE]_[*@3 ize])&]
[s2;%RU-RU Использует [%-*@3 ize] для представления 
значения узла Jsonize или Xmlize, с именем 
[%-*@3 id]. У лямбды должен быть один параметр 
`- IO`&.&]
[s3;%RU-RU &]
[s3;%RU-RU ]]