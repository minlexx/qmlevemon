#include "character_assets_models.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>
#include <QDataStream>
#include <algorithm>


EM::AssetLocationType EM::parseLocationType(const QString &typ)
{
    EM::AssetLocationType ret = UnknownLocationType;
    if (typ == QStringLiteral("station")) ret = Station;
    else if (typ == QStringLiteral("solarsystem")) ret = SolarSystem;
    else if (typ == QStringLiteral("other")) ret = Other;
    return ret;
}


EM::AssetLocationFlag EM::parseLocationFlag(const QString &fl)
{
    EM::AssetLocationFlag ret = UnknownLocationFlag;
    if (fl == QStringLiteral("AssetSafety")) ret = AssetSafety;
    else if (fl == QStringLiteral("AutoFit")) ret = AutoFit;
    else if (fl == QStringLiteral("BoosterBay")) ret = BoosterBay;
    else if (fl == QStringLiteral("Cargo")) ret = Cargo;
    else if (fl == QStringLiteral("CorpseBay")) ret = CorpseBay;
    else if (fl == QStringLiteral("Deliveries")) ret = Deliveries;
    else if (fl == QStringLiteral("DroneBay")) ret = DroneBay;
    else if (fl == QStringLiteral("FighterBay")) ret = FighterBay;
    else if (fl == QStringLiteral("FighterTube0")) ret = FighterTube0;
    else if (fl == QStringLiteral("FighterTube1")) ret = FighterTube1;
    else if (fl == QStringLiteral("FighterTube2")) ret = FighterTube2;
    else if (fl == QStringLiteral("FighterTube3")) ret = FighterTube3;
    else if (fl == QStringLiteral("FighterTube4")) ret = FighterTube4;
    else if (fl == QStringLiteral("FleetHangar")) ret = FleetHangar;
    else if (fl == QStringLiteral("Hangar")) ret = Hangar;
    else if (fl == QStringLiteral("HangarAll")) ret = HangarAll;
    else if (fl == QStringLiteral("HiSlot0")) ret = HiSlot0;
    else if (fl == QStringLiteral("HiSlot1")) ret = HiSlot1;
    else if (fl == QStringLiteral("HiSlot2")) ret = HiSlot2;
    else if (fl == QStringLiteral("HiSlot3")) ret = HiSlot3;
    else if (fl == QStringLiteral("HiSlot4")) ret = HiSlot4;
    else if (fl == QStringLiteral("HiSlot5")) ret = HiSlot5;
    else if (fl == QStringLiteral("HiSlot6")) ret = HiSlot6;
    else if (fl == QStringLiteral("HiSlot7")) ret = HiSlot7;
    else if (fl == QStringLiteral("HiddenModifiers")) ret = HiddenModifiers;
    else if (fl == QStringLiteral("Implant")) ret = Implant;
    else if (fl == QStringLiteral("LoSlot0")) ret = LoSlot0;
    else if (fl == QStringLiteral("LoSlot1")) ret = LoSlot1;
    else if (fl == QStringLiteral("LoSlot2")) ret = LoSlot2;
    else if (fl == QStringLiteral("LoSlot3")) ret = LoSlot3;
    else if (fl == QStringLiteral("LoSlot4")) ret = LoSlot4;
    else if (fl == QStringLiteral("LoSlot5")) ret = LoSlot5;
    else if (fl == QStringLiteral("LoSlot6")) ret = LoSlot6;
    else if (fl == QStringLiteral("LoSlot7")) ret = LoSlot7;
    else if (fl == QStringLiteral("Locked")) ret = Locked;
    else if (fl == QStringLiteral("MedSlot0")) ret = MedSlot0;
    else if (fl == QStringLiteral("MedSlot1")) ret = MedSlot1;
    else if (fl == QStringLiteral("MedSlot2")) ret = MedSlot2;
    else if (fl == QStringLiteral("MedSlot3")) ret = MedSlot3;
    else if (fl == QStringLiteral("MedSlot4")) ret = MedSlot4;
    else if (fl == QStringLiteral("MedSlot5")) ret = MedSlot5;
    else if (fl == QStringLiteral("MedSlot6")) ret = MedSlot6;
    else if (fl == QStringLiteral("MedSlot7")) ret = MedSlot7;
    else if (fl == QStringLiteral("QuafeBay")) ret = QuafeBay;
    else if (fl == QStringLiteral("RigSlot0")) ret = RigSlot0;
    else if (fl == QStringLiteral("RigSlot1")) ret = RigSlot1;
    else if (fl == QStringLiteral("RigSlot2")) ret = RigSlot2;
    else if (fl == QStringLiteral("RigSlot3")) ret = RigSlot3;
    else if (fl == QStringLiteral("RigSlot4")) ret = RigSlot4;
    else if (fl == QStringLiteral("RigSlot5")) ret = RigSlot5;
    else if (fl == QStringLiteral("RigSlot6")) ret = RigSlot6;
    else if (fl == QStringLiteral("RigSlot7")) ret = RigSlot7;
    else if (fl == QStringLiteral("ShipHangar")) ret = ShipHangar;
    else if (fl == QStringLiteral("Skill")) ret = Skill;
    else if (fl == QStringLiteral("SpecializedAmmoHold")) ret = SpecializedAmmoHold;
    else if (fl == QStringLiteral("SpecializedCommandCenterHold")) ret = SpecializedCommandCenterHold;
    else if (fl == QStringLiteral("SpecializedFuelBay")) ret = SpecializedFuelBay;
    else if (fl == QStringLiteral("SpecializedGasHold")) ret = SpecializedGasHold;
    else if (fl == QStringLiteral("SpecializedIndustrialShipHold")) ret = SpecializedIndustrialShipHold;
    else if (fl == QStringLiteral("SpecializedLargeShipHold")) ret = SpecializedLargeShipHold;
    else if (fl == QStringLiteral("SpecializedMaterialBay")) ret = SpecializedMaterialBay;
    else if (fl == QStringLiteral("SpecializedMediumShipHold")) ret = SpecializedMediumShipHold;
    else if (fl == QStringLiteral("SpecializedMineralHold")) ret = SpecializedMineralHold;
    else if (fl == QStringLiteral("SpecializedOreHold")) ret = SpecializedOreHold;
    else if (fl == QStringLiteral("SpecializedPlanetaryCommoditiesHold")) ret = SpecializedPlanetaryCommoditiesHold;
    else if (fl == QStringLiteral("SpecializedSalvageHold")) ret = SpecializedSalvageHold;
    else if (fl == QStringLiteral("SpecializedShipHold")) ret = SpecializedShipHold;
    else if (fl == QStringLiteral("SpecializedSmallShipHold")) ret = SpecializedSmallShipHold;
    else if (fl == QStringLiteral("SubSystemBay")) ret = SubSystemBay;
    else if (fl == QStringLiteral("SubSystemSlot0")) ret = SubSystemSlot0;
    else if (fl == QStringLiteral("SubSystemSlot1")) ret = SubSystemSlot1;
    else if (fl == QStringLiteral("SubSystemSlot2")) ret = SubSystemSlot2;
    else if (fl == QStringLiteral("SubSystemSlot3")) ret = SubSystemSlot3;
    else if (fl == QStringLiteral("SubSystemSlot4")) ret = SubSystemSlot4;
    else if (fl == QStringLiteral("SubSystemSlot5")) ret = SubSystemSlot5;
    else if (fl == QStringLiteral("SubSystemSlot6")) ret = SubSystemSlot6;
    else if (fl == QStringLiteral("SubSystemSlot7")) ret = SubSystemSlot7;
    else if (fl == QStringLiteral("Unlocked")) ret = Unlocked;
    else if (fl == QStringLiteral("Wardrobe")) ret = Wardrobe;
    return ret;
}


