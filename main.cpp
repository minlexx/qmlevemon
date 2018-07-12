#include <QCoreApplication>
#include <QQuickStyle>
#include "qmlevemon_app.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //QQuickStyle::setStyle(QLatin1String("Material"));

    EM::QmlEvemonApp app(argc, argv);
    if (!app.initQmlEngine()) {
        return -1;
    }

    return app.exec();
}
