/* -----------------------------------------------------------------------------------
 *
 *      File            CCGLProgram.cpp
 *      Ported By       Young-Hwan Mun
 *      Contact         xmsoft77@gmail.com 
 * 
 * -----------------------------------------------------------------------------------
 *   
 *      Copyright (c) 2010-2013 XMSoft
 *      Copyright (c) 2010-2013 cocos2d-x.org
 *      Copyright (c) 2011 Jeff Lamarche
 *      Copyright (c) 2012 Goffredo Marocchi
 *      Copyright (c) 2012 Ricardo Quesada
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

//#include "Precompiled.h"
#include "IrrCompileConfig.h"

#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
    #ifdef _IRR_COMPILE_WITH_OGLES1_
		#include "OpenGLES/ES1/gl.h"
		#include "OpenGLES/ES1/glext.h"
	#else
		#include "OpenGLES/ES2/gl.h"
		#include "OpenGLES/ES2/glext.h"
	#endif
#else
    #ifdef _WIN32
		#include "EGL/egl.h"
	#endif

    #ifdef _IRR_COMPILE_WITH_OGLES1_
		#include "GLES/gl.h"
		#include "GLES/glext.h"
	#else
		#include "GLES2/gl2.h"
		#include "GLES2/gl2ext.h"
	#endif
#endif

#include "support/data_support/uthash.h"
#include "cocoshader/CCGLProgram.h"
#include "cocoshader/CCGLStateCache.h"
#include "CCDirector.h"
#include "CCFileUtils.h"
#include "CCString.h"
#include "matrix4.h"
//#include "kazmath/GL/matrix.h"
//#include "kazmath/kazmath.h"

NS_CC_BEGIN

CCGLProgram::CCGLProgram ( void )
{
	m_uProgram			= 0;
	m_uVertShader		= 0;
	m_uFragShader		= 0;
	m_pHashForUniforms	= NULL;
	m_bUsesTime			= false;

    memset ( m_aUniforms, 0, sizeof ( m_aUniforms ) );
}

CCGLProgram::~CCGLProgram ( void )
{
    CCLOGINFO ( "XMCocos2D : %s %d deallocing CCGLProgram = %p", __FUNCTION__, __LINE__, this );

    // there is no need to delete the shaders. They should have been already deleted.
    CCAssert ( m_uVertShader == 0, "Vertex Shaders should have been already deleted" );
    CCAssert ( m_uFragShader == 0, "Fragment Shaders should have been already deleted" );

    if ( m_uProgram ) 
    {
        ccGLDeleteProgram ( m_uProgram );
    }

    tHashUniformEntry*  pCurrentElement;
	tHashUniformEntry*  pTemp;

    // Purge uniform hash
    HASH_ITER ( hh, m_pHashForUniforms, pCurrentElement, pTemp )
	{
        HASH_DEL ( m_pHashForUniforms, pCurrentElement );
        free ( pCurrentElement->value );
        free ( pCurrentElement );
    }
}

bool CCGLProgram::initWithVertexShaderByteArray ( const GLchar* pVShaderByteArray, const GLchar* pFShaderByteArray )
{
#if defined ( USE_OPEN_GLES2 )

	m_uProgram = glCreateProgram ( );
    CHECK_GL_ERROR_DEBUG ( );

    m_uVertShader = m_uFragShader = 0;

    if ( pVShaderByteArray ) 
	{
        if ( !compileShader ( &m_uVertShader, GL_VERTEX_SHADER, pVShaderByteArray ) )
		{
            CCLOG ( "XMCocos2D : ERROR: Failed to compile vertex shader" );
        }

    }

    // Create and compile fragment shader
    if ( pFShaderByteArray ) 
	{
        if ( !compileShader ( &m_uFragShader, GL_FRAGMENT_SHADER, pFShaderByteArray ) )
		{
            CCLOG ( "XMCocos2D : ERROR: Failed to compile fragment shader" );
        }
    }

    if ( m_uVertShader )
	{
        glAttachShader ( m_uProgram, m_uVertShader );
    }
    CHECK_GL_ERROR_DEBUG ( );

    if ( m_uFragShader ) 
	{
        glAttachShader ( m_uProgram, m_uFragShader );
    }
    m_pHashForUniforms = NULL;
    
    CHECK_GL_ERROR_DEBUG ( );

#endif

    return true;
}

/*bool CCGLProgram::initWithVertexShaderFilename ( const char* szVShaderFilename, const char* szFShaderFilename )
{
	const GLchar*  pVertexSource   = (GLchar*) CCString::createWithContentsOfFile ( CCFileUtils::sharedFileUtils ( )->fullPathForFilename ( szVShaderFilename ).c_str ( ) )->getCString ( );
	const GLchar*  pFragmentSource = (GLchar*) CCString::createWithContentsOfFile ( CCFileUtils::sharedFileUtils ( )->fullPathForFilename ( szFShaderFilename ).c_str ( ) )->getCString ( );

    return initWithVertexShaderByteArray ( pVertexSource, pFragmentSource );
}

const char* CCGLProgram::description ( void )
{
    return CCString::createWithFormat ( "<CCGLProgram = %08X | Program = %i, VertexShader = %i, FragmentShader = %i>", this, m_uProgram, m_uVertShader, m_uFragShader )->getCString ( );
}*/

