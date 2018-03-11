#ifndef H_QMLEVEMON_APP
#define H_QMLEVEMON_APP

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>

class QQuickWindow;

Q_DECLARE_LOGGING_CATEGORY(logApp)


namespace EM {

class PortraitCache;
class Db;
class PeriodicalRefresher;


class QmlEvemonApp: public QGuiApplication
{
    Q_OBJECT
    Q_PROPERTY(quint64 cruCharId READ curCharId NOTIFY curCharIdChanged)
    Q_PROPERTY(bool isDesktopPlatform READ isDesktopPlatform CONSTANT)

public:
    QmlEvemonApp(int& argc, char **argv);
    virtual ~QmlEvemonApp();

public:
    // returns false for Android platform
    bool isDesktopPlatform() const;
    // setup QML root context's properties, creates image providers, etc
    bool initQmlEngine();
    // returns pointer to app database
    Db *database() const;
    // returns AppData path, where to store app config/cache
    QString storageDirectory() const;
    // returns pointer to PortraitCache storage
    PortraitCache *portraitCache() const;
    // returns a pointer to app main window
    QQuickWindow *mainWindow() const;

public Q_SLOTS:
    quint64 curCharId() const;
    // called from QML when selcting character page
    void setCurrentCharacter(quint64 char_id);
    // called from QML to request update character portrait
    void requestRefreshCharacterPortrait(quint64 char_id);
    // called from QML to force refresh all characters
    void forceRefresh();
    // called from QML to request deletion of character from application
    void requestDeleteCharacter(quint64 characterId);

Q_SIGNALS:
    void curCharIdChanged();

private:
    void initStorageDirectory();

protected:
    QQmlApplicationEngine m_engine;
    QQuickWindow *m_mainWindow;
    PortraitCache *m_portraitCache;
    PeriodicalRefresher *m_refresher;
    quint64 m_curCharId;
};


QmlEvemonApp *globalAppInstance();


} // namespace


#endif
