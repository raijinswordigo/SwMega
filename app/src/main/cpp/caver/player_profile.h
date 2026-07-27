#ifndef NEEDLE_PLAYER_PROFILE_H
#define NEEDLE_PLAYER_PROFILE_H

#include "core/cpp_strings.h"
#include "core/hooks.h"
#include "types.h"

typedef struct GameState GameState;
typedef struct DateTime {
    int32_t seconds;
    int32_t nanos;
} DateTime;

// fuck 32 bit
// this is all incorrectly mapped i think but it works the cppstrings

typedef struct PlayerProfile {
    char _pad0[0x18];

    CppString *identifier;
    CppString *name;

    DateTime lastPlayedTime;

    CppString *currentLevelTitle;

    int32_t levelCache;
    int32_t percentCompletedCache;

    uint64_t economyPair;

    CppString *equippedWeaponName;
    CppString *equippedArmorName;
    CppString *weaponTrinketName;
    CppString *armorTrinketName;

    char loaded;
    char _pad1[0x17];

    GameState *gameState;

    char _pad2[0x8];

    char countersMap[0x30];

    char unk_flag;

} PlayerProfile;

#endif // NEEDLE_PLAYER_PROFILE_H