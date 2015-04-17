Proton_CM cross platform 3D engine open source
=================
1.

(a.)
Proton CM Open implement Irrlicht and Bullet Physics with OpenGL ES1.1 ES2.0

(b.)
Full open source

(c.)
You can build on Win32, MacOS, Android, Ios

(d.)
All platform can play music and sound easily

(e.)
GLSL shading language full control

2.

This Project is Proton CM Open + Cocos2D on OGLES1.1 OGLES2.0

3.

First run need to do

On Win32 or Android --->

xxxxxx/media/update_media.bat

On MacOs or Ios --->

chmod 777 xxxxxx/media/update_media.sh

xxxxxx/media/update_media.sh

4.

Switch between OGLES1 and OGLES2

(a.) Find define of IrrCompileConfig.h

_IRR_COMPILE_WITH_OGLES1_;

->_IRR_COMPILE_WITH_OGLES2_;

(b.) In Android

Find function in SharedActivity.java

//setEGLContextClientVersion(2); 

-> setEGLContextClientVersion(2);

(c.) In Win32

Because ogles1 and ogles2 use different libEGL.dll.

select correct dlls of ogles-1/2 on RT3DApp/bin,

RT3DApp/bin/update_OGLES1/2.bat will help you do it.

When meet some texture black in win32, its libGLESv2.dll simulator problem.

5.

When debug NDK c++ on Android, please set

android:installLocation="internalOnly" of AndroidManifest.xml

6.
Demo screenshots

https://plus.google.com/photos/106185541018774360364/albums/6128347518156528913

7.

The Brother site is https://github.com/fatalfeel/proton_sdk_source