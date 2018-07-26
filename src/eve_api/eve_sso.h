#ifndef H_EVE_SSO
#define H_EVE_SSO

#include <QObject>


namespace EM {


class EveSsoLoginManagerPrivate;

class EveSsoLoginManager: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int      localHttpServerPort
               READ     localHttpServerPort
               WRITE    slotSetLocalHttpServerPort
               NOTIFY   localHttpServerPortChanged)

public:
    explicit EveSsoLoginManager(QObject *parent = Q_NULLPTR);
    ~EveSsoLoginManager() override;
    static EveSsoLoginManager *instance();


public:
    int localHttpServerPort() const;

public Q_SLOTS:
    void slotStartSsoAuth();
    void slotCancelSsoAuth();
    void slotSetLocalHttpServerPort(int port);

Q_SIGNALS:
    void ssoAuthOk();
    void ssoAuthError(const QString& message);

    // properties
    void localHttpServerPortChanged();

private:
    Q_DISABLE_COPY(EveSsoLoginManager)
    Q_DECLARE_PRIVATE(EveSsoLoginManager)
    EveSsoLoginManagerPrivate *d_ptr = nullptr;
};


} // namespace


#endif
