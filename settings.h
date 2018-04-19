#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QNetworkProxy>

namespace EM {


class AppSettings: public QSettings
{
    Q_OBJECT
    Q_PROPERTY(bool     isProxyEnabled READ isProxyEnabled WRITE setProxyEnabled  NOTIFY proxyConfigChanged)
    Q_PROPERTY(int      proxyType      READ proxyType      WRITE setProxyType     NOTIFY proxyConfigChanged)
    Q_PROPERTY(QString  proxyHost      READ proxyHost      WRITE setProxyHost     NOTIFY proxyConfigChanged)
    Q_PROPERTY(int      proxyPort      READ proxyPort      WRITE setProxyPort     NOTIFY proxyConfigChanged)
    Q_PROPERTY(QString  proxyUser      READ proxyUser      WRITE setProxyUser     NOTIFY proxyConfigChanged)
    Q_PROPERTY(QString  proxyPassword  READ proxyPassword  WRITE setProxyPassword NOTIFY proxyConfigChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);

public:
    bool          isProxyEnabled() const;
    QNetworkProxy proxySettings() const;
    int           proxyType() const;
    QString       proxyHost() const;
    int           proxyPort() const;
    QString       proxyUser() const;
    QString       proxyPassword() const;

public Q_SLOTS:
    void setProxyEnabled(bool b);
    void setProxySettings(const QNetworkProxy &p);
    void setProxyType(int typ);
    void setProxyHost(const QString &host);
    void setProxyPort(int p);
    void setProxyUser(const QString &user);
    void setProxyPassword(const QString &password);

Q_SIGNALS:
    void proxyConfigChanged();
};


} // namespace


#endif // SETTINGS_H
