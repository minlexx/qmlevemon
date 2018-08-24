#ifndef H_QMLEVEMON_ANDROID_SERVICE
#define H_QMLEVEMON_ANDROID_SERVICE

#include <QtGlobal>

#ifdef Q_OS_ANDROID

#include <QtAndroid>
#include <QtAndroidExtras>

namespace EM {

class AppSettings;
class PeriodicalRefresher;


class QmlEvemonService: public QAndroidService
{
    Q_OBJECT

public:
    QmlEvemonService(int &argc, char **argv);
    ~QmlEvemonService() override;

    QAndroidBinder* onBind(const QAndroidIntent &intent) override;

public Q_SLOTS:
    AppSettings *settings() const;
    QString storageDirectory() const;

    void notify(const QString &title, const QString &message);
    void onRefresherActivity();

private:
    AppSettings *m_settings = nullptr;
    PeriodicalRefresher *m_refresher = nullptr;
};


QmlEvemonService *globalServiceAppInstance();

int android_service_main(int &argc, char **argv);


} // namespace EM


#endif

#endif
