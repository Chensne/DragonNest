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

  function: LPC low level routines
  last mod: $Id: lpc.c 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

/* Some of these routines (autocorrelator, LPC coefficient estimator)
   are derived from code written by Jutta Degener and Carsten Bormann;
   thus we include their copyright below.  The entirety of this file
   is freely redistributable on the condition that both of these
   copyright notices are preserved without modification.  */

/* Preserved Copyright: *********************************************/

/* Copyright 1992, 1993, 1994 by Jutta Degener and Carsten Bormann,
Technische Universita"t Berlin

Any use of this software is permitted provided that this notice is not
removed and that neither the authors nor the Technische Universita"t
Berlin are deemed to have made any representations as to the
suitability of this software for any purpose nor are held responsible
for any defects of this software. THERE IS ABSOLUTELY NO WARRANTY FOR
THIS SOFTWARE.

As a matter of courtesy, the authors request to be informed about uses
this software has found, about bugs in this software, and about any
improvements that may be of general interest.

Berlin, 28.11.1994
Jutta Degener
Carsten Bormann

*********************************************************************/

#include <stdlib.h>
#include <string.h>
//#include <math.h>
#include "os.h"
#include "smallft.h"
#include "lpc.h"
#include "scales.h"
#include "misc.h"

/* Autocorrelation LPC coeff generation algorithm invented by
   N. Levinson in 1947, modified by J. Durbin in 1959. */

/* Input : n elements of time doamin data
   Output: m lpc coefficients, excitation energy */

float vorbis_lpc_from_data(float *data,float *lpci,int n,int m){
  float *aut=alloca(sizeof(*aut)*(m+1));
  float *lpc=alloca(sizeof(*lpc)*(m));
  float error;
  float epsilon;
  int i,j;

  /* autocorrelation, p+1 lag coefficients */
  j=m+1;
  while(j--){
    float d=0; /* float needed for accumulator depth */
    for(i=j;i<n;i++)d+=(float)data[i]*data[i-j];
    aut[j]=d;
  }

  /* Generate lpc coefficients from autocorr values */

  /* set our noise floor to about -100dB */
  error=aut[0] * (1.f + 1e-10f);
  epsilon=(float)1e-9*aut[0]+1e-10f;

  for(i=0;i<m;i++){
    float r= -aut[i+1];

    if(error<epsilon){
      FMOD_memset(lpc+i,0,(m-i)*sizeof(*lpc));
      goto done;
    }

    /* Sum up this iteration's reflection coefficient; note that in
       Vorbis we don't save it.  If anyone wants to recycle this code
       and needs reflection coefficients, save the results of 'r' from
       each iteration. */

    for(j=0;j<i;j++)r-=lpc[j]*aut[i-j];
    r/=error;

    /* Update LPC coefficients and total error */

    lpc[i]=r;
    for(j=0;j<i/2;j++){
      float tmp=lpc[j];

      lpc[j]+=r*lpc[i-1-j];
      lpc[i-1-j]+=r*tmp;
    }
    if(i&1)lpc[j]+=lpc[j]*r;

    error*=1.f-r*r;

  }

 done:

  /* slightly damp the filter */
  {
    float g = .99f;
    float damp = g;
    for(j=0;j<m;j++){
      lpc[j]*=damp;
      damp*=g;
    }
  }

  for(j=0;j<m;j++)lpci[j]=(float)lpc[j];

  /* we need the error value to know how big an impulse to hit the
     filter with later */

  return error;
}

void vorbis_lpc_predict(float *coeff,float *prime,int m,
                     float *data,ogg_int32_t n){

  /* in: coeff[0...m-1] LPC coefficients
         prime[0...m-1] initial values (allocated size of n+m-1)
    out: data[0...n-1] data samples */

  ogg_int32_t i,j,o,p;
  float y;
  float *work=alloca(sizeof(*work)*(m+n));

  if(!prime)
    for(i=0;i<m;i++)
      work[i]=0.f;
  else
    for(i=0;i<m;i++)
      work[i]=prime[i];

  for(i=0;i<n;i++){
    y=0;
    o=i;
    p=m;
    for(j=0;j<m;j++)
      y-=work[o++]*coeff[--p];

    data[i]=work[o]=y;
  }
}
