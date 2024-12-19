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

 function: stdio-based convenience library for opening/seeking/decoding
 last mod: $Id: vorbisfile.h 16243 2009-07-10 02:49:31Z xiphmont $

 ********************************************************************/

#ifndef _OV_FILE_H_
#define _OV_FILE_H_

#define OV_EXCLUDE_STATIC_CALLBACKS

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include "codec.h"

/* The function prototypes for the callbacks are basically the same as for
 * the stdio functions fread, fseek, fclose, ftell.
 * The one difference is that the FILE * arguments have been replaced with
 * a void * - this is to be used as a pointer to whatever internal data these
 * functions might need. In the stdio case, it's just a FILE * cast to a void *
 *
 * If you use other functions, check the docs for these functions and return
 * the right values. For seek_func(), you *MUST* return -1 if the stream is
 * unseekable
 */
typedef struct {
  size_t (*read_func)  (void *ptr, size_t size, size_t nmemb, void *datasource);
  int    (*seek_func)  (void *datasource, ogg_int64_t offset, int whence);
  int    (*close_func) (void *datasource);
  ogg_int32_t   (*tell_func)  (void *datasource);
} ov_callbacks;

#ifndef OV_EXCLUDE_STATIC_CALLBACKS

/* a few sets of convenient callbacks, especially for use under
 * Windows where ov_open_callbacks() should always be used instead of
 * ov_open() to avoid problems with incompatable crt.o version linking
 * issues. */

static int _ov_header_fseek_wrap(FILE *f,ogg_int64_t off,int whence){
  if(f==NULL)return(-1);

#ifdef __MINGW32__
  return fseeko64(f,off,whence);
#elif defined (_WIN32)
  return _fseeki64(f,off,whence);
#else
  return fseek(f,off,whence);
#endif
}

/* These structs below (OV_CALLBACKS_DEFAULT etc) are defined here as
 * static data. That means that every file which includes this header
 * will get its own copy of these structs whether it uses them or
 * not unless it #defines OV_EXCLUDE_STATIC_CALLBACKS.
 * These static symbols are essential on platforms such as Windows on
 * which several different versions of stdio support may be linked to
 * by different DLLs, and we need to be certain we know which one
 * we're using (the same one as the main application).
 */

static ov_callbacks OV_CALLBACKS_DEFAULT = {
  (size_t (*)(void *, size_t, size_t, void *))  fread,
  (int (*)(void *, ogg_int64_t, int))           _ov_header_fseek_wrap,
  (int (*)(void *))                             fclose,
  (ogg_int32_t (*)(void *))                            ftell
};

static ov_callbacks OV_CALLBACKS_NOCLOSE = {
  (size_t (*)(void *, size_t, size_t, void *))  fread,
  (int (*)(void *, ogg_int64_t, int))           _ov_header_fseek_wrap,
  (int (*)(void *))                             NULL,
  (ogg_int32_t (*)(void *))                            ftell
};

static ov_callbacks OV_CALLBACKS_STREAMONLY = {
  (size_t (*)(void *, size_t, size_t, void *))  fread,
  (int (*)(void *, ogg_int64_t, int))           NULL,
  (int (*)(void *))                             fclose,
  (ogg_int32_t (*)(void *))                            NULL
};

static ov_callbacks OV_CALLBACKS_STREAMONLY_NOCLOSE = {
  (size_t (*)(void *, size_t, size_t, void *))  fread,
  (int (*)(void *, ogg_int64_t, int))           NULL,
  (int (*)(void *))                             NULL,
  (ogg_int32_t (*)(void *))                            NULL
};

#endif

#define  NOTOPEN   0
#define  PARTOPEN  1
#define  OPENED    2
#define  STREAMSET 3
#define  INITSET   4

