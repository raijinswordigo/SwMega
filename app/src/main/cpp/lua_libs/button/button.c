#include "button.h"

jclass g_cls = NULL;

// userdata
int mini_newindex(lua_State *L) {
	lua_getfenv(L, 1);
	lua_pushvalue(L, 2);
	lua_pushvalue(L, 3);
	lua_settable(L, -3);
	return 0;
}

// Add Button, everything starts from here....
static void java_add_button(MiniButton *btn) {
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("addButton", "(Ljava/lang/String;Ljava/lang/String;FFFF)V");
	jstring jid    = JID(env, btn);
	jstring jlabel = (*env)->NewStringUTF(env, btn->label);
	(*env)->CallStaticVoidMethod(env, cls, m, jid, jlabel, btn->nx, btn->ny, btn->nw, btn->nh);
	(*env)->DeleteLocalRef(env, jid);
	(*env)->DeleteLocalRef(env, jlabel);
}

// C Ctor
static int NewButton(lua_State *L) {
	const char *label = luaL_checkstring(L, 1);
	float nx = (float)luaL_checknumber(L, 2);
	float ny = (float)luaL_checknumber(L, 3);
	float nw = (float)luaL_checknumber(L, 4);
	float nh = (float)luaL_checknumber(L, 5);

	MiniButton *btn = lua_newuserdata(L, sizeof(*btn));
	memset(btn, 0, sizeof(*btn));

	char *buf = malloc(32);
	snprintf(buf, 32, "btn_%p", (void*)btn);

	btn->label = strdup(label);
	btn->id = buf;
	btn->nx = nx; btn->ny = ny;
	btn->nw = nw; btn->nh = nh;

	luaL_getmetatable(L, BUTTON_MT);
	lua_setmetatable(L, -2);

	lua_newtable(L);
	lua_setfenv(L, -2);

	java_add_button(btn);
	LOGD("Created Button: '%s' (%s)", label, buf);
	return 1;
}

/* META */

static int b_gc(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	if (btn->alwaysActive) return 0;
	if (btn->deleted) return 0;
	btn->deleted = 1;
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("removeButton", "(Ljava/lang/String;)V");
	jstring jid = JID(env, btn);
	(*env)->CallStaticVoidMethod(env, cls, m, jid);
	(*env)->DeleteLocalRef(env, jid);
	free(btn->id);
	free(btn->label);
	btn->id = NULL;
	btn->label = NULL;
	return 0;
}

static int b_delete(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	if (btn->deleted) return 0;
	btn->deleted = 1;
	CALL_VOID_ID("removeButton", "(Ljava/lang/String;)V");
	return 0;
}

static int b_isDeleted(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	lua_pushboolean(L, btn->deleted);
	return 1;
}

static int b_getID(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	lua_pushstring(L, btn->id);
	return 1;
}

static int b_isPressed(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	jboolean r; CALL_BOOL_ID("isPressed", "(Ljava/lang/String;)Z", r);
	lua_pushboolean(L, r);
	return 1;
}

static int b_isDragging(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	jboolean r; CALL_BOOL_ID("isDragging", "(Ljava/lang/String;)Z", r);
	lua_pushboolean(L, r);
	return 1;
}

static int b_setPosition(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	float nx = (float)luaL_checknumber(L, 2);
	float ny = (float)luaL_checknumber(L, 3);
	btn->nx = nx; btn->ny = ny;
	CALL_VOID_ID("setPosition", "(Ljava/lang/String;FF)V", nx, ny);
	lua_settop(L, 1);
	return 1;
}

static int b_getPosition(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	GET_ENV(); GET_CLS();
	jmethodID m = (*env)->GetStaticMethodID(env, cls, "getPosition", "(Ljava/lang/String;)[F");
	jstring jid = JID(env, btn);
	jfloatArray arr = (*env)->CallStaticObjectMethod(env, cls, m, jid);
	jfloat *v = (*env)->GetFloatArrayElements(env, arr, NULL);
	lua_pushnumber(L, v[0]);
	lua_pushnumber(L, v[1]);
	(*env)->ReleaseFloatArrayElements(env, arr, v, 0);
	(*env)->DeleteLocalRef(env, jid);
	(*env)->DeleteLocalRef(env, arr);
	return 2;
}

