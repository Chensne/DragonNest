
#include "Stdafx.h"
#include "Log.h"
#include "GsmCmdProcess.h"

CGsmCmdProcess::CGsmCmdProcess()
{
}

CGsmCmdProcess::~CGsmCmdProcess()
{
}

CGsmCmdProcess * CGsmCmdProcess::GetInstance()
{
	static CGsmCmdProcess s;
	return &s;
}

void CGsmCmdProcess::AddCmdProcess(const char * pCmd, const char * pRespectCmd, void * pf)
{
	if (pCmd == NULL || pRespectCmd == NULL || pf == NULL)
		return;

	if (IsExistCmd(pCmd))
	{
		return;
	}

	TGSMCmd cmd;
	
	cmd.strCmd = pCmd;
	cmd.strRespectCmd = pRespectCmd;
	cmd.nCmdTick = timeGetTime();
	cmd.pfCall = (GSM_PROC)pf;

	ScopeLock <CSyncLock> lock(m_Sync);
	m_CmdList.push_back(cmd);
}

void CGsmCmdProcess::ParseCmdProcess(const char * pCmd)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	for (std::list <TGSMCmd>::iterator ii = m_CmdList.begin(); ii != m_CmdList.end(); ii++)
	{
		if ((*ii).strRespectCmd == pCmd)
		{
			if ((*ii).pfCall)
				(*ii).pfCall();
			return;
		}
	}
}

bool CGsmCmdProcess::IsExistCmd(const char * pCmd)
{
	ScopeLock <CSyncLock> lock(m_Sync);	
	for (std::list <TGSMCmd>::iterator ii = m_CmdList.begin(); ii != m_CmdList.end(); ii++)
	{
		if ((*ii).strCmd == pCmd)
			return true;
	}
	return false;
}