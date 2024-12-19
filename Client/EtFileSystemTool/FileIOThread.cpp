#include "stdafx.h"
#include "FileIOThread.h"
#include "WorkDialog.h"
#include "SundriesFunc.h"
#include <utility>

//------------------------------------------------------------------------
AddFileList				g_AddFileList;
ExportInfoList			g_ExportList;
CEtPackingFile			g_FileSystem;

CFileImportThread*		g_pAddThread=NULL;
COptimizeThread*		g_pOptimizeThread=NULL;
CFileExportThread*		g_pExportThread=NULL;
CDivPackingThread*		g_pDivPackingThread = NULL;
CDivPackingCapacityThread* g_pDivPackingCapaThread = NULL;
PatchThread*			g_pPatchThread = NULL;

//------------------------------------------------------------------------
CFileImportThread::CFileImportThread(HWND hWnd)
	: m_hWnd( hWnd )
{
	
}

CFileImportThread::~CFileImportThread()
{

}

void
CFileImportThread::Run()
{
	if (m_hWnd == NULL)
		return;

	AddFileList_It FileListIt = g_AddFileList.begin();

	int nMaxSize = (int)g_AddFileList.size();
	int nCnt = 0;
	for (FileListIt ; FileListIt != g_AddFileList.end() ; ++FileListIt )
	{
		g_FileSystem.ChangeDir((*FileListIt).FilePath.c_str());
		g_FileSystem.AddFile((*FileListIt).FileName.c_str() );
		nCnt++;

		g_pWorking->PostMessage(WM_NFS_PROGRESS, nCnt, nMaxSize);
	}
	g_FileSystem.GeneratePackingMap();

	g_AddFileList.clear();
	PostMessage(m_hWnd,WM_COMPLETE_MSG,0,0);
 }


//------------------------------------------------------------------------

COptimizeThread::COptimizeThread(HWND hWnd, CString strPath)
	: m_hWnd( hWnd ) , m_strPath( strPath )
{
	
}

COptimizeThread::~COptimizeThread()
{

}

void
COptimizeThread::Run()
{

	CString strPath = m_strPath;
	strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	strPath = strPath + _T("\\Optimized.pak");

	g_FileSystem.OptimizeFileSystem(strPath);
	g_FileSystem.CloseFileSystem();
	
	
	DWORD nError = 0;
	if ( DeleteFile(m_strPath.GetBuffer()) == FALSE )
	{
		nError = GetLastError();
	}
	if ( MoveFile(strPath, m_strPath) == FALSE )
	{
		nError = GetLastError();
	}

	//_trename(strPath.GetBuffer(), m_strPath.GetBuffer());
	g_FileSystem.OpenFileSystem(m_strPath);

	
	PostMessage(m_hWnd,WM_COMPLETE_OPTIMIZE,0,0);
 }

//------------------------------------------------------------------------

CFileExportThread::CFileExportThread(HWND hWnd, CString strPath)
	: m_hWnd( hWnd ) , m_ExportPath( strPath )
{

}

CFileExportThread::~CFileExportThread()
{

}

void
CFileExportThread::Run()
{

	ExportInfoList_It Export_It = g_ExportList.begin();

	int nCnt = 0 ;
	for (Export_It ; Export_It != g_ExportList.end() ; ++Export_It )
	{
		g_pWorking->PostMessage(WM_NFS_PROGRESS, nCnt, g_ExportList.size());
		nCnt++;
		SetCurrentDirectory(m_ExportPath);	// Export 폴더로 이동

		ExportInfo tempFileInfo = (ExportInfo)(*Export_It);
		
		CString newPath = m_ExportPath + tempFileInfo.FilePath.c_str();
		CreateFolder( (LPTSTR)(LPCTSTR)newPath );
		SetCurrentDirectory((LPTSTR)(LPCTSTR)newPath);		// Export 폴더로 이동
		
		CString iPacPath = tempFileInfo.Name.c_str();	// 경로 대입
//				iPacPath += tempFileInfo.Name.c_str();		// 파일 이름까징~
	
		CEtFileHandle* FH =  g_FileSystem.OpenFile((LPTSTR)(LPCTSTR)iPacPath);
		if ( FH )
		{
			FH->ExportFile();
			g_FileSystem.CloseFile(FH);
		}
	}
	g_ExportList.clear();
	PostMessage(m_hWnd,WM_COMPLETE_EXPORT,0,0);
}

