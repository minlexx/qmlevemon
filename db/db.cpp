#include "db.h"


namespace EM {

Db::Db()
{
    //
}


Db::~Db()
{
    //
}


Db::Db(const Db &other)
{
    Q_UNUSED(other);
}


Db& Db::operator=(const Db& other)
{
    Q_UNUSED(other)
    return (*this);
}


}
