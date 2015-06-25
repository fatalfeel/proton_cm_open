/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include <cstdio>
#include <vector>
#include <cmath>
#include <deque>
#include <map>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include "FileSystem/FileManager.h"

#include "ccMacros.h"
#include "CCShaderCache.h"
#include "CCGLProgram.h"
//#include "ccShaders.h"
#include "PlatFormEnums.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_) && defined(_IRR_OPENGL_USE_EXTPOINTER_)
#define COCOS2DX_GLSL_EXTENSION(x)		wglGetProcAddress(reinterpret_cast<const char*>(x))

PFNGLGENBUFFERSPROC						__glewGenBuffers;
PFNGLDELETEBUFFERSPROC					__glewDeleteBuffers;
PFNGLBINDBUFFERPROC						__glewBindBuffer;
PFNGLBUFFERDATAPROC						__glewBufferData;
PFNGLBUFFERSUBDATAPROC					__glewBufferSubData;
PFNGLACTIVETEXTUREPROC					__glewActiveTexture;
PFNGLCLIENTACTIVETEXTUREPROC			__glewClientActiveTexture;
PFNGLBINDFRAMEBUFFERPROC				__glewBindFramebuffer;
PFNGLBINDRENDERBUFFERPROC				__glewBindRenderbuffer;
PFNGLGENFRAMEBUFFERSPROC				__glewGenFramebuffers;
PFNGLGENRENDERBUFFERSPROC				__glewGenRenderbuffers;
PFNGLDELETEFRAMEBUFFERSPROC				__glewDeleteFramebuffers;
PFNGLDELETERENDERBUFFERSPROC			__glewDeleteRenderbuffers;
PFNGLFRAMEBUFFERRENDERBUFFERPROC		__glewFramebufferRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC			__glewRenderbufferStorage;
PFNGLCHECKFRAMEBUFFERSTATUSPROC			__glewCheckFramebufferStatus;
PFNGLGENERATEMIPMAPPROC					__glewGenerateMipmap;
PFNGLFRAMEBUFFERTEXTURE2DPROC			__glewFramebufferTexture2D;
PFNGLCOMPRESSEDTEXIMAGE2DPROC			__glewCompressedTexImage2D;
	
/*PFNGLCREATEPROGRAMPROC					__glewCreateProgram;
PFNGLDELETEPROGRAMPROC					__glewDeleteProgram;
PFNGLLINKPROGRAMPROC					__glewLinkProgram;
PFNGLUSEPROGRAMPROC						__glewUseProgram;
PFNGLGETPROGRAMINFOLOGPROC				__glewGetProgramInfoLog;
PFNGLGETPROGRAMIVPROC					__glewGetProgramiv;
PFNGLCREATESHADERPROC					__glewCreateShader;
PFNGLATTACHSHADERPROC					__glewAttachShader;
PFNGLSHADERSOURCEPROC					__glewShaderSource;
PFNGLCOMPILESHADERPROC					__glewCompileShader;
PFNGLDELETESHADERPROC					__glewDeleteShader;
PFNGLGETSHADERINFOLOGPROC				__glewGetShaderInfoLog;
PFNGLGETSHADERIVPROC					__glewGetShaderiv;
PFNGLBINDATTRIBLOCATIONPROC				__glewBindAttribLocation;
PFNGLGETUNIFORMLOCATIONPROC				__glewGetUniformLocation;
PFNGLVERTEXATTRIBPOINTERPROC			__glewVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC		__glewEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC		__glewDisableVertexAttribArray;
PFNGLUNIFORM1FPROC						__glewUniform1f;
PFNGLUNIFORM1FVPROC						__glewUniform1fv;
PFNGLUNIFORM2FPROC						__glewUniform2f;
PFNGLUNIFORM2FVPROC						__glewUniform2fv;
PFNGLUNIFORM3FPROC						__glewUniform3f;
PFNGLUNIFORM3FVPROC						__glewUniform3fv;
PFNGLUNIFORM4FPROC						__glewUniform4f;
PFNGLUNIFORM4FVPROC						__glewUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC				__glewUniformMatrix4fv;*/
#endif

NS_CC_BEGIN

