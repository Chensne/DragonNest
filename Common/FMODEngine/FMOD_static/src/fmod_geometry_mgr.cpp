#include "fmod_settings.h"
#include "fmod_geometry_mgr.h"

#ifdef FMOD_SUPPORT_GEOMETRY

#include "fmod_memory.h"
#include "fmod_systemi.h"
#include "fmod_localcriticalsection.h"
#include "fmod_3d.h"

#if defined(FMOD_GEOMETRY_DEBUGGING)

// exported functions for debugging
void F_DECLSPEC F_DLLEXPORT geometryRenderTest(void (*renderBox)(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax))
{
    if (FMOD::GeometryMgr::sThis)
        FMOD::GeometryMgr::sThis->renderTree(renderBox);
}

void F_DECLSPEC F_DLLEXPORT geometryLineTestTest(FMOD_VECTOR* start, FMOD_VECTOR* end, float* intensity)
{
    if (FMOD::GeometryMgr::sThis)
    {
        float directOcclusion = 0;
        float reverbOcclusion = 0;
        FMOD::GeometryMgr::sThis->lineTestAll(start, end, &directOcclusion, &reverbOcclusion);
        *intensity = 1.0f - directOcclusion;
    }
}

void F_DECLSPEC F_DLLEXPORT testLineTestForEachPolygon()
{
    // does a line test for ever polygon and make sure that it
    // intersects with it.
    // hasn't been made to work with transformations.
    if (FMOD::GeometryMgr::sThis)
    {
        FMOD::GeometryMgr::sThis->testLineTestForEachPolygon();
    }
}
#endif

