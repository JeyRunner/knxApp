package com.pancake.knxApp;

import org.libsdl.app.SDLActivity;
import android.content.res.AssetManager;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

public class knxApp extends SDLActivity {

    // load libs
    static {
        System.loadLibrary("sqlite3");
        }
    
    public static AssetManager mAssetMgr;
    //public static Context context;


    // Setup
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        Log.i("SDL", "onCreate()");
        super.onCreate(savedInstanceState);
        //mio
        mAssetMgr = getAssets();
        Log.i("SDL/APP", getApplicationContext().getDatabasePath("home.client.db").getPath());
        Log.i("SDL/APP", "files dir: " + getFilesDir());
        Log.i("SDL/APP", "/data/data/" + getApplicationContext().getPackageName() + "/databases/home.client.db");
        //context = getApplicationContext();
    }
    
}
