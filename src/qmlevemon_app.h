#ifndef H_QMLEVEMON_APP
#define H_QMLEVEMON_APP

#include <QtGlobal>  // needed to define Q_OS_ANDROID or not
#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif
#include <QSet>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include "settings.h"

QT_FORWARD_DECLARE_CLASS(QQuickWindow);

Q_DECLARE_LOGGING_CATEGORY(logApp)


namespace EM {


class Db;
class PortraitCache;
class TypeIconsProvider;
class CorpIconsProvider;
class PeriodicalRefresher;
class NotificationSystem;


class QmlEvemonApp:
#ifdef Q_OS_ANDROID
        public QGuiApplication
#else
        public QApplication
#endif
{
    Q_OBJECT
    Q_PROPERTY(quint64 curCharId       READ curCharId          NOTIFY curCharIdChanged)
    Q_PROPERTY(bool isDesktopPlatform  READ isDesktopPlatform  CONSTANT)
    Q_PROPERTY(bool isFlatpakPlatform  READ isFlatpakPlatform  CONSTANT)
    Q_PROPERTY(bool isPortrait         READ isPortrait         NOTIFY isPortraitChanged)
    Q_PROPERTY(bool isLandscape        READ isLandscape        NOTIFY isLandscapeChanged)
    Q_PROPERTY(QObject* settings       READ settingsO          CONSTANT)
    Q_PROPERTY(int  lastCharacterTab   READ lastCharacterTab   WRITE setLastCharacterTab NOTIFY lastCharacterTabChanged)
    Q_PROPERTY(bool isCharacterSubInfoCollapsed READ isCharacterSubInfoCollapsed WRITE setIsCharacterSubInfoCollapsed NOTIFY isCharacterSubInfoCollapsedChanged)

public:
    QmlEvemonApp(int& argc, char **argv);
    ~QmlEvemonApp() override;

public:
    // returns false for Android platform
    bool isDesktopPlatform() const;
    // returns true for Flatpak build (configure with cmake -DBUILD_FOR_FLATPAK=ON,
    //         which sets -DFLATPAK_BUILD)
    bool isFlatpakPlatform() const;
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
    void onNewMailsReceivedMessage(const QString &msg);
    void onNewNotificationReceivedMessage(const QString &msg);

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
    // call from QML to trigger saving window size and position
    void saveWindowSize();

    // use in tests only. trigger displaying test notification
    void testErrorHandler(const QString &t, const QString &s);

    int  lastCharacterTab() const;
    void setLastCharacterTab(int t);
    bool isCharacterSubInfoCollapsed() const;
    void setIsCharacterSubInfoCollapsed(bool b);

    // Android service management
    void startAndroidService();
    void stopAndroidService();

Q_SIGNALS:
    void settingsChanged();
    void curCharIdChanged();
    void isPortraitChanged();
    void isLandscapeChanged();
    void lastCharacterTabChanged();
    void isCharacterSubInfoCollapsedChanged();
    void mailReady();
    void messagePopup(const QString &type, const QString &message);

private:
    void initStorageDirectory();
    void loadWindowSize();
    void messagePopupNotification(const QString &msg);

protected:
    AppSettings *m_settings = nullptr;
    NotificationSystem *m_notifications = nullptr;
    QQmlApplicationEngine m_engine;
    QQuickWindow *m_mainWindow = nullptr;
    PortraitCache *m_portraitCache = nullptr;
    TypeIconsProvider *m_typeIconsProvider = nullptr;
    CorpIconsProvider *m_corpIconsProvider = nullptr;
    PeriodicalRefresher *m_refresher = nullptr;
    quint64 m_curCharId = 0;
    bool m_isPortraitOrientation = true;
    int m_lastCharacterTab = -1;
    bool m_isCharacterSubInfoCollapsed = false;
    QSet<QString> m_displayedNotificationsCache;

};


QmlEvemonApp *globalAppInstance();
QString globalAppStorageDirectory();
Db *globalAppDatabaseInstance();
AppSettings *globalAppSettings();


} // namespace


#endif
