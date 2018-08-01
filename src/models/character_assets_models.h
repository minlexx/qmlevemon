#ifndef CHARACTER_ASSETS_MODELS_H
#define CHARACTER_ASSETS_MODELS_H

#include <QObject>


class AssetEntry {
public:
    AssetEntry() = default;
    AssetEntry(const AssetEntry &) = default;
    AssetEntry(AssetEntry &&) = default;
    ~AssetEntry() = default;
    AssetEntry &operator=(const AssetEntry &) = default;
    AssetEntry &operator=(AssetEntry &&) = default;

    quint64 item_id = 0;
    quint64 type_id = 0;
    QString type_name;
    bool is_singleton = false;
    quint64 quantity = 0;
    quint64 location_id = 0;
};


//class CharacterAssetsModels: public QObject
//{
//    Q_OBJECT
//};

#endif // CHARACTER_ASSETS_MODELS_H
