/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2009             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: miscellaneous prototypes
 last mod: $Id: misc.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#ifndef _V_RANDOM_H_
#define _V_RANDOM_H_
#include "vorbis/codec.h"

extern void *_FMOD_vorbis_block_alloc(void *context, vorbis_block *vb,ogg_int32_t bytes);
extern int _FMOD_vorbis_block_ripcord(void *context, vorbis_block *vb);

#ifdef ANALYSIS
extern int analysis_noisy;
extern void _analysis_output(char *base,int i,float *v,int n,int bark,int dB,
                             ogg_int64_t off);
extern void _analysis_output_always(char *base,int i,float *v,int n,int bark,int dB,
                             ogg_int64_t off);
#endif

#ifdef DEBUG_MALLOC

#define _VDBG_GRAPHFILE "malloc.m"
#undef _VDBG_GRAPHFILE
extern void *_VDBG_malloc(void *ptr,ogg_int32_t bytes,char *file,ogg_int32_t line);
extern void _VDBG_free(void *ptr,char *file,ogg_int32_t line);

#ifndef MISC_C
#undef _ogg_malloc
#undef _ogg_calloc
#undef _ogg_realloc
#undef _ogg_free

#define _ogg_malloc(x) _VDBG_malloc(NULL,(x),__FILE__,__LINE__)
#define _ogg_calloc(x,y) _VDBG_malloc(NULL,(x)*(y),__FILE__,__LINE__)
#define _ogg_realloc(x,y) _VDBG_malloc((x),(y),__FILE__,__LINE__)
#define _ogg_free(x) _VDBG_free((x),__FILE__,__LINE__)
#endif
#endif

#endif




