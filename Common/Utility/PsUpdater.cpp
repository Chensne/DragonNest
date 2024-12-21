

#include "STDAFX.H"
#include "PSUPDATER.H"
#include "COMMONMACROS.H"
#include "BASESET.H"
#include "DEBUGUTIL.H"
#include "SAFESTRINGMACROS.H"
#include "FILESET.H"
#include "DIRECTORY.H"
#include "STRINGSET.H"
#include "..\\..\\SERVER\\SERVERCOMMON\\INIFILE.H"
#include <IO.H>
#include <SHELLAPI.H>

CPsUpdater* g_PsUpdater = NULL;

void __stdcall Internet_Status_Callback (HINTERNET hSession, DWORD Context, DWORD Status, LPVOID pInformation, DWORD InfoLength);

CPsUpdater::CPsUpdater()
{
	Close();	
}

CPsUpdater::~CPsUpdater()
{	
	Close();
}

void CPsUpdater::Close()
{
	m_PsVersionFileName[0] = _T('\0');
	m_PsVersionFileUrl[0] = _T('\0');
	m_PsDownloadRootUrl[0] = _T('\0');
	m_PsDownloadPathUrl[0] = _T('\0');
	m_PsDownloadAccountId[0] = _T('\0');
	m_PsDownloadPassword[0] = _T('\0');
	m_CurPsVersion[0] = _T('\0');
	m_NewPsVersion[0] = _T('\0');
	m_CurPsVersionNo = 0;
	m_NewPsVersionNo = 0;

	m_IsOpen = false;	
	m_IsUpdate = false;
	if( m_hInternet )
	{
		::InternetCloseHandle(m_hInternet);
		m_hInternet = NULL;
	}
	if( m_hInternetConnection )
	{
		::InternetCloseHandle(m_hInternetConnection);
		m_hInternetConnection = NULL;
	}	
}


bool CPsUpdater::Open()
{
	DN_ASSERT(!IsOpen(),	"Already Opened!");

	// 플랜 스냅샷 업데이트 경로 정보 파일 읽음
	{
		TCHAR aIniFile[MAX_PATH] = { _T('\0'), };
		{
			TCHAR aLocalPath[MAX_PATH] = { _T('\0'), };
			TCHAR aFileName[MAX_PATH] = { _T('\0'), };
			TCHAR aExecName[MAX_PATH] = { _T('\0'), };
			TCHAR aDrvName[MAX_PATH] = { _T('\0'), };
			TCHAR aDirName[MAX_PATH] = { _T('\0'), };

			DWORD aRetVal1 = ::GetModuleFileName(NULL, aLocalPath, _countof(aLocalPath));
			if (!aRetVal1) {
				// 오류 발생
				return false;
			}

			errno_t aRetVal2 = _tsplitpath_s(aLocalPath, aDrvName, _countof(aDrvName), aDirName, _countof(aDirName), aFileName, _countof(aFileName), aExecName, _countof(aExecName));
			if (aRetVal2) {
				// 오류 발생
				return false;
			}

			_sntprintf_s(aIniFile, _countof(aIniFile), _T("%s%s%s"), aDrvName, aDirName, DF_PSUD_URLINIFILE);

			if (_taccess_s(CVarArg<MAX_PATH>(_T("%s"), aIniFile), 0)) {
				// 파일 없음
				return false;
			}
		}

		{
			CIniFile PsUrlIniFile;

			BOOL aRetVal3 = PsUrlIniFile.Open(aIniFile);
			if (!aRetVal3) {
				return false;
			}

			PsUrlIniFile.GetValue(DF_PSUD_URLINIKEY, DF_PSUD_URLINIVAL_VERFILE, m_PsVersionFileName);
			if (_T('\0') == m_PsVersionFileName[0]) {
				return false;
			}

			PsUrlIniFile.GetValue(DF_PSUD_URLINIKEY, DF_PSUD_URLINIVAL_VERURL, m_PsVersionFileUrl);
			if (_T('\0') == m_PsVersionFileUrl[0]) {
				return false;
			}
			if (!::PathIsURL(m_PsVersionFileUrl)) {
				return false;
			}

			PsUrlIniFile.GetValue(DF_PSUD_URLINIKEY, DF_PSUD_URLINIVAL_DOWNURL, m_PsDownloadPathUrl);
			if (_T('\0') == m_PsDownloadPathUrl[0]) {
				return false;
			}
			if (!::PathIsUNC(m_PsDownloadPathUrl)) {
				return false;
			}
			_tcsncpy_s(m_PsDownloadRootUrl, m_PsDownloadPathUrl, _countof(m_PsDownloadRootUrl));
			{
				TCHAR aPsDownloadRootUrl1[MAX_PATH] = { _T('\0'), };
				TCHAR aPsDownloadRootUrl2[MAX_PATH] = { _T('\0'), };
				while(true) {
					_tcsncpy_s(aPsDownloadRootUrl1, m_PsDownloadRootUrl, _countof(aPsDownloadRootUrl1));

					BOOL aRetVal = ::PathRemoveFileSpec(aPsDownloadRootUrl1);
					if (!aRetVal) {
						if (_T('\0') != aPsDownloadRootUrl2[0]) {
							_tcsncpy_s(m_PsDownloadRootUrl, aPsDownloadRootUrl2, _countof(m_PsDownloadRootUrl));
						}
						break;
					}

					_tcsncpy_s(aPsDownloadRootUrl2, m_PsDownloadRootUrl, _countof(aPsDownloadRootUrl2));
					_tcsncpy_s(m_PsDownloadRootUrl, aPsDownloadRootUrl1, _countof(m_PsDownloadRootUrl));
				}
			}

			PsUrlIniFile.GetValue(DF_PSUD_URLINIKEY, DF_PSUD_URLINIVAL_ACCID, m_PsDownloadAccountId, DF_PSUD_ACCIDMAXLEN);
// 			if (_T('\0') == m_PsDownloadAccountId[0]) {
// 				return false;
// 			}

			PsUrlIniFile.GetValue(DF_PSUD_URLINIKEY, DF_PSUD_URLINIVAL_PASS, m_PsDownloadPassword, DF_PSUD_PASSMAXLEN);
// 			if (_T('\0') == m_PsDownloadPassword[0]) {
// 				return false;
// 			}
		}
	}

	m_IsOpen = true;

	return true;	
}

