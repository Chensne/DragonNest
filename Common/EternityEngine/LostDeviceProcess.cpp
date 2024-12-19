#include "StdAfx.h"
#include "LostDeviceProcess.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

std::vector< CLostDeviceProcess * > CLostDeviceProcess::s_vecLostDevice;
CSyncLock CLostDeviceProcess::s_SyncLock;

extern CSyncLock *g_pEtRenderLock;

CLostDeviceProcess::CLostDeviceProcess(void)
{
	ScopeLock<CSyncLock> Lock(s_SyncLock);
	s_vecLostDevice.push_back( this );
}

CLostDeviceProcess::~CLostDeviceProcess(void)
{
	ScopeLock<CSyncLock> Lock(s_SyncLock);
	int i;
#ifndef _FINAL_BUILD
	// 가끔 여기서 뻑나는 경우 있어서 확인차 넣어 놓는다. by mapping
	int nLostCount = ( int )s_vecLostDevice.size();
#endif
	for( i = 0; i < ( int )s_vecLostDevice.size(); i++ )
	{
		if( s_vecLostDevice[ i ] == this ) {
			s_vecLostDevice.erase( s_vecLostDevice.begin() + i );
			break;
		}
	}
}

void CLostDeviceProcess::OnLostDeviceList()
{
	ScopeLock<CSyncLock> Lock1(g_pEtRenderLock);
	ScopeLock<CSyncLock> Lock2(s_SyncLock);
	int i;

	for( i = 0; i < ( int )s_vecLostDevice.size(); i++ )
	{
		if( s_vecLostDevice[ i ] )
		{
			s_vecLostDevice[ i ]->OnLostDevice();
		}
	}
}

void CLostDeviceProcess::OnResetDeviceList()
{
	ScopeLock<CSyncLock> Lock1(g_pEtRenderLock);
	ScopeLock<CSyncLock> Lock2(s_SyncLock);
	int i;

	for( i = 0; i < ( int )s_vecLostDevice.size(); i++ )
	{
		if( s_vecLostDevice[ i ] )
		{
			s_vecLostDevice[ i ]->OnResetDevice();
		}
	}
}
