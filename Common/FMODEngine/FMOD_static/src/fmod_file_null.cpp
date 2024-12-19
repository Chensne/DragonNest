#include "fmod_settings.h"

#include "fmod_file_null.h"

#include <stdio.h>

namespace FMOD
{

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT NullFile::reallyOpen(const char *name_or_data, unsigned int *filesize)
{
	mPosition = 0;

    *filesize = 0;  /* This will be set by the user after this call? */

	return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT NullFile::reallyClose()
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT NullFile::reallyRead(void *buffer, unsigned int size, unsigned int *read)
{
    FMOD_RESULT result = FMOD_OK;

	if (mPosition + size > mLength)
    {
		size = mLength - mPosition;

        result = FMOD_ERR_INVALID_PARAM;
    }

    *read = size;

    mPosition += size;

    return result;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT NullFile::reallySeek(unsigned int pos)
{
    mPosition = pos;

    if (mPosition > mLength)
    {
        mPosition = mLength;
    }
    if (mPosition < 0)
    {
        mPosition = 0;
    }

    return FMOD_OK;
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
#ifdef FMOD_SUPPORT_MEMORYTRACKER
FMOD_RESULT NullFile::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_FILE, sizeof(NullFile));

    return File::getMemoryUsedImpl(tracker);
}
#endif

}

