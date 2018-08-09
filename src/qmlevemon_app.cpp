#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QStandardPaths>
#include <QScreen>
#include <QDir>
#include <QDebug>

#ifndef Q_OS_ANDROID
// classes from QtWidgets are used for tray popup menu
#include <QMenu>
#include <QAction>
#include <QPoint>   // QtCore
#include <QCursor>  // QtGui
#endif

#include "db/db_sqlite.h"
#include "db/portrait_cache.h"
#include "db/type_icons_provider.h"
#include "db/corp_icons_provider.h"
#include "models/character.h"
#include "models/character_model.h"
#include "models/skill_tree_model.h"
#include "models/model_manager.h"
#include "eve_api/eve_sso.h"
#include "network/periodical_refresher.h"
#include "notificationsystem.h"
#include "qmlevemon_app.h"


Q_LOGGING_CATEGORY(logApp, "evemon.app")


namespace EM {


static QmlEvemonApp *g_globalAppInstance = nullptr;


QmlEvemonApp::QmlEvemonApp(int& argc, char **argv):
#ifdef Q_OS_ANDROID
    QGuiApplication(argc, argv)
#else
    QApplication(argc, argv)
#endif
{
    g_globalAppInstance = this;
    setApplicationName(QLatin1String("qmlevemon"));
    setApplicationDisplayName(QLatin1String("QML EVEMon"));
    setApplicationVersion(QLatin1String(QMLEVEMON_VERSION));

    // register types
    qRegisterMetaType<Mail>();

    // settings
    m_settings = new AppSettings(this);
    QObject::connect(m_settings, &AppSettings::settingsChanged,
                     this, &QmlEvemonApp::settingsChanged);

    // notifications
    m_notifications = new NotificationSystem(this,
#ifdef Q_OS_ANDROID
                                             NotificationSystem::AndroidNative
#else
                                             NotificationSystem::TrayIcon
#endif
                                             );
    QObject::connect(m_notifications, &NotificationSystem::trayIconClicked,
                     this, &QmlEvemonApp::onTrayIconClicked);
    QObject::connect(m_notifications, &NotificationSystem::trayIconRightClicked,
                     this, &QmlEvemonApp::onTrayIconRightClicked);

    m_portraitCache = new PortraitCache();
    m_typeIconsProvider = new TypeIconsProvider();
    m_corpIconsProvider = new CorpIconsProvider();
    m_refresher = new PeriodicalRefresher(this);

    QObject::connect(m_refresher, &PeriodicalRefresher::mailBodyDownloaded,
                     this, &QmlEvemonApp::onMailBodyDownloaded);
    QObject::connect(m_refresher, &PeriodicalRefresher::networkError,
                     this, &QmlEvemonApp::onNetworkError);

    QScreen *appScreen = primaryScreen();
    if (Q_LIKELY(appScreen)) {
        Qt::ScreenOrientation orientation = appScreen->primaryOrientation();
        m_isPortraitOrientation = (orientation == Qt::PortraitOrientation
                                   || orientation == Qt::InvertedPortraitOrientation);
        QObject::connect(appScreen, &QScreen::primaryOrientationChanged,
                         this, &QmlEvemonApp::onPrimaryOrientationChanged);
    }

    CharacterModel *charModel = ModelManager::instance()->characterModel();
    QObject::connect(charModel, &CharacterModel::skillCompletedNotification,
                     this, &QmlEvemonApp::onSkillCompletedMessage);
    QObject::connect(charModel, &CharacterModel::newMailsReceivedNotification,
                     this, &QmlEvemonApp::onNewMailsReceivedMessage);

    this->initStorageDirectory();
}


QmlEvemonApp::~QmlEvemonApp()
{
    qCDebug(logApp) << "~QmlEvemonApp(): destructor start";
    g_globalAppInstance = nullptr;  // indicate that shutdown is in progress, no app
    m_refresher->stopGracefully();
    // engine takes ownership of the image provider!
    // so we should not and cannot delete m_portraitCache.
    // It is automatically deleted in QQmlEngine destructor
    //delete m_portraitCache; // do not do this!
    qCDebug(logApp) << "~QmlEvemonApp(): destructor end";
}


bool QmlEvemonApp::isDesktopPlatform() const
{
#if defined(Q_OS_ANDROID)
    return false;
#endif
    return true;
}

bool QmlEvemonApp::isPortrait() const { return m_isPortraitOrientation; }

bool QmlEvemonApp::isLandscape() const { return !m_isPortraitOrientation; }


// setup QML root context's properties, creates image providers, etc
bool QmlEvemonApp::initQmlEngine()
{
    QQmlContext *rootContext = m_engine.rootContext();

    ModelManager::instance()->initModels();

    // set context properties
    rootContext->setContextProperty(QLatin1String("evemonapp"), this);
    rootContext->setContextProperty(QLatin1String("characterModel"),
                                    ModelManager::instance()->characterModel());
    rootContext->setContextProperty(QLatin1String("skillTreeModel"),
                                    ModelManager::instance()->skillTreeModel());
    rootContext->setContextProperty(QLatin1String("refresher"), m_refresher);
    rootContext->setContextProperty(QLatin1String("eveSsoLoginManager"),
                                    EveSsoLoginManager::instance());

    // initial defaults
    rootContext->setContextProperty(QLatin1String("curMail"), nullptr);

    DbSqlite::instance();  // init database

    // engine takes ownership of the image provider!
    // so we should not and cannot delete m_portraitCache
    m_engine.addImageProvider(QLatin1String("portrait"), m_portraitCache);
    m_engine.addImageProvider(QLatin1String("typeid"), m_typeIconsProvider);
    m_engine.addImageProvider(QLatin1String("corporation"), m_corpIconsProvider);

    QObject::connect(ModelManager::instance()->characterModel(),
                     &CharacterModel::newCharacterAdded,
                     m_refresher, &PeriodicalRefresher::refresh,
                     Qt::QueuedConnection);

    m_engine.load(QUrl(QLatin1String("qrc:/qml/main.qml")));
    if (m_engine.rootObjects().isEmpty()) {
        qCWarning(logApp) << "Failed to create QML engine!";
        return false;
    }

    QObject *firstRootObject = m_engine.rootObjects().first();
    m_mainWindow = qobject_cast<QQuickWindow *>(firstRootObject);
    if (!m_mainWindow) {
        qCWarning(logApp) << "  Could not find mainWindow! Something is wrong with main QML?";
    } else {
        // setup window icon
        const QIcon icon(QLatin1String(":/img/app_icon/128.png"));
        m_mainWindow->setIcon(icon);
    }

    return true;
}


Db *QmlEvemonApp::database() const
{
    return DbSqlite::instance();
}


QString QmlEvemonApp::storageDirectory() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}


