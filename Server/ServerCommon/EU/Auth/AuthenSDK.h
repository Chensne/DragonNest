#ifndef	_AUTHEN_H_
#define	_AUTHEN_H_

typedef void (*AUTHEN_CALLBACK)(unsigned __int64 nSessionId, int nResult, const char *pszUsername,
								const char *pszUserId, const char *pszUserCountry,
								const char *pszStartDate, const char *pszEndDate);

_declspec(dllexport) BOOL SDGASInit(AUTHEN_CALLBACK pfnAuthenCallback);
_declspec(dllexport) void SDGASAuthen(unsigned __int64 nSessionId, const char *pszUsername,
									   const char *pszPassword, const char *pszClientip);
_declspec(dllexport) void SDGASTerminate();

#endif	// _AUTHEN_H_