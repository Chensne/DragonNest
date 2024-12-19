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

 function: basic shared codebook operations
 last mod: $Id: sharedbook.c 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ogg/ogg.h>
#include "os.h"
#include "misc.h"
#include "vorbis/codec.h"
#include "codebook.h"
#include "scales.h"

/**** pack/unpack helpers ******************************************/
int _FMOD_ilog(unsigned int v){
  int ret=0;
  while(v){
    ret++;
    v>>=1;
  }
  return(ret);
}

/* 32 bit float (not IEEE; nonnormalized mantissa +
   biased exponent) : neeeeeee eeemmmmm mmmmmmmm mmmmmmmm
   Why not IEEE?  It's just not that important here. */

#define VQ_FEXP 10
#define VQ_FMAN 21
#define VQ_FEXP_BIAS 768 /* bias toward values smaller than 1. */

/* doesn't currently guard under/overflow */
ogg_int32_t _FMOD_float32_pack(float val){
  int sign=0;
  ogg_int32_t exp;
  ogg_int32_t mant;
  if(val<0){
    sign=0x80000000;
    val= -val;
  }
  exp= (ogg_int32_t)FMOD_ogg_floor(FMOD_ogg_log(val)/log(2.f));
  mant=(ogg_int32_t)FMOD_ogg_rint(FMOD_ogg_ldexp(val,(VQ_FMAN-1)-exp));
  exp=(exp+VQ_FEXP_BIAS)<<VQ_FMAN;

  return(sign|exp|mant);
}

float _FMOD_float32_unpack(ogg_int32_t val){
  double mant=val&0x1fffff;
  int    sign=val&0x80000000;
  ogg_int32_t   exp =(val&0x7fe00000L)>>VQ_FMAN;
  if(sign)mant= -mant;
  return(FMOD_ogg_ldexp(mant,exp-(VQ_FMAN-1)-VQ_FEXP_BIAS));
}

/* given a list of word lengths, generate a list of codewords.  Works
   for length ordered or unordered, always assigns the lowest valued
   codewords first.  Extended to handle unused entries (length 0) */
ogg_uint32_t *_FMOD_make_words(void *context, ogg_int32_t *l,ogg_int32_t n,ogg_int32_t sparsecount){
  ogg_int32_t i,j,count=0;
  ogg_uint32_t marker[33];
  ogg_uint32_t *r=_ogg_malloc((sparsecount?sparsecount:n)*sizeof(*r));
  if (!r)
  {
    return(NULL);
  }
  FMOD_memset(marker,0,sizeof(marker));

  for(i=0;i<n;i++){
    ogg_int32_t length=l[i];
    if(length>0){
      ogg_uint32_t entry=marker[length];

      /* when we claim a node for an entry, we also claim the nodes
         below it (pruning off the imagined tree that may have dangled
         from it) as well as blocking the use of any nodes directly
         above for leaves */

      /* update ourself */
      if(length<32 && (entry>>length)){
        /* error condition; the lengths must specify an overpopulated tree */
        _ogg_free(r);
        return(NULL);
      }
      r[count++]=entry;

      /* Look to see if the next shorter marker points to the node
         above. if so, update it and repeat.  */
      {
        for(j=length;j>0;j--){

          if(marker[j]&1){
            /* have to jump branches */
            if(j==1)
              marker[1]++;
            else
              marker[j]=marker[j-1]<<1;
            break; /* invariant says next upper marker would already
                      have been moved if it was on the same path */
          }
          marker[j]++;
        }
      }

      /* prune the tree; the implicit invariant says all the longer
         markers were dangling from our just-taken node.  Dangle them
         from our *new* node. */
      for(j=length+1;j<33;j++)
        if((marker[j]>>1) == entry){
          entry=marker[j];
          marker[j]=marker[j-1]<<1;
        }else
          break;
    }else
      if(sparsecount==0)count++;
  }

  /* sanity check the huffman tree; an underpopulated tree must be
     rejected. The only exception is the one-node pseudo-nil tree,
     which appears to be underpopulated because the tree doesn't
     really exist; there's only one possible 'codeword' or zero bits,
     but the above tree-gen code doesn't mark that. */
  if(sparsecount != 1){
    for(i=1;i<33;i++)
      if(marker[i] & (0xffffffffUL>>(32-i))){
	_ogg_free(r);
	return(NULL);
      }
  }

  /* bitreverse the words because our bitwise packer/unpacker is LSb
     endian */
  for(i=0,count=0;i<n;i++){
    ogg_uint32_t temp=0;
    for(j=0;j<l[i];j++){
      temp<<=1;
      temp|=(r[count]>>j)&1;
    }

    if(sparsecount){
      if(l[i])
        r[count++]=temp;
    }else
      r[count++]=temp;
  }

  return(r);
}

