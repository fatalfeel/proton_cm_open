/* -----------------------------------------------------------------------------------
 *
 *      File            CCShaderCache.cpp
 *      Ported By       Young-Hwan Mun
 *      Contact         xmsoft77@gmail.com 
 * 
 * -----------------------------------------------------------------------------------
 *   
 *      Copyright (c) 2010-2013 XMSoft
 *      Copyright (c) 2010-2013 cocos2d-x.org
 *      Copyright (c) 2011      Ricardo Quesada
 *      Copyright (c) 2011      Zynga Inc.
 *
 *         http://www.cocos2d-x.org      
 *
 * -----------------------------------------------------------------------------------
 * 
 *     Permission is hereby granted, free of charge, to any person obtaining a copy
 *     of this software and associated documentation files (the "Software"), to deal
 *     in the Software without restriction, including without limitation the rights
 *     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *     copies of the Software, and to permit persons to whom the Software is
 *     furnished to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included in
 *     all copies or substantial portions of the Software.
 *     
 *     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *     THE SOFTWARE.
 *
 * ----------------------------------------------------------------------------------- */ 

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES1_

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

#else

#ifdef __APPLE__
    #include "OpenGLES/ES2/gl2.h"
    #include "OpenGLES/ES2/gl2ext.h"
#else
    #ifdef _WIN32
		#include "EGL/egl.h"
	#endif

    #include "GLES/gl2.h"
    #include "GLES/gl2ext.h"
#endif

#endif

//#include "Precompiled.h"
//#include <CoreFoundation/CoreFoundation.h>
//#include "CCDictionary.h"
#include "PlatFormEnums.h"
#include "CCMutableDictionary.h"
#include "cocoshader/CCGLProgram.h"
#include "cocoshader/CCShaderCache.h"
#include "cocoshader/CCShaders.h"

NS_CC_BEGIN


enum
{
    kCCShaderType_PositionTextureColor			,
    kCCShaderType_PositionTextureColorAlphaTest	,
    kCCShaderType_PositionColor					,
    kCCShaderType_PositionTexture				,
    kCCShaderType_PositionTexture_uColor		,
    kCCShaderType_PositionTextureA8Color		,
    kCCShaderType_Position_uColor				,
	kCCShaderType_PositionLengthTexureColor		,
    
    kCCShaderType_MAX							,
};

char* ccPosition_uColor_frag;
//#include "ccShader_Position_uColor_frag"
char* ccPosition_uColor_vert;
//#include "ccShader_Position_uColor_vert"

char* ccPositionColor_frag;
//#include "ccShader_PositionColor_frag"
char* ccPositionColor_vert;
//#include "ccShader_PositionColor_vert"


char* ccPositionTexture_frag;
//#include "ccShader_PositionTexture_frag"
char* ccPositionTexture_vert;
//#include "ccShader_PositionTexture_vert"

char* ccPositionTextureA8Color_frag;
//#include "ccShader_PositionTextureA8Color_frag"
char* ccPositionTextureA8Color_vert;
//#include "ccShader_PositionTextureA8Color_vert"

char* ccPositionTextureColor_frag;
//#include "ccShader_PositionTextureColor_frag"
char* ccPositionTextureColor_vert;
//#include "ccShader_PositionTextureColor_vert"

char* ccPositionTexture_uColor_frag;
//#include "ccShader_PositionTexture_uColor_frag"
char* ccPositionTexture_uColor_vert;
//#include "ccShader_PositionTexture_uColor_vert"

char* ccPositionColorLengthTexture_frag;
//#include "ccShader_PositionColorLengthTexture_frag"
char* ccPositionColorLengthTexture_vert;
//#include "ccShader_PositionColorLengthTexture_vert"

char* ccPositionTextureColorAlphaTest_frag;
//#include "ccShader_PositionTextureColorAlphaTest_frag"
char* ccExSwitchMask_frag;
//#include "ccShaderEx_SwitchMask_frag"

