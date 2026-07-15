#ifndef SWMINI_MINI_LUA_LIBRARY_H
#define SWMINI_MINI_LUA_LIBRARY_H

#include "core/hooks.h"
#include "lauxlib.h"
#include "lua.h"

int miniLL_set_controls_hidden(lua_State *L);
int miniLL_get_profile_id(lua_State *L);
int miniLL_toggle_debug(lua_State *L);
int miniLL_recreate_hero(lua_State *L);
int miniLL_get_arch(lua_State *L);
int miniLL_the_map_function(lua_State *L);
int miniLL_scene_find_all(lua_State *L);
int miniLL_set_model(lua_State *L);

void initLL_mini();

void initLL_set_controls_hidden();
void initLL_debug();
void initLL_recreate_hero();
void initLL_scene_find_all();
void initLL_map_function();
void initLL_set_model();

#endif //SWMINI_MINI_LUA_LIBRARY_H
