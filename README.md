Proton_CM cross platform 2D 3D engine open source
=================
1.

(a.)
Proton CM Open implement Irrlicht Engine, Cocos2dx, Bullet Physics with OpenGL ES1.1 ES2.0

(b.)
Full open source

(c.)
You can build on Win32, MacOS, Android, Ios

(d.)
All platform can play music and sound easily

(e.)
GLSL shading language full control

(f.)
OGLES1/OGLES2 simulator on Win32

2.

Before download take care of carriage return in source files

Preinstall DirectX SDK (June 2010) in Win32, libGLESv2.dll depends on DirectX library

3.
First run need to do

(a)***Win32 or Android***

xxxxxx/media/update_media.bat

(b)***MacOs or Ios***

chmod 777 xxxxxx/media/update_media.sh

xxxxxx/media/update_media.sh

4.
Switch between OGLES1 and OGLES2

(a.) In Android

public AppGLSurfaceView(Context context, SharedActivity _app) in SharedActivity.java

//setEGLContextClientVersion(2); 

-> setEGLContextClientVersion(2);

(b.) In Win32

Ogles1 and ogles2 are using different libEGL.dll.

Please copy correct dlls from shared/dep32 to [projname]/bin.

[projname]/bin/update_OGLES[x].bat will help you do it.

Some texture black in ogles2 is simulator's problem.

5.
When debug NDK c++ on Android, please set

android:installLocation="internalOnly" of AndroidManifest.xml

and Android firmware need unlock s-on to s-off.

6.
Demo screenshots

(a) https://github.com/fatalfeel/proton_cm_open/tree/master/DemoPicture

(b) https://goo.gl/photos/oRBRaH8LALhSZe4m6

7.
Demo video

Irrlicht 3D + Cocos2dx on Ios Android by Mun

https://www.youtube.com/watch?v=BIEafKA2IBM

https://www.youtube.com/watch?v=pfH2ZL9Ze-c

8.
The brother site only implement 3D part

https://github.com/fatalfeel/proton_sdk_source

9.
Any bugs or questions please click github's [Issues] -> [New issue]

Post message, I will be there help you.
