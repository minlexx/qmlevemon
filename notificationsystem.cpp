#include <QLoggingCategory>
#include <QDebug>
#include "notificationsystem.h"

Q_LOGGING_CATEGORY(logNotifications, "evemon.notifications")

#ifdef Q_OS_ANDROID
// TODO: android includes?
#else
#include <QSystemTrayIcon>  // requires Qt Widgets library
#endif

namespace EM {


/**
 * @brief Abstract base class for notifications backend
 */
class NotificationsBackend: public QObject
{
    Q_OBJECT
public:
    NotificationsBackend() {}
    virtual ~NotificationsBackend() {}
    virtual void notify(const QString &title, const QString &message) = 0;

Q_SIGNALS:
    void trayIconClicked();
    void trayIconRightClicked();
    void notificationMessageClicked();

private:
    Q_DISABLE_COPY(NotificationsBackend)
};


#ifndef Q_OS_ANDROID
class NotificationsTrayIconBackend: public NotificationsBackend
{
    Q_OBJECT

public:
    NotificationsTrayIconBackend() {
        // TODO: create some icon?
        m_trayIcon = new QSystemTrayIcon(m_icon, this);
        m_trayIcon->show();
        QObject::connect(m_trayIcon, &QSystemTrayIcon::activated,
                         this, &NotificationsTrayIconBackend::onTrayIconActivated);
        QObject::connect(m_trayIcon, &QSystemTrayIcon::messageClicked,
                         this, &NotificationsTrayIconBackend::notificationMessageClicked);
    }

    void notify(const QString &title, const QString &message) override {
        m_trayIcon->showMessage(title, message, m_icon);
    }

public Q_SLOTS:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
        switch (reason) {
        case QSystemTrayIcon::Trigger: Q_EMIT trayIconClicked(); break;
        case QSystemTrayIcon::Context: Q_EMIT trayIconRightClicked(); break;
        default: break;
        }
    }

private:
    QSystemTrayIcon *m_trayIcon = nullptr;
    QIcon m_icon;
};
#endif


class NotificationSystemPrivate
{
public:
    NotificationSystemPrivate(NotificationSystem *parent): owner(parent) {}
    ~NotificationSystemPrivate();
    void setBackend(NotificationSystem::BackendType t);

public:
    NotificationSystem *owner = nullptr;
    NotificationSystem::BackendType backendType = NotificationSystem::TrayIcon;
    NotificationsBackend *backend = nullptr;
};


NotificationSystemPrivate::~NotificationSystemPrivate()
{
    delete backend;
    backend = nullptr;
}


void NotificationSystemPrivate::setBackend(NotificationSystem::BackendType t)
{
    if (backend) {
        delete backend;
        backend = nullptr;
        backendType = NotificationSystem::None;
    }
    switch (t) {
#ifndef Q_OS_ANDROID
    // desktop cases
    case NotificationSystem::TrayIcon: {
            backend = new NotificationsTrayIconBackend();
            backendType = t;
        } break;
    case NotificationSystem::FreedesktopDbus: {
            // TODO: FreedesktopDbus
        } break;
#endif
    case NotificationSystem::AndroidNative: {
            // TODO: AndroidNative
        } break;
    default: break;
    }

    // connect signals from backend to owner (re-emit signals)
    if (backend) {
        QObject::connect(backend, &NotificationsBackend::trayIconClicked,
                         owner, &NotificationSystem::trayIconClicked);
        QObject::connect(backend, &NotificationsBackend::trayIconRightClicked,
                         owner, &NotificationSystem::trayIconRightClicked);
        QObject::connect(backend, &NotificationsBackend::notificationMessageClicked,
                         owner, &NotificationSystem::notificationMessageClicked);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////


NotificationSystem::NotificationSystem(QObject *parent, BackendType backendType)
    : QObject(parent)
    , dptr(new NotificationSystemPrivate(this))
{
    dptr->setBackend(backendType);
}


NotificationSystem::~NotificationSystem()
{
    delete dptr;
    dptr = nullptr;
}

NotificationSystem::BackendType NotificationSystem::backendType() const
{
    Q_D(const NotificationSystem);
    return d->backendType;
}


void NotificationSystem::setBackend(BackendType backendType)
{
    Q_D(NotificationSystem);
    d->setBackend(backendType);
}


void NotificationSystem::notify(const QString &title, const QString &message)
{
    Q_D(NotificationSystem);
    if (d->backend) {
        d->backend->notify(title, message);
    }
}


} // namesapce EM


#include "notificationsystem.moc"
