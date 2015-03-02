// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
    #include "OpenGLES/ES1/gl.h"
    #include "OpenGLES/ES1/glext.h"
#else
    #ifdef _WIN32
		#include "EGL/egl.h"
	#endif

    #include "GLES/gl.h"
    #include "GLES/glext.h"
#endif

// needed here also because of the create methods' parameters
#include "CNullDriver.h"
#include "COGLESExtensionHandler.h"
#include "COGLESDriver.h"

#ifdef _IRR_COMPILE_WITH_OGLES1_
#include "COGLESTexture.h"
#include "COGLESMaterialRenderer.h"
#include "CImage.h"
#include "os.h"

#ifdef _WIN32
	#pragma comment(lib, "../../shared/dep32/ogles1/lib/libEGL.lib")
	#pragma comment(lib, "../../shared/dep32/ogles1/lib/libgles_cm.lib")
#endif

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
	#include <SDL/SDL.h>
#endif

namespace irr
{
namespace video
{

//! constructor and init code
COGLES1Driver::COGLES1Driver(const SIrrlichtCreationParameters& params,
		const SExposedVideoData& data, io::IFileSystem* io
#if defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
		, MIrrIPhoneDevice* device //by stone
#endif
		)
: CNullDriver(io, params.WindowSize), COGLES1ExtensionHandler(),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true),
	Transformation3DChanged(true), AntiAlias(params.AntiAlias), BridgeCalls(0),
	RenderTargetTexture(0), CurrentRendertargetSize(0,0), ColorFormat(ECF_R8G8B8)
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	,HDc(0)
#elif defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
	,ViewFramebuffer(0)
	,ViewRenderbuffer(0)
	,ViewDepthRenderbuffer(0)
#endif
{
	#ifdef _DEBUG
	setDebugName("COGLESDriver");
	#endif
	ExposedData=data;

    //by stone
/*
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	EglWindow = (NativeWindowType)data.OpenGLWin32.HWnd;
	HDc = GetDC((HWND)EglWindow);
	EglDisplay = eglGetDisplay((NativeDisplayType)HDc);
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	EglWindow = (NativeWindowType)ExposedData.OpenGLLinux.X11Window;
	EglDisplay = eglGetDisplay((NativeDisplayType)ExposedData.OpenGLLinux.X11Display);
#elif defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
	Device = device;
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
	EglWindow =	((struct android_app *)(params.PrivateData))->window;
	EglDisplay = EGL_NO_DISPLAY;
#endif
#ifdef EGL_VERSION_1_0
	if(EglDisplay == EGL_NO_DISPLAY)
		EglDisplay = eglGetDisplay((NativeDisplayType) EGL_DEFAULT_DISPLAY);
	if(EglDisplay == EGL_NO_DISPLAY)
	{
		os::Printer::log("Could not get OpenGL-ES1 display.");
	}

	EGLint majorVersion, minorVersion;
	if (!eglInitialize(EglDisplay, &majorVersion, &minorVersion))
	{
		os::Printer::log("Could not initialize OpenGL-ES1 display.");
	}
	else
		os::Printer::log("EGL version", core::stringc(majorVersion+(minorVersion/10.f)).c_str());

	EGLint attribs[] =
	{
#if defined( _IRR_COMPILE_WITH_ANDROID_DEVICE_ )
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_NONE
#else		
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 5,
		EGL_BLUE_SIZE, 5,
		EGL_ALPHA_SIZE, params.WithAlphaChannel?1:0,
		EGL_BUFFER_SIZE, params.Bits,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
//		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
		EGL_DEPTH_SIZE, params.ZBufferBits,
		EGL_STENCIL_SIZE, params.Stencilbuffer,
		EGL_SAMPLE_BUFFERS, params.AntiAlias?1:0,
		EGL_SAMPLES, params.AntiAlias,
#ifdef EGL_VERSION_1_3
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
#endif
		EGL_NONE, 0
#endif		
	};
	EGLint contextAttrib[] =
	{
#ifdef EGL_VERSION_1_3
		EGL_CONTEXT_CLIENT_VERSION, 1,
#endif
		EGL_NONE, 0
	};
	EGLConfig config;
	EGLint num_configs;
	u32 steps=5;
	while (!eglChooseConfig(EglDisplay, attribs, &config, 1, &num_configs) || !num_configs)
	{
		switch (steps)
		{
		case 5: // samples
			if (attribs[19]>2)
			{
				--attribs[19];
			}
			else
			{
				attribs[17]=0;
				attribs[19]=0;
				--steps;
			}
			break;
		case 4: // alpha
			if (attribs[7])
			{
				attribs[7]=0;
				if (params.AntiAlias)
				{
					attribs[17]=1;
					attribs[19]=params.AntiAlias;
					steps=5;
				}
			}
			else
				--steps;
			break;
		case 3: // stencil
			if (attribs[15])
			{
				attribs[15]=0;
				if (params.AntiAlias)
				{
					attribs[17]=1;
					attribs[19]=params.AntiAlias;
					steps=5;
				}
			}
			else
				--steps;
			break;
		case 2: // depth size
			if (attribs[13]>16)
			{
				attribs[13]-=8;
			}
			else
				--steps;
			break;
		case 1: // buffer size
			if (attribs[9]>16)
			{
				attribs[9]-=8;
			}
			else
				--steps;
			break;
		default:
			os::Printer::log("Could not get config for OpenGL-ES1 display.");
			return;
		}
	}
	if (params.AntiAlias && !attribs[17])
		os::Printer::log("No multisampling.");
	if (params.WithAlphaChannel && !attribs[7])
		os::Printer::log("No alpha.");
	if (params.Stencilbuffer && !attribs[15])
		os::Printer::log("No stencil buffer.");
	if (params.ZBufferBits > attribs[13])
		os::Printer::log("No full depth buffer.");
	if (params.Bits > attribs[9])
		os::Printer::log("No full color buffer.");

	#if defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
         EGLint format;
        eglGetConfigAttrib(EglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);

        ANativeWindow_setBuffersGeometry(EglWindow, 0, 0, format);
    #endif
   
	EglSurface = eglCreateWindowSurface(EglDisplay, config, EglWindow, NULL);
	
	if (EGL_NO_SURFACE==EglSurface)
		EglSurface = eglCreateWindowSurface(EglDisplay, config, NULL, NULL);

	if (EGL_NO_SURFACE==EglSurface)
	{
		testEGLError();
		os::Printer::log("Could not create surface for OpenGL-ES1 display.");
	}

#ifdef EGL_VERSION_1_2
	if (minorVersion>1)
		eglBindAPI(EGL_OPENGL_ES_API);
#endif
	EglContext = eglCreateContext(EglDisplay, config, EGL_NO_CONTEXT, contextAttrib);
	if (testEGLError())
	{
		os::Printer::log("Could not create Context for OpenGL-ES1 display.");
	}

	eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);
	if (testEGLError())
	{
		os::Printer::log("Could not make Context current for OpenGL-ES1 display.");
	}

	genericDriverInit(params.WindowSize, params.Stencilbuffer);

	// set vsync
	if (params.Vsync)
		eglSwapInterval(EglDisplay, 1);
#elif defined(GL_VERSION_ES_CM_1_0)
	glGenFramebuffersOES(1, &ViewFramebuffer);
	glGenRenderbuffersOES(1, &ViewRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, ViewRenderbuffer);

#if defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
	ExposedData.OGLESIPhone.AppDelegate = Device;
    Device->displayInitialize(&ExposedData.OGLESIPhone.Context, &ExposedData.OGLESIPhone.View);
#endif

	GLint backingWidth;
	GLint backingHeight;
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	glGenRenderbuffersOES(1, &ViewDepthRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, ViewDepthRenderbuffer);
    
    GLenum depthComponent = GL_DEPTH_COMPONENT16_OES;
    
    if(params.ZBufferBits >= 24)
        depthComponent = GL_DEPTH_COMPONENT24_OES;
    
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, depthComponent, backingWidth, backingHeight);
    
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, ViewFramebuffer);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, ViewRenderbuffer);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, ViewDepthRenderbuffer);
    
    core::dimension2d<u32> WindowSize(backingWidth, backingHeight);
    CNullDriver::ScreenSize = WindowSize;
    CNullDriver::ViewPort = core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(WindowSize));
#endif*/

	//genericDriverInit(WindowSize, params.Stencilbuffer);
    genericDriverInit(params.WindowSize, params.Stencilbuffer);
}

//! destructor
COGLES1Driver::~COGLES1Driver()
{
	RequestedLights.clear();
	deleteMaterialRenders();
	deleteAllTextures();

	//by CNullDriver::~CNullDriver()
	//removeAllOcclusionQueries();
	//removeAllHardwareBuffers();

	if (BridgeCalls)
		delete BridgeCalls;

/*
#if defined(EGL_VERSION_1_0)
	eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(EglDisplay, EglContext);
	eglDestroySurface(EglDisplay, EglSurface);
	eglTerminate(EglDisplay);
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	if (HDc)
		ReleaseDC((HWND)EglWindow, HDc);
#endif
#elif defined(GL_VERSION_ES_CM_1_0)
	if (0 != ViewFramebuffer)
	{
		extGlDeleteFramebuffers(1,&ViewFramebuffer);
		ViewFramebuffer = 0;
	}
	if (0 != ViewRenderbuffer)
	{
		extGlDeleteRenderbuffers(1,&ViewRenderbuffer);
		ViewRenderbuffer = 0;
	}
	if (0 != ViewDepthRenderbuffer)
	{
		extGlDeleteRenderbuffers(1,&ViewDepthRenderbuffer);
		ViewDepthRenderbuffer = 0;
	}
#endif
*/
}

// -----------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------

bool COGLES1Driver::genericDriverInit(const core::dimension2d<u32>& screenSize, bool stencilBuffer)
{
	Name=glGetString(GL_VERSION);
	printVersion();

	// print renderer information
	vendorName = glGetString(GL_VENDOR);
	os::Printer::log(vendorName.c_str(), ELL_INFORMATION);

	CurrentTexture.clear();

	// load extensions
	initExtensions(this, stencilBuffer);

	if (!BridgeCalls)
		BridgeCalls = new COGLES1CallBridge(this);

	StencilBuffer=stencilBuffer;

	DriverAttributes->setAttribute("MaxTextures", MaxTextureUnits);
	DriverAttributes->setAttribute("MaxSupportedTextures", MaxSupportedTextures);
	DriverAttributes->setAttribute("MaxLights", MaxLights);
	DriverAttributes->setAttribute("MaxAnisotropy", MaxAnisotropy);
	DriverAttributes->setAttribute("MaxUserClipPlanes", MaxUserClipPlanes);
	DriverAttributes->setAttribute("MaxAuxBuffers", MaxAuxBuffers);
	DriverAttributes->setAttribute("MaxMultipleRenderTargets", MaxMultipleRenderTargets);
	DriverAttributes->setAttribute("MaxIndices", (s32)MaxIndices);
	DriverAttributes->setAttribute("MaxTextureSize", (s32)MaxTextureSize);
	DriverAttributes->setAttribute("MaxTextureLODBias", MaxTextureLODBias);
	DriverAttributes->setAttribute("Version", Version);
	DriverAttributes->setAttribute("AntiAlias", AntiAlias);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//SETH we don't want them to screw with the view port
    // Reset The Current Viewport
	//glViewport(0, 0, screenSize.Width, screenSize.Height);

	UserClipPlane.reallocate(MaxUserClipPlanes);
	UserClipPlaneEnabled.reallocate(MaxUserClipPlanes);

	u32 i = 0;
	for (i=0; i<MaxUserClipPlanes; ++i)
	{
		UserClipPlane.push_back(core::plane3df());
		UserClipPlaneEnabled.push_back(false);
	}

	for (i=0; i<ETS_COUNT; ++i)
		setTransform(static_cast<E_TRANSFORMATION_STATE>(i), core::IdentityMatrix);

	setAmbientLight(SColorf(0.0f,0.0f,0.0f,0.0f));

    //by stone
#ifdef GL_EXT_separate_specular_color
	if (FeatureAvailable[IRR_EXT_separate_specular_color])
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif

    // TODO ogl-es
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	glClearDepthf(1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace( GL_CW );

	// create material renderers
	createMaterialRenderers();

	// set the renderstates
	setRenderStates3DMode();

	glAlphaFunc(GL_GREATER, 0.f);

	// set fog mode
	setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// create matrix for flipping textures
	TextureFlipMatrix.buildTextureTransform(0.0f, core::vector2df(0,0), core::vector2df(0,1.0f), core::vector2df(1.0f,-1.0f));

	// We need to reset once more at the beginning of the first rendering.
	// This fixes problems with intermediate changes to the material during texture load.
	ResetRenderStates = true;

	return true;
}

bool COGLES1Driver::OnAgainDriverInit() //by stone
{
	bool stencilBuffer = queryFeature(video::EVDF_STENCIL_BUFFER);

	Name=glGetString(GL_VERSION);
	printVersion();

	// print renderer information
	vendorName = glGetString(GL_VENDOR);
	os::Printer::log(vendorName.c_str(), ELL_INFORMATION);

	CurrentTexture.clear();

	// load extensions
	initExtensions(this, stencilBuffer);

	if(BridgeCalls)
		delete BridgeCalls;

	//deleteMaterialRenders();
	//removeAllHardwareBuffers();
	CNullDriver::OnAgainDriverInit(); //by stone

	BridgeCalls = new COGLES1CallBridge(this);

	StencilBuffer=stencilBuffer;

	DriverAttributes->setAttribute("MaxTextures", MaxTextureUnits);
	DriverAttributes->setAttribute("MaxSupportedTextures", MaxSupportedTextures);
	DriverAttributes->setAttribute("MaxLights", MaxLights);
	DriverAttributes->setAttribute("MaxAnisotropy", MaxAnisotropy);
	DriverAttributes->setAttribute("MaxUserClipPlanes", MaxUserClipPlanes);
	DriverAttributes->setAttribute("MaxAuxBuffers", MaxAuxBuffers);
	DriverAttributes->setAttribute("MaxMultipleRenderTargets", MaxMultipleRenderTargets);
	DriverAttributes->setAttribute("MaxIndices", (s32)MaxIndices);
	DriverAttributes->setAttribute("MaxTextureSize", (s32)MaxTextureSize);
	DriverAttributes->setAttribute("MaxTextureLODBias", MaxTextureLODBias);
	DriverAttributes->setAttribute("Version", Version);
	DriverAttributes->setAttribute("AntiAlias", AntiAlias);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//SETH we don't want them to screw with the view port
    // Reset The Current Viewport
	//glViewport(0, 0, screenSize.Width, screenSize.Height);

	UserClipPlane.reallocate(MaxUserClipPlanes);
	UserClipPlaneEnabled.reallocate(MaxUserClipPlanes);

	u32 i = 0;
	for (i=0; i<MaxUserClipPlanes; ++i)
	{
		UserClipPlane.push_back(core::plane3df());
		UserClipPlaneEnabled.push_back(false);
	}

	for (i=0; i<ETS_COUNT; ++i)
		setTransform(static_cast<E_TRANSFORMATION_STATE>(i), core::IdentityMatrix);

	setAmbientLight(SColorf(0.0f,0.0f,0.0f,0.0f));

    //by stone
#ifdef GL_EXT_separate_specular_color
	if (FeatureAvailable[IRR_EXT_separate_specular_color])
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif

    // TODO ogl-es
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	glClearDepthf(1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace( GL_CW );

	// create material renderers
	createMaterialRenderers();

	// set the renderstates
	setRenderStates3DMode();

	glAlphaFunc(GL_GREATER, 0.f);

	// set fog mode
	setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// create matrix for flipping textures
	TextureFlipMatrix.buildTextureTransform(0.0f, core::vector2df(0,0), core::vector2df(0,1.0f), core::vector2df(1.0f,-1.0f));

	// We need to reset once more at the beginning of the first rendering.
	// This fixes problems with intermediate changes to the material during texture load.
	ResetRenderStates = true;

	return true;
}

void COGLES1Driver::createMaterialRenderers()
{
	// create OGLES1 material renderers

	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_SOLID_2_LAYER(this));

	// add the same renderer for all lightmap types
	COGLES1MaterialRenderer_LIGHTMAP* lmr = new COGLES1MaterialRenderer_LIGHTMAP(this);
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_ADD:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M4:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M4:
	lmr->drop();

	// add remaining material renderer
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_DETAIL_MAP(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_SPHERE_MAP(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_REFLECTION_2_LAYER(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_TRANSPARENT_ADD_COLOR(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_TRANSPARENT_VERTEX_ALPHA(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(this));

	// add normal map renderers
// TODO ogl-es
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_SOLID(this));

	// add parallax map renderers
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_SOLID(this));

	// add basic 1 texture blending
	addAndDropMaterialRenderer(new COGLES1MaterialRenderer_ONETEXTURE_BLEND(this));
}


//! presents the rendered scene on the screen, returns false if failed
bool COGLES1Driver::endScene()
{
	CNullDriver::endScene();

/*
#if defined(EGL_VERSION_1_0)
 eglSwapBuffers(EglDisplay, EglSurface);
 EGLint g = eglGetError();
 if (EGL_SUCCESS != g)
 {
    if (EGL_CONTEXT_LOST == g)
    {
        // o-oh, ogl-es has lost contexts...
		os::Printer::log("Context lost, please restart your app.");
	}
	else
        os::Printer::log("Could not swap buffers for OpenGL-ES1 driver.");
 
    return false;
 }
#elif defined(GL_VERSION_ES_CM_1_0)
 glFlush();
 glBindRenderbufferOES(GL_RENDERBUFFER_OES, ViewRenderbuffer);
 #if defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
    Device->displayEnd();
 #endif
#endif
*/ 

	return true;
}


//! clears the zbuffer
bool COGLES1Driver::beginScene(bool backBuffer, bool zBuffer, SColor color,
		const SExposedVideoData& videoData,
		core::rect<s32>* sourceRect)
{
	CNullDriver::beginScene(backBuffer, zBuffer, color);
    
    setActiveTexture(0,0);
	ResetRenderStates = true;
/*
#if defined(GL_VERSION_ES_CM_1_0)
#if defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
    Device->displayBegin();
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, ViewFramebuffer);
#endif
#endif

	GLbitfield mask = 0;

	if (backBuffer)
	{
		const f32 inv = 1.0f / 255.0f;
		glClearColor(color.getRed() * inv, color.getGreen() * inv,
				color.getBlue() * inv, color.getAlpha() * inv);

		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (zBuffer)
	{
		glDepthMask(GL_TRUE);
		LastMaterial.ZWriteEnable=true;
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	glClear(mask);
 */
	return true;
}


//! Returns the transformation set by setTransform
const core::matrix4& COGLES1Driver::getTransform(E_TRANSFORMATION_STATE state) const
{
	return Matrices[state];
}


//! sets transformation
void COGLES1Driver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	Matrices[state] = mat;
	Transformation3DChanged = true;

	switch(state)
	{
	case ETS_VIEW:
	case ETS_WORLD:
		{
			// OGLES1 only has a model matrix, view and world is not existent. so lets fake these two.
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());
			// we have to update the clip planes to the latest view matrix
			for (u32 i=0; i<MaxUserClipPlanes; ++i)
				if (UserClipPlaneEnabled[i])
					uploadClipPlane(i);
		}
		break;
	case ETS_PROJECTION:
		{
			GLfloat glmat[16];
			createGLMatrix(glmat, mat);
			// flip z to compensate OGLES1s right-hand coordinate system
			glmat[12] *= -1.0f;
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(glmat);
            //RotateGLIfNeeded(); //by stone
		}
		break;
	case ETS_TEXTURE_0:
	case ETS_TEXTURE_1:
	case ETS_TEXTURE_2:
	case ETS_TEXTURE_3:
	{
		const u32 i = state - ETS_TEXTURE_0;
		if (i>= MaxTextureUnits)
			break;
		const bool isRTT = Material.getTexture(i) && Material.getTexture(i)->isRenderTarget();

		if (MultiTextureExtension)
			extGlActiveTexture(GL_TEXTURE0 + i);

		glMatrixMode(GL_TEXTURE);
		if (!isRTT && mat.isIdentity())
			glLoadIdentity();
		else
		{
			GLfloat glmat[16];
			if (isRTT)
				createGLTextureMatrix(glmat, mat * TextureFlipMatrix);
			else
				createGLTextureMatrix(glmat, mat);

			glLoadMatrixf(glmat);
		}
		break;
	}
	default:
		break;
	}
}

bool COGLES1Driver::updateVertexHardwareBuffer(SHWBufferLink_opengl *HWBuffer)
{
	if (!HWBuffer)
		return false;

	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
	const void* vertices=mb->getVertices();
	const u32 vertexCount=mb->getVertexCount();
	const E_VERTEX_TYPE vType=mb->getVertexType();
	const u32 vertexSize = getVertexPitchFromType(vType);

	//buffer vertex data, and convert colours...
	core::array<c8> buffer(vertexSize * vertexCount);
	memcpy(buffer.pointer(), vertices, vertexSize * vertexCount);

	// in order to convert the colors into opengl format (RGBA)
	switch (vType)
	{
		case EVT_STANDARD:
		{
			S3DVertex* pb = reinterpret_cast<S3DVertex*>(buffer.pointer());
			const S3DVertex* po = static_cast<const S3DVertex*>(vertices);
			for (u32 i=0; i<vertexCount; i++)
			{
				po[i].Color.toOpenGLColor((u8*)&(pb[i].Color.color));
			}
		}
		break;
		case EVT_2TCOORDS:
		{
			S3DVertex2TCoords* pb = reinterpret_cast<S3DVertex2TCoords*>(buffer.pointer());
			const S3DVertex2TCoords* po = static_cast<const S3DVertex2TCoords*>(vertices);
			for (u32 i=0; i<vertexCount; i++)
			{
				po[i].Color.toOpenGLColor((u8*)&(pb[i].Color.color));
			}
		}
		break;
		case EVT_TANGENTS:
		{
			S3DVertexTangents* pb = reinterpret_cast<S3DVertexTangents*>(buffer.pointer());
			const S3DVertexTangents* po = static_cast<const S3DVertexTangents*>(vertices);
			for (u32 i=0; i<vertexCount; i++)
			{
				po[i].Color.toOpenGLColor((u8*)&(pb[i].Color.color));
			}
		}
		break;
		default:
		{
			return false;
		}
	}

	//get or create buffer
	bool newBuffer=false;
	if (!HWBuffer->vbo_verticesID)
	{
		extGlGenBuffers(1, &HWBuffer->vbo_verticesID);
		if (!HWBuffer->vbo_verticesID) return false;
		newBuffer=true;
	}
	else if (HWBuffer->vbo_verticesSize < vertexCount*vertexSize)
	{
		newBuffer=true;
	}

	extGlBindBuffer(GL_ARRAY_BUFFER, HWBuffer->vbo_verticesID );

	// copy data to graphics card
	if (!newBuffer)
		extGlBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * vertexSize, buffer.const_pointer());
	else
	{
		HWBuffer->vbo_verticesSize = vertexCount*vertexSize;

		if (HWBuffer->Mapped_Vertex==scene::EHM_STATIC)
			extGlBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, buffer.const_pointer(), GL_STATIC_DRAW);
		else
			extGlBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, buffer.const_pointer(), GL_DYNAMIC_DRAW);
	}

	extGlBindBuffer(GL_ARRAY_BUFFER, 0);

	return (!testGLError());
}