static CCShaderCache*   l_pSharedShaderCache = NULL;

CCShaderCache* CCShaderCache::sharedShaderCache ( void )
{
    if ( !l_pSharedShaderCache )
	{
        l_pSharedShaderCache = new CCShaderCache ( );
        if ( !l_pSharedShaderCache->init ( ) )
        {
            CC_SAFE_DELETE ( l_pSharedShaderCache );
        }
    }

    return l_pSharedShaderCache;
}

void CCShaderCache::PurgeSharedShaderCache ( void )
{
    CC_SAFE_RELEASE ( l_pSharedShaderCache );
}

CCShaderCache::CCShaderCache ( void )
{
	m_pPrograms = 0;
}

CCShaderCache::~CCShaderCache ( void )
{
    CCLOGINFO ( "XMCocos2D deallocing CCShaderCache = %p", this );
    m_pPrograms->release ( );
    
#ifdef USE_OPEN_GLES2
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
    
    delete ccPositionColorLengthTexture_frag;
    delete ccPositionColorLengthTexture_vert;
    
    delete ccPositionTextureColorAlphaTest_frag;
    delete ccExSwitchMask_frag;
#endif
}

bool CCShaderCache::init ( void )
{
    m_pPrograms = new CCDictionary<std::string, CCGLProgram*>();
    
#ifdef USE_OPEN_GLES2
    ccPosition_uColor_frag =
    GetShaderFile("shaders/ccShader_Position_uColor.frag");
    ccPosition_uColor_vert =
    GetShaderFile("shaders/ccShader_Position_uColor.vert");
    
    ccPositionColor_frag=
    GetShaderFile("shaders/ccShader_PositionColor.frag");
    ccPositionColor_vert=
    GetShaderFile("shaders/ccShader_PositionColor.vert");
    
    ccPositionTexture_frag=
    GetShaderFile("shaders/ccShader_PositionTexture.frag");
    ccPositionTexture_vert=
    GetShaderFile("shaders/ccShader_PositionTexture.vert");
    
    ccPositionTextureA8Color_frag=
    GetShaderFile("shaders/ccShader_PositionTextureA8Color.frag");
    ccPositionTextureA8Color_vert=
    GetShaderFile("shaders/ccShader_PositionTextureA8Color.vert");
    
    ccPositionTextureColor_frag=
    GetShaderFile("shaders/ccShader_PositionTextureColor.frag");
    ccPositionTextureColor_vert=
    GetShaderFile("shaders/ccShader_PositionTextureColor.vert");
    
    ccPositionTexture_uColor_frag=
    GetShaderFile("shaders/ccShader_PositionTexture_uColor.frag");
    ccPositionTexture_uColor_vert=
    GetShaderFile("shaders/ccShader_PositionTexture_uColor.vert");
    
    ccPositionColorLengthTexture_frag=
    GetShaderFile("shaders/ccShader_PositionColorLengthTexture.frag");
    ccPositionColorLengthTexture_vert=
    GetShaderFile("shaders/ccShader_PositionColorLengthTexture.vert");
    
    ccPositionTextureColorAlphaTest_frag=
    GetShaderFile("shaders/ccShader_PositionTextureColorAlphaTest.frag");
    ccExSwitchMask_frag=
    GetShaderFile("shaders/ccShaderEx_SwitchMask.frag");
#endif
    
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WINRT ) || ( CC_TARGET_PLATFORM == CC_PLATFORM_WP8 )
	GLboolean	bHasCompiler = true;
	glGetBooleanv ( GL_SHADER_COMPILER, &bHasCompiler );
	if ( bHasCompiler )
	{
		LoadDefaultShaders ( );
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WINRT ) && defined ( CC_PLATFORM_WINRT_SAVE_SHADERS )
		savePrecompiledShaders ( m_pPrograms );
