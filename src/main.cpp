#include <QCoreApplication>
#include <QQuickStyle>
#include "qmlevemon_app.h"

#if defined(FLATPAK_BUILD)
#include <QIcon>
#endif


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // QQuickStyle::setStyle(QLatin1String("Material"));

#if defined(FLATPAK_BUILD)
    // The style must be configured before loading QML that imports Qt Quick Controls 2
    QQuickStyle::setStyle(QLatin1String("Default"));
#endif

    EM::QmlEvemonApp app(argc, argv);
#if defined(FLATPAK_BUILD)
    QIcon::setThemeName(QStringLiteral("breeze")); // after qApp is constructed only
#endif
    app.setQuitOnLastWindowClosed(true);
    if (!app.initQmlEngine()) {
        return -1;
    }

    return app.exec();
}
