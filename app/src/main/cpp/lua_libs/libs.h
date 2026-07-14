#ifndef NEEDLE_LUA_LIBS_H
#define NEEDLE_LUA_LIBS_H

#include "lua/lua.h"

/* Global Libs */

#define MINI_LIB_NAME "Mega"

int miniLL_open_mini(lua_State *L);
int miniLL_open_lni(lua_State *L);
int miniLL_open_components(lua_State *L);
int miniLL_open_camera(lua_State *L);

#endif /* NEEDLE_LUA_LIBS_H */
