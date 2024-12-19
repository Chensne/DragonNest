#ifndef _FMOD_FILE_NULL_H
#define _FMOD_FILE_NULL_H

#include "fmod_settings.h"
#include "fmod_file.h"

namespace FMOD
{
    class NullFile : public File
    {
        DECLARE_MEMORYTRACKER

      private:

        unsigned int  mPosition;
      
      public:

        FMOD_RESULT reallyOpen(const char *name_or_data, unsigned int *filesize);
        FMOD_RESULT reallyClose();
        FMOD_RESULT reallyRead(void *buffer, unsigned int size, unsigned int *read);
        FMOD_RESULT reallySeek(unsigned int pos);
   };
}

#endif


