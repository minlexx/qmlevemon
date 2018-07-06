#ifndef H_EVE_API
#define H_EVE_API

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QUrl>
#include <QUrlQuery>
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

bool eveapi_request_first_access_token(const QString &code,
                                       EveOAuthTokens &result,
                                       quint64 &character_id);

bool eveapi_refresh_access_token(EveOAuthTokens &tokens);


class EveApi: public QObject
{
    Q_OBJECT
public:
    enum class EsiReqType {
        GET, POST
    };

    explicit EveApi(QObject *parent = Q_NULLPTR);
    ~EveApi() override;

public:
    QNetworkAccessManager *nam();

    QString esiBaseUrl() const;
    void setEsiBaseUrl(const QString &esiUrl);

    int timeoutSecs() const;
    void setTimeoutSecs(int t);

    int maxAutoRetries() const;
    void setAutoRetry(int max_retries);

protected Q_SLOTS:
    void onProxySettingsChanged();

public:
    // character
    bool get_character_attributes(QJsonObject &reply, quint64 char_id, const QByteArray &access_token);
    bool get_character_clones(QJsonObject &reply, quint64 char_id, const QByteArray &access_token);
    bool get_character_fatigue(QJsonObject &reply, quint64 char_id, const QByteArray &access_token);
    bool get_character_implants(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token);
    bool get_character_location(QJsonObject &reply, quint64 char_id, const QByteArray &access_token);
    bool get_character_mail_headers(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token, quint64 last_mail_id = 0);
    bool get_character_mail_id(QJsonObject &reply, quint64 char_id, const QByteArray &access_token, quint64 mail_id);
    bool get_character_mail_labels(QJsonObject &reply, quint64 char_id, const QByteArray &access_token);
    bool get_character_mailing_lists(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token);
    bool get_character_notifications(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token);
    bool get_character_public_info(QJsonObject &reply, quint64 char_id);
    bool get_character_ship(QJsonObject &reply, quint64 char_id, const QByteArray &access_token);
    bool get_character_skillqueue(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token);
    bool get_character_skills(QJsonObject &reply, quint64 char_id, const QByteArray &access_token);
    bool get_character_wallet(float &reply, quint64 char_id, const QByteArray &access_token);
    bool get_character_wallet_journal(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token, int page = 0);
    bool get_character_wallet_transactions(QJsonArray &replyArr, quint64 char_id, const QByteArray &access_token, quint64 from_id = 0);
    // corporation, alliance
    bool get_corporation_public_data(QJsonObject &reply, quint64 corp_id);
    bool get_alliance_public_data(QJsonObject &reply, quint64 ally_id);
    // common
    bool get_server_status(QJsonObject &reply);
    // names resolving
    bool get_characters_names(QJsonArray &replyArr, const QVector<quint64> &ids);
    bool get_corporations_names(QJsonArray &replyArr, const QVector<quint64> &ids);
    bool get_alliances_names(QJsonArray &replyArr, const QVector<quint64> &ids);
    // universe
    bool get_universe_bloodlines(QJsonArray &replyArr);
    bool get_universe_constellation(QJsonObject &reply, quint64 constellation_id);
    bool post_universe_names(QJsonArray &replyArr, const QVector<quint64> &ids);
    bool get_universe_races(QJsonArray &replyArr);
    bool get_universe_region(QJsonObject &reply, quint64 region_id);
    bool get_universe_station(QJsonObject &reply, quint64 station_id);
    bool get_universe_structure(QJsonObject &reply, quint64 structure_id, const QByteArray &access_token);
    bool get_universe_system(QJsonObject &reply, quint64 ss_id);
    bool get_universe_typeid(QJsonObject &reply, quint64 type_id);

protected:
    // send raw GET or POST request. return read bytearray
    bool send_general_esi_request(
            // input arguments
            EsiReqType rtype,
            const QString &api_url,
            const QUrlQuery &get_params,
            const QByteArray &post_data,
            const QMap<QByteArray, QByteArray> &req_headers, // any extra request headers
            const QByteArray &access_token, // empty string, if not needed
            // output arguments
            int &reply_http_status,  // returns HTTP status code, or -1 on timeout
            QMap<QByteArray, QByteArray> &reply_headers, // if you need
            QByteArray &replyBa);

    // an overload with HTTP req/reply headers parameters omitted
    bool send_general_esi_request_simplified(
            // input arguments
            EsiReqType rtype,
            const QString& api_url,
            const QUrlQuery& get_params,
            const QByteArray& post_data,
            const QByteArray &access_token,
            // output arguments
            int& reply_http_status,
            QByteArray& replyBa);

    // another variant returning JSON directly
    bool send_general_esi_request_json(
            // input arguments
            EsiReqType rtype,
            const QString& api_url,
            const QUrlQuery& get_params,
            const QByteArray& post_data,
            const QByteArray &access_token,
            // output arguments
            int& reply_http_status,
            QJsonDocument& replyJson);

protected:
    QNetworkAccessManager *m_nam = nullptr;
    QString m_esi_base_url;
    int m_timeoutSecs = 15;
    int m_max_retries = 5;
};


} // namespace


#endif
