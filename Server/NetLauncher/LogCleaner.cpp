#include "StdAfx.h"
#include "LogCleaner.h"

const int CLEAN_DAY = 60;

CLogCleaner::CLogCleaner(void)
{
	SetPath();
}

CLogCleaner::~CLogCleaner(void)
{
}

void CLogCleaner::CleanLog() const
{
	if (path.empty())
		return;

	string filter(path + "\\*.log");

	WIN32_FIND_DATA wfd;
	HANDLE handle = FindFirstFile(filter.c_str(), &wfd);
	if (handle == INVALID_HANDLE_VALUE)
		return;

	try
	{
		CTime cleanTime = GetCleanTime();

		while (1)
		{
			CTime creationTime(wfd.ftCreationTime);
			if (creationTime < cleanTime)
			{
				string filename(path + "\\" + wfd.cFileName);
				BOOL result = DeleteFile(filename.c_str());
				if (!result)
				{
					DWORD error = GetLastError();
					// 권한이 없을 경우
				}
			}

			if (!FindNextFile(handle, &wfd))
				break;
		}
	}
	catch (...)
	{
	}

	FindClose(handle);
}

void CLogCleaner::SetPath()
{
	char buf[1024];
	GetCurrentDirectory(1024, buf);
	path = buf;
	path += "\\log";
}

CTime CLogCleaner::GetCleanTime() const
{
	return CTime::GetCurrentTime() - CTimeSpan(CLEAN_DAY, 0, 0, 0);
}