#include <windows.h>
#include "iasiodrv.h"
#include "asiolist.h"

#include "fmod_memory.h"

#define ASIODRV_DESC		"description"
#define INPROC_SERVER		"InprocServer32"
#define ASIO_PATH			"software\\asio"
#define COM_CLSID			"clsid"

// ******************************************************************
// Local Functions 
// ******************************************************************
static LONG findDrvPath (char *clsidstr,char *dllpath,int dllpathsize)
{
	HKEY			hkEnum,hksub,hkpath;
	char			databuf[512];
	LONG 			cr,rc = -1;
	DWORD			datatype,datasize;
	DWORD			index;
	OFSTRUCT		ofs;
	HFILE			hfile;
	BOOL			found = FALSE;

	CharLowerBuff(clsidstr,(DWORD)strlen(clsidstr));
	if ((cr = RegOpenKey(HKEY_CLASSES_ROOT,COM_CLSID,&hkEnum)) == ERROR_SUCCESS) {

		index = 0;
		while (cr == ERROR_SUCCESS && !found) {
			cr = RegEnumKey(hkEnum,index++,(LPTSTR)databuf,512);
			if (cr == ERROR_SUCCESS) {
				CharLowerBuff(databuf,(DWORD)strlen(databuf));
				if (!(strcmp(databuf,clsidstr))) {
					if ((cr = RegOpenKeyEx(hkEnum,(LPCTSTR)databuf,0,KEY_READ,&hksub)) == ERROR_SUCCESS) {
						if ((cr = RegOpenKeyEx(hksub,(LPCTSTR)INPROC_SERVER,0,KEY_READ,&hkpath)) == ERROR_SUCCESS) {
							datatype = REG_SZ; datasize = (DWORD)dllpathsize;
							cr = RegQueryValueEx(hkpath,0,0,&datatype,(LPBYTE)dllpath,&datasize);
							if (cr == ERROR_SUCCESS) {
								FMOD_memset(&ofs,0,sizeof(OFSTRUCT));
								ofs.cBytes = sizeof(OFSTRUCT); 
								hfile = OpenFile(dllpath,&ofs,OF_EXIST);
								if (hfile) rc = 0; 
							}
							RegCloseKey(hkpath);
						}
						RegCloseKey(hksub);
					}
					found = TRUE;	// break out 
				}
			}
		}				
		RegCloseKey(hkEnum);
	}
	return rc;
}


static LPASIODRVSTRUCT newDrvStruct (HKEY hkey,char *keyname,int drvID,LPASIODRVSTRUCT lpdrv)
{
	HKEY	hksub;
	char	databuf[256];
	char	dllpath[MAXPATHLEN];
	WORD	wData[100];
	CLSID	clsid;
	DWORD	datatype,datasize;
	LONG	cr,rc;

	if (!lpdrv) {
		if ((cr = RegOpenKeyEx(hkey,(LPCTSTR)keyname,0,KEY_READ,&hksub)) == ERROR_SUCCESS) {

			datatype = REG_SZ; datasize = 256;
			cr = RegQueryValueEx(hksub,COM_CLSID,0,&datatype,(LPBYTE)databuf,&datasize);
			if (cr == ERROR_SUCCESS) {
				rc = findDrvPath (databuf,dllpath,MAXPATHLEN);
				if (rc == 0) {
					lpdrv = FMOD_Object_Calloc(ASIODRVSTRUCT);
					if (lpdrv) {
						FMOD_memset(lpdrv,0,sizeof(ASIODRVSTRUCT));
						lpdrv->drvID = drvID;
						MultiByteToWideChar(CP_ACP,0,(LPCSTR)databuf,-1,(LPWSTR)wData,100);
						if ((cr = CLSIDFromString((LPOLESTR)wData,(LPCLSID)&clsid)) == S_OK) {
							FMOD_memcpy(&lpdrv->clsid,&clsid,sizeof(CLSID));
						}

						datatype = REG_SZ; datasize = 256;
						cr = RegQueryValueEx(hksub,ASIODRV_DESC,0,&datatype,(LPBYTE)databuf,&datasize);
						if (cr == ERROR_SUCCESS) {
							strcpy(lpdrv->drvname,databuf);
						}
						else strcpy(lpdrv->drvname,keyname);
					}
				}
			}
			RegCloseKey(hksub);
		}
	}	
	else lpdrv->next = newDrvStruct(hkey,keyname,drvID+1,lpdrv->next);

	return lpdrv;
}

static void deleteDrvStruct (LPASIODRVSTRUCT lpdrv)
{
	IASIO	*iasio;

	if (lpdrv != 0) {
		deleteDrvStruct(lpdrv->next);
		if (lpdrv->asiodrv) {
			iasio = (IASIO *)lpdrv->asiodrv;
			iasio->Release();
		}
		FMOD_Memory_Free(lpdrv);
	}
}


