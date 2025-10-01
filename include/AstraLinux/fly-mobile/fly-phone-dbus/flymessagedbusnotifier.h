#ifndef FLYMESSAGEDBUSNOTIFIER_H
#define FLYMESSAGEDBUSNOTIFIER_H

#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlyMessageDbusNotifierPrivate;

class FLYPHONEDBUSSHARED_EXPORT FlyMessageDbusNotifier : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.fly.phone.launcher.FlyMessageDbusNotifier")
    Q_DECLARE_PRIVATE(FlyMessageDbusNotifier)
    QScopedPointer<FlyMessageDbusNotifierPrivate> const d_ptr;

public:
    explicit FlyMessageDbusNotifier(QObject *parent = 0);
    ~FlyMessageDbusNotifier();

public slots:
    void emitEmailRecieved(const QString &id, const QString &from, const QString &subject);
    void emitEmailsRecieved(const int &count, const QString &message);
    void emitEmailError(const QString &message);

    void emitIncomingSms(const QString &phone, const QString &text);
    void emitOutgoingSms(const QString &phone, const QString &text, const QString &id);
    void emitOutgoingSmsStatus(const QString &status, const QString &id);

signals:
    void emailRecieved(const QString &id, const QString &from, const QString &subject);
    void emailsRecieved(const int &count, const QString &message);
    void emailError(const QString &message);

    void incomingSms(const QString &phone, const QString &text);
    void outgoingSms(const QString &phone, const QString &text, const QString &id);
    void outgoingSmsStatus(const QString &phone, const QString &id);
};

}

#endif // FLYMESSAGEDBUSNOTIFIER_H
