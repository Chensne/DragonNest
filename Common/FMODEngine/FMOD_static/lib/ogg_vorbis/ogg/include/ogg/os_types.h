/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: #ifdef jail to whip a few platforms into the UNIX ideal.
 last mod: $Id: os_types.h 14997 2008-06-04 03:27:18Z ivo $

 ********************************************************************/
#ifndef _OS_TYPES_H
#define _OS_TYPES_H

/* make it easy on the folks that want to compile the libs with a
   different malloc than stdlib */
#if 1

extern void * FMOD_OggVorbis_Malloc(void *context, int size);
extern void * FMOD_OggVorbis_Calloc(void *context, int count, int size);
extern void * FMOD_OggVorbis_ReAlloc(void *context, void *ptr, int size);
extern void   FMOD_OggVorbis_Free(void *context, void *ptr);


#define _ogg_malloc(_x)      FMOD_OggVorbis_Malloc(context, _x)
#define _ogg_calloc(_x, _y)  FMOD_OggVorbis_Calloc(context, _x, _y)
#define _ogg_realloc(_x, _y) FMOD_OggVorbis_ReAlloc(context, _x, _y)
#define _ogg_free(_x)        FMOD_OggVorbis_Free(context, _x)

#include "../../../../../src/fmod_types.h" /* For FMOD_memset/memcpy */

#else

#define _ogg_malloc          malloc
#define _ogg_calloc          calloc
#define _ogg_realloc         realloc
#define _ogg_free            free

#endif 

#if defined(WIN32) || defined(_XENON)

#  if defined(__CYGWIN__)
#    include <stdint.h>
     typedef int16_t ogg_int16_t;
     typedef uint16_t ogg_uint16_t;
     typedef int32_t ogg_int32_t;
     typedef uint32_t ogg_uint32_t;
     typedef int64_t ogg_int64_t;
     typedef uint64_t ogg_uint64_t;
#  elif defined(__MINGW32__)
#    include <sys/types.h>
     typedef short ogg_int16_t;
     typedef unsigned short ogg_uint16_t;
     typedef int ogg_int32_t;
     typedef unsigned int ogg_uint32_t;
     typedef long long ogg_int64_t;
     typedef unsigned long long ogg_uint64_t;
#  elif defined(__MWERKS__)
     typedef long long ogg_int64_t;
     typedef int ogg_int32_t;
     typedef unsigned int ogg_uint32_t;
     typedef short ogg_int16_t;
     typedef unsigned short ogg_uint16_t;
#  else
     /* MSVC/Borland */
     typedef __int64 ogg_int64_t;
     typedef __int32 ogg_int32_t;
     typedef unsigned __int32 ogg_uint32_t;
     typedef __int16 ogg_int16_t;
     typedef unsigned __int16 ogg_uint16_t;
#  endif

#elif defined(__MACOS__)

#  include <sys/types.h>
   typedef SInt16 ogg_int16_t;
   typedef UInt16 ogg_uint16_t;
   typedef SInt32 ogg_int32_t;
   typedef UInt32 ogg_uint32_t;
   typedef SInt64 ogg_int64_t;

#elif (defined(__APPLE__) && defined(__MACH__)) /* MacOS X Framework build */

#  include <sys/types.h>
   typedef int16_t ogg_int16_t;
   typedef u_int16_t ogg_uint16_t;
   typedef int32_t ogg_int32_t;
   typedef u_int32_t ogg_uint32_t;
   typedef int64_t ogg_int64_t;

#elif defined(__HAIKU__)

  /* Haiku */
#  include <sys/types.h>
   typedef short ogg_int16_t;
   typedef unsigned short ogg_uint16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;

#elif defined(__BEOS__)

   /* Be */
#  include <inttypes.h>
   typedef int16_t ogg_int16_t;
   typedef u_int16_t ogg_uint16_t;
   typedef int32_t ogg_int32_t;
   typedef u_int32_t ogg_uint32_t;
   typedef int64_t ogg_int64_t;

#elif defined (__EMX__)

   /* OS/2 GCC */
   typedef short ogg_int16_t;
   typedef unsigned short ogg_uint16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;

#elif defined (R5900)

  /* PS2 */
  typedef short ogg_int16_t;
  typedef int ogg_int32_t;
  typedef unsigned int ogg_uint32_t;
  typedef long ogg_int64_t;

   #if defined(__MWERKS__)
       #define alloca __alloca
   #endif 

#elif defined (DJGPP)

   /* DJGPP */
   typedef short ogg_int16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;

#elif defined(__SYMBIAN32__)

   /* Symbian GCC */
   typedef signed short ogg_int16_t;
   typedef unsigned short ogg_uint16_t;
   typedef signed int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long int ogg_int64_t;

#elif defined(SN_TARGET_PS3) || defined(_PS3)

   #include <alloca.h>

   typedef signed short          ogg_int16_t;
   typedef signed int            ogg_int32_t;
   typedef unsigned int          ogg_uint32_t;
   typedef signed long long      ogg_int64_t;

#elif defined (GEKKO)

   #if defined(PLATFORM_WII)
       #include <alloca.h>
   #else
       #if defined(__MWERKS__)
           #define alloca __alloca
       #endif
   #endif

  /* Gamecube */
  typedef short ogg_int16_t;
  typedef int ogg_int32_t;
  typedef unsigned int ogg_uint32_t;
  typedef long long ogg_int64_t;

#elif defined (__psp__)

  typedef short ogg_int16_t;
  typedef int ogg_int32_t;
  typedef unsigned int ogg_uint32_t;
  typedef long long ogg_int64_t;

#elif defined (__sun) || defined (__linux__)

  typedef short ogg_int16_t;
  typedef int ogg_int32_t;
  typedef unsigned int ogg_uint32_t;
  typedef long long ogg_int64_t;

#else
   
#  include <sys/types.h>
#  include <ogg/config_types.h>

#endif

#endif  /* _OS_TYPES_H */