#include "LogWnd.h"
//------------------------------------------------------------------------
void
CDivPackingThread::Run()
{
	LogWnd::CreateLog();
	
	for ( size_t i = 0 ; i < m_PackingInfo.size() ; ++i )
	{
		PostMessage(m_hWnd,WM_NFS_PROGRESS_TOTAL, i, m_PackingInfo.size());
		
		_PackingInfo info = m_PackingInfo[i];

		CString szFullPath;
		szFullPath = m_OutputPath;
		szFullPath += _T("\\");
		szFullPath += info.szFileName.c_str();
		
		LogWnd::TraceLog("패킹 폴더 %s", szFullPath.GetBuffer());
		//bool bResult = g_FileSystem.OpenFileSystem(szFullPath.GetBuffer());
		//if ( bResult == false )
		{
			bool bResult = false;	
			bResult = g_FileSystem.NewFileSystem(szFullPath.GetBuffer());
			bResult = g_FileSystem.OpenFileSystem(szFullPath.GetBuffer());
			if ( bResult == false )
			{
				AfxMessageBox(_T("파일 시스템 생성 실패"));
				return;
			}
		}
		
		g_AddFileList.clear();
//		g_FileSystem.AddDirWithChange(info.szBaseFolder.c_str());

		
		DropFolder(info.szSrcFolder.c_str(), info.bIncludeSubFolder);


		AddFileList_It FileListIt = g_AddFileList.begin();
		int nMaxSize = (int)g_AddFileList.size();
		int nCnt = 0;
		LogWnd::TraceLog("저장할 갯수 [%d]", nMaxSize);
		for (FileListIt ; FileListIt != g_AddFileList.end() ; ++FileListIt )
		{
			AddFileInfo& info = *FileListIt;
			LogWnd::TraceLog("%s|%s", info.FilePath.c_str(), info.FileName.c_str());
			bool b = g_FileSystem.ChangeDir((*FileListIt).FilePath.c_str());
			if ( !b )
				LogWnd::TraceLog("ChangeDir 오류!!!%s|%s", info.FilePath.c_str(), info.FileName.c_str());

			b = g_FileSystem.AddFile((*FileListIt).FileName.c_str() );
			if ( !b )
				LogWnd::TraceLog("AddFile 오류!!!%s|%s", info.FilePath.c_str(), info.FileName.c_str());
			nCnt++;

			PostMessage(m_hWnd,WM_NFS_PROGRESS, nCnt, nMaxSize);
		}

		g_AddFileList.clear();

	}

	LogWnd::DestroyLog();
	PostMessage(m_hWnd,WM_NFS_PROGRESS_TOTAL, m_PackingInfo.size(), m_PackingInfo.size());
	PostMessage(m_hWnd,WM_COMPLETE_MSG,0,0);
}




