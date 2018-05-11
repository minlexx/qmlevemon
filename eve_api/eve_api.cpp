#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QTimer>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QScopedPointer>
#include <QNetworkProxy>
#include <QLoggingCategory>
#include <QDebug>

#include "eve_api.h"
#include "qmlevemon_app.h"


Q_LOGGING_CATEGORY(logApi, "evemon.eveapi")


namespace EM {


QByteArray evesso_client_id() {
    return QByteArray::fromBase64("Y2U1NmU2YTQ3Zjc2NDI0ZGJjMzdlODA1NmY0N2YxMmQ=");
}

QByteArray evesso_client_secret_key() {
    return QByteArray::fromBase64("V0NWakdIN2RidlYwSXJjOUJpbUNDYW1tRGoyT2hpM3NkQWdxNlVTUA==");
}

QByteArray evesso_user_agent() {
    QByteArray ret;
    QmlEvemonApp *gApp = globalAppInstance();
    if (!gApp) return ret;
    ret.append(gApp->applicationName().toUtf8());
    ret.append("/");
    ret.append(gApp->applicationVersion().toUtf8());
    ret.append(", alexey.min@gmail.com");
    // "qmlevemon/0.1.0, alexey.min@gmail.com"
    return ret;
}



bool eveapi_request_first_access_token(const QString& code,
                                       EveOAuthTokens& result,
                                       quint64& character_id)
{
    QTimer req_timer;
    QEventLoop local_event_loop;
    QNetworkAccessManager nam;

    // check for proxy settings
    QmlEvemonApp *gApp = globalAppInstance();
    if (gApp) {
        AppSettings *settings = gApp->settings();
        if (settings->isProxyEnabled()) {
            nam.setProxy(settings->proxySettings());
        }
    }

    QObject::connect(&nam, &QNetworkAccessManager::finished,
                     &local_event_loop, &QEventLoop::quit);
    QObject::connect(&req_timer, &QTimer::timeout,
                     &local_event_loop, &QEventLoop::quit);

    QNetworkRequest req;
    req.setUrl(QUrl(QLatin1String("https://login.eveonline.com/oauth/token")));

    // create Authorization: Basic ... OAuth header
    // The header should be the string “Basic ” plus the Base64
    //     encoded string {client_id}:{client_secret}.
    QByteArray auth_data = evesso_client_id() + ":" + evesso_client_secret_key();
    QByteArray auth_header = "Basic " + auth_data.toBase64();
    req.setHeader(QNetworkRequest::UserAgentHeader, evesso_user_agent());
    req.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    req.setRawHeader("Authorization", auth_header);

    QByteArray post_data = "grant_type=authorization_code&code=" + code.toUtf8();

    req_timer.setSingleShot(true);
    req_timer.setInterval(15000); // 15 seconds
    req_timer.start();
    qCDebug(logApi) << "Requesting access token...";
    QScopedPointer<QNetworkReply> rep(nam.post(req, post_data));
    local_event_loop.exec();

    // here, either the timer timed out, or request finished
    if (rep->isFinished()) {
        // OK, request did not time out
        qCDebug(logApi) << "Access token request finished";
        req_timer.stop();
        // read reply
        QByteArray replyJson = rep->readAll();
        /* {
            "access_token":"uNEEh...a_WpiaA2",
            "token_type":"Bearer",
            "expires_in":1200,
            "refresh_token":"gEy...fM0"
        } */
        QJsonObject jobj = QJsonDocument::fromJson(replyJson).object();
        if (jobj.contains(QLatin1String("access_token"))
                && jobj.contains(QLatin1String("refresh_token"))
                && jobj.contains(QLatin1String("expires_in"))) {
            // correct reply
            qCDebug(logApi) << "Login OK, got all the tokens";
            // get data
            int expires_in = jobj.value(QLatin1String("expires_in")).toInt();
            QDateTime expire_dt = QDateTime::currentDateTime().addSecs(expires_in);
            result.access_token = jobj.value(QLatin1String("access_token")).toString().toUtf8();
            result.refresh_token = jobj.value(QLatin1String("refresh_token")).toString().toUtf8();
            result.expire_dt = expire_dt;
        } else {
            // incorrect reply
            qCWarning(logApi) << "EVE OAuth replied with error!";
            qCWarning(logApi) << replyJson;
            return false;
        }
    } else {
        qCWarning(logApi) << "Request timed out";
        return false;
    }

    // obtain caharacter ID
    QNetworkRequest req2;
    req2.setUrl(QUrl(QLatin1String("https://login.eveonline.com/oauth/verify")));
    req2.setRawHeader("User-Agent", evesso_user_agent());
    req2.setRawHeader("Authorization", QByteArray("Bearer ") + result.access_token);

    req_timer.start();
    qCDebug(logApi) << "Requesting character_id...";
    rep.reset(nam.get(req2));
    local_event_loop.exec();

    if (rep->isFinished()) {
        req_timer.stop();
        QByteArray replyJson = rep->readAll();
        QJsonObject jobj = QJsonDocument::fromJson(replyJson).object();
        if (jobj.contains(QLatin1String("CharacterID"))) {
            character_id = jobj.value(QLatin1String("CharacterID")).toVariant().toULongLong();
            qCDebug(logApi) << "Requested character_id OK";
        } else {
            qCWarning(logApi) << "Error obtaining character_id: not in reply!";
            return false;
        }
    } else {
        qCWarning(logApi) << "Request timed out (obtaining character_id)";
        return false;
    }

#ifdef QT_DEBUG
    qCDebug(logApi) << result.toString();
#endif

    return true;
}


bool eveapi_refresh_access_token(EveOAuthTokens& tokens)
{
    QTimer req_timer;
    QEventLoop local_event_loop;
    QNetworkAccessManager nam;

    // check for proxy settings
    QmlEvemonApp *gApp = globalAppInstance();
    if (gApp) {
        AppSettings *settings = gApp->settings();
        if (settings->isProxyEnabled()) {
            nam.setProxy(settings->proxySettings());
        }
    }

    QObject::connect(&nam, &QNetworkAccessManager::finished,
                     &local_event_loop, &QEventLoop::quit);
    QObject::connect(&req_timer, &QTimer::timeout,
                     &local_event_loop, &QEventLoop::quit);

    QNetworkRequest req;
    req.setUrl(QUrl(QLatin1String("https://login.eveonline.com/oauth/token")));

    // create Authorization: Basic ... OAuth header
    // The header should be the string “Basic ” plus the Base64
    //     encoded string {client_id}:{client_secret}.
    QByteArray auth_data = evesso_client_id() + ":" + evesso_client_secret_key();
    QByteArray auth_header = "Basic " + auth_data.toBase64();
    req.setHeader(QNetworkRequest::UserAgentHeader, evesso_user_agent());
    req.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    req.setRawHeader("Authorization", auth_header);

    QByteArray post_data = "grant_type=refresh_token&refresh_token=" + tokens.refresh_token;

    req_timer.setSingleShot(true);
    req_timer.setInterval(15000); // 15 seconds
    req_timer.start();
    qCDebug(logApi) << "Refreshing token...";
    QScopedPointer<QNetworkReply> rep(nam.post(req, post_data));
    local_event_loop.exec();

    // here, either the timer timed out, or request finished
    if (rep->isFinished()) {
        // OK, request did not time out
        qCDebug(logApi) << "Refresh request finished";
        req_timer.stop();
        // read reply
        QByteArray replyJson = rep->readAll();
        /* {
            "access_token":"uNEEh...a_WpiaA2",
            "token_type":"Bearer",
            "expires_in":1200,
            "refresh_token":"gEy...fM0"
        } */
        QJsonObject jobj = QJsonDocument::fromJson(replyJson).object();
        if (jobj.contains(QLatin1String("access_token"))
                && jobj.contains(QLatin1String("refresh_token"))
                && jobj.contains(QLatin1String("expires_in"))) {
            // correct reply
            qCDebug(logApi) << "Token refresh OK, got all the tokens";
            // get data
            int expires_in = jobj.value(QLatin1String("expires_in")).toInt();
            QDateTime expire_dt = QDateTime::currentDateTime().addSecs(expires_in);
            tokens.access_token = jobj.value(QLatin1String("access_token")).toString().toUtf8();
            tokens.refresh_token = jobj.value(QLatin1String("refresh_token")).toString().toUtf8();
            tokens.expire_dt = expire_dt;
        } else {
            // incorrect reply
            qCWarning(logApi) << "EVE OAuth replied with error! (refresh token)";
            qCWarning(logApi) << replyJson;
            return false;
        }
    } else {
        qCWarning(logApi) << "Refresh token request timed out";
        return false;
    }
    return true;
}


EveApi::EveApi(QObject *parent):
    QObject(parent),
    m_nam(new QNetworkAccessManager(this))
{
    m_esi_base_url = QLatin1String("https://esi.tech.ccp.is/latest");

    // check for proxy settings
    QmlEvemonApp *gApp = globalAppInstance();
    if (gApp) {
        QObject::connect(gApp, &QmlEvemonApp::settingsChanged,
                         this, &EveApi::onProxySettingsChanged);
    }
    // call slot to update proxy settings
    onProxySettingsChanged();
}


EveApi::~EveApi()
{
    // nothing?
}

void EveApi::onProxySettingsChanged()
{
    QmlEvemonApp *gApp = globalAppInstance();
    if (gApp) {
        AppSettings *settings = gApp->settings();
        if (settings->isProxyEnabled()) {
            m_nam->setProxy(settings->proxySettings());
        } else {
            m_nam->setProxy(QNetworkProxy::NoProxy);
        }
    }
}


bool EveApi::send_general_esi_request(
        // input arguments
        EsiReqType rtype,
        const QString& api_url,
        const QUrlQuery& get_params,
        const QByteArray& post_data,
        const QMap<QByteArray, QByteArray>& req_headers,
        int timeout_seconds,
        const QByteArray &access_token,
        // output arguments
        int& reply_http_status,
        QMap<QByteArray, QByteArray>& reply_headers,
        QByteArray& replyBa)
{
    replyBa.clear();
    QTimer req_timer;
    QEventLoop local_event_loop;

    QObject::connect(m_nam, &QNetworkAccessManager::finished,
                     &local_event_loop, &QEventLoop::quit);
    QObject::connect(&req_timer, &QTimer::timeout,
                     &local_event_loop, &QEventLoop::quit);

    // construct request URL
    QUrl url(m_esi_base_url + api_url);
    if (!get_params.isEmpty()) {
        url.setQuery(get_params);
    }

    // construct request object
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, evesso_user_agent());
    if (!access_token.isEmpty())
        req.setRawHeader("Authorization", QByteArray("Bearer ") + access_token);
    // set any custom headers
    if (!req_headers.isEmpty()) {
        const QList<QByteArray>& keys = req_headers.keys();
        for (const QByteArray& key: keys) {
            req.setRawHeader(key, req_headers.value(key));
        }
    }

