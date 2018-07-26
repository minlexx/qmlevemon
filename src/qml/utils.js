
function getAttributePicture(attributeId) {
    switch (attributeId) {
    case 164: return "charisma";
    case 165: return "intelligence";
    case 166: return "memory";
    case 167: return "perception";
    case 168: return "willpower";
    }
    console.warn("getAttributePicture(): unknown attribute ID: " + attributeId);
    return "unknown";
}
