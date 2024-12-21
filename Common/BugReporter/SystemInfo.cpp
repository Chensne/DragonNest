#include "stdafx.h"
#include "SystemInfo.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#pragma comment(lib, "dxguid.lib")


//#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p)=NULL; }		}
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);		(p)=NULL; }		}


SystemInfo::SystemInfo() 
: m_bCleanupCOM( false ), m_pDxDiagProvider( NULL ), m_pDxDiagRoot( NULL ), m_fSystemPhysicMemory( 0.0f ), m_dwDXVersionMajor( 0 ), m_dwDXVersionMinor( 0 ), m_cDXVersionLetter( NULL ), m_nVideoPhysicMemory( 0 )
{

}

SystemInfo::~SystemInfo()
{

}


bool SystemInfo::Initialize()
{

	HRESULT hr;
	if( m_pDxDiagRoot != NULL )
	{
		return true;
	}

	hr = CoInitialize(NULL);
	m_bCleanupCOM = SUCCEEDED(hr);

	hr = CoCreateInstance( CLSID_DxDiagProvider,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IDxDiagProvider,
		(LPVOID*) &m_pDxDiagProvider );
	if( SUCCEEDED(hr) )
	{
		DXDIAG_INIT_PARAMS dxDiagInitParam;
		ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );
		dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
		dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
		dxDiagInitParam.bAllowWHQLChecks        = false;
		dxDiagInitParam.pReserved               = NULL;

		hr = m_pDxDiagProvider->Initialize( &dxDiagInitParam ); 
		if( SUCCEEDED(hr) )
		{
			hr = m_pDxDiagProvider->GetRootContainer( & m_pDxDiagRoot );
			if( FAILED(hr) )
			{
				FreeIDxDiagContainer();
				return false;
			}
		}
		else
		{
			FreeIDxDiagContainer();
			return false;
		}
	}
	else
	{
		FreeIDxDiagContainer();
		return false;
	}

	m_szWinVersion = GetOSInfo();

	CPUInfo();
	GetDirectXVersion( &m_dwDXVersionMajor, &m_dwDXVersionMinor, &m_cDXVersionLetter );
	GetPhysicalMemoryInMB();

	DWORD dev = 0;
	GetDisplayDeviceDescription( dev, &m_szVideoCardName );
	GetDisplayDeviceMemoryInMB( dev, &m_nVideoPhysicMemory );

	char szBuff[128] = {0,};
	sprintf_s(szBuff, "[video physic mem : %d MB]", m_nVideoPhysicMemory);
	m_szVideoCardName += szBuff;

	CHAR szTemp[256] = {0,};
	sprintf_s( szTemp, "%d.%d%c", m_dwDXVersionMajor, m_dwDXVersionMinor, m_cDXVersionLetter );
	m_szDXVersion = szTemp;


	static CHAR szUserName[200] = {0,};

	ZeroMemory(szUserName, sizeof(szUserName));
	DWORD UserNameSize = sizeof(szUserName) - 1;

	if (!::GetUserNameA(szUserName, &UserNameSize))
		::strcpy(szUserName, "Unknown");

	m_szUserName = szUserName;

	GetMemInfo();

	return true;
}

bool
SystemInfo::GetMemInfo()
{
	static const int ONE_K = 1024;
	static const int ONE_M = ONE_K * ONE_K;
	static const int ONE_G = ONE_K * ONE_K * ONE_K;

	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	static CHAR szMemoryInfo[2048] = {0,};

	sprintf(szMemoryInfo,
		"%d%% of memory in use.\n"
		"%d MB physical memory.\n"
		"%d MB physical memory free.\n"
		"%d MB paging file.\n"
		"%d MB paging file free.\n"
		"%d MB user address space.\n"
		"%d MB user address space free.",
		MemInfo.dwMemoryLoad, 
		(MemInfo.dwTotalPhys + ONE_M - 1) / ONE_M, 
		(MemInfo.dwAvailPhys + ONE_M - 1) / ONE_M, 
		(MemInfo.dwTotalPageFile + ONE_M - 1) / ONE_M, 
		(MemInfo.dwAvailPageFile + ONE_M - 1) / ONE_M, 
		(MemInfo.dwTotalVirtual + ONE_M - 1) / ONE_M, 
		(MemInfo.dwAvailVirtual + ONE_M - 1) / ONE_M);

	m_szMemInfo = szMemoryInfo;
	return true;
}