    // init timer
    req_timer.setSingleShot(true);
    req_timer.setInterval(1000 * timeout_seconds);
    req_timer.start();

    QScopedPointer<QNetworkReply> reply(Q_NULLPTR);
    switch (rtype)
    {
        case EsiReqType::GET: reply.reset(m_nam->get(req)); break;
        case EsiReqType::POST: reply.reset(m_nam->post(req, post_data)); break;
    }

    local_event_loop.exec();

    if (reply->isFinished()) {
        req_timer.stop();
        reply_http_status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        replyBa = reply->readAll();
    } else {
        qCWarning(logApi) << "ESI request:" << url.toString()
                          << ": timed out.";
        return false;
    }

    const QList<QByteArray>& hlist = reply->rawHeaderList();
    reply_headers.clear();
    for (const QByteArray& rawheader: hlist) {
        reply_headers.insert(rawheader, reply->rawHeader(rawheader));
    }

    return true;
}


// an overload with several parameters omitted
bool EveApi::send_general_esi_request(
        // input arguments
        EsiReqType rtype,
        const QString& api_url,
        const QUrlQuery& get_params,
        const QByteArray& post_data,
        int timeout_seconds,
        const QByteArray &access_token,
        // output arguments
        int& reply_http_status,
        QByteArray& replyBa)
{
    QMap<QByteArray, QByteArray> req_headers;
    QMap<QByteArray, QByteArray> reply_headers;
    return this->send_general_esi_request(
                rtype, api_url, get_params, post_data,
                req_headers, timeout_seconds, access_token,
                reply_http_status, reply_headers, replyBa);
}