#endif
	}
	else
	{
		loadDefaultPrecompiledShaders ( );
	}
#else
	LoadDefaultShaders ( );
#endif

	return true;
}

char* CCShaderCache::GetShaderFile(std::string filename)
{
    long    SizeOut;
    char*   pData;
    FILE*   fp;
    
    filename = GetBaseAppPath() + filename;
    
    fp = fopen(filename.c_str(), "rb");
    
    if (!fp)
    {
        //file not found
        if (!fp)
            return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    SizeOut = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    pData = new char[SizeOut+1];

    if (!pData)
    {
        return 0;
    }

    pData[SizeOut] = 0;
    fread(pData, 1, SizeOut, fp);
    fclose(fp);
    
    return pData;
}

void CCShaderCache::LoadDefaultShaders ( void )
{
    // Position Texture Color shader
    CCGLProgram*  pProgram = new CCGLProgram ( );
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTextureColor );

    m_pPrograms->setObject ( pProgram, kCCShader_PositionTextureColor );
    pProgram->release ( );

    // Position Texture Color alpha test
    pProgram = new CCGLProgram ( );
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTextureColorAlphaTest );

    m_pPrograms->setObject ( pProgram, kCCShader_PositionTextureColorAlphaTest );
    pProgram->release ( );

    //
    // Position, Color shader
    //
    pProgram = new CCGLProgram ( );
    LoadDefaultShader ( pProgram, kCCShaderType_PositionColor );

    m_pPrograms->setObject ( pProgram, kCCShader_PositionColor );
    pProgram->release ( );

    //
    // Position Texture shader
    //
    pProgram = new CCGLProgram ( );
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTexture );

    m_pPrograms->setObject ( pProgram, kCCShader_PositionTexture );
    pProgram->release ( );

    //
    // Position, Texture attribs, 1 Color as uniform shader
    //
    pProgram = new CCGLProgram ( );
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTexture_uColor );

    m_pPrograms->setObject ( pProgram, kCCShader_PositionTexture_uColor );
    pProgram->release ( );

    //
    // Position Texture A8 Color shader
    //
    pProgram = new CCGLProgram ( );
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTextureA8Color );
    
    m_pPrograms->setObject ( pProgram, kCCShader_PositionTextureA8Color );
    pProgram->release ( );

    //
    // Position and 1 color passed as a uniform (to simulate glColor4ub )
    //
    pProgram = new CCGLProgram ( );
    LoadDefaultShader ( pProgram, kCCShaderType_Position_uColor );
    
    m_pPrograms->setObject ( pProgram, kCCShader_Position_uColor );
    pProgram->release ( );    

	//
	// Position, Legth(TexCoords, Color (used by Draw Node basically )
	//
    pProgram = new CCGLProgram ( );
    LoadDefaultShader ( pProgram, kCCShaderType_PositionLengthTexureColor );
    
    m_pPrograms->setObject ( pProgram, kCCShader_PositionLengthTexureColor );
    pProgram->release ( );
}

