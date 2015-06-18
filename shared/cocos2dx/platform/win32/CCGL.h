/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

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

#ifndef __CCGL_H__
#define __CCGL_H__

#ifdef _IRR_COMPILE_WITH_OPENGL_
	#include <windows.h>
	#include <GL/gl.h>
	#include "Irrlicht/source/glext.h"
#else
	#define GL_GLEXT_PROTOTYPES	

	#include "GLES/gl.h"
	#include "GLES/glext.h"

	#ifdef _IRR_COMPILE_WITH_OGLES2_
		#include "GLES2/gl2.h"
		#include "GLES2/gl2ext.h"
	#endif
#endif

//#define CC_GL_DEPTH24_STENCIL8		GL_DEPTH24_STENCIL8

#endif // __CCGL_H__