PortraitCache *QmlEvemonApp::portraitCache() const
{
    return m_portraitCache;
}


QQuickWindow *QmlEvemonApp::mainWindow() const
{
    return m_mainWindow;
}

QObject *QmlEvemonApp::settingsO()
{
    return static_cast<QObject *>(m_settings);
}

AppSettings *QmlEvemonApp::settings()
{
    return m_settings;
}


void QmlEvemonApp::initStorageDirectory()
{
    QString appdata_dirname = this->storageDirectory();
    // Note: The storage location returned can be a directory that does not exist;
    //       i.e., it may need to be created by the system or the user.
    QDir appdata_dir(appdata_dirname);
    if (!appdata_dir.exists()) {
        if (!appdata_dir.mkdir(QLatin1String("."))) {
            // retry using mkpath(); fix for Linux
            if (!appdata_dir.mkpath(QLatin1String("."))) {
                qCWarning(logApp) << "Failed to create app storage directory:"
                                  << appdata_dirname;
            }
        }
    }
}


quint64 QmlEvemonApp::curCharId() const
{
    return m_curCharId;
}

void QmlEvemonApp::onPrimaryOrientationChanged(Qt::ScreenOrientation orientation)
{
    switch (orientation) {
    case Qt::PortraitOrientation:
    case Qt::InvertedPortraitOrientation:
    case Qt::LandscapeOrientation:
    case Qt::InvertedLandscapeOrientation:
        m_isPortraitOrientation = (orientation == Qt::PortraitOrientation
                                   || orientation == Qt::InvertedPortraitOrientation);
        Q_EMIT isPortraitChanged();
        Q_EMIT isLandscapeChanged();
        break;
    default: break;
    }
}

void QmlEvemonApp::onTrayIconClicked()
{
    if (!m_mainWindow) {
        return;
    }
    // TODO: probably needs Windows-specific tweak about activating window
    m_mainWindow->showNormal();
    m_mainWindow->requestActivate();
}

void QmlEvemonApp::onTrayIconRightClicked()
{
    qCDebug(logApp) << Q_FUNC_INFO;
#ifndef Q_OS_ANDROID
    QMenu *menu = new QMenu(nullptr);
    QAction *actShow = menu->addAction(tr("Show window"));
    QAction *actQuit = menu->addAction(tr("Quit"));
    QObject::connect(actShow, &QAction::triggered, this, &QmlEvemonApp::onTrayIconClicked);
    QObject::connect(actQuit, &QAction::triggered, this, &QCoreApplication::quit);
    menu->exec(QCursor::pos());
    menu->deleteLater();
#endif
}


