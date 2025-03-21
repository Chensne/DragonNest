#ifndef _FMOD_FILE_NET_H
#define _FMOD_FILE_NET_H

#include "fmod_settings.h"

#include "fmod_file.h"
#include "fmod_linkedlist.h"
#include "fmod_metadata.h"

namespace FMOD
{
    class NetFile : public File
    {
        DECLARE_MEMORYTRACKER

      private:

	    void          *mHandle;
        int            mProtocol;
        unsigned int   mAbsolutePos;
        int            mHttpStatus;
        int            mMetaint;
        unsigned int   mBytesBeforeMeta;
        char          *mMetabuf;
        int            mMetaFormat;
        Metadata       mMetadata;
        unsigned short mPort;
        char           mHost[FMOD_STRING_MAXPATHLEN];
        FMOD_RESULT    mConnectStatus;
        bool           mChunked;
        unsigned int   mBytesLeftInChunk;

        FMOD_RESULT    parseUrl(char *url, char *host, int hostlen, char *auth, int authlen, unsigned short *port, char *file, const int filelen, bool *mms);
        FMOD_RESULT    openAsHTTP(const char *name_or_data, char *host, char *name, char *auth, unsigned short port, unsigned int *filesize);
        FMOD_RESULT    openAsMMS(const char *name_or_data, char *host, char *name, char *auth, unsigned short port, unsigned int *filesize);

        static void   asyncConnectCallback(void *userdata);

      public:

        NetFile();

        FMOD_RESULT   getMetadata(Metadata **metadata);

        FMOD_RESULT   reallyOpen(const char *name_or_data, unsigned int *filesize);
        FMOD_RESULT   reallyClose();
        FMOD_RESULT   reallyRead(void *buffer, unsigned int size, unsigned int *read);
        FMOD_RESULT   reallySeek(unsigned int pos);
        FMOD_RESULT   reallyCancel();

        FMOD_RESULT   getSeekable(bool *seekable) { *seekable = false; return FMOD_OK; }

        void          asyncConnect();

      protected:

        friend class File;

        static FMOD_RESULT init();
        static FMOD_RESULT shutDown();
   };
}

#endif


