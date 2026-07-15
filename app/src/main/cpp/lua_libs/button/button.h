#ifndef LUA_LIBS_BUTTON_BUTTON_H
#define LUA_LIBS_BUTTON_BUTTON_H

#include "core/hooks.h"
#include "core/java.h"
#include "core/log.h"
#include "lauxlib.h"
#include "lua.h"

#include <malloc.h>
#include <string.h>

#define LOG_TAG "KiwiCButton"

#define BUTTON_MT "MiniButton"
#define OVERLAY_MT "MiniOverlay"
#define DRAWER_MT "MiniDrawer"
#define SEEKBAR_MT "MiniSeekBar"
#define TEXTINPUT_MT "MiniTextInput"

#define JCLASS "net/itsjustsomedude/swrdg/ButtonController"

// Structs

typedef struct {
	char *id;
	char *label;
	float nx, ny, nw, nh;
	int deleted;
	int alwaysActive;
} MiniButton;

typedef struct {
	char *id;
	float nx, ny, nw, nh;
	int alwaysActive;
} MiniOverlay;

typedef struct {
	char *id;
	float nx, ny, nw, nh;
	int alwaysActive;
} MiniDrawer;

typedef struct {
	char *id;
	int alwaysActive;
} MiniSeekBar;

typedef struct {
	char *id;
	float nx, ny, nw, nh;
	int alwaysActive;
} MiniTextInput;

// JNI Ref
extern jclass g_cls;

// Userdata helpers

#define CHECK_BTN(L)      ((MiniButton*)luaL_checkudata(L, 1, BUTTON_MT))
#define CHECK_OVERLAY(L)  ((MiniOverlay*)luaL_checkudata(L, 1, OVERLAY_MT))
#define CHECK_DRAWER(L)   ((MiniDrawer*)luaL_checkudata(L, 1, DRAWER_MT))
#define CHECK_SEEKBAR(L)  ((MiniSeekBar*)luaL_checkudata(L, 1, SEEKBAR_MT))
#define CHECK_TEXTINPUT(L) ((MiniTextInput*)luaL_checkudata(L, 1, TEXTINPUT_MT))

#define GET_ENV() JNIEnv *env = miniJ_get_env()
#define GET_CLS() jclass cls = g_cls

#define JID(env, btn)           (*env)->NewStringUTF(env, (btn)->id)
#define JID_DRAWER(env, drw)    (*env)->NewStringUTF(env, (drw)->id)
#define JID_SEEKBAR(env, sb)    (*env)->NewStringUTF(env, (sb)->id)
#define JID_TEXTINPUT(env, ti) (*env)->NewStringUTF(env, ti->id)

#define VOID_METHOD(name, sig)  (*env)->GetStaticMethodID(env, cls, name, sig)
#define INT_METHOD(name, sig)   (*env)->GetStaticMethodID(env, cls, name, sig)
#define BOOL_METHOD(name, sig)  (*env)->GetStaticMethodID(env, cls, name, sig)

#define CALL_VOID_ID(mname, sig, ...) do { \
	GET_ENV(); GET_CLS(); \
	jmethodID m = VOID_METHOD(mname, sig); \
	jstring jid = JID(env, btn); \
	(*env)->CallStaticVoidMethod(env, cls, m, jid, ##__VA_ARGS__); \
	(*env)->DeleteLocalRef(env, jid); \
} while(0)

#define CALL_BOOL_ID(mname, sig, result) do { \
	GET_ENV(); GET_CLS(); \
	jmethodID m = BOOL_METHOD(mname, sig); \
	jstring jid = JID(env, btn); \
	result = (*env)->CallStaticBooleanMethod(env, cls, m, jid); \
	(*env)->DeleteLocalRef(env, jid); \
} while(0)

// LUA BRIDGEBRUH

int mini_newindex(lua_State *L);

// Button
int miniLL_open_button(lua_State *L);

// Overlay
void miniLL_open_overlay(lua_State *L);

// Drawer
void miniLL_open_drawer(lua_State *L);

// SeekBar
void miniLL_open_seekbar(lua_State *L);

// TextInput
void miniLL_open_textinput(lua_State *L);

// hooks
void miniLL_install_button_hooks(void);

// cleanup
void button_remove_all(void);

// init
void initLL_button(void);

#endif