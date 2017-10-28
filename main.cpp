#include "qmlevemon_app.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    EM::QmlEvemonApp app(argc, argv);
    if (!app.initQmlEngine()) {
        return -1;
    }

    return app.exec();
}
