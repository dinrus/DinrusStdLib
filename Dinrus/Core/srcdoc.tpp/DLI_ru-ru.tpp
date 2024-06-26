topic "Рантаймная динамическая компоновка с использованием файлов .dli";
[l288;i1120;a17;O9;~~~.1408;2 $$1,0#10431211400427159095818037425705:param]
[a83;*R6 $$2,5#31310162474203024125188417583966:caption]
[b83;*4 $$3,5#07864147445237544204411237157677:title]
[i288;O9;C2 $$4,6#40027414424643823182269349404212:item]
[b42;a42;2 $$5,5#45413000475342174754091244180557:text]
[l288;b17;a17;2 $$6,6#27521748481378242620020725143825:desc]
[l321;t246;C@5;1 $$7,7#20902679421464641399138805415013:code]
[b2503;2 $$8,0#65142375456100023862071332075487:separator]
[*@(0.0.255)2 $$9,0#83433469410354161042741608181528:base]
[t4167;C2 $$10,0#37138531426314131251341829483380:class]
[l288;a17;*1 $$11,11#70004532496200323422659154056402:requirement]
[i417;b42;a42;O9;~~~.416;2 $$12,12#10566046415157235020018451313112:tparam]
[b167;C2 $$13,13#92430459443460461911108080531343:item1]
[i288;a42;O9;C2 $$14,14#77422149456609303542238260500223:item2]
[*@2$(0.128.128)2 $$15,15#34511555403152284025741354420178:NewsDate]
[l321;*C$7;2 $$16,16#03451589433145915344929335295360:result]
[l321;b83;a83;*C$7;2 $$17,17#07531550463529505371228428965313:result`-line]
[l160;t4167;*C+117 $$18,5#88603949442205825958800053222425:package`-title]
[2 $$19,0#53580023442335529039900623488521:gap]
[t4167;C2 $$20,20#70211524482531209251820423858195:class`-nested]
[b50;2 $$21,21#03324558446220344731010354752573:Par]
[2 $$0,0#00000000000000000000000000000000:Default]
[{_}%RU-RU 
[s2; Рантаймная динамическая компоновка 
с использованием файлов .dli&]
[s0; В Ultimate`+`+ используется файл .dli для 
того, чтобы автоматизировать генерацию 
объекта`-обёртки .dll. Чтобы создать 
такой объект, нужно предоставить 
файл `".dli`" с содержимым типа (actual example 
is for Lotus Notes client .dll):&]
[s0; &]
[s7; FN(WORD,   OSLoadString, (HMODULE hModule, STATUS StringCode, 
char `*retBuffer, WORD BufferLength))&]
[s7; FN(WORD,   OSTranslate, (WORD TranslateMode, char far `*In, 
WORD InLength, char far `*Out, WORD OutLength))&]
[s7; FN(STATUS, NotesInitExtended, (int argc, char  `*`*argv))&]
[s7; FN(STATUS, OSPathNetConstruct, (char `*PortName, char `*ServerName, 
char far `*FileName, char `*retPathName))&]
[s7; FN(STATUS, NSFDbOpen,  (char far `*PathName, DBHANDLE far `*rethDB))&]
[s7; FN(STATUS, NSFDbClose,  (DBHANDLE hDB))&]
[s7; ...........&]
[s0; &]
[s0; `- в основном, это слегка репарсированный 
файл`-заголовочник для .dll. Далее помещаем&]
[s0; &]
[s7; #define DLLFILENAME `"nnotes.dll`"&]
[s7; #define DLIMODULE   NOTES&]
[s7; #define DLIHEADER   <notes/notes.dli>&]
[s7; #define DLLCALL     LNPUBLIC&]
[s7; #include <Core/dli`_header.h>&]
[s0; &]
[s0; в общий файл`-заголовочник и&]
[s0; &]
[s7; #define DLLFILENAME `"nnotes.dll`"&]
[s7; #define DLIMODULE   NOTES&]
[s7; #define DLIHEADER   <notes/notes.dli>&]
[s7; #define DLLCALL     LNPUBLIC&]
[s7; #include <Core/dli`_source.h>&]
[s0; &]
[s0; в какой`-либо файл .cpp или, если нужно, 
чтобы .dll была только в одном файле 
.cpp, используем&]
[s0; &]
[s7; #define DLLFILENAME `"nnotes.dll`"&]
[s7; #define DLIMODULE   NOTES&]
[s7; #define DLIHEADER   <notes/notes.dli>&]
[s7; #define DLLCALL     LNPUBLIC&]
[s7; #include <Core/dli.h>&]
[s0; &]
[s0; в .cpp (это равносильно помещению обеих 
вариантов в .cpp).&]
[s0; &]
[s0; Создаётся глобальная функция NOTES(), 
возвращающая экземпляр объекта, у 
которого есть все функции .dll, описанные 
в файле .dli, в виде его методов. Более 
того, у него есть оператор bool, который 
можно использовать для тестирования 
на наличие .dll:&]
[s0; &]
[s7; char h`[256`];&]
[s7; if(NOTES())&]
[s7; -|NOTES().OSLoadString(GetModuleHandle(NULL), ERR(nError), h, 
255);&]
[s0; &]
[s0;3 ]]