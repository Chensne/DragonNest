#include "StdAfx.h"
#include "SMScanActor.h"
#include "DnActor.h"

CSMScanActor::CSMScanActor( DNVector(DnActorHandle)& vlhActor )
{
	m_iType = SCAN_ACTOR;

	int iNumActor = (int)vlhActor.size();
	for( int i = 0; i < iNumActor; ++i )
		m_setScanedActor.insert( vlhActor.at(i)->GetUniqueID() );
}

CSMScanActor::~CSMScanActor(void)
{
}
