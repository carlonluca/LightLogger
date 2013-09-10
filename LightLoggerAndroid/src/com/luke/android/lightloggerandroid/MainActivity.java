package com.luke.android.lightloggerandroid;

import android.os.Bundle;
import android.app.Activity;

public class MainActivity extends Activity {
	static {
		System.loadLibrary("gnustl_shared");
		System.loadLibrary("LightLogger");
	}

	/*
	 * (non-Javadoc)
	 * @see android.app.Activity#onCreate(android.os.Bundle)
	 */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        log();
    }
    
    private native void log();
}
