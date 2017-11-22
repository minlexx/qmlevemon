#include "eve_sso.h"
#include <QTimer>
#include <QQmlContext>
#include <QGlobalStatic>
#include <QDesktopServices>
#include <QUrl>
#include <QMap>
#include <QCryptographicHash>
#include <QDateTime>
#include <QLoggingCategory>
#include <QDebug>

// QtWebServer includes
#include "tcp/tcpmultithreadedserver.h"
#include "http/httpwebengine.h"
#include "http/httpiodeviceresource.h"

#include "eve_api.h"
#include "models/model_manager.h"
#include "models/character.h"
#include "models/character_model.h"


Q_GLOBAL_STATIC(EM::EveSsoLoginManager, g_eveSsoMgr)
Q_LOGGING_CATEGORY(logSso, "evemon.sso")


class HttpSsoCallbackResource: public QtWebServer::Http::Resource
{
    Q_OBJECT
public:
    HttpSsoCallbackResource(QObject *parent = 0):
        QtWebServer::Http::Resource(QLatin1String("/evesso"), parent)
    {
        setContentType("text/plain");
    }

    /** Defines the resource's response behaviour. */
    virtual void deliver(const QtWebServer::Http::Request& request,
                         QtWebServer::Http::Response& response)
    {
        if (request.method() == "get") {
            // after successful auth you get request like:
            // params: QMap(("code", "IVoJ4rao...UM4Pm_mT0")
            //              ("state", "stf7d8....60e"))
            QMap<QString, QByteArray> params = request.urlParameters();
            response.setBody(tr("EVE OAuth login response received OK.\n"
                                "You now can close this tab and return back\n"
                                "to the application window to see the results.").toUtf8());
            response.setStatusCode(QtWebServer::Http::Ok);
            qCDebug(logSso) << "Web callback: response received OK!";
#ifdef QT_DEBUG
            qCDebug(logSso) << "Web callback: params:" << params;
#endif
            // ^^ QMap(("code", "nnpvVow_L0i7.....0Eytw_IJvGOLEvN0"))
            if (params.contains("code") && params.contains("state")) {
                QString code = params["code"];
                QString state = params["state"];
                emit codeReceived(code, state);
            }
        }
    }

Q_SIGNALS:
    void codeReceived(const QString& code, const QString& state);
};


class EM::EveSsoLoginManagerPrivate: public QObject
{
    Q_OBJECT
public:
    EveSsoLoginManagerPrivate():
        QObject(Q_NULLPTR),
        m_listenPort(10233)
    {
        m_webengine.addResource(&m_callback);
        m_server.setResponder(&m_webengine);
        QTime tmCur = QTime::currentTime();
        qsrand(static_cast<uint>(tmCur.msecsSinceStartOfDay()));
        //
        QObject::connect(&m_callback, &HttpSsoCallbackResource::codeReceived,
                         this, &EveSsoLoginManagerPrivate::onCodeReceived,
                         Qt::QueuedConnection);
    }

    virtual ~EveSsoLoginManagerPrivate() {
        if (m_server.isListening()) {
            stopSsoHandlerHttpServer();
        }
    }

    bool startSsoHandlerHttpServer() {
        if (m_server.isListening()) return true;
        bool ok = m_server.listen(QHostAddress::LocalHost, static_cast<quint16>(m_listenPort));
        if (ok) {
            // generate random state hash
            m_sso_state = "st";
            QCryptographicHash hasher(QCryptographicHash::Sha256);
            int rnd = qrand();
            hasher.addData(reinterpret_cast<const char *>(&rnd), sizeof(rnd));
            m_sso_state.append(hasher.result().toHex());
            qCDebug(logSso) << "Started handler webserver";
        } else {
            qCWarning(logSso) << "Failed to start sso handler webserver! Port busy?";
            qCWarning(logSso) << "Error:" << m_server.errorString();
        }
        return ok;
    }

    bool stopSsoHandlerHttpServer() {
        if (!m_server.isListening()) return true;
        bool ok = m_server.close(true);
        qCDebug(logSso) << "Stopped handler webserver";
        return ok;
    }

    bool isRunning() { return m_server.isListening(); }

    int listenPort() const { return m_listenPort; }
    void setListenPort(int port) { m_listenPort = port; }

    QString ssoState() const { return m_sso_state; }

public Q_SLOTS:
    void onCodeReceived(const QString& code, const QString& state)
    {
#ifdef QT_DEBUG
        qCDebug(logSso) << "code received:" << code << ", state:" << state;
#endif
        // check valid state
        if (state != m_sso_state) {
            emit ssoAuthError(tr("EVE OAuth login error: Invalid state!"));
            return;
        }
        EM::EveOAuthTokens tokens;
        quint64 character_id = 0;
        bool res = EM::eveapi_request_first_access_token(code, tokens, character_id);
        if (res) {
            emit ssoAuthOk();
            EM::Character *character = new EM::Character();
            character->setAuthTokens(tokens);
            character->setCharacterId(character_id);
            EM::CharacterModel *char_model = EM::ModelManager::instance()->characterModel();
            char_model->addNewCharacter(character);
        } else {
            emit ssoAuthError(tr("EVE OAuth login error: request error or time out!"));
        }
    }

Q_SIGNALS:
    void ssoAuthOk();
    void ssoAuthError(const QString& message);

protected:
    int m_listenPort;
    QtWebServer::Tcp::MultithreadedServer m_server;
    QtWebServer::Http::WebEngine m_webengine;
    HttpSsoCallbackResource m_callback;
    QString m_sso_state;
};


