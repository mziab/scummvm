package org.inodes.gus.scummvm;

import android.content.Context;
import android.content.res.AssetManager;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Process;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

import javax.microedition.khronos.opengles.GL;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGL11;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

import java.io.File;
import java.util.concurrent.Semaphore;
import java.util.Map;
import java.util.LinkedHashMap;


// At least in Android 2.1, eglCreateWindowSurface() requires an
// EGLNativeWindowSurface object, which is hidden deep in the bowels
// of libui.  Until EGL is properly exposed, it's probably safer to
// use the Java versions of most EGL functions :(

public class ScummVM implements SurfaceHolder.Callback {
	protected final static String LOG_TAG = "ScummVM";

	private native void create(AssetManager am, AudioTrack audio_track,
								int sample_rate, int buffer_size);

	public ScummVM(Context context) throws Exception {
		int sample_rate = AudioTrack.getNativeOutputSampleRate(
							AudioManager.STREAM_MUSIC);
		int buffer_size = AudioTrack.getMinBufferSize(sample_rate,
							AudioFormat.CHANNEL_CONFIGURATION_STEREO,
							AudioFormat.ENCODING_PCM_16BIT);

		// ~100ms
		int buffer_size_want = (sample_rate * 2 * 2 / 10) & ~1023;

		if (buffer_size < buffer_size_want) {
			Log.w(LOG_TAG, String.format(
				"adjusting audio buffer size (was: %d)", buffer_size));

			buffer_size = buffer_size_want;
		}

		Log.i(LOG_TAG, String.format("Using %d bytes buffer for %dHz audio",
										buffer_size, sample_rate));

		AudioTrack audio_track =
			new AudioTrack(AudioManager.STREAM_MUSIC,
							sample_rate,
							AudioFormat.CHANNEL_CONFIGURATION_STEREO,
							AudioFormat.ENCODING_PCM_16BIT,
							buffer_size,
							AudioTrack.MODE_STREAM);

		if (audio_track.getState() != AudioTrack.STATE_INITIALIZED)
			throw new Exception(
				String.format("Error initialising AudioTrack: %d",
								audio_track.getState()));

		// Init C++ code
		create(context.getAssets(), audio_track, sample_rate, buffer_size);
	}

	private native void nativeDestroy();

	public synchronized void destroy() {
		nativeDestroy();
	}

	protected void finalize() {
		destroy();
	}

	// Surface creation:
	// GUI thread: create surface, release lock
	// ScummVM thread: acquire lock (block), read surface

	// Surface deletion:
	// GUI thread: post event, acquire lock (block), return
	// ScummVM thread: read event, free surface, release lock

	// In other words, ScummVM thread does this:
	//	acquire lock
	//	setup surface
	//	when SCREEN_CHANGED arrives:
	//		destroy surface
	//		release lock
	//	back to acquire lock
	static final int configSpec[] = {
		EGL10.EGL_RED_SIZE, 5,
		EGL10.EGL_GREEN_SIZE, 5,
		EGL10.EGL_BLUE_SIZE, 5,
		EGL10.EGL_DEPTH_SIZE, 0,
		EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
		EGL10.EGL_NONE,
	};

	EGL10 egl;
	EGLDisplay eglDisplay = EGL10.EGL_NO_DISPLAY;
	EGLConfig eglConfig;
	EGLContext eglContext = EGL10.EGL_NO_CONTEXT;
	EGLSurface eglSurface = EGL10.EGL_NO_SURFACE;
	Semaphore surfaceLock = new Semaphore(0, true);
	SurfaceHolder nativeSurface;

	public void surfaceCreated(SurfaceHolder holder) {
		nativeSurface = holder;
		surfaceLock.release();
	}

	public void surfaceChanged(SurfaceHolder holder, int format,
								int width, int height) {
		// Disabled while I debug GL problems
		pushEvent(new Event(Event.EVENT_SCREEN_CHANGED));
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		try {
			surfaceLock.acquire();
		} catch (InterruptedException e) {
			Log.e(LOG_TAG, "Interrupted while waiting for surface lock", e);
		}
	}

	// For debugging
	private static final Map<String, Integer> attribs;