bool COGLES1Driver::updateIndexHardwareBuffer(SHWBufferLink_opengl *HWBuffer)
{
	if (!HWBuffer)
		return false;

	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;

	const void* indices=mb->getIndices();
	u32 indexCount= mb->getIndexCount();

	GLenum indexSize;
	switch (mb->getIndexType())
	{
		case (EIT_16BIT):
		{
			indexSize=sizeof(u16);
			break;
		}
		case (EIT_32BIT):
		{
			indexSize=sizeof(u32);
			break;
		}
		default:
		{
			return false;
		}
	}


	//get or create buffer
	bool newBuffer=false;
	if (!HWBuffer->vbo_indicesID)
	{
		extGlGenBuffers(1, &HWBuffer->vbo_indicesID);
		if (!HWBuffer->vbo_indicesID) return false;
		newBuffer=true;
	}
	else if (HWBuffer->vbo_indicesSize < indexCount*indexSize)
	{
		newBuffer=true;
	}

	extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HWBuffer->vbo_indicesID);

	// copy data to graphics card
	if (!newBuffer)
		extGlBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexCount * indexSize, indices);
	else
	{
		HWBuffer->vbo_indicesSize = indexCount*indexSize;

		if (HWBuffer->Mapped_Index==scene::EHM_STATIC)
			extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_STATIC_DRAW);
		else
			extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_DYNAMIC_DRAW);
	}

	extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return (!testGLError());
}


//! updates hardware buffer if needed
bool COGLES1Driver::updateHardwareBuffer(SHWBufferLink *HWBuffer)
{
	if (!HWBuffer)
		return false;

	if (HWBuffer->Mapped_Vertex!=scene::EHM_NEVER)
	{
		if (HWBuffer->ChangedID_Vertex != HWBuffer->MeshBuffer->getChangedID_Vertex()
			|| !((SHWBufferLink_opengl*)HWBuffer)->vbo_verticesID)
		{

			HWBuffer->ChangedID_Vertex = HWBuffer->MeshBuffer->getChangedID_Vertex();

			if (!updateVertexHardwareBuffer((SHWBufferLink_opengl*)HWBuffer))
				return false;
		}
	}

	if (HWBuffer->Mapped_Index!=scene::EHM_NEVER)
	{
		if (HWBuffer->ChangedID_Index != HWBuffer->MeshBuffer->getChangedID_Index()
			|| !((SHWBufferLink_opengl*)HWBuffer)->vbo_indicesID)
		{

			HWBuffer->ChangedID_Index = HWBuffer->MeshBuffer->getChangedID_Index();

			if (!updateIndexHardwareBuffer((SHWBufferLink_opengl*)HWBuffer))
				return false;
		}
	}

	return true;
}


//! Create hardware buffer from meshbuffer
COGLES1Driver::SHWBufferLink *COGLES1Driver::createHardwareBuffer(const scene::IMeshBuffer* mb)
{
	if (!mb || (mb->getHardwareMappingHint_Index()==scene::EHM_NEVER && mb->getHardwareMappingHint_Vertex()==scene::EHM_NEVER))
		return 0;

	SHWBufferLink_opengl *HWBuffer=new SHWBufferLink_opengl(mb);

	//add to map
	HWBufferMap.insert(HWBuffer->MeshBuffer, HWBuffer);

	HWBuffer->ChangedID_Vertex=HWBuffer->MeshBuffer->getChangedID_Vertex();
	HWBuffer->ChangedID_Index=HWBuffer->MeshBuffer->getChangedID_Index();
	HWBuffer->Mapped_Vertex=mb->getHardwareMappingHint_Vertex();
	HWBuffer->Mapped_Index=mb->getHardwareMappingHint_Index();
	HWBuffer->LastUsed=0;
	HWBuffer->vbo_verticesID=0;
	HWBuffer->vbo_indicesID=0;
	HWBuffer->vbo_verticesSize=0;
	HWBuffer->vbo_indicesSize=0;

	if (!updateHardwareBuffer(HWBuffer))
	{
		deleteHardwareBuffer(HWBuffer);
		return 0;
	}

	return HWBuffer;
}


