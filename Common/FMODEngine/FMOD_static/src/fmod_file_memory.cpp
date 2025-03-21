#include "fmod_settings.h"

#include "fmod_file_memory.h"

#include <stdio.h>
#include <string.h>

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
FMOD_RESULT MemoryFile::reallyOpen(const char *name_or_data, unsigned int *filesize)
{
	mMem             = (signed char *)name_or_data;
	mPosition        = 0;
    mFlags          &= ~FMOD_FILE_BUFFERISSTRING;
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
FMOD_RESULT MemoryFile::reallyClose()
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
FMOD_RESULT MemoryFile::reallyRead(void *buffer, unsigned int size, unsigned int *read)
{
    FMOD_RESULT result = FMOD_OK;

	if (mPosition + size > mFileSize)
    {
		size = mFileSize - mPosition;

        result = FMOD_ERR_FILE_EOF;
    }

	FMOD_memcpy(buffer, (char *)mMem + mPosition, size);

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
FMOD_RESULT MemoryFile::reallySeek(unsigned int pos)
{
    mPosition = pos;

    if (mPosition > mFileSize)
    {
        mPosition = mFileSize;
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
FMOD_RESULT MemoryFile::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_FILE, sizeof(MemoryFile));

    return File::getMemoryUsedImpl(tracker);
}
#endif

}


