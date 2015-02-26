Cls
call app_info_setup.bat

REM arm-none-linux-gnueabi-g++ -o tiltodemo ..\common\accelerometer.cpp ..\common\collision.cpp ..\common\gamelogic.cpp ..\common\geometry.cpp ..\common\graphics_ogl.cpp ..\common\graphics_sdl.cpp ..\common\main.cpp ..\common\sdl_init.cpp ..\common\sound.cpp "-I%PalmPDK%\include" "-I%PalmPDK%\include\SDL" "-L%PalmPDK%\device\lib" -Wl,--allow-shlib-undefined -lSDL -lSDL_net -lSDL_image -lSDL_mixer -lpdl -lGLES_CM

REM @echo off
@rem Set the device you want to build for to 1
@rem Use Pixi to allow running on either device
set PRE=0
set PIXI=1
set DEBUG=0

set SHARED=..\..\shared
set APP=..\source
set CLANMATH=..\..\shared\ClanLib-2.0\Sources\Core\Math
set ZLIBPATH=..\..\shared\util\zlib
set PPATH=..\..\shared\Renderer\linearparticle\sources
set COMPPATH=..\..\shared\Entity

REM I really can't figure out how to make it accept a source file search path.. grr.  So I'll prepend for each.  If you know how tell me! -Seth
@rem List your source files here


REM **************************************** ENGINE SOURCE CODE FILES
set SRC= %SHARED%\PlatformSetup.cpp  %SHARED%\WebOS\SDLMain.cpp %SHARED%\win\app\main.cpp %SHARED%\webOS\WebOSUtils.cpp ^
%SHARED%\Audio\AudioManager.cpp %CLANMATH%\angle.cpp %CLANMATH%\mat3.cpp %CLANMATH%\mat4.cpp %CLANMATH%\rect.cpp %CLANMATH%\vec2.cpp %CLANMATH%\vec3.cpp ^
%CLANMATH%\vec4.cpp %SHARED%\Entity\Entity.cpp %SHARED%\Entity\Component.cpp %SHARED%\GUI\RTFont.cpp %SHARED%\Manager\Console.cpp ^
%SHARED%\Manager\GameTimer.cpp %SHARED%\Manager\MessageManager.cpp %SHARED%\Manager\ResourceManager.cpp %SHARED%\Manager\VariantDB.cpp %SHARED%\Math\rtPlane.cpp ^
%SHARED%\Math\rtRect.cpp %SHARED%\Renderer\RenderBatcher.cpp %SHARED%\Renderer\SoftSurface.cpp %SHARED%\Renderer\Surface.cpp %SHARED%\Renderer\SurfaceAnim.cpp ^
%SHARED%\util\CRandom.cpp %SHARED%\util\GLESUtils.cpp %SHARED%\util\MathUtils.cpp %SHARED%\util\MiscUtils.cpp %SHARED%\util\RenderUtils.cpp %SHARED%\util\ResourceUtils.cpp ^
%SHARED%\util\Variant.cpp %SHARED%\util\boost\libs\signals\src\connection.cpp %SHARED%\util\boost\libs\signals\src\named_slot_map.cpp %SHARED%\util\boost\libs\signals\src\signal_base.cpp ^
%SHARED%\util\boost\libs\signals\src\slot.cpp %SHARED%\util\boost\libs\signals\src\trackable.cpp %SHARED%\BaseApp.cpp %SHARED%\util\TextScanner.cpp %SHARED%\Entity\EntityUtils.cpp ^
%SHARED%\Network\NetHTTP.cpp %SHARED%\Network\NetSocket.cpp %SHARED%\Network\NetUtils.cpp %SHARED%\Audio\AudioManagerSDL.cpp %SHARED%\util\unzip\unzip.c %SHARED%\util\unzip\ioapi.c ^
%SHARED%\FileSystem\StreamingInstance.cpp %SHARED%\FileSystem\StreamingInstanceZip.cpp %SHARED%\FileSystem\StreamingInstanceFile.cpp %SHARED%\FileSystem\FileSystem.cpp ^
%SHARED%\FileSystem\FileSystemZip.cpp %SHARED%\FileSystem\FileManager.cpp %SHARED%\Irrlicht\IrrlichtManager.cpp

REM **************************************** ENGINE COMPONENT SOURCE CODE FILES
set COMPONENT_SRC=%COMPPATH%\Button2DComponent.cpp %COMPPATH%\FilterInputComponent.cpp %COMPPATH%\FocusInputComponent.cpp %COMPPATH%\FocusRenderComponent.cpp %COMPPATH%\FocusUpdateComponent.cpp ^
%COMPPATH%\HTTPComponent.cpp %COMPPATH%\InputTextRenderComponent.cpp %COMPPATH%\InterpolateComponent.cpp %COMPPATH%\OverlayRenderComponent.cpp %COMPPATH%\ProgressBarComponent.cpp ^
%COMPPATH%\RectRenderComponent.cpp %COMPPATH%\ScrollBarRenderComponent.cpp %COMPPATH%\ScrollComponent.cpp %COMPPATH%\TapSequenceDetectComponent.cpp %COMPPATH%\TextBoxRenderComponent.cpp ^
%COMPPATH%\TextRenderComponent.cpp %COMPPATH%\TouchStripComponent.cpp %COMPPATH%\TrailRenderComponent.cpp %COMPPATH%\TyperComponent.cpp %COMPPATH%\UnderlineRenderComponent.cpp ^
%COMPPATH%\TouchHandlerComponent.cpp %COMPPATH%\CustomInputComponent.cpp %COMPPATH%\SelectButtonWithCustomInputComponent.cpp %COMPPATH%\SliderComponent.cpp %COMPPATH%\EmitVirtualKeyComponent.cpp ^
%COMPPATH%\RenderScissorComponent.cpp


