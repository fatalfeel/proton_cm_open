/*
 * libxml.h: internal header only used during the compilation of libxml
 *
 * See COPYRIGHT for the status of this software
 *
 * Author: breese@users.sourceforge.net
 */

#ifndef __XML_LIBXML_H__
#define __XML_LIBXML_H__

#ifndef NO_LARGEFILE_SOURCE
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#endif

#include "config.h"
#include <xmlversion.h>

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#ifdef _WIN32
    #include <io.h>

    #define write _write
    #define close _close
#endif

extern int win_vsnprintf(char* str, size_t size, const char* format, va_list ap);
extern int win_snprintf(char* str, size_t size, const char* format, ...);

/*
 * Internal variable indicating if a callback has been registered for
 * node creation/destruction. It avoids spending a lot of time in locking
 * function while checking if the callback exists.
 */
extern int __xmlRegisterCallbacks;
/*
 * internal error reporting routines, shared but not partof the API.
 */
void __xmlIOErr(int domain, int code, const char *extra);
void __xmlLoaderErr(void *ctx, const char *msg, const char *filename);
#ifdef LIBXML_HTML_ENABLED
/*
 * internal function of HTML parser needed for xmlParseInNodeContext
 * but not part of the API
 */
void __htmlParseContent(void *ctx);
#endif

/*
 * internal global initialization critical section routines.
 */
void __xmlGlobalInitMutexLock(void);
void __xmlGlobalInitMutexUnlock(void);
void __xmlGlobalInitMutexDestroy(void);

#ifndef stdout
#define stdout              NULL
#endif

#ifndef stderr
#define stderr              NULL
#endif

#endif /* ! __XML_LIBXML_H__ */