EM::EveSsoLoginManager *EM::EveSsoLoginManager::instance()
{
    return g_eveSsoMgr();
}


EM::EveSsoLoginManager::EveSsoLoginManager(QObject *parent):
    QObject(parent),
    d_ptr(new EM::EveSsoLoginManagerPrivate())
{
    qCDebug(logSso) << "EveSsoLoginManager ctor";
    QObject::connect(d_ptr, &EM::EveSsoLoginManagerPrivate::ssoAuthOk,
                     this, &EM::EveSsoLoginManager::ssoAuthOk);
    QObject::connect(d_ptr, &EM::EveSsoLoginManagerPrivate::ssoAuthError,
                     this, &EM::EveSsoLoginManager::ssoAuthError);
}


EM::EveSsoLoginManager::~EveSsoLoginManager()
{
    delete d_ptr;
}


void EM::EveSsoLoginManager::registerAsContextProperty(QQmlContext *ctx)
{
    ctx->setContextProperty("eveSsoLoginManager", this);
}


int EM::EveSsoLoginManager::localHttpServerPort() const {
    Q_D(const EveSsoLoginManager);
    return d->listenPort();
}


void EM::EveSsoLoginManager::slotSetLocalHttpServerPort(int port) {
    Q_D(EveSsoLoginManager);
    d->setListenPort(port);
    emit localHttpServerPortChanged();
}


void EM::EveSsoLoginManager::slotStartSsoAuth()
{
    qCDebug(logSso) << "start SSO Auth";
    Q_D(EveSsoLoginManager);
    if (!d->isRunning()) {
        d->startSsoHandlerHttpServer();
    }
    QString loginUrl(QStringLiteral("https://login.eveonline.com/oauth/authorize"));
    loginUrl += QStringLiteral("?response_type=code");
    loginUrl += QStringLiteral("&redirect_uri=");
    loginUrl += QString("http://localhost:%1/evesso").arg(d->listenPort());
    loginUrl += QStringLiteral("&client_id=");
    loginUrl += EM::evesso_client_id();
    loginUrl += QStringLiteral("&scope=");
    loginUrl += QStringLiteral("esi-calendar.read_calendar_events.v1 "
                               "esi-location.read_location.v1 "
                               "esi-location.read_ship_type.v1 "
                               "esi-mail.organize_mail.v1 "
                               "esi-mail.read_mail.v1 "
                               "esi-mail.send_mail.v1 "
                               "esi-skills.read_skills.v1 "
                               "esi-skills.read_skillqueue.v1 "
                               "esi-wallet.read_character_wallet.v1 "
                               "esi-clones.read_clones.v1 "
                               "esi-characters.read_contacts.v1 "
                               "esi-universe.read_structures.v1 "
                               "esi-bookmarks.read_character_bookmarks.v1 "
                               "esi-killmails.read_killmails.v1 "
                               "esi-assets.read_assets.v1 "
                               "esi-fleets.read_fleet.v1 "
                               "esi-fittings.read_fittings.v1 "
                               "esi-fittings.write_fittings.v1 "
                               "esi-characters.read_loyalty.v1 "
                               "esi-characters.read_opportunities.v1 "
                               "esi-characters.read_chat_channels.v1 "
                               "esi-characters.read_medals.v1 "
                               "esi-characters.read_standings.v1 "
                               "esi-characters.read_agents_research.v1 "
                               "esi-industry.read_character_jobs.v1 "
                               "esi-markets.read_character_orders.v1 "
                               "esi-characters.read_blueprints.v1 "
                               "esi-characters.read_corporation_roles.v1 "
                               "esi-location.read_online.v1 "
                               "esi-contracts.read_character_contracts.v1 "
                               "esi-clones.read_implants.v1 "
                               "esi-characters.read_fatigue.v1 "
                               "esi-characters.read_notifications.v1");
    loginUrl += QStringLiteral("&state=");
    loginUrl += d->ssoState();
    QDesktopServices::openUrl(QUrl(loginUrl, QUrl::TolerantMode));
}


void EM::EveSsoLoginManager::slotCancelSsoAuth()
{
    qCDebug(logSso) << "cancel SSO Auth";
    Q_D(EveSsoLoginManager);
    d->stopSsoHandlerHttpServer();
}


#include "eve_sso.moc"
