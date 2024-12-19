#pragma once

#include "EtFileSystem.h"
#include "HTTPClient.h"

// DnLauncher Download Patch Thread

#ifdef _USE_RTPATCH
#define  MAX_MESSAGES 10
#endif // _USE_RTPATCH

// CThread Class
class CThread
{
public:
	CThread();
	virtual ~CThread();

	BOOL Start();
	BOOL Terminate( DWORD nExitCode );
	void SetThreadName( DWORD dwThreadID, const char* szThreadName );
	BOOL WaitForTerminate( DWORD nTimeout = INFINITE );
	HANDLE const GetThreadHandle() { return m_hHandleThread; }

protected:
	virtual	void Run() = 0;
	HANDLE	m_hHandleThread;
	DWORD	m_nThreadID;
	BOOL	m_bThreadLoop;

	typedef struct tagTHREADNAME_INFO 
	{ 
		DWORD dwType;		// must be 0x1000 
		LPCSTR szName;		// pointer to name (in user addr space) 
		DWORD dwThreadID;	// thread ID (-1=caller thread) 
		DWORD dwFlags;		// reserved for future use, must be zero 
	} THREADNAME_INFO; 

private:
	static DWORD WINAPI _Runner( LPVOID pParam );
};

// CDnPatchDownloadThread Class
enum PatchReturn
{
	PR_FAIL = 0,
	PR_OK,
	PR_LAUNCHER_PATCH,
	PR_FULLPATCH,
	PR_TERMINATE,
};

enum FirstPatchReturn
{
	FPR_FAIL = 0 ,
	FPR_OK ,
	FPR_LAUNCHER_PATCH,
	FPR_NEED_FULLPATCH,

	FPR_MAX
};

#define WM_STATUS_MSG							WM_APP+101	// 패치 상태 메세지

enum DownloadPatchState
{
	PATCH_NONE = 0,
	PATCH_CHECK_VERSION,			// 버전 체크
	PATCH_CHECK_VERSION_FAILED,		// 버전 체크 실패
	PATCH_DOWNLOAD_DATA,			// 패치 데이터 다운로드
	PATCH_DOWNLOAD_DATA_FAILED,		// 패치 데이터 다운로드 실패
	PATCH_MD5_CHECK,				// 패치 데이터 유효성 검사
	PATCH_MD5_CHECK_FAILED,			// 패치 데이터 유효성 검사 실패
	PATCH_APPLY_PAK_FILE,			// 패치 데이터 설치
	PATCH_APPLY_PAK_FILE_FAILED,	// 패치 데이터 설치 실패
	PATCH_APPLY_PAK_FILE_END,		// 패치 데이터 설치 완료
	PATCH_COMPLETE,					// 패치 완료
	PATCH_LAUNCHER,					// 런처 패치
	PATCH_FAILED,					// 패치 실패
	PATCH_TERMINATE					// 패치 취소
};

struct stDownloadInfoUpdateData
{
	CString m_strFileName;
	__int64	m_nFileSize;
	__int64	m_nDownloadProgressMin;
	__int64	m_nDownloadProgressMax;
	int		m_nCurrentFileCount;
	int		m_nTotalFileCount;
	float	m_fDownloadRate;

	stDownloadInfoUpdateData() : m_nFileSize( 0 ), m_nDownloadProgressMin( 0 ), m_nDownloadProgressMax( 0 ), 
		m_nCurrentFileCount( 0 ), m_nTotalFileCount( 0 ), m_fDownloadRate( 0.0f ) {}
};

enum FileCheck{ FILE_FALSE = 0, FIlE_OK, FIlE_EXIST };

#ifdef _USE_SPLIT_COMPRESS_FILE
struct RARHeaderData
{
	char         ArcName[260];
	char         FileName[260];
	unsigned int Flags;
	unsigned int PackSize;
	unsigned int UnpSize;
	unsigned int HostOS;
	unsigned int FileCRC;
	unsigned int FileTime;
	unsigned int UnpVer;
	unsigned int Method;
	unsigned int FileAttr;
	char         *CmtBuf;
	unsigned int CmtBufSize;
	unsigned int CmtSize;
	unsigned int CmtState;
};

struct RAROpenArchiveDataEx
{
	char         *ArcName;
	wchar_t      *ArcNameW;
	unsigned int OpenMode;
	unsigned int OpenResult;
	char         *CmtBuf;
	unsigned int CmtBufSize;
	unsigned int CmtSize;
	unsigned int CmtState;
	unsigned int Flags;
	unsigned int Reserved[32];
};
#endif // _USE_SPLIT_COMPRESS_FILE



class CDnFistPatchDownloadThread : public CPatchCallback, public CThread
{
public:
	CDnFistPatchDownloadThread();
	virtual ~CDnFistPatchDownloadThread();
	