bool SystemInfo::GetLocalIP()
{
	m_szLocalIPString = "";
	char Name[256] = { 0, };
	char addr[16] = {0,};

	gethostname(Name,sizeof(Name));
	struct hostent* he = gethostbyname(Name);
	struct in_addr iaddr;
	memset(&iaddr,0,sizeof(iaddr));

	if ( !he )
	{
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);

		gethostname(Name,sizeof(Name));
		he = gethostbyname(Name);
		memset(&iaddr,0,sizeof(iaddr));
	}

	if ( !he )
		return false;

	for(int i =0; he->h_addr_list[i] !=0; i++)
	{
		if(he->h_addrtype!=AF_INET)	  continue;
		memcpy(&iaddr,he->h_addr_list[i],sizeof(iaddr));
		strcpy(addr,inet_ntoa(iaddr));
		
		m_szLocalIPString +=  std::string(addr);
		m_szLocalIPString += "\n";
	}

	return true;
}

std::string 
SystemInfo::GetOSInfo()
{
	CHAR szWinVer[50] = {0,};
	CHAR szMajorMinorBuild[50] = {0,};
	int nWinVer = 0;
	GetWinVersion(szWinVer, &nWinVer, szMajorMinorBuild);

	static CHAR szOSInfo[512] = {0,};

	sprintf_s(szOSInfo, "%s (%s)", szWinVer, szMajorMinorBuild);
	szOSInfo[511] = 0;

	
	return std::string(szOSInfo);
}


void SystemInfo::CPUInfo()
{
	HKEY hKey;
	DWORD dataSize=0;
	CHAR szCPUNAME [64] = {0,};
	CHAR szTempEx [64] = {0,};
	ZeroMemory(szCPUNAME,64);
	ZeroMemory(szTempEx,64);
	LONG regResult;
	regResult = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey);

	if(regResult == ERROR_SUCCESS){
		dataSize = sizeof (szCPUNAME);
		regResult = ::RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL,
			(LPBYTE)szCPUNAME, &dataSize);
		int num=0;
		bool bspace=true;
		for(int i=0;i<64;i++)
		{
			if(szCPUNAME[i] != ' ' || bspace==false)
			{
				bspace = false;
				szTempEx[num++] = szCPUNAME[i];
			}
		}
		m_szCpuString = szTempEx;
		//wcscpy(m_szCpuString,szTempEx);
	}
	RegCloseKey (hKey);
}


bool SystemInfo::GetDisplayDeviceMemoryInMB( DWORD dwDevice, int* pDisplayMemory )
{
	if( NULL == pDisplayMemory )
		return false;

	*pDisplayMemory = 0;
	std::string str;
	if( !GetDisplayDeviceProp( dwDevice, "szDisplayMemoryEnglish", &str ) )
		return false;

	int nMem;
	int num_fields;
	num_fields = sscanf_s( str.c_str(), "%d", &nMem );
	if( num_fields != 1 )
	{
		return false;
	}

	*pDisplayMemory = nMem;
	return true;
}


bool SystemInfo::GetDisplayDeviceProp( DWORD dwDevice, const char* prop_name, std::string* pwstrProp )
{
	if( NULL == prop_name || NULL == pwstrProp )
		return false;

	*pwstrProp = "";

	IDxDiagContainer * pDisplayDevice;
	if( !GetDisplayDeviceNode( dwDevice, &pDisplayDevice ) )
		return false;

	if( NULL == pDisplayDevice )
		return false;

	if( !GetProperty( pDisplayDevice, prop_name, pwstrProp ) )
		return false;

	return true;
}



bool SystemInfo::GetDisplayDeviceNode( DWORD dwDeviceIndex, IDxDiagContainer ** ppNode )
{
	if( NULL == ppNode )
		return false;

	IDxDiagContainer * pDevicesNode;
	if( !GetChildContainer( "DxDiag_DisplayDevices", &pDevicesNode ) )
		return false;

	if( !GetChildByIndex( pDevicesNode, dwDeviceIndex, ppNode ) )
		return false;

	if( pDevicesNode ) {
		pDevicesNode->Release();
		pDevicesNode = NULL;
	}
	return true;
}



