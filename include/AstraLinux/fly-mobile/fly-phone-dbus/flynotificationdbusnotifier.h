#ifndef FLYNOTIFICATIONDBUSNOTIFIER_H
#define FLYNOTIFICATIONDBUSNOTIFIER_H

#include <QDateTime>
#include <QObject>
#include <QVariantMap>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlyNotificationDbusNotifierPrivate;

class FLYPHONEDBUSSHARED_EXPORT FlyNotificationDbusNotifier : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FlyNotificationDbusNotifier)
    QScopedPointer<FlyNotificationDbusNotifierPrivate> const d_ptr;

    Q_PROPERTY(int id READ id)
    Q_PROPERTY(int type READ type)
    Q_PROPERTY(QDateTime creationDate READ creationDate)
    Q_PROPERTY(QDateTime changedDate READ changedDate NOTIFY changedDateChanged)
    Q_PROPERTY(bool save READ save WRITE setSave NOTIFY saveChanged)
    Q_PROPERTY(bool removable READ removable WRITE setRemovable NOTIFY removableChanged)
    Q_PROPERTY(QVariantMap dataMap READ dataMap WRITE setDataMap NOTIFY dataMapChanged)

public:
    explicit FlyNotificationDbusNotifier(QObject *parent = 0);
    explicit FlyNotificationDbusNotifier(const QString &objectPath, QObject *parent = 0);
    ~FlyNotificationDbusNotifier();

    bool valid() const;
    void setValid(bool valid);

    int id() const;

    int type() const;

    QDateTime creationDate() const;

    QDateTime changedDate() const;

    bool save() const;
    void setSave(bool save);

    bool removable() const;
    void setRemovable(bool removable);

    QVariantMap dataMap() const;
    void setDataMap(QVariantMap dataMap);

signals:
    void newData(const QString &data);
    void clicked();

    void saveChanged(const bool &save);
    void changedDateChanged(const QDateTime &date);
    void dataMapChanged(const QVariantMap &data);
    void removableChanged(const bool &removable);

public slots:
    void remove();
    void updateDataMap(const QVariantMap &dataMap);
    void deleteLater();
};

}

#endif // FLYNOTIFICATIONDBUSNOTIFIER_H
