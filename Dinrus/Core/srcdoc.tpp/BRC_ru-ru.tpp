topic "Включение бинарных данных с помощью файлов .brc";
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
[{_}%EN-US 
[s2; [@5 Включение бинарных данных с помощью 
файлов .brc]&]
[s0; В Ultimate`+`+, используя DinrusIDE, не так уж 
и сложно внедрить в приложение двоичные 
данные с помощью исходников с расширением 
brc (binary resource). Просто вставьте файл 
с расширением `".brc`" в пакет и напечатайте 
в нём:&]
[s0; &]
[s7; BINARY(std`_tmpl`_vfk, `"vfk.wtl`")&]
[s0; &]
[s0; Первый аргумент ([/ std`_tmpl`_vfk] в примере 
выше) `- это публичный идентификатор 
типа [/ byte `*, ]который указывает на начало 
блока двоичных данных. Также автоматически 
сгенерируется другая публичная переменная, 
int [/ std`_tmpl`_vfk`_length], которая содержит 
длину этого блока. `\0 автоматически 
приставляется в конце блока. Каждый 
блок будет aligned (`"разлинуется`") как 
произведение 4 байтов (в текущей версии, 
но можно `"разлиновывать`" его индивидуально)
. Второй параметр `- это имя файла (относитель
но к расположению файла brc) ,`- файла, 
который внедряется как двоичный блок.&]
[s0; &]
[s0; После этого просто наберите в исходнике:&]
[s0; &]
[s7; #include `"myfile.brc`"&]
[s0; &]
[s0; Now you can use the [/ std`_tmpl`_vfk] and [/ std`_tmpl`_vfk`_length] 
variables to access the binary data `- TheIDE builder performs 
the necessary steps to get them into your executable.&]
[s0; &]
[s0; You can use a very similar mechanism to generate binary data 
arrays. This is supposed to be handy e.g. for animations in games 
or generally for processing a large number of binary blocks in 
a database`-like manner. The variant of the above macro is as 
thus:&]
[s0; &]
[s7; BINARY`_ARRAY(std`_tmpl`_vfk, 1, `"vfk.wtl`")&]
[s0; &]
[s0; Here the new middle argument (1) is the index at which to put 
the binary file in the array. The size of the array is automatically 
set to maximum of these values `+ 1. The array is accessed via 
the variable&]
[s0; &]
[s7; byte `*std`_tmpl`_vfk`[`];&]
[s0; &]
[s0; The array can be managed with the help of another pair of variables:&]
[s0; &]
[s7; int `*std`_tmpl`_vfk`_length`[`];&]
[s7; int std`_tmpl`_vfk`_count;&]
[s0; &]
[s0; The first variable, [/ <ident>`_length], is an array of block 
lengths. If you omit certain index numbers in the brc file, the 
appropriate pointers in the std`_tmpl`_vfk array are set to NULL 
and the appropriate length elements in the [/ std`_tmpl`_vfk`_length] 
array are set to `-1. The other variable, [/ std`_tmpl`_vfk`_count], 
gives number of elements in the [/ std`_tmpl`_vfk] and [/ std`_tmpl`_vfk`_length] 
arrays.&]
[s0; &]
[s0; Another variant lets you put multiple files matching a certain 
file mask to the binary resource:&]
[s0; &]
[s0; [C1 -|BINARY`_MASK(std`_templates, `"`*.wtl`")]&]
[s0; &]
[s0; The binary resource generator locates all files matching the 
given mask (relative to the brc file directory) and puts them 
to the resource file in a very similar manner like with BINARY`_ARRAY`'s. 
Again, the following variables are created:&]
[s0; &]
[s7; byte `*std`_templates`[`];&]
[s7; int `*std`_templates`_length`[`];&]
[s7; int std`_templates`_count;&]
[s0; &]
[s0; However, in this case, another variable gets exported from the 
binary object:&]
[s0; &]
[s7; char `*std`_templates`_files`[`];&]
[s0; &]
[s0; This is an array (with [/ std`_templates`_count] element) of pointers 
to zero`-terminated strings holding the individual file names 
(without path). Also, before putting the files to the archive, 
they get sorted by their file names.&]
[s0; &]
[s0; In all BINARY objects, you can use BZ2 or ZIP after the filename 
string to specify that the file(s) should be zipped or bzip2`-ed 
before putting them to the archive. In this case, the [/ <ident>`_length] 
variables apply to the length of the compressed file, not the 
original file. Like this:&]
[s0; &]
[s7; BINARY(std`_tmpl`_vfk, `"vfk.wtl`" BZ2)&]
[s0; &]
[s0; Currently, in the WIN32 version, an internal mechanism is used 
to generate the appropriate OBJ file during the binary data import. 
Under LINUX, a c source file with the binary data is generated 
internally which is then normally passed to the gcc compiler. 
In both cases, the import is rather fast at least for, say, a 
few megabytes of imported data.&]
[s0; ]]