// another overload returning JSON directly
bool EveApi::send_general_esi_request_json(
        // input arguments
        EsiReqType rtype,
        const QString& api_url,
        const QUrlQuery& get_params,
        const QByteArray& post_data,
        int timeout_seconds,
        const QByteArray &access_token,
        // output arguments
        int& reply_http_status,
        QJsonDocument& replyJson)
{
    QMap<QByteArray, QByteArray> req_headers;
    QMap<QByteArray, QByteArray> reply_headers;
    QByteArray replyBa;
    bool req_ok = this->send_general_esi_request(
                rtype, api_url, get_params,
                post_data, req_headers, timeout_seconds,
                access_token, reply_http_status,
                reply_headers, replyBa);
    if (!req_ok) {
        replyJson = QJsonDocument();
        return false;
    }
    // parse JSON
    QJsonParseError parse_error;
    replyJson = QJsonDocument::fromJson(replyBa, &parse_error);
    if (replyJson.isNull()) { // parse failed
        qCWarning(logApi) << "ESI request: " << api_url
                          << "; JSON parse error:" << parse_error.errorString()
                          << "; HTTP status:" << reply_http_status;
        qCWarning(logApi) << "Reply was:" << replyBa;
        return false;
    }
    return true;
}


bool EveApi::get_server_status(QJsonObject& reply)
{
    QJsonDocument reply_doc;
    int reply_http_status = 0;
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, QLatin1String("/status/"),
                QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, reply_doc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!reply_doc.isObject()) return false;
    reply = reply_doc.object();
    return true;
}