bool CCGLProgram::compileShader ( GLuint* pShader, GLenum uType, const char* pSource )
{
    GLint  nStatus;
 
    if ( !pSource )
	{
        return false;
	}

#if defined ( USE_OPEN_GLES2 )

	/*const char* aSources = pSource;
	{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
     &&
     CC_TARGET_PLATFORM != CC_PLATFORM_LINUX
     &&
    CC_TARGET_PLATFORM != CC_PLATFORM_MAC )
        ( uType == GL_VERTEX_SHADER ? "precision highp float;\n" : "precision mediump float;\n" ),
#endif
        "uniform mat4 CC_PMatrix;\n"
        "uniform mat4 CC_MVMatrix;\n"
        "uniform mat4 CC_MVPMatrix;\n"
        "uniform vec4 CC_Time;\n"
        "uniform vec4 CC_SinTime;\n"
        "uniform vec4 CC_CosTime;\n"
        "uniform vec4 CC_Random01;\n"
        "//CC INCLUDES END\n\n",
        pSource,
    };*/
    
    const char* aSources[] = {pSource};

    *pShader = glCreateShader ( uType );
	
    glShaderSource  ( *pShader, sizeof ( aSources ) / sizeof ( *aSources ), aSources, NULL );
    glCompileShader ( *pShader );    
    glGetShaderiv   ( *pShader, GL_COMPILE_STATUS, &nStatus );

    if ( !nStatus ) 
	{
		GLsizei  nLength;
		glGetShaderiv ( *pShader, GL_SHADER_SOURCE_LENGTH, &nLength );
		GLchar*  szSrc = (GLchar*) malloc ( sizeof ( GLchar ) * nLength );
		
		glGetShaderSource ( *pShader, nLength, NULL, szSrc );
		CCLOG ( "XMCocos2D : ERROR - Failed to compile shader:\n%s", szSrc );
        
        if ( uType == GL_VERTEX_SHADER )
        {
            CCLOG ( "XMCocos2D : %s", vertexShaderLog ( ) );
        }
        else
        {
            CCLOG ( "XMCocos2D : %s", fragmentShaderLog ( ) );
        }

        free ( szSrc );

        kdExit ( 0 );
        
        return GL_FALSE;
    }

#else

	nStatus = GL_TRUE;

#endif

    return GL_TRUE;
}

void CCGLProgram::addAttribute ( const char* szAttributeName, GLuint uIndex )
{
#if defined ( USE_OPEN_GLES2 )
    glBindAttribLocation ( m_uProgram, uIndex, szAttributeName );
#endif
}