bool CPsUpdater::LoadVersion()
{

	DN_ASSERT(IsOpen(),	"Not Opened!");

	// 로컬 폴더에 버전 파일 존재여부 체크 (TRUE : 업데이트 체크 시작 / FALSE : 넘어감)
	TCHAR aCurPsVersion[DF_PSUD_VERSIONSIZE] = { _T('\0'), };
	UINT64 aCurPsVersionNo = 0;
	{
		TCHAR aVersionFile[MAX_PATH] = { _T('\0'), };
		{
			TCHAR aLocalPath[MAX_PATH] = { _T('\0'), };
			TCHAR aFileName[MAX_PATH] = { _T('\0'), };
			TCHAR aExecName[MAX_PATH] = { _T('\0'), };
			TCHAR aDrvName[MAX_PATH] = { _T('\0'), };
			TCHAR aDirName[MAX_PATH] = { _T('\0'), };

			DWORD aRetVal1 = ::GetModuleFileName(NULL, aLocalPath, _countof(aLocalPath));
			if (!aRetVal1) {
				// 오류 발생
				return false;
			}

			errno_t aRetVal2 = _tsplitpath_s(aLocalPath, aDrvName, _countof(aDrvName), aDirName, _countof(aDirName), aFileName, _countof(aFileName), aExecName, _countof(aExecName));
			if (aRetVal2) {
				// 오류 발생
				return false;
			}

			_sntprintf_s(aVersionFile, _countof(aVersionFile), _T("%s%s%s"), aDrvName, aDirName, m_PsVersionFileName);

			if (_taccess_s(CVarArg<MAX_PATH>(_T("%s"), aVersionFile), 0)) {
				// 파일 없음
				return false;
			}
		}

		CStringSet aStringSet;
		{
			CFileSet aPsVersionFile;
			DWORD aRetVal = aPsVersionFile.Open(CVarArg<MAX_PATH>(_T("%s"), aVersionFile));
			if (NOERROR != aRetVal) {
				return false;
			}

			TCHAR aBuffer[DF_PSUD_TMPBUFFSIZE];
			DWORD aBufferSize;
//			WHILE_INFINITE {
			for (;;) {
				USES_CONVERSION;

				::memset(aBuffer, 0, sizeof(aBuffer));
				aBufferSize = sizeof(aBuffer) - sizeof(TCHAR);
				aRetVal = aPsVersionFile.Read(static_cast<LPVOID>(aBuffer), aBufferSize);
				if (ERROR_HANDLE_EOF == aRetVal) {
					// 읽기 종료
					break;
				}
				if (NOERROR != aRetVal) {
					// 오류 발생
					return false;
				}

				if (0 < aBufferSize) {
					aStringSet += A2CT(reinterpret_cast<LPCSTR>(aBuffer));
				}
				else {
					break;
				}
			}
		}
		
		{
			USES_CONVERSION;

			WORD aYear, aBuildNo;
			BYTE aMonth, aDay;

			bool aRetVal = ParseVersion(T2CA(aStringSet.Get(0)), aYear, aMonth, aDay, aBuildNo);
			if (!aRetVal) {
				// 오류 발생
				return false;
			}

			aCurPsVersionNo = MAKELONGLONG(MAKELONG(aBuildNo, aDay), MAKELONG(aMonth, aYear));
		}

		// 버전 문자열에 불필요한 문자들 제거
		{
			USES_CONVERSION;

			INT aStartPtr = 0;
			CStringSet aToken = aStringSet.Tokenize(_T("\n\r"), aStartPtr);
			if (aToken.IsEmpty()) {
				// 오류 발생
				return false;
			}

			_tcsncpy_s(aCurPsVersion, _countof(aCurPsVersion), aToken.Get(0), _countof(aCurPsVersion));
		}
	}
	if (_T('\0') == aCurPsVersion[0]) {
		return false;
	}

	_tcsncpy_s(m_CurPsVersion, _countof(m_CurPsVersion), aCurPsVersion, _countof(m_CurPsVersion));
	m_CurPsVersionNo = aCurPsVersionNo;

	return true;
}


