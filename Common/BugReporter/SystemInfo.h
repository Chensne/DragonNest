#pragma once

#include <dxdiag.h>
#include <string>


class SystemInfo
{
public:
	SystemInfo();
	virtual ~SystemInfo();

	bool Initialize();

	std::string GetCpuString()				{ return m_szCpuString; }
	std::string GetVideoCardString()		{ return m_szVideoCardName; }
	
	std::string GetDxVersionString()		{ return m_szDXVersion; }
	std::string GetWindowVersionString()	{ return m_szWinVersion; }
	int			GetVideoPhysicMemory()		{ return m_nVideoPhysicMemory; }
	static std::string GetOSInfo();
	std::string GetUserInfo()				{ return m_szUserName;}
	std::string GetMemInfoString()			{ GetMemInfo(); return m_szMemInfo; }
	std::string GetLocalIPString()			{ GetLocalIP();return m_szLocalIPString;}

private:

	void FreeIDxDiagContainer();
	void GetPhysicalMemoryInMB();
	bool GetProperty( IDxDiagContainer * pContainer, const char* property_name, std::string* out_value );
	bool GetProperty( const char* container_name0, const char* property_name, std::string* out_value );
	bool GetChildContainer( const char* name0, IDxDiagContainer ** ppChild );
	bool GetDirectXVersion( DWORD * pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, CHAR* pcDirectXVersionLetter );
	bool GetDisplayDeviceDescription( DWORD dwDevice, std::string* pwstrName );
	bool GetDisplayDeviceMemoryInMB( DWORD dwDevice, int* pDisplayMemory );
	bool GetDisplayDeviceProp( DWORD dwDevice, const char* prop_name, std::string* pwstrProp );
	bool GetDisplayDeviceNode( DWORD dwDeviceIndex, IDxDiagContainer ** ppNode );
	bool GetChildByIndex(  IDxDiagContainer* pParent, DWORD dwIndex, IDxDiagContainer** ppChild );
	void CPUInfo();
	bool ConverteCHAR2WCHAR( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar );
	bool ConverWCHAR2CHAR( CHAR* strDestination, const WCHAR* wstrSource, int cchDestChar );
public:
	static bool GetWinVersion(char* pszVersion, int *nVersion, char* pszMajorMinorBuild);
private:
	bool GetMemInfo();
	bool GetLocalIP();

	bool				m_bCleanupCOM;
	IDxDiagProvider*	m_pDxDiagProvider;
	IDxDiagContainer*	m_pDxDiagRoot;

	std::string		m_szCpuString;
	std::string		m_szVideoCardName;
	std::string		m_szDXVersion;
	std::string		m_szWinVersion;
	std::string		m_szUserName;
	std::string		m_szMemInfo;
	std::string		m_szLocalIPString;
	
	float			m_fSystemPhysicMemory;
	int				m_nVideoPhysicMemory;
	DWORD			m_dwDXVersionMajor;
	DWORD			m_dwDXVersionMinor;
	CHAR			m_cDXVersionLetter;
};


