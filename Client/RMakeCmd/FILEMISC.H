#pragma once

#include <windows.h>
#include <time.h>

time_t GetLastModified(const char* szPath);
bool GetLastModified(const char* szPath, SYSTEMTIME& sysTime, bool bLocalTime = true); // files only
bool ResetLastModified(const char* szPath); // resets to current time
double GetFileSize(const char* szPath);

bool RemoveFolder(const char* szFolder, 
				  HANDLE hTerminate = NULL, 
				  BOOL bProcessMsgLoop = TRUE);

bool DeleteFolderContents(const char* szFolder, 
						  BOOL bIncludeSubFolders, 
						  const char* szFileMask, 
						  HANDLE hTerminate = NULL, 
						  BOOL bProcessMsgLoop = TRUE);

double GetFolderSize(const char* szFolder, 
					 BOOL bIncludeSubFolders = TRUE, 
					 const char* szFileMask = NULL, 
					 HANDLE hTerminate = NULL, 
					 BOOL bProcessMsgLoop = TRUE);

bool CreateFolder(const char* szFolder);

// will delete the source folder on success
bool MoveFolder(const char* szSrcFolder, 
				const char* szDestFolder, 
				HANDLE hTerminate = NULL, 
				BOOL bProcessMsgLoop = TRUE);

bool CopyFolder(const char* szSrcFolder, 
				const char* szDestFolder, 
				HANDLE hTerminate = NULL, 
				BOOL bProcessMsgLoop = TRUE);

// will delete the source folder only if file mask was "*.*"
bool MoveFolder(const char* szSrcFolder, 
				const char* szDestFolder, 
				const char* szFileMask, 
				HANDLE hTerminate = NULL, 
				BOOL bProcessMsgLoop = TRUE);

bool CopyFolder(const char* szSrcFolder, 
				const char* szDestFolder, 
				BOOL bIncludeSubFolders, 
				const char* szFileMask, 
				HANDLE hTerminate = NULL, 
				BOOL bProcessMsgLoop = TRUE);
