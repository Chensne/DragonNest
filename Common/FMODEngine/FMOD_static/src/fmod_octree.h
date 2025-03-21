#ifndef _FMOD_OCTREE_H
#define _FMOD_OCTREE_H

// This is a compramise between an octree and an axis alligned bounding box tree.
// Basicaly an octree which splits space exactly in 8 equal quadrents each time, 
// except that only the splits that are needed to seperate the objects are actually stored.

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_GEOMETRY

//#define FMOD_GEOMETRY_DEBUGGING
//#define FMOD_23TREE

#include "fmod.hpp"

namespace FMOD
{

struct FMOD_AABB
{
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float zMin;
    float zMax;
};
enum OctreeFlags
{
    OCTREE_FLAG_X_SPLIT =            0x000,
    OCTREE_FLAG_Y_SPLIT =            0x001,
    OCTREE_FLAG_Z_SPLIT =            0x002,
    OCTREE_FLAG_SPLIT_MASK =         0x003,
    OCTREE_FLAG_LEAF =               0x004,
    OCTREE_FLAG_CALCULATED_AABB =    0x008,
    OCTREE_FLAG_EXTRA_NODE =         0x010,
    OCTREE_FLAG_FREE =               0x020,
    OCTREE_FLAG_INSERTED =           0x040,
    OCTREE_FLAG_23LEAF =             0x080, 
    OCTREE_FLAG_23ROOT =             0x100,
    OCTREE_FLAG_23NODE =             0x200,
    OCTREE_FLAG_INTERNAL_NODE =      0x400,
};

struct OctreeNode
{
    FMOD_AABB       aabb;
    int             flags;
    unsigned int    splitLevel;
    unsigned int    pos[3];

    OctreeNode     *parent;
    OctreeNode     *hi;
    OctreeNode     *lo;
    OctreeNode     *nextItem;
};

class Octree
{
  public:
    // data stucture for the recursive testLine to save on extra
    // parameters on the stack.
    struct RecursionData
    {
        bool (*octreeLineTestCallback)(OctreeNode* item, void* data);
        void *data;
        bool  exit;
    };

    // see setMaxSize for description of maxSize
    Octree(float maxSize);
    ~Octree();

    OctreeNode* mRoot;
    FMOD_VECTOR mCenter;
    float mScale;
    OctreeNode* mFreeList;    

    // Inserting / deleting and updating items
    void insertItem(OctreeNode* item);    
    void deleteItem(OctreeNode* item);
    void updateItem(OctreeNode* item);

    // For update optimzation
    bool needsReinsert(OctreeNode* item);
    void updateItemAABB(OctreeNode* item) { adjustAABBs(item); }

    // Get the Axis Aligned Bounding Box that bounds the entire tree
    void getAABB(FMOD_AABB* aabb);

    // Internal nodes.
    // A memory pool for internal tree nodes.
    // The user is responsible for providing enough internal nodes.
    // The maximum number of internal nodes necessery is equal to the number of
    // leaf nodes added with insertItem.
    void addInternalNode(OctreeNode* item);
    void removeInternalNode(OctreeNode* item);

    // Debugging functions
#if defined(_DEBUG) && defined(FMOD_GEOMETRY_DEBUGGING)
    void checkTree(OctreeNode* node);
    void calculateAverageNodeElements();
    void checkNodeIsContained(OctreeNode* node);
    void calculateAverageNodeElements(OctreeNode* node);
#endif

    // same maxSize as passed as a parameter to the constructor.
    // sets the maximum range in x,y and z for the tree.
    // If the range is too large the tree will loose precision and
    // if the range is too small, dimensions may wrap and items
    // may end up in the top node. In both cases, the tree
    // will still return all desired intersecting nodes, but may
    // be very slow.
    void setMaxSize(float maxSize);

    // test a line for collision with the tree
    bool testLine(bool (*octreeLineTestCallback)(OctreeNode* item, void* data), void* data, const FMOD_VECTOR& a, const FMOD_VECTOR& b);

#if defined(FMOD_GEOMETRY_DEBUGGING)
    void renderTree(void (*renderBox)(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax));
    void renderTreeInternal(OctreeNode* node, void (*renderBox)(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax));
#endif

#if defined(FMOD_23TREE)
    // Use a 2-3-tree for to keep a sorted list of elements in each node.
    // This could be used to speed up the pathalogical case where there
    // are many AABBs in the one node. However, it may be slower in the
    // average case.
    // remove23Tree not implemented yet.
    void insert23Tree(OctreeNode* parent, OctreeNode** pParent, OctreeNode* item);
    void split23Tree(OctreeNode* node, OctreeNode* item);
    void remove23Tree(OctreeNode* item);
    void check23Tree(OctreeNode* parent, OctreeNode** pParent);
    void check23TreeRecursive(OctreeNode* node);
    int unsigned findLowest(OctreeNode* node);
#endif

    
    static unsigned int ftoint(float f)  
    { 
        unsigned int i;     

        #if (defined(PLATFORM_WINDOWS) && !defined(__MINGW32__) || defined(PLATFORM_XBOX)) && defined(PLATFORM_32BIT)
        
        __asm fld f;      
        __asm fistp i;
        
        #else
        
        i = (int)f;
        
        #endif

        return i;
    }

    // transformation functions
    unsigned int xGetCenter(OctreeNode* node)
    {
        return ftoint((((node->aabb.xMin + node->aabb.xMax) * 0.5f -  mCenter.x) * mScale  * (float)(1 << 30)) + (float)(1 << 30));
    }
    unsigned int yGetCenter(OctreeNode* node)
    {
        return ftoint((((node->aabb.yMin + node->aabb.yMax) * 0.5f -  mCenter.y) * mScale  * (float)(1 << 30)) + (float)(1 << 30));
    }
    unsigned int zGetCenter(OctreeNode* node)
    {
        return ftoint((((node->aabb.zMin + node->aabb.zMax) * 0.5f -  mCenter.z) * mScale  * (float)(1 << 30)) + (float)(1 << 30));
    }

  private:
    void insertInternal(OctreeNode* node, OctreeNode* item);
    void addToFreeList(OctreeNode* item);
    OctreeNode* getFreeNode();
    void adjustAABBs(OctreeNode* node);

    // each node of the tree potentially has a list of leaf nodes that are too close to be split.
    // these functions make adding and removing for this list easier.
    void removeListItem(OctreeNode* next);
    void addListItem(OctreeNode* list, OctreeNode* node);

    static void testLine(OctreeNode* node, FMOD_VECTOR a, FMOD_VECTOR b, RecursionData* recursionData);


};

}

#endif // FMOD_SUPPORT_GEOMETRY

#endif // _FMOD_OCTREE_H
