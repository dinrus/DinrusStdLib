#ifndef FLYDATETIMEDBUSNOTIFIER_H
#define FLYDATETIMEDBUSNOTIFIER_H

#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlyDateTimeDbusNotifierPrivate;

class FLYPHONEDBUSSHARED_EXPORT FlyDateTimeDbusNotifier : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.fly.phone.launcher.FlyDateTimeDbusNotifier")
    Q_DECLARE_PRIVATE(FlyDateTimeDbusNotifier)
    QScopedPointer<FlyDateTimeDbusNotifierPrivate> const d_ptr;

public:
    explicit FlyDateTimeDbusNotifier(QObject *parent = 0);
    ~FlyDateTimeDbusNotifier();

public slots:
    void emitSystemDateTimeChanged();
    void emitTimeZoneChanged();
    void emitAlarmTriggered(const QString &name, const QString &sound, int delay);

    void emitTimerStarted(const QString &ms, const QString &sound);
    void emitTimerPaused(const QString &ms);
    void emitTimerResumed(const QString &ms);
    void emitTimerStopped();

signals:
    void systemDateTimeChanged();
    void timeZoneChanged();
    void alarmTriggered(const QString &name, const QString &sound, int delay);

    void timerStarted(const QString &ms, const QString &sound);
    void timerPaused(const QString &ms);
    void timerResumed(const QString &ms);
    void timerStopped();
};

}

#endif // FLYDATETIMEDBUSNOTIFIER_H
