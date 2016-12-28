LOCAL_PATH := $(call my-dir)

##################################
########### cocos2dx.a ###########
##################################

include $(CLEAR_VARS)

LOCAL_MODULE := libcocos2dx
SHARED := ../../../shared
LOCAL_ARM_MODE := arm

COCOS2DXSRC := $(SHARED)/cocos2dx

#LOCAL_CPP_FEATURES += exceptions
LOCAL_CPP_FEATURES += rtti

APP_DEBUG := $(strip $(NDK_DEBUG))
ifeq ($(APP_DEBUG),0)
	LOCAL_CFLAGS := -DANDROID_NDK -DBUILD_ANDROID -DGC_BUILD_ANDROID -DNDEBUG -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
	LOCAL_CPPFLAGS := -DGC_BUILD_C -DANDROID_NDK -DBUILD_ANDROID -DNDEBUG -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
else
	LOCAL_CFLAGS := -DANDROID_NDK -DBUILD_ANDROID -DGC_BUILD_ANDROID -D_DEBUG -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
	LOCAL_CPPFLAGS := -DGC_BUILD_C -DANDROID_NDK -DBUILD_ANDROID -D_DEBUG -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
	LOCAL_STRIP_MODULE := false
endif

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/$(APP) \
$(LOCAL_PATH)/$(SHARED) \
$(LOCAL_PATH)/$(SHARED)/ClanLib-2.0/Sources \
$(LOCAL_PATH)/$(SHARED)/util/boost \
$(LOCAL_PATH)/$(SHARED)/libxml \
$(LOCAL_PATH)/$(SHARED)/Irrlicht/include \
$(LOCAL_PATH)/$(SHARED)/Irrlicht/source/libpng \
$(LOCAL_PATH)/$(SHARED)/Irrlicht/source/jpeglib \
$(LOCAL_PATH)/$(SHARED)/cocos2dx \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/include \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/extensions \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/kazmath/include \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/platform \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/platform/android
                
