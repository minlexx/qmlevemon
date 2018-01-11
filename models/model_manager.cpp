#include "model_manager.h"
#include <QtGlobal>
#include <QGlobalStatic>
#include <QLoggingCategory>
#include <QDebug>

// all models
#include "character_model.h"


Q_LOGGING_CATEGORY(logModels, "evemon.models")

namespace EM {

Q_GLOBAL_STATIC(ModelManager, g_modelManager)


ModelManager *ModelManager::instance()
{
    return g_modelManager();
}


ModelManager::ModelManager(QObject *parent):
    QObject(parent),
    m_characterModel(Q_NULLPTR)
{
    qCDebug(logModels) << "ModelManager ctor";
    m_characterModel = new CharacterModel(this);
    m_characterModel->loadCharacters();
}

}
