#pragma once

#include <winsock2.h>
#include <windows.h>
#include <TCHAR.h>
#include <time.h>
#include "IrrCompileConfig.h"

#ifndef PLATFORM_WINDOWS
	#define PLATFORM_WINDOWS
#endif

#if defined(_IRR_COMPILE_WITH_OGLES1_)
	#define GL_GLEXT_PROTOTYPES
	#include "GLES/gl.h"
	#include "GLES/glext.h"

	#define glTexParameterx			glTexParameteri
	#define glFrustumf				glFrustum
	#define glColor4x(r,g,b,a)		glColor4f( (float(r)/65536.0f),  (float(g)/65536.0f) , (float(b)/65536.0f), (float(a)/65536.0f));

#elif defined(_IRR_COMPILE_WITH_OGLES2_)
	#define GL_GLEXT_PROTOTYPES
	#include "GLES2/gl2.h"
	#include "GLES2/gl2ext.h"
						
	#define glTexParameterx			glTexParameteri
	#define glFrustumf				glFrustum
	#define glColor4x(r,g,b,a)		glColor4f( (float(r)/65536.0f),  (float(g)/65536.0f) , (float(b)/65536.0f), (float(a)/65536.0f));

#elif defined(_IRR_COMPILE_WITH_OPENGL_)
	#include <GL/gl.h>
	#include "Irrlicht/source/glext.h"
#endif

//#ifndef M_PI
//#define M_PI 3.141592f
//#endif
/* The following definitions are the same across platforms.  This first
** group are the sanctioned types.
*/
#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;     /* Boolean value type. */
#define _BOOLEAN_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int32;       /* Signed 32 bit value */

//for mysql
#define HAVE_INT32

#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  signed char        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

#if defined(__GNUC__)
#define __int64 long long
#endif

#ifndef _UINT64_DEFINED
typedef  unsigned __int64   uint64;      /* Unsigned 64 bit value */
#define _UINT64_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef  __int64            int64;       /* Signed 64 bit value */
#define _INT64_DEFINED
#endif

#ifndef _SINT64_DEFINED
typedef  __int64            sint64;       /* Signed 64 bit value */
#define _SINT64_DEFINED
#endif

#ifndef _BYTE_DEFINED
typedef  unsigned char      byte;        /* byte type */
#define  _BYTE_DEFINED
#endif


#include "WinUtils.h"

#ifdef RT_FLASH_TEST
#include "flash/app/cpp/GLFlashAdaptor.h"
#endif