	virtual void Run();
	void		SetWindowHandle( HWND hWnd ){ m_hWnd = hWnd; }
	void		TerminateThread()			{ m_bTerminateThread = true; }
	BOOL		IsThreadEnd()				{ return m_bTerminateThread; }

	FirstPatchReturn BegineModulePatch();									// FirstPatch( 모듈 & 런처 파일) 시작.
	int			ParsingModuleList();										// 모듈 패치에 필요한 정보 파싱.
	int			ChangeModuleFiles(int nVer);								// 모듈 파일 적용.
	BOOL		DownLoadModulePatch( int nVersion );						// 모듈파일 다운로드.
	BOOL		PatchLauncherFiles( int nVersion );							// 런처 다운로드 & 적용.
	BOOL		ProcessFullVersionPatch();									// 풀 버전 패치 프로세스
	void		ProcessRebootLauncher();									// 런처 다시 실행.
	BOOL		SaveModuleVersionToPak( int nVersion );						// Pak파일에 Version.cfg 저장.
	BOOL		SaveModuleVersionFile( CString& szFilePath, int nVersion ); // Version.cfg 저장.
	BOOL		SaveModuleVersionFileWithOutPakSave( CString& szFilePath, int nVersion );
	void		CloseThread();

private:
	HWND		m_hWnd;								// 윈도우 핸들
	BOOL		m_bTerminateThread;					// 쓰레드 종료 플래그
	CString		m_strFirstPatchURLPath;				// PatchURL/FirstPatch/ 경로 저장.
	CString		m_strPatchListFile;					// 클라이언트 패치 경로
	std::vector<std::string>	m_vecCopyList;		// 카피 파일 리스트
	std::vector<std::string>	m_vecDeleteList;	// 삭제 파일 리스트
};



class CDnPatchDownloadThread : public CPatchCallback, public CThread
{
public:
	CDnPatchDownloadThread();
	virtual ~CDnPatchDownloadThread();
	void SetWindowHandle( HWND hWnd ) { m_hWnd = hWnd; }

	virtual void Run();
	void TerminateThread() { m_bTerminateThread = true; }

private:
	PatchReturn BeginPatch();				// 패치 진행 함수

	// Download
	BOOL DownLoadPatch( int nVersion );						// 다운로드		 프로세스

#ifdef _USE_SPLIT_COMPRESS_FILE
	HRESULT URLDownload( LPCTSTR strPatchFileUrl, LPCTSTR strFilePath, LPCTSTR strFileName, 
						BOOL bSplitDownload = FALSE, DWORD dwAlreadyDownloadSize = 0, DWORD dwRemainDownloadFileSize = 0 );		// 실제 이어받기 다운로드 프로세스
	FileCheck CheckPatchFile( HINTERNET hInternetSession, LPCTSTR strFileName,
							CFile& file, DWORD& dwDownloadFileSize, BOOL bSplitDownload = FALSE );								// 다운로드 전에 파일 체크
#else // _USE_SPLIT_COMPRESS_FILE
	HRESULT URLDownload( LPCTSTR strPatchFileUrl, LPCTSTR strFilePath, LPCTSTR strFileName );	// 실제 이어받기 다운로드 프로세스
	FileCheck CheckPatchFile( HINTERNET hInternetSession, LPCTSTR strFileName,
							CFile& file, DWORD& dwDownloadFileSize );							// 다운로드 전에 파일 체크
#endif // _USE_SPLIT_COMPRESS_FILE

	DWORD GetDownloadFileSize( HINTERNET hInternetSession, CString strHttpPath );				// 다운받을 파일의 사이즈 구해오는 함수
	BOOL GetMD5Checksum();																		// 다운받은 파일의 유효성 검사 (MD5)

	// Apply Packing File
	PatchReturn PreApplyPakPatch();									// Pak파일 적용 이전 준비작업 (패치리스트 셋팅, 패킹파일 얻어오기)
	PatchReturn ApplyPakPatch();									// Pak파일 적용 프로세스
	PatchReturn ApplyDeleteList( CEtPackingFile& PatchSource );		// 삭제 리스트 Pak파일 적용
	PatchReturn ApplyPatchList( CEtPackingFile& PatchSource );		// 패치 리스트 Pak파일 적용
	PatchReturn ApplyCopyList( CEtPackingFile& PatchSource );		// 복사 리스트 Pak파일 적용
	PatchReturn GetPackingFile();									// 리소스 패킹 파일을 찾고 정보를 가져옴
	BOOL LoadPatchList();											// 패치 리스트를 읽어 패치 정보를 셋팅
	PatchReturn PatchFileExist( int nPatchFileCount, CEtPackingFile& PatchSource, SPackingFileInfo* pFileInfo, BOOL& bPatch );
	PatchReturn PatchFileSuitableSpace( CEtPackingFile& PatchSource, SPackingFileInfo* pFileInfo, BOOL& bPatch );
	void PatchFileReserve( int nPatchFileCount, SPackingFileInfo* pFileInfo );

