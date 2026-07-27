#include "core/hooks.h"
#include "core/log.h"
#include "init/lua_libs.h"
#include "lauxlib.h"
#include "libs.h"
#include "lua.h"

#define LOG_TAG "MiniGameController"

STATIC_DL_FUNCTION_SYMBOL(
    gvc_ResetView,
    "_ZN5Caver18GameViewController9ResetViewEv",
    void, (void *gvc)
)

STATIC_DL_FUNCTION_SYMBOL(
    gvc_ShowMenu,
    "_ZN5Caver18GameViewController8ShowMenuEv",
    void, (void *gvc)
)

STATIC_DL_FUNCTION_SYMBOL(
    gsc_HandleFall,
    "_ZN5Caver19GameSceneController10HandleFallEv",
    void, (void *gsc)
)

STATIC_DL_FUNCTION_SYMBOL(
    gsc_SpawnHeroAt,
    "_ZN5Caver19GameSceneController11SpawnHeroAtERKSs",
    void, (void *gsc, const char *portalName)
)

STATIC_DL_FUNCTION_SYMBOL(
    gsc_ApplyLevelUp,
    "_ZN5Caver19GameSceneController12ApplyLevelUpEiii",
    void, (void *gsc, int health, int attack, int magic)
)

STATIC_DL_FUNCTION_SYMBOL(
    gsc_UnequipArmor,
    "_ZN5Caver19GameSceneController12UnequipArmorEv",
    void, (void *gsc)
)

STATIC_DL_FUNCTION_SYMBOL(
    gsc_UpdateTarget,
    "_ZN5Caver19GameSceneController12UpdateTargetEv",
    void, (void *gsc)
)

STATIC_DL_FUNCTION_SYMBOL(
    gsc_GameControlButtonUp,
    "_ZN5Caver19GameSceneController19GameControlButtonUpENS_17GameControlButtonE",
    void, (void *gsc, int button)
)

STATIC_DL_FUNCTION_SYMBOL(
    gsc_GameControlButtonDown,
    "_ZN5Caver19GameSceneController21GameControlButtonDownENS_17GameControlButtonE",
    void, (void *gsc, int button)
)

STATIC_DL_FUNCTION_SYMBOL(
    gsc_Update,
    "_ZN5Caver19GameSceneController6UpdateEf",
    void, (void *gsc, float dt)
)

STATIC_DL_FUNCTION_SYMBOL(
	gvc_SaveGameState,
	"_ZN5Caver18GameViewController13SaveGameStateEb",
	void, (void *gvc, bool something)
)

static void *get_gvc(lua_State *L) {
    lua_getglobal(L, "gameController");
    const void *gvc = lua_topointer(L, -1);
    LOGD("Found GameViewController: %p", gvc);
    lua_pop(L, 1);
    return (void*)gvc;
}

static void *get_gsc(lua_State *L) {
    void *gvc = get_gvc(L);
    void *gsc = *$(void*, gvc, 0x68, 0xc8);
    LOGD("Found GameSceneController: %p", gsc);
    return gsc;
}

static int L_ResetView(lua_State *L) {
    gvc_ResetView(get_gvc(L));
    return 0;
}

static int L_ShowMenu(lua_State *L) {
    gvc_ShowMenu(get_gvc(L));
    return 0;
}

static int L_HandleFall(lua_State *L) {
    gsc_HandleFall(get_gsc(L));
    return 0;
}

static int L_SpawnHeroAt(lua_State *L) {
    const char *portalName = luaL_checkstring(L, 1);
    gsc_SpawnHeroAt(get_gsc(L), portalName);
    return 0;
}

static int L_ApplyLevelUp(lua_State *L) {
    int health = (int)luaL_checkinteger(L, 1);
    int attack = (int)luaL_checkinteger(L, 2);
    int magic = (int)luaL_checkinteger(L, 3);
    gsc_ApplyLevelUp(get_gsc(L), health, attack, magic);
    return 0;
}

static int L_UnequipArmor(lua_State *L) {
    gsc_UnequipArmor(get_gsc(L));
    return 0;
}

static int L_UpdateTarget(lua_State *L) {
    gsc_UpdateTarget(get_gsc(L));
    return 0;
}

static int L_GameControlButtonUp(lua_State *L) {
    int button = (int)luaL_checkinteger(L, 1);
    gsc_GameControlButtonUp(get_gsc(L), button);
    return 0;
}

static int L_GameControlButtonDown(lua_State *L) {
    int button = (int)luaL_checkinteger(L, 1);
    gsc_GameControlButtonDown(get_gsc(L), button);
    return 0;
}

static int L_Update(lua_State *L) {
    float dt = (float)luaL_checknumber(L, 1);
    gsc_Update(get_gsc(L), dt);
    return 0;
}

static int L_SaveGameState(lua_State *L) {
	bool save = (bool)lua_toboolean(L, 1);
    gvc_SaveGameState(get_gvc(L), save);
    return 0;
}

static const luaL_Reg gc_lib[] = {
    {"ResetView", L_ResetView},
    {"ShowMenu", L_ShowMenu},
    {"HandleFall", L_HandleFall},
    {"SpawnHeroAt", L_SpawnHeroAt},
    {"ApplyLevelUp", L_ApplyLevelUp},
    {"UnequipArmor", L_UnequipArmor},
    {"UpdateTarget", L_UpdateTarget},
    {"GameControlButtonUp", L_GameControlButtonUp},
    {"GameControlButtonDown", L_GameControlButtonDown},
	{"SaveGameState", L_SaveGameState},
    {"Update", L_Update},
    {NULL, NULL}
};

LUALIB_API int miniLL_open_game_controller(lua_State *L) {
    lua_newtable(L);
    luaL_register(L, NULL, gc_lib);
    return 1;
}

void initLL_game_controller() {
    dlsym_gvc_ResetView();
    dlsym_gvc_ShowMenu();
    dlsym_gsc_HandleFall();
    dlsym_gsc_SpawnHeroAt();
    dlsym_gsc_ApplyLevelUp();
    dlsym_gsc_UnequipArmor();
    dlsym_gsc_UpdateTarget();
    dlsym_gsc_GameControlButtonUp();
    dlsym_gsc_GameControlButtonDown();
	dlsym_gvc_SaveGameState();
    dlsym_gsc_Update();
}