static int b_setHidden(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	jboolean hidden = lua_toboolean(L, 2);
	CALL_VOID_ID("setHidden", "(Ljava/lang/String;Z)V", hidden);
	lua_settop(L, 1);
	return 1;
}

static int b_setClickable(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	int clickable = lua_toboolean(L, 2);
	CALL_VOID_ID("setClickable", "(Ljava/lang/String;I)V", clickable);
	lua_settop(L, 1);
	return 1;
}

static int b_setScaling(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	float sx = (float)luaL_checknumber(L, 2);
	float sy = (float)luaL_checknumber(L, 3);
	CALL_VOID_ID("setScaling", "(Ljava/lang/String;FF)V", sx, sy);
	lua_settop(L, 1);
	return 1;
}

static int b_setDimensions(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	float nw = (float)luaL_checknumber(L, 2);
	float nh = (float)luaL_checknumber(L, 3);
	btn->nw = nw; btn->nh = nh;
	CALL_VOID_ID("setDimensions", "(Ljava/lang/String;FF)V", nw, nh);
	lua_settop(L, 1);
	return 1;
}

static int b_setAlpha(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	int alpha = (int)luaL_checkinteger(L, 2);
	CALL_VOID_ID("setAlpha", "(Ljava/lang/String;I)V", alpha);
	lua_settop(L, 1);
	return 1;
}

static int b_setBackgroundAlpha(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	int alpha = (int)luaL_checkinteger(L, 2);
	CALL_VOID_ID("setBackgroundAlpha", "(Ljava/lang/String;I)V", alpha);
	lua_settop(L, 1);
	return 1;
}

static int b_setBackgroundResource(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	const char *res = luaL_checkstring(L, 2);
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("setBackgroundResource", "(Ljava/lang/String;Ljava/lang/String;)V");
	jstring jid  = JID(env, btn);
	jstring jres = (*env)->NewStringUTF(env, res);
	(*env)->CallStaticVoidMethod(env, cls, m, jid, jres);
	(*env)->DeleteLocalRef(env, jid);
	(*env)->DeleteLocalRef(env, jres);
	lua_settop(L, 1);
	return 1;
}

static int b_setText(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	const char *text = luaL_checkstring(L, 2);
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("setText", "(Ljava/lang/String;Ljava/lang/String;)V");
	jstring jid   = JID(env, btn);
	jstring jtext = (*env)->NewStringUTF(env, text);
	(*env)->CallStaticVoidMethod(env, cls, m, jid, jtext);
	(*env)->DeleteLocalRef(env, jid);
	(*env)->DeleteLocalRef(env, jtext);
	lua_settop(L, 1);
	return 1;
}

static int b_setTextSizeSp(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	float sp = (float)luaL_checknumber(L, 2);
	CALL_VOID_ID("setTextSizeSp", "(Ljava/lang/String;F)V", sp);
	lua_settop(L, 1);
	return 1;
}

static int b_setTextFont(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	const char *font = luaL_checkstring(L, 2);
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("setTextFont", "(Ljava/lang/String;Ljava/lang/String;)V");
	jstring jid   = JID(env, btn);
	jstring jfont = (*env)->NewStringUTF(env, font);
	(*env)->CallStaticVoidMethod(env, cls, m, jid, jfont);
	(*env)->DeleteLocalRef(env, jid);
	(*env)->DeleteLocalRef(env, jfont);
	lua_settop(L, 1);
	return 1;
}

static int b_setTextScale(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	float scale = (float)luaL_checknumber(L, 2);
	CALL_VOID_ID("setTextScale", "(Ljava/lang/String;F)V", scale);
	lua_settop(L, 1);
	return 1;
}

static int b_setTextColor(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	int color = (int)luaL_checkinteger(L, 2);
	CALL_VOID_ID("setTextColor", "(Ljava/lang/String;I)V", color);
	lua_settop(L, 1);
	return 1;
}

static int b_makeMovable(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	jboolean snapback = lua_toboolean(L, 2);
	CALL_VOID_ID("makeMovable", "(Ljava/lang/String;Z)V", snapback);
	lua_settop(L, 1);
	return 1;
}

static int b_setPadding(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	int l = (int)luaL_checkinteger(L, 2);
	int t = (int)luaL_checkinteger(L, 3);
	int r = (int)luaL_checkinteger(L, 4);
	int b = (int)luaL_checkinteger(L, 5);
	CALL_VOID_ID("setPadding", "(Ljava/lang/String;IIII)V", l, t, r, b);
	lua_settop(L, 1);
	return 1;
}

