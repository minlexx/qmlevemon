#ifndef CHARACTERIMPLANTSGROUP_H
#define CHARACTERIMPLANTSGROUP_H

#include <QObject>
#include <QVector>
#include "invtype.h"


namespace EM {


class CharacterImplantsGroup : public QObject
{
    Q_OBJECT
public:
    explicit CharacterImplantsGroup(QObject *parent = nullptr);

private:
    QVector<InvType> m_implants;
};


} // namespace

#endif // CHARACTERIMPLANTSGROUP_H