void CCGLProgram::updateUniforms ( void )
{   
#if defined ( USE_OPEN_GLES2 )

	m_aUniforms [ kCCUniformPMatrix   ] = glGetUniformLocation ( m_uProgram, kCCUniformPMatrix_s   );
	m_aUniforms [ kCCUniformMVMatrix  ] = glGetUniformLocation ( m_uProgram, kCCUniformMVMatrix_s  );
	m_aUniforms [ kCCUniformMVPMatrix ] = glGetUniformLocation ( m_uProgram, kCCUniformMVPMatrix_s );	
	m_aUniforms [ kCCUniformTime	  ] = glGetUniformLocation ( m_uProgram, kCCUniformTime_s	   );
	m_aUniforms [ kCCUniformSinTime   ] = glGetUniformLocation ( m_uProgram, kCCUniformSinTime_s   );
	m_aUniforms [ kCCUniformCosTime   ] = glGetUniformLocation ( m_uProgram, kCCUniformCosTime_s   );
	
	m_bUsesTime =
	(
		m_aUniforms [ kCCUniformTime	] != -1 ||
		m_aUniforms [ kCCUniformSinTime ] != -1 ||
		m_aUniforms [ kCCUniformCosTime ] != -1
	);
    
	m_aUniforms [ kCCUniformRandom01 ] = glGetUniformLocation ( m_uProgram, kCCUniformRandom01_s );
    m_aUniforms [ kCCUniformSampler  ] = glGetUniformLocation ( m_uProgram, kCCUniformSampler_s  );

#endif

    this->use ( );

	// Since sample most probably won't change, set it to 0 now.
    this->setUniformLocationWith1i ( m_aUniforms [ kCCUniformSampler ], 0 );
}

bool CCGLProgram::link ( void )
{
	GLint  nStatus = GL_TRUE;
#if defined ( USE_OPEN_GLES2 )

	CCAssert ( m_uProgram != 0, "Cannot link invalid program" );
    glLinkProgram ( m_uProgram );

    if ( m_uVertShader )
	{
        glDeleteShader ( m_uVertShader );
	}

    if ( m_uFragShader )
	{
        glDeleteShader ( m_uFragShader );
	}

#if DEBUG
    glGetProgramiv ( m_uProgram, GL_LINK_STATUS, &nStatus );	
    if ( nStatus == GL_FALSE )
    {
        CCLOG ( "XMCocos2D : ERROR: Failed to link program: %i", m_uProgram );
        ccGLDeleteProgram ( m_uProgram );
        m_uProgram = 0;
    }
#endif

#endif

    m_uVertShader = m_uFragShader = 0;

    return ( nStatus == GL_TRUE );
}

void CCGLProgram::use ( void )
{
    ccGLUseProgram ( m_uProgram );
}

const char* CCGLProgram::logForOpenGLObject ( GLuint uObject, GLInfoFunction pInfoFunc, GLLogFunction pLogFunc )
{
#if defined ( USE_OPEN_GLES2 )

    GLint  nLogLength = 0, nCharsWritten = 0;

    pInfoFunc ( uObject, GL_INFO_LOG_LENGTH, &nLogLength );
    if ( nLogLength < 1 )
	{
        return 0;
	}

    GLchar*  pLogBytes = (GLchar*) malloc ( nLogLength );
    pLogFunc ( uObject, nLogLength, &nCharsWritten, pLogBytes );

    CCString*  pLog = CCString::create ( (char*) pLogBytes );

    free ( pLogBytes );

    return pLog->getCString ( );

#else

	return 0;

#endif
}

const char* CCGLProgram::vertexShaderLog ( void )
{
#if defined ( USE_OPEN_GLES2 )
    return this->logForOpenGLObject ( m_uVertShader, (GLInfoFunction) &glGetShaderiv, (GLLogFunction) &glGetShaderInfoLog );
#else
	return 0;
#endif
}

const char* CCGLProgram::fragmentShaderLog ( void )
{
#if defined ( USE_OPEN_GLES2 )
    return this->logForOpenGLObject ( m_uFragShader, (GLInfoFunction) &glGetShaderiv, (GLLogFunction) &glGetShaderInfoLog );
#else
	return 0;
#endif
}

const char* CCGLProgram::programLog ( void )
{
#if defined ( USE_OPEN_GLES2 )
    return this->logForOpenGLObject ( m_uProgram, (GLInfoFunction) &glGetProgramiv, (GLLogFunction) &glGetProgramInfoLog );
#else
	return 0;
#endif
}

// Uniform cache