static int b_setAlignment(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	int gravity = (int)luaL_checkinteger(L, 2);
	CALL_VOID_ID("setAlignment", "(Ljava/lang/String;I)V", gravity);
	lua_settop(L, 1);
	return 1;
}

static int b_setConfined(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	jboolean confined = lua_toboolean(L, 2);
	CALL_VOID_ID("setButtonConfined", "(Ljava/lang/String;Z)V", confined);
	lua_settop(L, 1);
	return 1;
}

static int b_setAlwaysActive(lua_State *L) {
	MiniButton *btn = CHECK_BTN(L);
	btn->alwaysActive = (int)lua_toboolean(L, 1);
	lua_settop(L, 1);
	return 1;
}

static int b_index(lua_State *L) {
	lua_getfenv(L, 1);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	if (!lua_isnil(L, -1)) return 1;
	lua_pop(L, 2);
	luaL_getmetatable(L, BUTTON_MT);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	return 1;
}

// Library
static int removeAll(lua_State *L) {
	(void)L;
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("removeAll", "()V");
	(*env)->CallStaticVoidMethod(env, cls, m);
	return 0;
}

void button_remove_all(void) {
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("removeAll", "()V");
	(*env)->CallStaticVoidMethod(env, cls, m);
}

static int setHiddenAll(lua_State *L) {
	jboolean hidden = lua_toboolean(L, 1);
	GET_ENV(); GET_CLS();
	jmethodID m = VOID_METHOD("setHiddenAll", "(Z)V");
	(*env)->CallStaticVoidMethod(env, cls, m, hidden);
	return 0;
}

// FDec
int NewOverlay(lua_State *L);
int NewDrawer(lua_State *L);
int NewSeekBar(lua_State *L);
int NewTextInput(lua_State *L);

static const luaL_Reg overlay_library[] = {
	{"NewButton",  NewButton},
	{"NewOverlay", NewOverlay},
	{"NewDrawer",  NewDrawer},
	{"NewSeekBar", NewSeekBar},
	{"NewTextInput", NewTextInput},
	{"RemoveAll",  removeAll},
	{"HideAll",    setHiddenAll},
	{NULL, NULL}
};

static const luaL_Reg button_methods[] = {
	{"__gc", b_gc},
	{"getID", b_getID},
	{"isPressed", b_isPressed},
	{"isDragging", b_isDragging},
	{"setPosition", b_setPosition},
	{"getPosition", b_getPosition},
	{"setHidden", b_setHidden},
	{"setClickable", b_setClickable},
	{"setScaling", b_setScaling},
	{"setDimensions", b_setDimensions},
	{"setConfined", b_setConfined},
	{"setAlpha", b_setAlpha},
	{"setBackgroundAlpha", b_setBackgroundAlpha},
	{"setBackgroundResource", b_setBackgroundResource},
	{"setText", b_setText},
	{"setTextFont", b_setTextFont},
	{"setTextScale", b_setTextScale},
	{"setTextSizeSp", b_setTextSizeSp},
	{"setTextColor", b_setTextColor},
	{"makeMovable", b_makeMovable},
	{"setPadding", b_setPadding},
	{"setAlignment", b_setAlignment},
	{"delete", b_delete},
	{"isDeleted", b_isDeleted},
	{"setAlwaysActive", b_setAlwaysActive},
	{NULL, NULL}
};

int miniLL_open_button(lua_State *L) {
	luaL_newmetatable(L, BUTTON_MT);
	lua_pushcfunction(L, b_index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, mini_newindex);
	lua_setfield(L, -2, "__newindex");
	int i;
	for (i = 0; button_methods[i].name != NULL; i++) {
		lua_pushcfunction(L, button_methods[i].func);
		lua_setfield(L, -2, button_methods[i].name);
	}
	lua_pop(L, 1);

	miniLL_open_overlay(L);
	miniLL_open_drawer(L);
	miniLL_open_seekbar(L);
	miniLL_open_textinput(L);

	lua_newtable(L);
	for (i = 0; overlay_library[i].name != NULL; i++) {
		lua_pushcfunction(L, overlay_library[i].func);
		lua_setfield(L, -2, overlay_library[i].name);
	}
	return 1;
}
