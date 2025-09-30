#ifndef CTRLLIB_H
#define CTRLLIB_H

#include <drx/CtrlCore/CtrlCore.h>

#ifdef Status //X11 defines this, likely name clash...
#undef Status
#endif

namespace drx {

INITIALIZE(CtrlLib);

#define IMAGECLASS CtrlImg
#define IMAGEFILE <drx/CtrlLib/Ctrl.iml>
#include <drx/Draw/iml_header.h>

#define IMAGECLASS CtrlsImg
#define IMAGEFILE <drx/CtrlLib/Ctrls.iml>
#include <drx/Draw/iml_header.h>

class Bar;

#include <drx/CtrlLib/LabelBase.h>
#include <drx/CtrlLib/DisplayPopup.h>
#include <drx/CtrlLib/StaticCtrl.h>
#include <drx/CtrlLib/PushCtrl.h>
#include <drx/CtrlLib/MultiButton.h>
#include <drx/CtrlLib/ScrollBar.h>
#include <drx/CtrlLib/HeaderCtrl.h>
#include <drx/CtrlLib/EditCtrl.h>
#include <drx/CtrlLib/AKeys.h>
#include <drx/CtrlLib/Bar.h>
#include <drx/CtrlLib/StatusBar.h>
#include <drx/CtrlLib/TabCtrl.h>
#include <drx/CtrlLib/DlgColor.h>
#include <drx/CtrlLib/ArrayCtrl.h>
#include <drx/CtrlLib/DropChoice.h>
#include <drx/CtrlLib/TreeCtrl.h>
#include <drx/CtrlLib/Splitter.h>
#include <drx/CtrlLib/RichText.h>
#include <drx/CtrlLib/TextEdit.h>
#include <drx/CtrlLib/SliderCtrl.h>
#include <drx/CtrlLib/ColumnList.h>
#include <drx/CtrlLib/DateTimeCtrl.h>
#include <drx/CtrlLib/SuggestCtrl.h>

#define  LAYOUTFILE <drx/CtrlLib/Ctrl.lay>
#include <drx/CtrlCore/lay.h>

#include <drx/CtrlLib/Progress.h>
#include <drx/CtrlLib/FileSel.h>
#include <drx/CtrlLib/CtrlUtil.h>
#include <drx/CtrlLib/Lang.h>

#include <drx/CtrlLib/Ch.h>
}

#include <drx/CtrlLib/BlueBar/BlueBar.h>

#endif