bool CCGLProgram::updateUniformLocation ( GLint nLocation, GLvoid* pData, unsigned int uBytes )
{
#if defined ( USE_OPEN_GLES2 )
	if ( nLocation < 0 )
    {
        return false;
    }
#endif

    bool              bUpdated = true;
    tHashUniformEntry*  pElement = NULL;
    HASH_FIND_INT ( m_pHashForUniforms, &nLocation, pElement );

    if ( !pElement ) 
	{
        pElement = (tHashUniformEntry*) malloc ( sizeof ( *pElement ) );

        // key
        pElement->location = nLocation;

        // value
        pElement->value = malloc ( uBytes );
        memcpy ( pElement->value, pData, uBytes );

        HASH_ADD_INT ( m_pHashForUniforms, location, pElement );
    }
    else
    {
        if ( memcmp ( pElement->value, pData, uBytes ) == 0 )
		{
            bUpdated = false;
		}
        else
		{
            memcpy ( pElement->value, pData, uBytes );
		}
    }

    return bUpdated;
}

GLint CCGLProgram::getUniformLocationForName ( const char* szName )
{
    CCAssert ( szName != NULL, "Invalid uniform name" );
    CCAssert ( m_uProgram != 0, "Invalid operation. Cannot get uniform location when program is not initialized" );
#if defined ( USE_OPEN_GLES2 )    
    return glGetUniformLocation ( m_uProgram, szName );
#else
	return 0;
#endif
}

void CCGLProgram::setUniformLocationWith1i ( GLint nLocation, GLint i1 )
{
#if defined ( USE_OPEN_GLES2 )
    bool  bUpdated = updateUniformLocation ( nLocation, &i1, sizeof ( i1 ) * 1 );

    if ( bUpdated ) 
    {
        glUniform1i ( nLocation, i1 );
    }
#endif
}

void CCGLProgram::setUniformLocationWith2i ( GLint nLocation, GLint i1, GLint i2 )
{
#if defined ( USE_OPEN_GLES2 )
    GLint   ints[2]  = { i1, i2 };
    bool  bUpdated = updateUniformLocation ( nLocation, ints, sizeof ( ints ) );
    
    if ( bUpdated )
    {
        glUniform2i ( (GLint) nLocation, i1, i2 );
    }
#endif
}

void CCGLProgram::setUniformLocationWith3i ( GLint nLocation, GLint i1, GLint i2, GLint i3 )
{
#if defined ( USE_OPEN_GLES2 )
    GLint  ints[3]  = { i1, i2, i3 };
    bool bUpdated = updateUniformLocation ( nLocation, ints, sizeof ( ints ) );
    
    if ( bUpdated )
    {
        glUniform3i( (GLint) nLocation, i1, i2, i3 );
    }
#endif
}

void CCGLProgram::setUniformLocationWith4i ( GLint nLocation, GLint i1, GLint i2, GLint i3, GLint i4 )
{
#if defined ( USE_OPEN_GLES2 )
    GLint   ints[4]  = { i1, i2, i3, i4 };
    bool  bUpdated = updateUniformLocation ( nLocation, ints, sizeof ( ints ) );
    
    if ( bUpdated )
    {
        glUniform4i ( (GLint) nLocation, i1, i2, i3, i4 );
    }
#endif
}

void CCGLProgram::setUniformLocationWith2iv ( GLint nLocation, GLint* ints, GLuint uNumberOfArrays )
{
#if defined ( USE_OPEN_GLES2 )
    bool  bUpdated = updateUniformLocation ( nLocation, ints, sizeof ( int ) * 2 * uNumberOfArrays );
    
    if ( bUpdated )
    {
        glUniform2iv ( (GLint) nLocation, (GLsizei) uNumberOfArrays, ints );
    }
#endif
}

void CCGLProgram::setUniformLocationWith3iv ( GLint nLocation, GLint* ints, GLuint uNumberOfArrays )
{
#if defined ( USE_OPEN_GLES2 )
    bool bUpdated = updateUniformLocation ( nLocation, ints, sizeof ( int ) * 3 * uNumberOfArrays );
    
    if ( bUpdated )
    {
        glUniform3iv ( (GLint) nLocation, (GLsizei) uNumberOfArrays, ints );
    }
#endif
}

