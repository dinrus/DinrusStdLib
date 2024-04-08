topic "QtfEquation";
[ $$0,0#00000000000000000000000000000000:Default]
[H6;0 $$1,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$2,0#37138531426314131252341829483370:codeitem]
[l288;2 $$3,0#27521748481378242620020725143825:desc]
[0 $$4,0#96390100711032703541132217272105:end]
[{_} 
[ {{10000@(113.42.0) [s0;%% [*@7;4 Functions4U Reference. QtfEquation]]}}&]
[s1;2 &]
[s0; [2 QtfEquation converts a String with equations or formulas into 
a drawable object able to be painted or included into QTF.]&]
[s0;2 &]
[s0; [2 The functions parse the String searching for expresions and 
convert them into its graphic format.]&]
[s0;2 &]
[s0; [2 Some expressions parsed are:]&]
[s0;i150;O0; [2 Greek symbols]&]
[s0;i150;O0; [2 sqrt(content)]&]
[s0;i150;O0; [2 integral(content, from, to)]&]
[s0;i150;O0; [2 summation(content, from, to)]&]
[s0;i150;O0; [2 `^ as exponent.]&]
[s0;i150;O0; [2 Expressions like `=, `+, `-, `*, /.]&]
[s0;i150;O0; [2 Parenthesis () and brackets `[`].]&]
[s0;2 &]
[s0;2 &]
[ {{10000F(128)G(128)@1 [s0;%% [* Список Публичных Методов]]}}&]
[s1; &]
[s2;:QtfEquation`(const String`&`): [_^QtfRichObject^ QtfRichObject]_[* QtfEquation]([@(0.0.255) c
onst]_[_^String^ String]_`&[*@3 str])&]
[s3;%% It converts formulas in [%-*@3 str] into an object ready to 
be inserted into a QTF String.&]
[s4;%% &]
[s1; &]
[s2;:DrawEquation`(const String`&`): [_^Drawing^ Drawing]_[* DrawEquation]([@(0.0.255) cons
t]_[_^String^ String]_`&[*@3 str])&]
[s3;%% It converts formulas in [%-*@3 str] into a Drawing object.&]
[s4;%% &]
[s0;2%% &]
[s0;%% [2 For example, the result of this code:]&]
[s0;C2%% &]
[s0;%% [C2 -|String myqtf;]&]
[s0;C2%% &]
[s0;%% [C2 -|QtfRichObject a `= QtfEquation(`"(2/3)`");]&]
[s0;%% [C2 -|QtfRichObject b `= QtfEquation(`"integral(sqrt(cos(phi`_ini`^2)) 
`+ i`^2 `+ 6, i `= 1, 10)`*dx `= cos((27`+x`^2)`^3.25)/(PI`*R`_0`^2)`");]&]
[s0;%% [C2 -|QtfRichObject c `= QtfEquation(`"delta`_i `= a`+b`*x`+c`*x`^2`+d`*x`^3`");]&]
[s0;%% [C2 -|QtfRichObject d `= QtfEquation(`"sqrt(cos(phi`_ini`^2))`");]&]
[s0;%% [C2 -|QtfRichObject e `= QtfEquation(`"summation(a`+b`*x`+c`*x`^2`+d`*x`^3, 
x `= h, h`+1)`*dx `= SI`_h`");]&]
[s0;%% [C2 -|]&]
[s0;%% [C2 -|myqtf << `"`[R3 This are some formulas in QTF:`&`" << a 
<< `"`&`" << b << `"`&`" << c << `"`&`" << d << `"`&`" << e;]&]
[s0;C2%% &]
[s0;%% [C2 -|equation.SetData(myqtf);]&]
[s0;2%% &]
[s0;2%% &]
[s0;%% [2 is this:]&]
[s0;2%% &]
[s0;%% [2 
@@image:1995&1549
(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)
]&]
[s0;%% ]]