#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

#include "db/db_sqlite.h"
#include "db/portrait_cache.h"
#include "models/character.h"
#include "models/character_model.h"
#include "models/model_manager.h"
#include "eve_api/eve_sso.h"
#include "qmlevemon_app.h"
#include "periodical_refresher.h"


Q_LOGGING_CATEGORY(logApp, "evemon.app")


EM::QmlEvemonApp::QmlEvemonApp(int& argc, char **argv):
    QGuiApplication(argc, argv),
    m_mainWindow(nullptr),
    m_portraitCache(nullptr),
    m_refresher(nullptr)
{
    setApplicationName("qmlevemon");
    setApplicationDisplayName("QML EVEMon");
    setApplicationVersion(QMLEVEMON_VERSION);

    m_portraitCache = new EM::PortraitCache();
    m_refresher = new EM::PeriodicalRefresher(this);

    this->initStorageDirectory();
}


EM::QmlEvemonApp::~QmlEvemonApp()
{
    qCDebug(logApp) << "~QmlEvemonApp()";
    // engine takes ownership of the image provider!
    // so we should not and cannot delete m_portraitCache.
    // It is automatically deleted in QQmlEngine destructor
    //delete m_portraitCache;
}


bool EM::QmlEvemonApp::initQmlEngine()
{
    QQmlContext *rootContext = m_engine.rootContext();

    // set myself as context property
    rootContext->setContextProperty("evemonapp", this);

    EM::ModelManager::instance()->registerModelsAsContextProperties(rootContext);
    EM::EveSsoLoginManager::instance()->registerAsContextProperty(rootContext);
    rootContext->setContextProperty(QLatin1String("refresher"), m_refresher);
    EM::DbSqlite::instance();  // init database
    // engine takes ownership of the image provider!
    // so we should not and cannot delete m_portraitCache
    m_engine.addImageProvider("portrait", m_portraitCache);

    QObject::connect(EM::ModelManager::instance()->characterModel(),
                     &EM::CharacterModel::newCharacterAdded,
                     m_refresher, &EM::PeriodicalRefresher::refresh,
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
    }

    return true;
}


EM::Db *EM::QmlEvemonApp::database() const
{
    return EM::DbSqlite::instance();
}


QString EM::QmlEvemonApp::storageDirectory() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}


EM::PortraitCache *EM::QmlEvemonApp::portraitCache() const
{
    return m_portraitCache;
}


QQuickWindow *EM::QmlEvemonApp::mainWindow() const
{
    return m_mainWindow;
}


void EM::QmlEvemonApp::initStorageDirectory()
{
    QString appdata_dirname = this->storageDirectory();
    // Note: The storage location returned can be a directory that does not exist;
    //       i.e., it may need to be created by the system or the user.
    QDir appdata_dir(appdata_dirname);
    if (!appdata_dir.exists()) {
        if (!appdata_dir.mkdir(".")) {
            // retry using mkpath(); fix for Linux
            if (!appdata_dir.mkpath(".")) {
                qCWarning(logApp) << "Failed to create app storage directory:"
                                  << appdata_dirname;
            }
        }
    }
}


// called from QML when selcting character page
void EM::QmlEvemonApp::setCurrentCharacter(quint64 char_id)
{
    EM::Character *ch = EM::ModelManager::instance()->characterModel()->findCharacterById(char_id);
    if (ch == nullptr) {
        // something goes completely wrong
        qCDebug(logApp) << "setCurrentCharacter(" << char_id << ") failed!";
        return;
    }
    qCDebug(logApp) << "setting current displayed character as:" << ch->toString();
    m_engine.rootContext()->setContextProperty("curChar", ch);
}


// called from QML to request update character portrait
void EM::QmlEvemonApp::requestRefreshCharacterPortrait(quint64 char_id)
{
    m_portraitCache->removeCachedImageForCharacter(char_id);
}


// called from QML to force refresh all characters
void EM::QmlEvemonApp::forceRefresh()
{
    m_refresher->forceRefreshNow();
}



EM::QmlEvemonApp *EM::globalAppInstance()
{
    return dynamic_cast<EM::QmlEvemonApp *>(QCoreApplication::instance());
}
