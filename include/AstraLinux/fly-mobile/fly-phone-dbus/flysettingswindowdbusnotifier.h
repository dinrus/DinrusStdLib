#ifndef FLYSETTINGSWINDOWDBUSNOTIFIER_H
#define FLYSETTINGSWINDOWDBUSNOTIFIER_H

#include <QDBusVariant>
#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlySettingsWindowDbusNotifierPrivate;

/*!
 * \brief The FlySettingsWindowDbusNotifier class
 *
 * Use this class to open global settings window. It sends dbus call to fly-launcher.
 * \author YatmanovDA <support@rusbitech.ru>
 * \date 22.10.2015
 */
class FLYPHONEDBUSSHARED_EXPORT FlySettingsWindowDbusNotifier : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FlySettingsWindowDbusNotifier)
    QScopedPointer<FlySettingsWindowDbusNotifierPrivate> const d_ptr;
#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
    Q_ENUMS(SettingGroups)
#endif

public:
    /*!
     * \brief The SettingGroups enum
     */
    enum SettingGroups { AllGroup, WifiGroup, BluetoothGroup, CallsGroup, SmsGroup, SimGroup,
                         LockscreenGroup, DesktopGroup, ScreenGroup, SoundGroup, AppsGroup,
                         UsersGroup, DecorationGroup, LanguageGroup, DatetimeGroup, HelpGroup, LanGroup };
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    Q_ENUM(SettingGroups)
#endif

    explicit FlySettingsWindowDbusNotifier(QObject *parent = 0);
    ~FlySettingsWindowDbusNotifier();

public slots:
    /*!
     * \brief Show global settings window, starting with \p group.
     *
     * All applications that using shared settings should call this.
     * \param group FlySettingsWindowDbusNotifier::SettingGroups
     */
    void showSettingsWindow(const SettingGroups &group);
    /*!
     * \brief Convert \p group to string.
     *
     * It's not just string representation. It's a group name from launcher settings in QSettings.
     * Sometimes it's easier to compare string groups.
     * \param group FlySettingsWindowDbusNotifier::SettingGroups
     * \return group name
     * \warning Blocking dbus call.
     */
    QString settingsGroupToString(const SettingGroups &group);
    /*!
     * \brief Convert \p group name to FlySettingsWindowDbusNotifier::SettingGroups.
     *
     * \see FlySettingsWindowDbusNotifier::settingsGroupToString
     * \param group name
     * \return FlySettingsWindowDbusNotifier::SettingGroups
     * \warning Blocking dbus call.
     */
    SettingGroups settingsGroupToInt(const QString &group);
signals:
    /*!
     * \brief Global settings changed.
     *
     * This signal emitted every time when any launcher settings changed.
     * \param group FlySettingsWindowDbusNotifier::SettingGroups
     * \param key QSettings key
     * \param value New value
     */
    void settingsChanged(const SettingGroups &group, const QString &key, const QVariant &value);
};

}

#endif // FLYSETTINGSWINDOWDBUSNOTIFIER_H
