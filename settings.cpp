#include "settings.h"

EM::AppSettings::AppSettings(QObject *parent)
    : QSettings(IniFormat, UserScope, QLatin1String("evemonsoft"),
                QLatin1String("qmlevemon"), parent)
{
    //
}

bool EM::AppSettings::isProxyEnabled() const
{
    return value(QLatin1String("proxyEnabled"), false).toBool();
}

void EM::AppSettings::setProxyEnabled(bool b)
{
    setValue(QLatin1String("proxyEnabled"), b);
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

void EM::AppSettings::setProxySettings(const QNetworkProxy &p)
{
    setValue(QLatin1String("proxyType"), static_cast<int>(p.type()));
    setValue(QLatin1String("proxyHost"), p.hostName());
    setValue(QLatin1String("proxyPort"), static_cast<int>(p.port()));
    setValue(QLatin1String("proxyUser"), p.user());
    setValue(QLatin1String("proxyPassword"), p.password());
}
