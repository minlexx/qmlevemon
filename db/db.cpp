#include "db.h"


EM::Db::Db()
{
    //
}


EM::Db::~Db()
{
    //
}


EM::Db::Db(const Db &other)
{
    Q_UNUSED(other);
}


EM::Db& EM::Db::operator=(const Db& other)
{
    Q_UNUSED(other)
    return (*this);
}