bool EM::AssetEntry::operator==(const EM::AssetEntry &o) const
{
    return item_id == o.item_id;
}

bool EM::AssetEntry::operator!=(const EM::AssetEntry &o) const
{
    return !((*this) == o);
}

EM::AssetEntry EM::AssetEntry::fromJson(const QJsonObject &jobj)
{
    // {
    //    "is_singleton": true,
    //    "item_id": 915144094,
    //    "location_flag": "Unlocked",
    //    "location_id": 1010717963332,
    //    "location_type": "other",
    //    "quantity": 1,
    //    "type_id": 12824
    //  },
    EM::AssetEntry ret;
    ret.is_singleton   = jobj.value(QLatin1String("is_singleton")).toBool();
    ret.item_id        = jobj.value(QLatin1String("item_id")).toVariant().toULongLong();
    ret.location_flag  = parseLocationFlag(jobj.value(QLatin1String("location_flag")).toString());
    ret.location_id    = jobj.value(QLatin1String("location_id")).toVariant().toULongLong();
    ret.location_type  = parseLocationType(jobj.value(QLatin1String("location_type")).toString());
    ret.quantity       = jobj.value(QLatin1String("quantity")).toVariant().toULongLong();
    ret.type_id        = jobj.value(QLatin1String("type_id")).toVariant().toULongLong();
    return ret;
}

