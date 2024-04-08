topic "Концепты Дизайна Контролов";
[ $$0,0#00000000000000000000000000000000:Default]
[{_}%RU-RU 
[s0; [*R@5;6 Концепты Дизайна Контролов]&]
[s0; &]
[s0; При разработке собственных Ctrl`'ов 
часто `'реинвентируют`' `- повторно 
изобретают `- колесо... А ведь есть 
общие образцы, как это делать, и для 
вашего контрола Ultimate`+`+ , вероятно, 
уже имеется класс`-основа. Так, знание 
класса`-основы и некоторых его ключевых 
концептов дизайна может сделать вашу 
жизнь спокойней, а разработку которолов 
быстрее, позволяя более фокусироваться 
на проблеме, нежели на методах её 
решения. Как всегда, лучшая справка 
по Ctrl `- это его исходный код, который 
довольно`-таки не маловат, отчего 
попытаемся `"собрать до кучи`" (суммировать) 
некоторые полезности. В любом случае, 
стоит просмотреть [C virtual ]функции в 
Ctrl, чтобы понять, что подразумевается 
для использования или реализации 
лично вами.&]
[s0; &]
[s0; В целом, Ctrl в U`+`+ `- это визуализатор 
какого`-то рода данных. Данные, однако, 
не статичны, могут меняться из ГИП(GUI) 
(point and click) или из ИПП(API) перспектив, 
с помощью манипуляционных функций. 
Разница в том, что ГИП`-интеракция 
нацелена на изменение внутренних 
данных (или состояния) Ctrl`'а, И на уведомление
 приложения об изменениях, но изменение 
этого с помощью ИПП НЕ должно генерировать 
никаких уведомлений об изменении. 
Это очень важное правило дизайна, 
предохраняющее от множества `"головняков`", 
из`-за рекурсивных инвокаций при модификации
 Ctrl`'а через API.&]
[s0; &]
[s0; [* Ctrl Tree]&]
[s0; &]
[s0; В Ultimate`+`+ используется linked list (линкованный 
список) со всеми `"чадами`" (контролами`-отпрыс
ками) Ctrl`'а, которые Add()ированы в нему, 
разделяя с ним его пространство отрисовки. 
Ctrl НЕ `"владеет`" своими отпрысками, 
а просто ссылается на них (Ptr<Ctrl>). Владеть 
ими должно ваше приложение, где`-нибудь 
в контейнере U`+`+, напр. Array<Label>,  либо 
они уже являются членами вашего приложения, 
если используются файлы Layout. Если 
какой`-то Ctrl добавляется к какому`-то 
другому, надо удалить его у предыдущего 
родителя, а то Ctrl может стать частью 
двух деревьев.&]
[s0; &]
[s0; [* GetData / SetData]&]
[s0; &]
[s0; Большинству Ctrl`'ов, создаваемых вами, 
потребуется только одно единственное 
значение, чтобы визуализировать его 
или представить. Это верно для EditFields, 
Buttons, Labels и проч. Чтобы мочь Get / Set это 
единственное значение в/из Ctrl`'а, в 
U`+`+ используется его собственный 
класс `'полиморфного`' Value, который 
даёт контролам способность получать 
и обрабатывать intrinsic типы данных внутри 
себя, через единый интерфейс, не требуя 
от вас никаких преобразований. Для 
этого существует парочка GetData / SetData. 
Это главная дверь в ваш Ctrl. Она есть 
даже у более сложных Ctrl`'ов, типа TreeCtrl, 
 предоставляя текущий выделенный 
индекс. Думайте о вашем Ctrl`'е, о той 
информации, которую он должен донести, 
как о главном, через этот интерфейс. 
Это облегчает неявное использование, 
даже к понятиях нотификации (смотрите 
далее)&]
[s0; &]
[s0; [* WhenAction Callback `- обратный вызов (обрвыз) 
ПриДействии]&]
[s0; &]
[s0; Чтобы сообщить `"высшим слоям`" о каких`-либо 
изменениях, ваш Ctrl внутренне может 
воспользоваться (или внешне это сделать 
пользователь) функцией Action(), которая 
вызовет WhenCallback.и обеспечит feedback (поступление
 информации в обратном направлении 
по дереву контролов). Этот Callback можно 
установить, воспользовавшись подходом 
`'<<`= THISBACK()`'. Поэтому использовать 
его для своих собственных контролов 
предпочтительнее, так как это ведёт 
к соответствиям. кратких синтаксисов 
U`+`+. Нужно быть осторожным, только 
при вызове Action() внутри кода при графической 
интеракции с пользователем. Модифицируя 
свой контрол через ИПП, она не должна 
генерировать никакий Action(). Ещё больше 
или несколько иные нотификации можно 
обеспечить своим контролам с помощью 
других глобальных Callback`'ов (или даже 
Callback1<yourtype> или более), при необходимости. 
Используйте конвенцию имён WhenSomething, 
чтобы отражать поведение Event`'а (события).&]
[s0; &]
[s0; [* Updated(), SetModify(), ResetModify(), ClearModify(), IsModified()]&]
[s0; &]
[s0; Зачастую контрол должен обработать 
или вычислить иные вещи на основе 
изменений внутри него каких`-либо 
данных (это могут быть какие`-то результаты, 
кэшированные значения или типа того,а 
НЕ графические вспомогательные данные. 
Это делается с помощью Layout(), которая 
вызывается при изменении размеров 
(перемере) или открытии Ctrl`'а). Используйте 
виртуальную функцию Updated(), чтобы реализовать
 её, так как её можно триггировать 
снаружи, используя функцию Update(). Она 
также SetModified()`'ет ваш Ctrl, и вы это в 
состоянии проверить. Часто, при изменении 
данных, Ctrl`'у нужно каким`-то образом 
обновиться, перевычислить свои `"штуки`" 
и сообщить пользователю (`"нотифицировать`" 
его). UpdateAction() делает это одним шагом, 
вызывая обе. Если ещё нужно изменить 
и графические данные, UpdateActionRefresh() 
`- следующее звено, которое вызовет 
дополнительно Paint(). ClearModify() также 
рекурсивно действует на всех отпрысков.&]
[s0; &]
[s0;* &]
[s0; [* Стратегии Refresh()`'а]&]
[s0; &]
[s0; Каждый U`+`+ Ctrl можно явно запланировать 
под Refresh(). Это не всегда вызывает немедленный
 Draw(), т.е. в Win32, область отрисовки контрола 
отмечается меткой `"на перерисовку`", 
и она будет обработана `"как только, 
так сразу`" (Message Queue, т.е. очередь сообщений, 
решает и `"зажигает`" событие WM`_PAINT, 
что и ведёт к перерисовке). Sync() вызывает 
немедленную перерисовку в ручном 
режиме Иногда это очень полезно `- 
отобразить изменения незамедлительно, 
так как Main Thread (главный поток), который 
должен бы это сделать,на какое`-то 
время будет под блокировкой. Например, 
это используется в контроле Progress, 
чтобы дать знать пользователю о работе 
в Main Thread, которая `"в прогрессе`" (и 
из`-за этого перерисовка невозможна). 
Подробнее на эту тематику можно прочитать 
в документации к исходникам и в этом 
Руководстве.&]
[s0; &]
[s0; Так когда же бывает нужная точка 
для вызова Refresh()? Это относительно. 
Подумайте о своём контроле и логически 
определитесь, что можно считать представлен
ием ваших данных, а что является просто 
сетапом (настройкой) или вспомогательной 
параметризацией. Итак, изменение 
любых данных, при котором неверно 
отображается любая порция пространства 
отрисовки Ctrl`'а, должна триггировать 
Refresh(). При всём ином, вероятно, этого 
не требуется. Пункт помощи: SetData(), вероятно, 
наверняка меняет ваши данные. В целом, 
эта функция, как правило, должна вызывать 
Refresh() после манипуляций с данными 
Ctrl`'а. OTOH, т.е. изменение Style, немедленно 
делать Refresh() не должно, так как кроме 
того ещё необходимо изменить какие`-то 
иные параметры`-настройки, а неявные 
вызовы Refresh() будут напрасны и приведут 
к потере производительности.&]
[s0; &]
[s0; При взаимодействии пользователя 
с ГИП, изменении им данных, любой контрол 
должен перерисовываться, ну, хотя 
бы порциями. Изменение его со стороны 
ИПП ( API), should probably not, except for SetData....maybe. 
Because your application can call Refresh() anytime by itself, 
it knows best when and why.&]
[s0; &]
[s0; This all does apply for all Ctrl`'s in this world, cases differ. 
While the more static Ctr`'s like an EditField don`'t really 
care about heavy Refresh()ing (you wouldn`'t feel it), Ctrl`'s 
like a Plot Diagram showing some frequent live Data from some 
Generator would. Thats why in latter case, it might be useful 
to NOT Refresh() on data arrival (SetData) or change, and have 
the API determine the time, when to Refresh(), maybe after having 
inserted or changed a couple of related Plots.&]
[s0; &]
[s0; Refreshing a control just enough can sometimes be crucial. Take 
again the Diagram that plots live data.Refreshing it each and 
every little time due to small bits of info changes can keep 
your application busy and poor in response. Invent means of `'collecting`' 
data without posting it to the Ctrl, and once every 200 ms, do 
your work..and let the Ctrl Refresh(), this still yields a good 
look and keeps the GUI responsive.&]
[s0; &]
[s0; ]]