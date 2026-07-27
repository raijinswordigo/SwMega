#include "scene.h"

#include "core/hooks.h"
#include "lua_libs/button/button.h"

STATIC_DL_HOOK_SYMBOL(
	Scene_Destructor,
	"_ZN5Caver5Proto5SceneD1Ev",
	void, (void *this)
) {
	button_remove_all();
	orig_Scene_Destructor(this);
}

void initEV_scene() {
	hook_Scene_Destructor();
}