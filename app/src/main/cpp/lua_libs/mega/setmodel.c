#include "mini.h"
#include "caver/components.h"

STATIC_DL_FUNCTION_SYMBOL(
	setModelName,
	"_ZN5Caver14ModelComponent12setModelNameERKSs",
	void, (void *ModelComponent, CppString **name)
)

int miniLL_set_model(lua_State *L) {
	void **o_ud = luaL_checkudata(L, 1, "SceneObject");
	const char *modelname = luaL_checkstring(L, 2);
	SceneObject *obj = *o_ud;
	void *mc = SceneObject_ComponentWithInterface(obj, ModelComponent_Interface);
	if (!mc) return 0;
	CppString *name;
	CppString_create(&name, modelname);
	setModelName(mc, &name);
	CppString_release(name);
	return 0;
}

void initLL_set_model() {
	dlsym_setModelName();
}