LOCAL_SRC_FILES := \
$(SHARED)/libxml/c14n.c $(SHARED)/libxml/catalog.c $(SHARED)/libxml/chvalid.c $(SHARED)/libxml/debugXML.c $(SHARED)/libxml/dict.c \
$(SHARED)/libxml/DOCBparser.c $(SHARED)/libxml/encoding.c $(SHARED)/libxml/entities.c $(SHARED)/libxml/error.c $(SHARED)/libxml/globals.c \
$(SHARED)/libxml/hash.c $(SHARED)/libxml/HTMLparser.c $(SHARED)/libxml/HTMLtree.c $(SHARED)/libxml/legacy.c $(SHARED)/libxml/list.c \
$(SHARED)/libxml/nanoftp.c $(SHARED)/libxml/nanohttp.c $(SHARED)/libxml/parser.c $(SHARED)/libxml/parserInternals.c $(SHARED)/libxml/pattern.c \
$(SHARED)/libxml/relaxng.c $(SHARED)/libxml/runxmlconf.c $(SHARED)/libxml/SAX.c $(SHARED)/libxml/SAX2.c $(SHARED)/libxml/schematron.c \
$(SHARED)/libxml/threads.c $(SHARED)/libxml/tree.c $(SHARED)/libxml/uri.c $(SHARED)/libxml/valid.c $(SHARED)/libxml/winnprintf.c \
$(SHARED)/libxml/xinclude.c $(SHARED)/libxml/xlink.c $(SHARED)/libxml/xmlcatalog.c $(SHARED)/libxml/xmlIO.c $(SHARED)/libxml/xmlmemory.c \
$(SHARED)/libxml/xmlmodule.c $(SHARED)/libxml/xmlreader.c $(SHARED)/libxml/xmlregexp.c $(SHARED)/libxml/xmlsave.c $(SHARED)/libxml/xmlschemas.c \
$(SHARED)/libxml/xmlschemastypes.c $(SHARED)/libxml/xmlstring.c $(SHARED)/libxml/xmlunicode.c $(SHARED)/libxml/xmlwriter.c $(SHARED)/libxml/xpath.c \
$(SHARED)/libxml/xpointer.c \
\
$(COCOS2DXSRC)/actions/CCAction.cpp $(COCOS2DXSRC)/actions/CCActionCamera.cpp $(COCOS2DXSRC)/actions/CCActionCatmullRom.cpp $(COCOS2DXSRC)/actions/CCActionEase.cpp $(COCOS2DXSRC)/actions/CCActionGrid.cpp \
$(COCOS2DXSRC)/actions/CCActionGrid3D.cpp $(COCOS2DXSRC)/actions/CCActionInstant.cpp $(COCOS2DXSRC)/actions/CCActionInterval.cpp $(COCOS2DXSRC)/actions/CCActionManager.cpp $(COCOS2DXSRC)/actions/CCActionPageTurn3D.cpp \
$(COCOS2DXSRC)/actions/CCActionProgressTimer.cpp $(COCOS2DXSRC)/actions/CCActionTiledGrid.cpp $(COCOS2DXSRC)/actions/CCActionTween.cpp \
\
$(COCOS2DXSRC)/base_nodes/CCAtlasNode.cpp $(COCOS2DXSRC)/base_nodes/CCNode.cpp \
\
$(COCOS2DXSRC)/cocoa/CCAffineTransform.cpp $(COCOS2DXSRC)/cocoa/CCGeometry.cpp $(COCOS2DXSRC)/cocoa/CCAutoreleasePool.cpp $(COCOS2DXSRC)/cocoa/CCDictionary.cpp $(COCOS2DXSRC)/cocoa/CCNS.cpp \
$(COCOS2DXSRC)/cocoa/CCObject.cpp $(COCOS2DXSRC)/cocoa/CCSet.cpp $(COCOS2DXSRC)/cocoa/CCString.cpp $(COCOS2DXSRC)/cocoa/CCZone.cpp $(COCOS2DXSRC)/cocoa/CCArray.cpp \
\
$(COCOS2DXSRC)/effects/CCGrabber.cpp $(COCOS2DXSRC)/effects/CCGrid.cpp \
\
$(COCOS2DXSRC)/kazmath/src/aabb.cpp $(COCOS2DXSRC)/kazmath/src/ccmat3.cpp $(COCOS2DXSRC)/kazmath/src/ccmat4.cpp $(COCOS2DXSRC)/kazmath/src/ccplane.cpp $(COCOS2DXSRC)/kazmath/src/ccquaternion.cpp \
$(COCOS2DXSRC)/kazmath/src/ccray2.cpp $(COCOS2DXSRC)/kazmath/src/ccutility.cpp $(COCOS2DXSRC)/kazmath/src/ccvec2.cpp $(COCOS2DXSRC)/kazmath/src/ccvec3.cpp $(COCOS2DXSRC)/kazmath/src/ccvec4.cpp \
$(COCOS2DXSRC)/kazmath/src/GL/ccmat4stack.cpp $(COCOS2DXSRC)/kazmath/src/GL/ccmatrix.cpp \
\
$(COCOS2DXSRC)/keypad_dispatcher/CCKeypadDelegate.cpp $(COCOS2DXSRC)/keypad_dispatcher/CCKeypadDispatcher.cpp \
\
$(COCOS2DXSRC)/label_nodes/CCLabelAtlas.cpp $(COCOS2DXSRC)/label_nodes/CCLabelBMFont.cpp $(COCOS2DXSRC)/label_nodes/CCLabelTTF.cpp \
\
$(COCOS2DXSRC)/layers_scenes_transitions_nodes/CCLayer.cpp $(COCOS2DXSRC)/layers_scenes_transitions_nodes/CCScene.cpp $(COCOS2DXSRC)/layers_scenes_transitions_nodes/CCTransitionPageTurn.cpp $(COCOS2DXSRC)/layers_scenes_transitions_nodes/CCTransition.cpp $(COCOS2DXSRC)/layers_scenes_transitions_nodes/CCTransitionProgress.cpp \
\
$(COCOS2DXSRC)/menu_nodes/CCMenu.cpp $(COCOS2DXSRC)/menu_nodes/CCMenuItem.cpp \
\
$(COCOS2DXSRC)/misc_nodes/CCMotionStreak.cpp $(COCOS2DXSRC)/misc_nodes/CCProgressTimer.cpp $(COCOS2DXSRC)/misc_nodes/CCRenderTexture.cpp \
\
$(COCOS2DXSRC)/particle_nodes/CCParticleExamples.cpp $(COCOS2DXSRC)/particle_nodes/CCParticleSystem.cpp $(COCOS2DXSRC)/particle_nodes/CCParticleBatchNode.cpp $(COCOS2DXSRC)/particle_nodes/CCParticleSystemQuad.cpp \
\
$(COCOS2DXSRC)/platform/CCSAXParser.cpp $(COCOS2DXSRC)/platform/CCThread.cpp $(COCOS2DXSRC)/platform/platform.cpp \
\
$(COCOS2DXSRC)/platform/android/CCCommon.cpp $(COCOS2DXSRC)/platform/android/CCFileUtils.cpp $(COCOS2DXSRC)/platform/android/CCImage.cpp \
\
$(COCOS2DXSRC)/platform/android/jni/JniHelper.cpp \
\
$(COCOS2DXSRC)/script_support/CCScriptSupport.cpp \
\
$(COCOS2DXSRC)/shaders/CCGLProgram.cpp $(COCOS2DXSRC)/shaders/ccGLStateCache.cpp $(COCOS2DXSRC)/shaders/CCShaderCache.cpp \
\
$(COCOS2DXSRC)/sprite_nodes/CCAnimation.cpp $(COCOS2DXSRC)/sprite_nodes/CCAnimationCache.cpp $(COCOS2DXSRC)/sprite_nodes/CCSprite.cpp $(COCOS2DXSRC)/sprite_nodes/CCSpriteBatchNode.cpp $(COCOS2DXSRC)/sprite_nodes/CCSpriteFrame.cpp \
$(COCOS2DXSRC)/sprite_nodes/CCSpriteFrameCache.cpp \
\
$(COCOS2DXSRC)/support/CCNotificationCenter.cpp $(COCOS2DXSRC)/support/CCProfiling.cpp $(COCOS2DXSRC)/support/CCPointExtension.cpp $(COCOS2DXSRC)/support/TransformUtils.cpp $(COCOS2DXSRC)/support/CCUserDefault.cpp \
$(COCOS2DXSRC)/support/base64.cpp $(COCOS2DXSRC)/support/ccUtils.cpp $(COCOS2DXSRC)/support/CCVertex.cpp \
\
$(COCOS2DXSRC)/support/data_support/ccCArray.cpp $(COCOS2DXSRC)/support/image_support/TGAlib.cpp $(COCOS2DXSRC)/support/zip_support/ZipUtils.cpp \
\
$(COCOS2DXSRC)/text_input_node/CCIMEDispatcher.cpp $(COCOS2DXSRC)/text_input_node/CCTextFieldTTF.cpp \
\
$(COCOS2DXSRC)/textures/CCTexture2D.cpp $(COCOS2DXSRC)/textures/CCTextureAtlas.cpp $(COCOS2DXSRC)/textures/CCTextureCache.cpp $(COCOS2DXSRC)/textures/CCTexturePVR.cpp \
\
$(COCOS2DXSRC)/tilemap_parallax_nodes/CCParallaxNode.cpp $(COCOS2DXSRC)/tilemap_parallax_nodes/CCTMXLayer.cpp $(COCOS2DXSRC)/tilemap_parallax_nodes/CCTMXObjectGroup.cpp $(COCOS2DXSRC)/tilemap_parallax_nodes/CCTMXTiledMap.cpp $(COCOS2DXSRC)/tilemap_parallax_nodes/CCTMXXMLParser.cpp \
$(COCOS2DXSRC)/tilemap_parallax_nodes/CCTileMapAtlas.cpp \
\
$(COCOS2DXSRC)/touch_dispatcher/CCTouchDispatcher.cpp $(COCOS2DXSRC)/touch_dispatcher/CCTouchHandler.cpp  $(COCOS2DXSRC)/touch_dispatcher/CCTouch.cpp \
\
$(COCOS2DXSRC)/CCConfiguration.cpp $(COCOS2DXSRC)/CCDrawingPrimitives.cpp $(COCOS2DXSRC)/CCScheduler.cpp $(COCOS2DXSRC)/CCCamera.cpp $(COCOS2DXSRC)/CCDirector.cpp \
$(COCOS2DXSRC)/cocos2d.cpp \
\
$(COCOS2DXSRC)/extensions/CCScrollView/CCScrollView.cpp $(COCOS2DXSRC)/extensions/CCScrollView/CCSorting.cpp $(COCOS2DXSRC)/extensions/CCScrollView/CCTableView.cpp $(COCOS2DXSRC)/extensions/CCScrollView/CCTableViewCell.cpp \
\
$(COCOS2DXSRC)/extensions/CCControlExtension/CCControl.cpp $(COCOS2DXSRC)/extensions/CCControlExtension/CCControlButton.cpp $(COCOS2DXSRC)/extensions/CCControlExtension/CCControlColourPicker.cpp $(COCOS2DXSRC)/extensions/CCControlExtension/CCControlHuePicker.cpp $(COCOS2DXSRC)/extensions/CCControlExtension/CCControlPotentiometer.cpp \
$(COCOS2DXSRC)/extensions/CCControlExtension/CCControlSaturationBrightnessPicker.cpp $(COCOS2DXSRC)/extensions/CCControlExtension/CCControlSlider.cpp $(COCOS2DXSRC)/extensions/CCControlExtension/CCControlStepper.cpp $(COCOS2DXSRC)/extensions/CCControlExtension/CCControlSwitch.cpp $(COCOS2DXSRC)/extensions/CCControlExtension/CCControlUtils.cpp \
$(COCOS2DXSRC)/extensions/CCControlExtension/CCInvocation.cpp $(COCOS2DXSRC)/extensions/CCControlExtension/CCScale9Sprite.cpp \
\
$(COCOS2DXSRC)/extensions/CCBReader/CCBAnimationManager.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCBFileLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCBKeyframe.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCBReader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCBSequence.cpp \
$(COCOS2DXSRC)/extensions/CCBReader/CCBSequenceProperty.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCBValue.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCControlButtonLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCControlLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCData.cpp \
$(COCOS2DXSRC)/extensions/CCBReader/CCLabelBMFontLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCLabelTTFLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCLayerColorLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCLayerGradientLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCLayerLoader.cpp \
$(COCOS2DXSRC)/extensions/CCBReader/CCMenuItemImageLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCMenuItemLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCNode+CCBRelativePositioning.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCNodeLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCNodeLoaderLibrary.cpp \
$(COCOS2DXSRC)/extensions/CCBReader/CCParticleSystemQuadLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCScale9SpriteLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCScrollViewLoader.cpp $(COCOS2DXSRC)/extensions/CCBReader/CCSpriteLoader.cpp

