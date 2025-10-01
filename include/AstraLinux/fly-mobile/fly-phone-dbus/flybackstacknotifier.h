#ifndef FLYBACKSTACKNOTIFIER_H
#define FLYBACKSTACKNOTIFIER_H

#include <QObject>
#include <QVariant>

#include "fly-phone-dbus_global.h"

enum class BackAction { CloseEvent, SigTerm, SigStop, DoNothing };
Q_DECLARE_METATYPE(BackAction)

namespace FlyPhone {
class FlyBackStackNotifierPrivate;

/*!
 * \brief The FlyBackStackNotifier class
 *
 * Used to control application behavior on back button press.
 *
 * Strategies to use it:
 * 1. Set default action to DoNothing and process all presses by yourself (including app quiting).
 * For example you can minimize app instead of quitting.
 * 2. Use push/pop to work with stack. Every press will pop data from stack (if any).
 */
class FLYPHONEDBUSSHARED_EXPORT FlyBackStackNotifier : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FlyBackStackNotifier)
    QScopedPointer<FlyBackStackNotifierPrivate> const d_ptr;

    Q_PROPERTY(BackAction defaultAction READ defaultAction WRITE setDefaultAction NOTIFY defaultActionChanged)
    Q_PROPERTY(int  size                READ size    NOTIFY sizeChanged)
    Q_PROPERTY(bool isEmpty             READ isEmpty NOTIFY sizeChanged)
public:
    explicit FlyBackStackNotifier(const QString &objectPath, QObject *parent = nullptr);
    ~FlyBackStackNotifier();

    bool valid() const;

    /*!
     * \brief Default action for back button press when stack is empty.
     * \param action BackAction enum\n
     * CloseEvent - Default. Sending window close event.\n
     * SigTerm    - Sending SIGTERM signal. If app will be alive after some time (default 5s) SIGKILL will be sent.\n
     * SigStop    - Sending SIGSTOP signal. App will be minimized. SIGCONT will be sent on raise.\n
     * DoNothing  - Just send FlyBackStackNotifier::backPressed.
     */
    void setDefaultAction(BackAction action);
    BackAction defaultAction() const;

    int  size() const;
    bool isEmpty() const;

public slots:
    /*!
     * \brief Pops data from stack. Pops invalid variant if stack is empty.
     */
    QVariant pop();
    /*!
     * \brief Pushes \p variant to stack.
     */
    void push(const QVariant &variant = {});

signals:
    void sizeChanged(int size);
    void defaultActionChanged(BackAction defaultAction);

    /*!
     * \brief Emits when back button was pressed (not always).
     *
     * If any data was pushed to stack then it will be popped and sent with this signal.
     * If stack is empty default action will be triggered.
     *
     * \warning signal will not be sent if stack is empty and default action isn't DoNothing!
     * \param variant Will be invalid if stack is empty
     */
    void backPressed(const QVariant &variant);
};

}

#endif // FLYBACKSTACKNOTIFIER_H
