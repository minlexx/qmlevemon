#ifndef H_QMLEVEMON_APP
#define H_QMLEVEMON_APP

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include "settings.h"

class QQuickWindow;

Q_DECLARE_LOGGING_CATEGORY(logApp)


namespace EM {

class PortraitCache;
class Db;
class PeriodicalRefresher;


class QmlEvemonApp: public QGuiApplication
{
    Q_OBJECT
    Q_PROPERTY(quint64 curCharId       READ curCharId          NOTIFY curCharIdChanged)
    Q_PROPERTY(bool isDesktopPlatform  READ isDesktopPlatform  CONSTANT)
    Q_PROPERTY(bool isPortrait         READ isPortrait         NOTIFY isPortraitChanged)
    Q_PROPERTY(bool isLandscape        READ isLandscape        NOTIFY isLandscapeChanged)
    Q_PROPERTY(QObject* settings       READ settingsO          CONSTANT)

public:
    QmlEvemonApp(int& argc, char **argv);
    virtual ~QmlEvemonApp();

public:
    // returns false for Android platform
    bool isDesktopPlatform() const;
    // returns true when mobile device is in portrait orientation
    bool isPortrait() const;
    // returns true when mobile device is in landscape orientation
    bool isLandscape() const;

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
    // returns a pointer to settings
    QObject *settingsO();
    AppSettings *settings();

public Q_SLOTS:
    quint64 curCharId() const;
    void onPrimaryOrientationChanged(Qt::ScreenOrientation orientation);

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
    void isPortraitChanged();
    void isLandscapeChanged();

private:
    void initStorageDirectory();

protected:
    AppSettings m_settings;
    QQmlApplicationEngine m_engine;
    QQuickWindow *m_mainWindow = nullptr;
    PortraitCache *m_portraitCache = nullptr;
    PeriodicalRefresher *m_refresher = nullptr;
    quint64 m_curCharId = 0;
    bool m_isPortraitOrientation = true;
};


QmlEvemonApp *globalAppInstance();


} // namespace


#endif
