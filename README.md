Proton_CM cross platform 2D 3D engine open source
=================
1.

(a.)
Proton CM Open implement Irrlicht, Cocos2dx, Bullet Physics with OpenGL ES1.1 ES2.0

(b.)
Full open source

(c.)
You can build on Win32, MacOS, Android, Ios

(d.)
All platform can play music and sound easily

(e.)
GLSL shading language full control

2.
First run need to do

On Win32 or Android --->

xxxxxx/media/update_media.bat

On MacOs or Ios --->

chmod 777 xxxxxx/media/update_media.sh

xxxxxx/media/update_media.sh

3.
Switch between OGLES1 and OGLES2

(a.) Find define of IrrCompileConfig.h

_IRR_COMPILE_WITH_OGLES1_;

->_IRR_COMPILE_WITH_OGLES2_;

(b.) In Android Java

public AppGLSurfaceView(Context context, SharedActivity _app) in SharedActivity.java

//setEGLContextClientVersion(2); 

-> setEGLContextClientVersion(2);

(c.) In Win32

Because ogles1 and ogles2 use different libEGL.dll.

copy correct dlls from shared/dep32 to [projname]/bin,

[projname]/bin/update_OGLES[X].bat will help you do it.

Some texture mapping black in win32 is ogles2 simulator's problem.

4.
When debug NDK c++ on Android, please set

android:installLocation="internalOnly" of AndroidManifest.xml

and Android firmware need unlock s-on to s-off.

5.
Enable or disable Irrlicht debug info and gui

Find define _IRR_COMPILE_WITH_GUI_ of IrrCompileConfig.h

Enable or disable this define.

6.
Demo screenshots

(a) https://github.com/fatalfeel/proton_cm_open/tree/master/DemoPicture

(b) https://plus.google.com/photos/106185541018774360364/albums/6128347518156528913

7.
Demo video

Irrlicht 3D + Cocos2D on IOS by Mun

https://www.youtube.com/watch?v=BIEafKA2IBM

8.
The brother site only implement 3D part

https://github.com/fatalfeel/proton_sdk_source

9.
Any bugs or questions please search

[Proton SDK] of http://irrlicht.sourceforge.net/forum/

Post message, I will be there help you.