#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WINRT ) || ( CC_TARGET_PLATFORM == CC_PLATFORM_WP8 )
void CCShaderCache::loadDefaultPrecompiledShaders ( void )
{
	// Position Texture Color shader
	CCGLProgram*	pProgram = new CCGLProgram ( );
	loadDefaultPrecompiledShader ( pProgram, kCCShaderType_PositionTextureColor );
	m_pPrograms->setObject ( pProgram, kCCShader_PositionTextureColor );
	pProgram->release ( );

	// Position Texture Color alpha test
	pProgram = new CCGLProgram ( );
	loadDefaultPrecompiledShader ( pProgram, kCCShaderType_PositionTextureColorAlphaTest );

	m_pPrograms->setObject ( pProgram, kCCShader_PositionTextureColorAlphaTest );
	pProgram->release ( );

	//
	// Position, Color shader
	//
	pProgram = new CCGLProgram ( );
	loadDefaultPrecompiledShader ( pProgram, kCCShaderType_PositionColor );

	m_pPrograms->setObject ( pProgram, kCCShader_PositionColor );
	pProgram->release();

	//
	// Position Texture shader
	//
	pProgram = new CCGLProgram ( );
	loadDefaultPrecompiledShader ( pProgram, kCCShaderType_PositionTexture );

	m_pPrograms->setObject ( pProgram, kCCShader_PositionTexture );
	pProgram->release ( );

	//
	// Position, Texture attribs, 1 Color as uniform shader
	//
	pProgram = new CCGLProgram ( );
	loadDefaultPrecompiledShader ( pProgram, kCCShaderType_PositionTexture_uColor );

	m_pPrograms->setObject ( pProgram, kCCShader_PositionTexture_uColor );
	pProgram->release ( );

	//
	// Position Texture A8 Color shader
	//
	pProgram = new CCGLProgram ( );
	loadDefaultPrecompiledShader ( pProgram, kCCShaderType_PositionTextureA8Color );

	m_pPrograms->setObject ( pProgram, kCCShader_PositionTextureA8Color );
	pProgram->release ( );

	//
	// Position and 1 color passed as a uniform (to simulate glColor4ub )
	//
	pProgram = new CCGLProgram ( );
	loadDefaultPrecompiledShader ( pProgram, kCCShaderType_Position_uColor );

	m_pPrograms->setObject ( pProgram, kCCShader_Position_uColor );
	pProgram->release();

	//
	// Position, Length(TexCoords, Color (used by Draw Node basically )
	//
	pProgram = new CCGLProgram ( );
	loadDefaultPrecompiledShader ( pProgram, kCCShaderType_PositionLengthTexureColor );

	m_pPrograms->setObject ( pProgram, kCCShader_PositionLengthTexureColor );
	pProgram->release ( );
}
#endif

void CCShaderCache::ReloadDefaultShaders ( void )
{
    // reset all programs and reload them
    
    // Position Texture Color shader
    CCGLProgram*  pProgram = programForKey ( kCCShader_PositionTextureColor );    
    pProgram->reset ( );   
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTextureColor );

    // Position Texture Color alpha test
    pProgram = programForKey ( kCCShader_PositionTextureColorAlphaTest );
    pProgram->reset ( );    
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTextureColorAlphaTest );
    
    //
    // Position, Color shader
    //
    pProgram = programForKey ( kCCShader_PositionColor );
    pProgram->reset ( );   
    LoadDefaultShader ( pProgram, kCCShaderType_PositionColor );
    
    //
    // Position Texture shader
    //
    pProgram = programForKey ( kCCShader_PositionTexture );
    pProgram->reset ( );   
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTexture );
    
    //
    // Position, Texture attribs, 1 Color as uniform shader
    //
    pProgram = programForKey ( kCCShader_PositionTexture_uColor );
    pProgram->reset ( );   
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTexture_uColor );
    
    //
    // Position Texture A8 Color shader
    //
    pProgram = programForKey ( kCCShader_PositionTextureA8Color );
    pProgram->reset ( );   
    LoadDefaultShader ( pProgram, kCCShaderType_PositionTextureA8Color );
    
    //
    // Position and 1 color passed as a uniform (to simulate glColor4ub )
    //
    pProgram = programForKey ( kCCShader_Position_uColor );
    pProgram->reset ( );   
    LoadDefaultShader ( pProgram, kCCShaderType_Position_uColor );  

	//
	// Position, Legth(TexCoords, Color (used by Draw Node basically )
	//
    pProgram = programForKey ( kCCShader_PositionLengthTexureColor );
    pProgram->reset ( );
    LoadDefaultShader ( pProgram, kCCShaderType_PositionLengthTexureColor );
}

