1.
Proton 3d engine impelement irrlicht 1.9.0 with OGLES1.0, OGLES2.0 and OpenGL
You can build on Win32, MacOS, Android, Ios

2.
Rember first run
On Win32
xxxxxx/media/update_media.bat
&
On MacOs
chmod 777 xxxxxx/media/update_media.sh
xxxxxx/media/update_media.sh

3.
If debug c++ in Android, please set
(a) android:installLocation="internalOnly" of AndroidManifest.xml
(b) the android phone also need unlock s-on to s-off

4.
Enable or disable C_GL_MODE of IrrCompileConfig.h
(a) Win32 and MacOs Enable C_GL_MODE
-- only one of 3 options can choice
#define _IRR_COMPILE_WITH_OGLES1_
#define _IRR_COMPILE_WITH_OGLES2_
#define _IRR_COMPILE_WITH_OPENGL_

(b) Ios and Android Disable C_GL_MODE
-- only one of 2 options can choice
#define _IRR_COMPILE_WITH_OGLES1_
#define _IRR_COMPILE_WITH_OGLES2_

5.
Enable or disable Irrlicht info bar and gui
Find _IRR_COMPILE_WITH_GUI_ of IrrCompileConfig.h
Enable or disable this define.

6.
Switch between OGLES1 and OGLES2 on Android
public AppGLSurfaceView(Context context, SharedActivity _app) in SharedActivity.java
//setEGLContextClientVersion(2); -> setEGLContextClientVersion(2);

7.
Win32 OGLES1 and OGLES2 are using different libEGL.dll
please run update_OGLES1.bat or update_OGLES2.bat
the .bat file will copy correct libEGL.dll

8. AngleProject is a win32 OGLES2 simulator
git clone https://chromium.googlesource.com/angle/angle
Revision: cc4ec64cda54f4b138f8d16ce0fe40b8fcedb459
Date: 2013/9/24 Am 02:57:10

Preinstall VS2010 and DirectX SDK (June 2010)
Set DirectX SDK include and library to VS2010
Set /arch:SSE(not SSE2) to enable enhanced instruction set

9. AngleProject bug fixed
//edit ~/src/libEGL/Surface.cpp
bool Surface::resizeSwapChain(int backbufferWidth, int backbufferHeight)
{
    ASSERT(backbufferWidth >= 0 && backbufferHeight >= 0);
    ASSERT(mSwapChain);

    EGLint status = mSwapChain->resize(backbufferWidth, backbufferHeight);
	
    //by stone
    mWidth = backbufferWidth;
    mHeight = backbufferHeight;

    if (status == EGL_CONTEXT_LOST)
    {
        mDisplay->notifyDeviceLost();
        return false;
    }
    else if (status != EGL_SUCCESS)
    {
        return error(status, false);
    }

    //mWidth = backbufferWidth;
    //mHeight = backbufferHeight;

    return true;
}

10. AngleProject bug fixed download
http://www.mediafire.com/file/n81r28nym4mqjhn/AngleGLes2.rar