bool CPsUpdater::DoCheck()
{

	DN_ASSERT(IsOpen(),				"Not Opened!");

//	DN_ASSERT(0 != m_CurPsVersion,	"Invalid!");

	// 버전 파일이 존재한다면 원격지 버전 경로에서 플랜 스냅샷 버전 체크
	TCHAR aNewPsVersion[DF_PSUD_VERSIONSIZE] = { _T('\0'), };
	UINT64 aNewPsVersionNo = 0;
	{
		CHttpClient aHttpClient;
		aHttpClient.SetOption(200, 5*1000);

		BYTE aBuffer[DF_PSUD_TMPBUFFSIZE + sizeof(TCHAR)] = { 0, };

		TCHAR aPsVersionFileUrl[MAX_PATH] = { _T('\0'), };
		DWORD aPsVersionFileUrlSize = _countof(aPsVersionFileUrl);

		HRESULT aRetVal = ::UrlCombine(m_PsVersionFileUrl, m_PsVersionFileName, aPsVersionFileUrl, &aPsVersionFileUrlSize, 0);
		if (FAILED(aRetVal)) {
			return false;
		}

		if (TRUE == aHttpClient.Open(CVarArg<MAX_PATH>(_T("%s"), aPsVersionFileUrl))) 
		{
			INT iRetVal = 1;
			while (0 < iRetVal) {
//				::memset(aBuffer, 0, sizeof(aBuffer));	// 추후에 ::InternetReadFile() 의 읽은 바이트 수 참조하도록 수정 필요

				iRetVal = aHttpClient.RecvResponse(aBuffer, DF_PSUD_TMPBUFFSIZE);
				if (0 <= iRetVal) {
					// iRetVal - 0:완료 / 1:더받을수있음
					(*reinterpret_cast<LPTSTR>(&aBuffer[DF_PSUD_TMPBUFFSIZE])) = _T('\0');
				}
			}
			if (iRetVal < 0) {
				// 오류 발생
				return false;
			}

			{
				WORD aYear, aBuildNo;
				BYTE aMonth, aDay;

				bool aLocalRetVal = ParseVersion(reinterpret_cast<LPCSTR>(aBuffer), aYear, aMonth, aDay, aBuildNo);
				if (!aLocalRetVal) {
					// 오류 발생
					return false;
				}

				aNewPsVersionNo = MAKELONGLONG(MAKELONG(aBuildNo, aDay), MAKELONG(aMonth, aYear));
			}

			// 이전 버전과 신규 버전 비교
			if (m_CurPsVersionNo == aNewPsVersionNo) {
				return false;
			}

			// 버전 문자열에 불필요한 문자들 제거
			{
				USES_CONVERSION;

				CStringSet aStringSet = A2CT(reinterpret_cast<LPCSTR>(aBuffer));

				INT aStartPtr = 0;
				CStringSet aToken = aStringSet.Tokenize(_T("\n\r"), aStartPtr);
				if (aToken.IsEmpty()) {
					// 오류 발생
					return false;
				}

				_tcsncpy_s(aNewPsVersion, _countof(aNewPsVersion), aToken.Get(0), _countof(aNewPsVersion));
			}
		}
		else {
			// 오류 발생
			return false;
		}
	}
	if (_T('\0') == aNewPsVersion[0]) {
		return false;
	}

	_tcsncpy_s(m_NewPsVersion, _countof(m_NewPsVersion), aNewPsVersion, _countof(m_NewPsVersion));
	m_NewPsVersionNo = aNewPsVersionNo;

	return true;
}


