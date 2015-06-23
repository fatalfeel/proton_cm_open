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

#ifndef __CCSHADERCACHE_H__
#define __CCSHADERCACHE_H__

#include "cocoa/CCDictionary.h"

#if defined(_IRR_COMPILE_WITH_OGLES1_)
	#define GL_DEPTH24_STENCIL8			GL_DEPTH24_STENCIL8_OES

	#define glGenerateMipmap			glGenerateMipmapOES
	#define glGenFramebuffers			glGenFramebuffersOES
	#define glGenRenderbuffers			glGenRenderbuffersOES
	#define glDeleteFramebuffers		glDeleteFramebuffersOES
	#define glDeleteRenderbuffers		glDeleteRenderbuffersOES
	#define glBindFramebuffer			glBindFramebufferOES
	#define glBindRenderbuffer			glBindRenderbufferOES
	#define glFramebufferTexture2D		glFramebufferTexture2DOES
	#define glFramebufferRenderbuffer	glFramebufferRenderbufferOES
	#define glRenderbufferStorage		glRenderbufferStorageOES
	#define glCheckFramebufferStatus	glCheckFramebufferStatusOES

	#define GL_FRAMEBUFFER				GL_FRAMEBUFFER_OES
	#define GL_RENDERBUFFER				GL_RENDERBUFFER_OES
	#define GL_FRAMEBUFFER_BINDING		GL_FRAMEBUFFER_BINDING_OES
	#define GL_RENDERBUFFER_BINDING		GL_RENDERBUFFER_BINDING_OES
	#define GL_COLOR_ATTACHMENT0		GL_COLOR_ATTACHMENT0_OES
	#define GL_DEPTH_ATTACHMENT			GL_DEPTH_ATTACHMENT_OES
	#define GL_STENCIL_ATTACHMENT		GL_STENCIL_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_COMPLETE		GL_FRAMEBUFFER_COMPLETE_OES
	#define GL_NONE                     0

#elif defined(_IRR_COMPILE_WITH_OGLES2_)
	#define GL_DEPTH24_STENCIL8			GL_DEPTH24_STENCIL8_OES

#else
	extern PFNGLGENBUFFERSPROC						__glewGenBuffers;
	extern PFNGLDELETEBUFFERSPROC					__glewDeleteBuffers;
	extern PFNGLBINDBUFFERPROC						__glewBindBuffer;
	extern PFNGLBUFFERDATAPROC						__glewBufferData;
	extern PFNGLBUFFERSUBDATAPROC					__glewBufferSubData;
	extern PFNGLACTIVETEXTUREPROC					__glewActiveTexture;
	extern PFNGLCLIENTACTIVETEXTUREPROC				__glewClientActiveTexture;
	extern PFNGLBINDFRAMEBUFFERPROC					__glewBindFramebuffer;
	extern PFNGLBINDRENDERBUFFERPROC				__glewBindRenderbuffer;
	extern PFNGLGENFRAMEBUFFERSPROC					__glewGenFramebuffers;
	extern PFNGLGENRENDERBUFFERSPROC				__glewGenRenderbuffers;
	extern PFNGLDELETEFRAMEBUFFERSPROC				__glewDeleteFramebuffers;
	extern PFNGLDELETERENDERBUFFERSPROC				__glewDeleteRenderbuffers;
	extern PFNGLFRAMEBUFFERRENDERBUFFERPROC			__glewFramebufferRenderbuffer;
	extern PFNGLRENDERBUFFERSTORAGEPROC				__glewRenderbufferStorage;
	extern PFNGLCHECKFRAMEBUFFERSTATUSPROC			__glewCheckFramebufferStatus;
	extern PFNGLGENERATEMIPMAPPROC					__glewGenerateMipmap;
	extern PFNGLFRAMEBUFFERTEXTURE2DPROC			__glewFramebufferTexture2D;
	extern PFNGLCOMPRESSEDTEXIMAGE2DPROC			__glewCompressedTexImage2D;
		
	/*extern PFNGLCREATEPROGRAMPROC					__glewCreateProgram;
	extern PFNGLDELETEPROGRAMPROC					__glewDeleteProgram;
	extern PFNGLLINKPROGRAMPROC						__glewLinkProgram;
	extern PFNGLUSEPROGRAMPROC						__glewUseProgram;
	extern PFNGLGETPROGRAMINFOLOGPROC				__glewGetProgramInfoLog;
	extern PFNGLGETPROGRAMIVPROC					__glewGetProgramiv;
	extern PFNGLCREATESHADERPROC					__glewCreateShader;
	extern PFNGLATTACHSHADERPROC					__glewAttachShader;
	extern PFNGLSHADERSOURCEPROC					__glewShaderSource;
	extern PFNGLCOMPILESHADERPROC					__glewCompileShader;
	extern PFNGLDELETESHADERPROC					__glewDeleteShader;
	extern PFNGLGETSHADERINFOLOGPROC				__glewGetShaderInfoLog;
	extern PFNGLGETSHADERIVPROC						__glewGetShaderiv;
	extern PFNGLBINDATTRIBLOCATIONPROC				__glewBindAttribLocation;
	extern PFNGLGETUNIFORMLOCATIONPROC				__glewGetUniformLocation;
	extern PFNGLVERTEXATTRIBPOINTERPROC				__glewVertexAttribPointer;
	extern PFNGLENABLEVERTEXATTRIBARRAYPROC			__glewEnableVertexAttribArray;
	extern PFNGLDISABLEVERTEXATTRIBARRAYPROC		__glewDisableVertexAttribArray;

	extern PFNGLUNIFORM1FPROC						__glewUniform1f;
	extern PFNGLUNIFORM1FVPROC						__glewUniform1fv;
	extern PFNGLUNIFORM2FPROC						__glewUniform2f;
	extern PFNGLUNIFORM2FVPROC						__glewUniform2fv;
	extern PFNGLUNIFORM3FPROC						__glewUniform3f;
	extern PFNGLUNIFORM3FVPROC						__glewUniform3fv;
	extern PFNGLUNIFORM4FPROC						__glewUniform4f;
	extern PFNGLUNIFORM4FVPROC						__glewUniform4fv;
	extern PFNGLUNIFORMMATRIX4FVPROC				__glewUniformMatrix4fv;*/

