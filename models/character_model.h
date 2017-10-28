#ifndef H_EM_CHARACTER_MODEL
#define H_EM_CHARACTER_MODEL

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QAbstractListModel>
#include <QList>
#include <QMutex>


namespace EM {


class Character;

class CharacterModel: public QAbstractListModel
{
    Q_OBJECT

    enum Roles {
        // general info
        CharacterId = Qt::UserRole + 1,
        CharacterName,
        CorporationId,
        CorporationName,
        CorporationTicker,
        AllianceId,
        AllianceName,
        AllianceTicker,
        Race,
        Ancestry,
        Bloodline,
        Gender,
        Birthday,
        SecurityStatus,
        Bio,
        // wallet info
        ISK,
        // skills info
        TrainingSkill,
        TrainingSkillTimeLeft,
        TrainingSkillEndDateTime,
        QueueTimeLeft
    };

public:
    explicit CharacterModel(QObject *parent = Q_NULLPTR);
    ~CharacterModel();
    void loadCharacters();
    void addNewCharacter(Character *character);
    QList<Character *> getCharacters() const;
    // emit signal to model clients that some character has changed data
    void markCharacterAsUpdated(Character *character);
    Character *findCharacterById(int char_id);

public:  // reimplmented interface
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

Q_SIGNALS:
    void newCharacterAdded();

private:
    QHash<int, QByteArray> m_roles;
    QList<Character *> m_characterList;
    QMutex m_mutex;
};


} // namespace


#endif
