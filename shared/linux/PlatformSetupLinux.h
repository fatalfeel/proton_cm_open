#ifndef PlatformSetupLinux_h__
#define PlatformSetupLinux_h__

//need these for the real compile
#include <syslog.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "LinuxUtils.h"
//#include <GLES/gl.h>
#include <stdarg.h>

#include <cassert>
#include <sys/time.h>

#ifndef _CONSOLE

#ifdef C_GL_MODE

#include "Renderer/GL/gl.h"

#ifdef _IRR_STATIC_LIB_
#include "Irrlicht/source/glext.h"
#endif

//help with compatibility so I can use the GL ES calls with normal GL

#define glTexParameterx glTexParameteri
#define glFrustumf glFrustum
#define glOrthof glOrtho
#define glColor4x(r,g,b,a) glColor4f( (float(r)/65536.0f),  (float(g)/65536.0f) , (float(b)/65536.0f), (float(a)/65536.0f));
#define glActiveTexture glActiveTextureARB
#define glClientActiveTexture glClientActiveTextureARB

#else

#ifdef _IRR_STATIC_LIB_
#include "Irrlicht/source/gles-ext.h"
#endif

#endif  // #ifdef C_GL_MODE

#endif  // #ifndef _CONSOLE

#ifndef PLATFORM_LINUX
#define PLATFORM_LINUX
#endif

#ifndef M_PI
#define M_PI 3.141592f
#endif


#ifndef _SINT64
typedef int64_t sint64;
#define _SINT64
#endif

#ifndef _UINT64
typedef uint64_t uint64;
#define _UINT64
#endif

#ifndef HAVE_UINT64
 //need this to avoid mysql trying to redine uint64
#define HAVE_UINT64
#endif


#ifndef _SINT32
typedef int32_t sint32;
#define _SINT32
#endif
#ifndef _SINT16
typedef int16_t sint16;
#define _SINT16
#endif
#ifndef _SINT8
typedef int8_t sint8;
#define _SINT8
#endif
#ifndef _UINT32
typedef uint32_t uint32;
#define _UINT32
#endif
#ifndef _UINT16
typedef uint16_t uint16;
#define _UINT16
#endif
#ifndef _UINT8
typedef uint8_t uint8;
#define _UINT8
#endif

#ifndef _INT32
typedef int32_t int32;
#define _INT32
#endif

#ifndef _INT16
typedef int16_t int16;
#define _INT16
#endif

#ifndef _INT8_DEFINED
typedef  signed char        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

#if defined(__GNUC__)
#define __int64 long long
#endif

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define _strnicmp strncasecmp

#ifndef _INT64_DEFINED
typedef  __int64            int64;       /* Signed 64 bit value */
#define _INT64_DEFINED
#endif

#ifndef _BYTE
typedef uint8_t byte;
#define _BYTE
#endif
#endif // PlatformSetupLinux_h__
