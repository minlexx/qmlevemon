#ifndef NOTIFICATIONSYSTEM_H
#define NOTIFICATIONSYSTEM_H

#include <QObject>


namespace EM {

class NotificationSystemPrivate;

class NotificationSystem : public QObject
{
    Q_OBJECT

public:
    enum BackendType {
        TrayIcon = 0,
        FreedesktopDbus = 1,
        AndroidNative = 2
    };
public:
    explicit NotificationSystem(QObject *parent, BackendType backendType);
    ~NotificationSystem() override;

    BackendType backendType() const;

public Q_SLOTS:
    void setBackend(BackendType backendType);
    void notify(const QString &title, const QString &message);

Q_SIGNALS:
    void trayIconClicked();
    void trayIconRightClicked();
    void notificationMessageClicked();

private:
    NotificationSystemPrivate *dptr = nullptr;
    Q_DECLARE_PRIVATE_D(dptr, NotificationSystem)
    Q_DISABLE_COPY(NotificationSystem)
};

}

#endif // NOTIFICATIONSYSTEM_H
