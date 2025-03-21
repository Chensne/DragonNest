/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: window functions
 last mod: $Id: window.h 13293 2007-07-24 00:09:47Z xiphmont $

 ********************************************************************/

#ifndef _V_WINDOW_
#define _V_WINDOW_

#ifdef __cplusplus
extern "C" {
#endif

extern void _FMOD_vorbis_window_init();    /* Added by brett */
extern float *_FMOD_vorbis_window_get(int n);
extern void _FMOD_vorbis_apply_window(float *d,int *winno,ogg_int32_t *blocksizes,
                          int lW,int W,int nW);
#ifdef __cplusplus
}
#endif

#endif
