#include "character_assets_models.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>


EM::AssetLocationType EM::parseLocationType(const QString &typ)
{
    EM::AssetLocationType ret = UnknownLocationType;
    if (typ == QStringLiteral("station")) ret = Station;
    if (typ == QStringLiteral("solarsystem")) ret = SolarSystem;
    if (typ == QStringLiteral("other")) ret = Other;
    return ret;
}


EM::AssetLocationFlag EM::parseLocationFlag(const QString &fl)
{
    EM::AssetLocationFlag ret = UnknownLocationFlag;
    if (fl == QStringLiteral("AssetSafety")) ret = AssetSafety;
    if (fl == QStringLiteral("AutoFit")) ret = AutoFit;
    if (fl == QStringLiteral("BoosterBay")) ret = BoosterBay;
    if (fl == QStringLiteral("Cargo")) ret = Cargo;
    if (fl == QStringLiteral("CorpseBay")) ret = CorpseBay;
    if (fl == QStringLiteral("Deliveries")) ret = Deliveries;
    if (fl == QStringLiteral("DroneBay")) ret = DroneBay;
    if (fl == QStringLiteral("FighterBay")) ret = FighterBay;
    if (fl == QStringLiteral("FighterTube0")) ret = FighterTube0;
    if (fl == QStringLiteral("FighterTube1")) ret = FighterTube1;
    if (fl == QStringLiteral("FighterTube2")) ret = FighterTube2;
    if (fl == QStringLiteral("FighterTube3")) ret = FighterTube3;
    if (fl == QStringLiteral("FighterTube4")) ret = FighterTube4;
    if (fl == QStringLiteral("FleetHangar")) ret = FleetHangar;
    if (fl == QStringLiteral("Hangar")) ret = Hangar;
    if (fl == QStringLiteral("HangarAll")) ret = HangarAll;
    if (fl == QStringLiteral("HiSlot0")) ret = HiSlot0;
    if (fl == QStringLiteral("HiSlot1")) ret = HiSlot1;
    if (fl == QStringLiteral("HiSlot2")) ret = HiSlot2;
    if (fl == QStringLiteral("HiSlot3")) ret = HiSlot3;
    if (fl == QStringLiteral("HiSlot4")) ret = HiSlot4;
    if (fl == QStringLiteral("HiSlot5")) ret = HiSlot5;
    if (fl == QStringLiteral("HiSlot6")) ret = HiSlot6;
    if (fl == QStringLiteral("HiSlot7")) ret = HiSlot7;
    if (fl == QStringLiteral("HiddenModifiers")) ret = HiddenModifiers;
    if (fl == QStringLiteral("Implant")) ret = Implant;
    if (fl == QStringLiteral("LoSlot0")) ret = LoSlot0;
    if (fl == QStringLiteral("LoSlot1")) ret = LoSlot1;
    if (fl == QStringLiteral("LoSlot2")) ret = LoSlot2;
    if (fl == QStringLiteral("LoSlot3")) ret = LoSlot3;
    if (fl == QStringLiteral("LoSlot4")) ret = LoSlot4;
    if (fl == QStringLiteral("LoSlot5")) ret = LoSlot5;
    if (fl == QStringLiteral("LoSlot6")) ret = LoSlot6;
    if (fl == QStringLiteral("LoSlot7")) ret = LoSlot7;
    if (fl == QStringLiteral("Locked")) ret = Locked;
    if (fl == QStringLiteral("MedSlot0")) ret = MedSlot0;
    if (fl == QStringLiteral("MedSlot1")) ret = MedSlot1;
    if (fl == QStringLiteral("MedSlot2")) ret = MedSlot2;
    if (fl == QStringLiteral("MedSlot3")) ret = MedSlot3;
    if (fl == QStringLiteral("MedSlot4")) ret = MedSlot4;
    if (fl == QStringLiteral("MedSlot5")) ret = MedSlot5;
    if (fl == QStringLiteral("MedSlot6")) ret = MedSlot6;
    if (fl == QStringLiteral("MedSlot7")) ret = MedSlot7;
    if (fl == QStringLiteral("QuafeBay")) ret = QuafeBay;
    if (fl == QStringLiteral("RigSlot0")) ret = RigSlot0;
    if (fl == QStringLiteral("RigSlot1")) ret = RigSlot1;
    if (fl == QStringLiteral("RigSlot2")) ret = RigSlot2;
    if (fl == QStringLiteral("RigSlot3")) ret = RigSlot3;
    if (fl == QStringLiteral("RigSlot4")) ret = RigSlot4;
    if (fl == QStringLiteral("RigSlot5")) ret = RigSlot5;
    if (fl == QStringLiteral("RigSlot6")) ret = RigSlot6;
    if (fl == QStringLiteral("RigSlot7")) ret = RigSlot7;
    if (fl == QStringLiteral("ShipHangar")) ret = ShipHangar;
    if (fl == QStringLiteral("Skill")) ret = Skill;
    if (fl == QStringLiteral("SpecializedAmmoHold")) ret = SpecializedAmmoHold;
    if (fl == QStringLiteral("SpecializedCommandCenterHold")) ret = SpecializedCommandCenterHold;
    if (fl == QStringLiteral("SpecializedFuelBay")) ret = SpecializedFuelBay;
    if (fl == QStringLiteral("SpecializedGasHold")) ret = SpecializedGasHold;
    if (fl == QStringLiteral("SpecializedIndustrialShipHold")) ret = SpecializedIndustrialShipHold;
    if (fl == QStringLiteral("SpecializedLargeShipHold")) ret = SpecializedLargeShipHold;
    if (fl == QStringLiteral("SpecializedMaterialBay")) ret = SpecializedMaterialBay;
    if (fl == QStringLiteral("SpecializedMediumShipHold")) ret = SpecializedMediumShipHold;
    if (fl == QStringLiteral("SpecializedMineralHold")) ret = SpecializedMineralHold;
    if (fl == QStringLiteral("SpecializedOreHold")) ret = SpecializedOreHold;
    if (fl == QStringLiteral("SpecializedPlanetaryCommoditiesHold")) ret = SpecializedPlanetaryCommoditiesHold;
    if (fl == QStringLiteral("SpecializedSalvageHold")) ret = SpecializedSalvageHold;
    if (fl == QStringLiteral("SpecializedShipHold")) ret = SpecializedShipHold;
    if (fl == QStringLiteral("SpecializedSmallShipHold")) ret = SpecializedSmallShipHold;
    if (fl == QStringLiteral("SubSystemBay")) ret = SubSystemBay;
    if (fl == QStringLiteral("SubSystemSlot0")) ret = SubSystemSlot0;
    if (fl == QStringLiteral("SubSystemSlot1")) ret = SubSystemSlot1;
    if (fl == QStringLiteral("SubSystemSlot2")) ret = SubSystemSlot2;
    if (fl == QStringLiteral("SubSystemSlot3")) ret = SubSystemSlot3;
    if (fl == QStringLiteral("SubSystemSlot4")) ret = SubSystemSlot4;
    if (fl == QStringLiteral("SubSystemSlot5")) ret = SubSystemSlot5;
    if (fl == QStringLiteral("SubSystemSlot6")) ret = SubSystemSlot6;
    if (fl == QStringLiteral("SubSystemSlot7")) ret = SubSystemSlot7;
    if (fl == QStringLiteral("Unlocked")) ret = Unlocked;
    if (fl == QStringLiteral("Wardrobe")) ret = Wardrobe;
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
