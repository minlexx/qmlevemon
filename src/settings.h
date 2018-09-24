#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QNetworkProxy>

namespace EM {


class AppSettings: public QSettings
{
    Q_OBJECT
    // proxy settings
    Q_PROPERTY(bool     isProxyEnabled READ isProxyEnabled WRITE setProxyEnabled  NOTIFY proxyConfigChanged)
    Q_PROPERTY(int      proxyType      READ proxyType      WRITE setProxyType     NOTIFY proxyConfigChanged)
    Q_PROPERTY(QString  proxyHost      READ proxyHost      WRITE setProxyHost     NOTIFY proxyConfigChanged)
    Q_PROPERTY(int      proxyPort      READ proxyPort      WRITE setProxyPort     NOTIFY proxyConfigChanged)
    Q_PROPERTY(QString  proxyUser      READ proxyUser      WRITE setProxyUser     NOTIFY proxyConfigChanged)
    Q_PROPERTY(QString  proxyPassword  READ proxyPassword  WRITE setProxyPassword NOTIFY proxyConfigChanged)
    // window size/pos
    Q_PROPERTY(int winX READ winX WRITE setWinX NOTIFY winXChanged)
    Q_PROPERTY(int winY READ winY WRITE setWinY NOTIFY winYChanged)
    Q_PROPERTY(int winW READ winW WRITE setWinW NOTIFY winWChanged)
    Q_PROPERTY(int winH READ winH WRITE setWinH NOTIFY winHChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);
    virtual ~AppSettings() override;

public:
    // proxy settings
    bool          isProxyEnabled() const;
    QNetworkProxy proxySettings() const;
    int           proxyType() const;
    QString       proxyHost() const;
    int           proxyPort() const;
    QString       proxyUser() const;
    QString       proxyPassword() const;
    // window size/pos
    int winX() const;
    int winY() const;
    int winW() const;
    int winH() const;

public Q_SLOTS:
    // proxy settings
    void setProxyEnabled(bool b);
    void setProxySettings(const QNetworkProxy &p);
    void setProxyType(int typ);
    void setProxyHost(const QString &host);
    void setProxyPort(int p);
    void setProxyUser(const QString &user);
    void setProxyPassword(const QString &password);
    // window size/pos
    void setWinX(int d);
    void setWinY(int d);
    void setWinW(int d);
    void setWinH(int d);

Q_SIGNALS:
    // proxy settings
    void settingsChanged();
    void proxyConfigChanged();
    // window size/pos
    void winXChanged();
    void winYChanged();
    void winWChanged();
    void winHChanged();

private:
    Q_DISABLE_COPY(AppSettings)
};


} // namespace


#endif // SETTINGS_H
