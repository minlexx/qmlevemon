#ifndef H_EM_MODEL_MANAGER
#define H_EM_MODEL_MANAGER

#include <QObject>

namespace EM {


class CharacterModel;

class ModelManager: public QObject
{
    Q_OBJECT

public:
    explicit ModelManager(QObject *parent = Q_NULLPTR);
    static ModelManager *instance();

public:
    CharacterModel *characterModel() { return m_characterModel; }

private:
    CharacterModel *m_characterModel;
};


} // namespace


#endif
