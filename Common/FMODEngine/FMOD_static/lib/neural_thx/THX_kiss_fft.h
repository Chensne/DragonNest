#ifndef THX_KISS_FFT_H
#define THX_KISS_FFT_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <memory.h>
//#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ATTENTION!
 If you would like a :
 -- a utility that will handle the caching of fft objects
 -- real-only (no imaginary time component ) FFT
 -- a multi-dimensional FFT
 -- a command-line utility to perform ffts
 -- a command-line utility to perform fast-convolution filtering

 Then see kfc.h kiss_fftr.h kiss_fftnd.h fftutil.c kiss_fastfir.c
  in the tools/ directory.
*/

//#ifdef USE_SIMD
//# include <xmmintrin.h>
//# define THX_kiss_fft_scalar __m128
//#define KISS_FFT_MALLOC(nbytes) memalign(16,nbytes)
//#else	
//#define THX_KISS_FFT_MALLOC malloc
//#endif	


#ifdef FIXED_POINT
#include <sys/types.h>	
# if (FIXED_POINT == 32)
#  define kiss_fft_scalar int32_t
# else	
#  define kiss_fft_scalar int16_t
# endif
#else
# ifndef THX_kiss_fft_scalar
/*  default is float */
#   define THX_kiss_fft_scalar float
# endif
#endif

typedef struct {
    THX_kiss_fft_scalar r;
    THX_kiss_fft_scalar i;
}THX_kiss_fft_cpx;

typedef struct THX_kiss_fft_state* THX_kiss_fft_cfg;

/* 
 *  kiss_fft_alloc
 *  
 *  Initialize a FFT (or IFFT) algorithm's cfg/state buffer.
 *
 *  typical usage:      kiss_fft_cfg mycfg=kiss_fft_alloc(1024,0,NULL,NULL);
 *
 *  The return value from fft_alloc is a cfg buffer used internally
 *  by the fft routine or NULL.
 *
 *  If lenmem is NULL, then kiss_fft_alloc will allocate a cfg buffer using malloc.
 *  The returned value should be free()d when done to avoid memory leaks.
 *  
 *  The state can be placed in a user supplied buffer 'mem':
 *  If lenmem is not NULL and mem is not NULL and *lenmem is large enough,
 *      then the function places the cfg in mem and the size used in *lenmem
 *      and returns mem.
 *  
 *  If lenmem is not NULL and ( mem is NULL or *lenmem is not large enough),
 *      then the function returns NULL and places the minimum cfg 
 *      buffer size in *lenmem.
 * */

THX_kiss_fft_cfg THX_kiss_fft_alloc(int nfft,int inverse_fft,void * mem,size_t * lenmem); 

/*
 * kiss_fft(cfg,in_out_buf)
 *
 * Perform an FFT on a complex input buffer.
 * for a forward FFT,
 * fin should be  f[0] , f[1] , ... ,f[nfft-1]
 * fout will be   F[0] , F[1] , ... ,F[nfft-1]
 * Note that each element is complex and can be accessed like
    f[k].r and f[k].i
 * */
void THX_kiss_fft(THX_kiss_fft_cfg cfg,const THX_kiss_fft_cpx *fin,THX_kiss_fft_cpx *fout);

/*
 A more generic version of the above function. It reads its input from every Nth sample.
 * */
void THX_kiss_fft_stride(THX_kiss_fft_cfg cfg,const THX_kiss_fft_cpx *fin,THX_kiss_fft_cpx *fout,int fin_stride);

/* If kiss_fft_alloc allocated a buffer, it is one contiguous 
   buffer and can be simply free()d when no longer needed*/
//#define THX_kiss_fft_free free

/*
 Cleans up some memory that gets managed internally. Not necessary to call, but it might clean up 
 your compiler output to call this before you exit.
*/
void THX_kiss_fft_cleanup(void);
	

/*
 * Returns the smallest integer k, such that k>=n and k has only "fast" factors (2,3,5)
 */
int THX_kiss_fft_next_fast_size(int n);

#ifdef __cplusplus
} 
#endif

#endif