REM **************************************** ZLIB SOURCE CODE FILES
set ZLIB_SRC=%ZLIBPATH%/deflate.c %ZLIBPATH%/gzio.c %ZLIBPATH%/infback.c %ZLIBPATH%/inffast.c %ZLIBPATH%/inflate.c %ZLIBPATH%/inftrees.c %ZLIBPATH%/trees.c %ZLIBPATH%/uncompr.c %ZLIBPATH%/zutil.c %ZLIBPATH%/adler32.c %ZLIBPATH%/compress.c %ZLIBPATH%/crc32.c


REM **************************************** PARTICLE SYSTEM SOURCE CODE FILES
set PARTICLE_SRC=%PPATH%/L_Defination.cpp %PPATH%/L_DroppingEffect.cpp %PPATH%/L_EffectEmitter.cpp %PPATH%/L_ExplosionEffect.cpp %PPATH%/L_MotionController.cpp %PPATH%/L_Particle.cpp ^
%PPATH%/L_ParticleEffect.cpp %PPATH%/L_ParticleMem.cpp %PPATH%/L_ParticleSystem.cpp %PPATH%/L_ShootingEffect.cpp %PPATH%/L_EffectManager.cpp


REM **************************************** APP SOURCE CODE FILES
set APP_SRC=^
..\source\GUI\DebugMenu.cpp ^
..\source\GUI\MainMenu.cpp ^
..\source\GUI\Map3Menu.cpp ^
..\source\GUI\MapMenu.cpp ^
..\source\GUI\Mesh3DMenu.cpp ^
..\source\GUI\TerrainMenu.cpp ^
..\source\Component\FPSControlComponent.cpp ^
..\source\App.cpp

REM **************************************** END SOURCE


set IRRLICHT_SRC=^
..\..\shared\Irrlicht\source\Irrlicht\Irrlicht.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\COGLESDriver.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\COGLESExtensionHandler.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\COGLESTexture.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CNullDriver.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\C3DSMeshFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CAnimatedMeshMD2.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CAnimatedMeshMD3.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CB3DMeshFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CBSPMeshFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CColladaFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CCSMLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMD2MeshFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMD3MeshFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMS3DMeshFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMY3DMeshFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\COBJMeshFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CQ3LevelMesh.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CQuake3ShaderSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CXMeshFileLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleAnimatedMeshSceneNodeEmitter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleAttractionAffector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleBoxEmitter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleCylinderEmitter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleFadeOutAffector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleGravityAffector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleMeshEmitter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticlePointEmitter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleRingEmitter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleRotationAffector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleScaleAffector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleSphereEmitter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CParticleSystemSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CIrrDeviceIPhone.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CIrrDeviceStub.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CAnimatedMeshSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CBillboardSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CCubeSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneCollisionManager.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneManager.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneNodeAnimatorCameraMaya.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneNodeAnimatorCollisionResponse.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneNodeAnimatorDelete.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneNodeAnimatorFlyCircle.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneNodeAnimatorFlyStraight.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneNodeAnimatorFollowSpline.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneNodeAnimatorRotation.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneNodeAnimatorTexture.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CShadowVolumeSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CTerrainSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CTerrainTriangleSelector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CCameraSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSceneNodeAnimatorCameraFPS.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSkyBoxSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSkyDomeSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSphereSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CImage.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CImageLoaderJPG.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CImageLoaderPNG.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CImageLoaderRGB.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CImageLoaderBMP.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CImageLoaderRTTEX.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CImageLoaderTGA.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CAttributes.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CBoneSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CColorConverter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CDefaultSceneNodeAnimatorFactory.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CDefaultSceneNodeFactory.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CDepthBuffer.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CDummyTransformationSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CEmptySceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CFPSCounter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CGeometryCreator.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CLightSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CLogger.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMemoryFile.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMeshCache.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMeshManipulator.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMeshSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMetaTriangleSelector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\COCTLoader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\COctreeSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\COctreeTriangleSelector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CSkinnedMesh.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CTextSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CTriangleBBSelector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CTriangleSelector.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CVideoModeList.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CVolumeLightSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CWaterSurfaceSceneNode.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\Irrlicht.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\os.cpp


