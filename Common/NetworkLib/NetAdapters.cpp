
#include "stdafx.h"
#include "NetAdapters.h"

CNetAdapters::CNetAdapters() : m_hIpHlp(NULL), m_fnGetIpAddrTable(NULL), m_fnGetAdaptersInfo(NULL)
{
	m_nError = _Init();	
}

CNetAdapters::~CNetAdapters()
{
	if (m_hIpHlp)
		FreeLibrary(m_hIpHlp);
}

void CNetAdapters::GetFirstMacAddress(std::string &strMacAddr)
{
	if (m_vAdapterList.empty())
	{
		m_nError = NetAdapter::ERROR_EMPTYINFO;
		return;
	}

	/*
	std::vector <sAdapterInfo>::iterator ii;
	for (ii = m_vAdapterList.begin(); ii != m_vAdapterList.end(); ii++)
	{
		if ( strcmp((*ii).szIP, "0.0.0.0") != 0 )
		{
			strMacAddr = (*ii).strMacAddress;
			return;
		}
	}
	*/
	strMacAddr = m_vAdapterList[0].strMacAddress;
	m_nError = NetAdapter::ERROR_FIRST_SUCCESS;
}

void CNetAdapters::GetFirstPublicMacAddress(std::string &strMacAddr)
{
	if (m_vAdapterList.empty())
	{
		m_nError = NetAdapter::ERROR_EMPTYINFO;
		return;
	}

	strMacAddr.clear();
	std::vector <sAdapterInfo>::iterator ii;
	if (m_vAdapterList.size() == 1)
	{
		return GetFirstMacAddress(strMacAddr);
	}
	else
	{
		for (ii = m_vAdapterList.begin(); ii != m_vAdapterList.end(); ii++)
		{
			if ((*ii).bPublic)
			{
				strMacAddr = (*ii).strMacAddress;
				m_nError = NetAdapter::ERROR_PUBLIC_SUCCESS;
				break;
			}
		}

		if (ii == m_vAdapterList.end())
			return GetFirstMacAddress(strMacAddr);
	}
}

void CNetAdapters::GetMacAddressList(std::vector<std::string> &vMacAddrList)
{
	for (std::vector <sAdapterInfo>::iterator ii = m_vAdapterList.begin(); ii != m_vAdapterList.end(); ii++)
		vMacAddrList.push_back((*ii).strMacAddress);
}

void CNetAdapters::GetPublicMacAddressList(std::vector<std::string> &vMacAddrList)
{
	for (std::vector <sAdapterInfo>::iterator ii = m_vAdapterList.begin(); ii != m_vAdapterList.end(); ii++)
	{
		if ((*ii).bPublic)
			vMacAddrList.push_back((*ii).strMacAddress);
	}
}

int CNetAdapters::_Init()
{
	if (m_hIpHlp) return false;

	m_hIpHlp = LoadLibrary( _T("iphlpapi.dll") );
	if (m_hIpHlp)
	{
		m_fnGetIpAddrTable = (fnGetIpAddrTable)GetProcAddress(m_hIpHlp, "GetIpAddrTable");
		if (m_fnGetIpAddrTable == NULL)
			return NetAdapter::ERROR_LPTABLE;
		
		m_fnGetAdaptersInfo = (fnGetAdaptersInfo)GetProcAddress(m_hIpHlp, "GetAdaptersInfo");
		if (m_fnGetAdaptersInfo == NULL)
			return NetAdapter::ERROR_ADAPTERINFO;

		_MakeNetAdapterInfo();
		_FindPublicIndex(m_vAdapterList);
#if defined(_CONSOLE) && defined(_DEBUG)		
		for (std::vector <sAdapterInfo>::iterator ii = m_vAdapterList.begin(); ii != m_vAdapterList.end(); ii++)
		{
			printf("** Network Adapter Info ** \r\n** Index [%d]\r\n** Name [%s]\r\n** Desc [%s] \r\n** MacAddr [%s]\r\n** IP[%s]\r\n", (*ii).iIndex, (*ii).strAdapterName.c_str(), (*ii).strDescription.c_str(), (*ii).strMacAddress.c_str(), (*ii).szIP); 
		}
#endif		//#if defined(_CONSOLE) && defined(_DEBUG)
		return NetAdapter::ERROR_NONE;
	}
	return NetAdapter::ERROR_LIBRARY;
}