void CPsUpdater::DoUpdate(eNotifyType pNotifyType)
{

	DN_ASSERT(IsOpen(),				"Not Opened!");

// 	DN_ASSERT(0 != m_CurPsVersion,	"Invalid!");
// 	DN_ASSERT(0 != m_NewPsVersion,	"Invalid!");

	TCHAR aLocalPath[MAX_PATH] = { _T('\0'), };
	TCHAR aFileName[MAX_PATH] = { _T('\0'), };
	TCHAR aExecName[MAX_PATH] = { _T('\0'), };
	TCHAR aDrvName[MAX_PATH] = { _T('\0'), };
	TCHAR aDirName[MAX_PATH] = { _T('\0'), };

	// 사용자 알림 메시지
	switch(pNotifyType) {
	case eNotifyTypeUpdate:		// 업데이트
		{
			// P.S.> 프로세스 시작 시 기타 자원들 (*.DLL 등등) 로드 전이라면 업데이트 가능

			::MessageBox(
				HWND_DESKTOP, 
				CVarArg<MAX_PATH>(_T("다음과 같은 버전의 플랜 스냅샷이 발행되었습니다.\n\n버전 : %s\n\n'확인' 버튼을 누르시면 해당 플랜 스냅샷 업데이트가 시작되며 이후 리소스 폴더도 별도로 업데이트 해주시기 바랍니다."), m_NewPsVersion),
				_T("플랜 스냅샷 업데이트 안내"), 
				MB_ICONINFORMATION | MB_DEFBUTTON1
				);
		}
		break;
	case eNotifyTypeCheck:		// 버전체크
		{
			// P.S.> 프로세스 실행 중 기타 자원들 (*.DLL 등등) 로드 이후라면 재시작 해서 업데이트 받는 것이 안전

			::MessageBox(
				HWND_DESKTOP, 
				CVarArg<MAX_PATH>(_T("다음과 같은 버전의 플랜 스냅샷이 발행되었습니다.\n\n버전 : %s\n\n'확인' 버튼을 누르신 후 프로세스가 종료되면 재시작 해주시기 바랍니다."), m_NewPsVersion),
				_T("플랜 스냅샷 업데이트 안내"), 
				MB_ICONINFORMATION | MB_DEFBUTTON1
				);
		}
		return;
	default:
		return;
//		DN_RETURN_NONE;
	}

	// 플랜 스냅샷 버전이 서로 달라서 패치가 필요하다면 현재 실행파일 이름 변경
	{
		DWORD aRetVal1 = ::GetModuleFileName(NULL, aLocalPath, _countof(aLocalPath));
		if (!aRetVal1) {
			// 오류 발생
			return;
		}

		errno_t aRetVal2 = _tsplitpath_s(aLocalPath, aDrvName, _countof(aDrvName), aDirName, _countof(aDirName), aFileName, _countof(aFileName), aExecName, _countof(aExecName));
		if (aRetVal2) {
			// 오류 발생
			return;
		}

		TCHAR aSrcFile[MAX_PATH] = { _T('\0'), };
		TCHAR aTgtFile[MAX_PATH] = { _T('\0'), };

		_sntprintf_s(aSrcFile, _countof(aSrcFile), _T("%s%s%s%s"), aDrvName, aDirName, aFileName, aExecName);
		_sntprintf_s(aTgtFile, _countof(aTgtFile), _T("%s%s%s_%s%s"), aDrvName, aDirName, DF_PSUD_DELFILEPREP, aFileName, aExecName);

		CFileSet::Remove(aTgtFile);
		CFileSet::Rename(aSrcFile, aTgtFile);
	}

	// 네트워크 상의 플랜 스냅삿 호스트에 대한 접근 권한 획득
	{
		NETRESOURCE	stNetResource;

		stNetResource.dwScope		= RESOURCE_GLOBALNET;
		stNetResource.dwType		= RESOURCETYPE_ANY;
		stNetResource.dwDisplayType	= RESOURCEDISPLAYTYPE_DOMAIN;
		stNetResource.dwUsage		= RESOURCEUSAGE_CONNECTABLE;
		stNetResource.lpLocalName	= 0;

		stNetResource.lpRemoteName	= m_PsDownloadRootUrl;
		stNetResource.lpComment		= 0;
		stNetResource.lpProvider	= 0;

		DWORD aRetVal = ::WNetAddConnection2(&stNetResource, m_PsDownloadPassword, m_PsDownloadAccountId, CONNECT_UPDATE_PROFILE);

		if (NO_ERROR != aRetVal) {
			// 오류 발생
//			return;
		}
	}

/*
	// 플랜 스냅샷의 파일들을 'xcopy' 명령을 이용하여 로컬 폴더로 전체 복사
	{
		TCHAR aSrcPath[MAX_PATH] = { _T('\0'), };
		TCHAR aTgtPath[MAX_PATH] = { _T('\0'), };

		_sntprintf_s(aSrcPath, _countof(aSrcPath), _T("%s\\%s"), DF_PSUD_SRCHOSTPATH, m_NewPsVersion);
		_sntprintf_s(aTgtPath, _countof(aTgtPath), _T("%s%s"), aDrvName, aDirName);

		{
			TCHAR aExecPath[MAX_PATH] = { _T('\0'), };

			UINT aRetVal1 = ::GetSystemDirectory(aExecPath, _countof(aExecPath));
			if (!aRetVal1) {
				// 오류 발생
				return;
			}

			_sntprintf_s(aExecPath, _countof(aExecPath), _T("%s\\xcopy.exe \"%s\\*.*\" \"%s\" /Y /E /H /C /K /R"), aExecPath, aSrcPath, aTgtPath);
//			_sntprintf_s(aExecPath, _countof(aExecPath), _T("%s\\xcopy.exe \"%s\\*.*\" \"%s\" /Y /E /H /C /K /Q /R"), aExecPath, aSrcPath, aTgtPath);

			STARTUPINFO aStartupInfo;
			PROCESS_INFORMATION aProcessInformation;

			::memset(&aStartupInfo, 0, sizeof(aStartupInfo));
			aStartupInfo.cb = sizeof(aStartupInfo);
			::memset(&aProcessInformation, 0, sizeof(aProcessInformation));

			BOOL aRetVal = ::CreateProcess(NULL, aExecPath, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &aStartupInfo, &aProcessInformation);
//			BOOL aRetVal = ::CreateProcess(NULL, aExecPath, NULL, NULL, FALSE, 0, NULL, NULL, &aStartupInfo, &aProcessInformation);
			if (!aRetVal) {
				// 오류 발생
				return;
			}

			::WaitForSingleObject(aProcessInformation.hProcess, INFINITE);

			::CloseHandle(aProcessInformation.hProcess);
			::CloseHandle(aProcessInformation.hThread);
		}
	}
*/

	TP_CONSOLEOUTPUT aConsoleOutput(new(std::nothrow) CConsoleOutput());
	{
		BOOL aRetVal = static_cast<CConsoleOutput*>(aConsoleOutput.get())->Initialize(NULL);
		if (!aRetVal) {
			// 오류 발생
			
		}
	}

	// 플랜 스냅샷의 파일 목록을 확인하여 로컬 폴더로 복사
	TP_COPYFAILLIST aCopyFailList;
	{

		TCHAR aPsDownloadFullUrl[MAX_PATH] = { _T('\0'), };
		::PathCombine(aPsDownloadFullUrl, m_PsDownloadPathUrl, m_NewPsVersion);

		DWORD aRetVal = CopyNewFile(aCopyFailList, aConsoleOutput, CVarArg<MAX_PATH>(_T("%s"), aPsDownloadFullUrl), NULL);

		if (NOERROR != aRetVal) {
			// 오류 발생
			
		}
	}
	if (!aCopyFailList.empty()) {
		// 오류 발생

		_tstring aMsg = _T("플랜 스냅샷 업데이트 중 아래의 파일(들)의 복사에 실패하였습니다.\n\n'확인' 버튼을 누르시면 해당 버전의 플랜 스냅샷 폴더가 열리니 수동으로 복사하신 후 리소스 폴더도 별도로 업데이트 해주시기 바랍니다.\n\n");
		TP_COPYFAILLIST_CTR aIt = aCopyFailList.begin();
		for (; aCopyFailList.end() != aIt ; ++aIt) {
			aMsg += (_T("[") + (*(aIt)) + _T("]\n"));
		}

		::MessageBox(
			HWND_DESKTOP, 
			aMsg.c_str(), 
			_T("플랜 스냅샷 업데이트 실패"), 
			MB_ICONWARNING | MB_DEFBUTTON1
			);

		// 플랜 스냅샷 폴더 개방
		{
			TCHAR aEnvVar[32767] = { _T('\0'), };
			GetEnvironmentVariable(_T("ProgramFiles"), aEnvVar, _countof(aEnvVar));

			TCHAR aIEPath[MAX_PATH] = { _T('\0'), };
			{
				long lRet;
				HKEY hKey;

				lRet = ::RegOpenKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IExplore.exe"), &hKey);
				if (lRet == ERROR_SUCCESS)
				{
					long cbData = _countof(aIEPath);

					::RegQueryValue(hKey, _T(""), aIEPath, &cbData);
					::RegCloseKey(hKey);
				}
				else {
					// 오류 발생
					return;
				}
			}

			TCHAR aPsDownloadFullUrl[MAX_PATH] = { _T('\0'), };

			::PathCombine(aPsDownloadFullUrl, m_PsDownloadPathUrl, m_NewPsVersion);

			HINSTANCE aRetVal = ::ShellExecute(HWND_DESKTOP, _T("open"), aIEPath, aPsDownloadFullUrl, NULL, SW_SHOWNORMAL);
			if (32 >= PtrToInt(aRetVal)) {
				// 오류 발생
				return;
			}
		}
	}

	// 네트워크 상의 플랜 스냅삿 호스트에 대한 접근 권한 해제
	{

		DWORD aRetVal = ::WNetCancelConnection2(m_PsDownloadRootUrl, CONNECT_UPDATE_PROFILE, TRUE);

		if (NO_ERROR != aRetVal) {
			// 오류 발생
//			return;
		}
	}

	_tcsncpy_s(m_CurPsVersion, _countof(m_CurPsVersion), m_NewPsVersion, _countof(m_CurPsVersion));
	m_CurPsVersionNo = m_NewPsVersionNo;
}