include $(BUILD_STATIC_LIBRARY)

##################################
########## rt3dapp.so ############
##################################

include $(CLEAR_VARS)

LOCAL_MODULE := librt3dapp
SHARED := ../../../shared
APP := ../../source
LOCAL_ARM_MODE := arm

ENTITYSRC := $(SHARED)/Entity
CLANCORE := $(SHARED)/ClanLib-2.0/Sources/Core
IRRSRC :=  $(SHARED)/Irrlicht/source
IRRMESH := $(IRRSRC)
IRRSCENE := $(IRRSRC)
PNGSRC := $(SHARED)/Irrlicht/source/libpng
JPGSRC := $(SHARED)/Irrlicht/source/jpeglib

#LOCAL_CPP_FEATURES += exceptions
LOCAL_CPP_FEATURES += rtti

APP_DEBUG := $(strip $(NDK_DEBUG))
ifeq ($(APP_DEBUG),0)
	LOCAL_CFLAGS := -DANDROID_NDK -DBUILD_ANDROID -DGC_BUILD_ANDROID -DNDEBUG -D_IRR_STATIC_LIB_ -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
	LOCAL_CPPFLAGS := -DGC_BUILD_C -DANDROID_NDK -DBUILD_ANDROID -DNDEBUG -D_IRR_STATIC_LIB_ -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