	void SendStatusMsg( DownloadPatchState nID );				// 다이얼로그로 다운로드상태 값 전달

	BOOL ProcessFullVersionPatch();								// 풀 버전 패치 프로세스
	BOOL SaveVersionFile( CString& szFilePath, int nVersion );	// 패치 완료 후 Version.cfg 파일 갱신
	BOOL SaveNewVersionToPak( int nVersion );					// Version.cfg파일 Pak파일 안에 넣기

	void RecordFileExceptionLog( CFileException& e );

#ifdef _USE_RTPATCH
	BOOL ExportFile( CEtPackingFile& PakFile, const char* pExportFileName, const char* pCreateFilePath );	// pak파일내 파일 추출 하기
	PatchReturn ApplyRTPatch( CEtPackingFile& PatchSource, CEtPackingFile& RTPatchSource );					// 기존 pak파일을 RTPatch적용된 pak파일로 만든다
	static LPVOID CALLBACK EXPORT RTPatchCALLBACK( UINT Id, LPVOID Param );
#endif // _USE_RTPATCH
#ifdef _FIRST_PATCH
	BOOL CheckExistFirstPatch( int nVersion );
#endif // _FIRST_PATCH

#ifdef _USE_SPLIT_COMPRESS_FILE
	HRESULT DownLoadSplitFile( CString strUrl, int nVersion );
	DWORD GetSplitFileSizeList( CString strUrl, int nVersion, int nSplitCount, int nSplitCountCipher, std::vector<DWORD>& vecSplitFileSizeList );
	int LoadSplitCount( CString& szFilePath );
	HRESULT UnRARArchive( CString strPath, CString strRARFile );
#endif // _USE_SPLIT_COMPRESS_FILE

private:
	HINTERNET	m_hHttpConnection;		// Http Connection Handle

	DWORD		m_dwTempFileLength;		// 다운 받은 파일의 양
	DWORD		m_dwDownloadFileLength;	// 다운 받아야할 파일의 양
	DWORD		m_dwTotalFileLength;	// 총 파일의 양
	DWORD		m_dwTotalBytes;			// 총 받은 파일의 양

	CString		m_strPatchFileUrl;		// Http 패치 파일 주소
	CString		m_strPatchFileLocal;	// 로컬의 파일 위치 (다운로드 완료시 .tmp -> .pak)
	CString		m_strPatchFile;			// 패치 파일 (pak파일)
	CString		m_strPatchListFile;		// 패치 리스트 파일

	float		m_fRate;				// Download Rate
	float		m_fPercent;				// Download Percent

	DownloadPatchState m_emDownPatchState;	// 현재 다운로드 진행 상태
	
	HWND		m_hWnd;					// 윈도우 핸들
	BOOL		m_bTerminateThread;		// 쓰레드 종료 플래그

	std::vector<CEtPackingFile*>	m_vecPackingFile;		// 패킹 파일 리스트
	std::vector<std::string>		m_vecDeleteList;		// 삭제 파일 리스트
	std::vector<std::string>		m_vecPatchList;			// 패치 파일 리스트
	std::vector<std::string>		m_vecCopyList;			// 복사 파일 리스트
	struct stFileIndexSet
	{
		DWORD m_dwPakFileNum;
		DWORD m_dwPatchFileIndex;
		DWORD m_dwReserveFileIndex;
	};

	std::vector<stFileIndexSet>		m_vecReserveIndex;		// 예약된 패치 파일 인덱스


	int			m_nCurPatchCount;		// 현재 패치수 (파일 패킹 시 프로그래스바 정보에 사용)
	int			m_nTotalPatchCount;		// 총 패치수 (파일 패킹 시 프로그래스바 정보에 사용)

#ifdef _USE_RTPATCH
	std::string	m_strRTPatchTempFolder;		// RTPatch 임시 폴더
#endif // _USE_RTPATCH

#ifdef _USE_SPLIT_COMPRESS_FILE
	HINSTANCE	hUnrarDLL;					// UnrarDLL 헨들

	HANDLE (WINAPI *OpenArchiveEx)( RAROpenArchiveDataEx* pArchiveData );
	int    (WINAPI *CloseArchive)( HANDLE hArcData );
	int    (WINAPI *ReadRARHeader)( HANDLE hArcData, RARHeaderData *pHeaderData );
	int    (WINAPI *ProcessRARFile)( HANDLE hArcData, int iOperation, char* strDestFolder, char* strDestName );
#endif // _USE_SPLIT_COMPRESS_FILE
};
