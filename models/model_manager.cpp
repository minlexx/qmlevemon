#include "model_manager.h"
#include <QtGlobal>
#include <QGlobalStatic>
#include <QLoggingCategory>
#include <QDebug>

// all models
#include "character_model.h"
#include "skill_tree_model.h"


Q_LOGGING_CATEGORY(logModels, "evemon.models")

namespace EM {

Q_GLOBAL_STATIC(ModelManager, g_modelManager)


ModelManager *ModelManager::instance()
{
    return g_modelManager();
}


ModelManager::ModelManager(QObject *parent)
    : QObject(parent)
    , m_characterModel(new CharacterModel(this))
    , m_skillTreeModel(new SkillTreeModel(this))
{
    qCDebug(logModels) << "ModelManager ctor";

    m_characterModelRecalcTimer.setSingleShot(false);
    m_characterModelRecalcTimer.setInterval(5000);

    QObject::connect(&m_characterModelRecalcTimer, &QTimer::timeout,
                     m_characterModel, &CharacterModel::calcCharactersSkillQueue);
}


void ModelManager::initModels()
{
    // skills model should be loaded first,
    //   because it is used in characters model
    m_skillTreeModel->load();

    m_characterModel->loadCharacters();
}

}
