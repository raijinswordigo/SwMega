/* clang-format off */
#include "mini.h" /* clang-format on */

#include "core/hooks.h"
#include "core/log.h"
#include "lua.h"

#define LOG_TAG "MiniLuaDebug"

STATIC_DL_FUNCTION_SYMBOL(
	toggle_debug_info,
	"_ZN5Caver13GameSceneView15ToggleDebugInfoEv",
	void, (void* this)
)

int miniLL_toggle_debug(lua_State *L) {
	bool toggleDebug = (bool)lua_toboolean(L, 1);
	lua_getglobal(L, "scene");
	const void *scene = lua_topointer(L, -1);
	*$(bool, scene, 0x208, 0x358) = toggleDebug;
	return 0;
}

STATIC_DL_HOOK_SYMBOL(
	overlay_toggle,
	"_ZN5Caver16DebugInfoOverlay6ToggleEv",
	void, (void* this)
) {
	LOGD("The actual toggle function WAS called, this = %p", this);

	orig_overlay_toggle(this);

	LOGD("Done.");
}

STATIC_DL_HOOK_SYMBOL(
	overlay_update,
	"_ZN5Caver16DebugInfoOverlay6UpdateEf",
	void, (void* this, float delta)
) {
//	LOGD("Update called successfully: %p %f", this, delta);
	orig_overlay_update(this, delta);
}

STATIC_DL_FUNCTION_SYMBOL(
	overlay_update,
	"_ZN5Caver16DebugInfoOverlay6UpdateEf",
	void, (void* this, float delta)
)


void initLL_debug() {
	LOGD("nah im debug");
}