// called from QML when selcting character page
void QmlEvemonApp::setCurrentCharacter(quint64 char_id)
{
    if (m_curCharId == char_id) {
        // qCDebug(logApp) << " already current char:" << char_id;
        return;
    }
    // specia value - overview page, no character selected
    if (char_id == 0) {
        m_curCharId = 0;
        Q_EMIT curCharIdChanged();
        return;
    }
    // character should exist
    Character *ch = ModelManager::instance()->characterModel()->findCharacterById(char_id);
    if (ch == nullptr) {
        // something goes completely wrong
        qCWarning(logApp) << "setCurrentCharacter(" << char_id << ") failed!";
        return;
    }
    // qCDebug(logApp) << "setting current displayed character as:" << ch->toString();
    m_curCharId = ch->characterId();
    Q_EMIT curCharIdChanged();
    m_engine.rootContext()->setContextProperty(QLatin1String("curChar"), ch);
}


// called from QML to request update character portrait
void QmlEvemonApp::requestRefreshCharacterPortrait(quint64 char_id)
{
    m_portraitCache->removeCachedImageForCharacter(char_id);
}


// called from QML to force refresh all characters
void QmlEvemonApp::forceRefresh()
{
    m_refresher->forceRefreshNow();
}


// // called from QML to request deletion of character from application
void QmlEvemonApp::requestDeleteCharacter(quint64 characterId)
{
    qCDebug(logApp) << "Deleting character:" << characterId;
    ModelManager::instance()->characterModel()->removeCharacter(characterId);
}

void QmlEvemonApp::requestOpenMail(quint64 mailId)
{
    qCDebug(logApp) << "Request to open mail: " << mailId;
    Character *ch = ModelManager::instance()->characterModel()->findCharacterById(m_curCharId);
    if (ch == nullptr) {
        // something goes completely wrong
        qCWarning(logApp) << "requestOpenMail(): cannot find current char: " << m_curCharId << "!";
        return;
    }
    m_refresher->requestMailBody(ch, mailId);
}

void QmlEvemonApp::showNotification(const QString &title, const QString &message)
{
    if (!m_notifications) return;
    m_notifications->notify(title, message);
}

void QmlEvemonApp::testErrorHandler(const QString &t, const QString &s)
{
    if (t == QLatin1String("error")) {
        onNetworkError(s);
    } else {
        Q_EMIT messagePopup(t, s);
    }
}

int QmlEvemonApp::lastCharacterTab() const { return m_lastCharacterTab; }

void QmlEvemonApp::setLastCharacterTab(int t)
{
    if (m_lastCharacterTab != t) {
        m_lastCharacterTab = t;
        Q_EMIT lastCharacterTabChanged();
    }
}

void QmlEvemonApp::onMailBodyDownloaded(quint64 charId, quint64 mailId, const QString &body)
{
    Q_UNUSED(body)
    Character *ch = ModelManager::instance()->characterModel()->findCharacterById(charId);
    if (ch == nullptr) {
        // something goes completely wrong
        qCWarning(logApp) << "requestOpenMail(): cannot find char: " << charId << "!";
        return;
    }

    Mail mail;
    if (ch->findMailById(mailId, mail)) {
        QVariant var = QVariant::fromValue(mail);
        m_engine.rootContext()->setContextProperty(QLatin1String("curMail"), var);
        Q_EMIT mailReady();
    } else {
        m_engine.rootContext()->setContextProperty(QLatin1String("curMail"), nullptr);
    }
}

void QmlEvemonApp::onNetworkError(const QString &desc)
{
    const QString msg = QStringLiteral("Net error: ") + desc;
    Q_EMIT messagePopup(QStringLiteral("error"), msg);
}

void QmlEvemonApp::onSkillCompletedMessage(const QString &msg)
{
    Q_EMIT messagePopup(QStringLiteral("info"), msg);
    showNotification(applicationDisplayName() + tr(": Skill completed"), msg);
}

void QmlEvemonApp::onNewMailsReceivedMessage(const QString &msg)
{
    Q_EMIT messagePopup(QStringLiteral("info"), msg);
    showNotification(applicationDisplayName() + tr(": New mail"), msg);
}


QmlEvemonApp *globalAppInstance()
{
    // return dynamic_cast<QmlEvemonApp *>(QCoreApplication::instance());
    // we need to return nullptr if shutdown is in progress
    return g_globalAppInstance;
}

Db *globalAppDatabaseInstance()
{
    Db *ret = nullptr;
    if (g_globalAppInstance) {
        ret = g_globalAppInstance->database();
    }
    return ret;
}

AppSettings *globalAppSettings()
{
    AppSettings *ret = nullptr;
    if (g_globalAppInstance) {
        ret = g_globalAppInstance->settings();
    }
    return ret;
}

} // namespace
