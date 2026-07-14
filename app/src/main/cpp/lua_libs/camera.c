#include "libs.h"
#include "lua.h"
#include "lauxlib.h"
#include "caver/camera.h"
#include "core/log.h"

#define LOG_TAG "KiwiCamera"

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

static const luaL_Reg cc_lib[] = {
	{"SetOffset", SetOffset},
	{"GetOffset", GetOffset},
	{"SetUpVector", SetUpVector},
	{"GetUpVector", GetUpVector},
	{NULL, NULL}
};

LUALIB_API int miniLL_open_camera(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, cc_lib);
	return 1;
}

void initLL_camera() {
	LOGD("yogurt");
}
