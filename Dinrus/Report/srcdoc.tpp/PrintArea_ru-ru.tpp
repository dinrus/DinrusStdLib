topic "Подсказки по Печати Отчёта";
[l288;i1120;a17;O9;~~~.1408;2 $$1,0#10431211400427159095818037425705:param]
[a83;*R6 $$2,5#31310162474203024125188417583966:caption]
[H4;b83;*4 $$3,5#07864147445237544204411237157677:title]
[i288;O9;C2 $$4,6#40027414424643823182269349404212:item]
[b42;a42;2 $$5,5#45413000475342174754091244180557:text]
[l288;b17;a17;2 $$6,6#27521748481378242620020725143825:desc]
[l321;C@5;1 $$7,7#20902679421464641399138805415013:code]
[b2503;2 $$8,0#65142375456100023862071332075487:separator]
[*@(0.0.255)2 $$9,0#83433469410354161042741608181528:base]
[C2 $$10,0#37138531426314131251341829483380:class]
[l288;a17;*1 $$11,11#70004532496200323422659154056402:requirement]
[i417;b42;a42;O9;~~~.416;2 $$12,12#10566046415157235020018451313112:tparam]
[b167;C2 $$13,13#92430459443460461911108080531343:item1]
[i288;a42;O9;C2 $$14,14#77422149456609303542238260500223:item2]
[*@2$(0.128.128)2 $$15,15#34511555403152284025741354420178:NewsDate]
[l321;*C$7;2 $$16,16#03451589433145915344929335295360:result]
[l321;b83;a83;*C$7;2 $$17,17#07531550463529505371228428965313:result`-line]
[l160;*C+117 $$18,5#88603949442205825958800053222425:package`-title]
[2 $$19,0#53580023442335529039900623488521:gap]
[C2 $$20,20#70211524482531209251820423858195:class`-nested]
[b50;2 $$21,21#03324558446220344731010354752573:Par]
[2 $$0,0#00000000000000000000000000000000:Default]
[{_}%EN-US 
[s2; [4 Подсказки по Печати Отчёта]&]
[s5; [+92 Следуйте этим указаниям, чтобы 
получить на бумаге правильные ][*/@(0.0.255)3 print 
margins, print area][+92  и ][*/@(0.0.255)3 position]&]
[s5; Пример : для бумаги размером A4&]
[s5; &]
[s5; Printed Page Size : A4 ( 210x297 mm )&]
[s5;i150;O0; Left Margin : 25 mm&]
[s5;i150;O0; Right Margin : 10 mm&]
[s5;i150;O0; Top Margin : 10 mm&]
[s5;i150;O0; Bottom Margin : 10 mm&]
[s5; &]
[s5; QTF Print Calculations are done `@ 600DPI  1 mm `= 600/25.4 
`= 6000 / 254 is 23.6 dots per mm&]
[s5; Values in Report setting are int. multiplication by 6000 then 
divide by 254 we get better precision&]
[s5; &]
[s5; Report Setting :&]
[s5; These calculated margins can be used for printing to PDF file&]
[s5;i150;O0; Left Margin : 25 `* 6000 / 254 `= [* 590]&]
[s5;i150;O0; Right Margin : 10 `* 6000 / 254 `= 236&]
[s5; &]
[s5;i150;O0; Top Margin : 10 `* 6000 / 254 `= [* 236]&]
[s5;i150;O0; Bottom Margin : 10 `* 6000 / 254 `= 236&]
[s5; &]
[s5; [* Report Print area ( ]Report.SetPageSize[*@(0.0.255)  ( cx, cy 
)][*  )  ]Portrait orientation&]
[s5; cx `= (PageWidth `- LeftMargin `- RightMArgin (mm)) `* 6000 
/ 254&]
[s5;i150;O9;    `= ( 210 `- 25 `- 10 ) `* 6000 / 254 `= [* 4133]&]
[s5; cy `= (PageHeight `- TopMargin `- BottomMargin (mm)) `* 6000 
/ 254&]
[s5;    `= ( 297 `- 10 `-10 ) `* 6000 / 254   `= [* 6543]&]
[s5; For Landscape orientation use [*  ]Report.SetPageSize[*  ][*@(0.0.255) ( 
cy, cx )][*  ]&]
[s5; [* Margin Calculation when printing to Printer ]  &]
[s5; This is My observation with some of the PCL Printers. May be 
different with your printer.&]
[s5; Checked with Ricoh, Minolta, Samsung laser printers.&]
[s5; &]
[s5; [_ Value 4 may be different for different printers.]&]
[s5; &]
[s5; [* Report Margins ( Report.Margins ( TopMargin, LeftMargin )]&]
[s5; &]
[s5;i150;O0; [* TopMargin] `= ( Top margin `- 4 ) `* 6000 / 254&]
[s5; -|-|`= ( 10 `- 4 ) `* 6000 / 254 `= [*@(0.0.255) 142]&]
[s5;i150;O0; [* LeftMargin] `= ( Left Margin `- 4 ) `* 6000 / 254&]
[s5; -|-|`= ( 25 `- 4 ) `* 6000 / 254 `= [*@(0.0.255) 496]&]
[s5; &]
[s5; With these settings you will get Correct margins and print area 
on the paper.&]
[s5; &]
[s5; [3 Getting Print area in the][+117  ][*@(0.0.255)+117 center of the 
printed page.]&]
[s5; Use the same calculation as above to get your Print Area.&]
[s5; Do not set Margins. Page margins to be `'NULL`'&]
[s5; Your document will be printed on center of the page&]
[s5; 
@@image:2562&3543
(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)
&]
[s0; ]]