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
        TotalSP,
        TrainingSkill,
        TrainingSkillTimeLeft,
        TrainingSkillEndDateTime,
        QueueTimeLeft,
        QueueFinishDateTime,
        IsQueueEmpty
    };

public:
    explicit CharacterModel(QObject *parent = Q_NULLPTR);
    ~CharacterModel() override;
    void loadCharacters();
    void addNewCharacter(Character *character);
    void removeCharacter(quint64 char_id);
    QList<Character *> getCharacters() const;
    // emit signal to model clients that some character has changed data
    Character *findCharacterById(quint64 char_id);

public:  // reimplmented interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public Q_SLOTS:
    void updateCharacter(const Character *character);
    void calcCharactersSkillQueue();

Q_SIGNALS:
    void newCharacterAdded();
    void characterRemoved(quint64 char_id);

private:
    QHash<int, QByteArray> m_roles;
    QVector<Character *> m_characterList;
    mutable QMutex m_mutex;
};


} // namespace


#endif
