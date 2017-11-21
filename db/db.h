#ifndef H_EM_DATABASE
#define H_EM_DATABASE


#include <QtGlobal>
#include <QList>
#include <QImage>


namespace EM {


class Character;

/**
 * @brief The Db class
 * Very thin interface of storage for the app
 */
class Db
{
public:
    Db();
    virtual ~Db();

public:
    virtual bool loadCharacters(QList<Character *>& charList) = 0;
    virtual bool saveCharacters(const QList<Character *>& charList) = 0;
    virtual bool saveCharacter(const Character *character) = 0;

    virtual bool loadPortrait(quint64 char_id, QImage& img) = 0;
    virtual bool savePortrait(quint64 char_id, const QImage& img) = 0;

protected:
    Db(const Db &other);
    Db& operator=(const Db& other);
};


} // namespace


#endif