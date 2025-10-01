#ifndef FLYVKBDDBUSNOTIFIER_H
#define FLYVKBDDBUSNOTIFIER_H

#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlyVkbdDbusNotifierPrivate;

class FLYPHONEDBUSSHARED_EXPORT FlyVkbdDbusNotifier : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.fly.phone.launcher.FlyVkbdDbusNotifier")
    Q_DECLARE_PRIVATE(FlyVkbdDbusNotifier)
    QScopedPointer<FlyVkbdDbusNotifierPrivate> const d_ptr;

public:
    explicit FlyVkbdDbusNotifier(QObject *parent = 0);
    ~FlyVkbdDbusNotifier();

public slots:
    void emitHide();
    void emitVisibleChanged(const bool &visible);

signals:
    void hide();
    void visibleChanged(const bool &visible);
};

}

#endif // FLYVKBDDBUSNOTIFIER_H
