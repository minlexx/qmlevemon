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
        ISKAmountStr,
        // skills info
        TrainingSkill,
        TrainingSkillTimeLeft,
        TrainingSkillEndDateTime,
        QueueTimeLeft
    };

public:
    explicit CharacterModel(QObject *parent = Q_NULLPTR);
    ~CharacterModel() override;
    void loadCharacters();
    void addNewCharacter(Character *character);
    QList<Character *> getCharacters() const;
    // emit signal to model clients that some character has changed data
    void markCharacterAsUpdated(Character *character);
    Character *findCharacterById(quint64 char_id);

public:  // reimplmented interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void newCharacterAdded();

private:
    QHash<int, QByteArray> m_roles;
    QList<Character *> m_characterList;
    mutable QMutex m_mutex;
};


} // namespace


#endif