/* there might be a straightforward one-line way to do the below
   that's portable and totally safe against roundoff, but I haven't
   thought of it.  Therefore, we opt on the side of caution */
ogg_int32_t _FMOD_book_maptype1_quantvals(const static_codebook *b){
  ogg_int32_t vals=(ogg_int32_t)FMOD_ogg_floor(FMOD_ogg_pow((float)b->entries,1.f/b->dim));

  /* the above *should* be reliable, but we'll not assume that FP is
     ever reliable when bitstream sync is at stake; verify via integer
     means that vals really is the greatest value of dim for which
     vals^b->bim <= b->entries */
  /* treat the above as an initial guess */
  while(1){
    ogg_int32_t acc=1;
    ogg_int32_t acc1=1;
    int i;
    for(i=0;i<b->dim;i++){
      acc*=vals;
      acc1*=vals+1;
    }
    if(acc<=b->entries && acc1>b->entries){
      return(vals);
    }else{
      if(acc>b->entries){
        vals--;
      }else{
        vals++;
      }
    }
  }
}

/* unpack the quantized list of values for encode/decode ***********/
/* we need to deal with two map types: in map type 1, the values are
   generated algorithmically (each column of the vector counts through
   the values in the quant vector). in map type 2, all the values came
   in in an explicit list.  Both value lists must be unpacked */
float *_FMOD_book_unquantize(void *context, const static_codebook *b,int n,int *sparsemap){
  ogg_int32_t j,k,count=0;
  if(b->maptype==1 || b->maptype==2){
    int quantvals;
    float mindel=_FMOD_float32_unpack(b->q_min);
    float delta=_FMOD_float32_unpack(b->q_delta);
    float *r=_ogg_calloc(n*b->dim,sizeof(*r));
    if (!r)
    {
      return(NULL);
    }

    /* maptype 1 and 2 both use a quantized value vector, but
       different sizes */
    switch(b->maptype){
    case 1:
      /* most of the time, entries%dimensions == 0, but we need to be
         well defined.  We define that the possible vales at each
         scalar is values == entries/dim.  If entries%dim != 0, we'll
         have 'too few' values (values*dim<entries), which means that
         we'll have 'left over' entries; left over entries use zeroed
         values (and are wasted).  So don't generate codebooks like
         that */
      quantvals=_FMOD_book_maptype1_quantvals(b);
      for(j=0;j<b->entries;j++){
        if((sparsemap && b->lengthlist[j]) || !sparsemap){
          float last=0.f;
          int indexdiv=1;
          for(k=0;k<b->dim;k++){
            int index= (j/indexdiv)%quantvals;
            float val=(float)b->quantlist[index];
            val=FMOD_ogg_fabs(val)*delta+mindel+last;
            if(b->q_sequencep)last=val;
            if(sparsemap)
              r[sparsemap[count]*b->dim+k]=val;
            else
              r[count*b->dim+k]=val;
            indexdiv*=quantvals;
          }
          count++;
        }

      }
      break;
    case 2:
      for(j=0;j<b->entries;j++){
        if((sparsemap && b->lengthlist[j]) || !sparsemap){
          float last=0.f;

          for(k=0;k<b->dim;k++){
            float val=(float)b->quantlist[j*b->dim+k];
            val=FMOD_ogg_fabs(val)*delta+mindel+last;
            if(b->q_sequencep)last=val;
            if(sparsemap)
              r[sparsemap[count]*b->dim+k]=val;
            else
              r[count*b->dim+k]=val;
          }
          count++;
        }
      }
      break;
    }

    return(r);
  }
  return(NULL);
}

