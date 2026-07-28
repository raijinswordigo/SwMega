#include "caver/scene.h"
#include "caver/types.h"
#include "core/hooks.h"
#include "core/log.h"
#include "mini.h"
#include "caver/program_state.h"
#include <dlfcn.h>

#define LOG_TAG "MegaDepression"

typedef struct {
    float x;
    float y;
    float w;
    float h;
} Rectangle_t;

//static void (*GUIView_SetFrame)(void *view, Rectangle_t *rect) = NULL;

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
    // Run the original layout first so all the slot frames get set up normally.
    orig_LayoutSubviews(this);

    if (!GUIView_SetFrame) return;

    // this + 0x138 -> the extra GUIView (close button-ish thing in the decompile)
    void *extraView = *(void **)((char *)this + 0x138);
    if (extraView) {
        Rectangle_t r;
        r.x = -40.0f;                                   // was -60.0
        r.y = 40.0f;                                     // unchanged
        r.w = *(float *)((char *)this + 0x8c) + 60.0f;   // was + 60.0
        r.h = *(float *)((char *)extraView + 0x80) + 50.0f;      // keep whatever height it already had
        GUIView_SetFrame(extraView, &r);
    }
}

Scene *get(lua_State *L) {
    lua_getglobal(L, "scene");
    const void *scene = lua_topointer(L, -1);
    lua_pop(L, 1);
    return (Scene*)scene;
}

int miniLL_test(lua_State *L) {
    Scene *s = get(L);
    void *ps = program_state_from_L(L);

    SceneObject **objPtr = $(SceneObject*, ps, 0x10, 0x20); // address of the pointer slot
    SceneObject *obj = *objPtr;
    if (obj) { LOGD("%s", obj->identifier); } else LOGD("Bad luck");
    return 0;
}

void initLL_test() {
    hook_LayoutSubviews();
	dlsym_GUIView_SetFrame();
    LOGD("NAH THAT SHIT LOADED");
}