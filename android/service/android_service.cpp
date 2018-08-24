#include <QtGlobal>
#ifdef Q_OS_ANDROID

#include "android_service.h"
#include <QDebug>
#include <QLoggingCategory>
#include <QStandardPaths>

#include "settings.h"
#include "network/periodical_refresher.h"
#include "models/model_manager.h"
#include "models/character_model.h"
#include "db/db_sqlite.h"

Q_LOGGING_CATEGORY(logService, "qmlevemon.service")


namespace EM {


static QmlEvemonService *g_serviceAppInstance = nullptr;


QmlEvemonService::QmlEvemonService(int &argc, char **argv)
    : QAndroidService(argc, argv)
{
    g_serviceAppInstance = this;

    setObjectName(QStringLiteral("QmlEvemonService"));
    setApplicationName(QStringLiteral("qmlevemon"));
    setApplicationVersion(QStringLiteral(QMLEVEMON_VERSION));

    m_settings = new AppSettings(this);
    m_refresher = new PeriodicalRefresher(this);
    m_refresher->setRefreshIterval(30000); // for testing

    QObject::connect(m_refresher, &PeriodicalRefresher::networkActivityChanged,
                     this, &QmlEvemonService::onRefresherActivity);

    // init db
    DbSqlite::instance();
    // load models
    ModelManager::instance()->initModels();
    CharacterModel *charModel = ModelManager::instance()->characterModel();
    qCDebug(logService) << "  loaded " << charModel->rowCount() << " characters.";
}

QmlEvemonService::~QmlEvemonService()
{
    if (m_refresher) {
        m_refresher->stopGracefully();
    }
}

QAndroidBinder *QmlEvemonService::onBind(const QAndroidIntent &intent)
{
    Q_UNUSED(intent)
    QAndroidBinder *binder = new QAndroidBinder();
    // we don't actually use IPC between BG service and application
    return binder;
}

AppSettings *QmlEvemonService::settings() const { return m_settings; }

QString QmlEvemonService::storageDirectory() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

void QmlEvemonService::notify(const QString &title, const QString &message)
{
    QAndroidJniObject jsTitle = QAndroidJniObject::fromString(title);
    QAndroidJniObject jsMessage = QAndroidJniObject::fromString(message);
    // Returns a handle to this application's main Service
    QAndroidJniObject javaServiceObject = QtAndroid::androidService();
    javaServiceObject.callMethod<void>(
                "notify",
                "(Ljava/lang/String;Ljava/lang/String;)V",
                jsTitle.object<jstring>(),
                jsMessage.object<jstring>());
}

void QmlEvemonService::onRefresherActivity()
{
    if (m_refresher->networkActivity()) {
        // refresh started
        qCDebug(logService) << "Refresh started";
    } else {
        // refresh just ended?
        qCDebug(logService) << "Refresh ended";
        this->notify(QStringLiteral("BG Refresher"), QStringLiteral("Refresh ended"));
    }
}


QmlEvemonService *globalServiceAppInstance()
{
    qCDebug(logService) << "globalServiceAppInstance requested";
    return g_serviceAppInstance;
}


int android_service_main(int &argc, char **argv)
{
    qCDebug(logService) << "Hello from QMLEVEMon service!";

    QmlEvemonService androidService(argc, argv);
    // run usual Qt event loop for console application
    int mainret = androidService.exec();

    qCDebug(logService) << "Service stopping, returning " << mainret;
    return mainret;
}


} // namespace EM

#endif