void FMOD_vorbis_staticbook_clear(void *context, static_codebook *b){
  if(b->allocedp){
    if(b->quantlist)_ogg_free(b->quantlist);
    if(b->lengthlist)_ogg_free(b->lengthlist);
    //if(b->nearest_tree){
    //  _ogg_free(b->nearest_tree->ptr0);
    //  _ogg_free(b->nearest_tree->ptr1);
    //  _ogg_free(b->nearest_tree->p);
    //  _ogg_free(b->nearest_tree->q);
    //  FMOD_memset(b->nearest_tree,0,sizeof(*b->nearest_tree));
    //  _ogg_free(b->nearest_tree);
    //}
    //if(b->thresh_tree){
    //  _ogg_free(b->thresh_tree->quantthresh);
    //  _ogg_free(b->thresh_tree->quantmap);
    //  FMOD_memset(b->thresh_tree,0,sizeof(*b->thresh_tree));
    //  _ogg_free(b->thresh_tree);
    //}

    FMOD_memset(b,0,sizeof(*b));
  }
}

void FMOD_vorbis_staticbook_destroy(void *context, static_codebook *b){
  if(b->allocedp){
    FMOD_vorbis_staticbook_clear(context, b);
    _ogg_free(b);
  }
}

void FMOD_vorbis_book_clear(void *context, codebook *b){
  /* static book is not cleared; we're likely called on the lookup and
     the static codebook belongs to the info struct */
  if(b->valuelist)_ogg_free(b->valuelist);
  if(b->codelist)_ogg_free(b->codelist);

  if(b->dec_index)_ogg_free(b->dec_index);
  if(b->dec_codelengths)_ogg_free(b->dec_codelengths);
  if(b->dec_firsttable)_ogg_free(b->dec_firsttable);

  FMOD_memset(b,0,sizeof(*b));
}

#if 0
int FMOD_vorbis_book_init_encode(void *context, codebook *c,const static_codebook *s){

  FMOD_memset(c,0,sizeof(*c));
  c->c=s;
  c->entries=s->entries;
  c->used_entries=s->entries;
  c->dim=s->dim;
  c->codelist=_FMOD_make_words(context, s->lengthlist,s->entries,0);
  c->valuelist=_FMOD_book_unquantize(context, s,s->entries,NULL);

  return(0);
}
#endif

static ogg_uint32_t bitreverse(ogg_uint32_t x){
  x=    ((x>>16)&0x0000ffff) | ((x<<16)&0xffff0000);
  x=    ((x>> 8)&0x00ff00ff) | ((x<< 8)&0xff00ff00);
  x=    ((x>> 4)&0x0f0f0f0f) | ((x<< 4)&0xf0f0f0f0);
  x=    ((x>> 2)&0x33333333) | ((x<< 2)&0xcccccccc);
  return((x>> 1)&0x55555555) | ((x<< 1)&0xaaaaaaaa);
}

static int sort32a(const void *a,const void *b){
  return ( **(ogg_uint32_t **)a>**(ogg_uint32_t **)b)-
    ( **(ogg_uint32_t **)a<**(ogg_uint32_t **)b);
}

