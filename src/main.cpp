#include <QCoreApplication>
#include <QQuickStyle>
#include "qmlevemon_app.h"

#if defined(FLATPAK_BUILD)
#include <QIcon>
#endif

#ifdef Q_OS_ANDROID
#include "./android/service/android_service.h"  // for service_main()
#endif


int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    // check if we are running as android background service
    if (argc > 1) {
        if (strcmp(argv[1], "-service") == 0) {
            return EM::service_main(argc, argv);
        }
    }
#endif

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // QQuickStyle::setStyle(QLatin1String("Material"));

#if defined(FLATPAK_BUILD)
    // The style must be configured before loading QML that imports Qt Quick Controls 2
    QQuickStyle::setStyle(QLatin1String("Default"));
#endif

    EM::QmlEvemonApp app(argc, argv);
#if defined(FLATPAK_BUILD)
    QIcon::setThemeName(QStringLiteral("breeze")); // after qApp is constructed only
    app.setQuitOnLastWindowClosed(true); // this does not help anyway
#endif
    if (!app.initQmlEngine()) {
        return -1;
    }

    return app.exec();
}