	static {
		attribs = new LinkedHashMap<String, Integer>();
		attribs.put("CONFIG_ID", EGL10.EGL_CONFIG_ID);
		attribs.put("BUFFER_SIZE", EGL10.EGL_BUFFER_SIZE);
		attribs.put("RED_SIZE", EGL10.EGL_RED_SIZE);
		attribs.put("GREEN_SIZE", EGL10.EGL_GREEN_SIZE);
		attribs.put("BLUE_SIZE", EGL10.EGL_BLUE_SIZE);
		attribs.put("ALPHA_SIZE", EGL10.EGL_ALPHA_SIZE);
		//attribs.put("BIND_TO_RGB", EGL10.EGL_BIND_TO_TEXTURE_RGB);
		//attribs.put("BIND_TO_RGBA", EGL10.EGL_BIND_TO_TEXTURE_RGBA);
		attribs.put("CONFIG_CAVEAT", EGL10.EGL_CONFIG_CAVEAT);
		attribs.put("DEPTH_SIZE", EGL10.EGL_DEPTH_SIZE);
		attribs.put("LEVEL", EGL10.EGL_LEVEL);
		attribs.put("MAX_PBUFFER_WIDTH", EGL10.EGL_MAX_PBUFFER_WIDTH);
		attribs.put("MAX_PBUFFER_HEIGHT", EGL10.EGL_MAX_PBUFFER_HEIGHT);
		attribs.put("MAX_PBUFFER_PIXELS", EGL10.EGL_MAX_PBUFFER_PIXELS);
		//attribs.put("MAX_SWAP_INTERVAL", EGL10.EGL_MAX_SWAP_INTERVAL);
		//attribs.put("MIN_SWAP_INTERVAL", EGL10.EGL_MIN_SWAP_INTERVAL);
		attribs.put("NATIVE_RENDERABLE", EGL10.EGL_NATIVE_RENDERABLE);
		attribs.put("NATIVE_VISUAL_ID", EGL10.EGL_NATIVE_VISUAL_ID);
		attribs.put("NATIVE_VISUAL_TYPE", EGL10.EGL_NATIVE_VISUAL_TYPE);
		attribs.put("SAMPLE_BUFFERS", EGL10.EGL_SAMPLE_BUFFERS);
		attribs.put("SAMPLES", EGL10.EGL_SAMPLES);
		attribs.put("STENCIL_SIZE", EGL10.EGL_STENCIL_SIZE);
		attribs.put("SURFACE_TYPE", EGL10.EGL_SURFACE_TYPE);
		attribs.put("TRANSPARENT_TYPE", EGL10.EGL_TRANSPARENT_TYPE);
		attribs.put("TRANSPARENT_RED_VALUE", EGL10.EGL_TRANSPARENT_RED_VALUE);
		attribs.put("TRANSPARENT_GREEN_VALUE", EGL10.EGL_TRANSPARENT_GREEN_VALUE);
		attribs.put("TRANSPARENT_BLUE_VALUE", EGL10.EGL_TRANSPARENT_BLUE_VALUE);
	}

	private void dumpEglConfig(EGLConfig config) {
		int[] value = new int[1];

		for (Map.Entry<String, Integer> entry : attribs.entrySet()) {
			egl.eglGetConfigAttrib(eglDisplay, config,
									entry.getValue(), value);

			if (value[0] == EGL10.EGL_NONE)
				Log.d(LOG_TAG, entry.getKey() + ": NONE");
			else
				Log.d(LOG_TAG, String.format("%s: %d", entry.getKey(), value[0]));
		}
	}

	// Called by ScummVM thread (from initBackend)
	private void createScummVMGLContext() {
		egl = (EGL10)EGLContext.getEGL();
		eglDisplay = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);

		int[] version = new int[2];
		egl.eglInitialize(eglDisplay, version);

		int[] num_config = new int[1];
		egl.eglChooseConfig(eglDisplay, configSpec, null, 0, num_config);

		final int numConfigs = num_config[0];

		if (numConfigs <= 0)
			throw new IllegalArgumentException("No configs match configSpec");

		EGLConfig[] configs = new EGLConfig[numConfigs];
		egl.eglChooseConfig(eglDisplay, configSpec, configs, numConfigs,
							num_config);

		if (false) {
			Log.d(LOG_TAG, String.format("Found %d EGL configurations.", numConfigs));
			for (EGLConfig config : configs)
				dumpEglConfig(config);
		}

		// Android's eglChooseConfig is busted in several versions and
		// devices so we have to filter/rank the configs again ourselves.
		eglConfig = chooseEglConfig(configs);

		if (false) {
			Log.d(LOG_TAG, String.format("Chose EGL config from %d possibilities.", numConfigs));
			dumpEglConfig(eglConfig);
		}

		eglContext = egl.eglCreateContext(eglDisplay, eglConfig,
											EGL10.EGL_NO_CONTEXT, null);