/* decode codebook arrangement is more heavily optimized than encode */
int FMOD_vorbis_book_init_decode(void *context, codebook *c,const static_codebook *s){
  int i,j,n=0,tabn;
  int *sortindex;
  FMOD_memset(c,0,sizeof(*c));

  /* count actually used entries */
  for(i=0;i<s->entries;i++)
    if(s->lengthlist[i]>0)
      n++;

  c->entries=s->entries;
  c->used_entries=n;
  c->dim=s->dim;

  if(n>0){

    /* two different remappings go on here.

    First, we collapse the likely sparse codebook down only to
    actually represented values/words.  This collapsing needs to be
    indexed as map-valueless books are used to encode original entry
    positions as integers.

    Second, we reorder all vectors, including the entry index above,
    by sorted bitreversed codeword to allow treeless decode. */

    /* perform sort */
    ogg_uint32_t *codes=_FMOD_make_words(context, s->lengthlist,s->entries,c->used_entries);
    ogg_uint32_t **codep=alloca(sizeof(*codep)*n);

    if(codes==NULL)goto err_out;

    for(i=0;i<n;i++){
      codes[i]=bitreverse(codes[i]);
      codep[i]=codes+i;
    }

    qsort(codep,n,sizeof(*codep),sort32a);

    sortindex=alloca(n*sizeof(*sortindex));
    c->codelist=_ogg_malloc(n*sizeof(*c->codelist));
    if (!c->codelist)
    {
      goto err_out;
    }
    /* the index is a reverse index */
    for(i=0;i<n;i++){
      int position=codep[i]-codes;
      sortindex[position]=i;
    }

    for(i=0;i<n;i++)
      c->codelist[sortindex[i]]=codes[i];
    _ogg_free(codes);


    c->valuelist=_FMOD_book_unquantize(context, s,n,sortindex);
    c->dec_index=_ogg_malloc(n*sizeof(*c->dec_index));
    if (!c->dec_index)
    {
      goto err_out;
    }

    for(n=0,i=0;i<s->entries;i++)
      if(s->lengthlist[i]>0)
        c->dec_index[sortindex[n++]]=i;

    c->dec_codelengths=_ogg_malloc(n*sizeof(*c->dec_codelengths));
    if (!c->dec_codelengths)
    {
      goto err_out;
    }
    for(n=0,i=0;i<s->entries;i++)
      if(s->lengthlist[i]>0)
        c->dec_codelengths[sortindex[n++]]=s->lengthlist[i];

    c->dec_firsttablen=_FMOD_ilog(c->used_entries)-4; /* this is magic */
    if(c->dec_firsttablen<5)c->dec_firsttablen=5;
    if(c->dec_firsttablen>8)c->dec_firsttablen=8;

    tabn=1<<c->dec_firsttablen;
    c->dec_firsttable=_ogg_calloc(tabn,sizeof(*c->dec_firsttable));
    if (!c->dec_firsttable)
    {
        goto err_out;
    }
    c->dec_maxlength=0;

    for(i=0;i<n;i++){
      if(c->dec_maxlength<c->dec_codelengths[i])
        c->dec_maxlength=c->dec_codelengths[i];
      if(c->dec_codelengths[i]<=c->dec_firsttablen){
        ogg_uint32_t orig=bitreverse(c->codelist[i]);
        for(j=0;j<(1<<(c->dec_firsttablen-c->dec_codelengths[i]));j++)
          c->dec_firsttable[orig|(j<<c->dec_codelengths[i])]=i+1;
      }
    }

    /* now fill in 'unused' entries in the firsttable with hi/lo search
       hints for the non-direct-hits */
    {
      ogg_uint32_t mask=0xfffffffeUL<<(31-c->dec_firsttablen);
      ogg_int32_t lo=0,hi=0;

      for(i=0;i<tabn;i++){
        ogg_uint32_t word=i<<(32-c->dec_firsttablen);
        if(c->dec_firsttable[bitreverse(word)]==0){
          while((lo+1)<n && c->codelist[lo+1]<=word)lo++;
          while(    hi<n && word>=(c->codelist[hi]&mask))hi++;

          /* we only actually have 15 bits per hint to play with here.
             In order to overflow gracefully (nothing breaks, efficiency
             just drops), encode as the difference from the extremes. */
          {
            ogg_uint32_t loval=lo;
            ogg_uint32_t hival=n-hi;

            if(loval>0x7fff)loval=0x7fff;
            if(hival>0x7fff)hival=0x7fff;
            c->dec_firsttable[bitreverse(word)]=
              0x80000000UL | (loval<<15) | hival;
          }
        }
      }
    }
  }

  return(0);
 err_out:
  FMOD_vorbis_book_clear(context, c);
  return(-1);
}

static float _dist(int el,float *ref, float *b,int step){
  int i;
  float acc=0.f;
  for(i=0;i<el;i++){
    float val=(ref[i]-b[i*step]);
    acc+=val*val;
  }
  return(acc);
}

