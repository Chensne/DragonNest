#pragma once

#include <atltime.h>

class CLogCleaner
{
public:
	CLogCleaner(void);
	~CLogCleaner(void);

public:
	void CleanLog() const;

private:
	void SetPath();
	CTime GetCleanTime() const;

private:
	string path;
};