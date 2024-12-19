#ifndef _FMOD_METADATA_H
#define _FMOD_METADATA_H

#include "fmod_settings.h"

#include "fmod.hpp"
#include "fmod_linkedlist.h"

namespace FMOD
{
    class TagNode : public LinkedListNode
    {
      public:

        FMOD_TAGTYPE       mType;
        FMOD_TAGDATATYPE   mDataType;
        char              *mName;
        void              *mData[2];
        unsigned int       mDataLen;
        bool               mUpdated;
        bool               mUnique;
        int                mCurrentBuffer;

      public:

        TagNode()                   { mType = FMOD_TAGTYPE_UNKNOWN; mDataType = FMOD_TAGDATATYPE_BINARY, mName = 0; mData[0] = mData[1] = 0; mDataLen = 0; mUpdated = true; mUnique = false; mCurrentBuffer = 0; }

        FMOD_RESULT init            (FMOD_TAGTYPE type, const char *name, void *data, unsigned int datalen, FMOD_TAGDATATYPE datatype);
        FMOD_RESULT release         ();
        FMOD_RESULT update          (void *data, unsigned int datalen);
    };

    class Metadata
    {
      public:

        TagNode     mList;

        FMOD_RESULT release();

        FMOD_RESULT getNumTags      (int *numtags, int *numtagsupdated);
        FMOD_RESULT getTag          (const char *name, int index, FMOD_TAG *node);
        FMOD_RESULT add             (Metadata *metadata);
        FMOD_RESULT addTag          (TagNode *node);
        FMOD_RESULT addTag          (FMOD_TAGTYPE type, const char *name, void *data, unsigned int datalen, FMOD_TAGDATATYPE datatype, bool unique);
    };
}

#endif


