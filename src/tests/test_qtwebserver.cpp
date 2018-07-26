#include <QCoreApplication>
#include <QFile>
#include <QTimer>
#include <QtTest>
#include <QDebug>
#include <QLoggingCategory>

#include "tcp/tcpmultithreadedserver.h"
#include "http/httpwebengine.h"
#include "http/httpiodeviceresource.h"

Q_LOGGING_CATEGORY(logQtWebServer, "qtwebserver")
Q_LOGGING_CATEGORY(logQuitResource, "qtwebserver.quit")

using namespace QtWebServer;


class QuitResource: public Http::Resource {
    Q_OBJECT
public:
    QuitResource(QString uniqueIdentifier, QObject *parent = nullptr):
        Http::Resource(uniqueIdentifier, parent)
    {
        setContentType("text/plain");
    }

    /** Defines the resource's response behaviour. */
    void deliver(const Http::Request& request, Http::Response& response) override {
        if (request.method() == "get") {
            response.setBody(QByteArray("quit requested, exiting"));
            response.setStatusCode(Http::Ok);
            qCDebug(logQuitResource) << "quit requested!";
            QTimer::singleShot(1000, this, &QuitResource::delayedQuit);
        }
    }

public Q_SLOTS:
    void delayedQuit() {
        qCDebug(logQuitResource) << "calling qApp()->quit()";
        QCoreApplication::instance()->quit();
    }
};


class TestQtWebServer: public QObject
{
    Q_OBJECT
private:
    QtWebServer::Tcp::MultithreadedServer m_server;
    QtWebServer::Http::WebEngine m_webengine;

private Q_SLOTS:
    void initTestCase() {
        QtWebServer::Http::IODeviceResource *test_file = new QtWebServer::Http::IODeviceResource(
                    "/test", new QFile("hello.txt"));
        QtWebServer::Http::IODeviceResource *test_html = new QtWebServer::Http::IODeviceResource(
                    "/html", new QFile("html_test.html"));

        m_webengine.addResource(test_file);
        m_webengine.addResource(test_html);
        m_webengine.addResource(new QuitResource("/quit"));

        m_server.setResponder(&m_webengine);
    }

    void cleanupTestCase() {
        m_server.close();
    }

    void test_listen() {
        qCDebug(logQtWebServer) << "Before listen()";
        m_server.listen(QHostAddress::Any, 3000);
        qCDebug(logQtWebServer) << "After listen()";
    }

    void test_run() {
        // int mainret = app.exec();
        qCDebug(logQtWebServer) << "TODO: run webserver for a period of time (not implemented yet)";
    }
};

QTEST_GUILESS_MAIN(TestQtWebServer)

#include "test_qtwebserver.moc"
