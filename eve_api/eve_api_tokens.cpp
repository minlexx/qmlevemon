#include <QDataStream>
#include "eve_api_tokens.h"


namespace EM {

EveOAuthTokens::EveOAuthTokens()
{
    //
}


EveOAuthTokens::EveOAuthTokens(const EveOAuthTokens &other)
{
    (*this) = other;
}


EveOAuthTokens& EveOAuthTokens::operator=(const EveOAuthTokens& other)
{
    if (this == &other) return (*this);
    this->access_token = other.access_token;
    this->refresh_token = other.refresh_token;
    this->expire_dt = other.expire_dt;
    return (*this);
}


bool EveOAuthTokens::operator==(const EveOAuthTokens& other)
{
    if (this == &other) return true;
    if (this->access_token != other.access_token) return false;
    if (this->refresh_token != other.refresh_token) return false;
    if (this->expire_dt != other.expire_dt) return false;
    return true;
}


QString EveOAuthTokens::toString() const
{
    QString ret(QLatin1String("<EveEveOAuthTokens "));
    ret.append(QLatin1String("access_token="));
    ret.append(QString::fromUtf8(access_token));
    ret.append(QLatin1String("; refresh_token="));
    ret.append(QString::fromUtf8(refresh_token));
    ret.append(QLatin1String("; expire_dt="));
    ret.append(expire_dt.toString(Qt::ISODate));
    ret.append(QLatin1String(">"));
    return ret;
}


bool EveOAuthTokens::needsRefresh() const
{
    if (this->expire_dt <= QDateTime::currentDateTime()) return true;
    return false;
}

} // namespace



QDataStream& operator<<(QDataStream& stream, const EM::EveOAuthTokens& tokens)
{
    stream << tokens.access_token;
    stream << tokens.refresh_token;
    stream << tokens.expire_dt;
    return stream;
}


QDataStream& operator>>(QDataStream& stream, EM::EveOAuthTokens& tokens)
{
    stream >> tokens.access_token;
    stream >> tokens.refresh_token;
    stream >> tokens.expire_dt;
    return stream;
}
