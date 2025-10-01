#ifndef FLYSELECTIONDBUSNOTIFIER_H
#define FLYSELECTIONDBUSNOTIFIER_H

#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlySelectionDbusNotifierPrivate;

class FLYPHONEDBUSSHARED_EXPORT FlySelectionDbusNotifier : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.fly.phone.launcher.FlySelectionDbusNotifier")
    Q_DECLARE_PRIVATE(FlySelectionDbusNotifier)
    QScopedPointer<FlySelectionDbusNotifierPrivate> const d_ptr;

public:
    explicit FlySelectionDbusNotifier(QObject *parent = 0);
    ~FlySelectionDbusNotifier();

public slots:
    void emitVisibleChanged(bool visible);
    void emitCut();
    void emitCopy();
    void emitPaste();
    void emitSelectAll();

signals:
    void visibleChanged(bool visible);
    void cut();
    void copy();
    void paste();
    void selectAll();
};

}

#endif // FLYSELECTIONDBUSNOTIFIER_H
