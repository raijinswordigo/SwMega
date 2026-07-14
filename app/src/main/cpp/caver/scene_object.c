#include "scene_object.h"

#include "core/cpp_strings.h"
#include "core/hooks.h"
#include "lauxlib.h"
#include "lua/lua.h"

SceneObject *scene_object_from_L(lua_State *L, int idx) {
	SceneObject **obj = luaL_checkudata(L, idx, "SceneObject");
	if (!obj || !*obj) {
		luaL_error(L, "SceneObject is NULL.");
		return NULL;
	}
	return *obj;
}
CppString *scene_object_get_identifier(SceneObject *obj) {
	return obj->identifier;
}
float scene_object_get_speed(SceneObject *obj) {
	return obj->speed;
}
void scene_object_set_speed(SceneObject *obj, float speed) {
	obj->speed = speed;
}
