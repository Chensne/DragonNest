#ifndef _FMOD_NET_H
#define _FMOD_NET_H


#include "fmod.h"

enum
{
    FMOD_PROTOCOL_HTTP = 1,
    FMOD_PROTOCOL_SHOUTCAST,
    FMOD_PROTOCOL_ICECAST
};


FMOD_RESULT FMOD_Net_ParseHTTPStatus(char *buf, int buflen, int *version, int *statuscode);
FMOD_RESULT FMOD_Net_EncodeBase64(char *inbuf, char *outbuf, int outbuflen);
FMOD_RESULT FMOD_Net_SetProxy(const char *proxy);
FMOD_RESULT FMOD_Net_GetProxy(char *proxy, int proxylen);
FMOD_RESULT FMOD_Net_SetTimeout(int timeout);
FMOD_RESULT FMOD_Net_GetTimeout(int *timeout);

extern char *FMOD_Net_ProxyHostname;
extern int   FMOD_Net_ProxyPort;
extern char *FMOD_Net_ProxyAuth;
extern int   FMOD_Net_Timeout;

#endif


