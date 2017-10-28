#ifndef H_EVE_API
#define H_EVE_API

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include "eve_api_tokens.h"


class QNetworkAccessManager;


namespace EM {


QByteArray evesso_client_id();
QByteArray evesso_client_secret_key();
QByteArray evesso_user_agent();

bool eveapi_request_first_access_token(const QString& code,
                                       EveOAuthTokens& result,
                                       quint64& character_id);

bool eveapi_refresh_access_token(EveOAuthTokens& tokens);


class EveApi: public QObject
{
    Q_OBJECT
public:
    enum class EsiReqType {
        GET, POST
    };

    explicit EveApi(QObject *parent = Q_NULLPTR);
    virtual ~EveApi();

public:
    bool get_character_public_info(QJsonObject& reply, quint64 char_id);
    bool get_character_location(QJsonObject& reply, quint64 char_id, const QByteArray& access_token);
    bool get_character_ship(QJsonObject& reply, quint64 char_id, const QByteArray& access_token);
    bool get_character_wallet(float& reply, quint64 char_id, const QByteArray& access_token);
    bool get_corporation_public_data(QJsonObject& reply, quint64 corp_id);
    bool get_alliance_public_data(QJsonObject& reply, quint64 ally_id);
    // common
    bool get_server_status(QJsonObject& reply);
    // universe
    bool get_universe_bloodlines(QJsonArray& reply);
    bool get_universe_constellation(QJsonObject& reply, quint64 constellation_id);
    bool get_universe_races(QJsonArray& reply);
    bool get_universe_region(QJsonObject& reply, quint64 region_id);
    bool get_universe_station(QJsonObject& reply, quint64 station_id);
    bool get_universe_structure(QJsonObject& reply, quint64 structure_id, const QByteArray& access_token);
    bool get_universe_system(QJsonObject& reply, quint64 ss_id);
    bool get_universe_typeid(QJsonObject& reply, quint64 type_id);

protected:
    // send raw GET or POST request. return read bytearray
    bool send_general_esi_request(
            // input arguments
            EsiReqType rtype,
            const QString& api_url,
            const QByteArray& post_data,
            const QMap<QByteArray, QByteArray>& req_headers, // any extra headers
            int timeout_seconds,
            const QByteArray &access_token, // empty string, if not needed
            // output arguments
            int& reply_http_status,
            QMap<QByteArray, QByteArray>& reply_headers, // if you need
            QByteArray& replyBa);

    // an overload with HTTP req/reply headers parameters omitted
    bool send_general_esi_request(
            // input arguments
            EsiReqType rtype,
            const QString& api_url,
            const QByteArray& post_data,
            int timeout_seconds,
            const QByteArray &access_token,
            // output arguments
            int& reply_http_status,
            QByteArray& replyBa);

    // another variant returning JSON directly
    bool send_general_esi_request_json(
            // input arguments
            EsiReqType rtype,
            const QString& api_url,
            const QByteArray& post_data,
            int timeout_seconds,
            const QByteArray &access_token,
            // output arguments
            int& reply_http_status,
            QJsonDocument& replyJson);

protected:
    QNetworkAccessManager *m_nam;
    QString m_esi_base_url;
};


} // namespace


#endif