void COGLES1Driver::deleteHardwareBuffer(SHWBufferLink *_HWBuffer)
{
	if (!_HWBuffer) 
		return;

	SHWBufferLink_opengl *HWBuffer=(SHWBufferLink_opengl*)_HWBuffer;
	if (HWBuffer->vbo_verticesID)
	{
		extGlDeleteBuffers(1, &HWBuffer->vbo_verticesID);
		HWBuffer->vbo_verticesID=0;
	}
	if (HWBuffer->vbo_indicesID)
	{
		extGlDeleteBuffers(1, &HWBuffer->vbo_indicesID);
		HWBuffer->vbo_indicesID=0;
	}

	CNullDriver::deleteHardwareBuffer(_HWBuffer);
}


//! Draw hardware buffer
void COGLES1Driver::drawHardwareBuffer(SHWBufferLink *_HWBuffer)
{
	if (!_HWBuffer)
		return;

	SHWBufferLink_opengl *HWBuffer=(SHWBufferLink_opengl*)_HWBuffer;

	updateHardwareBuffer(HWBuffer); //check if update is needed

	HWBuffer->LastUsed=0;//reset count

	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
	const void *vertices=mb->getVertices();
	const void *indexList=mb->getIndices();

	if (HWBuffer->Mapped_Vertex!=scene::EHM_NEVER)
	{
		extGlBindBuffer(GL_ARRAY_BUFFER, HWBuffer->vbo_verticesID);
		vertices=0;
	}

	if (HWBuffer->Mapped_Index!=scene::EHM_NEVER)
	{
		extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HWBuffer->vbo_indicesID);
		indexList=0;
	}


	drawVertexPrimitiveList(vertices, mb->getVertexCount(), indexList,
			mb->getIndexCount()/3, mb->getVertexType(),
			scene::EPT_TRIANGLES, mb->getIndexType());

	if (HWBuffer->Mapped_Vertex!=scene::EHM_NEVER)
		extGlBindBuffer(GL_ARRAY_BUFFER, 0);

	if (HWBuffer->Mapped_Index!=scene::EHM_NEVER)
		extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


// small helper function to create vertex buffer object adress offsets
static inline u8* buffer_offset(const long offset)
{
	return ((u8*)0 + offset);
}


//! draws a vertex primitive list
void COGLES1Driver::drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
		const void* indexList, u32 primitiveCount,
		E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if (!checkPrimitiveCount(primitiveCount))
		return;

	setRenderStates3DMode();

	drawVertexPrimitiveList2d3d(vertices, vertexCount, (const u16*)indexList, primitiveCount, vType, pType, iType);
}


void COGLES1Driver::drawVertexPrimitiveList2d3d(const void* vertices, u32 vertexCount,
		const void* indexList, u32 primitiveCount,
		E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType, bool threed)
{
	if (!primitiveCount || !vertexCount)
		return;

	if (!threed && !checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::drawVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType);

	if (vertices)
	{
		// convert colors to gl color format.
		vertexCount *= 4; //reused as color component count
		ColorBuffer.set_used(vertexCount);
		u32 i;

		switch (vType)
		{
			case EVT_STANDARD:
			{
				const S3DVertex* p = static_cast<const S3DVertex*>(vertices);
				for ( i=0; i<vertexCount; i+=4)
				{
					p->Color.toOpenGLColor(&ColorBuffer[i]);
					++p;
				}
			}
			break;
			case EVT_2TCOORDS:
			{
				const S3DVertex2TCoords* p = static_cast<const S3DVertex2TCoords*>(vertices);
				for ( i=0; i<vertexCount; i+=4)
				{
					p->Color.toOpenGLColor(&ColorBuffer[i]);
					++p;
				}
			}
			break;
			case EVT_TANGENTS:
			{
				const S3DVertexTangents* p = static_cast<const S3DVertexTangents*>(vertices);
				for ( i=0; i<vertexCount; i+=4)
				{
					p->Color.toOpenGLColor(&ColorBuffer[i]);
					++p;
				}
			}
			break;
		}
	}

	// draw everything

	if (MultiTextureExtension)
		extGlClientActiveTexture(GL_TEXTURE0);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	if ((pType!=scene::EPT_POINTS) && (pType!=scene::EPT_POINT_SPRITES))
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#ifdef GL_OES_point_size_array
	else if (FeatureAvailable[IRR_OES_point_size_array] && (Material.Thickness==0.0f))
		glEnableClientState(GL_POINT_SIZE_ARRAY_OES);
#endif
	if (threed && (pType!=scene::EPT_POINTS) && (pType!=scene::EPT_POINT_SPRITES))
		glEnableClientState(GL_NORMAL_ARRAY);

	if (vertices)
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);

	switch (vType)
	{
		case EVT_STANDARD:
			if (vertices)
			{
#ifdef GL_OES_point_size_array
				if ((pType==scene::EPT_POINTS) || (pType==scene::EPT_POINT_SPRITES))
				{
					if (FeatureAvailable[IRR_OES_point_size_array] && (Material.Thickness==0.0f))
						glPointSizePointerOES(GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Normal.X);
				}
				else
#endif
				if (threed)
					glNormalPointer(GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Normal);
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].TCoords);
				glVertexPointer((threed?3:2), GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Pos);
			}
			else
			{
				glNormalPointer(GL_FLOAT, sizeof(S3DVertex), buffer_offset(12));
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(S3DVertex), buffer_offset(24));
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), buffer_offset(28));
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex), 0);
			}

			if (MultiTextureExtension && CurrentTexture[1])
			{
				extGlClientActiveTexture(GL_TEXTURE1);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (vertices)
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].TCoords);
				else
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), buffer_offset(28));
			}
			break;
		case EVT_2TCOORDS:
			if (vertices)
			{
				if (threed)
					glNormalPointer(GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Normal);
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords);
				glVertexPointer((threed?3:2), GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].Pos);
			}
			else
			{
				glNormalPointer(GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(12));
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(S3DVertex2TCoords), buffer_offset(24));
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(28));
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(0));
			}


			if (MultiTextureExtension)
			{
				extGlClientActiveTexture(GL_TEXTURE1);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (vertices)
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &(static_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords2);
				else
					glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), buffer_offset(36));
			}
			break;
		case EVT_TANGENTS:
			if (vertices)
			{
				if (threed)
					glNormalPointer(GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Normal);
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].TCoords);
				glVertexPointer((threed?3:2), GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Pos);
			}
			else
			{
				glNormalPointer(GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(12));
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(S3DVertexTangents), buffer_offset(24));
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(28));
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(0));
			}

			if (MultiTextureExtension)
			{
				extGlClientActiveTexture(GL_TEXTURE1);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (vertices)
					glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Tangent);
				else
					glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(36));

				extGlClientActiveTexture(GL_TEXTURE2);
				glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
				if (vertices)
					glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &(static_cast<const S3DVertexTangents*>(vertices))[0].Binormal);
				else
					glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), buffer_offset(48));
			}
			break;
	}

	GLenum indexSize=0;

	switch (iType)
	{
		case (EIT_16BIT):
		{
			indexSize=GL_UNSIGNED_SHORT;
			break;
		}
		case (EIT_32BIT):
		{
#ifdef GL_OES_element_index_uint
#ifndef GL_UNSIGNED_INT
#define GL_UNSIGNED_INT                   0x1405
#endif
			if (FeatureAvailable[IRR_OES_element_index_uint])
				indexSize=GL_UNSIGNED_INT;
			else
#endif
			indexSize=GL_UNSIGNED_SHORT;
			break;
		}
	}

	switch (pType)
	{
		case scene::EPT_POINTS:
		case scene::EPT_POINT_SPRITES:
		{
#ifdef GL_OES_point_sprite
			if (pType==scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_OES_point_sprite])
				glEnable(GL_POINT_SPRITE_OES);
#endif
			// if ==0 we use the point size array
			if (Material.Thickness!=0.f)
			{
				float quadratic[] = {0.0f, 0.0f, 10.01f};
				extGlPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, quadratic);
				float maxParticleSize=1.0f;
				glGetFloatv(GL_POINT_SIZE_MAX, &maxParticleSize);
//				maxParticleSize=maxParticleSize<Material.Thickness?maxParticleSize:Material.Thickness;
//				extGlPointParameterf(GL_POINT_SIZE_MAX,maxParticleSize);
//				extGlPointParameterf(GL_POINT_SIZE_MIN,Material.Thickness);
				extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 60.0f);
				glPointSize(Material.Thickness);
			}
#ifdef GL_OES_point_sprite
			if (pType==scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_OES_point_sprite])
				glTexEnvf(GL_POINT_SPRITE_OES,GL_COORD_REPLACE_OES, GL_TRUE);
#endif
			glDrawArrays(GL_POINTS, 0, primitiveCount);
#ifdef GL_OES_point_sprite
			if (pType==scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_OES_point_sprite])
			{
				glDisable(GL_POINT_SPRITE_OES);
				glTexEnvf(GL_POINT_SPRITE_OES,GL_COORD_REPLACE_OES, GL_FALSE);
			}
#endif
		}
			break;
		case scene::EPT_LINE_STRIP:
			glDrawElements(GL_LINE_STRIP, primitiveCount+1, indexSize, indexList);
			break;
		case scene::EPT_LINE_LOOP:
			glDrawElements(GL_LINE_LOOP, primitiveCount, indexSize, indexList);
			break;
		case scene::EPT_LINES:
			glDrawElements(GL_LINES, primitiveCount*2, indexSize, indexList);
			break;
		case scene::EPT_TRIANGLE_STRIP:
			glDrawElements(GL_TRIANGLE_STRIP, primitiveCount+2, indexSize, indexList);
			break;
		case scene::EPT_TRIANGLE_FAN:
			glDrawElements(GL_TRIANGLE_FAN, primitiveCount+2, indexSize, indexList);
			break;
		case scene::EPT_TRIANGLES:
			glDrawElements((LastMaterial.Wireframe)?GL_LINES:(LastMaterial.PointCloud)?GL_POINTS:GL_TRIANGLES, primitiveCount*3, indexSize, indexList);
			break;
		case scene::EPT_QUAD_STRIP:
// TODO ogl-es
//			glDrawElements(GL_QUAD_STRIP, primitiveCount*2+2, indexSize, indexList);
			break;
		case scene::EPT_QUADS:
// TODO ogl-es
//			glDrawElements(GL_QUADS, primitiveCount*4, indexSize, indexList);
			break;
		case scene::EPT_POLYGON:
// TODO ogl-es
//			glDrawElements(GL_POLYGON, primitiveCount, indexSize, indexList);
			break;
	}

	if (MultiTextureExtension)
	{
		if (vType==EVT_TANGENTS)
		{
			extGlClientActiveTexture(GL_TEXTURE2);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if ((vType!=EVT_STANDARD) || CurrentTexture[1])
		{
			extGlClientActiveTexture(GL_TEXTURE1);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		extGlClientActiveTexture(GL_TEXTURE0);
	}
#ifdef GL_OES_point_size_array
	if (FeatureAvailable[IRR_OES_point_size_array] && (Material.Thickness==0.0f))
		glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
#endif
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


//! draws a 2d image, using a color and the alpha channel of the texture
void COGLES1Driver::draw2DImage(const video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::rect<s32>& sourceRect,
				const core::rect<s32>* clipRect, SColor color,
				bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	if (!sourceRect.isValid())
		return;

#if defined(GL_OES_draw_texture)
	// currently disabled due to problems with the emulator.
	if (false && FeatureAvailable[IRR_OES_draw_texture])
	{
		disableTextures(1);
		if (!setActiveTexture(0, texture))
			return;
		setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);
		const GLint crop[] = {sourceRect.UpperLeftCorner.X, static_cast<GLint>(getCurrentRenderTargetSize().Height-sourceRect.LowerRightCorner.Y), sourceRect.getWidth(), sourceRect.getHeight()};
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);

		const bool isRTT = true;//texture->isRenderTarget();
		if (!isRTT)
		{
			glMatrixMode(GL_TEXTURE);
			GLfloat glmat[16];
			createGLTextureMatrix(glmat, TextureFlipMatrix);
			glLoadMatrixf(glmat);
		}
		core::rect<s32> destRect(pos,sourceRect.getSize());
		if (clipRect)
			destRect.clipAgainst(*clipRect);
		extGlDrawTex(destRect.UpperLeftCorner.X, getCurrentRenderTargetSize().Height-destRect.UpperLeftCorner.Y, 0, destRect.getWidth(), destRect.getHeight());
		return;
	}
#endif
	core::position2d<s32> targetPos(pos);
	core::position2d<s32> sourcePos(sourceRect.UpperLeftCorner);
	core::dimension2d<s32> sourceSize(sourceRect.getSize());
	if (clipRect)
	{
		if (targetPos.X < clipRect->UpperLeftCorner.X)
		{
			sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
			if (sourceSize.Width <= 0)
				return;

			sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
			targetPos.X = clipRect->UpperLeftCorner.X;
		}

		if (targetPos.X + sourceSize.Width > clipRect->LowerRightCorner.X)
		{
			sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
			if (sourceSize.Width <= 0)
				return;
		}

		if (targetPos.Y < clipRect->UpperLeftCorner.Y)
		{
			sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
			if (sourceSize.Height <= 0)
				return;

			sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
			targetPos.Y = clipRect->UpperLeftCorner.Y;
		}

		if (targetPos.Y + sourceSize.Height > clipRect->LowerRightCorner.Y)
		{
			sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
			if (sourceSize.Height <= 0)
				return;
		}
	}

	// clip these coordinates

	if (targetPos.X<0)
	{
		sourceSize.Width += targetPos.X;
		if (sourceSize.Width <= 0)
			return;

		sourcePos.X -= targetPos.X;
		targetPos.X = 0;
	}

	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

	if (targetPos.X + sourceSize.Width > (s32)renderTargetSize.Width)
	{
		sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
		if (sourceSize.Width <= 0)
			return;
	}

	if (targetPos.Y<0)
	{
		sourceSize.Height += targetPos.Y;
		if (sourceSize.Height <= 0)
			return;

		sourcePos.Y -= targetPos.Y;
		targetPos.Y = 0;
	}

	if (targetPos.Y + sourceSize.Height > (s32)renderTargetSize.Height)
	{
		sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
		if (sourceSize.Height <= 0)
			return;
	}

	// ok, we've clipped everything.
	// now draw it.

	// texcoords need to be flipped horizontally for RTTs
	const bool isRTT = texture->isRenderTarget();
	const core::dimension2d<u32>& ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	const core::rect<f32> tcoords(
			sourcePos.X * invW,
			(isRTT?(sourcePos.Y + sourceSize.Height):sourcePos.Y) * invH,
			(sourcePos.X + sourceSize.Width) * invW,
			(isRTT?sourcePos.Y:(sourcePos.Y + sourceSize.Height)) * invH);

	const core::rect<s32> poss(targetPos, sourceSize);

	disableTextures(1);
	if (!setActiveTexture(0, texture))
		return;
	setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);

	u16 indices[] = {0,1,2,3};
	S3DVertex vertices[4];
	vertices[0] = S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0,0,1, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	vertices[1] = S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0,0,1, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	vertices[2] = S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0,0,1, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	vertices[3] = S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0,0,1, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
	drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);
}