static QString convert_ids_to_string(const QVector<quint64> &ids) {
    QString ret;
    for (quint64 id: ids) {
        if (!ret.isEmpty()) {
            ret.append(QLatin1Char(','));
        }
        ret.append(QString::number(id));
    }
    return ret;
}


bool EveApi::get_characters_names(QJsonArray &replyArr, const QVector<quint64> &ids)
{
    QJsonDocument reply_doc;
    int reply_http_status = 0;
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QLatin1String("character_ids"), convert_ids_to_string(ids));
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, QLatin1String("/characters/names/"),
                urlQuery, QByteArray(), 15, QByteArray(),
                reply_http_status, reply_doc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!reply_doc.isArray()) return false;
    replyArr = reply_doc.array();
    //   [{
    //      "character_id": 91205062,
    //      "character_name": "Lexx Min"
    //    }, ...]
    return true;
}

bool EveApi::get_corporations_names(QJsonArray &replyArr, const QVector<quint64> &ids)
{
    QJsonDocument reply_doc;
    int reply_http_status = 0;
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QLatin1String("corporation_ids"), convert_ids_to_string(ids));
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, QLatin1String("/corporations/names/"),
                urlQuery, QByteArray(), 15, QByteArray(),
                reply_http_status, reply_doc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!reply_doc.isArray()) return false;
    replyArr = reply_doc.array();
    //   [{
    //      "corporation_id": 98441646,
    //      "corporation_name": "Ice Nine."
    //    }, ...]
    return true;
}

bool EveApi::get_alliances_names(QJsonArray &replyArr, const QVector<quint64> &ids)
{
    QJsonDocument reply_doc;
    int reply_http_status = 0;
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QLatin1String("alliance_ids"), convert_ids_to_string(ids));
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, QLatin1String("/alliances/names/"),
                urlQuery, QByteArray(), 15, QByteArray(),
                reply_http_status, reply_doc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!reply_doc.isArray()) return false;
    replyArr = reply_doc.array();
    //   [{
    //      "alliance_id": 1683694957,
    //      "alliance_name": "COASTAL BROTHERHOOD"
    //    }, ...]
    return true;
}


bool EveApi::get_character_attributes(QJsonObject& reply, quint64 char_id, const QByteArray& access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/attributes/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}

bool EveApi::get_character_clones(QJsonObject &reply, quint64 char_id, const QByteArray &access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/clones/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}

bool EveApi::get_character_fatigue(QJsonObject &reply, quint64 char_id, const QByteArray &access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/fatigue/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}

bool EveApi::get_character_implants(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/implants/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isArray()) return false;
    replyArr = replyJson.array();
    return true;
}


bool EveApi::get_character_public_info(QJsonObject& reply, quint64 char_id)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}


bool EveApi::get_character_location(QJsonObject& reply, quint64 char_id, const QByteArray& access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/location/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}

bool EveApi::get_character_mail_headers(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token, quint64 last_mail_id)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/mail/")).arg(char_id);
    QUrlQuery query;
    if (last_mail_id > 0) {
        query.addQueryItem(QLatin1String("last_mail_id"), QString::number(last_mail_id));
    }
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, query, QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isArray()) return false;
    replyArr = replyJson.array();
    return true;
}

bool EveApi::get_character_mail_id(QJsonObject &reply, quint64 char_id, const QByteArray &access_token, quint64 mail_id)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/mail/%2/")).arg(char_id).arg(mail_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}

bool EveApi::get_character_mail_labels(QJsonObject &reply, quint64 char_id, const QByteArray &access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/mail/labels/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}

bool EveApi::get_character_mailing_lists(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/mail/lists/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isArray()) return false;
    replyArr = replyJson.array();
    return true;
}


bool EveApi::get_character_ship(QJsonObject& reply, quint64 char_id, const QByteArray& access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/ship/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}