void CNetAdapters::_MakeNetAdapterInfo()
{
	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = m_fnGetAdaptersInfo(AdapterInfo, &dwBufLen); 
	_ASSERT(dwStatus == ERROR_SUCCESS);

	sAdapterInfo Info;
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	do {
		if (pAdapterInfo->AddressLength > 0 && strcmp(pAdapterInfo->IpAddressList.IpAddress.String, "0.0.0.0") != 0)
		{
			memset(&Info, 0, sizeof(Info));
			Info.iIndex = pAdapterInfo->Index;
			Info.strAdapterName = pAdapterInfo->AdapterName;
			Info.strDescription = pAdapterInfo->Description;

			char szMaxAddr[24];
#if defined(_ID)
			sprintf_s(szMaxAddr, sizeof(szMaxAddr), "%02X:%02X:%02X:%02X:%02X:%02X", pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
#else
			sprintf_s(szMaxAddr, sizeof(szMaxAddr), "%02X-%02X-%02X-%02X-%02X-%02X", pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
#endif
			Info.strMacAddress = szMaxAddr;
			sprintf_s(Info.szIP, sizeof(Info.szIP), "%s", pAdapterInfo->IpAddressList.IpAddress.String);
						
			m_vAdapterList.push_back(Info);
		}

		pAdapterInfo = pAdapterInfo->Next;		// Progress through linked list
	}
	while(pAdapterInfo);						// Terminate if last adapter
}

bool CNetAdapters::_FindPublicIndex(std::vector <sAdapterInfo> &vList)
{
	if(m_fnGetIpAddrTable)
	{
		PMIB_IPADDRTABLE pIPAddrTable;
		DWORD dwSize=0;

		pIPAddrTable=(MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));
		if(!pIPAddrTable)
			return false;

		if(m_fnGetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
		{
			free(pIPAddrTable);
			pIPAddrTable=(MIB_IPADDRTABLE *)malloc(dwSize);
			if(!pIPAddrTable)
				return false;
		}

		if(m_fnGetIpAddrTable(pIPAddrTable, &dwSize, 0) == NO_ERROR)
		{ 
			for(DWORD i=0; i<pIPAddrTable->dwNumEntries ; ++i)
			{
				std::vector <sAdapterInfo>::iterator ii;
				for (ii = vList.begin(); ii != vList.end(); ii++)
				{
					if (pIPAddrTable->table[i].dwIndex == (*ii).iIndex)
						break;
				}
				if (ii == vList.end()) continue;

				DWORD dwIP = ntohl(pIPAddrTable->table[i].dwAddr);
				BOOL bPrivate = FALSE;

				if(CXIP_A(dwIP) == 127)
					continue;
				else if(CXIP_A(dwIP) == 10)
					bPrivate = TRUE;
				else if(CXIP_A(dwIP) == 172)
				{
					if(CXIP_B(dwIP) >= 16 && CXIP_B(dwIP) <= 31) 
						bPrivate = TRUE;
				}
				else if(CXIP_A(dwIP) == 192)
				{
					if(CXIP_B(dwIP) == 168)
						bPrivate=TRUE;
				}

				if(bPrivate == FALSE)
					(*ii).bPublic = true;
				(*ii).dwPublicIP = dwIP;
				DWORD dwNPublicIP = htonl((*ii).dwPublicIP);				
				_strcpy((*ii).szIP, _countof((*ii).szIP), inet_ntoa(*((in_addr*)&dwNPublicIP)), (int)strlen(inet_ntoa(*((in_addr*)&dwNPublicIP))));
			}
		}
		else
		{
			_ASSERT_EXPR(0, L"Initfailed!!!");
			return false;
		}

		// Clear
		free(pIPAddrTable);
		return true;
	}

	return false;
}