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

NS_CC_BEGIN

#ifdef _IRR_COMPILE_WITH_OGLES1_
	#define glGenerateMipmap					glGenerateMipmapOES
	#define glGenFramebuffers					glGenFramebuffersOES
	#define glGenRenderbuffers					glGenRenderbuffersOES
	#define glDeleteFramebuffers				glDeleteFramebuffersOES
	#define glDeleteRenderbuffers				glDeleteRenderbuffersOES
	#define glBindFramebuffer					glBindFramebufferOES
	#define glBindRenderbuffer					glBindRenderbufferOES
	#define glFramebufferTexture2D				glFramebufferTexture2DOES
	#define glFramebufferRenderbuffer			glFramebufferRenderbufferOES
	#define glRenderbufferStorage				glRenderbufferStorageOES
	#define glCheckFramebufferStatus			glCheckFramebufferStatusOES

	#define GL_FRAMEBUFFER					GL_FRAMEBUFFER_OES
	#define GL_RENDERBUFFER					GL_RENDERBUFFER_OES
	#define GL_FRAMEBUFFER_BINDING			GL_FRAMEBUFFER_BINDING_OES
	#define GL_RENDERBUFFER_BINDING			GL_RENDERBUFFER_BINDING_OES
	#define GL_COLOR_ATTACHMENT0			GL_COLOR_ATTACHMENT0_OES
	#define GL_DEPTH_ATTACHMENT				GL_DEPTH_ATTACHMENT_OES
	#define GL_STENCIL_ATTACHMENT			GL_STENCIL_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_COMPLETE			GL_FRAMEBUFFER_COMPLETE_OES
#endif
	#define GL_DEPTH24_STENCIL8				GL_DEPTH24_STENCIL8_OES

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

    CCDictionary* m_pPrograms;

};

// end of shaders group
/// @}

NS_CC_END

#endif /* __CCSHADERCACHE_H__ */
