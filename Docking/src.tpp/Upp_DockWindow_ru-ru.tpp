topic "Класс DockWindow";
[i448;a25;kKO9; $$1,0#37138531426314131252341829483380:structitem]
[l288;2 $$2,0#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[a83;*R6 $$4,0#31310162474203024125188417583966:caption]
[H6;0 $$5,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$6,0#37138531426314131252341829483370:codeitem]
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s4; Класс DockWindow&]
[s2;l0;i64; DockWindow `- это класс `'основа`' или 
`'контейнер`' фреймворка докинга окон. 
Он отвечает за управление клиентскими 
окнами и за интерфейс пользователя, 
и содержит почти все методы, необходимые 
для настройки и управления этим фреймворком
.&]
[s0; &]
[s2;l0;i64; Для инициализации этого фреймворка 
требуется сделать три шага:&]
[s0; &]
[s2;i150;O0; Создать оконный класс, наследующий 
от DockWindow (вместо TopWindow)&]
[s2;i150;O0; Переопределить метод DockInit&]
[s2;i150;O0; Зарегистрировать или добавить 
DockableCtrls, используя предоставленные 
методы.&]
[s2;l0;i64; &]
[s2;l0;i64; Самая простая реализация может 
выглядеть примерно так:&]
[s2;l0;i64; &]
[s2;l0;i64; [C@(0.0.255)3 class ][C3 MyDockWindow : ][C@(0.0.255)3 public 
][C3 DockableWindow]&]
[s2;l0;i64; [C3 `{]&]
[s2;l0;i64; [C@(0.0.255)3 private][C3 :]&]
[s2;l0;i64; [C3     ArrayCtrl array;]&]
[s2;l0;i64;C3 &]
[s2;l0;i64; [C3     ][C@(0.0.255)3 virtual void ][C3 DockInit()]&]
[s2;l0;i64; [C3     `{]&]
[s2;l0;i64; [C3         DockLeft(Dockable(array, `"Dockable ArrayCtrl`").SizeHint(300, 
200));]&]
[s2;l0;i64; [C3     `}]&]
[s2;l0;i64; [C3 `};]&]
[s1;@(0.0.255) &]
[s1;:Upp`:`:DockWindow`:`:class: [@(0.0.255) class]_[* DockWindow]&]
[s2; DockWindow `- это класс `'основа`' или `'контейнер`' 
фреймворка докинга окон. Он отвечает 
за управление клиентскими окнами 
и за интерфейс пользоваателя, и содержит 
почти все методы, необходимые для 
настройки и управления этим фреймворком.&]
[s3; &]
[s5; &]
[s6;:Upp`:`:DockWindow`:`:DockInit`(`): [@(0.0.255) virtual] [@(0.0.255) void]_[* DockInit](
)&]
[s2;%RU-RU Требуется переопределить при 
реализации окна, чтобы обеспечить 
правильную инициализацию DockableCtrls.&]
[s3;%RU-RU &]
[s5;%RU-RU &]
[s6;K:Upp`:`:DockWindow`:`:State`(int`): [*@(64) virtual ][*@(0.0.255) void][*@(64) _][* Stat
e][*@(64) (][*@(0.0.255) int][*@(64) _][*@3 reason][*@(64) )]&]
[s2;%RU-RU  Используется для детекции открытия 
окна. Чтобы гарантировать правильную 
инициализацию, нужно вызвать DockWindow`::State(reason), 
если имело место переопределение 
этого метода.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Dockable`(Upp`:`:Ctrl`&`,Upp`:`:WString`): [^Upp`:`:DockableCtrl^ D
ockableCtrl]_`&_[* Dockable][@(64) (][^`:`:Ctrl^@(64) Ctrl][@(64) _`&][@3 ctrl][@(64) , 
][^`:`:WString^@(64) WString][@(64) _][@3 title][@(64) )]&]
[s2;%RU-RU Быстрый способ добавления контролов 
в интерфейс. Добавляет [%-*@3 ctrl] и внутренне 
управляет DockableCtrl, устанавливает его 
титул в [%-*@3 title] и возвращает ссылку 
для дальнейшей манипуляции.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Dockable`(Upp`:`:Ctrl`&`,const char`*`): [^`:`:DockableCtrl^ D
ockableCtrl]_`&_[* Dockable][@(64) (][^`:`:Ctrl^@(64) Ctrl][@(64) _`&][@3 ctrl][@(64) , 
][@(0.0.255) const][@(64) _][@(0.0.255) char][@(64) _`*][@3 title][@(64) _`=_][@3 0][@(64) )]&]
[s2;%RU-RU Быстрый способ добавления контролов 
в интерфейс. Добавляет [%-*@3 ctrl] и внутренне 
управляет DockableCtrl, устанавливает его 
титул в [%-*@3 title] и возвращает ссылку 
для дальнейшей манипуляции.&]
[s3;%RU-RU &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:DockLeft`(Upp`:`:DockableCtrl`&`,int`): [@(0.0.255) void][@(64) _
][* DockLeft][@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) , 
][@(0.0.255) int][@(64) _][@3 pos][@(64) _`=_`-][@3 1][@(64) )]&]
[s2; [%RU-RU   Docks ][*@3 dc][%RU-RU  to the ][%RU-RU* left ][%RU-RU docking 
frame, before the window indicated by ][*@3 pos][%RU-RU . ][*@3 pos 
]is always counted from left/top to right/bottom starting at 
&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:DockTop`(Upp`:`:DockableCtrl`&`,int`): [@(0.0.255) void][@(64) _
][* DockTop][@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) , 
][@(0.0.255) int][@(64) _][@3 pos][@(64) _`=_`-][@3 1][@(64) )]&]
[s2; [%RU-RU   Docks ][*@3 dc][%RU-RU  to the ][%RU-RU* top ][%RU-RU docking 
frame, before the window indicated by ][*@3 pos][%RU-RU . ][*@3 pos][@3  
]is always counted from left/top to right/bottom starting at 
&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:DockRight`(Upp`:`:DockableCtrl`&`,int`): [@(0.0.255) void][@(64) _
][* DockRight][@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) , 
][@(0.0.255) int][@(64) _][@3 pos][@(64) _`=_`-][@3 1][@(64) )]&]
[s2; [%RU-RU   Docks ][*@3 dc][%RU-RU  to the ][%RU-RU* right ][%RU-RU docking 
frame, before the window indicated by ][*@3 pos][%RU-RU . ][*@3 pos 
]is always counted from left/top to right/bottom starting at 
0.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:DockBottom`(Upp`:`:DockableCtrl`&`,int`): [@(0.0.255) void][@(64) _
][* DockBottom][@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) , 
][@(0.0.255) int][@(64) _][@3 pos][@(64) _`=_`-][@3 1][@(64) )]&]
[s2; [%RU-RU   Docks ][*@3 dc][%RU-RU  to the ][%RU-RU* bottom ][%RU-RU docking 
frame, before the window indicated by ][*@3 pos][%RU-RU . ][*@3 pos 
]is always counted from left/top to right/bottom starting at 
0.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Dock`(int`,Upp`:`:DockableCtrl`&`,int`): [@(0.0.255) void][@(64) _
][* Dock][@(64) (][@(0.0.255) int][@(64) _][@3 align][@(64) , ][^`:`:DockableCtrl^@(64) Dockabl
eCtrl][@(64) _`&][@3 dc][@(64) , ][@(0.0.255) int][@(64) _][@3 pos][@(64) _`=_`-][@3 1][@(64) )]&]
[s2; [%RU-RU3  ][%RU-RU  Docks ][*@3 dc][%RU-RU  to the docking frame specified 
by ][%RU-RU*@3 align][%RU-RU , before the window indicated by ][*@3 pos][%RU-RU . 
][*@3 pos ]is always counted from left/top to right/bottom starting 
at 0.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Tabify`(Upp`:`:DockableCtrl`&`,Upp`:`:DockableCtrl`&`): [@(0.0.255) v
oid][@(64) _][* Tabify][@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 target][@(64) ,
 ][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) )]&]
[s2; [%RU-RU3   ][%RU-RU Adds ][*@3 target][%RU-RU  as tab, sharing the same 
window as ][*@3 dc ](wherever [%RU-RU .][*@3 dc ]is positioned).&]
[s3;2 &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Float`(Upp`:`:DockableCtrl`&`,Upp`:`:Point`): [@(0.0.255) vo
id][@(64) _][* Float][@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) , 
][^`:`:Point^@(64) Point][@(64) _][@3 p][@(64) _`=_Null)]&]
[s2;%RU-RU  [3  ]Floats (opens as a normal child window) [%-*@3 dc] and 
if specified locates it at position [%-*@3 p].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Float`(Upp`:`:DockableCtrl`&`,const char`*`,Upp`:`:Point`): [@(0.0.255) v
oid][@(64) _][* Float][@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) ,
 ][@(0.0.255) const][@(64) _][@(0.0.255) char][@(64) _`*][@3 title][@(64) , 
][^`:`:Point^@(64) Point][@(64) _][@3 p][@(64) _`=_Null)]&]
[s2; [%RU-RU3   ][3 As above, but also sets ][*@3;3 dc][3 `'s title.]&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AutoHide`(Upp`:`:DockableCtrl`&`): [@(0.0.255) void][@(64) _][* A
utoHide][@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) )]&]
[s2;%RU-RU [3   Auto`-Hides (hides with a tab on the DockWindow) ][%-*@3;3 dc][3 . 
If docked ][%-*@3;3 dc ][3 will be hidden on the side of the window 
it is docked to, otherwise it will be hidden at the top of the 
DockWindow.]&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AutoHide`(int`,Upp`:`:DockableCtrl`&`): [@(0.0.255) void][@(64) _
][* AutoHide][@(64) (][@(0.0.255) int][@(64) _][@3 align][@(64) , ][^`:`:DockableCtrl^@(64) Doc
kableCtrl][@(64) _`&][@3 dc][@(64) )]&]
[s2; [%RU-RU3   Auto`-Hides (hides with a tab on the DockWindow) ][*@3;3 dc 
][3 with the tab appearing at the spcified alignment.]&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Close`(Upp`:`:DockableCtrl`&`): [@(0.0.255) void][@(64) _][* Clo
se][@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) )]&]
[s2; [%RU-RU3 Закрывает ][*@3;3 dc][3 .]&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:DockGroup`(int`,Upp`:`:String`,int`): [@(0.0.255) void][@(64) _
][* DockGroup][@(64) (][@(0.0.255) int][@(64) _][@3 align][@(64) , ][^`:`:String^@(64) String][@(64) _
][@3 group][@(64) , ][@(0.0.255) int][@(64) _][@3 pos][@(64) _`=_`-][@3 1][@(64) )]&]
[s2;%RU-RU  [3  Docks all windows in ][%-*@3;3 group ][%-3 to the given][3  
][%-*@3;3 align][3  at position ][%-*@3;3 pos][3 . See ][^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:Dock`(int`,`:`:DockableCtrl`&`,int`)^3 D
ock][3 .]&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:FloatGroup`(Upp`:`:String`): [@(0.0.255) void][@(64) _][* FloatG
roup][@(64) (][^`:`:String^@(64) String][@(64) _][@3 group][@(64) )]&]
[s2;%RU-RU  [3  Floats all windows in ][%-*@3;3 group][3 . See ][^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:Float`(`:`:DockableCtrl`&`,`:`:Point`)^3 F
loat]&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AutoHideGroup`(int`,Upp`:`:String`): [@(0.0.255) void][@(64) _
][* AutoHideGroup][@(64) (][@(0.0.255) int][@(64) _][@3 align][@(64) , ][^`:`:String^@(64) Stri
ng][@(64) _][@3 group][@(64) )]&]
[s0;l288;%RU-RU   Auto`-Hides all window in [%-*@3 group] at the specified 
[%-*@3 align][%- . See ][%-^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:AutoHide`(int`,`:`:DockableCtrl`&`)^ A
utoHide].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:TabDockGroup`(int`,Upp`:`:String`,int`): [@(0.0.255) void][@(64) _
][* TabDockGroup][@(64) (][@(0.0.255) int][@(64) _][@3 align][@(64) , ][^`:`:String^@(64) Strin
g][@(64) _][@3 group][@(64) , ][@(0.0.255) int][@(64) _][@3 pos][@(64) _`=_`-][@3 1][@(64) )]&]
[s0;l288;%RU-RU   Tabify all windows in [%-*@3 group] and dock them 
at the specified [%-*@3 align] and [%-*@3 pos]. See [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:Dock`(int`,Upp`:`:DockableCtrl`&`,int`)^ D
ock], [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:Tabify`(`:`:DockableCtrl`&`,`:`:DockableCtrl`&`)^ T
abify].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:TabFloatGroup`(Upp`:`:String`): [@(0.0.255) void][@(64) _][* Tab
FloatGroup][@(64) (][^`:`:String^@(64) String][@(64) _][@3 group][@(64) )]&]
[s2;%RU-RU  [3  Tabify all windows in ][%-*@3;3 group][3  and float them 
as one window. See ][^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:Float`(`:`:DockableCtrl`&`,`:`:Point`)^3 F
loat][3 , ][^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:Tabify`(`:`:DockableCtrl`&`,`:`:DockableCtrl`&`)^3 T
abify][3 . ]&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:CloseGroup`(Upp`:`:String`): [@(0.0.255) void][@(64) _][* CloseG
roup][@(64) (][^`:`:String^@(64) String][@(64) _][@3 group][@(64) )]&]
[s2;%RU-RU  [3  Close all windows in ][%-*@3;3 group][3 .]&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:IsDockVisible`(int`)const: [@(0.0.255) bool][@(64) _][* IsDockVisib
le][@(64) (][@(0.0.255) int][@(64) _][@3 align][@(64) )_][@(0.0.255) const]&]
[s2;%RU-RU [3   Determines if the dock frame at ][%-*@3;3 align][3  is 
visible.]&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:SetFrameSize`(int`,int`): [@(0.0.255) void][@(64) _][* SetFrameS
ize][@(64) (][@(0.0.255) int][@(64) _][@3 align][@(64) , ][@(0.0.255) int][@(64) _][@3 size][@(64) )
]&]
[s2;%RU-RU  [3  Sets the size of the frame at ][%-*@3;3 align][3  to ][%-*@3;3 size][3  
(pixels).]&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AnimateDelay`(int`): [^`:`:DockWindow^ DockWindow]_`&_Animat
eDelay[@(64) (][@(0.0.255) int][@(64) _][@3 ms][@(64) )]&]
[s2;%RU-RU   Sets the delay before UI animation starts to [%-*@3 ms] 
(milliseconds).&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Animate`(bool`,bool`,int`,int`): [^`:`:DockWindow^ DockWindo
w]_`&_Animate[@(64) (][@(0.0.255) bool][@(64) _][@3 highlight][@(64) _`=_true, 
][@(0.0.255) bool][@(64) _][@3 window][@(64) _`=_true, ][@(0.0.255) int][@(64) _][@3 ticks][@(64) _
`=_][@3 10][@(64) , ][@(0.0.255) int][@(64) _][@3 interval][@(64) _`=_][@3 20][@(64) )]&]
[s2;%RU-RU   Enables/Disables animation and controls animation speed. 
You may wish to change the defaults to improve the performance 
or appearance of animations. &]
[s2;%RU-RU [%-*@3 highlight] determines whether the visual indicator 
of a possible docking target is animated.&]
[s2;%RU-RU [%-*@3 window] determines whether floating window are animations 
are applied before docking. &]
[s2; [*@3 ticks ]sets the number of steps/frames involved in animation&]
[s2;%RU-RU [%-*@3 interval] sets the number of milliseconds between 
animation steps/frames.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:NoAnimate`(`): [^`:`:DockWindow^ DockWindow]_`&_NoAnimate[@(64) (
)]&]
[s2;%RU-RU   Disables both highlight and window animation.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:IsAnimated`(`)const: [@(0.0.255) bool][@(64) _]IsAnimated[@(64) (
)_][@(0.0.255) const]&]
[s2;%RU-RU   Returns true if any animation is enabled.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:IsAnimatedWindows`(`)const: [@(0.0.255) bool][@(64) _]IsAnimat
edWindows[@(64) ()_][@(0.0.255) const]&]
[s2;%RU-RU   Returns true only if window animation is enabled.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:IsAnimatedHighlight`(`)const: [@(0.0.255) bool][@(64) _]IsAnim
atedHighlight[@(64) ()_][@(0.0.255) const]&]
[s2;%RU-RU   Returns true only if highlight animation is enabled.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Tabbing`(bool`): [^`:`:DockWindow^ DockWindow]_`&_Tabbing[@(64) (
][@(0.0.255) bool][@(64) _][@3 `_tabbing][@(64) _`=_true)]&]
[s2;%RU-RU   Enables/Disables window tabbing for the user.&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:NoTabbing`(`): [^`:`:DockWindow^ DockWindow]_`&_NoTabbing[@(64) (
)]&]
[s2;%RU-RU   Disables window tabbing for the user.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:IsTabbing`(`)const: [@(0.0.255) bool][@(64) _]IsTabbing[@(64) ()
_][@(0.0.255) const]&]
[s2;%RU-RU   Returns true if window tabbing is enabled.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:NestedTabs`(bool`): [^`:`:DockWindow^ DockWindow]_`&_NestedT
abs[@(64) (][@(0.0.255) bool][@(64) _][@3 `_nestedtabs][@(64) _`=_true)]&]
[s2; [%RU-RU   Sets the default behaviour when a tabbed window is added 
as a tab to another window. If ][*@3 `_nestedtabs][%RU-RU  is true 
the inserting window will be nested as a single tab, if false 
all of it`'s tabs will be added to the target window. The user 
may alternate between these modes using a customizable key. See 
][^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:SetNestedToggleKey`(`:`:dword`)^ S
etNestedToggleKey].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:NoNestedTabs`(`): [^`:`:DockWindow^ DockWindow]_`&_NoNestedT
abs[@(64) ()]&]
[s2;%RU-RU   Equivalent to calling [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:NestedTabs`(bool`)^ N
estedTabs(false)].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:IsNestedTabs`(`)const: [@(0.0.255) bool][@(64) _]IsNestedTabs[@(64) (
)_][@(0.0.255) const]&]
[s2;%RU-RU   Returns true if nested tabs is the default behaviour 
and false otherwise.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Grouping`(bool`): [^`:`:DockWindow^ DockWindow]_`&_Grouping[@(64) (
][@(0.0.255) bool][@(64) _][@3 grouping][@(64) _`=_true)]&]
[s2;%RU-RU   Enable/Disables group`-based options in the user interface. 
Affects all DockableCtrls. &]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:NoGrouping`(`): [^`:`:DockWindow^ DockWindow]_`&_NoGrouping[@(64) (
)]&]
[s2;%RU-RU   Disables grouping&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:IsGrouping`(`)const: [@(0.0.255) bool][@(64) _]IsGrouping[@(64) (
)_][@(0.0.255) const]&]
[s2;%RU-RU   Returns true if grouping is enabled.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AllowDockAll`(`): [^`:`:DockWindow^ DockWindow]_`&_AllowDock
All[@(64) ()]&]
[s2;%RU-RU   Allows docking of windows to all alignments.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AllowDockNone`(`): [^`:`:DockWindow^ DockWindow]_`&_AllowDoc
kNone[@(64) ()]&]
[s2;%RU-RU   Denys docking of windows to all alignments.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AllowDockLeft`(bool`): [^`:`:DockWindow^ DockWindow]_`&_Allo
wDockLeft[@(64) (][@(0.0.255) bool][@(64) _][@3 v][@(64) _`=_true)]&]
[s2;%RU-RU   Allows/Denys docking of windows to [* left] dock frame. 
&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AllowDockTop`(bool`): [^`:`:DockWindow^ DockWindow]_`&_Allow
DockTop[@(64) (][@(0.0.255) bool][@(64) _][@3 v][@(64) _`=_true)]&]
[s2;%RU-RU   Allows/Denys docking of windows to [* top] dock frame. 
&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AllowDockRight`(bool`): [^`:`:DockWindow^ DockWindow]_`&_All
owDockRight[@(64) (][@(0.0.255) bool][@(64) _][@3 v][@(64) _`=_true)]&]
[s2;%RU-RU   Allows/Denys docking of windows to [* right] dock frame. 
&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AllowDockBottom`(bool`): [^`:`:DockWindow^ DockWindow]_`&_Al
lowDockBottom[@(64) (][@(0.0.255) bool][@(64) _][@3 v][@(64) _`=_true)]&]
[s2;%RU-RU   Allows/Denys docking of windows to [* bottom] dock frame. 
&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AllowDock`(int`,bool`): [^`:`:DockWindow^ DockWindow]_`&_All
owDock[@(64) (][@(0.0.255) int][@(64) _][@3 a][@(64) , ][@(0.0.255) bool][@(64) _][@3 v][@(64) _`=_
true)]&]
[s2;%RU-RU     Allows/Denys docking of windows to dock frame specified 
by alignment [%-*@3 a].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:IsDockAllowed`(int`): [@(0.0.255) bool][@(64) _]IsDockAllowed[@(64) (
][@(0.0.255) int][@(64) _][@3 align][@(64) )]&]
[s2;%RU-RU   Returns true if docking is allowed to the dock frame 
indicated by [%-*@3 align].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:IsDockAllowed`(int`,`:`:DockableCtrl`&`): [@(0.0.255) bool][@(64) _
]IsDockAllowed[@(64) (][@(0.0.255) int][@(64) _][@3 align][@(64) , ][^`:`:DockableCtrl^@(64) D
ockableCtrl][@(64) _`&][@3 dc][@(64) )]&]
[s2; [%RU-RU   ][^`:`:DockableCtrl^ DockableCtrl]s may have docking permissions 
set independently of the DockWindow. This function compares the 
permissions of [*@3 dc ]and the DockWindow and returns true if 
both allow docking to the dock frame specified by [*@3 align].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:AutoHide`(bool`): [^`:`:DockWindow^ DockWindow]_`&_AutoHide[@(64) (
][@(0.0.255) bool][@(64) _][@3 v][@(64) _`=_true)]&]
[s2;%RU-RU   Enables/Disables auto`-hide options in the user interface. 
Affects all DockableCtrls.&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:IsAutoHide`(`): [@(0.0.255) bool][@(64) _]IsAutoHide[@(64) ()]&]
[s2;%RU-RU   Returns true if auto`-hiding is enabled.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:NestedToggleKey`(`): [^`:`:dword^ dword]_NestedToggleKey[@(64) (
)]&]
[s2;%RU-RU   Returns the key (or key combination) used to alternate 
between tab nesting modes. See [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:NestedTabs`(bool`)^ N
estedTabs].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:SetNestedToggleKey`(`:`:dword`): [^`:`:dword^ dword]_SetNest
edToggleKey[@(64) (][^`:`:dword^@(64) dword][@(64) _][@3 key][@(64) )]&]
[s2;%RU-RU     Sets the key (or key combination) used to alternate 
between tab nesting modes to [%-*@3 key]. Default value is K`_CTRL 
`| K`_SHIFT (ctrl or shift key). Setting to 0 will prevent the 
user from changing mode. See [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:NestedTabs`(bool`)^ N
estedTabs].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:WindowButtons`(bool`,bool`,bool`): [^`:`:DockWindow^ DockWin
dow]_`&_WindowButtons[@(64) (][@(0.0.255) bool][@(64) _][@3 menu][@(64) , 
][@(0.0.255) bool][@(64) _][@3 hide][@(64) , ][@(0.0.255) bool][@(64) _][@3 close][@(64) )]&]
[s2;%RU-RU   Hide/Shows button type on all DockableCtrls.&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:HasMenuButtons`(`)const: [@(0.0.255) bool][@(64) _]HasMenuButtons
[@(64) ()_][@(0.0.255) const]&]
[s2;%RU-RU   Returns true if window menu buttons are currently shown.&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:HasHideButtons`(`)const: [@(0.0.255) bool][@(64) _]HasHideButtons
[@(64) ()_][@(0.0.255) const]&]
[s2;%RU-RU   Returns true if auto`-hide buttons are currently shown.&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:HasCloseButtons`(`)const: [@(0.0.255) bool][@(64) _]HasCloseButto
ns[@(64) ()_][@(0.0.255) const]&]
[s2;%RU-RU   Returns true if close buttons are currently shown.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Register`(Upp`:`:DockableCtrl`&`): [^`:`:DockableCtrl^ Docka
bleCtrl]_`&_Register[@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) )
]&]
[s2;%RU-RU   Registers [%-*@3 dc] without assigning it a state or position. 
This makes it available for Serialization and user interaction 
via the Window Manager of menus.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:Deregister`(Upp`:`:DockableCtrl`&`): [@(0.0.255) void][@(64) _
]Deregister[@(64) (][^`:`:DockableCtrl^@(64) DockableCtrl][@(64) _`&][@3 dc][@(64) )]&]
[s2;%RU-RU   The opposite of [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:Register`(`:`:DockableCtrl`&`)^ R
egister].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:GetDockableCtrls`(`)const: [@(0.0.255) const][@(64) _][^`:`:Vector^@(64) V
ector][@(64) <DockableCtrl_`*>_`&]GetDockableCtrls[@(64) ()_][@(0.0.255) const]&]
[s2;%RU-RU   Returns a Vector containing pointers to all DockableCtrls 
currently managed by this DockWindow.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:DockManager`(`): [@(0.0.255) void][@(64) _]DockManager[@(64) ()]&]
[s2;%RU-RU   Displays the default Window Manager dialog. This allows 
users to save/load layouts, create/delete/manage groups and adjust 
various global options.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:DockWindowMenu`(Upp`:`:Bar`&`): [@(0.0.255) void][@(64) _]Dock
WindowMenu[@(64) (][^`:`:Bar^@(64) Bar][@(64) _`&][@3 bar][@(64) )]&]
[s2;%RU-RU   Adds the default window menu to [%-*@3 bar]. This contains 
a list of windows (if [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:Grouping`(bool`)^ G
rouping] is enabled, organised by group) and an option to open 
Window Manager option. windows can be manipulated via sub`-menus.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:SerializeWindow`(Upp`:`:Stream`&`): [@(0.0.255) void][@(64) _]S
erializeWindow[@(64) (][^`:`:Stream^@(64) Stream][@(64) _`&][@3 s][@(64) )]&]
[s2;%RU-RU   Serializes all window settings and layouts (includeing 
saved layouts) to Stream [%-*@3 s].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:SerializeLayout`(Upp`:`:Stream`&`,bool`): [@(0.0.255) void][@(64) _
]SerializeLayout[@(64) (][^`:`:Stream^@(64) Stream][@(64) _`&][@3 s][@(64) , 
][@(0.0.255) bool][@(64) _][@3 withsavedlayouts][@(64) _`=_true)]&]
[s2;%RU-RU   Serializes the current layout to Stream [%-*@3 s]. If 
[%-*@3 withsavedlayouts ]`= true layout saved by the user will 
also be serialized.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:SaveLayout`(Upp`:`:String`): [@(0.0.255) int][@(64) _]SaveLayo
ut[@(64) (][^`:`:String^@(64) String][@(64) _][@3 name][@(64) )]&]
[s2;%RU-RU   Saves the current layout as [%-*@3 name].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:LoadLayout`(int`): [@(0.0.255) void][@(64) _]LoadLayout[@(64) (][@(0.0.255) i
nt][@(64) _][@3 ix][@(64) )]&]
[s2;%RU-RU   Load the saved layout number [%-*@3 ix]. Use [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:LayoutCount`(`)const^ L
ayoutCount] to find the upper limit for [*@3 ix].&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:LoadLayout`(`:`:String`): [@(0.0.255) void][@(64) _]LoadLayout[@(64) (
][^`:`:String^@(64) String][@(64) _][@3 name][@(64) )]&]
[s2;%RU-RU   Loads a saved layout by [%-*@3 name].&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:DeleteLayout`(`:`:String`): [@(0.0.255) void][@(64) _]DeleteLayou
t[@(64) (][^`:`:String^@(64) String][@(64) _][@3 name][@(64) )]&]
[s2;%RU-RU   Deletes a saved layout by [%-*@3 name].&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:GetLayoutName`(int`)const: [^`:`:String^ String]_GetLayoutName[@(64) (
][@(0.0.255) int][@(64) _][@3 ix][@(64) )_][@(0.0.255) const]&]
[s2;%RU-RU   Returns the name of layout number [%-*@3 ix]. Use [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:LayoutCount`(`)const^ L
ayoutCount] to find the upper limit for [*@3 ix].&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:LayoutCount`(`)const: [@(0.0.255) int][@(64) _]LayoutCount[@(64) ()
_][@(0.0.255) const]&]
[s2;%RU-RU   Returns the number of saved layouts.&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:GetLayouts`(`)const: [@(0.0.255) const][@(64) _][^`:`:ArrayMap^@(64) A
rrayMap][@(64) <][^`:`:String^@(64) String][@(64) , ][^`:`:String^@(64) String][@(64) >_`&_
]GetLayouts[@(64) ()_][@(0.0.255) const]&]
[s2;%RU-RU   Returns list of the saved layouts if the form ArrayMap<name, 
layout string>.&]
[s3; &]
[s5; &]
[s6;K:`:`:DockWindow`:`:DisableFloating`(`): [@(0.0.255) void][@(64) _]DisableFloating[@(64) (
)]&]
[s2;%RU-RU   Disables all floating windows. When manipulating windows 
from a modal dialog you may need to call this after [^topic`:`/`/Docking`/src`/DockWindow`$ru`-ru`#`:`:DockWindow`:`:Float`(`:`:DockableCtrl`&`,`:`:Point`)^ F
loating] a DockableCtrl to ensure correct modal behaviour.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:EnableFloating`(bool`): [@(0.0.255) void][@(64) _]EnableFloati
ng[@(64) (][@(0.0.255) bool][@(64) _][@3 enable][@(64) _`=_true)]&]
[s2;%RU-RU   Enables/Disables all floating windows.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:SetHighlightStyle`(Upp`:`:DockableCtrl`:`:Style`&`): [@(0.0.255) v
oid][@(64) _]SetHighlightStyle[@(64) (][^`:`:DockableCtrl`:`:Style^@(64) DockableCtrl`:
:Style][@(64) _`&][@3 s][@(64) )]&]
[s2;%RU-RU   Sets the style used by UI highlight of possible docking 
targets to [%-*@3 s].&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:DockLayout`(`): [@(0.0.255) void][@(64) _]DockLayout[@(64) ()]&]
[s2;%RU-RU   Prepares the window for user interaction by adding the 
required docking frames. This is called automatically when the 
window is opened, so should not normally be need to be called 
unless some unusual frame organization is required.&]
[s3; &]
[s5; &]
[s6;K:Upp`:`:DockWindow`:`:`:`:DockWindow`(`): DockWindow[@(64) ()]&]
[s2;%RU-RU Конструктор класса.&]
[s3;%RU-RU ]]