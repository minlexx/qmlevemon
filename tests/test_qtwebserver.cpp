#include <QCoreApplication>
#include <QFile>
#include <QTimer>
#include <QDebug>

#include "tcp/tcpmultithreadedserver.h"
#include "http/httpwebengine.h"
#include "http/httpiodeviceresource.h"

using namespace QtWebServer;


class QuitResource: public Http::Resource {
public:
    QuitResource(QString uniqueIdentifier, QObject *parent = 0):
        Http::Resource(uniqueIdentifier, parent)
    {
        setContentType("text/plain");
    }

    /** Defines the resource's response behaviour. */
    void deliver(const Http::Request& request, Http::Response& response) override {
        if (request.method() == "get") {
            response.setBody(QByteArray("quit requested, exiting"));
            response.setStatusCode(Http::Ok);
            qDebug() << "quit requested!";
            QTimer::singleShot(1000, this, &QuitResource::delayedQuit);
        }
    }

public Q_SLOTS:
    void delayedQuit() {
        qDebug() << "calling qApp()->quit()";
        QCoreApplication::instance()->quit();
    }
};


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QtWebServer::Tcp::MultithreadedServer server;
    QtWebServer::Http::WebEngine webengine;

    QtWebServer::Http::IODeviceResource *test_file = new QtWebServer::Http::IODeviceResource(
                "/test", new QFile("hello.txt"));
    QtWebServer::Http::IODeviceResource *test_html = new QtWebServer::Http::IODeviceResource(
                "/html", new QFile("html_test.html"));

    webengine.addResource(test_file);
    webengine.addResource(test_html);
    webengine.addResource(new QuitResource("/quit"));

    server.setResponder(&webengine);

    qDebug() << "Before listen()";

    server.listen(QHostAddress::Any, 3000);

    qDebug() << "After listen()";

    int mainret = app.exec();

    server.close();

    return mainret;
}