		if (eglContext == EGL10.EGL_NO_CONTEXT)
			throw new RuntimeException("Failed to create context");
	}

	private EGLConfig chooseEglConfig(EGLConfig[] configs) {
		int best = 0;
		int bestScore = -1;
		int[] value = new int[1];

		for (int i = 0; i < configs.length; i++) {
			EGLConfig config = configs[i];
			int score = 10000;

			egl.eglGetConfigAttrib(eglDisplay, config,
									EGL10.EGL_SURFACE_TYPE, value);

			// must have
			if ((value[0] & EGL10.EGL_WINDOW_BIT) == 0)
				continue;

			egl.eglGetConfigAttrib(eglDisplay, config,
									EGL10.EGL_CONFIG_CAVEAT, value);

			if (value[0] != EGL10.EGL_NONE)
				score -= 1000;

			// Must be at least 555, but then smaller is better
			final int[] colorBits = { EGL10.EGL_RED_SIZE,
										EGL10.EGL_GREEN_SIZE,
										EGL10.EGL_BLUE_SIZE,
										EGL10.EGL_ALPHA_SIZE
									};

			for (int component : colorBits) {
				egl.eglGetConfigAttrib(eglDisplay, config, component, value);

				// boost if >5 bits accuracy
				if (value[0] >= 5)
					score += 10;

				// penalize for wasted bits
				score -= value[0];
			}

			egl.eglGetConfigAttrib(eglDisplay, config,
									EGL10.EGL_DEPTH_SIZE, value);

			// penalize for wasted bits
			score -= value[0];

			if (score > bestScore) {
				best = i;
				bestScore = score;
			}
		}

		if (bestScore < 0) {
			Log.e(LOG_TAG, "Unable to find an acceptable EGL config, expect badness.");
			return configs[0];
		}

		return configs[best];
	}

	// Called by ScummVM thread
	static private boolean _log_version = true;

	protected void setupScummVMSurface() {
		try {
			surfaceLock.acquire();
		} catch (InterruptedException e) {
			Log.e(LOG_TAG, "Interrupted while waiting for surface lock", e);
			return;
		}

		eglSurface = egl.eglCreateWindowSurface(eglDisplay, eglConfig,
												nativeSurface, null);

		if (eglSurface == EGL10.EGL_NO_SURFACE)
			Log.e(LOG_TAG, "CreateWindowSurface failed!");

		egl.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

		GL10 gl = (GL10)eglContext.getGL();

		if (_log_version) {
			Log.i(LOG_TAG, String.format("Using EGL %s (%s); GL %s/%s (%s)",
											egl.eglQueryString(eglDisplay, EGL10.EGL_VERSION),
											egl.eglQueryString(eglDisplay, EGL10.EGL_VENDOR),
											gl.glGetString(GL10.GL_VERSION),
											gl.glGetString(GL10.GL_RENDERER),
											gl.glGetString(GL10.GL_VENDOR)));

			// only log this once
			_log_version = false;
		}

		int[] value = new int[1];
		egl.eglQuerySurface(eglDisplay, eglSurface, EGL10.EGL_WIDTH, value);

		int width = value[0];
		egl.eglQuerySurface(eglDisplay, eglSurface, EGL10.EGL_HEIGHT, value);

		int height = value[0];
		Log.i(LOG_TAG, String.format("New surface is %dx%d", width, height));
		setSurfaceSize(width, height);
	}

	// Called by ScummVM thread
	protected void destroyScummVMSurface() {
		if (eglSurface != null) {
			egl.eglMakeCurrent(eglDisplay, EGL10.EGL_NO_SURFACE,
								EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);

			egl.eglDestroySurface(eglDisplay, eglSurface);
			eglSurface = EGL10.EGL_NO_SURFACE;
		}

		surfaceLock.release();
	}

	public void setSurface(SurfaceHolder holder) {
		holder.addCallback(this);
	}

	final public boolean swapBuffers() {
		if (!egl.eglSwapBuffers(eglDisplay, eglSurface)) {
			int error = egl.eglGetError();
			Log.w(LOG_TAG, String.format("eglSwapBuffers exited with error 0x%x", error));
			if (error == EGL11.EGL_CONTEXT_LOST)
				return false;
		}

		return true;
	}

	// Set scummvm config options
	final public native void enableZoning(boolean enable);
	final public native void setSurfaceSize(int width, int height);

	// Feed an event to ScummVM.  Safe to call from other threads.
	final public native void pushEvent(Event e);

	// Runs the actual ScummVM program and returns when it does.
	// This should not be called from multiple threads simultaneously...
	final public native int scummVMMain(String[] argv);

	// Callbacks from C++ peer instance
	//protected GraphicsMode[] getSupportedGraphicsModes() {}
	protected void displayMessageOnOSD(String msg) {}
	protected void setWindowCaption(String caption) {}
	protected void showVirtualKeyboard(boolean enable) {}
	protected String[] getSysArchives() { return new String[0]; }
	protected String[] getPluginDirectories() { return new String[0]; }

	protected void initBackend() {
		createScummVMGLContext();
	}

	public void pause() {
		// TODO: need to pause audio
		// TODO: need to pause engine
	}

	public void resume() {
		// TODO: need to resume audio
		// TODO: need to resume engine
	}

	static {
		// For grabbing with gdb...
		final boolean sleep_for_debugger = false;
		if (sleep_for_debugger) {
			try {
				Thread.sleep(20 * 1000);
			} catch (InterruptedException e) {
			}
		}

		//System.loadLibrary("scummvm");
		File cache_dir = ScummVMApplication.getLastCacheDir();
		String libname = System.mapLibraryName("scummvm");
		File libpath = new File(cache_dir, libname);
		System.load(libpath.getPath());
	}
}

