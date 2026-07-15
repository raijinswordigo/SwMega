package net.itsjustsomedude.swrdg;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.text.InputType;
import android.util.Log;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;

import java.lang.ref.WeakReference;
import java.util.HashMap;
import android.util.DisplayMetrics;

@SuppressWarnings("unused")
public class ButtonController {
	private static final String LOG_TAG = "MiniBtnController";

	private static class MovableButtonData {
		public boolean dragging;

		public float touchOffsetX;
		public float touchOffsetY;

		public MovableButtonData(
		) {
		}
	}

	private static class SeekBarData {
		public android.widget.SeekBar seekBar;
		public boolean hidden;
		public boolean touching;
		public String overlayId;

		float nw;
		float nh;
		float nx;
		float ny;

		public SeekBarData(android.widget.SeekBar seekBar) {
			this.seekBar = seekBar;
		}
	}

	private static class TextInputData {
		public final EditText editText;

		public float nx, ny, nw, nh;
		public boolean hidden;
		public String overlayId;

		TextInputData(EditText editText) {
			this.editText = editText;
		}
	}

	private static class ButtonData {
		public Button button;
		public boolean pressed;
		public float baseTextSize;
		public MovableButtonData movable;
		public boolean hidden;
		public String overlayId;
		public boolean confined;

		float nw;
		float nh;
		float nx;
		float ny;

		public ButtonData(Button button) {
			this.button = button;
			this.baseTextSize = button.getTextSize();
		}
	}

	private static class OverlayData {
		public FrameLayout frame;
		public boolean hidden;
		public boolean movable;
		public boolean pinchable;
		public float scaleFactor = 1.0f;
		public boolean pinching = false;
		public boolean needsResync;

		public float nx;
		public float ny;
		public float nw;
		public float nh;

		public boolean dragging;
		public float touchOffsetX;
		public float touchOffsetY;

		public android.view.ScaleGestureDetector scaleDetector;

		public OverlayData(FrameLayout frame) {
			this.frame = frame;
		}
	}

	private static final HashMap<String, DrawerData> drawers = new HashMap<>();
	private static final HashMap<String, SeekBarData> seekBars = new HashMap<>();
	private static final HashMap<String, TextInputData> textInputs = new HashMap<>();

	private static class DrawerData {
		public FrameLayout frame;
		public android.widget.LinearLayout contentLayout;
		public android.widget.ScrollView scrollView;
		public String lastPressedButtonId = "";
		public String parentOverlayId = null;
		public int itemSpacingDp = 6;
		public int itemPaddingDp = 12;
		public float itemTextSizePx = -1;
		public int itemBgResource = 0;

		public float nx, ny, nw, nh;

		public DrawerData(FrameLayout frame, android.widget.LinearLayout contentLayout, android.widget.ScrollView scrollView) {
			this.frame = frame;
			this.contentLayout = contentLayout;
			this.scrollView = scrollView;
		}
	}

	private static DisplayMetrics getMetrics() {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null)
			return null;

