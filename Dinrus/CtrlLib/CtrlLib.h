#ifndef CTRLLIB_H
#define CTRLLIB_H

#include <Dinrus/CtrlCore/CtrlCore.h>

#ifdef Status //X11 defines this, likely name clash...
#undef Status
#endif

namespace Upp {

INITIALIZE(CtrlLib);

#define IMAGECLASS CtrlImg
#define IMAGEFILE <Dinrus/CtrlLib/Ctrl.iml>
#include <Dinrus/Draw/iml_header.h>

#define IMAGECLASS CtrlsImg
#define IMAGEFILE <Dinrus/CtrlLib/Ctrls.iml>
#include <Dinrus/Draw/iml_header.h>

class Bar;

#include <Dinrus/CtrlLib/LabelBase.h>
#include <Dinrus/CtrlLib/DisplayPopup.h>
#include <Dinrus/CtrlLib/StaticCtrl.h>
#include <Dinrus/CtrlLib/PushCtrl.h>
#include <Dinrus/CtrlLib/MultiButton.h>
#include <Dinrus/CtrlLib/ScrollBar.h>
#include <Dinrus/CtrlLib/HeaderCtrl.h>
#include <Dinrus/CtrlLib/EditCtrl.h>
#include <Dinrus/CtrlLib/AKeys.h>
#include <Dinrus/CtrlLib/Bar.h>
#include <Dinrus/CtrlLib/StatusBar.h>
#include <Dinrus/CtrlLib/TabCtrl.h>
#include <Dinrus/CtrlLib/DlgColor.h>
#include <Dinrus/CtrlLib/ArrayCtrl.h>
#include <Dinrus/CtrlLib/DropChoice.h>
#include <Dinrus/CtrlLib/TreeCtrl.h>
#include <Dinrus/CtrlLib/Splitter.h>
#include <Dinrus/CtrlLib/RichText.h>
#include <Dinrus/CtrlLib/TextEdit.h>
#include <Dinrus/CtrlLib/SliderCtrl.h>
#include <Dinrus/CtrlLib/ColumnList.h>
#include <Dinrus/CtrlLib/DateTimeCtrl.h>
#include <Dinrus/CtrlLib/SuggestCtrl.h>

#define  LAYOUTFILE <Dinrus/CtrlLib/Ctrl.lay>
#include <Dinrus/CtrlCore/lay.h>

#include <Dinrus/CtrlLib/Progress.h>
#include <Dinrus/CtrlLib/FileSel.h>
#include <Dinrus/CtrlLib/CtrlUtil.h>
#include <Dinrus/CtrlLib/Lang.h>

#include <Dinrus/CtrlLib/Ch.h>

}

#endif
