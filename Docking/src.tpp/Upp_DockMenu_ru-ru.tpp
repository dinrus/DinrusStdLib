topic "Класс DockMenu";
[i448;a25;kKO9; $$1,0#37138531426314131252341829483380:structitem]
[l288;2 $$2,0#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[a83;*R6 $$4,0#31310162474203024125188417583966:caption]
[i448;a25;kKO9;*@(64)2 $$5,0#37138531426314131252341829483370:item]
[H6;0 $$6,0#05600065144404261032431302351956:begin]
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s4;%RU-RU Класс DockMenu&]
[s1;:Upp`:`:DockMenu`:`:class: [@(0.0.255) class]_[* DockMenu]&]
[s3;* &]
[s2;%RU-RU The DockMenu class provides access to all of the standard 
menus used throughout the framework. It provides the means to 
create customized window management menus according to you needs.&]
[s3; &]
[s6; &]
[s5; void_[@0 Set]([^`:`:DockWindow^ DockWindow]_`&[@3 dockwindow])&]
[s2; [%RU-RU   Sets the owning ][^`:`:DockWindow^ DockWindow]. This function 
[* must] be called before any menu generating functions.&]
[s3; &]
[s6; &]
[s5; void_[@0 LayoutListMenu]([^`:`:Bar^ Bar]_`&[@3 bar])&]
[s2;%RU-RU   Adds a menu with a sub`-menu list of layouts to [%-*@3 bar] 
to allow quick layout changes.&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:GroupListMenu`(`:`:Bar`&`): [@(0.0.255) void]_[@0 GroupListMenu]([^`:`:Bar^ B
ar]_`&[@3 bar])&]
[s2;%RU-RU   Adds a list of all available groups to [%-*@3 bar] with 
a sub`-menu of [^topic`:`/`/Docking`/src`/Utility`$en`-us`#`:`:DockMenu`:`:GroupWindowsMenu`(`:`:Bar`&`,`:`:String`)^ G
roupWindowsMenu] for each group.&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:WindowListMenu`(`:`:Bar`&`,`:`:String`): [@(0.0.255) void]_[@0 Wind
owListMenu]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:String^ String]_[@3 group])&]
[s2; [%RU-RU   Adds a list of all DockableCtrls in ][*@3 group][%RU-RU  
to ][*@3 bar] with sub`-menus of [^topic`:`/`/Docking`/src`/Utility`$en`-us`#`:`:DockMenu`:`:WindowMenu`(`:`:Bar`&`,`:`:DockableCtrl`*`)^ W
indowMenu] for changing their states/positions.&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:GroupMenu`(`:`:Bar`&`,`:`:String`): [@(0.0.255) void]_[@0 GroupMenu
]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:String^ String]_[@3 group])&]
[s2; [%RU-RU   Adds a group actions menu for ][*@3 group][%RU-RU  to ][*@3 bar].&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:GroupWindowsMenu`(`:`:Bar`&`,`:`:String`): [@(0.0.255) void]_[@0 Gr
oupWindowsMenu]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:String^ String]_[@3 group])&]
[s2;%RU-RU   Adds [^topic`:`/`/Docking`/src`/Utility`$en`-us`#`:`:DockMenu`:`:GroupMenu`(`:`:Bar`&`,`:`:String`)^ G
roupMenu ]and [^topic`:`/`/Docking`/src`/Utility`$en`-us`#`:`:DockMenu`:`:WindowListMenu`(`:`:Bar`&`,`:`:String`)^ W
indowListMenu ]menu to [%-*@3 bar] for [%-*@3 group].&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:GroupDockMenu`(`:`:Bar`&`,`:`:String`): [@(0.0.255) void]_[@0 Group
DockMenu]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:String^ String]_[@3 group])&]
[s2;%RU-RU   Adds group docking options to [%-*@3 bar] for [%-*@3 group].&]
[s0;%RU-RU &]
[s5;K:`:`:DockMenu`:`:GroupHideMenu`(`:`:Bar`&`,`:`:String`): [@(0.0.255) void]_[@0 Group
HideMenu]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:String^ String]_[@3 group])&]
[s2;%RU-RU   Adds group auto`-hide options to [%-*@3 bar] for [%-*@3 group].&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:GroupTabDockMenu`(`:`:Bar`&`,`:`:String`): [@(0.0.255) void]_[@0 Gr
oupTabDockMenu]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:String^ String]_[@3 group])&]
[s2;%RU-RU   Adds group tabify`-docking options to [%-*@3 bar] for 
[%-*@3 group].&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:WindowMenuNoClose`(`:`:Bar`&`,`:`:DockableCtrl`*`): [@(0.0.255) v
oid]_[@0 WindowMenuNoClose]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:DockableCtrl^ DockableCtrl]_
`*[@3 dc])&]
[s2;%RU-RU   As [^topic`:`/`/Dinrus`/Docking`/src`/Upp`_DockMenu`_ru`-ru`#`:`:DockMenu`:`:WindowMenu`(`:`:Bar`&`,`:`:DockableCtrl`*`)^ W
indowMenu], but without the `'close`' option.&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:WindowMenu`(`:`:Bar`&`,`:`:DockableCtrl`*`): [@(0.0.255) void]_[@0 W
indowMenu]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:DockableCtrl^ DockableCtrl]_`*[@3 dc])&]
[s2;%RU-RU   Adds a standard window menu to [%-*@3 bar] for [%-*@3 dc].&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:WindowDockMenu`(`:`:Bar`&`,`:`:DockableCtrl`*`): [@(0.0.255) void
]_[@0 WindowDockMenu]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:DockableCtrl^ DockableCtrl]_`*[@3 d
c])&]
[s2;%RU-RU   Adds a window docking menu to [%-*@3 bar] for [%-*@3 dc] 
&]
[s3; &]
[s6; &]
[s5;K:`:`:DockMenu`:`:WindowHideMenu`(`:`:Bar`&`,`:`:DockableCtrl`*`): [@(0.0.255) void
]_[@0 WindowHideMenu]([^`:`:Bar^ Bar]_`&[@3 bar], [^`:`:DockableCtrl^ DockableCtrl]_`*[@3 d
c])&]
[s2;%RU-RU   Adds a window auto`-hide menu to [%-*@3 bar] for [%-*@3 dc] 
&]
[s3; &]
[s6; &]
[s5;K:Upp`:`:DockMenu`:`:`:`:DockMenu`(Upp`:`:DockWindow`*`): [@0 DockMenu]([^`:`:DockWindow^ D
ockWindow]_`*[@3 dockwindow]_`=_NULL)&]
[s2;%RU-RU   [%- Class Contructor. Optionally also set the owning ][%-^`:`:DockWindow^ Do
ckWindow][%- .] &]
[s3; &]
[s6; ]]