		return ctx.getResources().getDisplayMetrics();
	}

	private static int screenWidth() {
		DisplayMetrics dm = getMetrics();
		return dm == null ? 0 : dm.widthPixels;
	}

	private static int screenHeight() {
		DisplayMetrics dm = getMetrics();
		return dm == null ? 0 : dm.heightPixels;
	}

	private static int rootWidth() {
		ViewGroup root = viewRef.get();
		if (root != null && root.getWidth() > 0) return root.getWidth();
		return screenWidth();
	}

	private static int rootHeight() {
		ViewGroup root = viewRef.get();
		if (root != null && root.getHeight() > 0) return root.getHeight();
		return screenHeight();
	}

	private static int squareBase() {
		return Math.min(rootWidth(), rootHeight());
	}

	private static int dp(float dp) {
		DisplayMetrics dm = getMetrics();
		assert dm != null;
		return (int)(dp * dm.density);
	}

	private static boolean globallyHidden = false;
	private static WeakReference<MainActivity> mainActivityRef;
	private static WeakReference<ViewGroup> viewRef;

	private static final HashMap<String, ButtonData> buttons = new HashMap<>();
	private static final HashMap<String, OverlayData> overlays = new HashMap<>();

	public static void init(MainActivity act, ViewGroup view) {
		mainActivityRef = new WeakReference<>(act);
		viewRef = new WeakReference<>(view);
	}

	private static int getSquareBase(ViewGroup root) {
		return Math.min(root.getWidth(), root.getHeight());
	}

	@SuppressLint("ClickableViewAccessibility")
	private static void reattachOverlayTouchListener(String id) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		OverlayData data = overlays.get(id);
		if (data == null) return;

		if (data.scaleDetector == null) {
			data.scaleDetector = new android.view.ScaleGestureDetector(ctx,
					new android.view.ScaleGestureDetector.SimpleOnScaleGestureListener() {
						@Override
						@SuppressWarnings("NullableProblems")
						public boolean onScale(android.view.ScaleGestureDetector detector) {
							if (!data.pinchable) return false;
							data.scaleFactor = detector.getScaleFactor();
							data.pinching = true;
							return true;
						}

						@Override
						@SuppressWarnings("NullableProblems")
						public void onScaleEnd(android.view.ScaleGestureDetector detector) {
							data.pinching = false;
							data.scaleFactor = 1.0f;
						}
					}
			);
		}

		data.frame.setOnTouchListener((v, e) -> {
			data.scaleDetector.onTouchEvent(e);

			if (data.movable) {
				FrameLayout.LayoutParams flp = (FrameLayout.LayoutParams) data.frame.getLayoutParams();
				switch (e.getActionMasked()) {
					case MotionEvent.ACTION_DOWN:
						data.dragging = true;
						data.needsResync = false;
						data.touchOffsetX = e.getRawX() - flp.leftMargin;
						data.touchOffsetY = e.getRawY() - flp.topMargin;
						break;
					case MotionEvent.ACTION_POINTER_DOWN:
						data.dragging = false;
						break;
					case MotionEvent.ACTION_POINTER_UP:
						if (e.getPointerCount() - 1 == 1) {
							data.needsResync = true;
						}
						break;
					case MotionEvent.ACTION_MOVE:
						if (e.getPointerCount() > 1) break;
						if (data.needsResync) {
							data.touchOffsetX = e.getRawX() - flp.leftMargin;
							data.touchOffsetY = e.getRawY() - flp.topMargin;
							data.dragging = true;
							data.needsResync = false;
							break;
						}
						if (!data.dragging) break;
						flp.leftMargin = (int)(e.getRawX() - data.touchOffsetX);
						flp.topMargin = (int)(e.getRawY() - data.touchOffsetY);
						data.frame.setLayoutParams(flp);
						break;
					case MotionEvent.ACTION_UP:
					case MotionEvent.ACTION_CANCEL:
						data.dragging = false;
						data.needsResync = false;
						break;
				}
			}
			return true;
		});
	}

	@SuppressLint("ClickableViewAccessibility")
	public static void newOverlay(String id, float nx, float ny, float nw, float nh) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			if (overlays.containsKey(id))
				return;

			int rw = rootWidth();
			int rh = rootHeight();

			int widthPx = (int) (rw * nw);
			int heightPx = (int) (rh * nh);

			FrameLayout frame = new FrameLayout(ctx);

			android.graphics.drawable.GradientDrawable bg = new android.graphics.drawable.GradientDrawable();
			bg.setColor(0xCC000000);
			bg.setCornerRadius(dp(12));
			frame.setBackground(bg);

			FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(widthPx, heightPx);
			lp.leftMargin = (int) (rw * nx) - widthPx / 2;
			lp.topMargin = (int) (rh * ny) - heightPx / 2;

			OverlayData data = new OverlayData(frame);
			data.nx = nx;
			data.ny = ny;
			data.nw = nw;
			data.nh = nh;

			root.addView(frame, lp);
			overlays.put(id, data);
			reattachOverlayTouchListener(id);
			Log.d(LOG_TAG, "Added overlay: " + id);
		});
	}

	public static void setOverlayMovable(String id, boolean movable) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(id);
			if (data == null) return;
			data.movable = movable;
			reattachOverlayTouchListener(id);
		});
	}

	public static void setOverlayPinchable(String id, boolean pinchable) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(id);
			if (data == null) return;
			data.pinchable = pinchable;
			reattachOverlayTouchListener(id);
		});
	}

	public static void setOverlayBackgroundColor(String id, int color) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(id);
			if (data == null) return;
			android.graphics.drawable.Drawable bg = data.frame.getBackground();
			if (bg instanceof android.graphics.drawable.GradientDrawable)
				((android.graphics.drawable.GradientDrawable) bg).setColor(color);
		});
	}

	public static void setOverlayBackgroundAlpha(String id, int alpha) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(id);
			if (data == null) return;
			data.frame.getBackground().setAlpha(alpha);
		});
	}

	public static void setOverlayCornerRadius(String id, float radiusDp) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(id);
			if (data == null) return;
			android.graphics.drawable.Drawable bg = data.frame.getBackground();
			if (bg instanceof android.graphics.drawable.GradientDrawable)
				((android.graphics.drawable.GradientDrawable) bg).setCornerRadius(dp(radiusDp));
		});
	}

	public static void setOverlayDimensions(String id, float nw, float nh) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(id);
			if (data == null) return;

			data.nw = nw;
			data.nh = nh;

			int rw = rootWidth();
			int rh = rootHeight();

			int widthPx = (int)(rw * nw);
			int heightPx = (int)(rh * nh);

			FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) data.frame.getLayoutParams();
			lp.width = widthPx;
			lp.height = heightPx;
			lp.leftMargin = (int)(rw * data.nx) - widthPx / 2;
			lp.topMargin = (int)(rh * data.ny) - heightPx / 2;
			data.frame.setLayoutParams(lp);
		});
	}

	public static void removeOverlay(String id) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(id);
			if (data == null)
				return;

			for (HashMap.Entry<String, ButtonData> entry : buttons.entrySet()) {
				if (id.equals(entry.getValue().overlayId))
					entry.getValue().overlayId = null;
			}

			root.removeView(data.frame);
			overlays.remove(id);
			Log.d(LOG_TAG, "Removed overlay: " + id);
		});
	}

	public static void overlayAddButton(String overlayId, String btnId) {
		MainActivity ctx = mainActivityRef.get();

		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			OverlayData overlay = overlays.get(overlayId);
			ButtonData btn = buttons.get(btnId);

			if (overlay == null || btn == null) return;

			ViewGroup oldParent = (ViewGroup) btn.button.getParent();
			if (oldParent != null) oldParent.removeView(btn.button);

			int overlayWidthPx  = (int)(rootWidth()  * overlay.nw);
			int overlayHeightPx = (int)(rootHeight() * overlay.nh);
			int widthPx  = (int)(overlayWidthPx  * btn.nw);
			int heightPx = (int)(overlayHeightPx * btn.nh);

			FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(widthPx, heightPx);
			lp.leftMargin = (int)(overlayWidthPx  * btn.nx) - widthPx  / 2;
			lp.topMargin = (int)(overlayHeightPx * btn.ny) - heightPx / 2;

			btn.baseTextSize = heightPx * 0.35f;
			btn.button.setTextSize(android.util.TypedValue.COMPLEX_UNIT_PX, btn.baseTextSize);

			btn.overlayId = overlayId;
			overlay.frame.addView(btn.button, lp);
			Log.d(LOG_TAG, "Moved button " + btnId + " into overlay " + overlayId);
		});
	}

	public static void overlayAddSeparator(String overlayId, float ny) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(overlayId);
			if (data == null) return;

			View sep = new View(ctx);
			sep.setBackgroundColor(0x88ffffff);

			int overlayHeightPx = (int)(rootHeight() * data.nh);
			int overlayWidthPx  = (int)(rootWidth()  * data.nw);

			FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(overlayWidthPx, dp(1));
			lp.topMargin = (int)(overlayHeightPx * ny);

			data.frame.addView(sep, lp);
			Log.d(LOG_TAG, "Added separator to overlay: " + overlayId);
		});
	}

	public static void setOverlayHidden(String id, boolean hidden) {
		MainActivity ctx = mainActivityRef.get();

		if (ctx == null)
			return;

		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(id);
			if (data == null)
				return;
			data.hidden = hidden;
			boolean visible = !globallyHidden && !hidden;
			data.frame.setVisibility(visible ? View.VISIBLE : View.GONE);
		});
	}

	public static void setOverlayPosition(String id, float nx, float ny) {
		MainActivity ctx = mainActivityRef.get();

		if (ctx == null)
			return;

		ctx.runOnUiThread(() -> {
			OverlayData data = overlays.get(id);
			if (data == null)
				return;

			data.nx = nx;
			data.ny = ny;

			int rw = rootWidth();
			int rh = rootHeight();

			int widthPx  = (int)(rw * data.nw);
			int heightPx = (int)(rh * data.nh);

			FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) data.frame.getLayoutParams();
			lp.leftMargin = (int)(rw * nx) - widthPx  / 2;
			lp.topMargin  = (int)(rh * ny) - heightPx / 2;
			data.frame.setLayoutParams(lp);
		});
	}

	public static float[] getOverlayPosition(String id) {
		OverlayData data = overlays.get(id);
		if (data == null) return new float[]{ 0f, 0f };

		FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) data.frame.getLayoutParams();
		float nx = (lp.leftMargin + data.frame.getWidth()  / 2.0f) / rootWidth();
		float ny = (lp.topMargin  + data.frame.getHeight() / 2.0f) / rootHeight();
		return new float[]{ nx, ny };
	}

	public static float getOverlayScaleFactor(String id) {
		OverlayData data = overlays.get(id);
		return data != null ? data.scaleFactor : 1.0f;
	}

	public static boolean isOverlayPinching(String id) {
		OverlayData data = overlays.get(id);
		return data != null && data.pinching;
	}

	@SuppressLint("ClickableViewAccessibility")
	public static void newDrawer(String id, float nx, float ny, float nw, float nh) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null) return;

		ctx.runOnUiThread(() -> {
			if (drawers.containsKey(id)) return;

			int rw = rootWidth();
			int rh = rootHeight();
			int widthPx = (int) (rw * nw);
			int heightPx = (int) (rh * nh);

			FrameLayout frame = new FrameLayout(ctx);
			android.graphics.drawable.GradientDrawable bg = new android.graphics.drawable.GradientDrawable();
			bg.setColor(0xEE1A1A1A);
			bg.setCornerRadius(dp(8));
			frame.setBackground(bg);

			android.widget.ScrollView scrollView = new android.widget.ScrollView(ctx);
			scrollView.setFillViewport(true);
			scrollView.setVerticalScrollBarEnabled(true);

			android.widget.LinearLayout contentLayout = new android.widget.LinearLayout(ctx);
			contentLayout.setOrientation(android.widget.LinearLayout.VERTICAL);
			contentLayout.setPadding(dp(8), dp(8), dp(8), dp(8));

			scrollView.addView(contentLayout, new FrameLayout.LayoutParams(
					ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));
			frame.addView(scrollView, new FrameLayout.LayoutParams(
					ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

			FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(widthPx, heightPx);
			lp.leftMargin = (int) (rw * nx) - widthPx / 2;
			lp.topMargin = (int) (rh * ny) - heightPx / 2;

			root.addView(frame, lp);

			DrawerData data = new DrawerData(frame, contentLayout, scrollView);
			data.nx = nx; data.ny = ny; data.nw = nw; data.nh = nh;
			drawers.put(id, data);
		});
	}

	public static void overlayAddDrawer(String overlayId, String drawerId) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			OverlayData overlay = overlays.get(overlayId);
			DrawerData drawer = drawers.get(drawerId);
			if (overlay == null || drawer == null) return;

			ViewGroup oldParent = (ViewGroup) drawer.frame.getParent();
			if (oldParent != null) oldParent.removeView(drawer.frame);

			int overlayWidthPx  = (int) (rootWidth()  * overlay.nw);
			int overlayHeightPx = (int) (rootHeight() * overlay.nh);
			int widthPx  = (int) (overlayWidthPx  * drawer.nw);
			int heightPx = (int) (overlayHeightPx * drawer.nh);

			FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(widthPx, heightPx);
			lp.leftMargin = (int) (overlayWidthPx  * drawer.nx) - widthPx  / 2;
			lp.topMargin  = (int) (overlayHeightPx * drawer.ny) - heightPx / 2;

			drawer.parentOverlayId = overlayId;
			overlay.frame.addView(drawer.frame, lp);
			Log.d(LOG_TAG, "Moved Drawer " + drawerId + " inside Overlay " + overlayId);
		});
	}

	@SuppressLint("ClickableViewAccessibility")
	public static void drawerAddButtonCustom(String drawerId, String btnId, String itemLabel, int bgResId, float textSizeSp, int paddingDp, int spacingDp) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			DrawerData drawer = drawers.get(drawerId);
			if (drawer == null) return;

			Button btn = new Button(ctx);
			btn.setText(itemLabel);
			btn.setTextColor(0xffffffff);
			btn.setAllCaps(false);
			btn.setSingleLine(true);

			int finalPadding = paddingDp > 0 ? paddingDp : drawer.itemPaddingDp;
			btn.setPadding(dp(finalPadding), dp(finalPadding), dp(finalPadding), dp(finalPadding));

			int finalBg = bgResId != 0 ? bgResId : (drawer.itemBgResource != 0 ? drawer.itemBgResource : R.drawable.game_button);
			btn.setBackgroundResource(finalBg);

			if (textSizeSp > 0) {
				btn.setTextSize(android.util.TypedValue.COMPLEX_UNIT_SP, textSizeSp);
			} else if (drawer.itemTextSizePx > 0) {
				btn.setTextSize(android.util.TypedValue.COMPLEX_UNIT_PX, drawer.itemTextSizePx);
			}

			try {
				Typeface tf = Typeface.createFromAsset(ctx.getAssets(), "fonts/megalopolis_extra.otf");
				btn.setTypeface(tf);
			} catch (Exception ignored) {}

			btn.setOnTouchListener((v, e) -> {
				switch (e.getAction()) {
					case MotionEvent.ACTION_DOWN:
						drawer.lastPressedButtonId = btnId;
						break;
					case MotionEvent.ACTION_MOVE:
						if (e.getX() < 0 || e.getX() > v.getWidth() || e.getY() < 0 || e.getY() > v.getHeight()) {
							if (drawer.lastPressedButtonId.equals(btnId)) drawer.lastPressedButtonId = "";
						} else {
							drawer.lastPressedButtonId = btnId;
						}
						break;
					case MotionEvent.ACTION_UP:
					case MotionEvent.ACTION_CANCEL:
						if (drawer.lastPressedButtonId.equals(btnId)) drawer.lastPressedButtonId = "";
						break;
				}
				return false;
			});

			android.widget.LinearLayout.LayoutParams lp = new android.widget.LinearLayout.LayoutParams(
					ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);

			int finalSpacing = spacingDp > 0 ? spacingDp : drawer.itemSpacingDp;
			lp.setMargins(0, 0, 0, dp(finalSpacing));

			drawer.contentLayout.addView(btn, lp);
		});
	}

	public static void setDrawerStyle(String id, int bgColor, float cornerRadiusDp) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			DrawerData data = drawers.get(id);
			if (data == null) return;
			android.graphics.drawable.GradientDrawable bg = new android.graphics.drawable.GradientDrawable();
			bg.setColor(bgColor);
			bg.setCornerRadius(dp(cornerRadiusDp));
			data.frame.setBackground(bg);
		});
	}

	@SuppressLint("ClickableViewAccessibility")
	public static void drawerAddButton(String drawerId, String btnId, String itemLabel) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			DrawerData drawer = drawers.get(drawerId);
			if (drawer == null) return;

			Button btn = new Button(ctx);
			btn.setText(itemLabel);
			btn.setTextColor(0xffffffff);
			btn.setAllCaps(false);
			btn.setSingleLine(true);
			btn.setPadding(dp(12), dp(12), dp(12), dp(12));
			btn.setBackgroundResource(R.drawable.game_button);

			try {
				Typeface tf = Typeface.createFromAsset(ctx.getAssets(), "fonts/megalopolis_extra.otf");
				btn.setTypeface(tf);
			} catch (Exception ignored) {}

			btn.setOnTouchListener((v, e) -> {
				switch (e.getAction()) {
					case MotionEvent.ACTION_DOWN:
						drawer.lastPressedButtonId = btnId;
						break;
					case MotionEvent.ACTION_MOVE:
						if (e.getX() < 0 || e.getX() > v.getWidth() || e.getY() < 0 || e.getY() > v.getHeight()) {
							if (drawer.lastPressedButtonId.equals(btnId)) {
								drawer.lastPressedButtonId = "";
							}
						} else {
							drawer.lastPressedButtonId = btnId;
						}
						break;

					case MotionEvent.ACTION_UP:
					case MotionEvent.ACTION_CANCEL:
						if (drawer.lastPressedButtonId.equals(btnId)) {
							drawer.lastPressedButtonId = "";
						}
						break;
				}
				return false;
			});

			android.widget.LinearLayout.LayoutParams lp = new android.widget.LinearLayout.LayoutParams(
					ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
			lp.setMargins(0, 0, 0, dp(6));

			drawer.contentLayout.addView(btn, lp);
		});
	}

	public static void drawerRemoveAllItems(String drawerId) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			DrawerData drawer = drawers.get(drawerId);
			if (drawer == null) return;
			drawer.lastPressedButtonId = "";
			drawer.contentLayout.removeAllViews();
			Log.d(LOG_TAG, "Cleared all items from drawer: " + drawerId);
		});
	}

	public static String getDrawerPressedItem(String drawerId) {
		DrawerData drawer = drawers.get(drawerId);
		if (drawer == null) return "";
		return drawer.lastPressedButtonId;
	}

	public static void setDrawerHidden(String id, boolean hidden) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			DrawerData data = drawers.get(id);
			if (data == null) return;
			data.frame.setVisibility(hidden ? View.GONE : View.VISIBLE);
		});
	}

	public static void removeDrawer(String id) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null) return;
		ctx.runOnUiThread(() -> {
			DrawerData data = drawers.remove(id);
			if (data != null) {
				root.removeView(data.frame);
			}
		});
	}

	@SuppressLint("ClickableViewAccessibility")
	public static void addButton(
			String id,
			String label,
			float nx,
			float ny,
			float w,
			float h
	) {
		MainActivity ctx = mainActivityRef.get();

		if (ctx == null) return;

		ctx.runOnUiThread(() -> {

			if (buttons.containsKey(id)) return;

			Button btn = new Button(ctx);
			btn.setText(label);
			btn.setTextColor(0xffffffff);
			btn.setVisibility(Button.VISIBLE);
			btn.setAllCaps(false);
			btn.setBackgroundResource(R.drawable.game_button);
			btn.setSingleLine(false);
			btn.setEllipsize(null);
			btn.setMaxLines(Integer.MAX_VALUE);

			try {
				Typeface tf = Typeface.createFromAsset(
						ctx.getAssets(),
						"fonts/megalopolis_extra.otf"
				);
				btn.setTypeface(tf);
			} catch (Exception e) {
				Log.d(LOG_TAG, String.valueOf(e));
			}

			ButtonData data = new ButtonData(btn);

			btn.setOnTouchListener((v, e) -> {
				switch (e.getAction()) {
					case MotionEvent.ACTION_DOWN:
						data.pressed = true;
						break;
					case MotionEvent.ACTION_UP:
					case MotionEvent.ACTION_CANCEL:
						data.pressed = false;
						break;
				}
				return false;
			});

			data.nw = w;
			data.nh = h;
			data.nx = nx;
			data.ny = ny;

			int rw = rootWidth();
			int rh = rootHeight();
			float base = Math.min(rw, rh);
			int widthPx = (int)(base * w);
			int heightPx = (int)(base * h);

			FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(widthPx, heightPx);
			lp.leftMargin = (int)(rw * nx) - (widthPx / 2);
			lp.topMargin = (int)(rh * ny) - (heightPx / 2);

			data.baseTextSize = heightPx * 0.35f;

			btn.setTextSize(
					android.util.TypedValue.COMPLEX_UNIT_PX,
					data.baseTextSize
			);

			ViewGroup root = viewRef.get();

			if (root == null)
				return;

			root.addView(btn, lp);
			buttons.put(id, data);

			Log.d(LOG_TAG, "Added button: " + id);
		});
	}

	@SuppressLint("ClickableViewAccessibility")
	public static void makeMovable(String id, boolean movable) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;

			if (!movable) {
				data.movable = null;
				data.button.setOnTouchListener((v, e) -> {
					switch (e.getAction()) {
						case MotionEvent.ACTION_DOWN:
							data.pressed = true;
							break;
						case MotionEvent.ACTION_UP:
						case MotionEvent.ACTION_CANCEL:
							data.pressed = false;
							break;
					}
					return false;
				});
				return;
			}

			data.movable = new MovableButtonData();

			data.button.setOnTouchListener((v, e) -> {
				Button btn = data.button;
				FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) btn.getLayoutParams();

				float rawX = e.getRawX();
				float rawY = e.getRawY();

				if (data.overlayId != null) {
					OverlayData overlay = overlays.get(data.overlayId);
					if (overlay != null) {
						FrameLayout.LayoutParams flp = (FrameLayout.LayoutParams) overlay.frame.getLayoutParams();
						rawX -= flp.leftMargin;
						rawY -= flp.topMargin;
					}
				}

				switch (e.getAction()) {
					case MotionEvent.ACTION_DOWN: {
						data.pressed = true;
						data.movable.dragging = true;
						data.movable.touchOffsetX = rawX - lp.leftMargin;
						data.movable.touchOffsetY = rawY - lp.topMargin;
						break;
					}
					case MotionEvent.ACTION_MOVE: {
						if (!data.movable.dragging) break;

						int newLeft = (int)(rawX - data.movable.touchOffsetX);
						int newTop  = (int)(rawY - data.movable.touchOffsetY);

						if (data.confined && data.overlayId != null) {
							OverlayData overlay = overlays.get(data.overlayId);
							if (overlay != null) {
								int overlayW = overlay.frame.getWidth();
								int overlayH = overlay.frame.getHeight();
								int btnW = btn.getWidth();
								int btnH = btn.getHeight();

								newLeft = Math.max(0, Math.min(newLeft, overlayW - btnW));
								newTop  = Math.max(0, Math.min(newTop,  overlayH - btnH));
							}
						}

						lp.leftMargin = newLeft;
						lp.topMargin  = newTop;
						btn.setLayoutParams(lp);
						break;
					}
					case MotionEvent.ACTION_UP:
					case MotionEvent.ACTION_CANCEL: {
						data.pressed = false;
						data.movable.dragging = false;
						break;
					}
				}
				return true;
			});
		});
	}

	public static void setButtonConfined(String id, boolean confined) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;
			data.confined = confined;
		});
	}

	public static void setClickable(
			String id,
			int clickable
	) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;
			Button btn = data.button;
			Log.d(LOG_TAG, "Clickable: " + clickable);
			btn.setClickable(clickable == 1);
		});
	}

	public static void setTextFont(
			String id,
			String font
	) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;
			Button btn = data.button;
			try {
				Typeface tf = Typeface.createFromAsset(ctx.getAssets(), "fonts/" + font);
				btn.setTypeface(tf);
			} catch (Exception e) {
				Log.d(LOG_TAG, String.valueOf(e));
			}
		});
	}

	public static void setPosition(String id, float nx, float ny) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null || id == null) return;

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;

			Button btn = data.button;
			FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) btn.getLayoutParams();

			int widthPx;
			int heightPx;

			if (data.overlayId != null) {
				OverlayData overlay = overlays.get(data.overlayId);
				if (overlay != null) {
					int overlayWidthPx  = (int)(rootWidth()  * overlay.nw);
					int overlayHeightPx = (int)(rootHeight() * overlay.nh);
					widthPx  = (int)(overlayWidthPx  * data.nw);
					heightPx = (int)(overlayHeightPx * data.nh);
					lp.leftMargin = (int)(overlayWidthPx  * nx) - widthPx  / 2;
					lp.topMargin  = (int)(overlayHeightPx * ny) - heightPx / 2;
				} else {
					return;
				}
			} else {
				int base = squareBase();
				widthPx = (int)(base * data.nw);
				heightPx = (int)(base * data.nh);

				lp.leftMargin = (int)(rootWidth()  * nx) - widthPx  / 2;
				lp.topMargin  = (int)(rootHeight() * ny) - heightPx / 2;
			}

			lp.width = widthPx;
			lp.height = heightPx;
			btn.setLayoutParams(lp);

			data.baseTextSize = heightPx * 0.35f;
			btn.setTextSize(android.util.TypedValue.COMPLEX_UNIT_PX, data.baseTextSize);
		});
	}

	public static float[] getPosition(String id) {
		ViewGroup root = viewRef.get();
		if (root == null)
			return new float[] { 0.0f, 0.0f };

		ButtonData data = buttons.get(id);
		if (data == null) return new float[] { 0.0f, 0.0f };

		Button btn = data.button;
		FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) btn.getLayoutParams();

		int rw = root.getWidth();
		int rh = root.getHeight();
		if (rw <= 0 || rh <= 0) return new float[] { 0.0f, 0.0f };

		float btnCenterX = lp.leftMargin + (btn.getWidth() / 2.0f);
		float btnCenterY = lp.topMargin + (btn.getHeight() / 2.0f);

		if (data.overlayId != null) {
			OverlayData overlay = overlays.get(data.overlayId);
			if (overlay != null) {
				FrameLayout.LayoutParams overlayLp = (FrameLayout.LayoutParams) overlay.frame.getLayoutParams();
				btnCenterX += overlayLp.leftMargin;
				btnCenterY += overlayLp.topMargin;
			}
		}

		float nx = btnCenterX / rw;
		float ny = btnCenterY / rh;

		return new float[] { nx, ny };
	}

	public static void removeButton(String id) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null)
			return;
		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null)
				return;
			root.removeView(data.button);
			buttons.remove(id);
			Log.d(LOG_TAG, "Removed button: " + id);
		});
	}

	public static void removeAll() {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			for (ButtonData data : buttons.values()) {
				root.removeView(data.button);
			}
			for (OverlayData data : overlays.values()) {
				root.removeView(data.frame);
			}
			for (DrawerData data : drawers.values()) {
				root.removeView(data.frame);
			}
			for (SeekBarData data : seekBars.values()) {
				root.removeView(data.seekBar);
			}
			for (TextInputData data : textInputs.values()) {
				root.removeView(data.editText);
			}
			textInputs.clear();
			seekBars.clear();
			drawers.clear();
			buttons.clear();
			overlays.clear();
			Log.d(LOG_TAG, "Destroyed all buttons.");
		});
	}

	public static void setHidden(String id, boolean hidden) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null || id == null)
			return;

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;
			data.hidden = hidden;
			updateVisibility(data);
		});
	}

	private static void updateVisibility(ButtonData data) {
		if (data == null || data.button == null) return;
		boolean visible = !globallyHidden && !data.hidden;
		data.button.setVisibility(visible ? Button.VISIBLE : Button.GONE);
	}

	public static void setHiddenAll(boolean hidden) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			globallyHidden = hidden;
			for (ButtonData data : buttons.values()) {
				updateVisibility(data);
			}
			for (OverlayData data : overlays.values()) {
				boolean visible = !hidden && !data.hidden;
				data.frame.setVisibility(visible ? View.VISIBLE : View.GONE);
			}
		});
	}

	public static boolean isPressed(String id) {
		ButtonData data = buttons.get(id);
		return data != null && data.pressed;
	}

	public static boolean isDragging(String id) {
		ButtonData data = buttons.get(id);
		if (data == null || data.movable == null) return false;

		return data.movable.dragging;
	}

	public static boolean exists(String id) {
		ButtonData data = buttons.get(id);
		return data != null;
	}

	@SuppressLint("DiscouragedApi")
	public static int getResIdByName(
			Context ctx,
			String resName,
			String type
	) {
		return ctx.getResources().getIdentifier(
				resName,
				type,
				ctx.getPackageName()
		);
	}

	public static void setBackgroundResource(
			String id,
			String resName
	) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null) return;

		int resId = getResIdByName(
				ctx,
				resName,
				"drawable"
		);

		if (resId == 0) {
			Log.e(LOG_TAG, "Invalid drawable resource: " + resName);
			return;
		}

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null)
				return;

			data.button.setBackgroundResource(resId);
		});
	}

	public static void setBackgroundAlpha(String id, int alpha) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null)
			return;
		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;
			data.button.getBackground().setAlpha(alpha);
		});
	}

	public static void setAlpha(String id, int alpha) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null)
			return;
		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;
			data.button.getBackground().setAlpha(alpha);
		});
	}

	public static void setScaling(String id, float scaleX, float scaleY) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null || id == null) return;

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;

			Button btn = data.button;
			FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) btn.getLayoutParams();

			int base = getSquareBase(root);
			int screenW = root.getWidth();
			int screenH = root.getHeight();

			int newWidth  = (int)(base * data.nw * scaleX);
			int newHeight = (int)(base * data.nh * scaleY);

			lp.width  = newWidth;
			lp.height = newHeight;

			float currentNx = (lp.leftMargin + (newWidth / 2.0f)) / screenW;
			float currentNy = (lp.topMargin + (newHeight / 2.0f)) / screenH;

			lp.leftMargin = (int)(screenW * currentNx) - (newWidth / 2);
			lp.topMargin  = (int)(screenH * currentNy) - (newHeight / 2);

			btn.setLayoutParams(lp);
		});
	}

	public static void setDimensions(
			String id,
			float nw,
			float nh
	) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {

			ButtonData data = buttons.get(id);

			if (data == null) return;

			data.nw = nw;
			data.nh = nh;

			Button btn = data.button;

			FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) btn.getLayoutParams();

			int base = getSquareBase(root);

			int widthPx  = (int)(base * nw);
			int heightPx = (int)(base * nh);

			lp.width  = widthPx;
			lp.height = heightPx;
			float[] pos = getPosition(id);

			lp.leftMargin = (int)((root.getWidth() * pos[0]))
					- (widthPx / 2);

			lp.topMargin = (int)((root.getHeight() * pos[1]))
					- (heightPx / 2);

			btn.setLayoutParams(lp);

			// This should work...
			data.baseTextSize = heightPx * 0.35f;

			btn.setTextSize(
					android.util.TypedValue.COMPLEX_UNIT_PX,
					data.baseTextSize
			);
		});
	}

	// Text
	public static void setText(String id, String text) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null)
				return;

			data.button.setText(text);
		});
	}

	public static void setTextSizeSp(String id, float sp) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null) return;
		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;
			data.button.setTextSize(android.util.TypedValue.COMPLEX_UNIT_SP, sp);
		});
	}

	public static void setTextScale(String id, float scale) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null) return;
			data.button.setTextSize(
					android.util.TypedValue.COMPLEX_UNIT_PX,
					data.baseTextSize * scale
			);
		});
	}

	public static void setTextColor(String id, int color) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null)
				return;

			data.button.setTextColor(color);
		});
	}

	public static void setPadding(
			String id,
			int left,
			int top,
			int right,
			int bottom
	) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {

			ButtonData data = buttons.get(id);

			if (data == null)
				return;

			data.button.setPadding(
					left,
					top,
					right,
					bottom
			);
		});
	}

	public static void setAlignment(
			String id,
			int gravity
	) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {

			ButtonData data = buttons.get(id);

			if (data == null)
				return;

			data.button.setGravity(gravity);
		});
	}

	public static void setBackground(String id, int resId) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null)
			return;
		ctx.runOnUiThread(() -> {
			ButtonData data = buttons.get(id);
			if (data == null)
				return;
			if (resId == 0) {
				Log.e("MiniBtnController", "Invalid drawable resource ID");
				return;
			}
			data.button.setBackgroundResource(resId);
		});
	}

	/* Seekbars */

	@SuppressLint("ClickableViewAccessibility")
	public static void addSeekBar(String id, float nx, float ny, float w, float h, int min, int max, int progress) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			if (seekBars.containsKey(id)) return;

			android.widget.SeekBar sb = new android.widget.SeekBar(ctx);
			sb.setProgressDrawable(ctx.getDrawable(R.drawable.game_seekbar));
			sb.setThumb(ctx.getDrawable(R.drawable.game_seekbar_thumb));
			sb.setSplitTrack(false);
			if (android.os.Build.VERSION.SDK_INT >= 26) {
				sb.setMin(min);
			}
			sb.setMax(max);
			sb.setProgress(progress);

			SeekBarData data = new SeekBarData(sb);
			data.nx = nx;
			data.ny = ny;
			data.nw = w;
			data.nh = h;

			sb.setOnSeekBarChangeListener(new android.widget.SeekBar.OnSeekBarChangeListener() {
				@Override
				public void onProgressChanged(android.widget.SeekBar seekBar, int progress, boolean fromUser) {
				}

				@Override
				public void onStartTrackingTouch(android.widget.SeekBar seekBar) {
					data.touching = true;
				}

				@Override
				public void onStopTrackingTouch(android.widget.SeekBar seekBar) {
					data.touching = false;
				}
			});

			int rw = rootWidth();
			int rh = rootHeight();
			float base = Math.min(rw, rh);
			int widthPx = (int) (base * w);
			int heightPx = (int) (base * h);

			FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(widthPx, heightPx);
			lp.leftMargin = (int) (rw * nx) - widthPx / 2;
			lp.topMargin = (int) (rh * ny) - heightPx / 2;

			ViewGroup root = viewRef.get();
			if (root == null) return;

			root.addView(sb, lp);
			seekBars.put(id, data);
			Log.d(LOG_TAG, "Added seekbar: " + id);
		});
	}

	public static void overlayAddSeekBar(String overlayId, String seekBarId) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			OverlayData overlay = overlays.get(overlayId);
			SeekBarData sbData = seekBars.get(seekBarId);

			if (overlay == null || sbData == null)
				return;

			ViewGroup oldParent = (ViewGroup) sbData.seekBar.getParent();
			if (oldParent != null)
				oldParent.removeView(sbData.seekBar);

			int overlayWidthPx  = (int)(rootWidth()  * overlay.nw);
			int overlayHeightPx = (int)(rootHeight() * overlay.nh);

			int widthPx  = (int)(overlayWidthPx  * sbData.nw);
			int heightPx = (int)(overlayHeightPx * sbData.nh);

			FrameLayout.LayoutParams lp =
					new FrameLayout.LayoutParams(widthPx, heightPx);

			lp.leftMargin = (int)(overlayWidthPx  * sbData.nx) - widthPx / 2;
			lp.topMargin  = (int)(overlayHeightPx * sbData.ny) - heightPx / 2;

			sbData.overlayId = overlayId;

			overlay.frame.addView(sbData.seekBar, lp);

			Log.d(LOG_TAG,
					"Moved seekbar " + seekBarId +
							" into overlay " + overlayId);
		});
	}

	public static void setSeekBarProgress(String id, int progress) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			SeekBarData data = seekBars.get(id);
			if (data == null) return;
			data.seekBar.setProgress(progress);
		});
	}

	public static int getSeekBarProgress(String id) {
		SeekBarData data = seekBars.get(id);
		return data != null ? data.seekBar.getProgress() : 0;
	}

	public static void setSeekBarMax(String id, int max) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			SeekBarData data = seekBars.get(id);
			if (data == null) return;
			data.seekBar.setMax(max);
		});
	}

	public static int getSeekBarMax(String id) {
		SeekBarData data = seekBars.get(id);
		return data != null ? data.seekBar.getMax() : 0;
	}

	public static boolean isSeekBarTouching(String id) {
		SeekBarData data = seekBars.get(id);
		return data != null && data.touching;
	}

	public static void setSeekBarHidden(String id, boolean hidden) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			SeekBarData data = seekBars.get(id);
			if (data == null) return;
			data.hidden = hidden;
			boolean visible = !globallyHidden && !hidden;
			data.seekBar.setVisibility(visible ? View.VISIBLE : View.GONE);
		});
	}

	public static void setSeekBarClickable(String id, boolean clickable) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;
		ctx.runOnUiThread(() -> {
			SeekBarData data = seekBars.get(id);
			if (data == null) return;
			data.seekBar.setEnabled(clickable);
		});
	}

	public static void removeSeekBar(String id) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();
		if (ctx == null || root == null) return;
		ctx.runOnUiThread(() -> {
			SeekBarData data = seekBars.get(id);
			if (data == null) return;
			ViewGroup parent = (ViewGroup) data.seekBar.getParent();
			if (parent != null) parent.removeView(data.seekBar);
			seekBars.remove(id);
			Log.d(LOG_TAG, "Removed seekbar: " + id);
		});
	}

	// FUCK ASS INPUT
	public static void addTextInput(String id, float nx, float ny, float w, float h, String hint) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			if (textInputs.containsKey(id)) return;

			EditText edit = new EditText(ctx) {
				@Override
				public void draw(android.graphics.Canvas canvas) {
					try {
						super.draw(canvas);
					} catch (NullPointerException e) {
						// fuck miui crash bro
					}
				}
			};

			edit.setHint(hint);
			edit.setHorizontallyScrolling(false);
			edit.setMaxLines(Integer.MAX_VALUE);

			edit.setInputType(
					InputType.TYPE_CLASS_TEXT
							| InputType.TYPE_TEXT_FLAG_MULTI_LINE
							| InputType.TYPE_TEXT_FLAG_CAP_SENTENCES
			);
			edit.setImeOptions(
					EditorInfo.IME_FLAG_NO_EXTRACT_UI
							| EditorInfo.IME_FLAG_NO_FULLSCREEN
			);
			edit.setOnEditorActionListener((v, actionId, event) -> false);

			edit.setBackgroundResource(R.drawable.game_textinput);
			edit.setOverScrollMode(View.OVER_SCROLL_NEVER);

			edit.setTextColor(Color.WHITE);
			edit.setHintTextColor(0x88FFFFFF);
			edit.setTextSize(TypedValue.COMPLEX_UNIT_SP, 15);
			edit.setPadding(dp(12), dp(10), dp(12), dp(10));
			edit.setGravity(Gravity.TOP | Gravity.START);
			edit.setTypeface(Typeface.MONOSPACE);

			edit.setTextIsSelectable(true);
			edit.setLongClickable(true);
			edit.setFocusable(true);
			edit.setFocusableInTouchMode(true);
			edit.setCursorVisible(true);

			// fys
			edit.setVerticalScrollBarEnabled(true);

			TextInputData data = new TextInputData(edit);
			data.nx = nx;
			data.ny = ny;
			data.nw = w;
			data.nh = h;

			int rw = rootWidth();
			int rh = rootHeight();
			float base = Math.min(rw, rh);

			int widthPx = (int) (base * w);
			int heightPx = (int) (base * h);

			FrameLayout.LayoutParams lp =
					new FrameLayout.LayoutParams(widthPx, heightPx);

			lp.leftMargin = (int) (rw * nx) - widthPx / 2;
			lp.topMargin = (int) (rh * ny) - heightPx / 2;

			ViewGroup root = viewRef.get();
			if (root == null)
				return;

			root.addView(edit, lp);
			textInputs.put(id, data);

			Log.d(LOG_TAG, "Added text input: " + id);
		});
	}

	public static void overlayAddTextInput(String overlayId, String inputId) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			OverlayData overlay = overlays.get(overlayId);
			TextInputData data = textInputs.get(inputId);

			if (overlay == null || data == null)
				return;

			ViewGroup oldParent = (ViewGroup) data.editText.getParent();
			if (oldParent != null)
				oldParent.removeView(data.editText);

			int overlayWidthPx = (int) (rootWidth() * overlay.nw);
			int overlayHeightPx = (int) (rootHeight() * overlay.nh);

			int widthPx = (int) (overlayWidthPx * data.nw);
			int heightPx = (int) (overlayHeightPx * data.nh);

			FrameLayout.LayoutParams lp =
					new FrameLayout.LayoutParams(widthPx, heightPx);

			lp.leftMargin = (int) (overlayWidthPx * data.nx) - widthPx / 2;
			lp.topMargin = (int) (overlayHeightPx * data.ny) - heightPx / 2;

			data.overlayId = overlayId;

			overlay.frame.addView(data.editText, lp);

			Log.d(LOG_TAG,
					"Moved text input " + inputId +
							" into overlay " + overlayId);
		});
	}

	public static void setTextInputText(String id, String text) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			TextInputData data = textInputs.get(id);
			if (data == null) return;

			data.editText.setText(text);
		});
	}

	public static String getTextInputText(String id) {
		TextInputData data = textInputs.get(id);
		return data != null ? data.editText.getText().toString() : "";
	}

	public static void setTextInputHint(String id, String hint) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			TextInputData data = textInputs.get(id);
			if (data == null) return;

			data.editText.setHint(hint);
		});
	}

	public static void setTextInputBackgroundResource(String id, String resource) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			TextInputData data = textInputs.get(id);
			if (data == null) return;

			int res = getResIdByName(ctx, resource, "drawable");
			if (res != 0)
				data.editText.setBackgroundResource(res);
		});
	}

	public static void setTextInputFont(String id, String font) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			TextInputData data = textInputs.get(id);
			if (data == null) return;

			try {
				Typeface tf = Typeface.createFromAsset(
						ctx.getAssets(),
						"fonts/" + font);

				data.editText.setTypeface(tf);
			} catch (Exception e) {
				Log.e(LOG_TAG, "Couldn't load font " + font, e);
			}
		});
	}

	public static void setTextInputHidden(String id, boolean hidden) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			TextInputData data = textInputs.get(id);
			if (data == null) return;

			data.hidden = hidden;

			boolean visible = !globallyHidden && !hidden;
			data.editText.setVisibility(
					visible ? View.VISIBLE : View.GONE);
		});
	}

	public static void removeTextInput(String id) {
		MainActivity ctx = mainActivityRef.get();
		ViewGroup root = viewRef.get();

		if (ctx == null || root == null)
			return;

		ctx.runOnUiThread(() -> {
			TextInputData data = textInputs.get(id);
			if (data == null)
				return;

			ViewGroup parent = (ViewGroup) data.editText.getParent();
			if (parent != null)
				parent.removeView(data.editText);

			textInputs.remove(id);

			Log.d(LOG_TAG, "Removed text input: " + id);
		});
	}

	public static boolean isTextInputFocused(String id) {
		TextInputData data = textInputs.get(id);
		return data != null && data.editText.isFocused();
	}

	public static void setTextInputSelection(String id, int index) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			TextInputData data = textInputs.get(id);
			if (data == null) return;

			int len = data.editText.getText().length();
			data.editText.setSelection(Math.max(0, Math.min(index, len)));
		});
	}

	public static void setTextInputSelection(String id, int start, int end) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			TextInputData data = textInputs.get(id);
			if (data == null) return;
			int len = data.editText.getText().length();

			int s = Math.max(0, Math.min(start, len));
			int e = Math.max(0, Math.min(end, len));

			data.editText.setSelection(s, e);
		});
	}

	public static int[] getTextInputSelection(String id) {
		TextInputData data = textInputs.get(id);

		if (data == null) return new int[]{0, 0};

		return new int[]{
			data.editText.getSelectionStart(), data.editText.getSelectionEnd()
		};
	}
}
