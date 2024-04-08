topic "Класс Pusher";
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
[ {{10000@3 [s0;%RU-RU [*@(229)4 Класс Pusher]]}}&]
[s3; &]
[s1;:Pusher`:`:class: [@(0.0.255)3 class][3 _][*3 Pusher][3 _:_][@(0.0.255)3 public][3 _][*@3;3 Ctrl
]&]
[s9;%RU-RU Pusher `- это базовый класс для классов, 
выполняющих простое действие при 
клике мышью или использовании клавиатурного
 интерфейса (обычно, клавиши доступа). 
Pusher обрабатывает весь ввод от мыши, 
проверяет, совпадают ли клавиатурные 
события с клавишами доступа Pusher`'а, 
однако и клавиатурный ввод может 
обрабатываться производными классами 
и проектироваться в состояние Pusher`'а 
его методами (KeyPush, FinishPush). Клавиша`-акселерат
ор для Pusher назначается через ярлык 
(используя `'`&`' или `'`\b`', смотрите [^topic`:`/`/CtrlLib`/src`/LabelBase`_ru`-ru^ L
abelBase]).&]
[s9;%RU-RU Когда Pusher нажат и мышь отпущена, 
при этом оставаясь над ним, вызывается 
виртуальный метод PerformAction, а его стандартная
 реализация, в свою очередь, вызывает 
обратный вызов WhenAction. Помимо него 
доступны обратные вызовы WhenPush и WhenRepeat.&]
[s0;%RU-RU &]
[s5;K%RU-RU [/ Производный от] [^topic`:`/`/CtrlCore`/src`/Ctrl`_ru`-ru^ Ctr
l]&]
[s3;%RU-RU &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0;%RU-RU [* Детали Конструктора]]}}&]
[s3; &]
[s5;:Pusher`:`:Pusher`(`): [* Pusher]()&]
[s2;%RU-RU Инициализует Pusher. Шрифт устанавливается
 в стандартный шрифт ГИП, клавиша 
акселератора очищается.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:`~Pusher`(`): [@(0.0.255) `~][* Pusher]()&]
[s2;%RU-RU Дефолтный деструктор .&]
[s3;%RU-RU &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0;%RU-RU [* Список Защищённых Полей]]}}&]
[s3; &]
[s5;:Pusher`:`:accesskey: [_^byte^ byte]_[* accesskey]&]
[s2;%RU-RU Клавиша доступа, назначенная 
Pusher`'у.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:label: [_^String^ String]_[* label]&]
[s2;%RU-RU Текст, присвоенный Pusher`'у.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:font: [_^Font^ Font]_[* font]&]
[s2;%RU-RU Шрифт, назначенный Pusher`'у.&]
[s3;%RU-RU &]
[s0;%RU-RU &]
[ {{10000F(128)G(128)@1 [s0;%RU-RU [* Список Защищённых Членов]]}}&]
[s3; &]
[s5;:Pusher`:`:KeyPush`(`): [@(0.0.255) void]_[* KeyPush]()&]
[s2;%RU-RU Этот метод приводит Pusher в состояние 
`"нажат клавиатурно`". Производный 
класс должен использовать его в ответ 
на клавишу`-акселератор.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:IsPush`(`)const: [@(0.0.255) bool]_[* IsPush]()_[@(0.0.255) const]&]
[s7;%RU-RU [*/ Возвратное значение]-|true, когда 
в нажатом состоянии.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:IsKeyPush`(`): [@(0.0.255) bool]_[* IsKeyPush]()&]
[s7;%RU-RU [*/ Возвратное значение]-|true, когда 
в нажатом состоянии, инициированном 
с клавиатуры.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:FinishPush`(`): [@(0.0.255) bool]_[* FinishPush]()&]
[s2;%RU-RU Вызывает завершение нажатого 
состояния и метод PerformAction (дефолтная 
реализация вызывает стандартный 
обрвызов WhenAction).&]
[s3;%RU-RU &]
[s4;%RU-RU &]
[s5;:Pusher`:`:RefreshPush`(`): [@(0.0.255) virtual] [@(0.0.255) void]_[* RefreshPush]()&]
[s2;%RU-RU Этим методом освежается конкретная 
часть производного от Pusher класса, 
которая графически изменяется в ответ 
на состояние push. Дефолтная реализация 
вызывает Refresh, однако некоторые классы 
(типа Option)  перегружают его, чтобы 
уменьшить размер области перерисовки.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:RefreshFocus`(`): [@(0.0.255) virtual] [@(0.0.255) void]_[* RefreshFocus]()&]
[s2;%RU-RU Этим методом освежается конкретная 
часть производного от Pusher класса, 
которая графически изменяется в ответ 
на изменение фокуса ввода. Дефолтная 
реализация вызывает Refresh, однако некоторые 
классы (типа Option)  перегружают его, 
чтобы уменьшить размер области перерисовки.
&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:PerformAction`(`): [@(0.0.255) virtual] [@(0.0.255) void]_[* PerformAction](
)&]
[s2;%RU-RU Этот метод вызывается, когда `"push`" 
закончен. Дефолтная реализация вызывает 
WhenAction.&]
[s3;%RU-RU &]
[s0; &]
[ {{10000F(128)G(128)@1 [s0;%RU-RU [* Список Публичных Членов]]}}&]
[s3; &]
[s5;:Pusher`:`:SetFont`(Font`): [_^Pusher^ Pusher][@(0.0.255) `&]_[* SetFont]([_^Font^ Font]_
[*@3 fnt])&]
[s2;%RU-RU Устанавливает шрифт, который 
следует использовать при отрисовке 
конкретного производного класса 
Pusher. Сам Pusher совершенно не использует 
этот шрифт, а делают это только производные 
от него классы.&]
[s7;%RU-RU [*C@3 fnt]-|Font.&]
[s7;%RU-RU [*/ Возвратное значение]-|`*this для 
сцепки методов.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:SetLabel`(const char`*`): [_^Pusher^ Pusher][@(0.0.255) `&]_[* SetLabel]([@(0.0.255) c
onst]_[@(0.0.255) char]_`*[*@3 text])&]
[s2;%RU-RU Устанавливает текст пушера. Текст 
может содержать символы `'`&`' или `'`\b`', 
чтобы определять клавиши доступа.&]
[s7;%RU-RU [*C@3 text]-|новый текст&]
[s7;%RU-RU [*/ Возвратное значение]-|`*this для 
сцепки методов.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:PseudoPush`(`): [@(0.0.255) void]_[* PseudoPush]()&]
[s2;%RU-RU Вызывает `"push emulation`". Pusher анимируется 
в разных состояниях, как`-будто нажат 
пользователем, включая вызовы PerformAction 
(WhenAction), WhenPush и WhenRepeat.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:WhenPush: [_^Callback^ Callback]_[* WhenPush]&]
[s2;%RU-RU Этот обрвызов вызывается, когда 
pusher переходит из `"ненажатого`" в `"нажатое`" 
состояние.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:WhenRepeat: [_^Callback^ Callback]_[* WhenRepeat]&]
[s2;%RU-RU `"Автоповтор Pusher`'а`" `- этот обрвыз 
вызывается периодически, когда пользователь
 наводит мышь над Pusher`'ом и держит 
кнопку `- он эмулирует автоповтор 
клавиатуры (напр., кнопки`-стрелки 
в полосе промотки).&]
[s3; &]
[s4; &]
[s5;:Pusher`:`:ClickFocus`(bool`): [@(0.0.255) virtual] [_^Pusher^ Pusher][@(0.0.255) `&]_[* C
lickFocus]([@(0.0.255) bool]_[*@3 cf]_`=_[@(0.0.255) true])&]
[s2;%RU-RU Если [%-*@3 cf] равно true, Pusher сфокусируется 
при нажатии мыши.&]
[s7;%RU-RU [*@3 cf]-|фокусировка при клике&]
[s7;%RU-RU [*/ Возвратное значение]-|`*this для 
сцепки методов.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:NoClickFocus`(`): [_^Pusher^ Pusher][@(0.0.255) `&]_[* NoClickFocus]()&]
[s2;%RU-RU То же, что и [^topic`:`/`/CtrlLib`/src`/Pusher`_ru`-ru`#Pusher`:`:ClickFocus`(bool`)^ C
lickFocus](false).&]
[s3; &]
[s4; &]
[s5;:Pusher`:`:IsClickFocus`(`)const: [@(0.0.255) bool]_[* IsClickFocus]()_[@(0.0.255) cons
t]&]
[s2;%RU-RU Выводит значение ClickFocus.&]
[s3; &]
[s4; &]
[s5;:Pusher`:`:GetFont`(`)const: [_^Font^ Font]_[* GetFont]()_[@(0.0.255) const]&]
[s2;%RU-RU Возвращает шрифт, который следует 
использовать при отрисовке конкретного, 
производного от Pusher, класса. Сам Pusher 
совершенно не использует этот шрифт, 
но большинство производных классов 
это делают.&]
[s3; &]
[s4; &]
[s5;:Pusher`:`:GetLabel`(`)const: [_^String^ String]_[* GetLabel]()_[@(0.0.255) const]&]
[s2;%RU-RU Возвращает текст пушера. Текст 
может содержать символы `'`&`' или `'`\b`', 
чтобы определять клавиши доступа.&]
[s3;%RU-RU &]
[s4; &]
[s5;:Pusher`:`:GetVisualState`(`)const: [@(0.0.255) int]_[* GetVisualState]()_[@(0.0.255) c
onst]&]
[s2;%RU-RU Возвращает текущее зримое состояние 
текущего пушера:&]
[s7;%RU-RU CTRL`_NORMAL-|пушер активирован и не 
взаимодействует с мышью&]
[s7;%RU-RU CTRL`_HOT-|пушер активирован и находится 
под курсором мыши&]
[s7;%RU-RU CTRL`_PRESSED-|пушер активирован и нажат.&]
[s7;%RU-RU CTRL`_DISABLED-|пушер дезактивирован.&]
[s3; &]
[s0; ]]