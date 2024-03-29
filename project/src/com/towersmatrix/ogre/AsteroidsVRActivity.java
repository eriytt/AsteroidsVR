package com.towersmatrix.ogre;

import android.app.Activity;
import android.os.Bundle;

import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.WindowManager;

import com.google.vr.ndk.base.AndroidCompat;
import com.google.vr.ndk.base.GvrLayout;
import com.google.vr.ndk.base.GvrUiLayout;


public class AsteroidsVRActivity extends Activity {
    static {
        System.loadLibrary("asteroidsvr");
    }

    private GvrLayout gvrLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        // Ensure fullscreen immersion.
        setImmersiveSticky();
        getWindow()
            .getDecorView()
            .setOnSystemUiVisibilityChangeListener
	    ( new View.OnSystemUiVisibilityChangeListener() {
		    @Override
		    public void onSystemUiVisibilityChange(int visibility) {
			if ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
			    setImmersiveSticky();
			}
		    }
		});

        // Initialize GvrLayout
        gvrLayout = new GvrLayout(this);

        // Add the GLSurfaceView to the GvrLayout.
        GLSurfaceView glSurfaceView = new GLSurfaceView(this);
        glSurfaceView.setEGLContextClientVersion(2);
        glSurfaceView.setEGLConfigChooser(8, 8, 8, 0, 16, 8);
        glSurfaceView.setPreserveEGLContextOnPause(true);
        glSurfaceView.setRenderer(new OgreRenderer(glSurfaceView.getHolder(),
                                                   gvrLayout.getGvrApi().getNativeGvrContext(),
                                                   getResources().getAssets()));
        gvrLayout.setPresentationView(glSurfaceView);

        // Add the GvrLayout to the View hierarchy.
	setContentView(gvrLayout);
	//setContentView(glSurfaceView);
        // Enable scan line racing.
        // if (gvrLayout.setAsyncReprojectionEnabled(true)) {
        //     // Scanline racing decouples the app framerate from the display framerate,
        //     // allowing immersive interaction even at the throttled clockrates set by
        //     // sustained performance mode.
        //     AndroidCompat.setSustainedPerformanceMode(this, true);
        // }

        // Enable VR Mode.
        AndroidCompat.setVrModeEnabled(this, true);
        Log.i("OgreClient", "glSurfaceView dimensions "
              + glSurfaceView.getWidth()
              + "x"
              + glSurfaceView.getHeight());

        // Prevent screen from dimming/locking.
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.i("OgreClient", "onPause");
        gvrLayout.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.i("OgreClient", "onResume");
        gvrLayout.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Destruction order is important; shutting down the GvrLayout will detach
        // the GLSurfaceView and stop the GL thread, allowing safe shutdown of
        // native resources from the UI thread.

        gvrLayout.shutdown();
        Log.i("OgreClient", "onDestroy");
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            setImmersiveSticky();
        }
    }

    private void setImmersiveSticky() {
        getWindow()
            .getDecorView()
            .setSystemUiVisibility(
                                   View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                                   | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                                   | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                                   | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                   | View.SYSTEM_UI_FLAG_FULLSCREEN
                                   | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (Native.HandleKeyDown(keyCode))
            return true;
        else
            return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (Native.HandleKeyUp(keyCode))
            return true;
        else
            return super.onKeyUp(keyCode, event);

    }

    private static float getCenteredAxis(MotionEvent event,
                                         InputDevice device, int axis, int historyPos) {
        final InputDevice.MotionRange range =
            device.getMotionRange(axis, event.getSource());

        // A joystick at rest does not always report an absolute position of
        // (0,0). Use the getFlat() method to determine the range of values
        // bounding the joystick axis center.
        if (range != null) {
            final float flat = range.getFlat();
            final float value =
                historyPos < 0 ? event.getAxisValue(axis):
                event.getHistoricalAxisValue(axis, historyPos);

            // Ignore axis values that are within the 'flat' region of the
            // joystick axis center and normalize the value.
            if (Math.abs(value) > flat) {
                return value / range.getMax();
            }
        }
        return 0;
    }


    private void processJoystickInput(MotionEvent event,
                                      int historyPos) {

        InputDevice inputDevice = event.getDevice();

        float roll = getCenteredAxis(event, inputDevice,
                                     MotionEvent.AXIS_X, historyPos);
        float throttle = getCenteredAxis(event, inputDevice,
                                         MotionEvent.AXIS_Y, historyPos);
        float yaw = getCenteredAxis(event, inputDevice,
                                    MotionEvent.AXIS_Z, historyPos);
        float pitch = getCenteredAxis(event, inputDevice,
                                      MotionEvent.AXIS_RZ, historyPos);
        Native.HandleJoystick(throttle, yaw, pitch, roll);
    }


    @Override
    public boolean dispatchGenericMotionEvent (MotionEvent event) {
        // Check that the event came from a game controller
        if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) ==
                InputDevice.SOURCE_JOYSTICK &&
                event.getAction() == MotionEvent.ACTION_MOVE) {

            // Process the current movement sample in the batch (position -1)
            processJoystickInput(event, -1);
            return true;
        }
        return super.onGenericMotionEvent(event);
    }
}
