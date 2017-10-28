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
    ret.append(EM::globalAppInstance()->applicationName().toUtf8());
    ret.append("/");
    ret.append(EM::globalAppInstance()->applicationVersion().toUtf8());
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

    QObject::connect(&nam, &QNetworkAccessManager::finished,
                     &local_event_loop, &QEventLoop::quit);
    QObject::connect(&req_timer, &QTimer::timeout,
                     &local_event_loop, &QEventLoop::quit);

    QNetworkRequest req;
    req.setUrl(QUrl("https://login.eveonline.com/oauth/token"));

    // create Authorization: Basic ... OAuth header
    // The header should be the string “Basic ” plus the Base64
    //     encoded string {client_id}:{client_secret}.
    QByteArray auth_data = evesso_client_id() + ":" + evesso_client_secret_key();
    QByteArray auth_header = "Basic " + auth_data.toBase64();
    req.setHeader(QNetworkRequest::UserAgentHeader, evesso_user_agent());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
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
        if (jobj.contains("access_token") && jobj.contains("refresh_token")
                && jobj.contains("expires_in")) {
            // correct reply
            qCDebug(logApi) << "Login OK, got all the tokens";
            // get data
            int expires_in = jobj.value("expires_in").toInt();
            QDateTime expire_dt = QDateTime::currentDateTime().addSecs(expires_in);
            result.access_token = jobj.value("access_token").toString().toUtf8();
            result.refresh_token = jobj.value("refresh_token").toString().toUtf8();
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
    req2.setUrl(QUrl("https://login.eveonline.com/oauth/verify"));
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
        if (jobj.contains("CharacterID")) {
            character_id = jobj.value("CharacterID").toVariant().toULongLong();
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

    QObject::connect(&nam, &QNetworkAccessManager::finished,
                     &local_event_loop, &QEventLoop::quit);
    QObject::connect(&req_timer, &QTimer::timeout,
                     &local_event_loop, &QEventLoop::quit);

    QNetworkRequest req;
    req.setUrl(QUrl("https://login.eveonline.com/oauth/token"));

    // create Authorization: Basic ... OAuth header
    // The header should be the string “Basic ” plus the Base64
    //     encoded string {client_id}:{client_secret}.
    QByteArray auth_data = evesso_client_id() + ":" + evesso_client_secret_key();
    QByteArray auth_header = "Basic " + auth_data.toBase64();
    req.setHeader(QNetworkRequest::UserAgentHeader, evesso_user_agent());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
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
        if (jobj.contains("access_token") && jobj.contains("refresh_token")
                && jobj.contains("expires_in")) {
            // correct reply
            qCDebug(logApi) << "Token refresh OK, got all the tokens";
            // get data
            int expires_in = jobj.value("expires_in").toInt();
            QDateTime expire_dt = QDateTime::currentDateTime().addSecs(expires_in);
            tokens.access_token = jobj.value("access_token").toString().toUtf8();
            tokens.refresh_token = jobj.value("refresh_token").toString().toUtf8();
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
}


EveApi::~EveApi()
{
    // nothing?
}


bool EveApi::send_general_esi_request(
        // input arguments
        EsiReqType rtype,
        const QString& api_url,
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

    QNetworkRequest req;
    QUrl url(m_esi_base_url + api_url);
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
                rtype, api_url, post_data, req_headers, timeout_seconds,
                access_token, reply_http_status, reply_headers, replyBa);
}


// another overload returning JSON directly
bool EveApi::send_general_esi_request_json(
        // input arguments
        EsiReqType rtype,
        const QString& api_url,
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
    bool req_ok = this->send_general_esi_request(rtype, api_url, post_data,
            req_headers, timeout_seconds, access_token, reply_http_status,
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
        return false;
    }
    return true;
}


bool EveApi::get_server_status(QJsonObject& reply)
{
    QJsonDocument reply_doc;
    int reply_http_status = 0;
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, "/status/", QByteArray(), 15, QByteArray(),
                reply_http_status, reply_doc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!reply_doc.isObject()) return false;
    reply = reply_doc.object();
    return true;
}


bool EveApi::get_character_public_info(QJsonObject& reply, quint64 char_id)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString("/characters/%1/").arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
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
    QString url = QString("/characters/%1/location/").arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, access_token,
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}


bool EveApi::get_character_ship(QJsonObject& reply, quint64 char_id, const QByteArray& access_token)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString("/characters/%1/ship/").arg(char_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, access_token,
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
    QString url = QString("/characters/%1/wallet/").arg(char_id);
    bool req_ok = this->send_general_esi_request(
                EsiReqType::GET, url, QByteArray(), 15, access_token,
                reply_http_status, replyBa);
    if (!req_ok || (reply_http_status != 200)) return false;
    bool ok = false;
    reply = replyBa.toFloat(&ok);
    return ok;
}


bool EveApi::get_corporation_public_data(QJsonObject& reply, quint64 corp_id)
{
    QJsonDocument replyJson;
    int reply_http_status = 0;
    QString url = QString("/corporations/%1/").arg(corp_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
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
    QString url = QString("/alliances/%1/").arg(ally_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
                reply_http_status, replyJson);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!replyJson.isObject()) return false;
    reply = replyJson.object();
    return true;
}


bool EveApi::get_universe_bloodlines(QJsonArray& reply)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString("/universe/bloodlines/");
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isArray()) return false;
    reply = jdoc.array();
    return true;
}


bool EveApi::get_universe_constellation(QJsonObject& reply, quint64 constellation_id)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString("/universe/constellations/%1/").arg(constellation_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    return true;
}


bool EveApi::get_universe_races(QJsonArray& reply)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString("/universe/races/");
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isArray()) return false;
    reply = jdoc.array();
    return true;
}


bool EveApi::get_universe_region(QJsonObject& reply, quint64 region_id)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString("/universe/regions/%1/").arg(region_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
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
    QString url = QString("/universe/stations/%1/").arg(station_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    return true;
}


bool EveApi::get_universe_structure(QJsonObject& reply, quint64 structure_id, const QByteArray& access_token)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString("/universe/structures/%1/").arg(structure_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, access_token,
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    return true;
}


bool EveApi::get_universe_system(QJsonObject& reply, quint64 ss_id)
{
    QJsonDocument jdoc;
    int reply_http_status = 0; // √
    QString url = QString("/universe/systems/%1/").arg(ss_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
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
    QString url = QString("/universe/types/%1/").arg(type_id);
    bool req_ok = this->send_general_esi_request_json(
                EsiReqType::GET, url, QByteArray(), 15, QByteArray(),
                reply_http_status, jdoc);
    if (!req_ok || (reply_http_status != 200)) return false;
    if (!jdoc.isObject()) return false;
    reply = jdoc.object();
    return true;
}


} // namespace