bool CPsUpdater::ParseVersion(LPCSTR pPsVersion, WORD& pYear, BYTE& pMonth, BYTE& pDay, WORD& pBuildNo)
{

	DN_ASSERT(IsOpen(),	"Not Opened!");


	USES_CONVERSION;

	CStringSet aStringSet = A2CT(pPsVersion);
	if (aStringSet.IsEmpty()) {
		return false;
	}

	INT aStartPtr = 0;
	CStringSet aToken = aStringSet.Tokenize(_T(".\n\r"), aStartPtr);
	if (aToken.IsEmpty() || !aToken.IsNumeric()) {
		return false;
	}
	pYear = _ttoi(aToken.Get(0));

	aToken = aStringSet.Tokenize(_T(".\n\r"), aStartPtr);
	if (aToken.IsEmpty() || !aToken.IsNumeric()) {
		return false;
	}
	pMonth = _ttoi(aToken.Get(0));

	aToken = aStringSet.Tokenize(_T(".\n\r"), aStartPtr);
	if (aToken.IsEmpty() || !aToken.IsNumeric()) {
		return false;
	}
	pDay = _ttoi(aToken.Get(0));

	aToken = aStringSet.Tokenize(_T(".\n\r"), aStartPtr);
	if (aToken.IsEmpty() || !aToken.IsNumeric()) {
		return false;
	}
	pBuildNo = _ttoi(aToken.Get(0));
	
	return true;
}