bool SystemInfo::GetChildByIndex(  IDxDiagContainer * pParent,
									DWORD dwIndex,
									IDxDiagContainer ** ppChild )
{
	HRESULT hr = S_OK;
	if( NULL == pParent || NULL == ppChild )
		return false;

	WCHAR wstr[256] = {0,};
	if( FAILED( pParent->EnumChildContainerNames( dwIndex, wstr, 256 ) ) )
		return false;

	if( FAILED( pParent->GetChildContainer( wstr, ppChild ) ) )
		return false;

	return true;
}


bool SystemInfo::GetDisplayDeviceDescription( DWORD dwDevice, std::string* pstrName )
{
	HRESULT hr = S_OK;
	if( NULL == m_pDxDiagRoot || NULL == pstrName )
		return false;

	IDxDiagContainer * pDisplayDevice;
	if( !GetDisplayDeviceNode( dwDevice, &pDisplayDevice ) )
		return false;

	if( NULL == pDisplayDevice ) 
		return false;

	if( !GetProperty( pDisplayDevice, "szDescription", pstrName ) )
		return false;

	if( pDisplayDevice ) {
		pDisplayDevice->Release();
		pDisplayDevice = NULL;
	}
	return true;
}


bool SystemInfo::GetDirectXVersion( DWORD * pdwDirectXVersionMajor, 
									  DWORD * pdwDirectXVersionMinor,
									  CHAR * pcDirectXVersionLetter )
{
	if( NULL == m_pDxDiagRoot || NULL == pdwDirectXVersionMajor || 
		NULL == pdwDirectXVersionMinor || NULL == pcDirectXVersionLetter )
		return false;

	std::string propval;
	GetProperty( "DxDiag_SystemInfo", "dwDirectXVersionMajor", &propval );
	*pdwDirectXVersionMajor = atoi( propval.c_str() );

	GetProperty( "DxDiag_SystemInfo", "dwDirectXVersionMinor", &propval );
	*pdwDirectXVersionMinor = atoi( propval.c_str() );

	GetProperty( "DxDiag_SystemInfo", "szDirectXVersionLetter", &propval );

	std::string str;
	str = propval;

	if( str.length() > 0 )
		*pcDirectXVersionLetter = str.at(0);
	else
		*pcDirectXVersionLetter = ' ';

	return true;
}



void SystemInfo::GetPhysicalMemoryInMB()
{
	HRESULT hr = S_OK;
	std::string property;
	if( !GetProperty( "DxDiag_SystemInfo", "ullPhysicalMemory", &property ) )
		return;

	float mem = (float)atof( property.c_str() );
	m_fSystemPhysicMemory = mem / (1024 * 1024);
}


bool SystemInfo::GetProperty( IDxDiagContainer * pContainer, const char* property_name, std::string* out_value )
{
	if( NULL == pContainer || NULL == property_name || NULL == out_value )
		return false;

	CCHAR wszPropValue[256] = {0,};
	VARIANT var;
	VariantInit( &var );

	WCHAR wszName[256] = {0,};
	ConverteCHAR2WCHAR( wszName, property_name, 256 );
	wszName[255] = NULL;
	if( SUCCEEDED(pContainer->GetProp( wszName, &var )) )

	{
		switch( var.vt )
		{
		case VT_UI4:
			sprintf_s( wszPropValue, "%d", var.ulVal );
			break;
		case VT_I4:
			sprintf_s( wszPropValue, "%d", var.lVal );
			break;
		case VT_BOOL:
			sprintf_s( wszPropValue, "%s", (var.boolVal) ? "true" : "false" );
			break;
		case VT_BSTR:
			{
				ConverWCHAR2CHAR( wszPropValue, var.bstrVal, 255 );
				wszPropValue[255] = 0;
			}
			break;
		}
		(*out_value) = wszPropValue;
	}
	else
	{
		return false;
	}

	VariantClear( &var );
	return true;
}