enum {
    kCCShaderType_PositionTextureColor,
    kCCShaderType_PositionTextureColorAlphaTest,
    kCCShaderType_PositionColor,
    kCCShaderType_PositionTexture,
    kCCShaderType_PositionTexture_uColor,
    kCCShaderType_PositionTextureA8Color,
    kCCShaderType_Position_uColor,
    kCCShaderType_MAX,
};

char* ccPosition_uColor_frag;
char* ccPosition_uColor_vert;
char* ccPositionColor_frag;
char* ccPositionColor_vert;
char* ccPositionTexture_frag;
char* ccPositionTexture_vert;
char* ccPositionTextureA8Color_frag;
char* ccPositionTextureA8Color_vert;
char* ccPositionTextureColor_frag;
char* ccPositionTextureColor_vert;
char* ccPositionTexture_uColor_frag;
char* ccPositionTexture_uColor_vert;
char* ccPositionTextureColorAlphaTest_frag;
char* ccExSwitchMask_frag;

static CCShaderCache* _sharedShaderCache = 0;

CCShaderCache* CCShaderCache::sharedShaderCache()
{
    if (!_sharedShaderCache) 
	{
        _sharedShaderCache = new CCShaderCache();
        if (!_sharedShaderCache->init())
        {
            CC_SAFE_DELETE(_sharedShaderCache);
        }
    }
    return _sharedShaderCache;
}

void CCShaderCache::purgeSharedShaderCache()
{
    CC_SAFE_RELEASE_NULL(_sharedShaderCache);
}

CCShaderCache::CCShaderCache()
: m_pPrograms(0)
{

}

CCShaderCache::~CCShaderCache()
{
    CCLOGINFO("cocos2d deallocing 0x%X", this);
    m_pPrograms->release();

#if defined(_IRR_COMPILE_WITH_OGLES2_)
    delete ccPosition_uColor_frag;
    delete ccPosition_uColor_vert;

    delete ccPositionColor_frag;
    delete ccPositionColor_vert;

    delete ccPositionTexture_frag;
    delete ccPositionTexture_vert;
    
    delete ccPositionTextureA8Color_frag;
    delete ccPositionTextureA8Color_vert;
    
    delete ccPositionTextureColor_frag;
    delete ccPositionTextureColor_vert;
   
    delete ccPositionTexture_uColor_frag;
    delete ccPositionTexture_uColor_vert;
    
    delete ccPositionTextureColorAlphaTest_frag;
    delete ccExSwitchMask_frag;
#endif
}

bool CCShaderCache::init()
{
    m_pPrograms = new CCDictionary();

#if defined(_IRR_COMPILE_WITH_OGLES2_)
    ccPosition_uColor_frag = GetShaderFile("shaders/ccShader_Position_uColor_frag.h");
    ccPosition_uColor_vert = GetShaderFile("shaders/ccShader_Position_uColor_vert.h");
    
    ccPositionColor_frag = GetShaderFile("shaders/ccShader_PositionColor_frag.h");
    ccPositionColor_vert = GetShaderFile("shaders/ccShader_PositionColor_vert.h");
    
    ccPositionTexture_frag = GetShaderFile("shaders/ccShader_PositionTexture_frag.h");
    ccPositionTexture_vert = GetShaderFile("shaders/ccShader_PositionTexture_vert.h");
    
    ccPositionTextureA8Color_frag = GetShaderFile("shaders/ccShader_PositionTextureA8Color_frag.h");
    ccPositionTextureA8Color_vert = GetShaderFile("shaders/ccShader_PositionTextureA8Color_vert.h");
    
    ccPositionTextureColor_frag = GetShaderFile("shaders/ccShader_PositionTextureColor_frag.h");
    ccPositionTextureColor_vert = GetShaderFile("shaders/ccShader_PositionTextureColor_vert.h");
    
    ccPositionTexture_uColor_frag = GetShaderFile("shaders/ccShader_PositionTexture_uColor_frag.h");
    ccPositionTexture_uColor_vert = GetShaderFile("shaders/ccShader_PositionTexture_uColor_vert.h");
    
    ccPositionTextureColorAlphaTest_frag = GetShaderFile("shaders/ccShader_PositionTextureColorAlphaTest_frag.h");
    ccExSwitchMask_frag = GetShaderFile("shaders/ccShaderEx_SwitchMask_frag.h");

#elif defined(_IRR_COMPILE_WITH_OPENGL_) && defined(_IRR_OPENGL_USE_EXTPOINTER_)
	glewInit();

#endif

    loadDefaultShaders();
    return true;
}

