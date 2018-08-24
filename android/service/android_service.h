#ifndef H_QMLEVEMON_ANDROID_SERVICE
#define H_QMLEVEMON_ANDROID_SERVICE

#include <QtGlobal>

#ifdef Q_OS_ANDROID

#include <QtAndroid>
#include <QtAndroidExtras>

namespace EM {


class QmlEvemonService: public QAndroidService
{
    Q_OBJECT

public:
    QmlEvemonService(int &argc, char **argv);
    ~QmlEvemonService() override;

    QAndroidBinder* onBind(const QAndroidIntent &intent) override;
};


int android_service_main(int &argc, char **argv);


} // namespace EM

#endif

#endif
