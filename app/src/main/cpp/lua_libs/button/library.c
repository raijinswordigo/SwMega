#include "button.h"
#include "core/log.h"
#include "core/java.h"

static JavaVM *g_vm;
jclass b_cls;

#define LOG_TAG "KiwiOverlay"
#define CLASS_NAME "net/itsjustsomedude/swrdg/ButtonController"

JNIEnv *get_env() {
    JNIEnv *env;
    int status = (*g_vm)->GetEnv(g_vm, (void **)&env, JNI_VERSION_1_6);
    if (status == JNI_EDETACHED) {
        (*g_vm)->AttachCurrentThread(g_vm, &env, NULL);
    }
    return env;
}

LUALIB_API int initLL_open_button(lua_State *L) {
    button_register_metatable(L);
    button_register_module(L); // leaves the module table on top of the stack
    return 1;
}

void initLL_button() {
    g_vm = miniJ_jvm;

    JNIEnv *env = get_env();
    jclass tmpCls = (*env)->FindClass(env, CLASS_NAME);
    b_cls = (*env)->NewGlobalRef(env, tmpCls);
    (*env)->DeleteLocalRef(env, tmpCls);
}
