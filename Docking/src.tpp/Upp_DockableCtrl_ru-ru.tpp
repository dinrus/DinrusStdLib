topic "Класс DockableCtrl";
[i448;a25;kKO9; $$1,0#37138531426314131252341829483380:structitem]
[l288;2 $$2,0#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[a83;*R6 $$4,0#31310162474203024125188417583966:caption]
[i448;a25;kKO9;*@(64)2 $$5,0#37138531426314131252341829483370:item]
[0 $$6,6#38761573AFF0740B8CBB91A3F0F36C3C:begin]
[H6;0 $$7,0#05600065144404261032431302351956:begin]
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s4;%RU-RU Класс DockableCtrl&]
[s2;l0;i64;%RU-RU The DockableCtrl class provide a common container 
and interface for Ctrl`'s that wish to be managed by the Docking 
framework. It is deliberately light`-weight but provides methods 
for:&]
[s2;l0;i150;O3;%RU-RU Setting size`-hints that allow the DockWindow 
to better size and place the control with the dock frames&]
[s2;l0;i150;O3;%RU-RU Property methods for setting an icon, title 
and window group&]
[s2;l0;i150;O3;%RU-RU A Chameleon style interface enabling a custom 
appearance of the docking handle (the frame that displays the 
title and icon)&]
[s2;l0;i150;O3;%RU-RU Status methods for determining it`'s current 
position in the framework/layout.&]
[s1;@(0.0.255) &]
[s1;:Upp`:`:DockableCtrl`:`:class: [@(0.0.255) class]_[* DockableCtrl]&]
[s2;%RU-RU The DockableCtrl class provide a common container and 
interface for Ctrl`'s that wish to be managed by the Docking 
framework. &]
[s3; &]
[s7; &]
[s5; [^`:`:Callback1^ Callback1]<Bar_`&>_[@0 WhenMenuBar]&]
[s2;%RU-RU   Executed when the dockable ctrl`'s window menu is shown. 
This when the menu button is pressed or a context menu is activated 
on the handle/title bar of the window.&]
[s3; &]
[s7; &]
[s5; virtual [@(0.0.255) void]_[@0 WindowMenu]([^`:`:Bar^ Bar]_`&[@3 bar])&]
[s2;%RU-RU   Override called when the window menu is shown. Default 
behaviour is to execute WhenMenuBar.&]
[s3; &]
[s7; &]
[s5; [@(0.0.255) const]_[^`:`:Image^ Image]_`&_[@0 GetIcon]()&]
[s2;%RU-RU   Returns the image assigned to the ctrl.&]
[s3; &]
[s7; &]
[s5; [^`:`:DockableCtrl^ DockableCtrl]_`&_[@0 Icon]([@(0.0.255) const]_[^`:`:Image^ Image]`&_
[@3 m])&]
[s2;%RU-RU   Sets the image assigned to the ctrl.&]
[s3; &]
[s7; &]
[s5; [^`:`:DockableCtrl^ DockableCtrl]_`&_[@0 Title]([@(0.0.255) const]_[@(0.0.255) char]_`*[@3 `_
title])&]
[s2;%RU-RU   Sets the ctrl`'s title.&]
[s0;%RU-RU &]
[s5;K:Upp`:`:DockableCtrl`:`:Title`(const`:`:WString`&`): [^`:`:DockableCtrl^ DockableC
trl]_`&_[@0 Title]([@(0.0.255) const]_[^`:`:WString^ WString]`&_[@3 `_title])&]
[s2;%RU-RU   Sets the ctrl`'s title.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:GetTitle`(`): [@(0.0.255) const]_[^`:`:WString^ WString]_`&_
[@0 GetTitle]()&]
[s2;%RU-RU   Returns the current title.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:SizeHint`(const`:`:Size`&`,const`:`:Size`&`,const`:`:Size`&`): [^`:`:DockableCtrl^ D
ockableCtrl]_`&_[@0 SizeHint]([@(0.0.255) const]_[^`:`:Size^ Size]_`&[@3 min], 
[@(0.0.255) const]_[^`:`:Size^ Size]_`&[@3 max]_`=_Null, [@(0.0.255) const]_[^`:`:Size^ Siz
e]_`&[@3 std]_`=_Null)&]
[s2;%RU-RU   Shortcut to setting minimum, standard and maximum size 
to [%-*@3 min] [%-*@3 max] and [%-*@3 std] respectively. If Null is 
passed the hint remains unchanged.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:SetMinSize`(`:`:Size`): [@(0.0.255) void]_[@0 SetMinSize]([^`:`:Size^ S
ize]_[@3 sz])&]
[s2; [%RU-RU   ]Sets the minimum size for the ctrl to [*@3 sz].&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:SetMaxSize`(`:`:Size`): [@(0.0.255) void]_[@0 SetMaxSize]([^`:`:Size^ S
ize]_[@3 sz])&]
[s2;%RU-RU   Sets the maximum size of the ctrl to [%-*@3 sz].&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:SetStdSize`(`:`:Size`): [@(0.0.255) void]_[@0 SetStdSize]([^`:`:Size^ S
ize]_[@3 sz])&]
[s2;%RU-RU   Sets the best/standard size of the ctrl to [%-*@3 sz] 
&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:GetMinSize`(`)const: virtual [^`:`:Size^ Size]_[@0 GetMinSiz
e]()_[@(0.0.255) const]&]
[s2;%RU-RU   Returns the ctrl`'s minimum size.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:GetMaxSize`(`)const: virtual [^`:`:Size^ Size]_[@0 GetMaxSiz
e]()_[@(0.0.255) const]&]
[s2;%RU-RU   Returns the ctrl`'s maximum size.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:GetStdSize`(`)const: virtual [^`:`:Size^ Size]_[@0 GetStdSiz
e]()_[@(0.0.255) const]&]
[s2;%RU-RU   Returns the ctrl`'s best/standard size.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:SetStyle`(const`:`:DockableCtrl`:`:Style`&`): [^`:`:DockableCtrl^ D
ockableCtrl]_`&_[@0 SetStyle]([@(0.0.255) const]_[^`:`:DockableCtrl`:`:Style^ Style]_`&
[@3 s])&]
[s2;%RU-RU   Sets the current Chameleon style to [%-*@3 s]. [@3 s] should 
not be allowed to run out of scope or otherwise be destroyed 
by the calling function.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:GetStyle`(`): [@(0.0.255) const]_[^`:`:DockableCtrl`:`:Style^ S
tyle]_`&_[@0 GetStyle]()&]
[s2;%RU-RU   Returns th current Chameleon style.&]
[s3; &]
[s7; &]
[s5;i288;K:Upp`:`:DockableCtrl`:`:StyleDefault`(`): static [@(0.0.255) const]_[^`:`:DockableCtrl`:`:Style^ S
tyle]`&_[@0 StyleDefault]()&]
[s2;l0;i288;%RU-RU   Returns the default Chameleon style.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:StyleDefaultVert`(`): static [@(0.0.255) const]_[^`:`:DockableCtrl`:`:Style^ S
tyle]`&_[@0 StyleDefaultVert]()&]
[s2;%RU-RU   Returns the default Chameleon style for vertical handle.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:GetGroup`(`)const: virtual [@(0.0.255) const]_[^`:`:String^ S
tring]_`&[@0 GetGroup]()_[@(0.0.255) const]&]
[s2;%RU-RU   Returns the current group the ctrl is a member of, or 
Null if it is not in a group.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:SetGroup`(const`:`:String`&`): virtual 
[^`:`:DockableCtrl^ DockableCtrl]_`&[@0 SetGroup]([@(0.0.255) const]_[^`:`:String^ String
]_`&[@3 g])&]
[s2; [%RU-RU   Sets the ctrls group to ][*@3 g].&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:AllowDockAll`(`): [^`:`:DockableCtrl^ DockableCtrl]_`&_[@0 A
llowDockAll]()&]
[s2;%RU-RU   Allows docking for all alignments. Both the ctrl and 
the DockWindow managing it must allow docking.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:AllowDockNone`(`): [^`:`:DockableCtrl^ DockableCtrl]_`&_[@0 A
llowDockNone]()&]
[s2;%RU-RU   Denys docking for all alignments. Both the ctrl and 
the DockWindow managing it must allow docking.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:AllowDockLeft`(bool`): [^`:`:DockableCtrl^ DockableCtrl]_`&
_[@0 AllowDockLeft]([@(0.0.255) bool]_[@3 v]_`=_true)&]
[s2;%RU-RU  Allows/Denys docking permission for the [* left] frame. 
Both the ctrl and the DockWindow managing it must allow docking.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:AllowDockTop`(bool`): [^`:`:DockableCtrl^ DockableCtrl]_`&
_[@0 AllowDockTop]([@(0.0.255) bool]_[@3 v]_`=_true)&]
[s2;%RU-RU   Allows/Denys docking permission for the [* top] frame. 
Both the ctrl and the DockWindow managing it must allow docking.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:AllowDockRight`(bool`): [^`:`:DockableCtrl^ DockableCtrl]_
`&_[@0 AllowDockRight]([@(0.0.255) bool]_[@3 v]_`=_true)&]
[s2;%RU-RU   Allows/Denys docking permission for the [* right ]frame. 
Both the ctrl and the DockWindow managing it must allow docking.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:AllowDockBottom`(bool`): [^`:`:DockableCtrl^ DockableCtrl]_
`&_[@0 AllowDockBottom]([@(0.0.255) bool]_[@3 v]_`=_true)&]
[s2;%RU-RU   Allows/Denys docking permission for the [* bottom ]frame. 
Both the ctrl and the DockWindow managing it must allow docking.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:AllowDock`(int`,bool`): [^`:`:DockableCtrl^ DockableCtrl]_
`&_[@0 AllowDock]([@(0.0.255) int]_[@3 a], [@(0.0.255) bool]_[@3 v]_`=_true)&]
[s2;%RU-RU   Allows/Denys docking permission for the frame indicated 
by alignment [*@3 a]. Both the ctrl and the DockWindow managing 
it must allow docking.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:IsDockAllowed`(int`): [@(0.0.255) bool]_[@0 IsDockAllowed]([@(0.0.255) i
nt]_[@3 a])&]
[s2;%RU-RU   Returns true if docking is allowed for alignment [%-*@3 a]. 
Does not account for permissions set on the managing DockWindow.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:IsFloating`(`)const: [@(0.0.255) bool]_[@0 IsFloating]()_[@(0.0.255) c
onst]&]
[s2;%RU-RU   Returns true if the ctrl is currently floating (open 
as a normal child window).&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:IsDocked`(`)const: [@(0.0.255) bool]_[@0 IsDocked]()_[@(0.0.255) c
onst]&]
[s2;%RU-RU   Returns true if the window is docked at any alignment.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:IsAutoHide`(`)const: [@(0.0.255) bool]_[@0 IsAutoHide]()_[@(0.0.255) c
onst]&]
[s2;%RU-RU   Return true if the ctrl is currently being auto`-hidden.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:IsTabbed`(`)const: [@(0.0.255) bool]_[@0 IsTabbed]()_[@(0.0.255) c
onst]&]
[s2;%RU-RU   Returns true if the ctrl is currently displayed as a 
tab.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:IsHidden`(`)const: [@(0.0.255) bool]_[@0 IsHidden]()_[@(0.0.255) c
onst]&]
[s2;%RU-RU   Returns true if the ctrl is current hidden/closed.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:GetDockAlign`(`)const: [@(0.0.255) int]_[@0 GetDockAlign]()_
[@(0.0.255) const]&]
[s2;%RU-RU   Returns the alignment if the ctrl is docked.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:Highlight`(`): [@(0.0.255) void]_[@0 Highlight]()&]
[s2;%RU-RU   Displays a visual highlight on the ctrl to indicate 
it`'s position to the user.&]
[s3; &]
[s7; &]
[s5;K:Upp`:`:DockableCtrl`:`:`:`:DockableCtrl`(`): [@0 DockableCtrl]()&]
[s2;%RU-RU   Class Contructor.&]
[s0;%RU-RU &]
[s0;%RU-RU ]]