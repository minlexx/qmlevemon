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

#include "corp_icons_provider.h"
#include "db.h"
#include "qmlevemon_app.h"
#include "eve_api/eve_api.h"  // for evesso_user_agent()


Q_LOGGING_CATEGORY(logCorpIcons, "evemon.corpicons_cache")

namespace EM {


class CorpIconsAsyncImageResponse: public QQuickImageResponse, public QRunnable
{
public:
    CorpIconsAsyncImageResponse(const QString &id, const QSize &requestedSize):
        QQuickImageResponse(),
        QRunnable(),
        m_id(id),
        m_requestedSize(requestedSize)
    {
        setAutoDelete(false);
    }

    ~CorpIconsAsyncImageResponse() override
    {
    }

    QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(m_image);
    }

    void run() override
    {
        //qCDebug(logCorpIcons) << "Requested corp icon:" << m_id <<
        //                      "of size:" << m_requestedSize;

        // get corp_id from requested image id
        quint64 corp_id = m_id.toULongLong();

        // try to load icon from cache
        Db *db = globalAppDatabaseInstance();
        if (!db) {
            m_image.load(QLatin1String(":/img/character_icon_128.jpg"));
            Q_EMIT finished();
            return;
        }

        if (!db->loadTypeIcon(corp_id, m_image)) {
            // cache failed, download from eve image server

            {
                QEventLoop evtloop;
                QTimer rtimer;
                QNetworkAccessManager nam;
                QNetworkRequest req;

                QObject::connect(&rtimer, &QTimer::timeout, &evtloop, &QEventLoop::quit);
                QObject::connect(&nam, &QNetworkAccessManager::finished,
                                 &evtloop, &QEventLoop::quit);

                // https://imageserver.eveonline.com/Corporation/14264_128.png
                QString url = QString(QLatin1String("https://imageserver.eveonline.com/"
                                      "Corporation/%1_128.png")).arg(corp_id);
                req.setUrl(QUrl(url));
                req.setHeader(QNetworkRequest::UserAgentHeader, evesso_user_agent());

                rtimer.setSingleShot(true);
                rtimer.setInterval(20000); // 20 seconds
                rtimer.start();

                qCDebug(logCorpIcons) << "Requesting corp icon:" << url;
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
                        qCDebug(logCorpIcons) << "Corp icon download OK.";
                    } else {
                        qCWarning(logCorpIcons) << "Corp icon downloaded, but open as image failed!";
                        m_image = QImage();
                    }
                } else {
                    qCWarning(logCorpIcons) << "Corp icon download failed!";
                    m_image = QImage();
                }
            }

            if (m_image.isNull()) {
                // download failed for some reason,  use default icon
                m_image.load(QLatin1String(":/img/character_icon_128.jpg"));
            } else {
                // store downloaded image in the cache
                db->saveTypeIcon(corp_id, m_image);
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





CorpIconsProvider::CorpIconsProvider()
{
    qCDebug(logCorpIcons) << "created";
    m_threadPool.setMaxThreadCount(10);
}


CorpIconsProvider::~CorpIconsProvider()
{
    // wait for all threads to stop for 15 seconds
    qCDebug(logCorpIcons) << "stopping thread pool (15 secs)...";
    m_threadPool.waitForDone(15000);
    qCDebug(logCorpIcons) << "destructor complete";
}


QQuickImageResponse *CorpIconsProvider::requestImageResponse(
        const QString &id,
        const QSize &requestedSize)
{
    CorpIconsAsyncImageResponse *imgResponse = new CorpIconsAsyncImageResponse(
                id, requestedSize);
    m_threadPool.start(imgResponse);
    return imgResponse;
}


} // namespace