#if 0
int _FMOD_best(codebook *book, float *a, int step){
  encode_aux_threshmatch *tt=book->c->thresh_tree;

#if 0
  encode_aux_nearestmatch *nt=book->c->nearest_tree;
  encode_aux_pigeonhole *pt=book->c->pigeon_tree;
#endif
  int dim=book->dim;
  int k,o;
  /*int savebest=-1;
    float saverr;*/

  /* do we have a threshhold encode hint? */
  if(tt){
    int index=0,i;
    /* find the quant val of each scalar */
    for(k=0,o=step*(dim-1);k<dim;k++,o-=step){

      i=tt->threshvals>>1;
      if(a[o]<tt->quantthresh[i]){

        for(;i>0;i--)
          if(a[o]>=tt->quantthresh[i-1])
            break;

      }else{

        for(i++;i<tt->threshvals-1;i++)
          if(a[o]<tt->quantthresh[i])break;

      }

      index=(index*tt->quantvals)+tt->quantmap[i];
    }
    /* regular lattices are easy :-) */
    if(book->c->lengthlist[index]>0) /* is this unused?  If so, we'll
                                        use a decision tree after all
                                        and fall through*/
      return(index);
  }

#if 0
  /* do we have a pigeonhole encode hint? */
  if(pt){
    const static_codebook *c=book->c;
    int i,besti=-1;
    float best=0.f;
    int entry=0;

    /* dealing with sequentialness is a pain in the ass */
    if(c->q_sequencep){
      int pv;
      ogg_int32_t mul=1;
      float qlast=0;
      for(k=0,o=0;k<dim;k++,o+=step){
        pv=(int)((a[o]-qlast-pt->min)/pt->del);
        if(pv<0 || pv>=pt->mapentries)break;
        entry+=pt->pigeonmap[pv]*mul;
        mul*=pt->quantvals;
        qlast+=pv*pt->del+pt->min;
      }
    }else{
      for(k=0,o=step*(dim-1);k<dim;k++,o-=step){
        int pv=(int)((a[o]-pt->min)/pt->del);
        if(pv<0 || pv>=pt->mapentries)break;
        entry=entry*pt->quantvals+pt->pigeonmap[pv];
      }
    }

    /* must be within the pigeonholable range; if we quant outside (or
       in an entry that we define no list for), brute force it */
    if(k==dim && pt->fitlength[entry]){
      /* search the abbreviated list */
      ogg_int32_t *list=pt->fitlist+pt->fitmap[entry];
      for(i=0;i<pt->fitlength[entry];i++){
        float this=_dist(dim,book->valuelist+list[i]*dim,a,step);
        if(besti==-1 || this<best){
          best=this;
          besti=list[i];
        }
      }

      return(besti);
    }
  }

  if(nt){
    /* optimized using the decision tree */
    while(1){
      float c=0.f;
      float *p=book->valuelist+nt->p[ptr];
      float *q=book->valuelist+nt->q[ptr];

      for(k=0,o=0;k<dim;k++,o+=step)
        c+=(p[k]-q[k])*(a[o]-(p[k]+q[k])*.5);

      if(c>0.f) /* in A */
        ptr= -nt->ptr0[ptr];
      else     /* in B */
        ptr= -nt->ptr1[ptr];
      if(ptr<=0)break;
    }
    return(-ptr);
  }
#endif

  /* brute force it! */
  {
    const static_codebook *c=book->c;
    int i,besti=-1;
    float best=0.f;
    float *e=book->valuelist;
    for(i=0;i<book->entries;i++){
      if(c->lengthlist[i]>0){
        float this=_dist(dim,e,a,step);
        if(besti==-1 || this<best){
          best=this;
          besti=i;
        }
      }
      e+=dim;
    }

    /*if(savebest!=-1 && savebest!=besti){
      fprintf(stderr,"brute force/pigeonhole disagreement:\n"
              "original:");
      for(i=0;i<dim*step;i+=step)fprintf(stderr,"%g,",a[i]);
      fprintf(stderr,"\n"
              "pigeonhole (entry %d, err %g):",savebest,saverr);
      for(i=0;i<dim;i++)fprintf(stderr,"%g,",
                                (book->valuelist+savebest*dim)[i]);
      fprintf(stderr,"\n"
              "bruteforce (entry %d, err %g):",besti,best);
      for(i=0;i<dim;i++)fprintf(stderr,"%g,",
                                (book->valuelist+besti*dim)[i]);
      fprintf(stderr,"\n");
      }*/
    return(besti);
  }
}

ogg_int32_t vorbis_book_codeword(codebook *book,int entry){
  if(book->c) /* only use with encode; decode optimizations are
                 allowed to break this */
    return book->codelist[entry];
  return -1;
}

ogg_int32_t vorbis_book_codelen(codebook *book,int entry){
  if(book->c) /* only use with encode; decode optimizations are
                 allowed to break this */
    return book->c->lengthlist[entry];
  return -1;
}

#endif

#ifdef _V_SELFTEST

/* Unit tests of the dequantizer; this stuff will be OK
   cross-platform, I simply want to be sure that special mapping cases
   actually work properly; a bug could go unnoticed for a while */

#include <stdio.h>

/* cases:

   no mapping
   full, explicit mapping
   algorithmic mapping

   nonsequential
   sequential
*/

