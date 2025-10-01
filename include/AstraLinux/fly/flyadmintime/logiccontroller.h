#ifndef LOGICCONTROLLER_H
#define LOGICCONTROLLER_H

#include "libflyadmintime_global.h"

#include <QObject>
#include <QMap>
#include "enumnames.h"
#include "dbusservice.h"

class AbstractServiceClass;
class AbstractDataClass;

class FLYADMINTIME_EXPORT LogicController : public QObject
{    
    Q_OBJECT
public:
    explicit LogicController(QObject *parent = nullptr);
    virtual ~LogicController();

    const QStringList getListServersToSync() const;
    void reloadSystemState();
    void saveAddrAndSyncNow(const QString &addr);
    void syncNow(const QString &addr);

    void startServiceByDbus(const QString &serviceName);
    void stopServiceByDbus(const QString &serviceName);
    void restartServiceByDbus(const QString &serviceName);
    void enableServiceByDbus(const QString &serviceName);
    void disableServiceByDbus(const QString &serviceName);
    void moveAndRestartActive(const QString &sourceName, const QString &destName);
    const QList<QWidget *> getInstalledServiceWidgets() const;
    QWidget * getActiveServiceWidget();

    const QMap <PackageIndex, AbstractServiceClass *> getInstalledServicesMap() const;
    const QStringList getNotInstalledServicesNames() const;

    const QString getActiveServiceName() const;
    void setCurrentSelectedService(int newIndexInTree);
    ServiceState getCurrentServiceState();
    ServiceEnable getCurrentServiceEnabling();

    const QString getStartResult();

    void openMainSettingsDialog() const;
    bool isMainSettingsAvailable() const;
    bool isServiceRemovable() const;
    const QString saveCurrentConfig() const;
    const QString createConf(PackageIndex index) const;
    const QString getCurrentConfTarget() const;

    void setMainWindowId(const WId w_id);
    void getSysAlive();
    bool tryToInstallService(const QString &servName) const;
    bool tryToUninstallCurrentService() const;
    const QStringList getConflictedServices(const QString &servName) const;
    const QString getPackageNameByServiceName(const QString &servName) const;

public slots:
    void applyConfig();

private:
    QString m_syncNowAddress;
    QMap <PackageIndex, AbstractServiceClass *> m_servicesMap;
    QMap <PackageIndex, AbstractDataClass *> m_dataMap;
    QMap <PackageIndex, bool> m_serviceChangedList;
    QList <PackageIndex> m_installedPackageList;
    PackageIndex m_currentActiveService = PackageIndex::END_OF_LIST;
    PackageIndex m_currentSelectedService = PackageIndex::END_OF_LIST;
    QMultiMap<QString, QStringList> m_plainConfMap;
    QMap <QString, QMultiMap<QString, QStringList>> m_sectionedConfMap;
    QWidget *m_blankWidget = nullptr;
    VersionOfSystemd m_sysdVersion = VersionOfSystemd::v232;
    FlyAdmin::DbusService m_dbus;
    bool m_loginctlAlive = true;

    /*
     * other variables above
     */

    /*
     * other functions below
     */

    void fillServicesList();
    bool writeConf(PackageIndex index);
    void getAllServiceStates();
    void parseConfAndFillWidget(PackageIndex index) const;
    void readConfAndFillWidgets();
    AbstractServiceClass * createWidgetByPackageIndex(PackageIndex index);
    AbstractDataClass * createDataClassByPackageIndex(PackageIndex index);
    ServiceState getServiceStateFromSystem(PackageIndex index);
    ServiceEnable getServiceEnablingFromSystem(PackageIndex index);

    static const QMultiMap<QString, QStringList> readPlainConfFile(const QString &path);
    static const QMap<QString, QMultiMap<QString, QStringList> > readSectionedConfFile(const QString &path);

    void loadSettings();

private slots:
    void serviceChanged(PackageIndex index);

signals:
    void confWrited();
    void confWriteError();
    void oneServiceChanged();
    void syncError(QString text);
    void syncOK();
    void loginctlUnavailable();
};

#endif // LOGICCONTROLLER_H