BOOL
CDivPackingThread::DropFolder(const TCHAR* strPath, bool bIncludeSubFolder /* = true */)
{
	TCHAR strOldPath[_MAX_PATH] = {0, };
	GetCurrentDirectory(_MAX_PATH,strOldPath);	// 현재 이 프로그램이 실행중인 디렉토리를 알아낸다.
	SetCurrentDirectory(strPath);				// 드롭할 폴더로 이동.?? 

	TCHAR strName[_MAX_FNAME] = {0, };
	g_FileSystem.GetNameOnly(strName,strPath);

//	if(!g_FileSystem.CheckNameExist(strName))
	{
//		g_FileSystem.AddDir(strName);
	}

	if(!g_FileSystem.ChangeDir(strName))
		return FALSE;


	CFileFind FileFind;
	if(FileFind.FindFile())
	{
		for(BOOL bNext=TRUE;bNext;)
		{
			bNext = FileFind.FindNextFile();
			CString strP = FileFind.GetFilePath();
			CString strN = FileFind.GetFileName();

			
			if ( strP.Find(strPath)	< 0 )
			{
				CString str;
				str.Format(_T("DropFolder : 폴더가 다름 %s %s"), strP.GetBuffer(), strPath);
				AfxMessageBox(str);
				return FALSE;
			}
			if(strN == "." || strN == "..") continue;

			if(FileFind.IsDirectory())
			{
				if ( bIncludeSubFolder )
				{
					BOOL bResult = DropFolder(LPCTSTR(strP), bIncludeSubFolder);
					if ( !bResult )
						return bResult;
				}
			}
			else
			{
/*				if(g_FileSystem.CheckNameExist(strN))
				{
					if(m_iOverwriteType < 0)
						m_iOverwriteType = AfxMessageBox(_T("동일한 이름의 파일이 이미 존재합니다.\n\n모든 파일을 새로운 파일로 덮어쓰시려면 '예' 를\n\n모든 기존의 파일을 보존하시려면 '아니오' 를 눌러주십시오!"),MB_YESNO);

					if(m_iOverwriteType == IDNO) 
						continue;

					g_FileSystem.Remove(strN);
				}*/

				// 파일 추가..
				AddFileInfo add;
				add.FileName = strP;
				add.FilePath = g_FileSystem.GetCurDir();
				g_AddFileList.push_back(add);
			}
		}
	}

	FileFind.Close();

	g_FileSystem.ChangeDir(_T(".."));

	SetCurrentDirectory(strOldPath);

	return TRUE;
}

//--------------------------------------------------------------------------------------------
void
CDivPackingCapacityThread::Run()
{
	
	LogWnd::CreateLog();

	m_TotalFileList.clear();
	m_TotalFileList.reserve(1024 * 100);

	LogWnd::TraceLog("패킹 입력 폴더 %s", m_Info.m_InputPath.GetBuffer());
	LogWnd::TraceLog("패킹 출력 폴더 %s", m_Info.m_OutputPath.GetBuffer());
	LogWnd::TraceLog("베이스 파일 이름 %s.%s  %dMB 기준 분할", m_Info.m_szBaseName.GetBuffer(), m_Info.m_szExtName.GetBuffer(), m_Info.nDivCapacity);

	// 해당 경로의 모든 파일경로와 크기를 구한다.
	_GetTotalFileList(m_Info.m_InputPath.GetBuffer());

	LogWnd::TraceLog("전체 파일 갯수 %d 개", (int)m_TotalFileList.size());


	// 적당히 용량별 경로별로 묶음을 만들어주자.
	_DivFileList();

	LogWnd::TraceLog("전체 파일 갯수 %d 개", (int)m_TotalFileList.size());

	_Packing();

}


bool
CDivPackingCapacityThread::_GetTotalFileList(const TCHAR* strPath)
{
	TCHAR strOldPath[_MAX_PATH] = {0, };
	GetCurrentDirectory(_MAX_PATH,strOldPath);	// 현재 이 프로그램이 실행중인 디렉토리를 알아낸다.
	BOOL b = SetCurrentDirectory(strPath);				// 드롭할 폴더로 이동.?? 
	
	if ( !b )
	{
		DWORD nErr = GetLastError();
		return false;
	}

	CFileFind FileFind;
	if(FileFind.FindFile())
	{
		for(BOOL bNext=TRUE;bNext;)
		{
			bNext = FileFind.FindNextFile();
			CString strP = FileFind.GetFilePath();
			CString strN = FileFind.GetFileName();


			if ( strP.Find(strPath)	< 0 )
			{
				CString str;
				str.Format(_T("DropFolder : 폴더가 다름 %s %s"), strP.GetBuffer(), strPath);
				AfxMessageBox(str);
				return false;
			}
			if(strN == "." || strN == "..") continue;

			if(FileFind.IsDirectory())
			{
				bool bResult = _GetTotalFileList(LPCTSTR(strP));
				if ( !bResult )
					return bResult;
			}
			else
			{
				FileInfo info;
				info.szFileName = strN;
				info.szFilePath = strP;

				DWORD nFileSize = 0;
				_GetFileSize(strP.GetBuffer(), nFileSize);

				info.nFileSize = (__int64)nFileSize;
				m_TotalFileList.push_back(info);
				
		
			}
		}
	}

	FileFind.Close();


	SetCurrentDirectory(strOldPath);


	return true;
}


