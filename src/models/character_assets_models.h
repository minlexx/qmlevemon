#ifndef CHARACTER_ASSETS_MODELS_H
#define CHARACTER_ASSETS_MODELS_H

#include <QObject>
#include <QSortFilterProxyModel>
#include "common_model_base.h"

QT_FORWARD_DECLARE_CLASS(QJsonObject)
QT_FORWARD_DECLARE_CLASS(QDataStream)


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
    SubSystemSlot6, SubSystemSlot7,
    Unlocked,  // probably means - unlocked inside a container
    Wardrobe,
};

AssetLocationType parseLocationType(const QString &typ);
AssetLocationFlag parseLocationFlag(const QString &fl);

bool isAssetFitted(AssetLocationFlag fl);
bool isAssetInsideContainer(AssetLocationFlag fl);


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


class CharacterAssetsModel: public CommonModelBase<AssetEntry>
{
    Q_OBJECT
    enum Roles {
        ItemId, TypeId, TypeName, IsSingleton, Quantity,
        LocationId, LocationName, LocationType, LocationFlag
    };
public:
    CharacterAssetsModel(QObject *parent = nullptr);
    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};


class CharacterAssetsFilteredModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CharacterAssetsFilteredModel(QObject *parent = nullptr);

    void setFilterLocationId(quint64 locationId);
    quint64 setFilterLocationId() const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    quint64 m_filterLocationId = 0;
};


class AssetLocationEntry {
public:
    AssetLocationEntry() = default;
    AssetLocationEntry(const AssetLocationEntry &) = default;
    AssetLocationEntry(AssetLocationEntry &&) = default;
    ~AssetLocationEntry() = default;
    AssetLocationEntry &operator=(const AssetLocationEntry &) = default;
    AssetLocationEntry &operator=(AssetLocationEntry &&) = default;
    bool operator==(const AssetLocationEntry &o) const;
    bool operator!=(const AssetLocationEntry &o) const;

    quint64 locationId = 0;
    QString locationName;
    quint64 count = 0;
};


class CharacterAssetsLocationsModel: public CommonModelBase<AssetLocationEntry>
{
    Q_OBJECT
    enum Roles { LocationId, LocationName, Count };
public:
    CharacterAssetsLocationsModel(QObject *parent = nullptr);
    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void fillLocationsFromAssets(const QVector<AssetEntry> &assets);
};


} // namespace EM


QDataStream &operator<<(QDataStream &stream, const EM::AssetEntry &o);
QDataStream &operator>>(QDataStream &stream, EM::AssetEntry &o);

QDataStream &operator<<(QDataStream &stream, const EM::AssetLocationEntry &o);
QDataStream &operator>>(QDataStream &stream, EM::AssetLocationEntry &o);


#endif // CHARACTER_ASSETS_MODELS_H
