topic "Класс LazyUpdate";
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
[{_} 
[ {{10000@(113.42.0) [s0;%RU-RU [*@7;4 Класс LazyUpdate]]}}&]
[s3; &]
[s1;:LazyUpdate`:`:class: [@(0.0.255)3 class][3 _][*3 LazyUpdate]&]
[s9;%RU-RU Класс LazyUpdate предоставляет потокобезопас
ные (thread`-safe) средства для `"ленивой`" 
оценки (lazy evaluation). Как правило, он используетс
я совместно с кэшем переменных изменяемого 
типа. Операции изменения над этим 
классом устанавливают LazyUpdate в состояние 
`"invalid`". Когда вызывается какой`-либо 
метод для получения оцененного значения, 
вначале LazyUpdate проверяет, полноценен 
ли кэш, используя BeginUpdate.(такой метод 
обычно константен (const), поэтому кэш 
должен быть mutable (изменяемым)). Если 
BeginUpdate возвращает true, то кэш должен 
быть обновлён. В конце обновления 
нужно вызвать EndUpdate.&]
[s9;%RU-RU BeginUpdate также блокирует любой другой 
поток (нить), когда обновление в процессе.&]
[s9;%RU-RU Таким образом, несколько потоков 
могут вызывать одновременно методы 
чтения, в то время как выполняются 
обновления и идёт корректная сериализация. 
Конечно, код`-клиент такого класса 
всё ещё должен сериализовать доступ 
к экземпляру, точно так же, как к любому 
другому объекту. Метод Invalidate может, 
например, вызываться только при операции 
сериализованной записи. Таким образом, 
LazyUpdate решает проблему доступности 
множества конкурентных читателей 
(считывателей) (но только с одним записывател
ем), а `"ленивое`" обновление изменчивого 
кэша выполняется при операции чтения.&]
[s9;%RU-RU Когда кэш в обновлённом состоянии, 
BeginUpdate is wait`-free.&]
[s3; &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0;%RU-RU [* Список Публичных Методов]]}}&]
[s3; &]
[s5;:LazyUpdate`:`:Invalidate`(`): [@(0.0.255) void]_[* Invalidate]()&]
[s2;%RU-RU Устанавливает LazyUpdate в неполноценное 
состояние.&]
[s3; &]
[s4; &]
[s5;:LazyUpdate`:`:BeginUpdate`(`)const: [@(0.0.255) bool]_[* BeginUpdate]()_[@(0.0.255) co
nst]&]
[s2;%RU-RU Запрашивает, находится ли LazyUpdate 
в инвалидном состоянии. В таком случае 
возвращается true, а любой иной поток, 
вызывающий BeginUpdate, блокируется до 
EndUpdate (и такой блокированный поток 
затем возвращает false, так как кэш уже 
обновлён). Wait`-free, если кэш обновлён.&]
[s3; &]
[s4; &]
[s5;:LazyUpdate`:`:EndUpdate`(`)const: [@(0.0.255) void]_[* EndUpdate]()_[@(0.0.255) const]&]
[s2;%RU-RU Сигнализирует о том, что кэш был 
обновлён.&]
[s3; &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0;%RU-RU [* Детали Конструктора]]}}&]
[s3; &]
[s5;:LazyUpdate`:`:LazyUpdate`(`): [* LazyUpdate]()&]
[s2;%RU-RU Устанавливает LazyUpdate в состояние 
обновления.&]
[s3; &]
[s0; ]]