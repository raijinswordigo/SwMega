#include "caver/components.h"
#include "caver/components/CharControllerComponent.h"
#include "caver/program_state.h"
#include "caver/scene.h"
#include "caver/types.h"
#include "core/hooks.h"
#include "core/log.h"
#include "mini.h"

#include <dlfcn.h>

#define LOG_TAG "MegaDepression"

typedef struct {
    float x;
    float y;
    float w;
    float h;
} Rectangle_t;

STATIC_DL_FUNCTION_SYMBOL(
	GUIView_SetFrame,
	"_ZN5Caver7GUIView8SetFrameERKNS_9RectangleE",
	void, (void *view, Rectangle_t *rect)
)

STATIC_DL_HOOK_SYMBOL(
    LayoutSubviews,
    "_ZN5Caver13InventoryView14LayoutSubviewsEv",
    void, (void *this)
) {
    orig_LayoutSubviews(this);

    if (!GUIView_SetFrame) return;
    void *extraView = *(void **)((char *)this + 0x138);
}

Scene *get(lua_State *L) {
    lua_getglobal(L, "scene");
    const void *scene = lua_topointer(L, -1);
    lua_pop(L, 1);
    return (Scene*)scene;
}

int miniLL_test(lua_State *L) {
    SceneObject **hero = luaL_checkudata(L, 1, "SceneObject");
	CharControllerComponent *c = SceneObject_ComponentWithInterface(*hero, CharControllerComponent_Interface);

    return 0;
}

void initLL_test() {
//    hook_LayoutSubviews();
//	dlsym_GUIView_SetFrame();
    LOGD("NAH THAT SHIT LOADED");
}