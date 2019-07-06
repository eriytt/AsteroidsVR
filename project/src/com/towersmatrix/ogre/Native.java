package com.towersmatrix.ogre;

import android.content.res.AssetManager;
import android.view.Surface;

public class Native
{
    public native static long InitOgre(Surface surface, long nativeGvrContext, AssetManager assetManager);
    public native static void Render();
    public native static boolean HandleKeyDown(int code);
    public native static boolean HandleKeyUp(int code);
    public native static void HandleJoystick(float throttle, float yaw, float pitch, float roll);
}
