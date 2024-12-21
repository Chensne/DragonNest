#include "pch.h"
#include "MachineIdInfo.h"
#include <Nb30.h>
#include <tlhelp32.h>

CSystemInfo::CSystemInfo ()
{
	memset(SupportId, 0, sizeof(SupportId));
	memset(MachineId, 0, sizeof(MachineId));
/*	
	m_szVGAInfo[0] = _T('\0');
	GenerateSystemInfo( m_szVGAInfo );
*/
}

CSystemInfo::~CSystemInfo ()
{
}

void CSystemInfo::Init()
{
	// Get MachineId (with MAC address, Hard Serial of System Hard)
	{
		BYTE  OriginalId[16] = { 0, };
		bool  bAddressSet = false;
		UCHAR adapter_type = 0;

		// MAC Address (0-5)

		// WinXP에서 MAC 주소를 제대로 못 가져와서 고침
		// Thanks to veblush

		//LANA_ENUM AdapterList;
		//AdapterList.length = 0;

		//NCB Ncb; memset(&Ncb, 0, sizeof(NCB));
		//Ncb.ncb_command = NCBENUM;
		//Ncb.ncb_buffer	= (unsigned char *)&AdapterList;
		//Ncb.ncb_length	= sizeof(AdapterList);
		//Netbios(&Ncb);

		//for (i=0; i<AdapterList.length; i++)
		//{
		//	memset(&Ncb, 0, sizeof(Ncb));
		//	Ncb.ncb_command  = NCBRESET;
		//	Ncb.ncb_lana_num = i;
		//	
		//	if (Netbios(&Ncb) != NRC_GOODRET) continue;

		//	memset(&Ncb, 0, sizeof(Ncb));
		//	Ncb.ncb_command	 = NCBASTAT;
		//	Ncb.ncb_lana_num = i;
		//	strcpy((char *) Ncb.ncb_callname, "*");
		//	struct ASTAT {
		//		ADAPTER_STATUS adapt;
		//		NAME_BUFFER NameBuff[30];
		//	} Adapter;
		//	memset(&Adapter, 0, sizeof(Adapter));
		//	Ncb.ncb_buffer = (unsigned char *)&Adapter;
		//	Ncb.ncb_length = sizeof(Adapter);

		//	if (Netbios(&Ncb) == 0) 
		//	{
		//		adapter_type = Adapter.adapt.adapter_type;
		//		if (adapter_type == 0xFE || adapter_type == 0xFF)
		//		{
		//			for(j=0; j<6; j++) OriginalId[j] = Adapter.adapt.adapter_address[j];
		//			bAddressSet = true;
		//		}
		//		break;
		//	}
		//}

		//if (!bAddressSet && adapter_type != 0) OriginalId[0] = adapter_type;
		{
			UINT			 dataByte = 0;
			IP_ADAPTER_INFO* adapterInfo = NULL;

			if (GetAdaptersInfo(NULL, (ULONG*)&dataByte) == ERROR_BUFFER_OVERFLOW && dataByte > 0)
			{
				adapterInfo = (IP_ADAPTER_INFO*)malloc(dataByte);
				if (GetAdaptersInfo(adapterInfo, (ULONG*)&dataByte) != ERROR_SUCCESS)
				{
					free(adapterInfo);
					adapterInfo = NULL;
				}
			}

			for (IP_ADAPTER_INFO* curInfo = adapterInfo; curInfo != NULL; curInfo = curInfo->Next)
			{
				if (curInfo->Type == MIB_IF_TYPE_ETHERNET)
				{
					bool useThis = true;

					// skip PPP
					if (curInfo->Address[0] == 'D' &&
						curInfo->Address[1] == 'E' &&
						curInfo->Address[2] == 'S' &&
						curInfo->Address[3] == 'T' &&
						curInfo->Address[4] == 0 &&
						curInfo->Address[5] == 0) useThis = false;

					// choose high mac address
					for (UINT j=0; j<6 && useThis; j++)
					{
						if (curInfo->Address[j] > OriginalId[j])
						{
							break;
						}
						else if (curInfo->Address[j] < OriginalId[j])
						{
							useThis = false;
						}
					}

					if (useThis)
					{
						for (UINT j = 0; j < 6; j++)
						{
							OriginalId[j] = curInfo->Address[j];
						}
					}
				}
			}

			if (adapterInfo != NULL)
			{
				free(adapterInfo);
			}
		}

		// Hard Serial (6-9)

		TCHAR MainFolder[512];
		GetWindowsDirectory(MainFolder, sizeof(MainFolder));
		MainFolder[3] = 0;

		TCHAR VolumeNameBuffer[512], FileSystemNameBuffer[512]; 
		DWORD VolumeSerialNumber, MaximumComponentLength, FileSystemFlags;
		
		if (GetVolumeInformation(MainFolder, VolumeNameBuffer, sizeof(FileSystemNameBuffer), &VolumeSerialNumber, 
			&MaximumComponentLength, &FileSystemFlags, FileSystemNameBuffer, sizeof(FileSystemNameBuffer)) == TRUE)
		{
			*(DWORD*)&OriginalId[6] = VolumeSerialNumber;
		}

		// Empty Space (10-13 : 0 padding)

		// Markup Hash (14-15)

		WORD nHash = 0;
		for(UINT i = 0; i < 6; i++)
		{
			nHash += ((WORD*)OriginalId)[i] * (i+3);
		}
		*(WORD*)&OriginalId[14] = nHash;

		/*
		// TODO: Test Code
		{
			#define V16(x) ((x >= 'A') ? (x - 'A' + 10) : (x - '0'))
			const char* TestId = "000000000893EA172E0D00000000E6CA";
			for(int i=0; i<16; i++)
				OriginalId[i] = (V16(TestId[i*2]) << 4) + V16(TestId[i*2+1]);
		}
		*/

		// Encryption
//		SimpleStreamEncrypt3(OriginalId, MachineId, 16, 0x94F901A6);
		memcpy(MachineId, OriginalId, 16);
	}

	// Get or Create SupportId
	{
		HKEY hKey;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"), 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
		if (hKey == NULL) 
		{
			for(int i=0; i<16; i++) SupportId[i] = MachineId[i]; return;
			//throw CAppException("Cannot open SupportId.");
		}

		DWORD type;
		BYTE  data[256];
		DWORD datasize = sizeof(data);
		GUID  uid;
		BOOL  bValid = FALSE;
 
		if (RegQueryValueEx(hKey, _T("CxSupportId"), NULL, &type, data, &datasize) == ERROR_SUCCESS && datasize == 16)
		{
			memcpy(SupportId, &data, sizeof(GUID));
		}
		else
		{
			CoCreateGuid(&uid);
			RegSetValueEx(hKey, _T("CxSupportId"), NULL, REG_BINARY, (LPBYTE)&uid, sizeof(GUID));
			memcpy(SupportId, &uid, sizeof(GUID));
		}
		
		RegCloseKey(hKey);
	}   
}

