#ifndef CHARACTER_ASSETS_MODELS_H
#define CHARACTER_ASSETS_MODELS_H

#include <QObject>
QT_FORWARD_DECLARE_CLASS(QJsonObject)

namespace EM {


enum AssetLocationType {
    UnknownLocationType = 0, Station, SolarSystem, Other,
};


enum AssetLocationFlag {
    UnknownLocationFlag = 0,
    AssetSafety, AutoFit, BoosterBay, Cargo, CorpseBay, Deliveries,
    DroneBay, FighterBay, FighterTube0, FighterTube1, FighterTube2,
    FighterTube3, FighterTube4, FleetHangar, Hangar, HangarAll,
    HiSlot0, HiSlot1, HiSlot2, HiSlot3, HiSlot4, HiSlot5, HiSlot6,
    HiSlot7, HiddenModifiers, Implant, LoSlot0, LoSlot1, LoSlot2,
    LoSlot3, LoSlot4, LoSlot5, LoSlot6, LoSlot7, Locked, MedSlot0,
    MedSlot1, MedSlot2, MedSlot3, MedSlot4, MedSlot5, MedSlot6,
    MedSlot7, QuafeBay, RigSlot0, RigSlot1, RigSlot2, RigSlot3,
    RigSlot4, RigSlot5, RigSlot6, RigSlot7, ShipHangar, Skill,
    SpecializedAmmoHold, SpecializedCommandCenterHold, SpecializedFuelBay,
    SpecializedGasHold, SpecializedIndustrialShipHold, SpecializedLargeShipHold,
    SpecializedMaterialBay, SpecializedMediumShipHold, SpecializedMineralHold,
    SpecializedOreHold, SpecializedPlanetaryCommoditiesHold, SpecializedSalvageHold,
    SpecializedShipHold, SpecializedSmallShipHold, SubSystemBay, SubSystemSlot0,
    SubSystemSlot1, SubSystemSlot2, SubSystemSlot3, SubSystemSlot4, SubSystemSlot5,
    SubSystemSlot6, SubSystemSlot7, Unlocked, Wardrobe,
};

AssetLocationType parseLocationType(const QString &typ);
AssetLocationFlag parseLocationFlag(const QString &fl);


class AssetEntry {
public:
    AssetEntry() = default;
    AssetEntry(const AssetEntry &) = default;
    AssetEntry(AssetEntry &&) = default;
    ~AssetEntry() = default;
    AssetEntry &operator=(const AssetEntry &) = default;
    AssetEntry &operator=(AssetEntry &&) = default;
    bool operator==(const AssetEntry &o) const;
    bool operator!=(const AssetEntry &o) const;

    static AssetEntry fromJson(const QJsonObject &jobj);

    // fields that come directly from JSON
    quint64 item_id = 0;
    quint64 type_id = 0;
    bool is_singleton = false;
    quint64 quantity = 0;
    quint64 location_id = 0;
    AssetLocationType location_type = UnknownLocationType;
    AssetLocationFlag location_flag = UnknownLocationFlag;
    // calculated fields - resolving related
    QString type_name;
    QString location_name;
};


//class CharacterAssetsModels: public QObject
//{
//    Q_OBJECT
//};

} // namespace EM

#endif // CHARACTER_ASSETS_MODELS_H