//! The same, but with a four element array of colors, one for each vertex
void COGLES1Driver::draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
		const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
		const video::SColor* const colors, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	// texcoords need to be flipped horizontally for RTTs
	const bool isRTT = texture->isRenderTarget();
	const core::dimension2du& ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	const core::rect<f32> tcoords(
			sourceRect.UpperLeftCorner.X * invW,
			(isRTT?sourceRect.LowerRightCorner.Y:sourceRect.UpperLeftCorner.Y) * invH,
			sourceRect.LowerRightCorner.X * invW,
			(isRTT?sourceRect.UpperLeftCorner.Y:sourceRect.LowerRightCorner.Y) *invH);

	const video::SColor temp[4] =
	{
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF
	};

	const video::SColor* const useColor = colors ? colors : temp;

	disableTextures(1);
	setActiveTexture(0, texture);
	setRenderStates2DMode(useColor[0].getAlpha()<255 || useColor[1].getAlpha()<255 ||
			useColor[2].getAlpha()<255 || useColor[3].getAlpha()<255,
			true, useAlphaChannelOfTexture);

	if (clipRect)
	{
		if (!clipRect->isValid())
			return;

		glEnable(GL_SCISSOR_TEST);
		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
		glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height-clipRect->LowerRightCorner.Y,
			clipRect->getWidth(), clipRect->getHeight());
	}

	u16 indices[] = {0,1,2,3};
	S3DVertex vertices[4];
	vertices[0] = S3DVertex((f32)destRect.UpperLeftCorner.X, (f32)destRect.UpperLeftCorner.Y, 0, 0,0,1, useColor[0], tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	vertices[1] = S3DVertex((f32)destRect.LowerRightCorner.X, (f32)destRect.UpperLeftCorner.Y, 0, 0,0,1, useColor[3], tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	vertices[2] = S3DVertex((f32)destRect.LowerRightCorner.X, (f32)destRect.LowerRightCorner.Y, 0, 0,0,1, useColor[2], tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	vertices[3] = S3DVertex((f32)destRect.UpperLeftCorner.X, (f32)destRect.LowerRightCorner.Y, 0, 0,0,1, useColor[1], tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
	drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}


//! draws a set of 2d images, using a color and the alpha channel
void COGLES1Driver::draw2DImage(const video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::array<core::rect<s32> >& sourceRects,
				const core::array<s32>& indices,
				const core::rect<s32>* clipRect, SColor color,
				bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	disableTextures(1);
	if (!setActiveTexture(0, texture))
		return;
	setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);

	if (clipRect)
	{
		if (!clipRect->isValid())
			return;

		glEnable(GL_SCISSOR_TEST);
		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
		glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height-clipRect->LowerRightCorner.Y,
			clipRect->getWidth(),clipRect->getHeight());
	}

	const core::dimension2du& ss = texture->getOriginalSize();
	core::position2d<s32> targetPos(pos);
	// texcoords need to be flipped horizontally for RTTs
	const bool isRTT = texture->isRenderTarget();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);

	core::array<S3DVertex> vertices;
	core::array<u16> quadIndices;
	vertices.reallocate(indices.size()*4);
	quadIndices.reallocate(indices.size()*6);
	for (u32 i=0; i<indices.size(); ++i)
	{
		const s32 currentIndex = indices[i];
		if (!sourceRects[currentIndex].isValid())
			break;

		const core::rect<f32> tcoords(
				sourceRects[currentIndex].UpperLeftCorner.X * invW,
				(isRTT?sourceRects[currentIndex].LowerRightCorner.Y:sourceRects[currentIndex].UpperLeftCorner.Y) * invH,
				sourceRects[currentIndex].LowerRightCorner.X * invW,
				(isRTT?sourceRects[currentIndex].UpperLeftCorner.Y:sourceRects[currentIndex].LowerRightCorner.Y) * invH);

		const core::rect<s32> poss(targetPos, sourceRects[currentIndex].getSize());

		const u32 vstart = vertices.size();

		vertices.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0,0,1, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));
		vertices.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0,0,1, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));
		vertices.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0,0,1, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));
		vertices.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0,0,1, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));

		quadIndices.push_back(vstart);
		quadIndices.push_back(vstart+1);
		quadIndices.push_back(vstart+2);
		quadIndices.push_back(vstart);
		quadIndices.push_back(vstart+2);
		quadIndices.push_back(vstart+3);

		targetPos.X += sourceRects[currentIndex].getWidth();
	}
	if (vertices.size())
		drawVertexPrimitiveList2d3d(vertices.pointer(), vertices.size(),
				quadIndices.pointer(), vertices.size()/2,
				video::EVT_STANDARD, scene::EPT_TRIANGLES,
				EIT_16BIT, false);
	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}


//! draws a set of 2d images, using a color and the alpha channel of the texture if desired.
void COGLES1Driver::draw2DImageBatch(const video::ITexture* texture,
				const core::array<core::position2d<s32> >& positions,
				const core::array<core::rect<s32> >& sourceRects,
				const core::rect<s32>* clipRect,
				SColor color,
				bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	const u32 drawCount = core::min_<u32>(positions.size(), sourceRects.size());
	if (!drawCount)
		return;

	const core::dimension2d<u32>& ss = texture->getOriginalSize();
	if (!ss.Width || !ss.Height)
		return;
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

	disableTextures(1);
	if (!setActiveTexture(0, texture))
		return;
	setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);

	core::array<S3DVertex> vertices;
	core::array<u16> quadIndices;
	vertices.reallocate(drawCount*4);
	quadIndices.reallocate(drawCount*6);

	for (u32 i=0; i<drawCount; ++i)
	{
		if (!sourceRects[i].isValid())
			continue;

		core::position2d<s32> targetPos(positions[i]);
		core::position2d<s32> sourcePos(sourceRects[i].UpperLeftCorner);
		// This needs to be signed as it may go negative.
		core::dimension2d<s32> sourceSize(sourceRects[i].getSize());
		if (clipRect)
		{
			if (targetPos.X < clipRect->UpperLeftCorner.X)
			{
				sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
				if (sourceSize.Width <= 0)
					continue;

				sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
				targetPos.X = clipRect->UpperLeftCorner.X;
			}

			if (targetPos.X + sourceSize.Width > clipRect->LowerRightCorner.X)
			{
				sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
				if (sourceSize.Width <= 0)
					continue;
			}

			if (targetPos.Y < clipRect->UpperLeftCorner.Y)
			{
				sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
				if (sourceSize.Height <= 0)
					continue;

				sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
				targetPos.Y = clipRect->UpperLeftCorner.Y;
			}

			if (targetPos.Y + sourceSize.Height > clipRect->LowerRightCorner.Y)
			{
				sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
				if (sourceSize.Height <= 0)
					continue;
			}
		}

		// clip these coordinates

		if (targetPos.X<0)
		{
			sourceSize.Width += targetPos.X;
			if (sourceSize.Width <= 0)
				continue;

			sourcePos.X -= targetPos.X;
			targetPos.X = 0;
		}

		if (targetPos.X + sourceSize.Width > (s32)renderTargetSize.Width)
		{
			sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
			if (sourceSize.Width <= 0)
				continue;
		}

		if (targetPos.Y<0)
		{
			sourceSize.Height += targetPos.Y;
			if (sourceSize.Height <= 0)
				continue;

			sourcePos.Y -= targetPos.Y;
			targetPos.Y = 0;
		}

		if (targetPos.Y + sourceSize.Height > (s32)renderTargetSize.Height)
		{
			sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
			if (sourceSize.Height <= 0)
				continue;
		}

		// ok, we've clipped everything.

		const core::rect<f32> tcoords(
				sourcePos.X * invW,
				sourcePos.Y * invH,
				(sourcePos.X + sourceSize.Width) * invW,
				(sourcePos.Y + sourceSize.Height) * invH);

		const core::rect<s32> poss(targetPos, sourceSize);

		const u32 vstart = vertices.size();

		vertices.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0,0,1, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));
		vertices.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0, 0,0,1, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));
		vertices.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0,0,1, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));
		vertices.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0, 0,0,1, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));

		quadIndices.push_back(vstart);
		quadIndices.push_back(vstart+1);
		quadIndices.push_back(vstart+2);
		quadIndices.push_back(vstart);
		quadIndices.push_back(vstart+2);
		quadIndices.push_back(vstart+3);
	}
	if (vertices.size())
		drawVertexPrimitiveList2d3d(vertices.pointer(), vertices.size(),
				quadIndices.pointer(), vertices.size()/2,
				video::EVT_STANDARD, scene::EPT_TRIANGLES,
				EIT_16BIT, false);
}


//! draw a 2d rectangle
void COGLES1Driver::draw2DRectangle(SColor color, const core::rect<s32>& position,
		const core::rect<s32>* clip)
{
	disableTextures();
	setRenderStates2DMode(color.getAlpha() < 255, false, false);

	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	u16 indices[] = {0,1,2,3};
	S3DVertex vertices[4];
	vertices[0] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0,0,1, color, 0,0);
	vertices[1] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0,0,1, color, 0,0);
	vertices[2] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0,0,1, color, 0,0);
	vertices[3] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0,0,1, color, 0,0);
	drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);
}


//! draw an 2d rectangle
void COGLES1Driver::draw2DRectangle(const core::rect<s32>& position,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32>* clip)
{
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	disableTextures();

	setRenderStates2DMode(colorLeftUp.getAlpha() < 255 ||
		colorRightUp.getAlpha() < 255 ||
		colorLeftDown.getAlpha() < 255 ||
		colorRightDown.getAlpha() < 255, false, false);

	u16 indices[] = {0,1,2,3};
	S3DVertex vertices[4];
	vertices[0] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0,0,1, colorLeftUp, 0,0);
	vertices[1] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.UpperLeftCorner.Y, 0, 0,0,1, colorRightUp, 0,0);
	vertices[2] = S3DVertex((f32)pos.LowerRightCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0,0,1, colorRightDown, 0,0);
	vertices[3] = S3DVertex((f32)pos.UpperLeftCorner.X, (f32)pos.LowerRightCorner.Y, 0, 0,0,1, colorLeftDown, 0,0);
	drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);
}


//! Draws a 2d line.
void COGLES1Driver::draw2DLine(const core::position2d<s32>& start,
				const core::position2d<s32>& end,
				SColor color)
{
	disableTextures();
	setRenderStates2DMode(color.getAlpha() < 255, false, false);

	u16 indices[] = {0,1};
	S3DVertex vertices[2];
	vertices[0] = S3DVertex((f32)start.X, (f32)start.Y, 0, 0,0,1, color, 0,0);
	vertices[1] = S3DVertex((f32)end.X, (f32)end.Y, 0, 0,0,1, color, 1,1);
	drawVertexPrimitiveList2d3d(vertices, 2, indices, 1, video::EVT_STANDARD, scene::EPT_LINES, EIT_16BIT, false);
}


//! Draws a pixel
void COGLES1Driver::drawPixel(u32 x, u32 y, const SColor &color)
{
	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
	if (x > (u32)renderTargetSize.Width || y > (u32)renderTargetSize.Height)
		return;

	disableTextures();
	setRenderStates2DMode(color.getAlpha() < 255, false, false);

	u16 indices[] = {0};
	S3DVertex vertices[1];
	vertices[0] = S3DVertex((f32)x, (f32)y, 0, 0, 0, 1, color, 0, 0);
	drawVertexPrimitiveList2d3d(vertices, 1, indices, 1, video::EVT_STANDARD, scene::EPT_POINTS, EIT_16BIT, false);
}


