#ifndef _FMOD_GEOMETRYMGR_H
#define _FMOD_GEOMETRYMGR_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_GEOMETRY

#include "fmod_octree.h"
#include "fmod_geometryi.h"
#include "fmod_thread.h"
#include "fmod_channeli.h"

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{
    class GeometryI;
    class SystemI;
    class GeometryMgr;
   
#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
    typedef enum 
    {
        OCCLUSION_STATE_READY,
        OCCLUSION_STATE_UPDATED,
        OCCLUSION_STATE_CHECKED
    } OCCLUSION_STATE;

    struct OCCLUSION_TASK : LinkedListNode
    {
        OCCLUSION_STATE state;
        unsigned int    currenthandle;
        FMOD_VECTOR     position;
        float           directocclusion;
        float           reverbocclusion;
#ifdef FMOD_SUPPORT_MULTIREVERB
        float           reverbgain;
#endif
    };

    class OcclusionThread : public Thread
    {
      public:
        friend class GeometryMgr;

        OcclusionThread();

        FMOD_RESULT     threadFunc();
        void            init();
        FMOD_RESULT     release();
        void            enqueue(unsigned int index, unsigned int currentHandle, FMOD_VECTOR *worldposition);
        OCCLUSION_TASK* retrieveOcclusion(unsigned int index);
        
      private:          
        bool mInitialized;
        OCCLUSION_TASK* dequeue();

        OCCLUSION_TASK           *mTasks;
        LinkedListNode            mQueueRoot;
        FMOD_OS_CRITICALSECTION  *mQueueCrit;
        GeometryMgr              *mGeometryMgr;
     };
#endif //FMOD_SUPPORT_GEOMETRY_THREADED

    class GeometryMgr
    {
        DECLARE_MEMORYTRACKER

      public:       
        friend class GeometryI;
        friend class ChannelI;
       
        SystemI             *mSystem;
        bool                 mMoved;
#if defined(FMOD_GEOMETRY_DEBUGGING)
        static GeometryMgr  *sThis;
#endif
       
        GeometryMgr();
        ~GeometryMgr();
       
        FMOD_RESULT  aquireMainOctree          ();
        void         releaseMainOctree         ();
#ifdef _DEBUG
        Octree      *mainOctree                ();
#else
        Octree      *mainOctree                ()  { return mMainOctree; }
#endif
        FMOD_RESULT  setWorldSize              (float worldSize);    
        float        getWorldSize              ()  { return mWorldSize; }        
        FMOD_RESULT  lineTestAll               (const FMOD_VECTOR* start, const FMOD_VECTOR* end, float* directOcclusion, float* reverbOcclusion);
         
#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
        friend class OcclusionThread;

        FMOD_RESULT  initCritalSection         ();
        FMOD_RESULT  releaseOcclusionThread    ();
#endif
#if defined(FMOD_GEOMETRY_DEBUGGING)
        FMOD_RESULT  renderTree                (void (*renderBox)(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax));
        FMOD_RESULT  testLineTestForEachPolygon();
#endif
       
         static bool mainOctreeLineTestCallback (OctreeNode* item, void* data);
       
       private:
         FMOD_RESULT  flushAll                  ();

#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
         OcclusionThread            mOcclusionThread;
         FMOD_OS_CRITICALSECTION   *mGeometryCrit;
#endif
         Octree                    *mMainOctree;
         int                        mRefCount;
         GeometryI                 *mFirstUpdateItem;
         float                      mWorldSize;           
    };    
}

#endif

#endif