static LPASIODRVSTRUCT getDrvStruct (int drvID,LPASIODRVSTRUCT lpdrv)
{
	while (lpdrv) {
		if (lpdrv->drvID == drvID) return lpdrv;
		lpdrv = lpdrv->next;
	}
	return 0;
}
// ******************************************************************


// ******************************************************************
//	AsioDriverList
// ******************************************************************
AsioDriverList::AsioDriverList ()
{
	HKEY			hkEnum = 0;
	char			keyname[MAXDRVNAMELEN];
	LPASIODRVSTRUCT	pdl;
	LONG 			cr;
	DWORD			index = 0;
	BOOL			fin = FALSE;

	numdrv		= 0;
	lpdrvlist	= 0;

	cr = RegOpenKey(HKEY_LOCAL_MACHINE,ASIO_PATH,&hkEnum);
	while (cr == ERROR_SUCCESS) {
		if ((cr = RegEnumKey(hkEnum,index++,(LPTSTR)keyname,MAXDRVNAMELEN))== ERROR_SUCCESS) {
			lpdrvlist = newDrvStruct (hkEnum,keyname,0,lpdrvlist);
		}
		else fin = TRUE;
	}
	if (hkEnum) RegCloseKey(hkEnum);

	pdl = lpdrvlist;
	while (pdl) {
		numdrv++;
		pdl = pdl->next;
	}

	if (numdrv) CoInitialize(0);	// initialize COM
}


bool AsioDriverList::close()
{
	if (numdrv) {
		deleteDrvStruct(lpdrvlist);
		CoUninitialize();
	}

    return true;
}


LONG AsioDriverList::asioGetNumDev (VOID)
{
	return (LONG)numdrv;
}


LONG AsioDriverList::asioOpenDriver (int drvID,LPVOID *asiodrv)
{
	LPASIODRVSTRUCT	lpdrv = 0;
	long			rc;

	if (!asiodrv) return DRVERR_INVALID_PARAM;

	if ((lpdrv = getDrvStruct(drvID,lpdrvlist)) != 0) {
		if (!lpdrv->asiodrv) {
			rc = CoCreateInstance(lpdrv->clsid,0,CLSCTX_INPROC_SERVER,lpdrv->clsid,asiodrv);
			if (rc == S_OK) {
				lpdrv->asiodrv = *asiodrv;
				return 0;
			}
			// else if (rc == REGDB_E_CLASSNOTREG)
			//	strcpy (info->messageText, "Driver not registered in the Registration Database!");
		}
		else rc = DRVERR_DEVICE_ALREADY_OPEN;
	}
	else rc = DRVERR_DEVICE_NOT_FOUND;
	
	return rc;
}


LONG AsioDriverList::asioCloseDriver (int drvID)
{
	LPASIODRVSTRUCT	lpdrv = 0;
	IASIO			*iasio;

	if ((lpdrv = getDrvStruct(drvID,lpdrvlist)) != 0) {
		if (lpdrv->asiodrv) {
			iasio = (IASIO *)lpdrv->asiodrv;
			iasio->Release();
			lpdrv->asiodrv = 0;
		}
	}

	return 0;
}

LONG AsioDriverList::asioGetDriverName (int drvID,char *drvname,int drvnamesize)
{	
	LPASIODRVSTRUCT			lpdrv = 0;

	if (!drvname) return DRVERR_INVALID_PARAM;

	if ((lpdrv = getDrvStruct(drvID,lpdrvlist)) != 0) {
		if (strlen(lpdrv->drvname) < (unsigned int)drvnamesize) {
			strcpy(drvname,lpdrv->drvname);
		}
		else {
			FMOD_memcpy(drvname,lpdrv->drvname,drvnamesize-4);
			drvname[drvnamesize-4] = '.';
			drvname[drvnamesize-3] = '.';
			drvname[drvnamesize-2] = '.';
			drvname[drvnamesize-1] = 0;
		}
		return 0;
	}
	return DRVERR_DEVICE_NOT_FOUND;
}

LONG AsioDriverList::asioGetDriverPath (int drvID,char *dllpath,int dllpathsize)
{
	LPASIODRVSTRUCT			lpdrv = 0;

	if (!dllpath) return DRVERR_INVALID_PARAM;

	if ((lpdrv = getDrvStruct(drvID,lpdrvlist)) != 0) {
		if (strlen(lpdrv->dllpath) < (unsigned int)dllpathsize) {
			strcpy(dllpath,lpdrv->dllpath);
			return 0;
		}
		dllpath[0] = 0;
		return DRVERR_INVALID_PARAM;
	}
	return DRVERR_DEVICE_NOT_FOUND;
}

LONG AsioDriverList::asioGetDriverCLSID (int drvID,CLSID *clsid)
{
	LPASIODRVSTRUCT			lpdrv = 0;

	if (!clsid) return DRVERR_INVALID_PARAM;

	if ((lpdrv = getDrvStruct(drvID,lpdrvlist)) != 0) {
		FMOD_memcpy(clsid,&lpdrv->clsid,sizeof(CLSID));
		return 0;
	}
	return DRVERR_DEVICE_NOT_FOUND;
}


