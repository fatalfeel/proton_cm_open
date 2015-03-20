/* -----------------------------------------------------------------------------------
 *
 *      File            CCShaderCache.h
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

#ifndef __CCShaderCache_h__
#define __CCShaderCache_h__

#include "ccConfig.h"

NS_CC_BEGIN

/**
 * @addtogroup shaders
 * @{
 */

class CCGLProgram;

/** 
 *	CCShaderCache
 *	Singleton that stores manages GL shaders
 *	@since v2.0
 */
class CCShaderCache : public CCObject 
{
public:
	         CCShaderCache ( void );
	virtual ~CCShaderCache ( void );

	virtual bool			init ( void );

	/** returns the shared instance */
	static  CCShaderCache*	sharedShaderCache ( void );

	/** purges the cache. It releases the retained instance. */
	static  void			PurgeSharedShaderCache ( void );

	/** loads the default shaders */
	void					LoadDefaultShaders ( void );
    
	/** reload the default shaders */
	void					ReloadDefaultShaders ( void );

	/** returns a GL program for a given key 
	 *  @js getProgram
	 */
	CCGLProgram*			programForKey ( const char* szKey );

	/** adds a CCGLProgram to the cache for a given name */
	void					addProgram ( CCGLProgram* pProgram, const char* szKey );

	void					LoadDefaultShader ( CCGLProgram* pProgram, int nType );

	char*					GetShaderFile(std::string filename);

private:
	CCDictionary<std::string, CCGLProgram*>* m_pPrograms;
};

// end of shaders group
/// @}

NS_CC_END

#endif // __CCShaderCache_h__