const unsigned char* CSystemInfo::GetSupportId()
{
	return SupportId;
}

const unsigned char* CSystemInfo::GetMachineId()
{
	return MachineId;
}

// TODO: Implement vector-like class for storing variable length array.
class CCandidates
{
public:
	enum 
	{
		kMaxCandidateCount = 128,
		kMaxProcNameLen = 32,
		kMaxWindowCaptionLen = 32,
	};
	CCandidates() { m_Count = 0; memset(m_Code, 0, sizeof(m_Code)); }
	inline	void	InsertCandidate(char* Code, LPCTSTR ProcName, LPCTSTR WindowCaption)
	{
		if (m_Count >= kMaxCandidateCount) 
		{
#ifdef _DEBUG
            MessageBox(0, _T("Candidates list overflow, Increase kMaxCandidates constant!"), _T("Error"), MB_ICONERROR | MB_OK);
			DebugBreak();
#endif
			return;
		}

		m_Code[m_Count] = *((DWORD*)Code);

		//_tcsncpy(m_ProcName[m_Count], ProcName, kMaxProcNameLen - 1);
		_tcsncpy_s(m_ProcName[m_Count], ProcName, kMaxProcNameLen - 1);
		m_ProcName[m_Count][kMaxProcNameLen - 1] = '\0';

		//_tcsncpy(m_WindowCaption[m_Count], WindowCaption, kMaxWindowCaptionLen - 1);
		_tcsncpy_s(m_WindowCaption[m_Count], WindowCaption, kMaxWindowCaptionLen - 1);
		m_WindowCaption[m_Count][kMaxWindowCaptionLen - 1] = '\0';

		m_Count++;
	}

	int			m_Count;
	DWORD		m_Code[kMaxCandidateCount];
	TCHAR		m_ProcName[kMaxCandidateCount][kMaxProcNameLen];
	TCHAR		m_WindowCaption[kMaxCandidateCount][kMaxWindowCaptionLen];
};

