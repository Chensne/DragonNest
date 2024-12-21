
#pragma once

#include "ProcessManager.h"
#include "LauncherPatcherSession.h"
#include "MiniDump.h"
#include "DNConfig.h"
#include "IniFile.h"
#include "Version.h"

class CESMFrameWork
{
public:
	static CESMFrameWork * GetInstance();

	CESMFrameWork();
	~CESMFrameWork();

	bool LoadConfig();
	bool AppInit();
	void AppClose();
};