DWORD CPsUpdater::DeleteOldFile(LPCTSTR pLocalPath)
{

	TCHAR aLocalPath[MAX_PATH] = { _T('\0'), };
	TCHAR aDrvName[MAX_PATH] = { _T('\0'), };
	TCHAR aDirName[MAX_PATH] = { _T('\0'), };

	if (!pLocalPath) {
		DWORD aRetVal1 = ::GetModuleFileName(NULL, aLocalPath, _countof(aLocalPath));
		if (!aRetVal1) {
			// 오류 발생
			return(HASERROR+0);
		}

		errno_t aRetVal2 = _tsplitpath_s(aLocalPath, aDrvName, _countof(aDrvName), aDirName, _countof(aDirName), NULL, 0, NULL, 0);
		if (aRetVal2) {
			// 오류 발생
			return(HASERROR+0);
		}

		_sntprintf_s(aLocalPath, _countof(aLocalPath), _T("%s%s"), aDrvName, aDirName);
	}
	else {
		_tcsncpy_s(aLocalPath, _countof(aLocalPath), pLocalPath, _countof(aLocalPath));
	}
	::PathAddBackslash(aLocalPath);

	{
		CDirectory aDirectory;
		DWORD aRetVal1 = aDirectory.Begin(CVarArg<MAX_PATH>(_T("%s*.*"), aLocalPath));
//		DWORD aRetVal1 = aDirectory.Begin(CVarArg<MAX_PATH>(_T("%s%s*.*"), aLocalPath, DF_PSUD_DELFILEPREP));
		if (NOERROR != aRetVal1) {
			// 오류 발생
			return(aRetVal1);
		}

		while(NOERROR == aRetVal1) {
			if (aDirectory.IsDirectory()) {
				// 디렉토리

				if (_T('.') != aDirectory.GetFindData().cFileName[0]) {
					DWORD aRetVal2 = DeleteOldFile(CVarArg<MAX_PATH>(_T("%s%s"), aLocalPath, aDirectory.GetFindData().cFileName));
					if (NOERROR != aRetVal2) {
//						return aRetVal2;	// P.S.> 삭제 시 오류가 발생한 파일은 넘어가고 지울 수 있는 만큼 최대한 지움
					}
				}
			}
			else {
				// 파일

				if (_tcsstr(aDirectory.GetFindData().cFileName, DF_PSUD_DELFILEPREP)) {
					CFileSet::Remove(CVarArg<MAX_PATH>(_T("%s%s"), aLocalPath, aDirectory.GetFindData().cFileName));
				}
			}

			aRetVal1 = aDirectory.Next();
			if (NOERROR != aRetVal1 && ERROR_NO_MORE_FILES != aRetVal1) {
				// 오류 발생
				aDirectory.End();
				return(aRetVal1);
			}
		}

		aDirectory.End();
	}

	return NOERROR;
}