bool SystemInfo::GetProperty( const char* container_name0, const char* property_name, std::string* out_value )
{
	if( NULL == container_name0 || NULL == property_name || NULL == out_value )
		return false;

	IDxDiagContainer * pContainer;
	if( !GetChildContainer( container_name0, &pContainer ) )
		return false;

	if( NULL == pContainer )
		return false;

	if( !GetProperty( pContainer, property_name, out_value ) )
		return false;

	if( pContainer ) {
		pContainer->Release();
		pContainer = NULL;
	}

	return true;
}

bool SystemInfo::GetChildContainer( const char* name0, IDxDiagContainer ** ppChild )
{
	if( NULL == m_pDxDiagRoot || NULL == ppChild )
		return false;

	WCHAR wszName[256];
	ConverteCHAR2WCHAR( wszName, name0, 256 );
	wszName[255] = NULL;
	if( FAILED( m_pDxDiagRoot->GetChildContainer( wszName, ppChild ) ) )
		return false;

	if( NULL == *ppChild )
		return false;

	return true;
}



void SystemInfo::FreeIDxDiagContainer()
{
	if( m_pDxDiagProvider ) {
		m_pDxDiagProvider->Release();
		m_pDxDiagProvider = NULL;
	}
	if( m_pDxDiagRoot ) {
		m_pDxDiagRoot->Release();
		m_pDxDiagRoot = NULL;
	}
	if( m_bCleanupCOM )
	{
		CoUninitialize();
		m_bCleanupCOM = false;
	}
}



bool SystemInfo::ConverteCHAR2WCHAR( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar )
{
	if( wstrDestination==NULL || strSource==NULL || cchDestChar < 1 )
		return false;

	int nResult = MultiByteToWideChar( CP_ACP, 0, strSource, -1, wstrDestination, cchDestChar );
	wstrDestination[cchDestChar-1] = 0;

	if( nResult == 0 )
		return false;

	return true;
}


bool SystemInfo::ConverWCHAR2CHAR( CHAR* strDestination, const WCHAR* wstrSource, int cchDestChar )
{
	if( strDestination==NULL || wstrSource==NULL || cchDestChar < 1 )
		return false;

	int nResult = WideCharToMultiByte( CP_ACP, 0, wstrSource, -1, strDestination, cchDestChar*sizeof(CHAR), NULL, NULL );
	strDestination[cchDestChar-1] = 0;

	if( nResult == 0 )
		return false;

	return true;
}

