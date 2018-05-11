#ifndef TYPE_ICONS_PROVIDER_H
#define TYPE_ICONS_PROVIDER_H

#include <QObject>
#include <QString>
#include <QQuickAsyncImageProvider>
#include <QThreadPool>


namespace EM {


class TypeIconsProvider: public QQuickAsyncImageProvider
{
public:
    TypeIconsProvider();
    ~TypeIconsProvider() override;
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

protected:
    QThreadPool m_threadPool;
};

} // namespace


#endif // TYPE_ICONS_PROVIDER_H
