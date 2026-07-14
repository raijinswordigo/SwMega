#include "button.h"
#include "core/java.h"

#include <stdio.h>

static int NewButton(lua_State *L) {
    const char *display = luaL_checkstring(L, 1);
    float nx = (float)lua_tonumber(L, 2);
    float ny = (float)lua_tonumber(L, 3);
    float nw = (float)lua_tonumber(L, 4);
    float nh = (float)lua_tonumber(L, 5);

    KiwiButton *data = (KiwiButton *)lua_newuserdata(L, sizeof(KiwiButton));
    snprintf(data->id, sizeof(data->id), "%p", (void *)data);

    luaL_getmetatable(L, KIWIBUTTON_MT);
    lua_setmetatable(L, -2);

    GET_ENV();

    jmethodID mid = (*env)->GetStaticMethodID(env, b_cls, "NewButton", "(Ljava/lang/String;Ljava/lang/String;FFFF)V");
    jstring jtext = (*env)->NewStringUTF(env, display);
    jstring jid = (*env)->NewStringUTF(env, data->id);

    (*env)->CallStaticVoidMethod(env, b_cls, mid, jid, jtext, nx, ny, nw, nh);

    (*env)->DeleteLocalRef(env, jtext);
    (*env)->DeleteLocalRef(env, jid);

    // userdata is already on top of the stack from lua_newuserdata
    return 1;
}

/* Text Functions */

static int b_setText(lua_State *L) {
    KiwiButton *data = CHECK_BTN(L, 1);
    const char *display = luaL_checkstring(L, 2);
    GET_ENV();

    jmethodID mid = (*env)->GetStaticMethodID(env, b_cls, "b_setText", "(Ljava/lang/String;Ljava/lang/String;)V");
    jstring jtext = (*env)->NewStringUTF(env, display);
    jstring jid = (*env)->NewStringUTF(env, data->id);

    (*env)->CallStaticVoidMethod(env, b_cls, mid, jid, jtext);

    (*env)->DeleteLocalRef(env, jtext);
    (*env)->DeleteLocalRef(env, jid);

    lua_pushvalue(L, 1); // chaining self
    return 1;
}

static int b_isPressed(lua_State *L) {
    KiwiButton *data = CHECK_BTN(L, 1);

    GET_ENV();
    jmethodID mid = (*env)->GetStaticMethodID(env, b_cls, "b_isPressed", "(Ljava/lang/String;)Z");
    jstring jid = (*env)->NewStringUTF(env, data->id);
    jboolean jPressed = (*env)->CallStaticBooleanMethod(env, b_cls, mid, jid);
    (*env)->DeleteLocalRef(env, jid);

    lua_pushboolean(L, jPressed);
    return 1;
}

static int b_gc(lua_State *L) {
    KiwiButton *data = CHECK_BTN(L, 1);
    GET_ENV();
    jmethodID mid = (*env)->GetStaticMethodID(env, b_cls, "b_destroy", "(Ljava/lang/String;)V");
    jstring jid = (*env)->NewStringUTF(env, data->id);
    (*env)->CallStaticVoidMethod(env, b_cls, mid, jid);
    (*env)->DeleteLocalRef(env, jid);
    return 0;
}

// methods callable on a KiwiButton instance via btn:method()
static const luaL_Reg button_methods[] = {
    {"setText", b_setText},
    {"isPressed", b_isPressed},
    {"__gc", b_gc},
    {NULL, NULL}
};

// module-level functions, e.g. KiwiOverlay.NewButton(...)
static const luaL_Reg button_lib[] = {
    {"NewButton", NewButton},
    {NULL, NULL}
};

void button_register_metatable(lua_State *L) {
    luaL_newmetatable(L, KIWIBUTTON_MT);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_register(L, NULL, button_methods);

    lua_pop(L, 1);
}

void button_register_module(lua_State *L) {
    lua_newtable(L);
    luaL_register(L, NULL, button_lib);
}
