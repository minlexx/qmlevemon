#ifndef CORP_ICONS_PROVIDER_H
#define CORP_ICONS_PROVIDER_H

#include <QObject>
#include <QString>
#include <QQuickAsyncImageProvider>
#include <QThreadPool>


namespace EM {


class CorpIconsProvider: public QQuickAsyncImageProvider
{
public:
    CorpIconsProvider();
    ~CorpIconsProvider() override;
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

protected:
    QThreadPool m_threadPool;
};

} // namespace


#endif // CORP_ICONS_PROVIDER_H
