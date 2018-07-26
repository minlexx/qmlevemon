#ifndef H_EVEAPI_TOKENS
#define H_EVEAPI_TOKENS

#include <QString>
#include <QDateTime>
#include <QByteArray>


class QDataStream;


namespace EM {


class EveOAuthTokens
{
public:
    EveOAuthTokens();
    EveOAuthTokens(const EveOAuthTokens &other);
    EveOAuthTokens(EveOAuthTokens &&other);
    EveOAuthTokens& operator=(const EveOAuthTokens& other);
    EveOAuthTokens& operator=(EveOAuthTokens&& other);
    bool operator==(const EveOAuthTokens& other);
    QString toString() const;
    bool needsRefresh() const;

public:
    QByteArray access_token;
    QByteArray refresh_token;
    QDateTime expire_dt;
};


} // namespace


// serializing functions
QDataStream& operator<<(QDataStream& stream, const EM::EveOAuthTokens& tokens);
QDataStream& operator>>(QDataStream& stream, EM::EveOAuthTokens& tokens);


#endif
