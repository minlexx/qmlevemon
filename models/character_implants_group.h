#ifndef CHARACTERIMPLANTSGROUP_H
#define CHARACTERIMPLANTSGROUP_H

#include <QObject>
#include <QVector>
#include <QDataStream>
#include <QDebug>
#include "invtype.h"


namespace EM { class CharacterImplantsGroup; }

QDataStream& operator<<(QDataStream &stream, const EM::CharacterImplantsGroup &impGroup);
QDataStream& operator>>(QDataStream &stream, EM::CharacterImplantsGroup &impGroup);
QDebug operator<<(QDebug stream, const EM::CharacterImplantsGroup &impGroup);


namespace EM {


class CharacterImplantsGroup : public QObject
{
    Q_OBJECT
public:
    CharacterImplantsGroup(QObject *parent = nullptr);
    CharacterImplantsGroup(const CharacterImplantsGroup &other);
    CharacterImplantsGroup(CharacterImplantsGroup &&other);
    CharacterImplantsGroup &operator=(const CharacterImplantsGroup &other);
    CharacterImplantsGroup &operator=(CharacterImplantsGroup &&other);

private:
    QVector<InvType> m_implants;

    friend QDataStream& (::operator<<)(QDataStream &stream, const CharacterImplantsGroup &impGroup);
    friend QDataStream& (::operator>>)(QDataStream &stream, CharacterImplantsGroup &impGroup);
    friend QDebug (::operator<<)(QDebug stream, const EM::CharacterImplantsGroup &impGroup);
};


} // namespace


#endif // CHARACTERIMPLANTSGROUP_H
