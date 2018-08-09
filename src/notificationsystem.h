#ifndef NOTIFICATIONSYSTEM_H
#define NOTIFICATIONSYSTEM_H

#include <QObject>


namespace EM {

class NotificationSystemPrivate;

class NotificationSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int backendType READ backendTypeI WRITE setBackendType NOTIFY backendTypeChanged)

public:
    enum BackendType {
        None = 0,
        TrayIcon = 1,
        FreedesktopDbus = 2,
        AndroidNative = 3
    };

public:
    explicit NotificationSystem(QObject *parent, BackendType backendType);
    ~NotificationSystem() override;

    BackendType backendType() const;
    int backendTypeI() const;

public Q_SLOTS:
    void setBackend(BackendType backendType);
    void setBackendType(int typ);
    void notify(const QString &title, const QString &message, bool forceDisplay = false);

Q_SIGNALS:
    void backendTypeChanged();
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