/// \brief 윈도우즈 버전을 알아낸다.
///
/// This table has been assembled from Usenet postings, personal observations, 
/// and reading other people's code.  Please feel free to add to it or correct 
/// it.
///
/// <pre>
/// dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
/// 95            1               4                 0            950
/// 95 SP1        1               4                 0            >950 && <=1080
/// 95 OSR2       1               4               <10            >1080
/// 98            1               4                10            1998
/// 98 SP1        1               4                10            >1998 && <2183
/// 98 SE         1               4                10            >=2183
/// ME            1               4                90            3000
///
/// NT 3.51       2               3                51
/// NT 4          2               4                 0            1381
/// 2000          2               5                 0            2195
/// XP            2               5                 1            2600
/// 2003 Server   2               5                 2            3790
///
/// CE            3
/// </pre>
///
/// \param pszVersion 버전 문자열을 집어넣을 포인터
/// \param nVersion 버전 숫자값을 집어넣을 포인터
/// \param pszMajorMinorBuild 빌드 문자열을 집어넣을 포인터
/// \return bool 무사히 실행한 경우에는 true, 뭔가 에러가 생긴 경우에는 false
bool
SystemInfo::GetWinVersion(char* pszVersion, int *nVersion, char* pszMajorMinorBuild)
{
	// from winbase.h
#ifndef VER_PLATFORM_WIN32s
#define VER_PLATFORM_WIN32s 0
#endif

#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS 1
#endif

#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT 2
#endif

#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE 3
#endif

	static const char* WUNKNOWNSTR     = ("Unknown Windows Version");
	static const char* W95STR          = ("Windows 95");
	static const char* W95SP1STR       = ("Windows 95 SP1");
	static const char* W95OSR2STR      = ("Windows 95 OSR2");
	static const char* W98STR          = ("Windows 98");
	static const char* W98SP1STR       = ("Windows 98 SP1");
	static const char* W98SESTR        = ("Windows 98 SE");
	static const char* WMESTR          = ("Windows ME");
	static const char* WNT351STR       = ("Windows NT 3.51");
	static const char* WNT4STR         = ("Windows NT 4");
	static const char* W2KSTR          = ("Windows 2000");
	static const char* WXPSTR          = ("Windows XP");
	static const char* W2003SERVERSTR  = ("Windows 2003 Server");
	static const char* WVISTASTR			= ("Windows Vista");
	static const char* W7STR			= ("Windows 7");
	static const char* WCESTR          = ("Windows CE");

	static const int WUNKNOWN      = 0;
	static const int W9XFIRST      = 1;
	static const int W95           = 1;
	static const int W95SP1        = 2;
	static const int W95OSR2       = 3;
	static const int W98           = 4;
	static const int W98SP1        = 5;
	static const int W98SE         = 6;
	static const int WME           = 7;
	static const int W9XLAST       = 99;
	static const int WNTFIRST      = 101;
	static const int WNT351        = 101;
	static const int WNT4          = 102;
	static const int W2K           = 103;
	static const int WXP           = 104;
	static const int W2003SERVER   = 105;
	static const int WVISTA = 106;
	static const int W7 = 107;
	static const int WNTLAST       = 199;
	static const int WCEFIRST      = 201;
	static const int WCE           = 201;
	static const int WCELAST       = 299;

	if (!pszVersion || !nVersion || !pszMajorMinorBuild) 
		return false;

	::strcpy(pszVersion, WUNKNOWNSTR);
	*nVersion = WUNKNOWN;

	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo)) return false;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF; // Win 95 needs this

	CHAR buf[1024] = {0, };
	sprintf(buf, "%u.%u.%u", dwMajorVersion, dwMinorVersion, dwBuildNumber);
	::strcpy(pszMajorMinorBuild, buf);

	if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
	{
		if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
		{
			::strcpy(pszVersion, W95STR);
			*nVersion = W95;
		}
		else if ((dwMinorVersion < 10) && 
			((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
		{
			::strcpy(pszVersion, W95SP1STR);
			*nVersion = W95SP1;
		}
		else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
		{
			::strcpy(pszVersion, W95OSR2STR);
			*nVersion = W95OSR2;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
		{
			::strcpy(pszVersion, W98STR);
			*nVersion = W98;
		}
		else if ((dwMinorVersion == 10) && 
			((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
		{
			::strcpy(pszVersion, W98SP1STR);
			*nVersion = W98SP1;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
		{
			::strcpy(pszVersion, W98SESTR);
			*nVersion = W98SE;
		}
		else if (dwMinorVersion == 90)
		{
			::strcpy(pszVersion, WMESTR);
			*nVersion = WME;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
		{
			::strcpy(pszVersion, WNT351STR);
			*nVersion = WNT351;
		}
		else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
		{
			::strcpy(pszVersion, WNT4STR);
			*nVersion = WNT4;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
		{
			::strcpy(pszVersion, W2KSTR);
			*nVersion = W2K;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
		{
			::strcpy(pszVersion, WXPSTR);
			*nVersion = WXP;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
		{
			::strcpy(pszVersion, W2003SERVERSTR);
			*nVersion = W2003SERVER;
		}
		else if ((dwMajorVersion == 6) && (dwMinorVersion == 0))
		{
			::strcpy(pszVersion, WVISTASTR);
			*nVersion = WVISTA;
		}
		else if ((dwMajorVersion == 6) && (dwMinorVersion == 1))
		{
			::strcpy(pszVersion, W7STR);
			*nVersion = W7;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
	{
		::strcpy(pszVersion, WCESTR);
		*nVersion = WCE;
	}

	return true;

#undef VER_PLATFORM_WIN32s
#undef VER_PLATFORM_WIN32_WINDOWS
#undef VER_PLATFORM_WIN32_NT
#undef VER_PLATFORM_WIN32_CE
}