DWORD CPsUpdater::CopyNewFile(TP_COPYFAILLIST& pCopyFailList, TP_CONSOLEOUTPUT& pConsoleOutput, LPCTSTR pRemotePath, LPCTSTR pLocalPath)
{

	DN_ASSERT(IsOpen(),				"Not Opened!");
	DN_ASSERT(NULL != pRemotePath,	"Invalid!");

	TCHAR aRemotePath[MAX_PATH] = { _T('\0'), };
	TCHAR aLocalPath[MAX_PATH] = { _T('\0'), };
	TCHAR aDrvName[MAX_PATH] = { _T('\0'), };
	TCHAR aDirName[MAX_PATH] = { _T('\0'), };

	_tcsncpy_s(aRemotePath, _countof(aRemotePath), pRemotePath, _countof(aRemotePath));
	::PathAddBackslash(aRemotePath);

	if (!pLocalPath) {
		DWORD aRetVal1 = ::GetModuleFileName(NULL, aLocalPath, _countof(aLocalPath));
		if (!aRetVal1) {
			// 오류 발생
			return(HASERROR+0);
		}

		errno_t aRetVal2 = _tsplitpath_s(aLocalPath, aDrvName, _countof(aDrvName), aDirName, _countof(aDirName), NULL, 0, NULL, 0);
		if (aRetVal2) {
			// 오류 발생
			return(HASERROR+0);
		}

		_sntprintf_s(aLocalPath, _countof(aLocalPath), _T("%s%s"), aDrvName, aDirName);
	}
	else {
		_tcsncpy_s(aLocalPath, _countof(aLocalPath), pLocalPath, _countof(aLocalPath));
	}
	::PathAddBackslash(aLocalPath);

	// 원격지 파일 목록 순회
	{
		CDirectory aDirectory;
		DWORD aRetVal1 = aDirectory.Begin(CVarArg<MAX_PATH>(_T("%s*.*"), aRemotePath));
		if (NOERROR != aRetVal1) {
			// 오류 발생
			return(aRetVal1);
		}

		while(NOERROR == aRetVal1) {
			if (aDirectory.IsDirectory()) {
				// 디렉토리

				if (_T('.') != aDirectory.GetFindData().cFileName[0]) {
					DWORD aRetVal2 = CopyNewFile(pCopyFailList, pConsoleOutput, CVarArg<MAX_PATH>(_T("%s%s"), aRemotePath, aDirectory.GetFindData().cFileName), CVarArg<MAX_PATH>(_T("%s%s"), aLocalPath, aDirectory.GetFindData().cFileName));
					if (NOERROR != aRetVal2) {
//						return aRetVal2;
					}
				}
			}
			else {
				// 파일

				BOOL aRetVal2 = TRUE;

				// 로컬 파일 확인 및 이름 변경 (DF_PSUD_VERSIONFILE 제외)
				if (!_tcsicmp(aDirectory.GetFindData().cFileName, m_PsVersionFileName)) {
					aRetVal2 = FALSE;
				}

				if (aRetVal2 &&
					!_taccess_s(CVarArg<MAX_PATH>(_T("%s%s"), aLocalPath, aDirectory.GetFindData().cFileName), 0))
				{
					// 파일 있음

					aRetVal2 = CFileSet::Rename(CVarArg<MAX_PATH>(_T("%s%s"), aLocalPath, aDirectory.GetFindData().cFileName), CVarArg<MAX_PATH>(_T("%s%s_%s"), aLocalPath, DF_PSUD_DELFILEPREP, aDirectory.GetFindData().cFileName));
					if (!aRetVal2) {
//						return(HASERROR+0);
					}
				}

				// 원격지 파일 복사
				{
					aRetVal2 = CFileSet::Copy(CVarArg<MAX_PATH>(_T("%s%s"), aRemotePath, aDirectory.GetFindData().cFileName), CVarArg<MAX_PATH>(_T("%s%s"), aLocalPath, aDirectory.GetFindData().cFileName), FALSE);
					if (!aRetVal2) {
						pCopyFailList.push_back(aDirectory.GetFindData().cFileName);
//						return(HASERROR+0);		// P.S.> 복사 시 오류가 발생하면 사용자에게 해당 사실을 알리고 수동 복사할 수 있도록 지원해야 함 !!!

						static_cast<CConsoleOutput*>(pConsoleOutput.get())->Write(CVarArg<MAX_PATH>(_T("Copy (Fail) : %s%s\n"), aLocalPath, aDirectory.GetFindData().cFileName));
					}
					else {
						static_cast<CConsoleOutput*>(pConsoleOutput.get())->Write(CVarArg<MAX_PATH>(_T("Copy (Succeed) : %s%s\n"), aLocalPath, aDirectory.GetFindData().cFileName));
					}
				}
			}

			aRetVal1 = aDirectory.Next();
			if (NOERROR != aRetVal1 && ERROR_NO_MORE_FILES != aRetVal1) {
				// 오류 발생
				aDirectory.End();
				return(aRetVal1);
			}
		}

		aDirectory.End();
	}

	return NOERROR;
}

void CPsUpdater::AsyncUpdate() 
{

	// 기존파일 삭제
	this->DeleteOldFile();


	// 업데이팅 여부 확인
	if (m_IsUpdate) 
		return;
	
	if (!Open()) {
#if !defined(_WORK)
		g_Log.Log(LogType::_ERROR, L"업데이트 준비에 실패했습니다.\n");
#endif
		return;
	}

	if (!LoadVersion()) {
		g_Log.Log(LogType::_ERROR, L"업데이트중 버젼읽기에 실패하였습니다. \n");
		return;
	}

	// 인터넷 관련함수 호출(비동기)
	ExecInternetFunc();

}


