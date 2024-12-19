#include "StdAfx.h"
//#include "DnSkill.h"
#include "MAAiReceiver.h"
#include "DnActor.h"
#include "MAAiBase.h"
#include "DnActorState.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MAAiReceiver::MAAiReceiver()
:m_pAi(NULL)
{
}

MAAiReceiver::~MAAiReceiver()
{
	SAFE_DELETE(m_pAi);
}

bool MAAiReceiver::Initialize( int nMonsterTableID )
{
	m_hActor = (dynamic_cast<CDnActor*>(this))->GetMySmartPtr();
	m_pAi = new MAAiBase( m_hActor, this );
	m_pAi->Initialize();
	
	return true;
}

void MAAiReceiver::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_pAi ) 
		return;

	m_pAi->Process( LocalTime, fDelta );
}

void MAAiReceiver::SetAggroTarget( DnActorHandle hActor )
{
	if( m_pAi ) m_pAi->SetTarget( hActor );
}

DnActorHandle MAAiReceiver::GetAggroTarget()
{
	return ( m_pAi ) ? m_pAi->GetTarget() : CDnActor::Identity();
}
