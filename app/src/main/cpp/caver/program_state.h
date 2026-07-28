#ifndef NEEDLE_PROGRAM_STATE_H
#define NEEDLE_PROGRAM_STATE_H

#include "core/hooks.h"
#include "lua.h"
#include "scene_object.h"

H_DL_FUNCTION(
	program_state_from_L,
	void*, (lua_State * L)
)

typedef struct ProgramState {
    lua_State *L;
    char _pad0[archSplit(0x4, 0x8)];
    char _children_list[archSplit(0x8, 0x10)];
    SceneObject *parent;
    char _pad1[archSplit(0x10, 0x20)];
    int isWaiting;
    float waitTime;
    char active;
    char flag1;
    char flag2;
    char pendingRemoval;
    float speedMultiplier;
} ProgramState;

extern void *game_controller_from_L(lua_State *L);
extern void *scene_controller_from_L(lua_State *L);
extern SceneObject *hero_object_from_L(lua_State *L);

#endif