set LIBJPEG=^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcapimin.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcapistd.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jccoefct.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jccolor.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcdctmgr.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jchuff.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcinit.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcmainct.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcmarker.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcmaster.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcomapi.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcparam.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcphuff.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcprepct.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jcsample.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jctrans.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdapimin.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdapistd.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdatadst.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdatasrc.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdcoefct.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdcolor.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jddctmgr.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdhuff.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdinput.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdmainct.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdmarker.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdmaster.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdmerge.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdphuff.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdpostct.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdsample.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jdtrans.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jerror.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jfdctflt.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jfdctfst.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jfdctint.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jidctflt.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jidctfst.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jidctint.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jidctred.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jmemmgr.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jmemnobs.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jquant1.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jquant2.c ^
..\..\shared\Irrlicht\source\Irrlicht\jpeglib\jutils.c

set LIBPNG=^
..\..\shared\Irrlicht\source\Irrlicht\libpng\png.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngerror.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pnggccrd.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngget.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngmem.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngpread.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngread.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngrio.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngrtran.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngrutil.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngset.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngtrans.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngvcrd.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngwio.c ^
..\..\shared\Irrlicht\source\Irrlicht\libpng\pngwtran.c

set SRC_FILESYSTEM=^
..\..\shared\Irrlicht\source\Irrlicht\CFileList.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CFileSystem.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CLimitReadFile.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CMountPointReader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\COSOperator.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CPakReader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CProtonReader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CReadFile.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CWriteFile.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CXMLReader.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CXMLWriter.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CZBuffer.cpp ^
..\..\shared\Irrlicht\source\Irrlicht\CZipReader.cpp


set DEFINES=-D RT_WEBOS -D BOOST_ALL_NO_LIB  -D RT_WEBOS_ARM -D NDEBUG -D_IRR_STATIC_LIB_


@rem List the libraries needed
set LIBS=-lSDL -lSDL_net -lSDL_image -lSDL_mixer -lpdl -lGLES_CM

@rem Name your output executable
set OUTFILE=%APPNAME%

set INCLUDE_DIRS=-I..\..\shared\util\boost -I%SHARED% -I%APP% "-I%PALMPDK%\include" "-I%PALMPDK%\include\SDL" -I..\..\shared\ClanLib-2.0\Sources -I%ZLIBPATH% -I..\..\shared\Irrlicht\include

set LIB_DIRS="-L%PALMPDK%\device\lib"

set FLAGS=-O2

if %PRE% equ 0 if %PIXI% equ 0 goto :END

if %DEBUG% equ 1 (
   set DEVICEOPTS=-g
) else (
   set DEVICEOPTS=
)

if %PRE% equ 1 (
   set DEVICEOPTS=%DEVICEOPTS% -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
)

if %PIXI% equ 1 (
   set DEVICEOPTS=%DEVICEOPTS% -mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp
)

echo %DEVICEOPTS%
del *.o
echo Building the .o files first, as we have too many files to do it all on one line
arm-none-linux-gnueabi-gcc -c %DEFINES% %DEVICEOPTS% %INCLUDE_DIRS% %LIB_DIRS% %APP_SRC% %SRC% %COMPONENT_SRC% %ZLIB_SRC% -Wl,--allow-shlib-undefined %LIBS% -Wno-deprecated %FLAGS%

arm-none-linux-gnueabi-gcc -c %DEFINES% %DEVICEOPTS% %INCLUDE_DIRS% %LIB_DIRS% %PARTICLE_SRC% -Wl,--allow-shlib-undefined %LIBS% -Wno-deprecated %FLAGS%

arm-none-linux-gnueabi-gcc -c %DEFINES% %DEVICEOPTS% %INCLUDE_DIRS% %LIB_DIRS% %IRRLICHT_SRC% -Wl,--allow-shlib-undefined %LIBS% -Wno-deprecated %FLAGS%

arm-none-linux-gnueabi-gcc -c %DEFINES% %DEVICEOPTS% %INCLUDE_DIRS% %LIB_DIRS% %LIBJPEG% -Wl,--allow-shlib-undefined %LIBS% -Wno-deprecated %FLAGS%

arm-none-linux-gnueabi-gcc -c %DEFINES% %DEVICEOPTS% %INCLUDE_DIRS% %LIB_DIRS% %LIBPNG% -Wl,--allow-shlib-undefined %LIBS% -Wno-deprecated %FLAGS%

arm-none-linux-gnueabi-gcc -c %DEFINES% %DEVICEOPTS% %INCLUDE_DIRS% %LIB_DIRS% %SRC_FILESYSTEM% -Wl,--allow-shlib-undefined %LIBS% -Wno-deprecated %FLAGS%

echo Now combine them into the final binary
arm-none-linux-gnueabi-gcc %DEFINES% %DEVICEOPTS% %INCLUDE_DIRS% %LIB_DIRS% -o %OUTFILE% *.o -Wl,--allow-shlib-undefined %LIBS% -Wno-deprecated %FLAGS%
del *.o
if not exist %APPNAME% beeper.exe /p

goto :EOF

:END
echo Please select the target device by editing the PRE/PIXI variable in this file.

REM exit /b 1