EM::CharacterAssetsModel::CharacterAssetsModel(QObject *parent)
    : CommonModelBase<AssetEntry>(parent)
{
}

QHash<int, QByteArray> EM::CharacterAssetsModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {ItemId,       QByteArrayLiteral("itemId")},
        {TypeId,       QByteArrayLiteral("typeId")},
        {TypeName,     QByteArrayLiteral("typeName")},
        {IsSingleton,  QByteArrayLiteral("isSingleton")},
        {Quantity,     QByteArrayLiteral("quantity")},
        {LocationId,   QByteArrayLiteral("locationId")},
        {LocationName, QByteArrayLiteral("locationName")},
        {LocationType, QByteArrayLiteral("locationType")},
        {LocationFlag, QByteArrayLiteral("locationFlag")},
    };
    return roles;
}

QVariant EM::CharacterAssetsModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    const AssetEntry *entry = validateIndexAndGetData(index);
    if (!entry) return ret;
    switch (role) {
    case Roles::ItemId: ret = entry->item_id; break;
    case Roles::TypeId: ret = entry->type_id; break;
    case Roles::TypeName: ret = entry->type_name; break;
    case Roles::IsSingleton: ret = entry->is_singleton; break;
    case Roles::Quantity: ret = entry->quantity; break;
    case Roles::LocationId: ret = entry->location_id; break;
    case Roles::LocationName: ret = entry->location_name; break;
    case Roles::LocationType: ret = entry->location_type; break;
    case Roles::LocationFlag: ret = entry->location_flag; break;
    }
    return ret;
}



EM::CharacterAssetsFilteredModel::CharacterAssetsFilteredModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void EM::CharacterAssetsFilteredModel::setFilterLocationId(quint64 locationId)
{
    m_filterLocationId = locationId;
    invalidateFilter();
}

quint64 EM::CharacterAssetsFilteredModel::setFilterLocationId() const
{
    return m_filterLocationId;
}

bool EM::CharacterAssetsFilteredModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filterLocationId == 0) {
        // no filter - return all
        return true;
    }
    const CharacterAssetsModel *parentModel = qobject_cast<const CharacterAssetsModel *>(sourceModel());
    if (Q_UNLIKELY(!parentModel)) {
        // somehow our source model is not CharacterAssetsModel instance
        return true; // blindly accept, should never happen
    }
    const QModelIndex sourceIndex = parentModel->index(sourceRow, 0, sourceParent);
    const QVector<AssetEntry> &allAssets = parentModel->internalData();
    const AssetEntry &sourceAsset = allAssets.at(sourceIndex.row());
    // accept only specific location id
    if (sourceAsset.location_id == m_filterLocationId) {
        return true;
    }
    return false;
}



bool EM::AssetLocationEntry::operator==(const EM::AssetLocationEntry &o) const
{
    return locationId == o.locationId;
}

bool EM::AssetLocationEntry::operator!=(const EM::AssetLocationEntry &o) const
{
    return !((*this) == o);
}

EM::CharacterAssetsLocationsModel::CharacterAssetsLocationsModel(QObject *parent)
    : CommonModelBase<AssetLocationEntry>(parent)
{
}

QHash<int, QByteArray> EM::CharacterAssetsLocationsModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {LocationId,   QByteArrayLiteral("locationId")},
        {LocationName, QByteArrayLiteral("locationName")},
        {Count,        QByteArrayLiteral("count")},
    };
    return roles;
}

QVariant EM::CharacterAssetsLocationsModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    const AssetLocationEntry *entry = validateIndexAndGetData(index);
    if (!entry) return ret;
    switch (role) {
    case Roles::LocationId:   ret = entry->locationId; break;
    case Roles::LocationName: ret = entry->locationName; break;
    case Roles::Count:        ret = entry->count; break;
    }
    return ret;
}

void EM::CharacterAssetsLocationsModel::fillLocationsFromAssets(const QVector<EM::AssetEntry> &assets)
{
    beginResetModel();
    m_data.clear();
    for (const EM::AssetEntry &asset: assets) {
        bool is_in_station = false;
        bool is_in_citadel = false;
        is_in_station = (asset.location_type == AssetLocationType::Station);
        is_in_citadel = (asset.location_type == AssetLocationType::Other
                         && asset.location_flag == AssetLocationFlag::Hangar);
        if (is_in_station || is_in_citadel) {
            EM::AssetLocationEntry aloc;
            aloc.locationId = asset.location_id;
            int idx = m_data.indexOf(aloc);
            if (idx == -1) {
                // new item
                aloc.locationName = asset.location_name;
                aloc.count = 1;
                m_data.push_back(std::move(aloc));
            } else {
                // existing item, increase count
                m_data[idx].count++;
            }
        }
    }
    std::sort(m_data.begin(), m_data.end(), [](const AssetLocationEntry &a, const AssetLocationEntry &b) -> bool {
        return a.locationName < b.locationName;
    });
    endResetModel();
}


QDataStream &operator<<(QDataStream &stream, const EM::AssetEntry &o)
{
    stream << o.item_id;
    stream << o.type_id;
    stream << o.is_singleton;
    stream << o.quantity;
    stream << o.location_id;
    stream << static_cast<int>(o.location_type);
    stream << static_cast<int>(o.location_flag);
    // calculated fields - resolving related
    stream << o.type_name;
    stream << o.location_name;
    return stream;
}


QDataStream &operator>>(QDataStream &stream, EM::AssetEntry &o)
{
    int i;
    //
    stream >> o.item_id;
    stream >> o.type_id;
    stream >> o.is_singleton;
    stream >> o.quantity;
    stream >> o.location_id;
    stream >> i;    o.location_type = static_cast<EM::AssetLocationType>(i);
    stream >> i;    o.location_flag = static_cast<EM::AssetLocationFlag>(i);
    // calculated fields - resolving related
    stream >> o.type_name;
    stream >> o.location_name;
    return stream;
}


QDataStream &operator<<(QDataStream &stream, const EM::AssetLocationEntry &o)
{
    stream << o.locationId;
    stream << o.locationName;
    stream << o.count;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::AssetLocationEntry &o)
{
    stream >> o.locationId;
    stream >> o.locationName;
    stream >> o.count;
    return stream;
}


bool EM::isAssetFitted(EM::AssetLocationFlag fl)
{
    if ((fl >= HiSlot0) && (fl <= HiSlot7)) return true;
    if ((fl >= LoSlot0) && (fl <= LoSlot7)) return true;
    if ((fl >= MedSlot0) && (fl <= MedSlot7)) return true;
    if ((fl >= RigSlot0) && (fl <= RigSlot7)) return true;
    if ((fl >= SubSystemSlot0) && (fl <= SubSystemSlot7)) return true;
    if ((fl >= FighterTube0) && (fl <= FighterTube4)) return true;
    if ((fl == BoosterBay) || (fl == Cargo) || (fl == CorpseBay)
            || (fl == DroneBay) || (fl == FighterBay) || (fl == FleetHangar)
            || (fl == QuafeBay) || (fl == SubSystemBay)) return true;
    if ((fl >= SpecializedAmmoHold) && (fl <= SpecializedSmallShipHold)) return true;
    return false;
}

bool EM::isAssetInsideContainer(EM::AssetLocationFlag fl)
{
    if ((fl == Locked) || (fl == Unlocked) || (fl == AutoFit)) return true;
    // if ((fl == AssetSafety) || (fl == Deliveries)) return true;
    // if ((fl == Hangar) || (fl == HangarAll) || (fl == ShipHangar)) return true;
    return false;
}
