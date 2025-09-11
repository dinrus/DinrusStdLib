#ifndef FLYCALLDBUSNOTIFIER_H
#define FLYCALLDBUSNOTIFIER_H

#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlyCallDbusNotifierPrivate;

class FLYPHONEDBUSSHARED_EXPORT FlyCallDbusNotifier : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.fly.phone.launcher.FlyCallDbusNotifier")
    Q_DECLARE_PRIVATE(FlyCallDbusNotifier)
    QScopedPointer<FlyCallDbusNotifierPrivate> const d_ptr;

public:
    explicit FlyCallDbusNotifier(QObject *parent = 0);
    ~FlyCallDbusNotifier();

public slots:
    void emitCallChanged(const bool &incoming); // incoming, phone, ... TODO
    void emitCallAddedToDb(const int &id);
    void emitStatusChanged(const QString &status);
    void emitDial(const QString &phone);

signals:
    void callChanged(const bool &incoming); // incoming, phone, ...
    void callAddedToDb(const int &id);
    void statusChanged(const QString &status);
    void dial(const QString &phone);
};

}

#endif // FLYCALLDBUSNOTIFIER_H