bool COGLES1Driver::setActiveTexture(u32 stage, const video::ITexture* texture)
{
	if (stage >= MaxTextureUnits)
		return false;

	if (CurrentTexture[stage]==texture)
		return true;

	if (MultiTextureExtension)
		extGlActiveTexture(GL_TEXTURE0 + stage);

	CurrentTexture.set(stage,texture);

	if (!texture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		return true;
	}
	else
	{
		if (texture->getDriverType() != EDT_OGLES1)
		{
			CurrentTexture.set(stage, 0);
			glDisable(GL_TEXTURE_2D);
			os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
			return false;
		}

		glEnable(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D,
		//	static_cast<const COGLES1Texture*>(texture)->getOGLES1TextureName());

        //SETH, tweaked below after I changed constness which was needed for dynamic bad surface loading
		glBindTexture(GL_TEXTURE_2D, ((COGLES1Texture*)texture)->getOGLES1TextureName());
	}
	return true;
}


//! disables all textures beginning with the optional fromStage parameter.
bool COGLES1Driver::disableTextures(u32 fromStage)
{
	bool result=true;
	for (u32 i=fromStage; i<MaxTextureUnits; ++i)
		result &= setActiveTexture(i, 0);
	return result;
}


//! creates a matrix in supplied GLfloat array to pass to OGLES1
inline void COGLES1Driver::createGLMatrix(GLfloat gl_matrix[16], const core::matrix4& m)
{
	memcpy(gl_matrix, m.pointer(), 16 * sizeof(f32));
}


//! creates a opengltexturematrix from a D3D style texture matrix
inline void COGLES1Driver::createGLTextureMatrix(GLfloat *o, const core::matrix4& m)
{
	o[0] = m[0];
	o[1] = m[1];
	o[2] = 0.f;
	o[3] = 0.f;

	o[4] = m[4];
	o[5] = m[5];
	o[6] = 0.f;
	o[7] = 0.f;

	o[8] = 0.f;
	o[9] = 0.f;
	o[10] = 1.f;
	o[11] = 0.f;

	o[12] = m[8];
	o[13] = m[9];
	o[14] = 0.f;
	o[15] = 1.f;
}


//! returns a device dependent texture from a software surface (IImage)
video::ITexture* COGLES1Driver::createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData)
{
	COGLES1Texture* texture = 0;

	if (surface && checkColorFormat(surface->getColorFormat(), surface->getDimension()))
		texture = new COGLES1Texture(surface, name, this, mipmapData);

	return texture;
}


//! Sets a material.
void COGLES1Driver::setMaterial(const SMaterial& material)
{
	Material = material;
	OverrideMaterial.apply(Material);

	for (s32 i = MaxTextureUnits-1; i>= 0; --i)
	{
		setTransform ((E_TRANSFORMATION_STATE) ( ETS_TEXTURE_0 + i ), Material.getTextureMatrix(i));
	}
}


//! prints error if an error happened.
bool COGLES1Driver::testGLError()
{
#ifdef _DEBUG
	GLenum g = glGetError();
	switch(g)
	{
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		os::Printer::log("GL_INVALID_ENUM", ELL_ERROR); break;
	case GL_INVALID_VALUE:
		os::Printer::log("GL_INVALID_VALUE", ELL_ERROR); break;
	case GL_INVALID_OPERATION:
		os::Printer::log("GL_INVALID_OPERATION", ELL_ERROR); break;
	case GL_STACK_OVERFLOW:
		os::Printer::log("GL_STACK_OVERFLOW", ELL_ERROR); break;
	case GL_STACK_UNDERFLOW:
		os::Printer::log("GL_STACK_UNDERFLOW", ELL_ERROR); break;
	case GL_OUT_OF_MEMORY:
		os::Printer::log("GL_OUT_OF_MEMORY", ELL_ERROR); break;
	};
//	_IRR_DEBUG_BREAK_IF(true);
	return true;
#else
	return false;
#endif
}


bool COGLES1Driver::testEGLError()
{
#if defined(EGL_VERSION_1_0) && defined(_DEBUG)
	EGLint g = eglGetError();
	switch (g)
	{
		case EGL_SUCCESS: return false;
		case EGL_NOT_INITIALIZED :
			os::Printer::log("Not Initialized", ELL_ERROR); break;
		case EGL_BAD_ACCESS:
			os::Printer::log("Bad Access", ELL_ERROR); break;
		case EGL_BAD_ALLOC:
			os::Printer::log("Bad Alloc", ELL_ERROR); break;
		case EGL_BAD_ATTRIBUTE:
			os::Printer::log("Bad Attribute", ELL_ERROR); break;
		case EGL_BAD_CONTEXT:
			os::Printer::log("Bad Context", ELL_ERROR); break;
		case EGL_BAD_CONFIG:
			os::Printer::log("Bad Config", ELL_ERROR); break;
		case EGL_BAD_CURRENT_SURFACE:
			os::Printer::log("Bad Current Surface", ELL_ERROR); break;
		case EGL_BAD_DISPLAY:
			os::Printer::log("Bad Display", ELL_ERROR); break;
		case EGL_BAD_SURFACE:
			os::Printer::log("Bad Surface", ELL_ERROR); break;
		case EGL_BAD_MATCH:
			os::Printer::log("Bad Match", ELL_ERROR); break;
		case EGL_BAD_PARAMETER:
			os::Printer::log("Bad Parameter", ELL_ERROR); break;
		case EGL_BAD_NATIVE_PIXMAP:
			os::Printer::log("Bad Native Pixmap", ELL_ERROR); break;
		case EGL_BAD_NATIVE_WINDOW:
			os::Printer::log("Bad Native Window", ELL_ERROR); break;
		case EGL_CONTEXT_LOST:
			os::Printer::log("Context Lost", ELL_ERROR); break;
	};
	return true;
#else
	return false;
#endif
}


//! sets the needed renderstates
void COGLES1Driver::setRenderStates3DMode()
{
	if (CurrentRenderMode != ERM_3D)
	{
		// Reset Texture Stages
		BridgeCalls->setBlend(false);
		glDisable(GL_ALPHA_TEST);
		BridgeCalls->setBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

		// switch back the matrices
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());

		GLfloat glmat[16];
		createGLMatrix(glmat, Matrices[ETS_PROJECTION]);
		glmat[12] *= -1.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glmat);

		ResetRenderStates = true;
	}

	if ( ResetRenderStates || LastMaterial != Material)
	{
		// unset old material

		if (LastMaterial.MaterialType != Material.MaterialType &&
				static_cast<u32>(LastMaterial.MaterialType) < MaterialRenderers.size())
			MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();

		// set new material.
		if (static_cast<u32>(Material.MaterialType) < MaterialRenderers.size())
			MaterialRenderers[Material.MaterialType].Renderer->OnSetMaterial(
				Material, LastMaterial, ResetRenderStates, this);

		LastMaterial = Material;
		ResetRenderStates = false;
	}

	if (static_cast<u32>(Material.MaterialType) < MaterialRenderers.size())
		MaterialRenderers[Material.MaterialType].Renderer->OnRender(this, video::EVT_STANDARD);

	CurrentRenderMode = ERM_3D;
}


GLint COGLES1Driver::getTextureWrapMode(u8 clamp) const
{
	switch (clamp)
	{
		case ETC_CLAMP:
			//	return GL_CLAMP; not supported in ogl-es
			return GL_CLAMP_TO_EDGE;
			break;
		case ETC_CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
			break;
		case ETC_CLAMP_TO_BORDER:
			//	return GL_CLAMP_TO_BORDER; not supported in ogl-es
			return GL_CLAMP_TO_EDGE;
			break;
		case ETC_MIRROR:
#ifdef GL_OES_texture_mirrored_repeat
			if (FeatureAvailable[IRR_OES_texture_mirrored_repeat])
				return GL_MIRRORED_REPEAT_OES;
			else
#endif
			return GL_REPEAT;
			break;
		// the next three are not yet supported at all
		case ETC_MIRROR_CLAMP:
		case ETC_MIRROR_CLAMP_TO_EDGE:
		case ETC_MIRROR_CLAMP_TO_BORDER:
#ifdef GL_OES_texture_mirrored_repeat
			if (FeatureAvailable[IRR_OES_texture_mirrored_repeat])
				return GL_MIRRORED_REPEAT_OES;
			else
#endif
			return GL_CLAMP_TO_EDGE;
			break;
		case ETC_REPEAT:
		default:
			return GL_REPEAT;
			break;
	}
}


void COGLES1Driver::setWrapMode(const SMaterial& material)
{
	// texture address mode
	// Has to be checked always because it depends on the textures
	for (u32 u=0; u<MaxTextureUnits; ++u)
	{
		if (MultiTextureExtension)
			extGlActiveTexture(GL_TEXTURE0 + u);
		else if (u>0)
			break; // stop loop

		// the APPLE npot restricted extension needs some care as it only supports CLAMP_TO_EDGE
		if (queryFeature(EVDF_TEXTURE_NPOT) && !FeatureAvailable[IRR_OES_texture_npot] &&
				CurrentTexture[u] && (CurrentTexture[u]->getSize() != CurrentTexture[u]->getOriginalSize()))
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrapMode(material.TextureLayer[u].TextureWrapU));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrapMode(material.TextureLayer[u].TextureWrapV));
		}
	}
}


//! Can be called by an IMaterialRenderer to make its work easier.
void COGLES1Driver::setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
	bool resetAllRenderStates)
{
	if (resetAllRenderStates ||
		lastmaterial.ColorMaterial != material.ColorMaterial)
	{
		// we only have diffuse_and_ambient in ogl-es
		if (material.ColorMaterial == ECM_DIFFUSE_AND_AMBIENT)
			glEnable(GL_COLOR_MATERIAL);
		else
			glDisable(GL_COLOR_MATERIAL);
	}

	if (resetAllRenderStates ||
		lastmaterial.AmbientColor != material.AmbientColor ||
		lastmaterial.DiffuseColor != material.DiffuseColor ||
		lastmaterial.EmissiveColor != material.EmissiveColor ||
		lastmaterial.ColorMaterial != material.ColorMaterial)
	{
		GLfloat color[4];

		const f32 inv = 1.0f / 255.0f;

		if ((material.ColorMaterial != video::ECM_AMBIENT) &&
			(material.ColorMaterial != video::ECM_DIFFUSE_AND_AMBIENT))
		{
			color[0] = material.AmbientColor.getRed() * inv;
			color[1] = material.AmbientColor.getGreen() * inv;
			color[2] = material.AmbientColor.getBlue() * inv;
			color[3] = material.AmbientColor.getAlpha() * inv;
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
		}

		if ((material.ColorMaterial != video::ECM_DIFFUSE) &&
			(material.ColorMaterial != video::ECM_DIFFUSE_AND_AMBIENT))
		{
			color[0] = material.DiffuseColor.getRed() * inv;
			color[1] = material.DiffuseColor.getGreen() * inv;
			color[2] = material.DiffuseColor.getBlue() * inv;
			color[3] = material.DiffuseColor.getAlpha() * inv;
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
		}

		if (material.ColorMaterial != video::ECM_EMISSIVE)
		{
			color[0] = material.EmissiveColor.getRed() * inv;
			color[1] = material.EmissiveColor.getGreen() * inv;
			color[2] = material.EmissiveColor.getBlue() * inv;
			color[3] = material.EmissiveColor.getAlpha() * inv;
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
		}
	}

	if (resetAllRenderStates ||
		lastmaterial.SpecularColor != material.SpecularColor ||
		lastmaterial.Shininess != material.Shininess)
	{
		GLfloat color[]={0.f,0.f,0.f,1.f};
		const f32 inv = 1.0f / 255.0f;

		// disable Specular colors if no shininess is set
		if ((material.Shininess != 0.0f) &&
			(material.ColorMaterial != video::ECM_SPECULAR))
		{
#ifdef GL_EXT_separate_specular_color
			if (FeatureAvailable[IRR_EXT_separate_specular_color])
				glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.Shininess);
			color[0] = material.SpecularColor.getRed() * inv;
			color[1] = material.SpecularColor.getGreen() * inv;
			color[2] = material.SpecularColor.getBlue() * inv;
			color[3] = material.SpecularColor.getAlpha() * inv;
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
		}
#ifdef GL_EXT_separate_specular_color
		else
			if (FeatureAvailable[IRR_EXT_separate_specular_color])
				glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
#endif
	}

	// Texture filter
	// Has to be checked always because it depends on the textures
	// Filtering has to be set for each texture layer
	for (u32 i=0; i<MaxTextureUnits; ++i)
	{
		if (MultiTextureExtension)
			extGlActiveTexture(GL_TEXTURE0 + i);
		else if (i>0)
			break;

#ifdef GL_EXT_texture_lod_bias
		if (FeatureAvailable[IRR_EXT_texture_lod_bias])
		{
			if (material.TextureLayer[i].LODBias)
			{
				const float tmp = core::clamp(material.TextureLayer[i].LODBias * 0.125f, -MaxTextureLODBias, MaxTextureLODBias);
				glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, tmp);
			}
			else
				glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0.f);
		}