#if defined(_IRR_COMPILE_WITH_OPENGL_) && defined(_IRR_OPENGL_USE_EXTPOINTER_)
void CCShaderCache::glewInit()
{
	__glewGenBuffers				= (PFNGLGENBUFFERSPROC)COCOS2DX_GLSL_EXTENSION("glGenBuffersARB");
	__glewDeleteBuffers				= (PFNGLDELETEBUFFERSPROC)COCOS2DX_GLSL_EXTENSION("glDeleteBuffersARB");
	__glewBindBuffer				= (PFNGLBINDBUFFERPROC)COCOS2DX_GLSL_EXTENSION("glBindBufferARB");
	__glewBufferData				= (PFNGLBUFFERDATAPROC)COCOS2DX_GLSL_EXTENSION("glBufferDataARB");
	__glewBufferSubData				= (PFNGLBUFFERSUBDATAPROC)COCOS2DX_GLSL_EXTENSION("glBufferSubDataARB");
	__glewActiveTexture				= (PFNGLACTIVETEXTUREPROC)COCOS2DX_GLSL_EXTENSION("glActiveTextureARB");
	__glewClientActiveTexture		= (PFNGLCLIENTACTIVETEXTUREPROC)COCOS2DX_GLSL_EXTENSION("glClientActiveTextureARB");
	__glewBindFramebuffer			= (PFNGLBINDFRAMEBUFFERPROC)COCOS2DX_GLSL_EXTENSION("glBindFramebuffer");
	__glewBindRenderbuffer			= (PFNGLBINDRENDERBUFFERPROC)COCOS2DX_GLSL_EXTENSION("glBindRenderbuffer");
	__glewGenFramebuffers			= (PFNGLGENFRAMEBUFFERSPROC)COCOS2DX_GLSL_EXTENSION("glGenFramebuffers");
	__glewGenRenderbuffers			= (PFNGLGENRENDERBUFFERSPROC)COCOS2DX_GLSL_EXTENSION("glGenRenderbuffers");
	__glewDeleteFramebuffers		= (PFNGLDELETEFRAMEBUFFERSPROC)COCOS2DX_GLSL_EXTENSION("glDeleteFramebuffers");
	__glewDeleteRenderbuffers		= (PFNGLDELETERENDERBUFFERSPROC)COCOS2DX_GLSL_EXTENSION("glDeleteRenderbuffers");
	__glewFramebufferRenderbuffer	= (PFNGLFRAMEBUFFERRENDERBUFFERPROC)COCOS2DX_GLSL_EXTENSION("glFramebufferRenderbuffer");
	__glewRenderbufferStorage		= (PFNGLRENDERBUFFERSTORAGEPROC)COCOS2DX_GLSL_EXTENSION("glRenderbufferStorage");
	__glewCheckFramebufferStatus	= (PFNGLCHECKFRAMEBUFFERSTATUSPROC)COCOS2DX_GLSL_EXTENSION("glCheckFramebufferStatus");
	__glewGenerateMipmap			= (PFNGLGENERATEMIPMAPPROC)COCOS2DX_GLSL_EXTENSION("glGenerateMipmap");
	__glewFramebufferTexture2D		= (PFNGLFRAMEBUFFERTEXTURE2DPROC)COCOS2DX_GLSL_EXTENSION("glFramebufferTexture2D");
	__glewCompressedTexImage2D		= (PFNGLCOMPRESSEDTEXIMAGE2DPROC)COCOS2DX_GLSL_EXTENSION("glCompressedTexImage2D");
	
	/*__glewCreateProgram				= (PFNGLCREATEPROGRAMPROC)COCOS2DX_GLSL_EXTENSION("glCreateProgram");
	__glewDeleteProgram				= (PFNGLDELETEPROGRAMPROC)COCOS2DX_GLSL_EXTENSION("glDeleteProgram");
	__glewLinkProgram				= (PFNGLLINKPROGRAMPROC)COCOS2DX_GLSL_EXTENSION("glLinkProgram");
	__glewUseProgram				= (PFNGLUSEPROGRAMPROC)COCOS2DX_GLSL_EXTENSION("glUseProgram");
	__glewGetProgramInfoLog			= (PFNGLGETPROGRAMINFOLOGPROC)COCOS2DX_GLSL_EXTENSION("glGetProgramInfoLog");
	__glewGetProgramiv				= (PFNGLGETPROGRAMIVPROC)COCOS2DX_GLSL_EXTENSION("glGetProgramiv");
	__glewCreateShader				= (PFNGLCREATESHADERPROC)COCOS2DX_GLSL_EXTENSION("glCreateShader");
	__glewAttachShader				= (PFNGLATTACHSHADERPROC)COCOS2DX_GLSL_EXTENSION("glAttachShader");
	__glewShaderSource				= (PFNGLSHADERSOURCEPROC)COCOS2DX_GLSL_EXTENSION("glShaderSource");
	__glewCompileShader				= (PFNGLCOMPILESHADERPROC)COCOS2DX_GLSL_EXTENSION("glCompileShader");
	__glewDeleteShader				= (PFNGLDELETESHADERPROC)COCOS2DX_GLSL_EXTENSION("glDeleteShader");
	__glewGetShaderInfoLog			= (PFNGLGETSHADERINFOLOGPROC)COCOS2DX_GLSL_EXTENSION("glGetShaderInfoLog");
	__glewGetShaderiv				= (PFNGLGETSHADERIVPROC)COCOS2DX_GLSL_EXTENSION("glGetShaderiv");
	__glewBindAttribLocation		= (PFNGLBINDATTRIBLOCATIONPROC)COCOS2DX_GLSL_EXTENSION("glBindAttribLocation");
	__glewGetUniformLocation		= (PFNGLGETUNIFORMLOCATIONPROC)COCOS2DX_GLSL_EXTENSION("glGetUniformLocation");
	__glewVertexAttribPointer		= (PFNGLVERTEXATTRIBPOINTERPROC)COCOS2DX_GLSL_EXTENSION("glVertexAttribPointer");
	__glewEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC)COCOS2DX_GLSL_EXTENSION("glEnableVertexAttribArray");
	__glewDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC)COCOS2DX_GLSL_EXTENSION("glDisableVertexAttribArray");
	
	__glewUniform1f			= (PFNGLUNIFORM1FPROC)COCOS2DX_GLSL_EXTENSION("glUniform1fARB");
	__glewUniform1fv		= (PFNGLUNIFORM1FVPROC)COCOS2DX_GLSL_EXTENSION("glUniform1fvARB");
	__glewUniform2f			= (PFNGLUNIFORM2FPROC)COCOS2DX_GLSL_EXTENSION("glUniform2fARB");
	__glewUniform2fv		= (PFNGLUNIFORM2FVPROC)COCOS2DX_GLSL_EXTENSION("glUniform2fvARB");
	__glewUniform3f			= (PFNGLUNIFORM3FPROC)COCOS2DX_GLSL_EXTENSION("glUniform3fARB");
	__glewUniform3fv		= (PFNGLUNIFORM3FVPROC)COCOS2DX_GLSL_EXTENSION("glUniform3fvARB");
	__glewUniform4f			= (PFNGLUNIFORM4FPROC)COCOS2DX_GLSL_EXTENSION("glUniform4fARB");
	__glewUniform4fv		= (PFNGLUNIFORM4FVPROC)COCOS2DX_GLSL_EXTENSION("glUniform4fvARB");
	__glewUniformMatrix4fv	= (PFNGLUNIFORMMATRIX4FVPROC)COCOS2DX_GLSL_EXTENSION("glUniformMatrix4fvARB");*/
}
#endif

