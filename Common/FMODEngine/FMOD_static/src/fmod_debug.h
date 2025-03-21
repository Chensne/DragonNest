#ifndef _FMOD_DEBUG_H
#define _FMOD_DEBUG_H

#include "fmod_settings.h"

#ifdef FMOD_DEBUG

#include "fmod.h"

namespace FMOD
{
    typedef enum
    {
        DEBUG_STDOUT,
        DEBUG_FILE
    } FMOD_DEBUGMODE;

    #define FMOD_DEBUG_USER_BRETT   0x10000000
    #define FMOD_DEBUG_USER_ANDREW  0x20000000
    #define FMOD_DEBUG_USER_CHENPO  0x40000000
    #define FMOD_DEBUG_USER_PETER   0x80000000
    #define FMOD_DEBUG_USER_ALL     0xF0000000
  
    void Debug(FMOD_DEBUGLEVEL level, const char *file, const int line, const char *fnname, const char *format, ...);

    extern int            *gDebugIndent;
    
    #define FLOG(_x)         Debug _x
    #define FLOGC(_x)        FMOD::Debug _x
    #define FLOG_INDENT(_x) (*gDebugIndent) += (_x)
}

#else

    #define FLOG(_x) ;
    #define FLOGC(_x) ;
    #define FLOG_INDENT(_x) ;

#endif

#endif

