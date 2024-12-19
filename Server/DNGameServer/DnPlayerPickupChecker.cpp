#include "StdAfx.h"
#include "DnPlayerPickupChecker.h"
#include "DnDropItem.h"
#include "DnPlayerActor.h"
#include "DNUserSession.h"

CDnPlayerPickupChecker::CDnPlayerPickupChecker( CDnPlayerActor *pPlayer )
: IDnPlayerChecker( pPlayer )
{
	m_nInvalidCount = 0;
}

CDnPlayerPickupChecker::~CDnPlayerPickupChecker()
{
}

bool CDnPlayerPickupChecker::IsInvalidPlayer()
{
	return ( m_nInvalidCount >= 3 ) ? true : false;
}

void CDnPlayerPickupChecker::OnPickupDist( EtVector3 &vPos, DnDropItemHandle hDropItem )
{
	if( !hDropItem ) return;
	EtVector3 vTemp = vPos - *hDropItem->GetPosition();
	vTemp.y = 0.f;
	float fDist = EtVec3LengthSq( &vTemp );
	if( fDist < 0.1f ) m_nInvalidCount++;

	if( m_pActor && m_pActor->GetUserSession() && fDist <= 0.1f )
	{
		if( m_nInvalidCount >= 2 ) 
		{
			g_Log.Log(LogType::_HACK, m_pActor->GetUserSession(), L"HackChecker(PickUp) : CharName=%s, DistSQ=%.2f\n", m_pActor->GetUserSession()->GetCharacterName(), fDist );
			m_pActor->OnInvalidPlayerChecker( 10 );
		}
	}
}
