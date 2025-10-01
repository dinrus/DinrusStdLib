#ifndef FLYTASKDBUSNOTIFIER_H
#define FLYTASKDBUSNOTIFIER_H

#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlyTaskDbusNotifierPrivate;

class FlyTaskDbusNotifier : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.fly.phone.launcher.FlyTaskDbusNotifier")
    Q_DECLARE_PRIVATE(FlyTaskDbusNotifier)
    QScopedPointer<FlyTaskDbusNotifierPrivate> const d_ptr;

public:
    explicit FlyTaskDbusNotifier(QObject *parent = 0);
    ~FlyTaskDbusNotifier();

public slots:
    void emitTaskTriggered(const QString &id, const QString &message);
    void emitTaskCreated(const QString &id);
    void emitTaskUpdated(const QString &id);
    void emitTaskRemoved(const QString &id);

signals:
    void taskTriggered(const QString &id, const QString &message);
    void taskCreated(const QString &id);
    void taskUpdated(const QString &id);
    void taskRemoved(const QString &id);
};

}

#endif // FLYTASKDBUSNOTIFIER_H
