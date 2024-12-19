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

#define WM_STATUS_MSG							WM_APP+101	// ��ġ ���� �޼���

enum DownloadPatchState
{
	PATCH_NONE = 0,
	PATCH_CHECK_VERSION,			// ���� üũ
	PATCH_CHECK_VERSION_FAILED,		// ���� üũ ����
	PATCH_DOWNLOAD_DATA,			// ��ġ ������ �ٿ�ε�
	PATCH_DOWNLOAD_DATA_FAILED,		// ��ġ ������ �ٿ�ε� ����
	PATCH_MD5_CHECK,				// ��ġ ������ ��ȿ�� �˻�
	PATCH_MD5_CHECK_FAILED,			// ��ġ ������ ��ȿ�� �˻� ����
	PATCH_APPLY_PAK_FILE,			// ��ġ ������ ��ġ
	PATCH_APPLY_PAK_FILE_FAILED,	// ��ġ ������ ��ġ ����
	PATCH_APPLY_PAK_FILE_END,		// ��ġ ������ ��ġ �Ϸ�
	PATCH_COMPLETE,					// ��ġ �Ϸ�
	PATCH_LAUNCHER,					// ��ó ��ġ
	PATCH_FAILED,					// ��ġ ����
	PATCH_TERMINATE					// ��ġ ���
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

	FirstPatchReturn BegineModulePatch();									// FirstPatch( ��� & ��ó ����) ����.
	int			ParsingModuleList();										// ��� ��ġ�� �ʿ��� ���� �Ľ�.
	int			ChangeModuleFiles(int nVer);								// ��� ���� ����.
	BOOL		DownLoadModulePatch( int nVersion );						// ������� �ٿ�ε�.
	BOOL		PatchLauncherFiles( int nVersion );							// ��ó �ٿ�ε� & ����.
	BOOL		ProcessFullVersionPatch();									// Ǯ ���� ��ġ ���μ���
	void		ProcessRebootLauncher();									// ��ó �ٽ� ����.
	BOOL		SaveModuleVersionToPak( int nVersion );						// Pak���Ͽ� Version.cfg ����.
	BOOL		SaveModuleVersionFile( CString& szFilePath, int nVersion ); // Version.cfg ����.
	BOOL		SaveModuleVersionFileWithOutPakSave( CString& szFilePath, int nVersion );
	void		CloseThread();

private:
	HWND		m_hWnd;								// ������ �ڵ�
	BOOL		m_bTerminateThread;					// ������ ���� �÷���
	CString		m_strFirstPatchURLPath;				// PatchURL/FirstPatch/ ��� ����.
	CString		m_strPatchListFile;					// Ŭ���̾�Ʈ ��ġ ���
	std::vector<std::string>	m_vecCopyList;		// ī�� ���� ����Ʈ
	std::vector<std::string>	m_vecDeleteList;	// ���� ���� ����Ʈ
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
	PatchReturn BeginPatch();				// ��ġ ���� �Լ�

	// Download
	BOOL DownLoadPatch( int nVersion );						// �ٿ�ε�		 ���μ���

#ifdef _USE_SPLIT_COMPRESS_FILE
	HRESULT URLDownload( LPCTSTR strPatchFileUrl, LPCTSTR strFilePath, LPCTSTR strFileName, 
						BOOL bSplitDownload = FALSE, DWORD dwAlreadyDownloadSize = 0, DWORD dwRemainDownloadFileSize = 0 );		// ���� �̾�ޱ� �ٿ�ε� ���μ���
	FileCheck CheckPatchFile( HINTERNET hInternetSession, LPCTSTR strFileName,
							CFile& file, DWORD& dwDownloadFileSize, BOOL bSplitDownload = FALSE );								// �ٿ�ε� ���� ���� üũ
#else // _USE_SPLIT_COMPRESS_FILE
	HRESULT URLDownload( LPCTSTR strPatchFileUrl, LPCTSTR strFilePath, LPCTSTR strFileName );	// ���� �̾�ޱ� �ٿ�ε� ���μ���
	FileCheck CheckPatchFile( HINTERNET hInternetSession, LPCTSTR strFileName,
							CFile& file, DWORD& dwDownloadFileSize );							// �ٿ�ε� ���� ���� üũ
#endif // _USE_SPLIT_COMPRESS_FILE

	DWORD GetDownloadFileSize( HINTERNET hInternetSession, CString strHttpPath );				// �ٿ���� ������ ������ ���ؿ��� �Լ�
	BOOL GetMD5Checksum();																		// �ٿ���� ������ ��ȿ�� �˻� (MD5)

