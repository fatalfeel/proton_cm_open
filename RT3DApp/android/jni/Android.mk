LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := rt3dapp
SHARED := ../../../shared
APP := ../../source
LOCAL_ARM_MODE := arm

ENTITYSRC := $(SHARED)/Entity
CLANCORE := $(SHARED)/ClanLib-2.0/Sources/Core
ZLIBPATH := $(SHARED)/util/zlib
IRRSRC :=  $(SHARED)/Irrlicht/source
IRRMESH := $(IRRSRC)
IRRPARTICLE := $(IRRSRC)
IRRSCENE := $(IRRSRC)
PNGSRC :=  $(SHARED)/Irrlicht/source/libpng
JPGSRC :=  $(SHARED)/Irrlicht/source/jpeglib
LZMASRC :=  $(SHARED)/Irrlicht/source/lzma

#LOCAL_CPP_FEATURES += exceptions
LOCAL_CPP_FEATURES += rtti

#release flags
#LOCAL_CFLAGS := -DANDROID_NDK -DBUILD_ANDROID -DGC_BUILD_ANDROID -DNDEBUG -D_IRR_STATIC_LIB_
#LOCAL_CPPFLAGS := -DGC_BUILD_C -DANDROID_NDK -DBUILD_ANDROID -DNDEBUG -D_IRR_STATIC_LIB_
#debug flags
#LOCAL_CFLAGS := -DANDROID_NDK -DBUILD_ANDROID -DGC_BUILD_ANDROID -D_DEBUG -D_IRR_STATIC_LIB_
#LOCAL_CPPFLAGS := -DGC_BUILD_C -DANDROID_NDK -DBUILD_ANDROID -D_DEBUG -D_IRR_STATIC_LIB_

APP_DEBUG := $(strip $(NDK_DEBUG))
ifeq ($(APP_DEBUG),0)
	LOCAL_CFLAGS := -DANDROID_NDK -DBUILD_ANDROID -DGC_BUILD_ANDROID -DNDEBUG -D_IRR_STATIC_LIB_ -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
	LOCAL_CPPFLAGS := -DGC_BUILD_C -DANDROID_NDK -DBUILD_ANDROID -DNDEBUG -D_IRR_STATIC_LIB_ -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
else
	LOCAL_CFLAGS := -DANDROID_NDK -DBUILD_ANDROID -DGC_BUILD_ANDROID -D_DEBUG -D_IRR_STATIC_LIB_ -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
	LOCAL_CPPFLAGS := -DGC_BUILD_C -DANDROID_NDK -DBUILD_ANDROID -D_DEBUG -D_IRR_STATIC_LIB_ -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SHARED) $(LOCAL_PATH)/$(SHARED)/Irrlicht/include \
 $(LOCAL_PATH)/$(APP) $(LOCAL_PATH)/$(SHARED)/ClanLib-2.0/Sources $(LOCAL_PATH)/$(SHARED)/util/boost
                