#endif

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

		if (material.getTexture(i) && material.getTexture(i)->hasMipMaps())
			// the npot extensions need some checks, because APPLE
			// npot is somewhat restricted and only support non-mipmap filter
			if (queryFeature(EVDF_TEXTURE_NPOT) && !FeatureAvailable[IRR_OES_texture_npot] &&
					(CurrentTexture[i]->getSize() != CurrentTexture[i]->getOriginalSize()))
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					material.TextureLayer[i].TrilinearFilter ? GL_LINEAR:
					material.TextureLayer[i].BilinearFilter ? GL_LINEAR:
					GL_NEAREST);
			}
			else
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					material.TextureLayer[i].TrilinearFilter ? GL_LINEAR_MIPMAP_LINEAR :
					material.TextureLayer[i].BilinearFilter ? GL_LINEAR_MIPMAP_NEAREST :
					GL_NEAREST_MIPMAP_NEAREST );
			}
		else
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

#ifdef GL_EXT_texture_filter_anisotropic
		if (FeatureAvailable[IRR_EXT_texture_filter_anisotropic])
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
				static_cast<GLfloat>(material.TextureLayer[i].AnisotropicFilter>1 ? core::min_(MaxAnisotropy, material.TextureLayer[i].AnisotropicFilter) : 1));
#endif
	}

// TODO ogl-es
	// fillmode
//	if (resetAllRenderStates || (lastmaterial.Wireframe != material.Wireframe) || (lastmaterial.PointCloud != material.PointCloud))
//		glPolygonMode(GL_FRONT_AND_BACK, material.Wireframe ? GL_LINE : material.PointCloud? GL_POINT : GL_FILL);

	// shademode
	if (resetAllRenderStates || (lastmaterial.GouraudShading != material.GouraudShading))
	{
		if (material.GouraudShading)
			glShadeModel(GL_SMOOTH);
		else
			glShadeModel(GL_FLAT);
	}

	// lighting
	if (resetAllRenderStates || (lastmaterial.Lighting != material.Lighting))
	{
		if (material.Lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
	}

	// zbuffer
	if (resetAllRenderStates || lastmaterial.ZBuffer != material.ZBuffer)
	{
		switch (material.ZBuffer)
		{
			case ECFN_DISABLED:
				glDisable(GL_DEPTH_TEST);
				break;
			case ECFN_LESSEQUAL:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				break;
			case ECFN_EQUAL:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_EQUAL);
				break;
			case ECFN_LESS:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				break;
			case ECFN_NOTEQUAL:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_NOTEQUAL);
				break;
			case ECFN_GREATEREQUAL:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_GEQUAL);
				break;
			case ECFN_GREATER:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_GREATER);
				break;
			case ECFN_ALWAYS:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_ALWAYS);
				break;
			case ECFN_NEVER:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_NEVER);
				break;
		}
	}

	// Zwrite
	//if (resetAllRenderStates || lastmaterial.ZWriteEnable != material.ZWriteEnable)
	{
		if ( material.ZWriteEnable 
			 && 
			 (AllowZWriteOnTransparent || (material.BlendOperation == EBO_NONE && !MaterialRenderers[material.MaterialType].Renderer->isTransparent())))
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
	}

	// back face culling
	if (resetAllRenderStates || (lastmaterial.FrontfaceCulling != material.FrontfaceCulling) || (lastmaterial.BackfaceCulling != material.BackfaceCulling))
	{
		if ((material.FrontfaceCulling) && (material.BackfaceCulling))
		{
			glCullFace(GL_FRONT_AND_BACK);
			glEnable(GL_CULL_FACE);
		}
		else
		if (material.BackfaceCulling)
		{
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
		}
		else
		if (material.FrontfaceCulling)
		{
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
		}
		else
			glDisable(GL_CULL_FACE);
	}

	// fog
	if (resetAllRenderStates || lastmaterial.FogEnable != material.FogEnable)
	{
		if (material.FogEnable)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);
	}

	// normalization
	if (resetAllRenderStates || lastmaterial.NormalizeNormals != material.NormalizeNormals)
	{
		if (material.NormalizeNormals)
			glEnable(GL_NORMALIZE);
		else
			glDisable(GL_NORMALIZE);
	}

	// Color Mask
	if (resetAllRenderStates || lastmaterial.ColorMask != material.ColorMask)
	{
		glColorMask(
			(material.ColorMask & ECP_RED)?GL_TRUE:GL_FALSE,
			(material.ColorMask & ECP_GREEN)?GL_TRUE:GL_FALSE,
			(material.ColorMask & ECP_BLUE)?GL_TRUE:GL_FALSE,
			(material.ColorMask & ECP_ALPHA)?GL_TRUE:GL_FALSE);
	}

	// Blend Equation
	if (material.BlendOperation == EBO_NONE)
		BridgeCalls->setBlend(false);
	else
	{
		BridgeCalls->setBlend(true);

		if (queryFeature(EVDF_BLEND_OPERATIONS))
		{
			switch (material.BlendOperation)
			{
			case EBO_ADD:
#if defined(GL_OES_blend_subtract)
				BridgeCalls->setBlendEquation(GL_FUNC_ADD_OES);
#endif
				break;
			case EBO_SUBTRACT:
#if defined(GL_OES_blend_subtract)
				BridgeCalls->setBlendEquation(GL_FUNC_SUBTRACT_OES);
#endif
				break;
			case EBO_REVSUBTRACT:
#if defined(GL_OES_blend_subtract)
				BridgeCalls->setBlendEquation(GL_FUNC_REVERSE_SUBTRACT_OES);
#endif
				break;
			default:
				break;
			}
		}
	}

    // Blend Factor
	if (IR(material.BlendFactor) & 0xFFFFFFFF)
	{
        E_BLEND_FACTOR srcRGBFact = EBF_ZERO;
        E_BLEND_FACTOR dstRGBFact = EBF_ZERO;
        E_BLEND_FACTOR srcAlphaFact = EBF_ZERO;
        E_BLEND_FACTOR dstAlphaFact = EBF_ZERO;
        E_MODULATE_FUNC modulo = EMFN_MODULATE_1X;
        u32 alphaSource = 0;

        unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulo, alphaSource, material.BlendFactor);

        if (queryFeature(EVDF_BLEND_SEPARATE))
        {
            BridgeCalls->setBlendFuncSeparate(getGLBlend(srcRGBFact), getGLBlend(dstRGBFact),
                getGLBlend(srcAlphaFact), getGLBlend(dstAlphaFact));
        }
        else
        {
            BridgeCalls->setBlendFunc(getGLBlend(srcRGBFact), getGLBlend(dstRGBFact));
        }
	}

	// thickness
	if (resetAllRenderStates || lastmaterial.Thickness != material.Thickness)
	{
		if (AntiAlias)
		{
//			glPointSize(core::clamp(static_cast<GLfloat>(material.Thickness), DimSmoothedPoint[0], DimSmoothedPoint[1]));
			// we don't use point smoothing
			glPointSize(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedPoint[0], DimAliasedPoint[1]));
			glLineWidth(core::clamp(static_cast<GLfloat>(material.Thickness), DimSmoothedLine[0], DimSmoothedLine[1]));
		}
		else
		{
			glPointSize(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedPoint[0], DimAliasedPoint[1]));
			glLineWidth(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedLine[0], DimAliasedLine[1]));
		}
	}

	// Anti aliasing
	if (resetAllRenderStates || lastmaterial.AntiAliasing != material.AntiAliasing)
	{
//		if (FeatureAvailable[IRR_ARB_multisample])
		{
			if (material.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else if (lastmaterial.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

			if ((AntiAlias >= 2) && (material.AntiAliasing & (EAAM_SIMPLE|EAAM_QUALITY)))
				glEnable(GL_MULTISAMPLE);
			else
				glDisable(GL_MULTISAMPLE);
		}
		if ((material.AntiAliasing & EAAM_LINE_SMOOTH) != (lastmaterial.AntiAliasing & EAAM_LINE_SMOOTH))
		{
			if (material.AntiAliasing & EAAM_LINE_SMOOTH)
				glEnable(GL_LINE_SMOOTH);
			else if (lastmaterial.AntiAliasing & EAAM_LINE_SMOOTH)
				glDisable(GL_LINE_SMOOTH);
		}
		if ((material.AntiAliasing & EAAM_POINT_SMOOTH) != (lastmaterial.AntiAliasing & EAAM_POINT_SMOOTH))
		{
			if (material.AntiAliasing & EAAM_POINT_SMOOTH)
				// often in software, and thus very slow
				glEnable(GL_POINT_SMOOTH);
			else if (lastmaterial.AntiAliasing & EAAM_POINT_SMOOTH)
				glDisable(GL_POINT_SMOOTH);
		}
	}

	setWrapMode(material);

	// be sure to leave in texture stage 0
	if (MultiTextureExtension)
		extGlActiveTexture(GL_TEXTURE0);
}


//! sets the needed renderstates
void COGLES1Driver::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
{
    SMaterial mat2d;
	
	if (CurrentRenderMode != ERM_2D || Transformation3DChanged)
	{
		// unset last 3d material
		if (CurrentRenderMode == ERM_3D)
		{
			if (static_cast<u32>(LastMaterial.MaterialType) < MaterialRenderers.size())
				MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();
		}
		if (Transformation3DChanged)
		{
			glMatrixMode(GL_PROJECTION);

			const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
			core::matrix4 m;
			m.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0, 1.0);
			m.setTranslation(core::vector3df(-1,1,0));
			glLoadMatrixf(m.pointer());

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(0.375, 0.375, 0.0);

			// Make sure we set first texture matrix
			if (MultiTextureExtension)
				extGlActiveTexture(GL_TEXTURE0);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();

			Transformation3DChanged = false;
		}
		
		//by stone, fix gui bug
        /*if (!OverrideMaterial2DEnabled)
		{
			setBasicRenderStates(InitMaterial2D, LastMaterial, true);
			LastMaterial = InitMaterial2D;
		}*/

		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	
	//by stone, fix gui bug
    /*if (OverrideMaterial2DEnabled)
	{
		OverrideMaterial2D.Lighting=false;
		setBasicRenderStates(OverrideMaterial2D, LastMaterial, false);
		LastMaterial = OverrideMaterial2D;
	}*/
    
    mat2d.Lighting = false;
    mat2d.TextureLayer[0].BilinearFilter=false;
    setBasicRenderStates(mat2d, mat2d, true);
    LastMaterial = mat2d;

	if (alphaChannel || alpha)
	{
		BridgeCalls->setBlend(true);
		BridgeCalls->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.f);
	}
	else
	{
		BridgeCalls->setBlend(false);
		glDisable(GL_ALPHA_TEST);
	}

	if (texture)
	{
		if (!OverrideMaterial2DEnabled)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		Material.setTexture(0, const_cast<video::ITexture*>(CurrentTexture[0]));
		setTransform(ETS_TEXTURE_0, core::IdentityMatrix);
		// Due to the transformation change, the previous line would call a reset each frame
		// but we can safely reset the variable as it was false before
		Transformation3DChanged=false;

		if (alphaChannel)
		{
			// if alpha and alpha texture just modulate, otherwise use only the alpha channel
			if (alpha)
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			else
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE);
				// rgb always modulates
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
			}
		}
		else
		{
			if (alpha)
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);
				// rgb always modulates
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
			}
			else
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
		}
	}

	CurrentRenderMode = ERM_2D;
}


//! \return Returns the name of the video driver.
const wchar_t* COGLES1Driver::getName() const
{
	return Name.c_str();
}


//! deletes all dynamic lights there are
void COGLES1Driver::deleteAllDynamicLights()
{
	for (s32 i=0; i<MaxLights; ++i)
		glDisable(GL_LIGHT0 + i);

	RequestedLights.clear();

	CNullDriver::deleteAllDynamicLights();
}


//! adds a dynamic light
s32 COGLES1Driver::addDynamicLight(const SLight& light)
{
	CNullDriver::addDynamicLight(light);

	RequestedLights.push_back(RequestedLight(light));

	u32 newLightIndex = RequestedLights.size() - 1;

	// Try and assign a hardware light just now, but don't worry if I can't
	assignHardwareLight(newLightIndex);

	return (s32)newLightIndex;
}


void COGLES1Driver::assignHardwareLight(u32 lightIndex)
{
	setTransform(ETS_WORLD, core::matrix4());

	s32 lidx;
	for (lidx=GL_LIGHT0; lidx < GL_LIGHT0 + MaxLights; ++lidx)
	{
		if(!glIsEnabled(lidx))
		{
			RequestedLights[lightIndex].HardwareLightIndex = lidx;
			break;
		}
	}

	if(lidx == GL_LIGHT0 + MaxLights) // There's no room for it just now
		return;

	GLfloat data[4];
	const SLight & light = RequestedLights[lightIndex].LightData;

	switch (light.Type)
	{
	case video::ELT_SPOT:
		data[0] = light.Direction.X;
		data[1] = light.Direction.Y;
		data[2] = light.Direction.Z;
		data[3] = 0.0f;
		glLightfv(lidx, GL_SPOT_DIRECTION, data);

		// set position
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f; // 1.0f for positional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, light.Falloff);
		glLightf(lidx, GL_SPOT_CUTOFF, light.OuterCone);
	break;
	case video::ELT_POINT:
		// set position
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f; // 1.0f for positional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
	break;
	case video::ELT_DIRECTIONAL:
		// set direction
		data[0] = -light.Direction.X;
		data[1] = -light.Direction.Y;
		data[2] = -light.Direction.Z;
		data[3] = 0.0f; // 0.0f for directional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
	break;
	case video::ELT_COUNT:
		return;
	}

	// set diffuse color
	data[0] = light.DiffuseColor.r;
	data[1] = light.DiffuseColor.g;
	data[2] = light.DiffuseColor.b;
	data[3] = light.DiffuseColor.a;
	glLightfv(lidx, GL_DIFFUSE, data);

	// set specular color
	data[0] = light.SpecularColor.r;
	data[1] = light.SpecularColor.g;
	data[2] = light.SpecularColor.b;
	data[3] = light.SpecularColor.a;
	glLightfv(lidx, GL_SPECULAR, data);

	// set ambient color
	data[0] = light.AmbientColor.r;
	data[1] = light.AmbientColor.g;
	data[2] = light.AmbientColor.b;
	data[3] = light.AmbientColor.a;
	glLightfv(lidx, GL_AMBIENT, data);

	// 1.0f / (constant + linear * d + quadratic*(d*d);

	// set attenuation
	glLightf(lidx, GL_CONSTANT_ATTENUATION, light.Attenuation.X);
	glLightf(lidx, GL_LINEAR_ATTENUATION, light.Attenuation.Y);
	glLightf(lidx, GL_QUADRATIC_ATTENUATION, light.Attenuation.Z);

	glEnable(lidx);
}