	// Apply Packing File
	PatchReturn PreApplyPakPatch();									// Pak���� ���� ���� �غ��۾� (��ġ����Ʈ ����, ��ŷ���� ������)
	PatchReturn ApplyPakPatch();									// Pak���� ���� ���μ���
	PatchReturn ApplyDeleteList( CEtPackingFile& PatchSource );		// ���� ����Ʈ Pak���� ����
	PatchReturn ApplyPatchList( CEtPackingFile& PatchSource );		// ��ġ ����Ʈ Pak���� ����
	PatchReturn ApplyCopyList( CEtPackingFile& PatchSource );		// ���� ����Ʈ Pak���� ����
	PatchReturn GetPackingFile();									// ���ҽ� ��ŷ ������ ã�� ������ ������
	BOOL LoadPatchList();											// ��ġ ����Ʈ�� �о� ��ġ ������ ����
	PatchReturn PatchFileExist( int nPatchFileCount, CEtPackingFile& PatchSource, SPackingFileInfo* pFileInfo, BOOL& bPatch );
	PatchReturn PatchFileSuitableSpace( CEtPackingFile& PatchSource, SPackingFileInfo* pFileInfo, BOOL& bPatch );
	void PatchFileReserve( int nPatchFileCount, SPackingFileInfo* pFileInfo );

	void SendStatusMsg( DownloadPatchState nID );				// ���̾�α׷� �ٿ�ε���� �� ����

	BOOL ProcessFullVersionPatch();								// Ǯ ���� ��ġ ���μ���
	BOOL SaveVersionFile( CString& szFilePath, int nVersion );	// ��ġ �Ϸ� �� Version.cfg ���� ����
	BOOL SaveNewVersionToPak( int nVersion );					// Version.cfg���� Pak���� �ȿ� �ֱ�

	void RecordFileExceptionLog( CFileException& e );

#ifdef _USE_RTPATCH
	BOOL ExportFile( CEtPackingFile& PakFile, const char* pExportFileName, const char* pCreateFilePath );	// pak���ϳ� ���� ���� �ϱ�
	PatchReturn ApplyRTPatch( CEtPackingFile& PatchSource, CEtPackingFile& RTPatchSource );					// ���� pak������ RTPatch����� pak���Ϸ� �����
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

	DWORD		m_dwTempFileLength;		// �ٿ� ���� ������ ��
	DWORD		m_dwDownloadFileLength;	// �ٿ� �޾ƾ��� ������ ��
	DWORD		m_dwTotalFileLength;	// �� ������ ��
	DWORD		m_dwTotalBytes;			// �� ���� ������ ��

	CString		m_strPatchFileUrl;		// Http ��ġ ���� �ּ�
	CString		m_strPatchFileLocal;	// ������ ���� ��ġ (�ٿ�ε� �Ϸ�� .tmp -> .pak)
	CString		m_strPatchFile;			// ��ġ ���� (pak����)
	CString		m_strPatchListFile;		// ��ġ ����Ʈ ����

	float		m_fRate;				// Download Rate
	float		m_fPercent;				// Download Percent

	DownloadPatchState m_emDownPatchState;	// ���� �ٿ�ε� ���� ����
	
	HWND		m_hWnd;					// ������ �ڵ�
	BOOL		m_bTerminateThread;		// ������ ���� �÷���

	std::vector<CEtPackingFile*>	m_vecPackingFile;		// ��ŷ ���� ����Ʈ
	std::vector<std::string>		m_vecDeleteList;		// ���� ���� ����Ʈ
	std::vector<std::string>		m_vecPatchList;			// ��ġ ���� ����Ʈ
	std::vector<std::string>		m_vecCopyList;			// ���� ���� ����Ʈ
	struct stFileIndexSet
	{
		DWORD m_dwPakFileNum;
		DWORD m_dwPatchFileIndex;
		DWORD m_dwReserveFileIndex;
	};

	std::vector<stFileIndexSet>		m_vecReserveIndex;		// ����� ��ġ ���� �ε���


	int			m_nCurPatchCount;		// ���� ��ġ�� (���� ��ŷ �� ���α׷����� ������ ���)
	int			m_nTotalPatchCount;		// �� ��ġ�� (���� ��ŷ �� ���α׷����� ������ ���)

#ifdef _USE_RTPATCH
	std::string	m_strRTPatchTempFolder;		// RTPatch �ӽ� ����
#endif // _USE_RTPATCH

#ifdef _USE_SPLIT_COMPRESS_FILE
	HINSTANCE	hUnrarDLL;					// UnrarDLL ���

	HANDLE (WINAPI *OpenArchiveEx)( RAROpenArchiveDataEx* pArchiveData );
	int    (WINAPI *CloseArchive)( HANDLE hArcData );
	int    (WINAPI *ReadRARHeader)( HANDLE hArcData, RARHeaderData *pHeaderData );
	int    (WINAPI *ProcessRARFile)( HANDLE hArcData, int iOperation, char* strDestFolder, char* strDestName );
#endif // _USE_SPLIT_COMPRESS_FILE
};