LOCAL_SRC_FILES := \
$(SHARED)/PlatformSetup.cpp $(SHARED)/android/AndroidUtils.cpp ../temp_final_cpp_src/AndroidApp.cpp \
$(SHARED)/Audio/AudioManager.cpp $(SHARED)/Audio/AudioManagerAndroid.cpp \
$(CLANCORE)/core_global.cpp \
$(CLANCORE)/Math/angle.cpp $(CLANCORE)/Math/mat3.cpp $(CLANCORE)/Math/mat4.cpp $(CLANCORE)/Math/rect.cpp $(CLANCORE)/Math/vec2.cpp $(CLANCORE)/Math/vec3.cpp $(CLANCORE)/Math/vec4.cpp \
$(CLANCORE)/System/mutex.cpp $(CLANCORE)/System/runnable.cpp $(CLANCORE)/System/system.cpp $(CLANCORE)/System/thread.cpp $(CLANCORE)/System/thread_local_storage.cpp $(CLANCORE)/System/thread_local_storage_impl.cpp $(CLANCORE)/System/Unix/thread_unix.cpp \
$(SHARED)/Manager/Console.cpp \
$(SHARED)/Manager/GameTimer.cpp $(SHARED)/Manager/MessageManager.cpp $(SHARED)/Manager/ResourceManager.cpp $(SHARED)/Manager/VariantDB.cpp $(SHARED)/Math/rtPlane.cpp \
$(SHARED)/Math/rtRect.cpp \
$(SHARED)/util/CRandom.cpp $(SHARED)/util/GLESUtils.cpp $(SHARED)/util/MathUtils.cpp $(SHARED)/util/MiscUtils.cpp $(SHARED)/util/RenderUtils.cpp $(SHARED)/util/ResourceUtils.cpp \
$(SHARED)/util/Variant.cpp $(SHARED)/util/boost/libs/signals/src/connection.cpp $(SHARED)/util/boost/libs/signals/src/named_slot_map.cpp $(SHARED)/util/boost/libs/signals/src/signal_base.cpp \
$(SHARED)/util/boost/libs/signals/src/slot.cpp $(SHARED)/util/boost/libs/signals/src/trackable.cpp $(SHARED)/BaseApp.cpp \
$(SHARED)/util/unzip/zip.c $(SHARED)/util/unzip/unzip.c $(SHARED)/util/unzip/ioapi.c $(SHARED)/util/unzip/ioapi_mem.c \
$(SHARED)/util/TextScanner.cpp \
$(SHARED)/Network/NetHTTP.cpp $(SHARED)/Network/NetSocket.cpp $(SHARED)/Network/NetUtils.cpp $(SHARED)/Audio/AudioManagerSDL.cpp  $(SHARED)/FileSystem/StreamingInstance.cpp \
$(SHARED)/FileSystem/StreamingInstanceZip.cpp $(SHARED)/FileSystem/StreamingInstanceFile.cpp $(SHARED)/FileSystem/FileSystem.cpp $(SHARED)/FileSystem/FileSystemZip.cpp \
$(SHARED)/FileSystem/FileManager.cpp \
\
\
$(ENTITYSRC)/Entity.cpp $(ENTITYSRC)/Component.cpp $(ENTITYSRC)/EntityUtils.cpp $(ENTITYSRC)/FocusInputComponent.cpp $(ENTITYSRC)/FocusRenderComponent.cpp $(ENTITYSRC)/FocusUpdateComponent.cpp \
\
\
$(SHARED)/Irrlicht/IrrlichtManager.cpp $(IRRSRC)/CAttributes.cpp $(IRRSRC)/CBoneSceneNode.cpp $(IRRSRC)/CColorConverter.cpp \
$(IRRSRC)/CDefaultSceneNodeAnimatorFactory.cpp $(IRRSRC)/CDefaultSceneNodeFactory.cpp $(IRRSRC)/CDepthBuffer.cpp $(IRRSRC)/CDummyTransformationSceneNode.cpp $(IRRSRC)/CEmptySceneNode.cpp \
$(IRRSRC)/CFPSCounter.cpp $(IRRSRC)/CGeometryCreator.cpp $(IRRSRC)/CLightSceneNode.cpp $(IRRSRC)/CLogger.cpp $(IRRSRC)/CMemoryFile.cpp \
$(IRRSRC)/CMeshCache.cpp $(IRRSRC)/CMeshManipulator.cpp $(IRRSRC)/CMeshSceneNode.cpp $(IRRSRC)/COCTLoader.cpp $(IRRSRC)/COctreeSceneNode.cpp \
$(IRRSRC)/CSkinnedMesh.cpp $(IRRSRC)/CTextSceneNode.cpp $(IRRSRC)/CTriangleBBSelector.cpp $(IRRSRC)/CTriangleSelector.cpp $(IRRSRC)/COctreeTriangleSelector.cpp \
$(IRRSRC)/CVideoModeList.cpp $(IRRSRC)/CVolumeLightSceneNode.cpp $(IRRSRC)/CWaterSurfaceSceneNode.cpp $(IRRSRC)/Irrlicht.cpp \
$(IRRSRC)/irrXML.cpp $(IRRSRC)/os.cpp $(IRRSRC)/CMetaTriangleSelector.cpp \
\
$(IRRSRC)/CCameraSceneNode.cpp $(IRRSRC)/CSceneNodeAnimatorCameraFPS.cpp $(IRRSRC)/CIrrDeviceIPhone.cpp $(IRRSRC)/CIrrDeviceStub.cpp \
\
$(IRRSRC)/CFileList.cpp $(IRRSRC)/CFileSystem.cpp $(IRRSRC)/CLimitReadFile.cpp $(IRRSRC)/CMountPointReader.cpp \
$(IRRSRC)/COSOperator.cpp $(IRRSRC)/CReadFile.cpp $(IRRSRC)/CWriteFile.cpp  $(IRRSRC)/CXMLReader.cpp  $(IRRSRC)/CXMLWriter.cpp \
$(IRRSRC)/CZBuffer.cpp $(IRRSRC)/CZipReader.cpp $(IRRSRC)/CProtonReader.cpp \
\
$(IRRSRC)/CImage.cpp $(IRRSRC)/CImageLoaderBMP.cpp $(IRRSRC)/CImageLoaderJPG.cpp $(IRRSRC)/CImageLoaderPNG.cpp  $(IRRSRC)/CImageLoaderRGB.cpp $(IRRSRC)/CImageLoaderTGA.cpp \
\
$(JPGSRC)/jcapimin.c $(JPGSRC)/jcapistd.c $(JPGSRC)/jccoefct.c $(JPGSRC)/jccolor.c $(JPGSRC)/jcdctmgr.c $(JPGSRC)/jchuff.c $(JPGSRC)/jcinit.c $(JPGSRC)/jcmainct.c \
$(JPGSRC)/jcmarker.c $(JPGSRC)/jcmaster.c $(JPGSRC)/jcomapi.c $(JPGSRC)/jcparam.c $(JPGSRC)/jcphuff.c $(JPGSRC)/jcprepct.c $(JPGSRC)/jcsample.c $(JPGSRC)/jctrans.c \
$(JPGSRC)/jdapimin.c $(JPGSRC)/jdapistd.c $(JPGSRC)/jdatadst.c $(JPGSRC)/jdatasrc.c $(JPGSRC)/jdcoefct.c $(JPGSRC)/jdcolor.c $(JPGSRC)/jddctmgr.c \
$(JPGSRC)/jdhuff.c $(JPGSRC)/jdinput.c $(JPGSRC)/jdmainct.c $(JPGSRC)/jdmarker.c $(JPGSRC)/jdmaster.c $(JPGSRC)/jdmerge.c $(JPGSRC)/jdphuff.c $(JPGSRC)/jdpostct.c \
$(JPGSRC)/jdsample.c $(JPGSRC)/jdtrans.c $(JPGSRC)/jerror.c $(JPGSRC)/jfdctflt.c $(JPGSRC)/jfdctfst.c $(JPGSRC)/jfdctint.c $(JPGSRC)/jidctflt.c $(JPGSRC)/jidctfst.c \
$(JPGSRC)/jidctint.c $(JPGSRC)/jidctred.c $(JPGSRC)/jmemmgr.c $(JPGSRC)/jmemnobs.c $(JPGSRC)/jquant1.c $(JPGSRC)/jquant2.c $(JPGSRC)/jutils.c \
\
$(PNGSRC)/png.c $(PNGSRC)/pngerror.c $(PNGSRC)/pnggccrd.c $(PNGSRC)/pngget.c $(PNGSRC)/pngmem.c $(PNGSRC)/pngpread.c $(PNGSRC)/pngread.c \
$(PNGSRC)/pngrio.c $(PNGSRC)/pngrtran.c $(PNGSRC)/pngrutil.c $(PNGSRC)/pngset.c $(PNGSRC)/pngtrans.c $(PNGSRC)/pngvcrd.c $(PNGSRC)/pngwio.c $(PNGSRC)/pngwtran.c \
\
$(IRRMESH)/C3DSMeshFileLoader.cpp $(IRRMESH)/CAnimatedMeshMD2.cpp $(IRRMESH)/CAnimatedMeshMD3.cpp $(IRRMESH)/CB3DMeshFileLoader.cpp $(IRRMESH)/CBSPMeshFileLoader.cpp $(IRRMESH)/CAnimatedMeshHalfLife.cpp \
$(IRRMESH)/CColladaFileLoader.cpp $(IRRMESH)/CCSMLoader.cpp $(IRRMESH)/CMD2MeshFileLoader.cpp $(IRRMESH)/CMD3MeshFileLoader.cpp $(IRRMESH)/CMS3DMeshFileLoader.cpp \
$(IRRMESH)/CMY3DMeshFileLoader.cpp $(IRRMESH)/COBJMeshFileLoader.cpp $(IRRMESH)/CQ3LevelMesh.cpp $(IRRMESH)/CQuake3ShaderSceneNode.cpp $(IRRMESH)/CXMeshFileLoader.cpp $(IRRMESH)/CDMFLoader.cpp $(IRRMESH)/CLMTSMeshFileLoader.cpp $(IRRMESH)/CLWOMeshFileLoader.cpp $(IRRMESH)/COgreMeshFileLoader.cpp $(IRRMESH)/CSMFMeshFileLoader.cpp $(IRRMESH)/CSTLMeshFileLoader.cpp \
\
$(IRRMESH)/CParticleAnimatedMeshSceneNodeEmitter.cpp $(IRRMESH)/CParticleAttractionAffector.cpp $(IRRMESH)/CParticleBoxEmitter.cpp $(IRRMESH)/CParticleCylinderEmitter.cpp $(IRRMESH)/CParticleFadeOutAffector.cpp \
$(IRRMESH)/CParticleGravityAffector.cpp $(IRRMESH)/CParticleMeshEmitter.cpp $(IRRMESH)/CParticlePointEmitter.cpp $(IRRMESH)/CParticleRingEmitter.cpp $(IRRMESH)/CParticleRotationAffector.cpp \
$(IRRMESH)/CParticleScaleAffector.cpp $(IRRMESH)/CParticleSphereEmitter.cpp $(IRRMESH)/CParticleSystemSceneNode.cpp \
\
$(IRRSCENE)/CAnimatedMeshSceneNode.cpp $(IRRSCENE)/CBillboardSceneNode.cpp $(IRRSCENE)/CCubeSceneNode.cpp $(IRRSCENE)/CSceneCollisionManager.cpp $(IRRSCENE)/CSceneManager.cpp \
$(IRRSCENE)/CSceneNodeAnimatorCameraMaya.cpp $(IRRSCENE)/CSceneNodeAnimatorCollisionResponse.cpp $(IRRSCENE)/CSceneNodeAnimatorDelete.cpp $(IRRSCENE)/CSceneNodeAnimatorFlyCircle.cpp $(IRRSCENE)/CSceneNodeAnimatorFlyStraight.cpp \
$(IRRSCENE)/CSceneNodeAnimatorFollowSpline.cpp $(IRRSCENE)/CSceneNodeAnimatorRotation.cpp $(IRRSCENE)/CSceneNodeAnimatorTexture.cpp $(IRRSCENE)/CShadowVolumeSceneNode.cpp $(IRRSCENE)/CSceneLoaderIrr.cpp \
\
$(IRRSCENE)/CDefaultGUIElementFactory.cpp $(IRRSCENE)/CGUIEnvironment.cpp \
$(IRRSCENE)/CGUIButton.cpp $(IRRSCENE)/CGUICheckBox.cpp $(IRRSCENE)/CGUIColorSelectDialog.cpp $(IRRSCENE)/CGUIComboBox.cpp $(IRRSCENE)/CGUIContextMenu.cpp $(IRRSCENE)/CGUIEditBox.cpp $(IRRSCENE)/CGUIFileOpenDialog.cpp \
$(IRRSCENE)/CGUIFont.cpp $(IRRSCENE)/CGUIImage.cpp $(IRRSCENE)/CGUIImageList.cpp $(IRRSCENE)/CGUIInOutFader.cpp $(IRRSCENE)/CGUIListBox.cpp $(IRRSCENE)/CGUIMenu.cpp $(IRRSCENE)/CGUIMeshViewer.cpp $(IRRSCENE)/CGUIMessageBox.cpp \
$(IRRSCENE)/CGUIModalScreen.cpp $(IRRSCENE)/CGUIScrollBar.cpp $(IRRSCENE)/CGUISkin.cpp $(IRRSCENE)/CGUISpinBox.cpp $(IRRSCENE)/CGUISpriteBank.cpp $(IRRSCENE)/CGUIStaticText.cpp $(IRRSCENE)/CGUITabControl.cpp $(IRRSCENE)/CGUITable.cpp \
$(IRRSCENE)/CGUIToolBar.cpp $(IRRSCENE)/CGUITreeView.cpp $(IRRSCENE)/CGUIWindow.cpp \
\
$(IRRSRC)/CSkyBoxSceneNode.cpp $(IRRSRC)/CSkyDomeSceneNode.cpp $(IRRSRC)/CSphereSceneNode.cpp $(IRRSRC)/CTerrainSceneNode.cpp $(IRRSRC)/CTerrainTriangleSelector.cpp \
\
$(IRRSRC)/CNullDriver.cpp $(IRRSRC)/COGLESDriver.cpp $(IRRSRC)/COGLESExtensionHandler.cpp $(IRRSRC)/COGLESTexture.cpp $(IRRSRC)/COGLES2Driver.cpp $(IRRSRC)/COGLES2ExtensionHandler.cpp $(IRRSRC)/COGLES2Texture.cpp $(IRRSRC)/COGLES2FixedPipelineRenderer.cpp $(IRRSRC)/COGLES2MaterialRenderer.cpp $(IRRSRC)/COGLES2NormalMapRenderer.cpp $(IRRSRC)/COGLES2ParallaxMapRenderer.cpp $(IRRSRC)/COGLES2Renderer2D.cpp $(IRRSRC)/BuiltInFont.cpp \
\
$(APP)/App.cpp $(APP)/GUI/MainMenu.cpp $(APP)/GUI/Mesh3DMenu.cpp $(APP)/GUI/QuakeMenu.cpp $(APP)/GUI/QuakeShaderMenu.cpp $(APP)/GUI/HouseMenu.cpp $(APP)/GUI/TerrainMenu.cpp $(APP)/GUI/ShaderMenu.cpp $(APP)/GUI/StencilMenu.cpp \
\
$(APP)/Component/EventControlComponent.cpp

#Need match _IRR_COMPILE_WITH_OGLES2_ of IrrCompileConfig.h
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -ldl -llog -lz
#LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lz

include $(BUILD_SHARED_LIBRARY)
