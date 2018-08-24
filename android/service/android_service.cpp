#include <QtGlobal>
#ifdef Q_OS_ANDROID

#include "android_service.h"
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logService, "qmlevemon.service")


namespace EM {


QmlEvemonService::QmlEvemonService(int &argc, char **argv)
    : QAndroidService(argc, argv)
{
    setObjectName(QStringLiteral("QmlEvemonService"));
    setApplicationName(QStringLiteral("qmlevemon_service"));
    setApplicationVersion(QStringLiteral(QMLEVEMON_VERSION));
}

QmlEvemonService::~QmlEvemonService()
{
    //
}

QAndroidBinder *QmlEvemonService::onBind(const QAndroidIntent &intent)
{
    Q_UNUSED(intent)
    QAndroidBinder *binder = new QAndroidBinder();
    // we don't actually use IPC between BG service and application
    return binder;
}


int service_main(int &argc, char **argv)
{
    qCDebug(logService) << "Hello from QMLEVEMon service!";

    QmlEvemonService androidService(argc, argv);
    // run usual Qt event loop for console application
    int mainret = androidService.exec();

    qCDebug(logService) << "Service stopping, returning " << mainret;
    return mainret;
}


} // namespace EM

#endif
