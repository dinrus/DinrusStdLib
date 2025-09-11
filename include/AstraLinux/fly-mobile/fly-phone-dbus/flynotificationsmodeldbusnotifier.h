#ifndef FLYNOTIFICATIONSMODELDBUSNOTIFIER_H
#define FLYNOTIFICATIONSMODELDBUSNOTIFIER_H

#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlyNotificationsModelDbusNotifierPrivate;
class FlyNotificationDbusNotifier;

/*!
 * \brief The FlyNotificationsModelDbusNotifier class
 *
 * Used to work with user-visible notification system. It using dbus calls to collaborate with notification model in fly-launcher.
 * \author YatmanovDA <support@rusbitech.ru>
 * \date 12.10.2015
 * \todo Move type enum from fly-launcher here or to fly-phone-db-client
 */
class FLYPHONEDBUSSHARED_EXPORT FlyNotificationsModelDbusNotifier : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FlyNotificationsModelDbusNotifier)
    QScopedPointer<FlyNotificationsModelDbusNotifierPrivate> const d_ptr;

public:
    explicit FlyNotificationsModelDbusNotifier(QObject *parent = 0);
    ~FlyNotificationsModelDbusNotifier();

public slots:
    /*!
     * \brief Add notification to model
     *
     * Notifications model is used in Notifications panel, Status bar and Lockscreen.
     * \param type Notification type\n
     * 0 - default text type\n
     * 1 - call type\n
     * 2 - message(sms) type\n
     * 3 - alarm type\n
     * 4 - email type\n
     * 5 - custom type\n
     * \param removable Can be removed by user (it can't be saved if it's not removable)
     * \param save Save notification (after reboot it will be restored, if it wasn't removed)
     * \param dataMap Notification properties\n
     * For custom widgets:
     * \code { "customComponentPath": <Path to qml file>, \*... any data you want*\ } \endcode
     * For all standart widgets:
     * \code { "caption": <Main title>, "description": <Additional description>, "icon": <Icon path>, "datetime": <ISO date>, "sound": <Sound path>, "click": <App path> } \endcode
     * For message, call types:
     * \code { "phone": <Caller phone> } \endcode
     * \return Notification
     * \warning Blocking dbus call.
     */
    FlyNotificationDbusNotifier* addNotification(const int &type, const bool &removable, const bool &save, const QVariantMap &dataMap);

    /*!
     * \brief Get notification from model
     * \param id
     * \return Notification
     */
    FlyNotificationDbusNotifier* getNotification(const int &id);

    /*!
     * \brief Remove notification from model
     *
     * If notification is not removable, only this function can remove it.\n
     * If it's not saved then fly-launcher restart remove it too.
     * \param id Notification id
     */
    void removeNotification(const int &id);

    /*!
     * \brief Remove all message type notifications with \p phone
     *
     * Used when user opened or viewed message chain.
     * If there is any new messages you should call it to remove all connected notifications from model.
     * \param phone caller phone number
     */
    void removeMessageNotificationsWithPhone(const QString &phone);
};

}

#endif // FLYNOTIFICATIONSMODELDBUSNOTIFIER_H