namespace FMOD
{
    
/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/

#if defined(_DEBUG) && defined(FMOD_GEOMETRY_DEBUGGING)
#define ASSERT(x) do { if (!(x)) __asm { __emit 0xF1 } } while (false)
#else
#define ASSERT(x)    
#endif
    
#if defined(FMOD_GEOMETRY_DEBUGGING)
GeometryMgr* GeometryMgr::sThis = 0;
#endif

#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
OcclusionThread::OcclusionThread() : Thread()
{
    mQueueCrit = 0;
    mTasks = 0;
    mGeometryMgr = 0;
    mInitialized = false;
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
void OcclusionThread::init()
{
    ASSERT(mGeometryMgr);
    ASSERT(mGeometryMgr->mSystem);
    ASSERT(!mTasks);
    mInitialized = true;

    unsigned int numchans = mGeometryMgr->mSystem->mNumChannels;
    FMOD_OS_CriticalSection_Create(&mQueueCrit);
    LocalCriticalSection crit(mQueueCrit, true);
    
    mTasks = (OCCLUSION_TASK *)FMOD_Memory_Calloc(numchans * sizeof(OCCLUSION_TASK));
    
    for (unsigned int count = 0; count < numchans; count++)
    {
        mTasks[count].state = OCCLUSION_STATE_READY;
        mTasks[count].initNode();
    }

    mGeometryMgr->initCritalSection();
    initThread("FMOD geometry thread", NULL, NULL, GEOMETRY_THREADPRIORITY, 0, GEOMETRY_STACKSIZE, false, 0, mGeometryMgr->mSystem);
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
FMOD_RESULT OcclusionThread::release()
{
    FMOD_RESULT result = FMOD_OK;

    mInitialized = false;
    if (mQueueCrit)
    {
        result = closeThread();
        ASSERT(result == FMOD_OK);
        if (result != FMOD_OK)
        {
            return result;
        }
        
        ASSERT(mTasks);
        FMOD_Memory_Free(mTasks);
        mTasks = 0;
        result = FMOD_OS_CriticalSection_Free(mQueueCrit);
        mQueueCrit = 0;
    }
    return result;
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
FMOD_RESULT OcclusionThread::threadFunc()
{
    mGeometryMgr->mSystem->mGeometryTimeStamp.stampIn();
    OCCLUSION_TASK *task = dequeue();
    if (task && task->state == OCCLUSION_STATE_READY)
    {
        /* test occlusion from channel to listener */
        mGeometryMgr->lineTestAll(&mGeometryMgr->mSystem->mListener[0].mPosition, &task->position, &task->directocclusion, &task->reverbocclusion);

#ifdef FMOD_SUPPORT_MULTIREVERB
        /* test occlusion from main 3D reverb to channel (this replaces ChannelI::calculate3DReverbGain)*/
        float direct_o, reverb_o;
        FMOD_VECTOR reverbpos;
        mGeometryMgr->mSystem->mReverb3D.get3DAttributes(&reverbpos, 0, 0);
        mGeometryMgr->lineTestAll(&task->position, &reverbpos, &direct_o, &reverb_o);
        task->reverbgain = 1.0f - reverb_o;
#endif
        /* tell channeli->update to trigger callback */
        task->state = OCCLUSION_STATE_UPDATED;
        mGeometryMgr->mSystem->mGeometryTimeStamp.stampOut(95);
    }
    else
    {
        mGeometryMgr->mSystem->mGeometryTimeStamp.stampOut(95);
        FMOD_OS_Time_Sleep(10);
    }

    return FMOD_OK;
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
void OcclusionThread::enqueue(unsigned int index, unsigned int currenthandle, FMOD_VECTOR *worldposition)
{   
    if (!mInitialized)
    {
        init();
    }

    LocalCriticalSection crit(mQueueCrit, true);
    OCCLUSION_TASK *task = &mTasks[index];

    if (task->state != OCCLUSION_STATE_UPDATED) /* don't enqueue if it's most recent update hasn't be checked by main thread */
    {
        task->state = OCCLUSION_STATE_READY;
        task->currenthandle = currenthandle;
        FMOD_Vector_Copy(worldposition, &task->position);
        if (task->getNext() == task && task->getPrev() == task) /* make sure it is not already queued */
        {
            task->addBefore(&mQueueRoot);
        }
    }
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
OCCLUSION_TASK* OcclusionThread::dequeue()
{
    OCCLUSION_TASK *task = 0;
    LocalCriticalSection crit(mQueueCrit, true);

    if (!mQueueRoot.isEmpty())
    {
        task = (OCCLUSION_TASK*)mQueueRoot.getNext();
        task->removeNode();
    }

    return task;
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
OCCLUSION_TASK* OcclusionThread::retrieveOcclusion(unsigned int index)
{
    ASSERT(index > 0);

    if (!mTasks)
    {
        return NULL;
    }

    OCCLUSION_TASK *task = &mTasks[index];
    if (task->state == OCCLUSION_STATE_UPDATED)
    {
        task->state = OCCLUSION_STATE_CHECKED;
        return task;
    }
    
    return NULL;
}

#endif //FMOD_SUPPORT_GEOMETRY_THREADED


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
GeometryMgr::GeometryMgr()
{
    mSystem = 0;
    mMainOctree = 0;
    mRefCount = 0;
    mFirstUpdateItem = 0;
    mWorldSize = 1000.0f;   /* Arbitrary value, could be anything */
    mMoved = true;
#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
    mOcclusionThread.mGeometryMgr = this;
#endif //FMOD_SUPPORT_GEOMETRY_THREADED
#if defined(FMOD_GEOMETRY_DEBUGGING)
    sThis = this;
#endif
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
GeometryMgr::~GeometryMgr()
{
#if defined(FMOD_GEOMETRY_DEBUGGING)
    sThis = 0;
#endif
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
FMOD_RESULT GeometryMgr::aquireMainOctree()
{
    mRefCount++;
    if (!mMainOctree)
    {
        mMainOctree = (Octree *)FMOD_Memory_Alloc(sizeof (Octree));
        if (!mMainOctree)
            return FMOD_ERR_MEMORY;
        new (mMainOctree) Octree(mWorldSize);
    }
    return FMOD_OK;
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
void GeometryMgr::releaseMainOctree()
{
    mRefCount--;
    if (mRefCount <= 0)
    {
        mRefCount = 0;
        if (mMainOctree)
        {
            mMainOctree->~Octree();
            FMOD_Memory_Free(mMainOctree);
            mMainOctree = 0;
        }
    }
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
bool GeometryMgr::mainOctreeLineTestCallback(OctreeNode* item, void* data)
{
    GeometryI::SpatialData* spatialdata = (GeometryI::SpatialData*)item;
    ASSERT(spatialdata->geometry);
    GeometryI::LineTestData* lineTestData = (GeometryI::LineTestData*)data;
    lineTestData->geometryI = spatialdata->geometry;

    return spatialdata->geometry->lineTest(lineTestData);
}

#ifdef _DEBUG
/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
Octree *GeometryMgr::mainOctree()
{
    ASSERT(mMainOctree);
    return mMainOctree;
}
#endif // _DEBUG


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
FMOD_RESULT GeometryMgr::setWorldSize(float worldSize)
{
    if (mWorldSize == worldSize)
    {
        return FMOD_OK;
    }

    mWorldSize = worldSize;

    if (mMainOctree)
    {
        mMainOctree->setMaxSize(worldSize);
    }
    
    return FMOD_OK;
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
FMOD_RESULT GeometryMgr::lineTestAll(const FMOD_VECTOR* start, const FMOD_VECTOR* end, float* directOcclusion, float* reverbOcclusion)
{
#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
    LocalCriticalSection crit(mGeometryCrit, true);
#endif
    flushAll();
    
    GeometryI::LineTestData lineTestData;
    lineTestData.start = *start;
    lineTestData.end = *end;
    lineTestData.directTransmission = 1.0f;
    lineTestData.reverbTransmission = 1.0f;
    lineTestData.geometryI = 0;

    if (mMainOctree)
    {
        mainOctree()->testLine(mainOctreeLineTestCallback, &lineTestData, *start, *end);
    }

    *directOcclusion = 1.0f - lineTestData.directTransmission;
    *reverbOcclusion = 1.0f - lineTestData.reverbTransmission;
    
    return FMOD_OK;
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
FMOD_RESULT GeometryMgr::flushAll()
{    
    GeometryI* geometryI = mFirstUpdateItem;
    mFirstUpdateItem = 0;

    while (geometryI)
    {
        GeometryI* next = geometryI->mNextUpdateItem;
        
        geometryI->mNextUpdateItem = 0;
        ASSERT(geometryI->mToBeUpdated);
        geometryI->mToBeUpdated = false;
    
        geometryI->flush();       
        geometryI = next;
    }

    return FMOD_OK;
}

#if defined(FMOD_GEOMETRY_DEBUGGING)

/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
FMOD_RESULT GeometryMgr::renderTree(void (*renderBox)(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax))
{
    mainOctree()->renderTree(renderBox);
    return FMOD_OK;
}

/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

    [SEE_ALSO]
]
*/
FMOD_RESULT GeometryMgr::testLineTestForEachPolygon()
{
  	GeometryI* current = mSystem->mGeometryList;
	do
	{ 
	    current->testLineTestForEachPolygon();
		current = SAFE_CAST(GeometryI, current->getNext());
	} 
    while (current != mSystem->mGeometryList);

    return FMOD_OK;
}
#endif


#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT GeometryMgr::initCritalSection()
{
    return FMOD_OS_CriticalSection_Create(&mGeometryCrit);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT GeometryMgr::releaseOcclusionThread()
{
    FMOD_RESULT result;
    mOcclusionThread.release();
    result = FMOD_OS_CriticalSection_Free(mGeometryCrit);
    mGeometryCrit = 0;
    return result;
}

#endif //FMOD_SUPPORT_GEOMETRY_THREADED


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT GeometryMgr::getMemoryUsedImpl(MemoryTracker *tracker)
{
    //AJS incomplete
    tracker->add(false, FMOD_MEMBITS_GEOMETRY, sizeof(*this));

    return FMOD_OK;
}

#endif


}

#endif // FMOD_SUPPORT_GEOMETRY
