#include "caver/types.h"
#include "core/hooks.h"
#include "core/log.h"
#include "mini.h"

#define LOG_TAG "MegaDepression"

void *next = NULL;

STATIC_DL_HOOK_SYMBOL(
    SetColor,
    "_ZN5Caver16RenderingContext8SetColorERKNS_5ColorE",
    void, (void *rctx, unsigned int *color)
) {
    unsigned int red = 0xff0000ff;
    orig_SetColor(rctx, (next == rctx) ? &red : color);
	if (next) next = NULL;
}

STATIC_DL_HOOK_SYMBOL(
	DrawRect,
	"_ZN5Caver7MapView8DrawRectEPNS_16RenderingContextERKNS_9RectangleERKNS_7Matrix4E",
	void, (void *MapView, void *rctx, Rectangle *rect, Matrix4 *m)
) {
	next = rctx;
	orig_DrawRect(MapView, rctx, rect, m);
}

int miniLL_test(lua_State *L) {

    return 0;
}

void initLL_test() {
	hook_DrawRect();
	hook_SetColor();
}