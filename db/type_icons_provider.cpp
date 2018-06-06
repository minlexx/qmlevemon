#include <QRunnable>
#include <QImage>
#include <QByteArray>
#include <QBuffer>
// network support
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QScopedPointer>
#include <QEventLoop>
#include <QTimer>
// logging support
#include <QLoggingCategory>
#include <QDebug>

#include "type_icons_provider.h"
#include "db.h"
#include "qmlevemon_app.h"
#include "eve_api/eve_api.h"  // for evesso_user_agent()


Q_LOGGING_CATEGORY(logTypeIcons, "evemon.typeicons_cache")

namespace EM {


class TypeIconsAsyncImageResponse: public QQuickImageResponse, public QRunnable
{
public:
    TypeIconsAsyncImageResponse(const QString &id, const QSize &requestedSize):
        QQuickImageResponse(),
        QRunnable(),
        m_id(id),
        m_requestedSize(requestedSize)
    {
        setAutoDelete(false);
    }

    ~TypeIconsAsyncImageResponse() override
    {
        //qCDebug(logPcache) << Q_FUNC_INFO;
    }

    QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(m_image);
    }

    void run() override
    {
        qCDebug(logTypeIcons) << "Requested typeIcon:" << m_id <<
                              "of size:" << m_requestedSize;

        // get type_id from requested image id
        quint64 type_id = m_id.toULongLong();

        // try to load icon from cache
        Db *db = globalAppDatabaseInstance();
        if (!db) {
            m_image.load(QLatin1String(":/img/unknown_type.png"));
            Q_EMIT finished();
            return;
        }

        if (!db->loadTypeIcon(type_id, m_image)) {
            // cache failed, download from eve image server

            {
                QEventLoop evtloop;
                QTimer rtimer;
                QNetworkAccessManager nam;
                QNetworkRequest req;

                QObject::connect(&rtimer, &QTimer::timeout, &evtloop, &QEventLoop::quit);
                QObject::connect(&nam, &QNetworkAccessManager::finished,
                                 &evtloop, &QEventLoop::quit);

                // https://imageserver.eveonline.com/Type/14264_32.png
                QString url = QString(QLatin1String("https://imageserver.eveonline.com/"
                                      "Type/%1_32.png")).arg(type_id);
                req.setUrl(QUrl(url));
                req.setHeader(QNetworkRequest::UserAgentHeader, evesso_user_agent());

                rtimer.setSingleShot(true);
                rtimer.setInterval(20000); // 20 seconds
                rtimer.start();

                qCDebug(logTypeIcons) << "Requesting type icon:" << url;
                QScopedPointer<QNetworkReply> rep(nam.get(req));
                evtloop.exec();

                // here, either the timer timed out, or request finished
                if (rep->isFinished()) {
                    // OK, request did not time out
                    rtimer.stop();
                    // read reply
                    QByteArray replyImage = rep->readAll();
                    QBuffer buf(&replyImage);
                    buf.open(QIODevice::ReadOnly);
                    if (m_image.load(&buf, "PNG")) {
                        qCDebug(logTypeIcons) << "Type icon download OK.";
                    } else {
                        qCWarning(logTypeIcons) << "Type icon downloaded, but open as image failed!";
                        m_image = QImage();
                    }
                } else {
                    qCWarning(logTypeIcons) << "Type icon download failed!";
                    m_image = QImage();
                }
            }

            if (m_image.isNull()) {
                // download failed for some reason,  use default icon
                m_image.load(QLatin1String(":/img/unknown_type.png"));
            } else {
                // store downloaded image in the cache
                db->saveTypeIcon(type_id, m_image);
            }
        }

        if (m_requestedSize.isValid())
            m_image = m_image.scaled(m_requestedSize);

        Q_EMIT finished();
    }

    QString m_id;
    QSize   m_requestedSize;
    QImage  m_image;
};





TypeIconsProvider::TypeIconsProvider()
{
    qCDebug(logTypeIcons) << "created";
    m_threadPool.setMaxThreadCount(10);
}


TypeIconsProvider::~TypeIconsProvider()
{
    // wait for all threads to stop for 15 seconds
    qCDebug(logTypeIcons) << "stopping thread pool (15 secs)...";
    m_threadPool.waitForDone(15000);
    qCDebug(logTypeIcons) << "destructor complete";
}


QQuickImageResponse *TypeIconsProvider::requestImageResponse(
        const QString &id,
        const QSize &requestedSize)
{
    TypeIconsAsyncImageResponse *imgResponse = new TypeIconsAsyncImageResponse(
                id, requestedSize);
    m_threadPool.start(imgResponse);
    return imgResponse;
}


} // namespace
