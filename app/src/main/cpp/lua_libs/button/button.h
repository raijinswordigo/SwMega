#ifndef MEGA_BUTTON_H
#define MEGA_BUTTON_H

#include "lauxlib.h"
#include "lua.h"
#include <jni.h>

#define KIWIBUTTON_MT "KiwiButton"

typedef struct {
    char id[32];
} KiwiButton;

extern jclass b_cls;

JNIEnv *get_env();

/* Shorthand for grabbing this thread's JNIEnv* inside a Lua-bound function */
#define GET_ENV() JNIEnv *env = get_env()

/* Shorthand for validating arg `idx` is a KiwiButton userdata and grabbing its data.
 * Raises a Lua error automatically (via luaL_checkudata) if the type doesn't match. */
#define CHECK_BTN(L, idx) ((KiwiButton *)luaL_checkudata((L), (idx), KIWIBUTTON_MT))

void button_register_metatable(lua_State *L);
void button_register_module(lua_State *L);

int initLL_open_button(lua_State *L);
void initLL_button();

#endif //MEGA_BUTTON_H
