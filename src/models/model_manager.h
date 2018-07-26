#ifndef H_EM_MODEL_MANAGER
#define H_EM_MODEL_MANAGER

#include <QObject>
#include <QTimer>

namespace EM {


class CharacterModel;
class SkillTreeModel;

class ModelManager: public QObject
{
    Q_OBJECT

public:
    explicit ModelManager(QObject *parent = Q_NULLPTR);
    static ModelManager *instance();

    void initModels();

public:
    CharacterModel *characterModel() { return m_characterModel; }
    SkillTreeModel *skillTreeModel() { return m_skillTreeModel; }

private:
    CharacterModel *m_characterModel = nullptr;
    SkillTreeModel *m_skillTreeModel = nullptr;

    QTimer m_characterModelRecalcTimer;
};


} // namespace


#endif