///////////
	#define glGenBuffers					__glewGenBuffers
	#define glDeleteBuffers					__glewDeleteBuffers
	#define glBindBuffer					__glewBindBuffer
	#define glBufferData					__glewBufferData
	#define glBufferSubData					__glewBufferSubData
	#define glActiveTexture					__glewActiveTexture
	#define glClientActiveTexture			__glewClientActiveTexture
	#define glBindFramebuffer				__glewBindFramebuffer
	#define glBindRenderbuffer				__glewBindRenderbuffer
	#define glGenFramebuffers				__glewGenFramebuffers
	#define glGenRenderbuffers				__glewGenRenderbuffers
	#define glDeleteFramebuffers			__glewDeleteFramebuffers
	#define glDeleteRenderbuffers			__glewDeleteRenderbuffers
	#define glFramebufferRenderbuffer		__glewFramebufferRenderbuffer
	#define glRenderbufferStorage			__glewRenderbufferStorage
	#define glCheckFramebufferStatus		__glewCheckFramebufferStatus
	#define glGenerateMipmap				__glewGenerateMipmap
	#define glFramebufferTexture2D			__glewFramebufferTexture2D
	#define glCompressedTexImage2D			__glewCompressedTexImage2D
		
	/*#define glCreateProgram				__glewCreateProgram
	#define glDeleteProgram					__glewDeleteProgram
	#define glLinkProgram					__glewLinkProgram
	#define glUseProgram					__glewUseProgram
	#define glGetProgramInfoLog				__glewGetProgramInfoLog
	#define glGetProgramiv					__glewGetProgramiv
	#define glCreateShader					__glewCreateShader
	#define glAttachShader					__glewAttachShader
	#define glShaderSource					__glewShaderSource
	#define glCompileShader					__glewCompileShader
	#define glDeleteShader					__glewDeleteShader
	#define glGetShaderInfoLog				__glewGetShaderInfoLog
	#define glGetShaderiv					__glewGetShaderiv
	#define glBindAttribLocation			__glewBindAttribLocation
	#define glGetUniformLocation			__glewGetUniformLocation
	#define glEnableVertexAttribArray		__glewEnableVertexAttribArray
	#define glDisableVertexAttribArray		__glewDisableVertexAttribArray
	#define glVertexAttribPointer			__glewVertexAttribPointer
	#define glUniform1f						__glewUniform1f
	#define glUniform1fv					__glewUniform1fv
	#define glUniform2f						__glewUniform2f
	#define glUniform2fv					__glewUniform2fv
	#define glUniform3f						__glewUniform3f
	#define glUniform3fv					__glewUniform3fv
	#define glUniform4f						__glewUniform4f
	#define glUniform4fv					__glewUniform4fv
	#define glUniformMatrix4fv				__glewUniformMatrix4fv*/
#endif

NS_CC_BEGIN

class CCGLProgram;

/**
 * @addtogroup shaders
 * @{
 */

/** CCShaderCache
 Singleton that stores manages GL shaders
 @since v2.0
 */
class CC_DLL CCShaderCache : public CCObject 
{
public:
    CCShaderCache();

    virtual ~CCShaderCache();
    /** returns the shared instance */
    static CCShaderCache* sharedShaderCache();

    /** purges the cache. It releases the retained instance. */
    static void purgeSharedShaderCache();

	char* GetShaderFile(std::string filename);
    
	/** loads the default shaders */
    void loadDefaultShaders();
	    
    /** reload the default shaders */
    void reloadDefaultShaders();

    /** returns a GL program for a given key */
    CCGLProgram * programForKey(const char* key);

    /** adds a CCGLProgram to the cache for a given name */
    void addProgram(CCGLProgram* program, const char* key);

private:
    bool init();
    void loadDefaultShader(CCGLProgram *program, int type);

#if defined(_IRR_COMPILE_WITH_OPENGL_)		
	void glewInit();
#endif

    CCDictionary* m_pPrograms;

};

// end of shaders group
/// @}

NS_CC_END

#endif /* __CCSHADERCACHE_H__ */