typedef struct OggVorbis_File {
  void            *datasource; /* Pointer to a FILE *, etc. */
  int              seekable;
  ogg_int64_t      offset;
  ogg_int64_t      end;
  ogg_sync_state   oy;

  /* If the FILE handle isn't seekable (eg, a pipe), only the current
     stream appears */
  int              links;
  ogg_int64_t     *offsets;
  ogg_int64_t     *dataoffsets;
  ogg_int32_t            *serialnos;
  ogg_int64_t     *pcmlengths; /* overloaded to maintain binary
                                  compatability; x2 size, stores both
                                  beginning and end values */
  vorbis_info     *vi;
  vorbis_comment  *vc;

  /* Decoding working state local storage */
  ogg_int64_t      pcm_offset;
  int              ready_state;
  ogg_int32_t             current_serialno;
  int              current_link;

  float           bittrack;
  float           samptrack;

  ogg_stream_state os; /* take physical pages, weld into a logical
                          stream of packets */
  vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
  vorbis_block     vb; /* local working space for packet->PCM decode */

  ov_callbacks callbacks;

} OggVorbis_File;


extern int ov_clear(void *context, OggVorbis_File *vf);
extern int ov_fopen(void *context, char *path,OggVorbis_File *vf);
extern int ov_open(void *context, FILE *f,OggVorbis_File *vf,char *initial,ogg_int32_t ibytes);
extern int ov_open_callbacks(void *context, void *datasource, OggVorbis_File *vf,
                char *initial, ogg_int32_t ibytes, ov_callbacks callbacks);

extern int ov_test(void *context, FILE *f,OggVorbis_File *vf,char *initial,ogg_int32_t ibytes);
extern int ov_test_callbacks(void *context, void *datasource, OggVorbis_File *vf,
                char *initial, ogg_int32_t ibytes, ov_callbacks callbacks);
extern int ov_test_open(void *context, OggVorbis_File *vf);

extern ogg_int32_t ov_bitrate(OggVorbis_File *vf,int i);
extern ogg_int32_t ov_bitrate_instant(OggVorbis_File *vf);
extern ogg_int32_t ov_streams(OggVorbis_File *vf);
extern ogg_int32_t ov_seekable(OggVorbis_File *vf);
extern ogg_int32_t ov_serialnumber(OggVorbis_File *vf,int i);

extern ogg_int64_t ov_raw_total(OggVorbis_File *vf,int i);
extern ogg_int64_t ov_pcm_total(OggVorbis_File *vf,int i);
extern float ov_time_total(OggVorbis_File *vf,int i);

extern int ov_raw_seek(void *context, OggVorbis_File *vf,ogg_int64_t pos);
extern int ov_pcm_seek(void *context, OggVorbis_File *vf,ogg_int64_t pos);
extern int ov_pcm_seek_page(void *context, OggVorbis_File *vf,ogg_int64_t pos);
extern int ov_time_seek(void *context, OggVorbis_File *vf,float pos);
extern int ov_time_seek_page(void *context, OggVorbis_File *vf,float pos);

extern int ov_raw_seek_lap(void *context, OggVorbis_File *vf,ogg_int64_t pos);
extern int ov_pcm_seek_lap(void *context, OggVorbis_File *vf,ogg_int64_t pos);
extern int ov_pcm_seek_page_lap(void *context, OggVorbis_File *vf,ogg_int64_t pos);
extern int ov_time_seek_lap(void *context, OggVorbis_File *vf,float pos);
extern int ov_time_seek_page_lap(void *context, OggVorbis_File *vf,float pos);

extern ogg_int64_t ov_raw_tell(OggVorbis_File *vf);
extern ogg_int64_t ov_pcm_tell(OggVorbis_File *vf);
extern float ov_time_tell(OggVorbis_File *vf);

extern vorbis_info *ov_info(OggVorbis_File *vf,int link);
extern vorbis_comment *ov_comment(OggVorbis_File *vf,int link);

extern ogg_int32_t ov_read_float(void *context, OggVorbis_File *vf,float ***pcm_channels,int samples,
                          int *bitstream);
extern ogg_int32_t ov_read_filter(void *context, OggVorbis_File *vf,char *buffer,int length,
                          int bigendianp,int word,int sgned,int *bitstream,
                          void (*filter)(float **pcm,ogg_int32_t channels,ogg_int32_t samples,void *filter_param),void *filter_param);
extern ogg_int32_t ov_read(void *context, OggVorbis_File *vf,char *buffer,int length,
                    int bigendianp,int word,int sgned,int *bitstream);
extern int ov_crosslap(void *context, OggVorbis_File *vf1,OggVorbis_File *vf2);

extern int ov_halfrate(void *context, OggVorbis_File *vf,int flag);
extern int ov_halfrate_p(OggVorbis_File *vf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

