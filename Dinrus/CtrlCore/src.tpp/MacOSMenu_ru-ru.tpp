topic "Поддержка меню MacOS";
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
[ {{10000@(113.42.0) [s0; [*@7;4 Поддержка меню MacOS]]}}&]
[s0; &]
[s0; Приложения U`+`+ без изменения при 
поддержки меню приложения MacOS выполняются 
с меню, расположенным внутри окна.&]
[s0; &]
[s0; Чтобы правильно поддерживать меню 
MacOS, приложение должно предоставлять 
иной код для MacOS (условная компиляция, 
зависящая от макроса PLATFORM`_COCOA). Как 
правило, это небольшие изменения,например, 
в приложении Uword единственное изменение 
такое&]
[s0; &]
[s0; [*C #ifdef PLATFORM`_COCOA]&]
[s0; [*C -|SetMainMenu(THISBACK(MainMenu));]&]
[s0; [*C #else]&]
[s0; [*C -|AddFrame(menubar);]&]
[s0; [*C #endif]&]
[s0; &]
[s0; заменяющее вызов AddFrame для добавления 
бруса главного меню на вызов TopWindow`::SetMainMenu.&]
[s0; &]
[s0; Другой вариант `- не использовать 
AddFrame, когда определено PLATFORM`_COCOA, и 
изменить код настройки меню (вызываемый 
при любом изменении главного меню) 
на что`-то типа&]
[s0; &]
[s0; [*C #ifdef PLATFORM`_COCOA]&]
[s0; [*C -|SetMainMenu(THISBACK(MainMenu));]&]
[s0; [*C #else]&]
[s0; [*C -|menubar.Set(THISBACK(MainMenu));]&]
[s0; [*C #endif]&]
[s0; &]
[s0; Другая фича MacOS `- dock menu. Его можно 
установить с помощью TopWindow`::WhenDockMenu.&]
[s0; &]
[s0;%- &]
[ {{10000F(128)G(128)@1 [s0; [* TopWindow members for MacOS style menu support]]}}&]
[s3;%- &]
[s5;:Upp`:`:TopWindow`:`:WhenDockMenu:%- [_^Upp`:`:Event^ Event]<Bar[@(0.0.255) `&]>_[* Whe
nDockMenu]&]
[s2; Представляет док`-меню приложения. 
Вызывается, если TopWindow на самом верху 
и пользователь выводит док`-меню приложения 
(например, правым кликом на иконке 
приложения).&]
[s3; &]
[s4;%- &]
[s5;:Upp`:`:TopWindow`:`:SetMainMenu`(Upp`:`:Event`<Upp`:`:Bar`&`>`):%- [@(0.0.255) voi
d]_[* SetMainMenu]([_^Upp`:`:Event^ Event]<Bar[@(0.0.255) `&]>_[*@3 menu])&]
[s2; Устанавливает главное меню приложения. 
Это меню будет активно, если данное 
высокоуровневое TopWindow на самом верху.&]
[s0; ]]