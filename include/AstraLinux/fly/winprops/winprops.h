//
// C++ Interface: winprops
//
// Description:
//
//
// Author: Alexander Volkov <support@rusbitech.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WINPROPS_H
#define WINPROPS_H

#include <QString>
#include <QMetaType>

#include <QtGlobal>

#if defined(FLY_WINPROPS_LIBRARY)
#  define FLY_WINPROPS_EXPORT Q_DECL_EXPORT
#else
#  define FLY_WINPROPS_EXPORT Q_DECL_IMPORT
#endif

class QDBusArgument;

struct WinPropsPrivate;

struct FLY_WINPROPS_EXPORT WinProps
{
    WinProps();
    WinProps(const QString& str);
    WinProps(const QString& winName, const QString& winProps);

    QString toString() const;
    void clear();
    bool isValid() const;
    bool isNull() const;
    QString winName() const;
    void setWinName(const QString& winName);

    bool sticky;
    bool no_focus;
    bool no_title;
    bool no_border;
    bool no_border_edge;
    bool no_ctrlbtn;
    bool no_button1;
    bool no_button2;
    bool no_button3;
    bool no_tbutton;
    bool no_pager;
    QString small_img;
    QString large_img;
    bool close_soon;
    bool ontop;
    bool onbottom;
    bool init_maximize;
    bool init_fullscreen;
    bool init_minimize;
    bool focus_on_click;
    QString geometry;
    int shadow;
    int transparency;
    int onPage;
    bool noTray;
    bool forceDecor;
    bool trayIndicator;

private:
    WinPropsPrivate* d;
};

using WinPropsList = QList<WinProps>;

Q_DECLARE_METATYPE(WinProps)
Q_DECLARE_METATYPE(WinPropsList)

FLY_WINPROPS_EXPORT QDBusArgument &operator<<(QDBusArgument &arg, const WinProps &wp);
FLY_WINPROPS_EXPORT const QDBusArgument &operator>>(const QDBusArgument &arg, WinProps &wp);
FLY_WINPROPS_EXPORT QDBusArgument &operator<<(QDBusArgument &arg, const WinPropsList &wpl);
FLY_WINPROPS_EXPORT const QDBusArgument &operator>>(const QDBusArgument &arg, WinPropsList &wpl);

#endif