char* CCShaderCache::GetShaderFile(std::string filename)
{
    int		SizeOut;
    char*   pData;
    
    filename = GetBaseAppPath() + filename;
    
    pData = (char*)FileManager::GetFileManager()->Get(filename, &SizeOut, false);
    
    return pData;
}

void CCShaderCache::loadDefaultShaders()
{
    // Position Texture Color shader
    CCGLProgram *p = new CCGLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTextureColor);

    m_pPrograms->setObject(p, kCCShader_PositionTextureColor);
    p->release();

    // Position Texture Color alpha test
    p = new CCGLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTextureColorAlphaTest);

    m_pPrograms->setObject(p, kCCShader_PositionTextureColorAlphaTest);
    p->release();

    //
    // Position, Color shader
    //
    p = new CCGLProgram();
    loadDefaultShader(p, kCCShaderType_PositionColor);

    m_pPrograms->setObject(p, kCCShader_PositionColor);
    p->release();

    //
    // Position Texture shader
    //
    p = new CCGLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTexture);

    m_pPrograms->setObject(p, kCCShader_PositionTexture);
    p->release();

    //
    // Position, Texture attribs, 1 Color as uniform shader
    //
    p = new CCGLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTexture_uColor);

    m_pPrograms->setObject(p ,kCCShader_PositionTexture_uColor);
    p->release();

    //
    // Position Texture A8 Color shader
    //
    p = new CCGLProgram();
    loadDefaultShader(p, kCCShaderType_PositionTextureA8Color);
    
    m_pPrograms->setObject(p, kCCShader_PositionTextureA8Color);
    p->release();

    //
    // Position and 1 color passed as a uniform (to simulate glColor4ub )
    //
    p = new CCGLProgram();
    loadDefaultShader(p, kCCShaderType_Position_uColor);
    
    m_pPrograms->setObject(p, kCCShader_Position_uColor);
    p->release();    
}

