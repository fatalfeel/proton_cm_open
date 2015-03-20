#include "libxml.h"

int win_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
#ifdef _WIN32
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
#else
    return vsnprintf(str,size,format,ap);
#endif
}

int win_snprintf(char* str, size_t size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = win_vsnprintf(str, size, format, ap);
    va_end(ap);

    return count;
}