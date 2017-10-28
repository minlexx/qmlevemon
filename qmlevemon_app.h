#ifndef H_QMLEVEMON_APP
#define H_QMLEVEMON_APP

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(logApp)


namespace EM {


class PortraitCache;
class Db;
class PeriodicalRefresher;


class QmlEvemonApp: public QGuiApplication
{
    Q_OBJECT

public:
    QmlEvemonApp(int& argc, char **argv);
    virtual ~QmlEvemonApp();

    bool initQmlEngine();

    // returns pointer to app database
    Db *database() const;

    // returns AppData path, where to store app config/cache
    QString storageDirectory() const;

    // returns pointer to PortraitCache storage
    PortraitCache *portraitCache() const;

public Q_SLOTS:
    // called from QML when selcting character page
    void setCurrentCharacter(int char_id);

private:
    void initStorageDirectory();

protected:
    QQmlApplicationEngine m_engine;
    PortraitCache *m_portraitCache;
    PeriodicalRefresher *m_refresher;
};


QmlEvemonApp *globalAppInstance();


} // namespace


#endif