// TODO: Insertion Sort & Binary Search for performance.
class CProcessMap
{
public:
	enum 
	{
		kMaxProcCount = 256,
		kMaxProcNameLen = 32,
	};
	CProcessMap() { m_Count = 0; memset(m_ProcId, 0, sizeof(m_ProcId)); }
	void	AddProcess(DWORD ProcId, LPCTSTR ProcName)
	{
		// No duplication check implemented.

		if (m_Count >= kMaxProcCount) return;
		m_ProcId[m_Count] = ProcId;

		//_tcsncpy(m_ProcName[m_Count], ProcName, kMaxProcNameLen - 1);
		_tcsncpy_s(m_ProcName[m_Count], ProcName, kMaxProcNameLen - 1);
		m_ProcName[m_Count][kMaxProcNameLen - 1] = '\0';

		m_Count++;
	}
    LPCTSTR	GetProcName(DWORD ProcId)
	{
		for (int i = 0; i < m_Count; i++)
			if (m_ProcId[i] == ProcId) return m_ProcName[i];

        return NULL;
	}

	int		m_Count;
	DWORD	m_ProcId[kMaxProcCount];
	TCHAR	m_ProcName[kMaxProcCount][kMaxProcNameLen];
};

LPCTSTR	EnumProcessName;
DWORD	EnumRet;

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
	CCandidates* pCandidates = (CCandidates*)lParam;

	TCHAR WindowCaption[512];
	GetWindowText(hwnd, WindowCaption, sizeof(WindowCaption));
	
	for(int i = 0; i < pCandidates->m_Count; i++)
	{
		if (pCandidates->m_WindowCaption[0] != 0 && WindowCaption[0] != 0 &&
			_tcsicmp(EnumProcessName, pCandidates->m_ProcName[i]) == 0 &&
			_tcsstr(WindowCaption, pCandidates->m_WindowCaption[i]) != 0)
		{ 
			EnumRet = pCandidates->m_Code[i];
			return FALSE;
		}
	}

	// Check for future version of getto separately
	if (_tcsnicmp(EnumProcessName, _T("agent"), 5) == 0 && _tcsstr(WindowCaption, _T("게토")) != 0)
	{
		EnumRet = *(DWORD*)(_T("GT99")); // Future version of Getto
		return FALSE;
	}

	return TRUE;
}

#define INSERT_CANDIDATE(Code, ProcName, WindowCaption) Candidates.InsertCandidate(Code, ProcName, WindowCaption);

DWORD CSystemInfo::GetGameRoomClient()
{
	CCandidates	Candidates;
	CProcessMap ProcessMap;
//	std::map<DWORD, String>	MapPIdName;

	// Build Candidate List (TODO: be secret!)
	{
#include "GRC_DB.h" // couples of INSERT_CANDIDATE(...)
	}

	// Build Map(ProcessId -> ProcessName)
	{
		HANDLE         hProcessSnap = NULL; 
		PROCESSENTRY32 pe32			= {0};

		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) return FALSE;

		pe32.dwSize = sizeof(pe32);
		if (Process32First(hProcessSnap, &pe32) == FALSE) return FALSE;
		do
		{
			TCHAR *p = _tcsrchr(pe32.szExeFile, '\\');
			if (p == NULL) p = pe32.szExeFile; else p++;
			//_tcslwr(p);
			_tcslwr_s(p, _tcslen(p) + 1);

			for(int i = 0; i < Candidates.m_Count; i++)
			{
				if (Candidates.m_WindowCaption[0] == 0 && 
					_tcscmp(Candidates.m_ProcName[i], p) == 0) return Candidates.m_Code[i];
			}
			ProcessMap.AddProcess(pe32.th32ProcessID, p);
//			MapPIdName[pe32.th32ProcessID] = p;
		}
		while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
	}

	// Check Process & Window
	{
		HANDLE         hThreadSnap  = NULL;
		BOOL           bRet			= FALSE; 
		THREADENTRY32  te32			= {0};
 
		hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadSnap == INVALID_HANDLE_VALUE) return FALSE;

		te32.dwSize = sizeof(THREADENTRY32);
		if (Thread32First(hThreadSnap, &te32))
		{
			do
			{
				if (te32.th32ThreadID != 0) 
				{
					EnumProcessName = ProcessMap.GetProcName(te32.th32OwnerProcessID);
//					EnumProcessName = MapPIdName[te32.th32OwnerProcessID];
					if (EnumProcessName != NULL)
					{
						EnumRet = 0;

						EnumThreadWindows(te32.th32ThreadID, EnumThreadWndProc, (LPARAM)&Candidates);
						if (EnumRet != 0) return EnumRet;
					}
				}
			}
			while(Thread32Next(hThreadSnap, &te32));
		}

		CloseHandle(hThreadSnap);
	}
	
	return 0;
}