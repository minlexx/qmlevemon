#include "model_manager.h"
#include <QtGlobal>
#include <QGlobalStatic>
#include <QQmlContext>
#include <QLoggingCategory>
#include <QDebug>

// all models
#include "character_model.h"


Q_GLOBAL_STATIC(EM::ModelManager, g_modelManager)

Q_LOGGING_CATEGORY(logModels, "evemon.models")


EM::ModelManager *EM::ModelManager::instance()
{
    return g_modelManager();
}


EM::ModelManager::ModelManager(QObject *parent):
    QObject(parent),
    m_characterModel(Q_NULLPTR)
{
    qCDebug(logModels) << "ModelManager ctor";
    m_characterModel = new EM::CharacterModel(this);
    m_characterModel->loadCharacters();
}


void EM::ModelManager::registerModelsAsContextProperties(QQmlContext *ctx)
{
    ctx->setContextProperty(QLatin1String("characterModel"), m_characterModel);
}
