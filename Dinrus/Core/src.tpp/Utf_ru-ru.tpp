topic "Поддержка Unicode UTF[8,16,32]";
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
[ {{10000@(113.42.0) [s0;%RU-RU [*@7;4 Поддержка Unicode UTF`[8,16,32`]]]}}&]
[s0;%RU-RU &]
[s5;:IsUtf8Lead`(int`): [@(0.0.255) bool]_[* IsUtf8Lead]([@(0.0.255) int]_[*@3 c])&]
[s2;%RU-RU Проверяет, является ли [%-*@3 c ]вводным 
байтом UTF`-8.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:FetchUtf8`(const char`*`&`,const char`*`,bool`&`): [_^Upp`:`:dword^ dword]_
[* FetchUtf8]([@(0.0.255) const]_[@(0.0.255) char]_`*`&[*@3 s], [@(0.0.255) const]_[@(0.0.255) c
har]_`*[*@3 lim], [@(0.0.255) bool`&]_[*@3 ok])&]
[s5;:Upp`:`:FetchUtf8`(const char`*`&`,const char`*`): [_^Upp`:`:dword^ dword]_[* FetchUt
f8]([@(0.0.255) const]_[@(0.0.255) char]_`*`&[*@3 s], [@(0.0.255) const]_[@(0.0.255) char]_
`*[*@3 lim])&]
[s2;%RU-RU Считывает единичную кодточку 
UTF`-32 из строки UTF`-8 [%-*@3 s] с окончанием 
на [%-*@3 lim]. [%-*@3 s] должна быть меньше [%-*@3 lim]. 
[%-*@3 s] продвигается соответственно. 
[%-*@3 ok] установлен в false, если UTF`-8 неверен 
`- в таких случаях возвращается error`-escape 
единичного байта (но он НЕ устанавливается 
в true, если считывается полноценный 
символ UTF`-8).&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:CheckUtf8`(const char`*`,int`): [@(0.0.255) bool]_[* CheckUtf8]([@(0.0.255) con
st]_[@(0.0.255) char]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:CheckUtf8`(const char`*`): [@(0.0.255) bool]_[* CheckUtf8]([@(0.0.255) const]_[@(0.0.255) c
har]_`*[*@3 s])&]
[s5;:CheckUtf8`(const String`&`): [@(0.0.255) bool]_[* CheckUtf8]([@(0.0.255) const]_[_^String^ S
tring][@(0.0.255) `&]_[*@3 src])&]
[s2;%RU-RU Проверяет, содержит ли строка 
полноценную цепочку UTF`-8. Если источник 
указан как указатель [%-*@3 s] без [%-*@3 len], 
то он должен завершаться нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:Utf8Len`(const Upp`:`:dword`*`,int`): [@(0.0.255) int]_[* Utf8Len]([@(0.0.255) c
onst]_[_^Upp`:`:dword^ dword]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:Utf8Len`(const Upp`:`:dword`*`): [@(0.0.255) int]_[* Utf8Len]([@(0.0.255) const
]_[_^Upp`:`:dword^ dword]_`*[*@3 s])&]
[s5;:Upp`:`:Utf8Len`(const Upp`:`:Vector`<Upp`:`:dword`>`&`): [@(0.0.255) int]_[* Utf8Len
]([@(0.0.255) const]_[_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ dword]>`&_[*@3 s])&]
[s2;%RU-RU Возвращает размер в байтах текста 
UTF`-32 Unicode в кодировке UTF`-8. Если источник 
указан как указатель [%-*@3 s] без [%-*@3 len], 
то он должен завершаться нулём.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Utf8Len`(const Upp`:`:wchar`*`,int`): [@(0.0.255) int]_[* Utf8Len]([@(0.0.255) c
onst]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:Utf8Len`(const Upp`:`:wchar`*`): [@(0.0.255) int]_[* Utf8Len]([@(0.0.255) const
]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s])&]
[s5;:Upp`:`:Utf8Len`(const Upp`:`:WString`&`): [@(0.0.255) int]_[* Utf8Len]([@(0.0.255) con
st]_[_^Upp`:`:WString^ WString][@(0.0.255) `&]_[*@3 s])&]
[s2;%RU-RU Возвращает размер в байтах текста 
UTF`-16 Unicode в кодировке UTF`-8. Если источник 
указан как указатель [%-*@3 s] без [%-*@3 len], 
то он должен завершаться нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:Utf8Len`(Upp`:`:dword`): [@(0.0.255) int]_[* Utf8Len]([_^Upp`:`:dword^ dword]_[*@3 c
ode])&]
[s2;%RU-RU Возвращает размер в байтах единичного 
кодпойнта в UTF`-8.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:ToUtf8`(char`*`,const Upp`:`:wchar`*`,int`): [@(0.0.255) void]_[* ToUtf8]([@(0.0.255) c
har]_`*[*@3 t], [@(0.0.255) const]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s], 
[@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf8`(const Upp`:`:wchar`*`,int`): [_^Upp`:`:String^ String]_[* ToUtf8]([@(0.0.255) c
onst]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf8`(const Upp`:`:wchar`*`): [_^Upp`:`:String^ String]_[* ToUtf8]([@(0.0.255) c
onst]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s])&]
[s5;:Upp`:`:ToUtf8`(const Upp`:`:WString`&`): [_^Upp`:`:String^ String]_[* ToUtf8]([@(0.0.255) c
onst]_[_^Upp`:`:WString^ WString][@(0.0.255) `&]_[*@3 s])&]
[s2;%RU-RU Преобразование UTF`-16 в UTF`-8. Если 
цель (приёмник) указан как указатель 
на буфер [%-*@3 t], этот буфер должен содержать 
достаточно места для вывода. Если 
источник указан как указатель [%-*@3 s] 
без [%-*@3 len], то он должен завершаться 
нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:ToUtf8`(char`*`,const Upp`:`:dword`*`,int`): [@(0.0.255) void]_[* ToUtf8]([@(0.0.255) c
har]_`*[*@3 t], [@(0.0.255) const]_[_^Upp`:`:dword^ dword]_`*[*@3 s], 
[@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf8`(const Upp`:`:dword`*`,int`): [_^Upp`:`:String^ String]_[* ToUtf8]([@(0.0.255) c
onst]_[_^Upp`:`:dword^ dword]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf8`(const Upp`:`:dword`*`): [_^Upp`:`:String^ String]_[* ToUtf8]([@(0.0.255) c
onst]_[_^Upp`:`:dword^ dword]_`*[*@3 s])&]
[s5;:Upp`:`:ToUtf8`(const Upp`:`:Vector`<Upp`:`:dword`>`&`): [_^Upp`:`:String^ String]_
[* ToUtf8]([@(0.0.255) const]_[_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ dword]>`&_[*@3 s])
&]
[s2;%RU-RU Преобразование UTF`-32 в UTF`-8. Если 
цель (приёмник) указан как указатель 
на буфер [%-*@3 t], этот буфер должен содержать 
достаточно места для вывода. Если 
источник указан как указатель [%-*@3 s] 
без [%-*@3 len], то он должен завершаться 
нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:ToUtf8`(Upp`:`:dword`): [_^Upp`:`:String^ String]_[* ToUtf8]([_^Upp`:`:dword^ d
word]_[*@3 code])&]
[s2;%RU-RU Преобразует единичный кодпойнт 
в UTF`-8.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Utf16Len`(const Upp`:`:dword`*`,int`): [@(0.0.255) int]_[* Utf16Len]([@(0.0.255) c
onst]_[_^Upp`:`:dword^ dword]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:Utf16Len`(const Upp`:`:dword`*`): [@(0.0.255) int]_[* Utf16Len]([@(0.0.255) con
st]_[_^Upp`:`:dword^ dword]_`*[*@3 s])&]
[s5;:Upp`:`:Utf16Len`(const Upp`:`:Vector`<Upp`:`:dword`>`&`): [@(0.0.255) int]_[* Utf16L
en]([@(0.0.255) const]_[_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ dword]>`&_[*@3 s])&]
[s2;%RU-RU Возвращает размер в широких символов 
текста UTF`-32 Unicode в UTF`-16. Если источник 
указан как указатель [%-*@3 s] без [%-*@3 len], 
то он должен завершаться нулём.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Utf16Len`(Upp`:`:dword`): [@(0.0.255) int]_[* Utf16Len]([_^Upp`:`:dword^ dword]_
[*@3 code])&]
[s2;%RU-RU Возвращает размер в широких символах 
единичной кодточки в UTF`-16.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:Utf16Len`(const char`*`,int`): [@(0.0.255) int]_[* Utf16Len]([@(0.0.255) const]_
[@(0.0.255) char]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:Utf16Len`(const char`*`): [@(0.0.255) int]_[* Utf16Len]([@(0.0.255) const]_[@(0.0.255) c
har]_`*[*@3 s])&]
[s5;:Upp`:`:Utf16Len`(const Upp`:`:String`&`): [@(0.0.255) int]_[* Utf16Len]([@(0.0.255) co
nst]_[_^Upp`:`:String^ String][@(0.0.255) `&]_[*@3 s])&]
[s2;%RU-RU Возвращает размер в широких символах 
текста UTF`-8 Unicode в UTF`-16. Если источник 
указан как указатель [%-*@3 s] без [%-*@3 len], 
то он должен завершаться нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:ToUtf16`(Upp`:`:wchar`*`,const Upp`:`:dword`*`,int`): [@(0.0.255) void]_[* To
Utf16]([_^Upp`:`:wchar^ wchar]_`*[*@3 t], [@(0.0.255) const]_[_^Upp`:`:dword^ dword]_`*[*@3 s
], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf16`(const Upp`:`:dword`*`,int`): [_^Upp`:`:WString^ WString]_[* ToUtf16](
[@(0.0.255) const]_[_^Upp`:`:dword^ dword]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf16`(const Upp`:`:dword`*`): [_^Upp`:`:WString^ WString]_[* ToUtf16]([@(0.0.255) c
onst]_[_^Upp`:`:dword^ dword]_`*[*@3 s])&]
[s5;:Upp`:`:ToUtf16`(const Upp`:`:Vector`<Upp`:`:dword`>`&`): [_^Upp`:`:WString^ WStrin
g]_[* ToUtf16]([@(0.0.255) const]_[_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ dword]>`&_[*@3 s
])&]
[s2;%RU-RU [%- Преобразование UTF`-32 в UTF`-16.] Если 
цель задана как указатель на буфер 
[%-*@3 t], то этот буфер должен содержать 
достаточно пространства для вывода. 
Если источник указан как указатель 
[%-*@3 s] без [%-*@3 len], то он должен завершаться 
нулём.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ToUtf16`(Upp`:`:dword`): [_^Upp`:`:WString^ WString]_[* ToUtf16]([_^Upp`:`:dword^ d
word]_[*@3 code])&]
[s2;%RU-RU Преобразует единичную кодточку 
в UTF`-16.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:ToUtf16`(Upp`:`:wchar`*`,const char`*`,int`): [@(0.0.255) void]_[* ToUtf16]([_^Upp`:`:wchar^ w
char]_`*[*@3 t], [@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf16`(const char`*`,int`): [_^Upp`:`:WString^ WString]_[* ToUtf16]([@(0.0.255) c
onst]_[@(0.0.255) char]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf16`(const char`*`): [_^Upp`:`:WString^ WString]_[* ToUtf16]([@(0.0.255) co
nst]_[@(0.0.255) char]_`*[*@3 s])&]
[s5;:Upp`:`:ToUtf16`(const Upp`:`:String`&`): [_^Upp`:`:WString^ WString]_[* ToUtf16]([@(0.0.255) c
onst]_[_^Upp`:`:String^ String][@(0.0.255) `&]_[*@3 s])&]
[s2;%RU-RU Преобразование UTF`-8 в UTF`-16. Если 
цель задана как указатель на буфер 
[%-*@3 t], то этот буфер должен содержать 
достаточно пространства для вывода. 
Если источник указан как указатель 
[%-*@3 s] без [%-*@3 len], то он должен завершаться 
нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:Utf32Len`(const Upp`:`:wchar`*`,int`): [@(0.0.255) int]_[* Utf32Len]([@(0.0.255) c
onst]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:Utf32Len`(const Upp`:`:wchar`*`): [@(0.0.255) int]_[* Utf32Len]([@(0.0.255) con
st]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s])&]
[s5;:Upp`:`:Utf32Len`(const Upp`:`:WString`&`): [@(0.0.255) int]_[* Utf32Len]([@(0.0.255) c
onst]_[_^Upp`:`:WString^ WString][@(0.0.255) `&]_[*@3 s])&]
[s2;%RU-RU Возвращает размер в dword`'ах текста 
UTF`-16 Unicode в UTF`-32. Заметьте, что это то 
же, что и число кодточек Unicode  в этом 
тексте. Если источник указан как указатель 
[%-*@3 s] без [%-*@3 len], то он должен завершаться 
нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:Utf32Len`(const char`*`,int`): [@(0.0.255) int]_[* Utf32Len]([@(0.0.255) const]_
[@(0.0.255) char]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:Utf32Len`(const char`*`): [@(0.0.255) int]_[* Utf32Len]([@(0.0.255) const]_[@(0.0.255) c
har]_`*[*@3 s])&]
[s5;:Upp`:`:Utf32Len`(const Upp`:`:String`&`): [@(0.0.255) int]_[* Utf32Len]([@(0.0.255) co
nst]_[_^Upp`:`:String^ String][@(0.0.255) `&]_[*@3 s])&]
[s2;%RU-RU Возвращает размер в dword`'ах текста 
UTF`-8 Unicode в UTF`-32. Заметьте, что это то 
же, что и число кодточек Unicode  в этом 
тексте. Если источник указан как указатель 
[%-*@3 s] без [%-*@3 len], то он должен завершаться 
нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:ReadSurrogatePair`(const Upp`:`:wchar`*`,const Upp`:`:wchar`*`): [_^Upp`:`:dword^ d
word]_[* ReadSurrogatePair]([@(0.0.255) const]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s], 
[@(0.0.255) const]_[_^Upp`:`:wchar^ wchar]_`*[*@3 lim])&]
[s2;%RU-RU Считывает единичный кодпойнт 
utf32 из [%-*@3 s], [%-*@3 lim]. Возвращает 0, если 
в [%-*@3 s ]нет суррогатной пары.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:ToUtf32`(Upp`:`:dword`*`,const Upp`:`:wchar`*`,int`): [@(0.0.255) void]_[* To
Utf32]([_^Upp`:`:dword^ dword]_`*[*@3 t], [@(0.0.255) const]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s
], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf32`(const Upp`:`:wchar`*`,int`): [_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ d
word]>_[* ToUtf32]([@(0.0.255) const]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s], 
[@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf32`(const Upp`:`:wchar`*`): [_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ d
word]>_[* ToUtf32]([@(0.0.255) const]_[_^Upp`:`:wchar^ wchar]_`*[*@3 s])&]
[s5;:Upp`:`:ToUtf32`(const Upp`:`:WString`&`): [_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ d
word]>_[* ToUtf32]([@(0.0.255) const]_[_^Upp`:`:WString^ WString][@(0.0.255) `&]_[*@3 s])&]
[s2;%RU-RU Преобразование UTF`-16 в UTF`-32. Если 
цель задана как указатель на буфер 
[%-*@3 t], то этот буфер должен содержать 
достаточно пространства для вывода. 
Если источник указан как указатель 
[%-*@3 s] без [%-*@3 len], то он должен завершаться 
нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:ToUtf32`(Upp`:`:dword`*`,const char`*`,int`): [@(0.0.255) void]_[* ToUtf32]([_^Upp`:`:dword^ d
word]_`*[*@3 t], [@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf32`(const char`*`,int`): [_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ dwor
d]>_[* ToUtf32]([@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 s], [@(0.0.255) int]_[*@3 len])&]
[s5;:Upp`:`:ToUtf32`(const char`*`): [_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ dword]>_[* T
oUtf32]([@(0.0.255) const]_[@(0.0.255) char]_`*[*@3 s])&]
[s5;:Upp`:`:ToUtf32`(const Upp`:`:String`&`): [_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ d
word]>_[* ToUtf32]([@(0.0.255) const]_[_^Upp`:`:String^ String][@(0.0.255) `&]_[*@3 s])&]
[s2;%RU-RU Преобразование UTF`-8 в UTF`-32. Если 
цель задана как указатель на буфер 
[%-*@3 t], то этот буфер должен содержать 
достаточно пространства для вывода. 
Если источник указан как указатель 
[%-*@3 s] без [%-*@3 len], то он должен завершаться 
нулём.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:UnicodeDecompose`(Upp`:`:dword`,Upp`:`:dword`[MAX`_DECOMPOSED`]`,bool`): [@(0.0.255) i
nt]_[* UnicodeDecompose]([_^Upp`:`:dword^ dword]_[*@3 codepoint], [_^Upp`:`:dword^ dword]_
[*@3 t][@(0.0.255) `[]MAX`_DECOMPOSED[@(0.0.255) `]], [@(0.0.255) bool]_[*@3 only`_canonica
l])&]
[s5;:Upp`:`:UnicodeDecompose`(Upp`:`:dword`,bool`): [_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ d
word]>_[* UnicodeDecompose]([_^Upp`:`:dword^ dword]_[*@3 codepoint], 
[@(0.0.255) bool]_[*@3 only`_canonical])&]
[s2;%RU-RU Возвращает UNICODE`-декомпозицию 
заданной кодточки [%-*@3 codepoint] на базовые 
и комбинирующие символы. Если [%-*@3 only`_canonical] 
равно true, то допускается только каноническая
 декомпозиция.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Upp`:`:UnicodeCompose`(const Upp`:`:dword`*`,int`): [_^Upp`:`:dword^ dword]_[* Unico
deCompose]([@(0.0.255) const]_[_^Upp`:`:dword^ dword]_`*[*@3 t], [@(0.0.255) int]_[*@3 coun
t])&]
[s5;:Upp`:`:UnicodeCompose`(const Upp`:`:Vector`<Upp`:`:dword`>`&`): [_^Upp`:`:dword^ d
word]_[* UnicodeCompose]([@(0.0.255) const]_[_^Upp`:`:Vector^ Vector]<[_^Upp`:`:dword^ dw
ord]>`&_[*@3 t])&]
[s2;%RU-RU Потается скомпонировать мультикодточеч
ную графему в одну кодточку, если 
она существует. Если такой кодточки 
нет, возвращает 0.&]
[s3;%RU-RU &]
[s0;%RU-RU ]]