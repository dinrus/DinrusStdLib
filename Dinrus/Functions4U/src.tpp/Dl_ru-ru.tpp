topic "Dl";
[2 $$0,0#00000000000000000000000000000000:Default]
[i448;a25;kKO9;2 $$1,0#37138531426314131252341829483380:class]
[l288;2 $$2,2#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:item]
[l288;a4;*@5;1 $$6,6#70004532496200323422659154056402:requirement]
[l288;i1121;b17;O9;~~~.1408;2 $$7,0#10431211400427159095818037425705:param]
[i448;b42;O9;2 $$8,8#61672508125594000341940100500538:tparam]
[b42;2 $$9,9#13035079074754324216151401829390:normal]
[{_} 
[ {{10000@(113.42.0) [s0;:Dl`:`:class:%% [*@7;4 Functions4U Reference. Dl]]}}&]
[s0;i448;a25;kKO9; &]
[s0;i448;a25;kKO9; Dl class is an easy way to load dynamic libraries:&]
[s0;i150;a25;kKO0; Windows: DLL.&]
[s0;i150;a25;kKO0; Linux: Shared libraries, relocatable files, or 
programs&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Список Публичных Методов]]}}&]
[s4; &]
[s5;:Dl`:`:Load`(const String`&`): [@(0.0.255) bool]_[* Load]([@(0.0.255) const]_[_^String^ S
tring]_`&[*@3 fileDl])&]
[s2;%% Loads library file [%-*@3 fileDl] before loading its functions. 
It returns false if error or [%-*@3 fileDl] does not exist.&]
[s2;%% [%-*@3 fileDl] has to contain the full path or has to be included 
in environment variables:&]
[s2;i150;O0;%% Windows: PATH&]
[s2;i150;O0; Linux: LD`_LIBRARY`_PATH&]
[s3; &]
[s4; &]
[s5;:Dl`:`:GetFunction`(const String`&`): [@(0.0.255) void]_`*[* GetFunction]([@(0.0.255) c
onst]_[_^String^ String]_`&[*@3 functionName])&]
[s2;%% It returns a pointer to [%-*@3 functionName] in the dynamic 
library or NULL if error or [%-*@3 functionName] is not in the 
dynamic library.&]
[s3; &]
[s0; &]
[s0; For example, in Windows:&]
[s0; &]
[s0;%% [C+75 Dl adsapi32;]&]
[s0;%% [C+75 long (`*GetDevice)(int);]&]
[s0;C+75%% &]
[s0;%% [C+75 if (!adsapi32.Load(AppendFileName(myDllFolder, `"adsapi32.dll`")))]&]
[s0;%% [C+75 -|throw Exc(Format(t`_(`"% dll not found`"), `"Adsapi32`"));]&]
[s0;C+75%% &]
[s0;%% [C+75 GetDevice `= (long (`*)(int))adsapi32.GetFunction(`"GetDevice`");]&]
[s0;%% [C+75 if (!GetDevice)]&]
[s0;%% [C+75 -|throw Exc(Format(t`_(`"Function %s does not found in 
dll`"), `"GetDevice`"));]&]
[s0;%% [C+75 long device `= GetDevice(3);]&]
[s0;C+75%% &]
[s0;C+75%% &]
[s0; And in Linux:&]
[s0; &]
[s0;%% [C+75 Dl libm;]&]
[s0;%% [C+75 double (`*cosine)(double);]&]
[s0;C+75%% &]
[s0;%% [C+75 if (!libm.Load(`"][%-C+75 /lib/libm.so.6][C+75 `"))]&]
[s0;%% [C+75 -|throw Exc(Format(t`_(`"% dl not found`"), `"Libm`"));]&]
[s0;C+75%% &]
[s0;%% [C+75 cosine `= (double (`*)(double))libm.GetFunction(`"cosine`");]&]
[s0;%% [C+75 if (!cosine)]&]
[s0;%% [C+75 -|throw Exc(Format(t`_(`"Function %s does not found in 
dl`"), `"cosine`"));]&]
[s0;%% [C+75 double val `= cosine(3.1416);]&]
[s0; ]]