void CCGLProgram::setUniformLocationWith4iv ( GLint nLocation, GLint* ints, GLuint uNumberOfArrays )
{
#if defined ( USE_OPEN_GLES2 )
    bool updated =  updateUniformLocation ( nLocation, ints, sizeof ( int ) * 4 * uNumberOfArrays );
    
    if ( updated )
    {
        glUniform4iv( (GLint) nLocation, (GLsizei) uNumberOfArrays, ints );
    }
#endif
}

void CCGLProgram::setUniformLocationWith1f ( GLint nLocation, GLfloat f1 )
{
#if defined ( USE_OPEN_GLES2 )
    bool  bUpdated = updateUniformLocation ( nLocation, &f1, sizeof ( f1 ) * 1 );

    if ( bUpdated )
    {
        glUniform1f ( nLocation, f1 );
    }
#endif
}

void CCGLProgram::setUniformLocationWith2f ( GLint nLocation, GLfloat f1, GLfloat f2 )
{
#if defined ( USE_OPEN_GLES2 )
    GLfloat  pFloats[2] = { f1, f2 };
    bool   bUpdated = updateUniformLocation ( nLocation, pFloats, sizeof ( pFloats ) );

    if ( bUpdated )
    {
        glUniform2f ( nLocation, f1, f2 );
    }
#endif
}

void CCGLProgram::setUniformLocationWith3f ( GLint nLocation, GLfloat f1, GLfloat f2, GLfloat f3 )
{
#if defined ( USE_OPEN_GLES2 )
    GLfloat  pFloats[3] = { f1, f2, f3 };
    bool   bUpdated = updateUniformLocation ( nLocation, pFloats, sizeof ( pFloats ) );

    if ( bUpdated )
    {
        glUniform3f ( nLocation, f1, f2, f3 );
    }
#endif
}

void CCGLProgram::setUniformLocationWith4f ( GLint nLocation, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4 )
{
#if defined ( USE_OPEN_GLES2 )
    GLfloat  pFloats[4] = { f1, f2, f3, f4 };
    bool   bUpdated = updateUniformLocation ( nLocation, pFloats, sizeof ( pFloats ) );

    if ( bUpdated )
    {
        glUniform4f ( nLocation, f1, f2, f3,f4 );
    }
#endif
}

void CCGLProgram::setUniformLocationWith2fv ( GLint nLocation, GLfloat* pFloats, unsigned int uNumberOfArrays )
{
#if defined ( USE_OPEN_GLES2 )
    bool  bUpdated = updateUniformLocation ( nLocation, pFloats, sizeof ( GLfloat ) * 2 * uNumberOfArrays );

    if ( bUpdated )
    {
        glUniform2fv ( nLocation, (GLsizei) uNumberOfArrays, pFloats );
    }
#endif
}

void CCGLProgram::setUniformLocationWith3fv ( GLint nLocation, GLfloat* pFloats, unsigned int uNumberOfArrays )
{
#if defined ( USE_OPEN_GLES2 )
    bool  bUpdated = updateUniformLocation ( nLocation, pFloats, sizeof ( GLfloat ) * 3 * uNumberOfArrays );

    if ( bUpdated )
    {
        glUniform3fv ( nLocation, (GLsizei) uNumberOfArrays, pFloats );
    }
#endif
}

void CCGLProgram::setUniformLocationWith4fv ( GLint nLocation, GLfloat* pFloats, unsigned int uNumberOfArrays )
{
#if defined ( USE_OPEN_GLES2 )
    bool  bUpdated = updateUniformLocation ( nLocation, pFloats, sizeof ( GLfloat ) * 4 * uNumberOfArrays );

    if ( bUpdated )
    {
        glUniform4fv ( nLocation, (GLsizei) uNumberOfArrays, pFloats );
    }
#endif
}


void CCGLProgram::setUniformLocationWithMatrix4fv ( GLint nLocation, GLfloat* pMatrixArray, unsigned int uNumberOfMatrices )
{
#if defined ( USE_OPEN_GLES2 )
    bool  bUpdated = updateUniformLocation ( nLocation, pMatrixArray, sizeof ( GLfloat ) * 16 * uNumberOfMatrices );

    if ( bUpdated )
    {
        glUniformMatrix4fv ( nLocation, (GLsizei) uNumberOfMatrices, GL_FALSE, pMatrixArray );
    }
#endif
}