void CCShaderCache::reloadDefaultShaders()
{
    // reset all programs and reload them
    
    // Position Texture Color shader
    CCGLProgram *p = programForKey(kCCShader_PositionTextureColor);    
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionTextureColor);

    // Position Texture Color alpha test
    p = programForKey(kCCShader_PositionTextureColorAlphaTest);
    p->reset();    
    loadDefaultShader(p, kCCShaderType_PositionTextureColorAlphaTest);
    
    //
    // Position, Color shader
    //
    p = programForKey(kCCShader_PositionColor);
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionColor);
    
    //
    // Position Texture shader
    //
    p = programForKey(kCCShader_PositionTexture);
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionTexture);
    
    //
    // Position, Texture attribs, 1 Color as uniform shader
    //
    p = programForKey(kCCShader_PositionTexture_uColor);
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionTexture_uColor);
    
    //
    // Position Texture A8 Color shader
    //
    p = programForKey(kCCShader_PositionTextureA8Color);
    p->reset();
    loadDefaultShader(p, kCCShaderType_PositionTextureA8Color);
    
    //
    // Position and 1 color passed as a uniform (to simulate glColor4ub )
    //
    p = programForKey(kCCShader_Position_uColor);
    p->reset();
    loadDefaultShader(p, kCCShaderType_Position_uColor);  
}

void CCShaderCache::loadDefaultShader(CCGLProgram *p, int type)
{
    switch (type) {
        case kCCShaderType_PositionTextureColor:
            p->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccPositionTextureColor_frag);
            
            p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
            p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            
            break;
        case kCCShaderType_PositionTextureColorAlphaTest:
            p->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccPositionTextureColorAlphaTest_frag);
            
            p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
            p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

            break;
        case kCCShaderType_PositionColor:  
            p->initWithVertexShaderByteArray(ccPositionColor_vert ,ccPositionColor_frag);
            
            p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);

            break;
        case kCCShaderType_PositionTexture:
            p->initWithVertexShaderByteArray(ccPositionTexture_vert ,ccPositionTexture_frag);
            
            p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

            break;
        case kCCShaderType_PositionTexture_uColor:
            p->initWithVertexShaderByteArray(ccPositionTexture_uColor_vert, ccPositionTexture_uColor_frag);
            
            p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

            break;
        case kCCShaderType_PositionTextureA8Color:
            p->initWithVertexShaderByteArray(ccPositionTextureA8Color_vert, ccPositionTextureA8Color_frag);
            
            p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
            p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

            break;
        case kCCShaderType_Position_uColor:
            p->initWithVertexShaderByteArray(ccPosition_uColor_vert, ccPosition_uColor_frag);    
            
            p->addAttribute("aVertex", kCCVertexAttrib_Position);    
            
            break;
        default:
            CCLOG("cocos2d: %s:%d, error shader type", __FUNCTION__, __LINE__);
            return;
    }
    
    p->link();
    p->updateUniforms();
    
    CHECK_GL_ERROR_DEBUG();
}

CCGLProgram* CCShaderCache::programForKey(const char* key)
{
    return (CCGLProgram*)m_pPrograms->objectForKey(key);
}

void CCShaderCache::addProgram(CCGLProgram* program, const char* key)
{
    m_pPrograms->setObject(program, key);
}

NS_CC_END
