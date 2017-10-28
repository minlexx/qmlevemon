#ifndef H_EM_PORTRAIT_CACHE
#define H_EM_PORTRAIT_CACHE

#include <QString>
#include <QQuickAsyncImageProvider>
#include <QThreadPool>


namespace EM {


class PortraitCache: public QQuickAsyncImageProvider
{
public:
    PortraitCache();
    virtual ~PortraitCache();
    virtual QQuickImageResponse *requestImageResponse(const QString &id,
                                                      const QSize &requestedSize) override;

protected:
    QThreadPool m_threadPool;
};

} // namespace

#endif