else
	LOCAL_CFLAGS := -DANDROID_NDK -DBUILD_ANDROID -DGC_BUILD_ANDROID -D_DEBUG -D_IRR_STATIC_LIB_ -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
	LOCAL_CPPFLAGS := -DGC_BUILD_C -DANDROID_NDK -DBUILD_ANDROID -D_DEBUG -D_IRR_STATIC_LIB_ -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp
	LOCAL_STRIP_MODULE := false
endif

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/$(APP) \
$(LOCAL_PATH)/$(SHARED) \
$(LOCAL_PATH)/$(SHARED)/ClanLib-2.0/Sources \
$(LOCAL_PATH)/$(SHARED)/util/boost \
$(LOCAL_PATH)/$(SHARED)/libxml \
$(LOCAL_PATH)/$(SHARED)/Irrlicht/include \
$(LOCAL_PATH)/$(SHARED)/Irrlicht/source/libpng \
$(LOCAL_PATH)/$(SHARED)/Irrlicht/source/jpeglib \
$(LOCAL_PATH)/$(SHARED)/cocos2dx \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/include \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/extensions \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/kazmath/include \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/platform \
$(LOCAL_PATH)/$(SHARED)/cocos2dx/platform/android
                
LOCAL_SRC_FILES := \
$(SHARED)/PlatformSetup.cpp $(SHARED)/android/AndroidUtils.cpp ../temp_final_cpp_src/AndroidApp.cpp \
$(SHARED)/Audio/AudioManager.cpp $(SHARED)/Audio/AudioManagerAndroid.cpp \
$(CLANCORE)/core_global.cpp \
$(CLANCORE)/Math/angle.cpp $(CLANCORE)/Math/mat3.cpp $(CLANCORE)/Math/mat4.cpp $(CLANCORE)/Math/rect.cpp $(CLANCORE)/Math/vec2.cpp $(CLANCORE)/Math/vec3.cpp $(CLANCORE)/Math/vec4.cpp \
$(CLANCORE)/System/mutex.cpp $(CLANCORE)/System/runnable.cpp $(CLANCORE)/System/system.cpp $(CLANCORE)/System/thread.cpp $(CLANCORE)/System/thread_local_storage.cpp $(CLANCORE)/System/thread_local_storage_impl.cpp $(CLANCORE)/System/Unix/thread_unix.cpp \
$(SHARED)/Manager/Console.cpp \
$(SHARED)/Manager/GameTimer.cpp $(SHARED)/Manager/MessageManager.cpp $(SHARED)/Manager/ResourceManager.cpp $(SHARED)/Manager/VariantDB.cpp $(SHARED)/Math/rtPlane.cpp \
$(SHARED)/Math/rtRect.cpp \
$(SHARED)/util/CRandom.cpp $(SHARED)/util/MathUtils.cpp $(SHARED)/util/MiscUtils.cpp $(SHARED)/util/RenderUtils.cpp $(SHARED)/util/ResourceUtils.cpp \
$(SHARED)/util/Variant.cpp $(SHARED)/util/boost/libs/signals/src/connection.cpp $(SHARED)/util/boost/libs/signals/src/named_slot_map.cpp $(SHARED)/util/boost/libs/signals/src/signal_base.cpp \
$(SHARED)/util/boost/libs/signals/src/slot.cpp $(SHARED)/util/boost/libs/signals/src/trackable.cpp $(SHARED)/BaseApp.cpp \
$(SHARED)/util/unzip/zip.c $(SHARED)/util/unzip/unzip.c $(SHARED)/util/unzip/ioapi.c $(SHARED)/util/unzip/ioapi_mem.c \
$(SHARED)/Audio/AudioManagerSDL.cpp \
$(SHARED)/FileSystem/StreamingInstance.cpp $(SHARED)/FileSystem/StreamingInstanceFile.cpp \
$(SHARED)/FileSystem/StreamingInstanceZip.cpp $(SHARED)/FileSystem/FileSystem.cpp $(SHARED)/FileSystem/FileSystemZip.cpp \
$(SHARED)/FileSystem/FileManager.cpp \
\
$(ENTITYSRC)/Entity.cpp $(ENTITYSRC)/Component.cpp $(ENTITYSRC)/EntityUtils.cpp $(ENTITYSRC)/FocusRenderComponent.cpp $(ENTITYSRC)/FocusUpdateComponent.cpp \
\
$(SHARED)/Irrlicht/IrrlichtManager.cpp $(IRRSRC)/CAttributes.cpp $(IRRSRC)/CBoneSceneNode.cpp $(IRRSRC)/CColorConverter.cpp \
$(IRRSRC)/CDefaultSceneNodeAnimatorFactory.cpp $(IRRSRC)/CDefaultSceneNodeFactory.cpp $(IRRSRC)/CDepthBuffer.cpp $(IRRSRC)/CDummyTransformationSceneNode.cpp $(IRRSRC)/CEmptySceneNode.cpp \
$(IRRSRC)/CFPSCounter.cpp $(IRRSRC)/CGeometryCreator.cpp $(IRRSRC)/CLightSceneNode.cpp $(IRRSRC)/CLogger.cpp $(IRRSRC)/CMemoryFile.cpp \
$(IRRSRC)/CMeshCache.cpp $(IRRSRC)/CMeshManipulator.cpp $(IRRSRC)/CMeshSceneNode.cpp $(IRRSRC)/COCTLoader.cpp $(IRRSRC)/COctreeSceneNode.cpp \
$(IRRSRC)/CSkinnedMesh.cpp $(IRRSRC)/CTextSceneNode.cpp $(IRRSRC)/CTriangleBBSelector.cpp $(IRRSRC)/CTriangleSelector.cpp $(IRRSRC)/COctreeTriangleSelector.cpp \
$(IRRSRC)/CVideoModeList.cpp $(IRRSRC)/CVolumeLightSceneNode.cpp $(IRRSRC)/CMetaTriangleSelector.cpp $(IRRSRC)/CWaterSurfaceSceneNode.cpp \
$(IRRSRC)/Irrlicht.cpp $(IRRSRC)/irrXML.cpp $(IRRSRC)/os.cpp \
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
$(PNGSRC)/pngwrite.c $(PNGSRC)/pngwutil.c \
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
$(IRRSRC)/CNullDriver.cpp $(IRRSRC)/COGLESDriver.cpp $(IRRSRC)/COGLESExtensionHandler.cpp $(IRRSRC)/COGLESTexture.cpp \
$(IRRSRC)/COGLES2Driver.cpp $(IRRSRC)/COGLES2ExtensionHandler.cpp $(IRRSRC)/COGLES2Texture.cpp $(IRRSRC)/COGLES2FixedPipelineRenderer.cpp $(IRRSRC)/COGLES2MaterialRenderer.cpp $(IRRSRC)/COGLES2NormalMapRenderer.cpp $(IRRSRC)/COGLES2ParallaxMapRenderer.cpp $(IRRSRC)/COGLES2Renderer2D.cpp $(IRRSRC)/BuiltInFont.cpp \
\
$(APP)/App.cpp $(APP)/GUI/MainMenu.cpp \
$(APP)/GUI/Mesh3DMenu.cpp $(APP)/GUI/QuakeMenu.cpp $(APP)/GUI/QuakeShaderMenu.cpp $(APP)/GUI/HouseMenu.cpp \
$(APP)/GUI/TerrainMenu.cpp $(APP)/GUI/ShaderMenu.cpp $(APP)/GUI/StencilMenu.cpp \
$(APP)/GUI/HelloWorldScene.cpp $(APP)/GUI/GuiScroll.cpp \
$(APP)/Component/EventControlComponent.cpp

LOCAL_STATIC_LIBRARIES := libcocos2dx
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -ldl -llog -lz

include $(BUILD_SHARED_LIBRARY)