//! Turns a dynamic light on or off
//! \param lightIndex: the index returned by addDynamicLight
//! \param turnOn: true to turn the light on, false to turn it off
void COGLES1Driver::turnLightOn(s32 lightIndex, bool turnOn)
{
	if(lightIndex < 0 || lightIndex >= (s32)RequestedLights.size())
		return;

	RequestedLight & requestedLight = RequestedLights[lightIndex];

	requestedLight.DesireToBeOn = turnOn;

	if(turnOn)
	{
		if(-1 == requestedLight.HardwareLightIndex)
			assignHardwareLight(lightIndex);
	}
	else
	{
		if(-1 != requestedLight.HardwareLightIndex)
		{
			// It's currently assigned, so free up the hardware light
			glDisable(requestedLight.HardwareLightIndex);
			requestedLight.HardwareLightIndex = -1;

			// Now let the first light that's waiting on a free hardware light grab it
			for(u32 requested = 0; requested < RequestedLights.size(); ++requested)
				if(RequestedLights[requested].DesireToBeOn
					&&
					-1 == RequestedLights[requested].HardwareLightIndex)
				{
					assignHardwareLight(requested);
					break;
				}
		}
	}
}


//! returns the maximal amount of dynamic lights the device can handle
u32 COGLES1Driver::getMaximalDynamicLightAmount() const
{
	return MaxLights;
}


//! Sets the dynamic ambient light color.
void COGLES1Driver::setAmbientLight(const SColorf& color)
{
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
}


// this code was sent in by Oliver Klems, thank you
void COGLES1Driver::setViewPort(const core::rect<s32>& area)
{
	core::rect<s32> vp = area;
	core::rect<s32> rendert(0,0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
	vp.clipAgainst(rendert);

	if (vp.getHeight()>0 && vp.getWidth()>0)
		glViewport(vp.UpperLeftCorner.X,
				getCurrentRenderTargetSize().Height - vp.UpperLeftCorner.Y - vp.getHeight(),
				vp.getWidth(), vp.getHeight());

	ViewPort = vp;
}

//! Draws a shadow volume into the stencil buffer.
//void COGLES1Driver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
void COGLES1Driver::drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail, u32 debugDataVisible)
{
	const u32 count=triangles.size();
	
	if (!StencilBuffer || !count)
		return;

	u8 colorMask = LastMaterial.ColorMask;
	const GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
	const GLboolean fogEnabled = glIsEnabled(GL_FOG);
	const GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);

	GLint cullFaceMode = 0;
	glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);
	GLint depthFunc = 0;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
	GLboolean depthMask = 0;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	if( !(debugDataVisible & (scene::EDS_SKELETON|scene::EDS_MESH_WIRE_OVERLAY)) )
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glEnable(GL_STENCIL_TEST);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(core::vector3df), triangles.const_pointer());

	glStencilMask(~0);
	glStencilFunc(GL_ALWAYS, 0, ~0);

	GLenum decr = GL_DECR;
	GLenum incr = GL_INCR;

#if defined(GL_OES_stencil_wrap)
	if (FeatureAvailable[IRR_OES_stencil_wrap])
	{
		decr = GL_DECR_WRAP_OES;
		incr = GL_INCR_WRAP_OES;
	}
#endif

	glEnable(GL_CULL_FACE);

	if (zfail)
	{
		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, incr, GL_KEEP);
		glDrawArrays(GL_TRIANGLES, 0, count);

		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, decr, GL_KEEP);
		glDrawArrays(GL_TRIANGLES, 0, count);
	}
	else // zpass
	{
		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_KEEP, incr);
		glDrawArrays(GL_TRIANGLES, 0, count);

		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_KEEP, decr);
		glDrawArrays(GL_TRIANGLES, 0, count);
	}

	glDisableClientState(GL_VERTEX_ARRAY);

	glColorMask((colorMask & ECP_RED)?GL_TRUE:GL_FALSE,
				(colorMask & ECP_GREEN)?GL_TRUE:GL_FALSE,
				(colorMask & ECP_BLUE)?GL_TRUE:GL_FALSE,
				(colorMask & ECP_ALPHA)?GL_TRUE:GL_FALSE);

	glDisable(GL_STENCIL_TEST);

	if (lightingEnabled)
		glEnable(GL_LIGHTING);

	if (fogEnabled)
		glEnable(GL_FOG);

	if (cullFaceEnabled)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glCullFace(cullFaceMode);
	glDepthFunc(depthFunc);
	glDepthMask(depthMask);
}


void COGLES1Driver::drawStencilShadow(bool clearStencilBuffer,
		video::SColor leftUpEdge, video::SColor rightUpEdge,
		video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	disableTextures();

	u8 colorMask = LastMaterial.ColorMask;
	const GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
	const GLboolean fogEnabled = glIsEnabled(GL_FOG);
	const GLboolean blendEnabled = glIsEnabled(GL_BLEND);

	GLboolean depthMask = 0;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
	GLint shadeModel = 0;
	glGetIntegerv(GL_SHADE_MODEL, &shadeModel);
	GLint blendSrc = 0, blendDst = 0;
	glGetIntegerv(GL_BLEND_SRC, &blendSrc);
	glGetIntegerv(GL_BLEND_DST, &blendDst);

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);

	glShadeModel(GL_FLAT);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	u16 indices[] = {0, 1, 2, 3};
	S3DVertex vertices[4];
	vertices[0] = S3DVertex(-1.f, 1.f, 0.9f, 0, 0, 1, leftDownEdge, 0, 0);
	vertices[1] = S3DVertex(1.f, 1.f, 0.9f, 0, 0, 1, leftUpEdge, 0, 0);
	vertices[2] = S3DVertex(1.f, -1.f, 0.9f, 0, 0, 1, rightUpEdge, 0, 0);
	vertices[3] = S3DVertex(-1.f, -1.f, 0.9f, 0, 0, 1, rightDownEdge, 0, 0);
	drawVertexPrimitiveList2d3d(vertices, 4, indices, 2, EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT, false);

	if (clearStencilBuffer)
		glClear(GL_STENCIL_BUFFER_BIT);

	glColorMask((colorMask & ECP_RED)?GL_TRUE:GL_FALSE,
			(colorMask & ECP_GREEN)?GL_TRUE:GL_FALSE,
			(colorMask & ECP_BLUE)?GL_TRUE:GL_FALSE,
			(colorMask & ECP_ALPHA)?GL_TRUE:GL_FALSE);

	glDisable(GL_STENCIL_TEST);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	if (lightingEnabled)
		glEnable(GL_LIGHTING);

	if (fogEnabled)
		glEnable(GL_FOG);

	if (!blendEnabled)
		glDisable(GL_BLEND);

	glDepthMask(depthMask);
	glShadeModel(shadeModel);
	glBlendFunc(blendSrc, blendDst);
}

//! Sets the fog mode.
void COGLES1Driver::setFog(SColor c, E_FOG_TYPE fogType, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	CNullDriver::setFog(c, fogType, start, end, density, pixelFog, rangeFog);

	glFogf(GL_FOG_MODE, GLfloat((fogType==EFT_FOG_LINEAR)? GL_LINEAR : (fogType==EFT_FOG_EXP)?GL_EXP:GL_EXP2));

#ifdef GL_EXT_fog_coord
	if (FeatureAvailable[IRR_EXT_fog_coord])
		glFogi(GL_FOG_COORDINATE_SOURCE, GL_FRAGMENT_DEPTH);
#endif

	if (fogType==EFT_FOG_LINEAR)
	{
		glFogf(GL_FOG_START, start);
		glFogf(GL_FOG_END, end);
	}
	else
		glFogf(GL_FOG_DENSITY, density);

	if (pixelFog)
		glHint(GL_FOG_HINT, GL_NICEST);
	else
		glHint(GL_FOG_HINT, GL_FASTEST);

	SColorf color(c);
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glFogfv(GL_FOG_COLOR, data);
}


//! Draws a 3d line.
void COGLES1Driver::draw3DLine(const core::vector3df& start,
				const core::vector3df& end, SColor color)
{
	setRenderStates3DMode();

	u16 indices[] = {0,1};
	S3DVertex vertices[2];
	vertices[0] = S3DVertex(start.X,start.Y,start.Z, 0,0,1, color, 0,0);
	vertices[1] = S3DVertex(end.X,end.Y,end.Z, 0,0,1, color, 0,0);
	drawVertexPrimitiveList2d3d(vertices, 2, indices, 1, video::EVT_STANDARD, scene::EPT_LINES);
}


//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void COGLES1Driver::OnResize(const core::dimension2d<u32>& size)
{
	CNullDriver::OnResize(size);
	glViewport(0, 0, size.Width, size.Height);
}


//! Returns type of video driver
E_DRIVER_TYPE COGLES1Driver::getDriverType() const
{
	return EDT_OGLES1;
}


//! returns color format
ECOLOR_FORMAT COGLES1Driver::getColorFormat() const
{
	return ColorFormat;
}


//! Get a vertex shader constant index.
s32 COGLES1Driver::getVertexShaderConstantID(const c8* name)
{
	return getPixelShaderConstantID(name);
}

//! Get a pixel shader constant index.
s32 COGLES1Driver::getPixelShaderConstantID(const c8* name)
{
	os::Printer::log("Error: Please use IMaterialRendererServices from IShaderConstantSetCallBack::OnSetConstants not VideoDriver->getPixelShaderConstantID().");
	return -1;
}

//! Sets a constant for the vertex shader based on an index.
bool COGLES1Driver::setVertexShaderConstant(s32 index, const f32* floats, int count)
{
	//pass this along, as in GLSL the same routine is used for both vertex and fragment shaders
	return setPixelShaderConstant(index, floats, count);
}

//! Int interface for the above.
bool COGLES1Driver::setVertexShaderConstant(s32 index, const s32* ints, int count)
{
	return setPixelShaderConstant(index, ints, count);
}

//! Sets a constant for the pixel shader based on an index.
bool COGLES1Driver::setPixelShaderConstant(s32 index, const f32* floats, int count)
{
	os::Printer::log("Error: Please use IMaterialRendererServices from IShaderConstantSetCallBack::OnSetConstants not VideoDriver->setPixelShaderConstant().");
	return false;
}

//! Int interface for the above.
bool COGLES1Driver::setPixelShaderConstant(s32 index, const s32* ints, int count)
{
	os::Printer::log("Error: Please use IMaterialRendererServices from IShaderConstantSetCallBack::OnSetConstants not VideoDriver->setPixelShaderConstant().");
	return false;
}

//! Sets a vertex shader constant.
void COGLES1Driver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
#ifdef GL_vertex_program
	for (s32 i=0; i<constantAmount; ++i)
		extGlProgramLocalParameter4fv(GL_VERTEX_PROGRAM, startRegister+i, &data[i*4]);
#endif
}

//! Sets a pixel shader constant.
void COGLES1Driver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
#ifdef GL_fragment_program
	for (s32 i=0; i<constantAmount; ++i)
		extGlProgramLocalParameter4fv(GL_FRAGMENT_PROGRAM, startRegister+i, &data[i*4]);
#endif
}


//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32 COGLES1Driver::addShaderMaterial(const c8* vertexShaderProgram,
	const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial, s32 userData)
{
	os::Printer::log("No shader support.");
	return -1;
}


//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
s32 COGLES1Driver::addHighLevelShaderMaterial(
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData)
{
	os::Printer::log("No shader support.");
	return -1;
}

//! Returns a pointer to the IVideoDriver interface. (Implementation for
//! IMaterialRendererServices)
IVideoDriver* COGLES1Driver::getVideoDriver()
{
	return this;
}


//! Returns pointer to the IGPUProgrammingServices interface.
IGPUProgrammingServices* COGLES1Driver::getGPUProgrammingServices()
{
	return this;
}