static ogg_int32_t full_quantlist1[]={0,1,2,3,    4,5,6,7, 8,3,6,1};
static ogg_int32_t partial_quantlist1[]={0,7,2};

/* no mapping */
static_codebook test1={
  4,16,
  NULL,
  0,
  0,0,0,0,
  NULL,
  NULL,NULL,NULL,
  0
};
static float *test1_result=NULL;

/* linear, full mapping, nonsequential */
static_codebook test2={
  4,3,
  NULL,
  2,
  -533200896,1611661312,4,0,
  full_quantlist1,
  NULL,NULL,NULL,
  0
};
static float test2_result[]={-3,-2,-1,0, 1,2,3,4, 5,0,3,-2};

/* linear, full mapping, sequential */
static_codebook test3={
  4,3,
  NULL,
  2,
  -533200896,1611661312,4,1,
  full_quantlist1,
  NULL,NULL,NULL,
  0
};
static float test3_result[]={-3,-5,-6,-6, 1,3,6,10, 5,5,8,6};

/* linear, algorithmic mapping, nonsequential */
static_codebook test4={
  3,27,
  NULL,
  1,
  -533200896,1611661312,4,0,
  partial_quantlist1,
  NULL,NULL,NULL,
  0
};
static float test4_result[]={-3,-3,-3, 4,-3,-3, -1,-3,-3,
                              -3, 4,-3, 4, 4,-3, -1, 4,-3,
                              -3,-1,-3, 4,-1,-3, -1,-1,-3,
                              -3,-3, 4, 4,-3, 4, -1,-3, 4,
                              -3, 4, 4, 4, 4, 4, -1, 4, 4,
                              -3,-1, 4, 4,-1, 4, -1,-1, 4,
                              -3,-3,-1, 4,-3,-1, -1,-3,-1,
                              -3, 4,-1, 4, 4,-1, -1, 4,-1,
                              -3,-1,-1, 4,-1,-1, -1,-1,-1};

/* linear, algorithmic mapping, sequential */
static_codebook test5={
  3,27,
  NULL,
  1,
  -533200896,1611661312,4,1,
  partial_quantlist1,
  NULL,NULL,NULL,
  0
};
static float test5_result[]={-3,-6,-9, 4, 1,-2, -1,-4,-7,
                              -3, 1,-2, 4, 8, 5, -1, 3, 0,
                              -3,-4,-7, 4, 3, 0, -1,-2,-5,
                              -3,-6,-2, 4, 1, 5, -1,-4, 0,
                              -3, 1, 5, 4, 8,12, -1, 3, 7,
                              -3,-4, 0, 4, 3, 7, -1,-2, 2,
                              -3,-6,-7, 4, 1, 0, -1,-4,-5,
                              -3, 1, 0, 4, 8, 7, -1, 3, 2,
                              -3,-4,-5, 4, 3, 2, -1,-2,-3};

void run_test(static_codebook *b,float *comp){
  float *out=_FMOD_book_unquantize(context, b,b->entries,NULL);
  int i;

  if(comp){
    if(!out){
      fprintf(stderr,"_book_unquantize incorrectly returned NULL\n");
      exit(1);
    }

    for(i=0;i<b->entries*b->dim;i++)
      if(FMOD_ogg_fabs(out[i]-comp[i])>.0001f){
        fprintf(stderr,"disagreement in unquantized and reference data:\n"
                "position %d, %g != %g\n",i,out[i],comp[i]);
        exit(1);
      }

  }else{
    if(out){
      fprintf(stderr,"_book_unquantize returned a value array: \n"
              " correct result should have been NULL\n");
      exit(1);
    }
  }
}

int main(){
  /* run the nine dequant tests, and compare to the hand-rolled results */
  fprintf(stderr,"Dequant test 1... ");
  run_test(&test1,test1_result);
  fprintf(stderr,"OK\nDequant test 2... ");
  run_test(&test2,test2_result);
  fprintf(stderr,"OK\nDequant test 3... ");
  run_test(&test3,test3_result);
  fprintf(stderr,"OK\nDequant test 4... ");
  run_test(&test4,test4_result);
  fprintf(stderr,"OK\nDequant test 5... ");
  run_test(&test5,test5_result);
  fprintf(stderr,"OK\n\n");

  return(0);
}

#endif
