#ifndef CHARACTER_CLONES_MODEL_H
#define CHARACTER_CLONES_MODEL_H

#include <QObject>

class CharacterClonesModel : public QObject
{
    Q_OBJECT
public:
    explicit CharacterClonesModel(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CHARACTER_CLONES_MODEL_H