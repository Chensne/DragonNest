#pragma once

#include "Thread.h"
#include "EtFileSystem.h"
#include <list>
#include <algorithm>
#include "BuildDivisionPacking.h"

#define WM_COMPLETE_MSG				WM_USER+1		// ������ �Ϸ� �޼���
#define WM_COMPLETE_OPTIMIZE		WM_USER+2		// ����ȭ������ �Ϸ� �޼���
#define WM_COMPLETE_EXPORT			WM_USER+3		// ���⾲���� �Ϸ� �޼���
#define WM_NFS_PROGRESS				WM_USER+4		
#define WM_NFS_PROGRESS_TOTAL		WM_USER+5
#define WM_NFS_PROGRESS_CAP			WM_USER+6		
#define WM_NFS_PROGRESS_TOTAL_CAP	WM_USER+7
#define WM_COMPLETE_MSG_CAP			WM_USER+8
#define WM_COMPLETE_PATCH			WM_USER+9		// ��ġ �Ϸ� �޼���


//------------------------------------------------------------------------
enum FILETYPE 
{
	NFS_DIR,
	NFS_FILE,
};

typedef struct 
{
	tstring		FileName;
	tstring		FilePath;
}AddFileInfo;

typedef struct 
{
	tstring		Name;
	tstring		FilePath;
	int			FileType; //enum FILETYPE {FT_DIR,FT_FILE};
}ExportInfo;

//------------------------------------------------------------------------

typedef std::list<AddFileInfo>				AddFileList;
typedef std::list<AddFileInfo>::iterator	AddFileList_It;

typedef std::list<ExportInfo>				ExportInfoList;
typedef std::list<ExportInfo>::iterator		ExportInfoList_It;

extern  AddFileList						g_AddFileList;
extern  ExportInfoList					g_ExportList;
extern	CEtPackingFile					g_FileSystem;


//------------------------------------------------------------------------
/*
 * CFileIoThread ���� ����� ������.. 
 */
class CFileImportThread : public Thread
{
protected:
			HWND	m_hWnd;
public:
	virtual	void	Run();

			CFileImportThread(HWND hWnd);
	virtual ~CFileImportThread();

};

extern CFileImportThread*			g_pAddThread;

//------------------------------------------------------------------------
/*
 * COptimizeThread ���� �ý��� ����ȭ ������
 */
class COptimizeThread : public Thread
{
protected:
			HWND		m_hWnd;
			CString		m_strPath;
public:
	virtual	void	Run();

			COptimizeThread(HWND hWnd, CString strPath);
	virtual ~COptimizeThread();
};

extern COptimizeThread*			g_pOptimizeThread;

//------------------------------------------------------------------------
/*
 * CFileIoThread ���� ����� ������.. 
 */
class CFileExportThread : public Thread
{
protected:
			HWND		m_hWnd;
			CString		m_ExportPath;
public:
	virtual	void	Run();
			void	Export();

	CFileExportThread(HWND hWnd, CString strPath);
	virtual ~CFileExportThread();

};

extern CFileExportThread*			g_pExportThread;

//------------------------------------------------------------------------
class CDivPackingThread : public Thread
{
protected:
	HWND		m_hWnd;
	std::vector<_PackingInfo>	m_PackingInfo;
	CString		m_OutputPath;
	int			m_iOverwriteType;
public:
	virtual	void	Run();
	BOOL DropFolder(const TCHAR* strPath, bool bIncludeSubFolder = true);

	CDivPackingThread(HWND hWnd, CString& OutputPath, std::vector<_PackingInfo>& info)
	{
		m_iOverwriteType = -1;
		m_hWnd = hWnd;
		m_OutputPath = OutputPath;
		m_PackingInfo = info;
	}
	virtual ~CDivPackingThread(){}

};

extern CDivPackingThread*			g_pDivPackingThread;

struct DIVPackingCapacityInfo 
{
	DIVPackingCapacityInfo() { nDivCapacity = 50; m_szBaseName = _T("Data"); m_szExtName = _T("Nfs"); }
	CString		m_InputPath;
	CString		m_OutputPath;
	CString		m_szBaseName;
	CString		m_szExtName;
	int			nDivCapacity; // ���� �뷮 MB ����.
};


class CDivPackingCapacityThread : public Thread
{
protected:
	HWND		m_hWnd;
	DIVPackingCapacityInfo	m_Info;

	struct FileInfo
	{
		CString szFilePath;
		CString szFileName;
		__int64	nFileSize;
	};

	typedef std::vector<FileInfo>	FILE_LIST;
	
	FILE_LIST	m_TotalFileList;
	__int64		m_nTotalFileListBytes;
	int			m_nTotalFileSizeMB;

	struct FileListInfo 
	{
		void clear()
		{
			FileList.clear();
			nFileCnt = 0;
			nTotalFileBytes = 0;
			nTotalFileSizeMB = 0;
		}
		FILE_LIST	FileList;
		int			nFileCnt;
		__int64		nTotalFileBytes;
		int			nTotalFileSizeMB;
	};
	
	std::vector< FileListInfo >  m_DivisionList;
	std::map<CString, FileInfo>	 m_TotalFileMaps;

private:

	bool _GetTotalFileList(const TCHAR* strPath);
	bool _GetFileSize(const TCHAR* strPath, DWORD& nfileSize );
	bool _DivFileList();
	bool _Packing();
	bool _AddFileWithFolder(std::string& szPath, FileInfo& info);
public:
	virtual	void	Run();
	BOOL DropFolder(const TCHAR* strPath, bool bIncludeSubFolder = true);

	CDivPackingCapacityThread(HWND hWnd, DIVPackingCapacityInfo& info)
	{
		m_hWnd = hWnd;
		m_Info = info;
	}
	virtual ~CDivPackingCapacityThread(){}

};

extern CDivPackingCapacityThread* g_pDivPackingCapaThread;


class PatchThread : public Thread, public CPatchCallback
{
public:
	PatchThread(HWND hWnd, std::string& szFileName);
	virtual ~PatchThread();

	virtual void OnPatch(int nCurrentCnt, int nMaxCount, std::string szFilename);
	virtual void OnError(int nErrorCode, std::string szMsg);
	virtual	void	Run();

	std::string		m_srcPackFilePath;
	HWND m_hWnd;
};

extern PatchThread*	g_pPatchThread;

class PatchLog : public CEtFileSystemLog
{
public:
	virtual void Log(const TCHAR* fmt, ...);
};