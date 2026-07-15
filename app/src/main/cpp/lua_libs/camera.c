#include "libs.h"
#include "lua.h"
#include "lauxlib.h"
#include "caver/camera.h"
#include "core/log.h"
#include "init/lua_libs.h"

#define LOG_TAG "KiwiCamera"

static bool g_disable_optimizations = false;

static CameraController* get_cc(lua_State *L) {
	lua_getglobal(L, "cameraController");
	const void *cc = lua_topointer(L, -1);
	LOGD("Camera: %p", cc);
	lua_pop(L, 1);
	return (CameraController*)cc;
}

static int SetOffset(lua_State *L) {
	float x = (float)lua_tonumber(L, 1);
	float y = (float)lua_tonumber(L, 2);
	float z = (float)lua_tonumber(L, 3);
	CameraController *cc = get_cc(L);
	Vector3 newOffset = {x, y, z};
	Vector3 offset = cc->offset;
	LOGD(
		"Offset: {%f, %f, %f} -> {%f, %f, %f}",
		offset.x, offset.y, offset.z,
		newOffset.x, newOffset.y, newOffset.z 
	);
	cc->offset = newOffset;
	return 0;
}

static int GetOffset(lua_State *L) {
	CameraController *cc = get_cc(L);
	Vector3 offset = cc->offset;
	lua_pushnumber(L, offset.x);
	lua_pushnumber(L, offset.y);
	lua_pushnumber(L, offset.z);
	return 3;
}

static int SetUpVector(lua_State *L) {
	float x = (float)lua_tonumber(L, 1);
	float y = (float)lua_tonumber(L, 2);
	float z = (float)lua_tonumber(L, 3);
	CameraController *cc = get_cc(L);
	Vector3 newUpVector = {x, y, z};
	Vector3 upVector = cc->upVector;
	LOGD(
		"UpVector: {%f, %f, %f} -> {%f, %f, %f}",
		upVector.x, upVector.y, upVector.z,
		newUpVector.x, newUpVector.y, newUpVector.z
	);
	cc->upVector = newUpVector;
	return 0;
}

static int GetUpVector(lua_State *L) {
	CameraController *cc = get_cc(L);
	Vector3 upVector = cc->upVector;
	lua_pushnumber(L, upVector.x);
	lua_pushnumber(L, upVector.y);
	lua_pushnumber(L, upVector.z);
	return 3;
}

static int DisableOptimizations(lua_State *L) {
	bool optimizationsDisabled = lua_toboolean(L, 1);
	g_disable_optimizations = optimizationsDisabled ? true : false;
	return 0;
}

static const luaL_Reg cc_lib[] = {
	{"SetOffset", SetOffset},
	{"GetOffset", GetOffset},
	{"SetUpVector", SetUpVector},
	{"GetUpVector", GetUpVector},

	{"DisableOptimizations", DisableOptimizations},
	{NULL, NULL}
};

LUALIB_API int miniLL_open_camera(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, cc_lib);
	return 1;
}

STATIC_DL_HOOK_SYMBOL(
	SceneGrid_UpdateVisibleAreasWithCamera,
	"_ZN5Caver9SceneGrid28UpdateVisibleAreasWithCameraEPNS_6CameraE",
	void, (void *self, void *camera)
) {
	orig_SceneGrid_UpdateVisibleAreasWithCamera(self, camera);
	if (g_disable_optimizations) {
		int layer_count = *(int*)self;
		if (layer_count > 0) {
			char** array = *(char***)((char*)self + 8);
			if (array) {
				for (int i = 0; i < layer_count; i++) {
					char* layer = array[i * 2];
					if (layer) {
						*$(float, layer, 0x34, 0x38) = -1000000.0f; // x
						*$(float, layer, 0x38, 0x3c) = -1000000.0f; // y
						*$(float, layer, 0x3c, 0x40) = 2000000.0f; // width
						*$(float, layer, 0x40, 0x44) = 2000000.0f; // height
					}
				}
			}
		}
		return;
	}
}

void initLL_camera() {
	LOGD("yogurt");
	hook_SceneGrid_UpdateVisibleAreasWithCamera();
}
