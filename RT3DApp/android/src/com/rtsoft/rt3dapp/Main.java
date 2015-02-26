//This is the only app-specific java file for the android project.  The real work is done in
//shared/android/v2_src/java.  When editing that, be careful, as it is used by all Proton projects.

//Thanks to Phil Hassey for his help and code

//Defines that help control the preprocessing step:

//************* EDIT THESE DEFINES ****************
//Because I'm using javapp preprocesser processing, //# means the command IS being used.  To comment out a preprocessor
//command, do // #command  (ie, insert a space after the //)

// #define RT_HOOKED_SUPPORT
// #define RT_TAPJOY_SUPPORT
// #define RT_CHARTBOOST_SUPPORT
// #define RT_FLURRY_SUPPORT

//**********************************************

package com.rtsoft.rt3dapp;
import com.rtsoft.rt3dapp.SharedActivity;
import android.os.Bundle;





public class Main extends SharedActivity
{
	@Override
    protected void onCreate(Bundle savedInstanceState) 
	{
		//*************** EDIT THESE TO MATCH YOUR PROJECT STUFF ****************
		dllname= "rt3dapp";
		BASE64_PUBLIC_KEY = "public key from android market, if used, set securityEnabled to true below";
		securityEnabled = false; 
   		IAPEnabled = false;

		//********************* You probably don't need to change anything below this **************
		
   		
   		HookedEnabled = false;
   		
   		PackageName= "com.rtsoft.rt3dapp"; //edit build.xml to change this setting, not here
	
		System.loadLibrary(dllname);
		
		
		super.onCreate(savedInstanceState);
		if (HookedEnabled)
		{
			
		}
    }
}
	