bool EveApi::get_character_skillqueue(QJsonArray& replyArr, quint64 char_id, const QByteArray& access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/skillqueue/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isArray()) return false;
    replyArr = replyJson.array();
    return true;
}


bool EveApi::get_character_skills(QJsonObject& reply, quint64 char_id, const QByteArray& access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/skills/")).arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}


bool EveApi::get_character_wallet(float& reply, quint64 char_id, const QByteArray& access_token)
{
    // /characters/91205062/wallet/
    QByteArray replyBa;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/wallet/")).arg(char_id);
    bool req_ok = this->send_general_esi_request(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, replyBa);
    if (!req_ok || (reply_http_status != 200)) return false;
    bool ok = false;
    reply = replyBa.toFloat(&ok);
    return ok;
}

bool EveApi::get_character_wallet_journal(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token, int page)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/wallet/journal/")).arg(char_id);
    QUrlQuery query;
    if (page > 0) {
        query.addQueryItem(QLatin1String("page"), QString::number(page));
    }
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, query, QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    bool ok = false;
    replyArr = replyJson.array();
    return ok;
}

bool EveApi::get_character_wallet_transactions(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token, quint64 from_id)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/characters/%1/wallet/transactions/")).arg(char_id);
    QUrlQuery query;
    if (from_id > 0) {
        query.addQueryItem(QLatin1String("from_id"), QString::number(from_id));
    }
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, query, QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    bool ok = false;
    replyArr = replyJson.array();
    return ok;
}


bool EveApi::get_corporation_public_data(QJsonObject& reply, quint64 corp_id)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/corporations/%1/")).arg(corp_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}


bool EveApi::get_alliance_public_data(QJsonObject& reply, quint64 ally_id)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString(QLatin1String("/alliances/%1/")).arg(ally_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}


bool EveApi::get_universe_bloodlines(QJsonArray& replyArr)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString(QLatin1String("/universe/bloodlines/"));
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isArray()) return false;
    replyArr = jdoc.array();
    return true;
}


bool EveApi::get_universe_constellation(QJsonObject& reply, quint64 constellation_id)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString(QLatin1String("/universe/constellations/%1/")).arg(constellation_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    return true;
}


bool EveApi::get_universe_races(QJsonArray& replyArr)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString(QLatin1String("/universe/races/"));
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isArray()) return false;
    replyArr = jdoc.array();
    return true;
}


bool EveApi::get_universe_region(QJsonObject& reply, quint64 region_id)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString(QLatin1String("/universe/regions/%1/")).arg(region_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    return true;
}


bool EveApi::get_universe_station(QJsonObject& reply, quint64 station_id)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString(QLatin1String("/universe/stations/%1/")).arg(station_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    /* Example reply: (station_id = 60003466)
        {
          "station_id": 60003466,
          "name": "Jita IV - Moon 4 - Caldari Business Tribunal Bureau Offices",
          "type_id": 1529,
          "position": {
            "x": -107302625280,
            "y": -18745221120,
            "z": 436489789440
          },
          "system_id": 30000142,
          "reprocessing_efficiency": 0.5,
          "reprocessing_stations_take": 0.05,
          "max_dockable_ship_volume": 50000000,
          "office_rental_cost": 41727534,
          "services": [
            "bounty-missions",
            "courier-missions",
            "interbus",
            "reprocessing-plant",
            "market",
            "stock-exchange",
            "cloning",
            "fitting",
            "news",
            "insurance",
            "docking",
            "office-rental",
            "loyalty-point-store",
            "navy-offices"
          ],
          "owner": 1000033,
          "race_id": 1
        }
     * */
    return true;
}


bool EveApi::get_universe_structure(QJsonObject& reply, quint64 structure_id, const QByteArray& access_token)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString(QLatin1String("/universe/structures/%1/")).arg(structure_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, access_token,
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    /* Example reply: (structure_id = 1021551599501)
     * {
     *   "name": "J215758 - Tiamat",
     *   "solar_system_id": 31001668,
     *   "type_id": 35833,
     *   "position": {
     *      "x": -81264264086,
     *      "y": -2002654044,
     *      "z": 107567112590
     *    }
     * } */
    return true;
}


bool EveApi::get_universe_system(QJsonObject& reply, quint64 ss_id)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString(QLatin1String("/universe/systems/%1/")).arg(ss_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    return true;
}


bool EveApi::get_universe_typeid(QJsonObject& reply, quint64 type_id)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString(QLatin1String("/universe/types/%1/")).arg(type_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QUrlQuery(), QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    return true;
}


} // namespace