void CCShaderCache::LoadDefaultShader ( CCGLProgram* pProgram, int nType )
{
    switch ( nType )
	{
        case kCCShaderType_PositionTextureColor :
            
			pProgram->initWithVertexShaderByteArray ( ccPositionTextureColor_vert, ccPositionTextureColor_frag );
            
            pProgram->addAttribute ( kCCAttributeNamePosition, kCCVertexAttrib_Position  );
            pProgram->addAttribute ( kCCAttributeNameColor   , kCCVertexAttrib_Color     );
            pProgram->addAttribute ( kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords );
            
            break;

        case kCCShaderType_PositionTextureColorAlphaTest :
            
			pProgram->initWithVertexShaderByteArray ( ccPositionTextureColor_vert, ccPositionTextureColorAlphaTest_frag );
            
            pProgram->addAttribute ( kCCAttributeNamePosition, kCCVertexAttrib_Position  );
            pProgram->addAttribute ( kCCAttributeNameColor   , kCCVertexAttrib_Color     );
            pProgram->addAttribute ( kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords );

            break;

        case kCCShaderType_PositionColor :

            pProgram->initWithVertexShaderByteArray ( ccPositionColor_vert ,ccPositionColor_frag );
            
            pProgram->addAttribute ( kCCAttributeNamePosition, kCCVertexAttrib_Position );
            pProgram->addAttribute ( kCCAttributeNameColor   , kCCVertexAttrib_Color    );

            break;

        case kCCShaderType_PositionTexture :
            pProgram->initWithVertexShaderByteArray ( ccPositionTexture_vert ,ccPositionTexture_frag );
            
            pProgram->addAttribute ( kCCAttributeNamePosition, kCCVertexAttrib_Position  );
            pProgram->addAttribute ( kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords );

            break;

        case kCCShaderType_PositionTexture_uColor :

            pProgram->initWithVertexShaderByteArray ( ccPositionTexture_uColor_vert, ccPositionTexture_uColor_frag );
            
            pProgram->addAttribute ( kCCAttributeNamePosition, kCCVertexAttrib_Position  );
            pProgram->addAttribute ( kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords );

            break;

        case kCCShaderType_PositionTextureA8Color :

            pProgram->initWithVertexShaderByteArray ( ccPositionTextureA8Color_vert, ccPositionTextureA8Color_frag );
            
            pProgram->addAttribute ( kCCAttributeNamePosition, kCCVertexAttrib_Position  );
            pProgram->addAttribute ( kCCAttributeNameColor   , kCCVertexAttrib_Color     );
            pProgram->addAttribute ( kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords );

            break;

        case kCCShaderType_Position_uColor :

            pProgram->initWithVertexShaderByteArray ( ccPosition_uColor_vert, ccPosition_uColor_frag );    
            
            pProgram->addAttribute ( "aVertex", kCCVertexAttrib_Position );    
            
            break;

		case kCCShaderType_PositionLengthTexureColor :

            pProgram->initWithVertexShaderByteArray ( ccPositionColorLengthTexture_vert, ccPositionColorLengthTexture_frag );
            
            pProgram->addAttribute ( kCCAttributeNamePosition, kCCVertexAttrib_Position  );
            pProgram->addAttribute ( kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords );
            pProgram->addAttribute ( kCCAttributeNameColor   , kCCVertexAttrib_Color     );
            
            break;

        default :

            CCLOG ( "XMCocos2D : %s:%d, error shader type", __FUNCTION__, __LINE__ );
            
			return;
    }
    
    pProgram->link ( );
    pProgram->updateUniforms ( );
    //CHECK_GL_ERROR_DEBUG ( );
}

CCGLProgram* CCShaderCache::programForKey ( const char* szKey )
{
    return (CCGLProgram*) m_pPrograms->objectForKey ( szKey );
}

void CCShaderCache::addProgram ( CCGLProgram* pProgram, const char* szKey )
{
    m_pPrograms->setObject ( pProgram, szKey );
}

NS_CC_END
