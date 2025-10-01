#ifndef FLYBACKSTACKSYSTEMNOTIFIER_H
#define FLYBACKSTACKSYSTEMNOTIFIER_H

#include <QObject>

#include "fly-phone-dbus_global.h"

namespace FlyPhone {
class FlyBackStackSystemNotifierPrivate;
class FlyBackStackNotifier;

/*!
 * \brief The FlyBackStackSystemNotifier class
 *
 * Use this class to process back button presses.
 *
 * \code
 * using namespace FlyPhone;
 * FlyBackStackSystemNotifier bssystem;
 * QObject::connect(&bssystem, &FlyBackStackSystemNotifier::stackRegistered, [](FlyBackStackNotifier* stack) {
 *     stack->setDefaultAction(BackAction::DoNothing);
 *
 *     QObject::connect(stack, &FlyBackStackNotifier::backPressed, []() {
 *         // do something
 *         qApp->quit();
 *     });
 * });
 *
 * bssystem.registerStack();
 * // ...
 * app.exec();
 * \endcode
 *
 * \note Default behavior if not registered or registered but don't change anything is to send CloseEvent.
 */
class FLYPHONEDBUSSHARED_EXPORT FlyBackStackSystemNotifier : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FlyBackStackSystemNotifier)
    QScopedPointer<FlyBackStackSystemNotifierPrivate> const d_ptr;

public:
    explicit FlyBackStackSystemNotifier(QObject *parent = nullptr);
    ~FlyBackStackSystemNotifier();

    /*!
     * \brief Register app (subscribe to back button press).
     */
    void registerStack();

signals:
    /*!
     * \brief Registered backstack
     * \param stack Can be null if invalid via some errors
     */
    void stackRegistered(FlyBackStackNotifier* stack);
};

}

#endif // FLYBACKSTACKSYSTEMNOTIFIER_H
