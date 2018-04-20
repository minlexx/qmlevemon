#ifndef EM_CHARACTERCLONE_H
#define EM_CHARACTERCLONE_H

#include <QObject>
#include <QDebug>
#include "character_implants_group.h"
#include "eve_location.h"

class QDataStream;

// forward declare operators
namespace EM { class CharacterClone; }

QDataStream& operator<<(QDataStream &stream, const EM::CharacterClone &clone);
QDataStream& operator>>(QDataStream &stream, EM::CharacterClone &clone);
QDebug operator<<(QDebug stream, const EM::CharacterClone &clone);


namespace EM {


class CharacterClone : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cloneName      READ cloneName     WRITE setCloneName     NOTIFY cloneChanged)
    Q_PROPERTY(quint64 cloneId        READ cloneId       WRITE setCloneId       NOTIFY cloneChanged)
    Q_PROPERTY(QObject* location      READ locationObj   NOTIFY locationChanged)
    Q_PROPERTY(QObject* implantsModel READ implantsModel CONSTANT)

public:
    CharacterClone(QObject *parent = nullptr);
    CharacterClone(const CharacterClone &other);
    CharacterClone(CharacterClone &&other);
    CharacterClone &operator=(const CharacterClone &other);
    CharacterClone &operator=(CharacterClone &&other);
    bool operator==(const CharacterClone &other) const;
    bool operator!=(const CharacterClone &other) const;

public:
    QString cloneName() const;
    quint64 cloneId() const;
    QObject *locationObj();
    const QObject *locationObj() const;
    EveLocation *location();
    const EveLocation *location() const;
    QObject* implantsModel();
    const QObject* implantsModel() const;
    CharacterImplantsGroup *implants();

public Q_SLOTS:
    void setCloneName(const QString &name);
    void setCloneId(quint64 id);
    void setLocation(const EveLocation &loc);
    void setImplantsGroup(const CharacterImplantsGroup &imps);
    void setImplantsGroup(CharacterImplantsGroup &&imps);

Q_SIGNALS:
    void cloneChanged();
    void locationChanged();
    void implantsChanged();

private:
    QString m_cloneName;
    quint64 m_cloneId = 0;
    EveLocation m_location;
    CharacterImplantsGroup m_implants;

    // friends
    friend QDataStream& (::operator<<)(QDataStream &stream, const EM::CharacterClone &clone);
    friend QDataStream& (::operator>>)(QDataStream &stream, EM::CharacterClone &clone);
    friend QDebug (::operator<<)(QDebug stream, const EM::CharacterClone &clone);
};


}

#endif // CHARACTERCLONE_H