bool
CDivPackingCapacityThread::_GetFileSize(const TCHAR* strPath, DWORD& nFileSize )
{
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	hFile = CreateFile(strPath, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	// 파일 크기를 알아내서.
	DWORD _nFileSize = 0;
	nFileSize = 0;
	_nFileSize = GetFileSize(hFile, NULL);
	if(!_nFileSize)
	{
		CloseHandle(hFile);
		nFileSize = _nFileSize;
		return false;
	}
	CloseHandle(hFile);
	nFileSize = _nFileSize;
	return true;
}


bool
CDivPackingCapacityThread::_DivFileList()
{
	// 최대한 비슷한 경로끼리 묶어 주기 위해서 맵으로 한번더 소팅하자.
	for ( size_t i = 0 ; i < m_TotalFileList.size() ; i++ )
	{
		FileInfo& info = m_TotalFileList[i];
		m_TotalFileMaps.insert(std::make_pair(info.szFilePath, info));
	}

	m_DivisionList.clear();

	FileListInfo filelist_info;
	filelist_info.clear();
	__int64 nCapa = m_Info.nDivCapacity*1024*1024;

	int nFileCnt = 0;
	std::map<CString, FileInfo>::iterator it = m_TotalFileMaps.begin();
	for ( it ; it != m_TotalFileMaps.end() ; it++ )
	{
		FileInfo& info = it->second;

		filelist_info.nFileCnt++;
		filelist_info.nTotalFileBytes += info.nFileSize;
		filelist_info.FileList.push_back(info);


		if ( filelist_info.nTotalFileBytes+info.nFileSize >= nCapa )
		{
			LogWnd::TraceLog("[%d]번 파일 [Cnt:%3d] FileSortList %s(%d)" , nFileCnt, filelist_info.nFileCnt, info.szFilePath.GetBuffer(), info.nFileSize );

			m_DivisionList.push_back(filelist_info);
			filelist_info.clear();
			nFileCnt++;
		}
	}

	// 마지막껏도 추가 
	m_DivisionList.push_back(filelist_info);


	return true;
}

bool
CDivPackingCapacityThread::_Packing()
{

	CString szCurFileName;
	
	LogWnd::TraceLog("분할될 파일 총 %d 개 " , m_DivisionList.size() );
	for ( size_t i = 0 ; i < m_DivisionList.size() ; i++ )
	{
		szCurFileName.Format(_T("%s%d.%s"), m_Info.m_szBaseName.GetBuffer(), i, m_Info.m_szExtName.GetBuffer());
		FileListInfo& filelist_info = m_DivisionList[i];

		CString strPath;
		strPath = m_Info.m_OutputPath;
		strPath += "\\";
		strPath += szCurFileName;
		
		LogWnd::TraceLog("NewFileSystem %s" , strPath.GetBuffer());

		bool bResult = g_FileSystem.NewFileSystem(strPath.GetBuffer());
		if ( !bResult )
			LogWnd::TraceLog("NewFileSystem Error  %s" , strPath.GetBuffer());

		bResult = g_FileSystem.OpenFileSystem(strPath.GetBuffer());
		if ( !bResult )
			LogWnd::TraceLog("NewFileSystem Error  %s" , strPath.GetBuffer());

		bResult = g_FileSystem.ChangeDir("\\");
		if ( bResult )
			LogWnd::TraceLog("NewFileSystem Success %s" , strPath.GetBuffer());


		for ( size_t j = 0 ; j < filelist_info.FileList.size() ; j++ ) 
		{
			FileInfo& info = filelist_info.FileList[j];
			CString szNewPath = info.szFilePath;
			szNewPath.Delete(0, m_Info.m_InputPath.GetLength());
			_AddFileWithFolder(std::string(szNewPath.GetBuffer()), info);
			PostMessage(m_hWnd,WM_NFS_PROGRESS_CAP, j, filelist_info.FileList.size());
		}
		g_FileSystem.CloseFileSystem();

		PostMessage(m_hWnd,WM_NFS_PROGRESS_TOTAL_CAP, i, m_DivisionList.size());
	}

	LogWnd::DestroyLog();
	PostMessage(m_hWnd,WM_NFS_PROGRESS_TOTAL_CAP, m_DivisionList.size(), m_DivisionList.size());
	PostMessage(m_hWnd,WM_COMPLETE_MSG_CAP,0,0);
	LogWnd::TraceLog("------------------ 완료 --------------------");

	return true;
}

bool
CDivPackingCapacityThread::_AddFileWithFolder(std::string& szPath, FileInfo& info)
{
	CHAR drive[2048] = {0,};
	CHAR dir[2048] = {0,};
	CHAR fname[2048] = {0,};
	CHAR ext[2048] = {0,};

	_splitpath( szPath.c_str(), drive, dir, fname, ext ); 

	std::string szNewPath = dir;
	std::string szFileName;
	szFileName = fname;
	szFileName += ext;

	std::vector<std::string> tokens;
	TokenizeA(szNewPath, tokens, std::string("\\"));

	g_FileSystem.ChangeDir("\\");
	for ( size_t i = 0 ; i < tokens.size() ; i++ )
	{
		std::string szFolder = tokens[i];
		bool bResult = g_FileSystem.ChangeDir(szFolder.c_str());
/*		if (!bResult)
		{
			g_FileSystem.AddDirWithChange(szFolder.c_str());
		}*/
	}

	LogWnd::TraceLog("AddFile %s(%d)" , info.szFilePath.GetBuffer(), info.nFileSize );
	g_FileSystem.AddFile(info.szFilePath.GetBuffer());
//	LogWnd::TraceLog("_AddFileWithFolder %s" , info.szFilePath.GetBuffer());

	return true;
}


//------------------------------------------------------------------------
void PatchLog::Log(const TCHAR* fmt, ...)
{
	static TCHAR gs_Buffer[8192] = { _T('0'), };

	ZeroMemory(gs_Buffer, sizeof(TCHAR) * 8192);
	va_list args;
	va_start( args, fmt );
	_vsntprintf_s( gs_Buffer, 8192-1, fmt, args );
	va_end( args );
	gs_Buffer[8192-1] = 0;

	CString str;
	str = gs_Buffer;
	str += _T('\n');

	TRACE(str.GetBuffer());

	LogWnd::TraceLog( _T("%s"), str.GetBuffer() );
}
PatchLog g_PatchLog;

PatchThread::PatchThread(HWND hWnd, std::string& szFileName)
{
	m_hWnd = hWnd;
	m_srcPackFilePath = szFileName;
}

PatchThread::~PatchThread()
{

}

void
PatchThread::OnPatch(int nCurrentCnt, int nMaxCount, std::string szFilename)
{
	if ( g_pWorking )
		g_pWorking->PostMessage(WM_NFS_PROGRESS, nCurrentCnt, nMaxCount);

}

void
PatchThread::OnError(int nErrorCode, std::string szMsg)
{

}

void 
PatchThread::Run()
{

	//g_FileSystem.SetLogger(&g_PatchLog);
	bool bResult = g_FileSystem.Patch( m_srcPackFilePath.c_str(), this, NULL);

	PostMessage(m_hWnd,WM_COMPLETE_PATCH, bResult,0);

}