package net.itsjustsomedude.swrdg;

import android.annotation.SuppressLint;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;

import java.lang.ref.WeakReference;
import java.util.HashMap;

@SuppressLint("unused")
public class ButtonController {
	private static final String LOG_TAG = "MiniBtnController";
	private static WeakReference<MainActivity> mainActivityRef;
	private static WeakReference<ViewGroup> viewRef;

	private static class KiwiButton {
		Button btn;
		boolean pressed;
	}

	private static final HashMap<String, KiwiButton> datas = new HashMap<>();

	public static void init(MainActivity act, ViewGroup view) {
		mainActivityRef = new WeakReference<>(act);
		viewRef = new WeakReference<>(view);
	}

	@SuppressLint("ClickableViewAccessibility")
	public static void NewButton(String id, String text, float nx, float ny, float nw, float nh) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		ctx.runOnUiThread(() -> {
			DisplayMetrics dm = ctx.getResources().getDisplayMetrics();

			int screenW = dm.widthPixels;
			int screenH = dm.heightPixels;

			int widthPx = Math.round(nw * screenW);
			int heightPx = Math.round(nh * screenH);

			// nx, ny is the CENTER of the button — so shift back by half width/height
			int leftPx = Math.round(nx * screenW - widthPx / 2f);
			int topPx = Math.round(ny * screenH - heightPx / 2f);

			Button btn = new Button(ctx);
			btn.setText(text);
			btn.setVisibility(View.VISIBLE);
			btn.setBackgroundResource(R.drawable.game_button);
			btn.setTextColor(0xffffffff);

			FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
				widthPx,
				heightPx,
				Gravity.TOP | Gravity.START
			);
			layoutParams.leftMargin = leftPx;
			layoutParams.topMargin = topPx;

			ctx.addContentView(btn, layoutParams);

			KiwiButton d = new KiwiButton();
			d.btn = btn;
			d.pressed = false;
			datas.put(id, d);

			btn.setOnTouchListener(new View.OnTouchListener() {
				@Override
				public boolean onTouch(View v, MotionEvent event) {
					switch (event.getAction()) {
						case MotionEvent.ACTION_DOWN:
							d.pressed = true;
							break;
						case MotionEvent.ACTION_UP:
						case MotionEvent.ACTION_CANCEL:
							d.pressed = false;
							break;
					}
					return false; // Allows onClickListener to fire
				}
			});

			Log.d(LOG_TAG, "Added button at (" + leftPx + "," + topPx + ") size " + widthPx + "x" + heightPx);
		});
	}

	public static boolean b_isPressed(String id) {
		MainActivity ctx = mainActivityRef.get();
		KiwiButton d = datas.get(id);
		if (d == null) return false;
		return d.pressed;
	}

	public static void b_destroy(String id) {
		MainActivity ctx = mainActivityRef.get();
		KiwiButton d = datas.get(id);
		if (d == null) return;
		ctx.runOnUiThread(() -> {
			Button btn = d.btn;
			viewRef.get().removeView(btn);
		});
	}

	/* Text Functions */

	public static void b_setText(String id, String text) {
		MainActivity ctx = mainActivityRef.get();
		KiwiButton d = datas.get(id);
		if (d == null) return;
		ctx.runOnUiThread(() -> {
			Button btn = d.btn;
			btn.setText(text);
		});
	}
}