void CPsUpdater::ExecInternetFunc()
{
	// 해당 Url 만들기
	TCHAR aPsVersionFileUrl[MAX_PATH] = { _T('\0'), };
	DWORD aPsVersionFileUrlSize = _countof(aPsVersionFileUrl);
	HRESULT aRetVal = ::UrlCombine(m_PsVersionFileUrl, m_PsVersionFileName, aPsVersionFileUrl, &aPsVersionFileUrlSize, 0);

	if (FAILED(aRetVal)) {
		g_Log.Log(LogType::_ERROR, L"업데이트 URL 생성 실패\n");
		return;
	}

	m_hInternet = ::InternetOpen(_T("Microsoft Internet Explorer"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, INTERNET_FLAG_ASYNC);
	if (!m_hInternet)
		return;

	// 비동기 호출을 위한 콜백함수 설정
	::InternetSetStatusCallback(m_hInternet,  (INTERNET_STATUS_CALLBACK)&Internet_Status_Callback);

	// Url 오픈
	m_hInternetConnection = InternetOpenUrl (m_hInternet, 
		aPsVersionFileUrl, 
		NULL,
		0, 
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 
		(DWORD_PTR) this);
	if( m_hInternetConnection == NULL )
	{
		DWORD Error=GetLastError();
		if( Error != WSA_IO_PENDING )
		{
			InternetCloseHandle(m_hInternet);	
			m_hInternet = NULL;
		}		
	}
}

void __stdcall Internet_Status_Callback(HINTERNET hSession, DWORD Context, DWORD Status, LPVOID pInformation, DWORD InfoLength)
{
	
	static HINTERNET hInternetConnection;	
	static DWORD	 dwNumberOfBytesAvailable = 0;

	switch (Status) 
	{
		case INTERNET_STATUS_RESPONSE_RECEIVED:
			dwNumberOfBytesAvailable = (*(DWORD *)pInformation);
		break;

		case INTERNET_STATUS_HANDLE_CREATED:
			hInternetConnection = * (HINTERNET *) pInformation;
		break;

		case INTERNET_STATUS_REQUEST_COMPLETE:
		{

			 LPBYTE pVersionBuff = new BYTE[dwNumberOfBytesAvailable];
			 ZeroMemory(pVersionBuff, dwNumberOfBytesAvailable);

			 DWORD dwBytesRead(0);
			 BOOL bRef(TRUE);
			 while (bRef)
			 {
				 bRef = InternetReadFile (hInternetConnection, pVersionBuff, dwNumberOfBytesAvailable, &dwBytesRead);
				 if (!dwBytesRead)
					 bRef = FALSE;
			 }
			
			 g_PsUpdater->UpdateProcess(pVersionBuff);

			 if (g_PsUpdater)
			 {
				g_PsUpdater->Close();
				SAFE_DELETE(g_PsUpdater);
			 }

			 delete [] pVersionBuff;
			 
		}
		break;

	default:
		break;
	} 
}

void CPsUpdater::UpdateProcess(LPBYTE pVersionBuff)
{

	TCHAR aNewPsVersion[DF_PSUD_VERSIONSIZE] = { _T('\0'), };
	UINT64 aNewPsVersionNo = 0;

	// 버전데이터 파싱
	{
		WORD aYear, aBuildNo;
		BYTE aMonth, aDay;

		bool aLocalRetVal = ParseVersion(reinterpret_cast<LPCSTR>(pVersionBuff), aYear, aMonth, aDay, aBuildNo);
		if (!aLocalRetVal) {
			// 오류 발생
			return;
		}

		aNewPsVersionNo = MAKELONGLONG(MAKELONG(aBuildNo, aDay), MAKELONG(aMonth, aYear));
	}

	// 버전 문자열에 불필요한 문자들 제거
	{
		USES_CONVERSION;

		CStringSet aStringSet = A2CT(reinterpret_cast<LPCSTR>(pVersionBuff));

		INT aStartPtr = 0;
		CStringSet aToken = aStringSet.Tokenize(_T("\n\r"), aStartPtr);
		if (aToken.IsEmpty()) {
			// 오류 발생
			return;
		}

		_tcsncpy_s(aNewPsVersion, _countof(aNewPsVersion), aToken.Get(0), _countof(aNewPsVersion));

	}

	// 이전 버전과 신규 버전 비교
	if (m_CurPsVersionNo == aNewPsVersionNo) {
		return;
	}

	if (_T('\0') == aNewPsVersion[0]) {
		return;
	}

	_tcsncpy_s(m_NewPsVersion, _countof(m_NewPsVersion), aNewPsVersion, _countof(m_NewPsVersion));
	m_NewPsVersionNo = aNewPsVersionNo;

	g_Log.Log(LogType::_NORMAL, L"업데이트 준비중입니다\n");

	if (!g_PsUpdater)
	{
		g_Log.Log(LogType::_ERROR, L"업데이트가 원활하지 않습니다.\n");
		return;
	}

	// 버젼 체크 완료후 업데이트한다.
	m_IsUpdate = true;

	g_Log.Log(LogType::_NORMAL, L"업데이트를 시작합니다\n");

	DoUpdate(CPsUpdater::eNotifyTypeUpdate);
	g_PsUpdater->Close();
	SAFE_DELETE(g_PsUpdater);
	::ExitProcess(0);
}
