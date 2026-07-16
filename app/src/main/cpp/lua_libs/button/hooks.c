#include "button.h"

static void setButtonsHidden(jboolean hidden) {
	JNIEnv *env = miniJ_get_env();

	jclass localCls = (*env)->FindClass(env, "net/itsjustsomedude/swrdg/ButtonController");
	if (!localCls) return;

	jmethodID setHiddenAll = (*env)->GetStaticMethodID(env, localCls, "setHiddenAll", "(Z)V");
	if (setHiddenAll) (*env)->CallStaticVoidMethod(env, localCls, setHiddenAll, hidden);

	(*env)->DeleteLocalRef(env, localCls);
}

static void hideAllButtons(void) {
	setButtonsHidden(JNI_TRUE);
}

static void unhideAllButtons(void) {
	setButtonsHidden(JNI_FALSE);
}

STATIC_DL_HOOK_SYMBOL(
	GameSceneView_SetCinematicMode,
	"_ZN5Caver13GameSceneView23SetCinematicModeEnabledEbbb",
	void, (void *thiz, bool enabled, bool animate, bool unknown)
) {
	if (enabled) {
		hideAllButtons();
	} else {
		unhideAllButtons();
	}
	orig_GameSceneView_SetCinematicMode(thiz, enabled, animate, unknown);
}

#define HIDE_HOOK(NAME, SYMBOL, SIG, ...) \
STATIC_DL_HOOK_SYMBOL( \
    NAME, \
    SYMBOL, \
    void, SIG \
) { \
    hideAllButtons(); \
    orig_##NAME(__VA_ARGS__); \
}

#define SHOW_HOOK(NAME, SYMBOL, SIG, ...) \
STATIC_DL_HOOK_SYMBOL( \
    NAME, \
    SYMBOL, \
    void, SIG \
) { \
    unhideAllButtons(); \
    orig_##NAME(__VA_ARGS__); \
}

HIDE_HOOK(GameMenu_LoadView, "_ZN5Caver22GameMenuViewController8LoadViewEv", (void *thiz), thiz)
SHOW_HOOK(GameMenu_ViewWillDisappear, "_ZN5Caver22GameMenuViewController17ViewWillDisappearEv", (void *thiz), thiz)

HIDE_HOOK(PauseView_C, "_ZN5Caver9PauseViewC2Ev", (void *thiz), thiz)
SHOW_HOOK(PauseView_D, "_ZN5Caver9PauseView10TouchEndedERKNS_7FWTouchE", (void *thiz, void *touch), thiz, touch)

HIDE_HOOK(PortalView_AnimateIn, "_ZN5Caver10PortalView9AnimateInEv", (void *thiz), thiz)
SHOW_HOOK(PortalView_AnimateOut, "_ZN5Caver10PortalView10AnimateOutEv", (void *thiz), thiz)

HIDE_HOOK(SkillPicker_Load, "_ZN5Caver25SkillPickerViewController8LoadViewEv", (void *thiz), thiz)
SHOW_HOOK(SkillPicker_Destroy, "_ZN5Caver25SkillPickerViewControllerD0Ev", (void *thiz), thiz)

STATIC_DL_HOOK_SYMBOL(
	GameOverView_C,
	"_ZN5Caver12GameOverViewC2Ev",
	void, (void *thiz)
) {
	orig_GameOverView_C(thiz);
	button_remove_all();
}

void miniLL_install_button_hooks(void) {
	hook_GameMenu_LoadView();
	hook_GameMenu_ViewWillDisappear();
	hook_GameSceneView_SetCinematicMode();
	hook_PauseView_C();
	hook_PauseView_D();
	hook_PortalView_AnimateIn();
	hook_PortalView_AnimateOut();
	hook_SkillPicker_Load();
	hook_SkillPicker_Destroy();
}

void initLL_button(void) {
	LOGD("Initialized Kiwi ButtonController library.");
	JNIEnv *env = miniJ_get_env();
	g_cls = (*env)->NewGlobalRef(env, (*env)->FindClass(env, JCLASS));
	miniLL_install_button_hooks();
}
