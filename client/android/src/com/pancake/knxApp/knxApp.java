package com.pancake.knxApp;

import org.libsdl.app.SDLActivity;
import android.content.res.AssetManager;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

public class knxApp extends SDLActivity {

    // load libs
    static
     {
        System.loadLibrary("sqlitenative");
     }
    
    public static AssetManager mAssetMgr;
    
    // Setup
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        Log.v("SDL", "onCreate()");
        super.onCreate(savedInstanceState);
        //mio
        mAssetMgr = getAssets();
    }
    
}
