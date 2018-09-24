#include "settings.h"

EM::AppSettings::AppSettings(QObject *parent)
    : QSettings(IniFormat, UserScope, QLatin1String("evemonsoft"),
                QLatin1String("qmlevemon"), parent)
{ }

EM::AppSettings::~AppSettings() { }

bool EM::AppSettings::isProxyEnabled() const
{
    return value(QLatin1String("proxyEnabled"), false).toBool();
}

void EM::AppSettings::setProxyEnabled(bool b)
{
    setValue(QLatin1String("proxyEnabled"), b);
    Q_EMIT proxyConfigChanged();
    Q_EMIT settingsChanged();
}

QNetworkProxy EM::AppSettings::proxySettings() const
{
    QNetworkProxy::ProxyType ptype = static_cast<QNetworkProxy::ProxyType>(
                value(QLatin1String("proxyType"), 0).toInt());
    QString hostName = value(QLatin1String("proxyHost")).toString();
    quint16 port = static_cast<quint16>(value(QLatin1String("proxyPort"), 0).toInt());
    QString user = value(QLatin1String("proxyUser")).toString();
    QString password = value(QLatin1String("proxyPassword")).toString();

    return QNetworkProxy(ptype, hostName, port, user, password);
}

int EM::AppSettings::proxyType() const
{
    return value(QLatin1String("proxyType"), 0).toInt();
}

QString EM::AppSettings::proxyHost() const
{
    return value(QLatin1String("proxyHost")).toString();
}

int EM::AppSettings::proxyPort() const
{
    return value(QLatin1String("proxyPort"), 0).toInt();
}

QString EM::AppSettings::proxyUser() const
{
    return value(QLatin1String("proxyUser")).toString();
}

QString EM::AppSettings::proxyPassword() const
{
    return value(QLatin1String("proxyPassword")).toString();
}

int EM::AppSettings::winX() const
{
    return value(QLatin1String("win_x")).toInt();
}

int EM::AppSettings::winY() const
{
    return value(QLatin1String("win_y")).toInt();
}

int EM::AppSettings::winW() const
{
    return value(QLatin1String("win_w")).toInt();
}

int EM::AppSettings::winH() const
{
    return value(QLatin1String("win_h")).toInt();
}

void EM::AppSettings::setProxySettings(const QNetworkProxy &p)
{
    setValue(QLatin1String("proxyType"), static_cast<int>(p.type()));
    setValue(QLatin1String("proxyHost"), p.hostName());
    setValue(QLatin1String("proxyPort"), static_cast<int>(p.port()));
    setValue(QLatin1String("proxyUser"), p.user());
    setValue(QLatin1String("proxyPassword"), p.password());
    Q_EMIT proxyConfigChanged();
    Q_EMIT settingsChanged();
}

void EM::AppSettings::setProxyType(int typ)
{
    setValue(QLatin1String("proxyType"), typ);
    Q_EMIT proxyConfigChanged();
    Q_EMIT settingsChanged();
}

void EM::AppSettings::setProxyHost(const QString &host)
{
    setValue(QLatin1String("proxyHost"), host);
    Q_EMIT proxyConfigChanged();
    Q_EMIT settingsChanged();
}

void EM::AppSettings::setProxyPort(int p)
{
    setValue(QLatin1String("proxyPort"), p);
    Q_EMIT proxyConfigChanged();
    Q_EMIT settingsChanged();
}

void EM::AppSettings::setProxyUser(const QString &user)
{
    setValue(QLatin1String("proxyUser"), user);
    Q_EMIT proxyConfigChanged();
    Q_EMIT settingsChanged();
}

void EM::AppSettings::setProxyPassword(const QString &password)
{
    setValue(QLatin1String("proxyPassword"), password);
    Q_EMIT proxyConfigChanged();
    Q_EMIT settingsChanged();
}

void EM::AppSettings::setWinX(int d)
{
    setValue(QLatin1String("win_x"), d);
    Q_EMIT winXChanged();
}

void EM::AppSettings::setWinY(int d)
{
    setValue(QLatin1String("win_y"), d);
    Q_EMIT winYChanged();
}

void EM::AppSettings::setWinW(int d)
{
    setValue(QLatin1String("win_w"), d);
    Q_EMIT winWChanged();
}

void EM::AppSettings::setWinH(int d)
{
    setValue(QLatin1String("win_h"), d);
    Q_EMIT winHChanged();
}
