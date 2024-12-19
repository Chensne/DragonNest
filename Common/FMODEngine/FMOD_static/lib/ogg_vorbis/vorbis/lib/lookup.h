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

  function: lookup based functions
  last mod: $Id: lookup.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#ifndef _V_LOOKUP_H_

#ifdef FLOAT_LOOKUP
extern float FMOD_vorbis_coslook(float a);
extern float FMOD_vorbis_invsqlook(float a);
extern float FMOD_vorbis_invsq2explook(int a);
extern float FMOD_vorbis_fromdBlook(float a);
#endif
#ifdef INT_LOOKUP
extern ogg_int32_t FMOD_vorbis_invsqlook_i(ogg_int32_t a,ogg_int32_t e);
extern ogg_int32_t FMOD_vorbis_coslook_i(ogg_int32_t a);
extern float FMOD_vorbis_fromdBlook_i(ogg_int32_t a);
#endif

#endif
