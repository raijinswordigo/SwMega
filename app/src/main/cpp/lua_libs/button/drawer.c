#include "button.h"

int NewDrawer(lua_State *L) {
	float nx = (float)luaL_optnumber(L, 1, 0.5);
	float ny = (float)luaL_optnumber(L, 2, 0.5);
	float nw = (float)luaL_optnumber(L, 3, 0.3);
	float nh = (float)luaL_optnumber(L, 4, 0.5);

	MiniDrawer *drw = lua_newuserdata(L, sizeof(*drw));
	memset(drw, 0, sizeof(*drw));

	char *buf = malloc(32);
	snprintf(buf, 32, "drw_%p", (void*)drw);

	drw->id = buf;
	drw->nx = nx; drw->ny = ny;
	drw->nw = nw; drw->nh = nh;

	luaL_getmetatable(L, DRAWER_MT);
	lua_setmetatable(L, -2);

	lua_newtable(L);
	lua_setfenv(L, -2);

	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("newDrawer", "(Ljava/lang/String;FFFF)V");
	jstring jid = JID_DRAWER(env, drw);
	(*env)->CallStaticVoidMethod(env, cls, m, jid, nx, ny, nw, nh);
	(*env)->DeleteLocalRef(env, jid);

	LOGD("Created Drawer Layout Container: %s", buf);
	return 1;
}

static int drw_gc(lua_State *L) {
	MiniDrawer *drw = CHECK_DRAWER(L);
	if (drw->alwaysActive) return 0;
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("removeDrawer", "(Ljava/lang/String;)V");
	jstring jid = JID_DRAWER(env, drw);
	(*env)->CallStaticVoidMethod(env, cls, m, jid);
	(*env)->DeleteLocalRef(env, jid);
	free(drw->id);
	return 0;
}

// kizi did the MOST here wtf
static int drw_addButton(lua_State *L) {
	MiniDrawer *drw = CHECK_DRAWER(L);
	const char *btnId = luaL_checkstring(L, 2);
	const char *label = luaL_checkstring(L, 3);

	const char *resName = luaL_optstring(L, 4, "");
	float textSizeSp = (float)luaL_optnumber(L, 5, -1.0);
	int paddingDp = (int)luaL_optinteger(L, 6, -1);
	int spacingDp = (int)luaL_optinteger(L, 7, -1);

	GET_ENV(); GET_CLS();
	int bgResId = 0;
	if (strlen(resName) > 0) {
		jmethodID getRes = (*env)->GetStaticMethodID(env, cls, "getResIdByName", "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)I");
		jmethodID getCtx = (*env)->GetStaticMethodID(env, cls, "getMetrics", "()Landroid/util/DisplayMetrics;");
		jstring jres = (*env)->NewStringUTF(env, resName);
		jstring jtype = (*env)->NewStringUTF(env, "drawable");
		(*env)->DeleteLocalRef(env, jres);
		(*env)->DeleteLocalRef(env, jtype);
	}

	jmethodID m = VOID_METHOD("drawerAddButtonCustom", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IFII)V");
	jstring jid = JID_DRAWER(env, drw);
	jstring jbid = (*env)->NewStringUTF(env, btnId);
	jstring jlabel = (*env)->NewStringUTF(env, label);

	(*env)->CallStaticVoidMethod(env, cls, m, jid, jbid, jlabel, bgResId, textSizeSp, paddingDp, spacingDp);

	(*env)->DeleteLocalRef(env, jid);
	(*env)->DeleteLocalRef(env, jbid);
	(*env)->DeleteLocalRef(env, jlabel);

	lua_settop(L, 1);
	return 1;
}

static int drw_setStyle(lua_State *L) {
	MiniDrawer *drw = CHECK_DRAWER(L);
	int color = (int)luaL_checkinteger(L, 2);
	float radius = (float)luaL_checknumber(L, 3);

	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("setDrawerStyle", "(Ljava/lang/String;IF)V");
	jstring jid = JID_DRAWER(env, drw);
	(*env)->CallStaticVoidMethod(env, cls, m, jid, color, radius);
	(*env)->DeleteLocalRef(env, jid);

	lua_settop(L, 1);
	return 1;
}

static int drw_getPressedItem(lua_State *L) {
	MiniDrawer *drw = CHECK_DRAWER(L);
	GET_ENV(); GET_CLS();
	jmethodID m = (*env)->GetStaticMethodID(env, cls, "getDrawerPressedItem", "(Ljava/lang/String;)Ljava/lang/String;");
	jstring jid = JID_DRAWER(env, drw);
	jstring res = (*env)->CallStaticObjectMethod(env, cls, m, jid);
	(*env)->DeleteLocalRef(env, jid);

	if (!res) {
		lua_pushnil(L);
		return 1;
	}

	const char *str = (*env)->GetStringUTFChars(env, res, NULL);
	if (str[0] == '\0') {
		(*env)->ReleaseStringUTFChars(env, res, str);
		(*env)->DeleteLocalRef(env, res);
		lua_pushnil(L);
		return 1;
	}

	lua_pushstring(L, str);
	(*env)->ReleaseStringUTFChars(env, res, str);
	(*env)->DeleteLocalRef(env, res);
	return 1;
}

static int drw_setHidden(lua_State *L) {
	MiniDrawer *drw = CHECK_DRAWER(L);
	jboolean hidden = lua_toboolean(L, 2);
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("setDrawerHidden", "(Ljava/lang/String;Z)V");
	jstring jid = JID_DRAWER(env, drw);
	(*env)->CallStaticVoidMethod(env, cls, m, jid, hidden);
	(*env)->DeleteLocalRef(env, jid);
	lua_settop(L, 1);
	return 1;
}

static int drw_removeAllItems(lua_State *L) {
	MiniDrawer *drw = CHECK_DRAWER(L);
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("drawerRemoveAllItems", "(Ljava/lang/String;)V");
	jstring jid = JID_DRAWER(env, drw);

	(*env)->CallStaticVoidMethod(env, cls, m, jid);

	(*env)->DeleteLocalRef(env, jid);
	lua_settop(L, 1);
	return 1;
}

static int drw_index(lua_State *L) {
	lua_getfenv(L, 1);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	if (!lua_isnil(L, -1)) return 1;
	lua_pop(L, 2);
	luaL_getmetatable(L, DRAWER_MT);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	return 1;
}

static const luaL_Reg drawer_methods[] = {
	{"__gc", drw_gc},
	{"addButton", drw_addButton},
	{"setStyle", drw_setStyle},
	{"getPressedItem", drw_getPressedItem},
	{"setHidden", drw_setHidden},
	{"removeAllItems", drw_removeAllItems},
	{NULL, NULL}
};

void miniLL_open_drawer(lua_State *L) {
	luaL_newmetatable(L, DRAWER_MT);
	lua_pushcfunction(L, drw_index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, mini_newindex);
	lua_setfield(L, -2, "__newindex");
	int i;
	for (i = 0; drawer_methods[i].name != NULL; i++) {
		lua_pushcfunction(L, drawer_methods[i].func);
		lua_setfield(L, -2, drawer_methods[i].name);
	}
	lua_pop(L, 1);
}
