#ifndef EM_CHARACTERCLONE_H
#define EM_CHARACTERCLONE_H

#include <QObject>
#include <QDebug>
#include "character_implants_group.h"

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
    Q_PROPERTY(quint64 locationId     READ locationId    WRITE setLocationId    NOTIFY locationChanged)
    Q_PROPERTY(QString locationName   READ locationName  WRITE setLocationName  NOTIFY locationChanged)
    Q_PROPERTY(QString locationType   READ locationType  WRITE setLocationType  NOTIFY locationChanged)
    Q_PROPERTY(QObject* implantsModel READ implantsModel NOTIFY implantsChanged)
    // TODO: maybe make implantsModel CONSTANT and no need for signal implantsChanged
    // model will update itself and emit signals?

public:
    CharacterClone(QObject *parent = nullptr);
    CharacterClone(const CharacterClone &other);
    CharacterClone(CharacterClone &&other);
    CharacterClone &operator=(const CharacterClone &other);
    CharacterClone &operator=(CharacterClone &&other);

public:
    QString cloneName() const;
    quint64 cloneId() const;
    quint64 locationId() const;
    QString locationName() const;
    QString locationType() const;
    QObject* implantsModel();
    CharacterImplantsGroup *implants();

public Q_SLOTS:
    void setCloneName(const QString &name);
    void setCloneId(quint64 id);
    void setLocationId(quint64 id);
    void setLocationName(const QString &name);
    void setLocationType(const QString &typ);
    void setImplantsGroup(const CharacterImplantsGroup &imps);
    void setImplantsGroup(CharacterImplantsGroup &&imps);

Q_SIGNALS:
    void cloneChanged();
    void locationChanged();
    void implantsChanged();

private:
    QString m_cloneName;
    quint64 m_cloneId = 0;
    quint64 m_locationId = 0;
    QString m_locationName;
    QString m_locationType;
    CharacterImplantsGroup m_implants;

    // friends
    friend QDataStream& (::operator<<)(QDataStream &stream, const EM::CharacterClone &clone);
    friend QDataStream& (::operator>>)(QDataStream &stream, EM::CharacterClone &clone);
    friend QDebug (::operator<<)(QDebug stream, const EM::CharacterClone &clone);
};


}

#endif // CHARACTERCLONE_H
