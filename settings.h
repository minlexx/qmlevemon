#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QNetworkProxy>

namespace EM {


class AppSettings: public QSettings
{
public:
    explicit AppSettings(QObject *parent = nullptr);

public:
    bool isProxyEnabled() const;
    void setProxyEnabled(bool b);

    QNetworkProxy proxySettings() const;
    void setProxySettings(const QNetworkProxy &p);
};


} // namespace


#endif // SETTINGS_H