ITexture* COGLES1Driver::addRenderTargetTexture(const core::dimension2d<u32>& size,
					const io::path& name,
					const ECOLOR_FORMAT format)
{
	//disable mip-mapping
	const bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	video::ITexture* rtt = 0;

#if defined(GL_OES_framebuffer_object)
	// if driver supports FrameBufferObjects, use them
	if (queryFeature(EVDF_FRAMEBUFFER_OBJECT))
	{
		rtt = new COGLES1FBOTexture(size, name, this, format);
		if (rtt)
		{
			bool success = false;
			addTexture(rtt);

			ITexture* tex = createDepthTexture(rtt);
			if (tex)
			{
				success = static_cast<video::COGLES1FBODepthTexture*>(tex)->attach(rtt);
				if (!success)
				{
					removeDepthTexture(tex);
				}
				tex->drop();
			}
			rtt->drop();
			if (!success)
			{
				removeTexture(rtt);
				rtt=0;
			}
		}
	}
	else
#endif
	{
		// the simple texture is only possible for size <= screensize
		// we try to find an optimal size with the original constraints
		core::dimension2du destSize(core::min_(size.Width,ScreenSize.Width), core::min_(size.Height,ScreenSize.Height));
		destSize = destSize.getOptimalSize((size==size.getOptimalSize()), false, false);
		rtt = addTexture(destSize, name, ECF_A8R8G8B8);
		if (rtt)
			static_cast<video::COGLES1Texture*>(rtt)->setIsRenderTarget(true);
	}

	//restore mip-mapping
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, generateMipLevels);

	return rtt;
}


//! Returns the maximum amount of primitives
u32 COGLES1Driver::getMaximalPrimitiveCount() const
{
	return 65535;
}


//! set or reset render target
bool COGLES1Driver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
					bool clearZBuffer, SColor color)
{
	// check for right driver type

	if (texture && texture->getDriverType() != EDT_OGLES1)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	// check if we should set the previous RT back

	setActiveTexture(0, 0);
	ResetRenderStates=true;
	if (RenderTargetTexture!=0)
	{
		RenderTargetTexture->unbindRTT();
	}

	if (texture)
	{
		// we want to set a new target. so do this.
		RenderTargetTexture = static_cast<COGLES1Texture*>(texture);
		RenderTargetTexture->bindRTT();
		CurrentRendertargetSize = texture->getSize();
	}
	else
	{
		glViewport(0,0,ScreenSize.Width,ScreenSize.Height);
		RenderTargetTexture = 0;
		CurrentRendertargetSize = core::dimension2d<u32>(0,0);
	}

	GLbitfield mask = 0;
	if (clearBackBuffer)
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		Material.ColorMask = ECP_ALL;

		const f32 inv = 1.0f / 255.0f;
		glClearColor(color.getRed() * inv, color.getGreen() * inv,
				color.getBlue() * inv, color.getAlpha() * inv);

		mask |= GL_COLOR_BUFFER_BIT;
	}
	if (clearZBuffer)
	{
		glDepthMask(GL_TRUE);
		Material.ZWriteEnable = true;

		mask |= GL_DEPTH_BUFFER_BIT;
	}

	glClear(mask);

	return true;
}


// returns the current size of the screen or rendertarget
const core::dimension2d<u32>& COGLES1Driver::getCurrentRenderTargetSize() const
{
	if ( CurrentRendertargetSize.Width == 0 )
		return ScreenSize;
	else
		return CurrentRendertargetSize;
}


//! Clears the ZBuffer.
void COGLES1Driver::clearZBuffer()
{
	glDepthMask(GL_TRUE);
	Material.ZWriteEnable = true;

	glClear(GL_DEPTH_BUFFER_BIT);
}


//! Returns an image created from the last rendered frame.
// We want to read the front buffer to get the latest render finished.
// This is not possible under ogl-es, though, so one has to call this method
// outside of the render loop only.
IImage* COGLES1Driver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
{
	if (target==video::ERT_MULTI_RENDER_TEXTURES || target==video::ERT_RENDER_TEXTURE || target==video::ERT_STEREO_BOTH_BUFFERS)
		return 0;
	GLint internalformat=GL_RGBA;
	GLint type=GL_UNSIGNED_BYTE;
	if (false && (FeatureAvailable[IRR_IMG_read_format] || FeatureAvailable[IRR_OES_read_format] || FeatureAvailable[IRR_EXT_read_format_bgra]))
	{
#ifdef GL_IMPLEMENTATION_COLOR_READ_TYPE_OES
		glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES, &internalformat);
		glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE_OES, &type);
#endif
		// there are formats we don't support ATM
		if (GL_UNSIGNED_SHORT_4_4_4_4==type)
			type=GL_UNSIGNED_SHORT_5_5_5_1;
#ifdef GL_EXT_read_format_bgra
		else if (GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT==type)
			type=GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT;
#endif
	}

	IImage* newImage = 0;
	if ((GL_RGBA==internalformat)
#ifdef GL_EXT_read_format_bgra
			|| (GL_BGRA_EXT==internalformat)
#endif
			)
	{
		if (GL_UNSIGNED_BYTE==type)
			newImage = new CImage(ECF_A8R8G8B8, ScreenSize);
		else
			newImage = new CImage(ECF_A1R5G5B5, ScreenSize);
	}
	else
	{
		if (GL_UNSIGNED_BYTE==type)
			newImage = new CImage(ECF_R8G8B8, ScreenSize);
		else
			newImage = new CImage(ECF_R5G6B5, ScreenSize);
	}

	u8* pixels = static_cast<u8*>(newImage->lock());
	if (!pixels)
	{
		newImage->drop();
		return 0;
	}

	glReadPixels(0, 0, ScreenSize.Width, ScreenSize.Height, internalformat, type, pixels);

	// opengl images are horizontally flipped, so we have to fix that here.
	const s32 pitch=newImage->getPitch();
	u8* p2 = pixels + (ScreenSize.Height - 1) * pitch;
	u8* tmpBuffer = new u8[pitch];
	for (u32 i=0; i < ScreenSize.Height; i += 2)
	{
		memcpy(tmpBuffer, pixels, pitch);
		memcpy(pixels, p2, pitch);
		memcpy(p2, tmpBuffer, pitch);
		pixels += pitch;
		p2 -= pitch;
	}
	delete [] tmpBuffer;

	newImage->unlock();

	if (testGLError())
	{
		newImage->drop();
		return 0;
	}

	return newImage;
}


//! get depth texture for the given render target texture
ITexture* COGLES1Driver::createDepthTexture(ITexture* texture, bool shared)
{
	if ((texture->getDriverType() != EDT_OGLES1) || (!texture->isRenderTarget()))
		return 0;
	COGLES1Texture* tex = static_cast<COGLES1Texture*>(texture);

	if (!tex->isFrameBufferObject())
		return 0;

	if (shared)
	{
		for (u32 i=0; i<DepthTextures.size(); ++i)
		{
			if (DepthTextures[i]->getSize()==texture->getSize())
			{
				DepthTextures[i]->grab();
				return DepthTextures[i];
			}
		}
		DepthTextures.push_back(new COGLES1FBODepthTexture(texture->getSize(), "depth1", this));
		return DepthTextures.getLast();
	}
	return (new COGLES1FBODepthTexture(texture->getSize(), "depth1", this));
}


void COGLES1Driver::removeDepthTexture(ITexture* texture)
{
	for (u32 i=0; i<DepthTextures.size(); ++i)
	{
		if (texture==DepthTextures[i])
		{
			DepthTextures.erase(i);
			return;
		}
	}
}


void COGLES1Driver::removeTexture(ITexture* texture)
{
	if (!texture)
		return;

	CNullDriver::removeTexture(texture);
	CurrentTexture.remove(texture);
}


//! Set/unset a clipping plane.
bool COGLES1Driver::setClipPlane(u32 index, const core::plane3df& plane, bool enable)
{
	if (index >= MaxUserClipPlanes)
		return false;

	UserClipPlane[index]=plane;
	enableClipPlane(index, enable);
	return true;
}


void COGLES1Driver::uploadClipPlane(u32 index)
{
	// opengl needs an array of doubles for the plane equation
	float clip_plane[4];
	clip_plane[0] = UserClipPlane[index].Normal.X;
	clip_plane[1] = UserClipPlane[index].Normal.Y;
	clip_plane[2] = UserClipPlane[index].Normal.Z;
	clip_plane[3] = UserClipPlane[index].D;
	glClipPlanef(GL_CLIP_PLANE0 + index, clip_plane);
}


//! Enable/disable a clipping plane.
void COGLES1Driver::enableClipPlane(u32 index, bool enable)
{
	if (index >= MaxUserClipPlanes)
		return;
	if (enable)
	{
		if (!UserClipPlaneEnabled[index])
		{
			uploadClipPlane(index);
			glEnable(GL_CLIP_PLANE0 + index);
		}
	}
	else
		glDisable(GL_CLIP_PLANE0 + index);

	UserClipPlaneEnabled[index]=enable;
}


core::dimension2du COGLES1Driver::getMaxTextureSize() const
{
	return core::dimension2du(MaxTextureSize, MaxTextureSize);
}


GLenum COGLES1Driver::getGLBlend(E_BLEND_FACTOR factor) const
{
	GLenum r = 0;
	switch (factor)
	{
		case EBF_ZERO:			r = GL_ZERO; break;
		case EBF_ONE:			r = GL_ONE; break;
		case EBF_DST_COLOR:		r = GL_DST_COLOR; break;
		case EBF_ONE_MINUS_DST_COLOR:	r = GL_ONE_MINUS_DST_COLOR; break;
		case EBF_SRC_COLOR:		r = GL_SRC_COLOR; break;
		case EBF_ONE_MINUS_SRC_COLOR:	r = GL_ONE_MINUS_SRC_COLOR; break;
		case EBF_SRC_ALPHA:		r = GL_SRC_ALPHA; break;
		case EBF_ONE_MINUS_SRC_ALPHA:	r = GL_ONE_MINUS_SRC_ALPHA; break;
		case EBF_DST_ALPHA:		r = GL_DST_ALPHA; break;
		case EBF_ONE_MINUS_DST_ALPHA:	r = GL_ONE_MINUS_DST_ALPHA; break;
		case EBF_SRC_ALPHA_SATURATE:	r = GL_SRC_ALPHA_SATURATE; break;
	}
	return r;
}


COGLES1CallBridge* COGLES1Driver::getBridgeCalls() const
{
	return BridgeCalls;
}


COGLES1CallBridge::COGLES1CallBridge(COGLES1Driver* driver) : Driver(driver),
#if defined(GL_OES_blend_subtract)
	BlendEquation(GL_FUNC_ADD_OES),
#endif
	BlendSourceRGB(GL_ONE), BlendDestinationRGB(GL_ZERO),
	BlendSourceAlpha(GL_ONE), BlendDestinationAlpha(GL_ZERO),
	Blend(false)
{
	// Initial OpenGL ES1.x values from specification.

	if (Driver->queryFeature(EVDF_BLEND_OPERATIONS))
	{
#if defined(GL_OES_blend_subtract)
		Driver->extGlBlendEquation(GL_FUNC_ADD_OES);
#endif
	}

	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);
}

void COGLES1CallBridge::setBlendEquation(GLenum mode)
{
	if (BlendEquation != mode)
	{
		Driver->extGlBlendEquation(mode);

		BlendEquation = mode;
	}
}

void COGLES1CallBridge::setBlendFunc(GLenum source, GLenum destination)
{
	if (BlendSourceRGB != source || BlendDestinationRGB != destination ||
        BlendSourceAlpha != source || BlendDestinationAlpha != destination)
	{
		glBlendFunc(source, destination);

		BlendSourceRGB = source;
		BlendDestinationRGB = destination;
		BlendSourceAlpha = source;
		BlendDestinationAlpha = destination;
	}
}

void COGLES1CallBridge::setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
{
    if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
    {
        if (BlendSourceRGB != sourceRGB || BlendDestinationRGB != destinationRGB ||
            BlendSourceAlpha != sourceAlpha || BlendDestinationAlpha != destinationAlpha)
        {
            Driver->extGlBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

			BlendSourceRGB = sourceRGB;
			BlendDestinationRGB = destinationRGB;
			BlendSourceAlpha = sourceAlpha;
			BlendDestinationAlpha = destinationAlpha;
        }
    }
    else
    {
        setBlendFunc(sourceRGB, destinationRGB);
    }
}

void COGLES1CallBridge::setBlend(bool enable)
{
	if (Blend != enable)
	{
		if (enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		Blend = enable;
	}
}

} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_OGLES1_

namespace irr
{
namespace video
{

// -----------------------------------
// WINDOWS VERSION
// -----------------------------------
#if defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined(_IRR_COMPILE_WITH_SDL_DEVICE_) || defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_) || defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
IVideoDriver* createOGLES1Driver(const SIrrlichtCreationParameters& params,
		video::SExposedVideoData& data, io::IFileSystem* io)
{
#ifdef _IRR_COMPILE_WITH_OGLES1_
	return new COGLES1Driver(params, data, io);
#else
	return 0;
#endif // _IRR_COMPILE_WITH_OGLES1_
}
#endif

// -----------------------------------
// MACOSX VERSION
// -----------------------------------
#if defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
IVideoDriver* createOGLES1Driver(const SIrrlichtCreationParameters& params,
		io::IFileSystem* io, CIrrDeviceMacOSX *device)
{
#ifdef _IRR_COMPILE_WITH_OGLES1_
	return new COGLES1Driver(params, io, device);
#else
	return 0;
#endif //  _IRR_COMPILE_WITH_OGLES1_
}
#endif // _IRR_COMPILE_WITH_OSX_DEVICE_

// -----------------------------------
// IPHONE VERSION
// -----------------------------------
#if defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
IVideoDriver* createOGLES1Driver(const SIrrlichtCreationParameters& params,
		video::SExposedVideoData& data, io::IFileSystem* io,
		MIrrIPhoneDevice* device)
{
#ifdef _IRR_COMPILE_WITH_OGLES1_
	return new COGLES1Driver(params, data, io, device);
#else
	return 0;
#endif // _IRR_COMPILE_WITH_OGLES1_
}
#endif // _IRR_COMPILE_WITH_IPHONE_DEVICE_

} // end namespace
} // end namespace
