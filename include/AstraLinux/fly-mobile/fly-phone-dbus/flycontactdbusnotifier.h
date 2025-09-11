#ifndef FLYCONTACTDBUSNOTIFIER_H
#define FLYCONTACTDBUSNOTIFIER_H

#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlyContactDbusNotifierPrivate;

class FLYPHONEDBUSSHARED_EXPORT FlyContactDbusNotifier : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.fly.phone.launcher.FlyContactDbusNotifier")
    Q_DECLARE_PRIVATE(FlyContactDbusNotifier)
    QScopedPointer<FlyContactDbusNotifierPrivate> const d_ptr;

public:
    explicit FlyContactDbusNotifier(QObject *parent = 0);
    ~FlyContactDbusNotifier();

public slots:
    void emitContactCreated(const QString &id);
    void emitContactDataUpdated(const QString &id);
    void emitContactPhotoUpdated(const QString &id);
    void emitContactRemoved(const QString &id);

signals:
    void contactCreated(const QString &id);
    void contactDataUpdated(const QString &id);
    void contactPhotoUpdated(const QString &id);
    void contactRemoved(const QString &id);
};

}

#endif // FLYCONTACTDBUSNOTIFIER_H
