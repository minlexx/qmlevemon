#ifndef H_QMLEVEMON_APP
#define H_QMLEVEMON_APP

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include "settings.h"

QT_FORWARD_DECLARE_CLASS(QQuickWindow);

Q_DECLARE_LOGGING_CATEGORY(logApp)


namespace EM {


class Db;
class PortraitCache;
class TypeIconsProvider;
class PeriodicalRefresher;
class NotificationSystem;


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
    ~QmlEvemonApp() override;

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
    void onTrayIconClicked();
    void onTrayIconRightClicked();
    void onMailBodyDownloaded(quint64 charId, quint64 mailId, const QString &body);
    void onNetworkError(const QString &desc);
    void onSkillCompletedMessage(const QString &msg);

    // called from QML when selcting character page
    void setCurrentCharacter(quint64 char_id);
    // called from QML to request update character portrait
    void requestRefreshCharacterPortrait(quint64 char_id);
    // called from QML to force refresh all characters
    void forceRefresh();
    // called from QML to request deletion of character from application
    void requestDeleteCharacter(quint64 characterId);
    // called from QML to request open specific mail
    void requestOpenMail(quint64 mailId);
    // call from QML to show notification
    void showNotification(const QString &title, const QString &message);

    // use in tests only. trigger displaying test notification
    void testErrorHandler(const QString &t, const QString &s);

Q_SIGNALS:
    void settingsChanged();
    void curCharIdChanged();
    void isPortraitChanged();
    void isLandscapeChanged();
    void mailReady();
    void messagePopup(const QString &type, const QString &message);

private:
    void initStorageDirectory();

protected:
    AppSettings *m_settings = nullptr;
    NotificationSystem *m_notifications = nullptr;
    QQmlApplicationEngine m_engine;
    QQuickWindow *m_mainWindow = nullptr;
    PortraitCache *m_portraitCache = nullptr;
    TypeIconsProvider *m_typeIconsProvider = nullptr;
    PeriodicalRefresher *m_refresher = nullptr;
    quint64 m_curCharId = 0;
    bool m_isPortraitOrientation = true;

};


QmlEvemonApp *globalAppInstance();
Db *globalAppDatabaseInstance();
AppSettings *globalAppSettings();


} // namespace


#endif
