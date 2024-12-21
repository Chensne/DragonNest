#include "StdAfx.h"
#include "DnPvPTask.h"
#include "PvPSendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDNPvPTask::CDNPvPTask() : CTaskListener(true)
{
}

CDNPvPTask::~CDNPvPTask()
{
}

bool CDNPvPTask::Initialize()
{
	return true;
}

void CDNPvPTask::Process(LOCAL_TIME LocalTime, float fDelta)
{
}

void CDNPvPTask::OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize)
{
	switch (nMainCmd)
	{
		case SC_PVP: OnRecvPvPMessage(nSubCmd, pData, nSize); break;
	}
}

void CDNPvPTask::OnRecvPvPMessage(int nSubCmd, char * pData, int nSize)
{
}
