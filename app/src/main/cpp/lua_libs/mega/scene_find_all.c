#include "mini.h" /* clang-format on */

#include "core/hooks.h"
#include "lauxlib.h"
#include "core/log.h"
#include "lua.h"
#include "caver/scene_object.h"
#include "core/cpp_strings.h"

#define LOG_TAG "MiniLuaFindAll"

STATIC_DL_FUNCTION_SYMBOL(
	pushSceneObject,
	"_ZN5Caver14SceneObjectLib15PushSceneObjectEPNS_12ProgramStateEPNS_11SceneObjectE",
	void, (void *programState, void *sceneObject)
)

STATIC_DL_FUNCTION_SYMBOL(
	progStateFromL,
	"_ZN5Caver12ProgramState12FromLuaStateEP9lua_State",
	void*, (lua_State * L)
)

// From Ghidra...
static void* rbTreeIncrement(void* param_1) {
	void* lVar1;
	void* lVar2 = *(void**)((char*)param_1 + archSplit(0xc, 0x18));

	if (lVar2 != NULL) {
		do {
			lVar1 = lVar2;
			lVar2 = *(void**)((char*)lVar1 + archSplit(0x8, 0x10));
		} while (lVar2 != NULL);
		return lVar1;
	}

	lVar2 = *(void**)((char*)param_1 + archSplit(0x4, 0x8));
	if (param_1 == *(void**)((char*)lVar2 + archSplit(0xc, 0x18))) {
		do {
			lVar1 = lVar2;
			lVar2 = *(void**)((char*)lVar1 + archSplit(0x4, 0x8));
		} while (*(void**)((char*)lVar2 + archSplit(0xc, 0x18)) == lVar1);

		if (lVar2 == *(void**)((char*)lVar1 + archSplit(0xc, 0x18))) lVar2 = lVar1;
		return lVar2;
	}

	return lVar2;
}

int miniLL_scene_find_all(lua_State* L) {
	lua_getglobal(L, "scene");
	void* scene = lua_touserdata(L, -1);
	lua_pop(L, 1);

	if (!scene) return 0;

	lua_newtable(L);
	void* sentinel = (char*)scene + archSplit(0x70, 0xb8);
	void* node = *(void**)((char*)sentinel + archSplit(0x8, 0x10));

	int i = 1;
	void* ps = progStateFromL(L);

	while (node != sentinel)
	{
		SceneObject* obj = *(SceneObject**)((char*)node + archSplit(0x14, 0x28));

		if (obj) {
			CppString* obj_id = *(CppString**)((char*)obj + archSplit(0x2c, 0x50));
			if (obj_id) LOGD("ID: '%s'", obj_id);
			lua_pushinteger(L, i++);
			pushSceneObject(ps, obj);
			lua_settable(L, -3);
		}

		node = rbTreeIncrement(node);
	}

	return 1;
}

void initLL_scene_find_all() {
	dlsym_pushSceneObject();
	dlsym_progStateFromL();
}