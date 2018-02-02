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

#include "portrait_cache.h"
#include "db.h"
#include "qmlevemon_app.h"
#include "eve_api/eve_api.h"


Q_LOGGING_CATEGORY(logPcache, "evemon.portrait_cache")


namespace EM {


class PortraitCacheAsyncImageResponse: public QQuickImageResponse, public QRunnable
{
public:
    PortraitCacheAsyncImageResponse(const QString &id, const QSize &requestedSize):
        QQuickImageResponse(),
        QRunnable(),
        m_id(id),
        m_requestedSize(requestedSize)
    {
        setAutoDelete(false);
    }

    virtual ~PortraitCacheAsyncImageResponse()
    {
        //qCDebug(logPcache) << Q_FUNC_INFO;
    }

    virtual QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(m_image);
    }

    virtual void run() override
    {
        qCDebug(logPcache) << "Requested portrait:" << m_id <<
                              "of size:" << m_requestedSize;

        // get char_id from requested image id
        quint64 char_id = 0;
        // m_id can be in format:
        // 1) "91205062"
        // 2) "91205062/tm1514278421702"
        if (m_id.contains(QLatin1Char('/'), Qt::CaseSensitive)) {
            // 2) "91205062/tm1514278421702"
            QStringList sl = m_id.split(QLatin1Char('/'));
            char_id = sl.first().toULongLong();
        } else {
            // 1) simple id
            char_id = m_id.toULongLong();
        }

        // try to load portrait from cache
        Db *db = globalAppInstance()->database();

        if (!db->loadPortrait(char_id, m_image)) {
            // cache failed, download from eve image server

            {
                QEventLoop evtloop;
                QTimer rtimer;
                QNetworkAccessManager nam;
                QNetworkRequest req;

                QObject::connect(&rtimer, &QTimer::timeout, &evtloop, &QEventLoop::quit);
                QObject::connect(&nam, &QNetworkAccessManager::finished,
                                 &evtloop, &QEventLoop::quit);

                QString url = QString(QLatin1String("https://imageserver.eveonline.com/"
                                      "Character/%1_128.jpg")).arg(char_id);
                req.setUrl(QUrl(url));
                req.setHeader(QNetworkRequest::UserAgentHeader, evesso_user_agent());

                rtimer.setSingleShot(true);
                rtimer.setInterval(20000); // 20 seconds
                rtimer.start();

                qCDebug(logPcache) << "Requesting portrait:" << url;
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
                    if (m_image.load(&buf, "JPG")) {
                        qCDebug(logPcache) << "Portrait download OK.";
                    } else {
                        qCWarning(logPcache) << "Portrait downloaded, but open as image failed!";
                        m_image = QImage();
                    }
                } else {
                    qCWarning(logPcache) << "Portrait download failed!";
                    m_image = QImage();
                }
            }

            if (m_image.isNull()) {
                // download failed for some reason,  use default picture
                m_image.load(QLatin1String(":/img/character_icon_128.jpg"), "JPG");
            } else {
                // store downloaded image in the cache
                db->savePortrait(char_id, m_image);
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





PortraitCache::PortraitCache()
{
    qCDebug(logPcache) << "created";
    m_threadPool.setMaxThreadCount(10);
}


PortraitCache::~PortraitCache()
{
    // wait for all threads to stop for 15 seconds
    qCDebug(logPcache) << "stopping thread pool (15 secs)...";
    m_threadPool.waitForDone(15000);
    qCDebug(logPcache) << "destructor complete";
}


QQuickImageResponse *PortraitCache::requestImageResponse(
        const QString &id,
        const QSize &requestedSize)
{
    PortraitCacheAsyncImageResponse *imgResponse = new PortraitCacheAsyncImageResponse(
                id, requestedSize);
    m_threadPool.start(imgResponse);
    return imgResponse;
}


// specific hack to clear cached image for character
void PortraitCache::removeCachedImageForCharacter(quint64 char_id)
{
    Db *db = globalAppInstance()->database();
    db->deletePortrait(char_id);
}


} // namespace