void CCGLProgram::setUniformsForBuiltins(void)
{
	//kmMat4  tMatrixP;
    //kmMat4  tMatrixMV;
	irr::core::matrix4	tMatrixP;
	irr::core::matrix4	tMatrixMV;

	//irr::core::matrix4 modelview_matrix_stack;
	//irr::core::matrix4 projection_matrix_stack;
	//irr::core::matrix4 texture_matrix_stack;
    
	//kmGLGetMatrix ( KM_GL_PROJECTION, &tMatrixP );
	//kmGLGetMatrix ( KM_GL_MODELVIEW , &tMatrixMV );

	glGetFloatv(GL_PROJECTION_MATRIX, tMatrixP.pointer() );
	glGetFloatv(GL_MODELVIEW_MATRIX, tMatrixMV.pointer() );

#if defined ( USE_OPEN_GLES2 )
	irr::core::matrix4  tMatrixMVP;
    //kmMat4Multiply ( &tMatrixMVP, &tMatrixP, &tMatrixMV );
	tMatrixMVP = tMatrixP * tMatrixMV;

	setUniformLocationWithMatrix4fv ( m_aUniforms [ kCCUniformPMatrix   ], tMatrixP.pointer()  , 1 );
    setUniformLocationWithMatrix4fv ( m_aUniforms [ kCCUniformMVMatrix  ], tMatrixMV.pointer() , 1 );
    setUniformLocationWithMatrix4fv ( m_aUniforms [ kCCUniformMVPMatrix ], tMatrixMVP.pointer(), 1 );

	if ( m_bUsesTime )
    {
		CCDirector*  pDirector = CCDirector::sharedDirector ( );
		
		// This doesn't give the most accurate global time value.
		// Cocos2D doesn't store a high precision time value, so this will have to do.
		// Getting Mach time per frame per shader using time could be extremely expensive.
        KDfloat  fTime = pDirector->getTotalFrames ( ) * pDirector->getAnimationInterval ( );
		
        setUniformLocationWith4f ( m_aUniforms [ kCCUniformTime    ], fTime / 10.0f, fTime        , fTime * 2    , fTime * 4 );
        setUniformLocationWith4f ( m_aUniforms [ kCCUniformSinTime ], fTime /  8.0f, fTime / 4.0f , fTime / 2.0f , kdSinf ( fTime ) );
        setUniformLocationWith4f ( m_aUniforms [ kCCUniformCosTime ], fTime /  8.0f, fTime / 4.0f , fTime / 2.0f , kdCosf ( fTime ) );
	}

	if ( m_aUniforms [ kCCUniformRandom01 ] != -1 )
    {
        setUniformLocationWith4f ( m_aUniforms [ kCCUniformRandom01 ], CCRANDOM_0_1 ( ), CCRANDOM_0_1 ( ), CCRANDOM_0_1 ( ), CCRANDOM_0_1 ( ) );
	}
#else
	glMatrixMode ( GL_PROJECTION );
	glLoadMatrixf ( tMatrixP.pointer() );

	glMatrixMode ( GL_MODELVIEW );
	glLoadMatrixf ( tMatrixMV.pointer() );
#endif
}

void CCGLProgram::reset ( void )
{
    m_uVertShader = m_uFragShader = 0;
    memset ( m_aUniforms, 0, sizeof ( m_aUniforms ) );
    
    // it is already deallocated by android
    //ccGLDeleteProgram ( m_uProgram );
    m_uProgram = 0;
    
    tHashUniformEntry*  pCurrentElement;
	tHashUniformEntry*  pTemp;
    
    // Purge uniform hash
    HASH_ITER ( hh, m_pHashForUniforms, pCurrentElement, pTemp ) 
    {
        HASH_DEL ( m_pHashForUniforms, pCurrentElement );
        free ( pCurrentElement->value );
        free ( pCurrentElement );
    }
    m_pHashForUniforms = NULL;
}

NS_CC_END
