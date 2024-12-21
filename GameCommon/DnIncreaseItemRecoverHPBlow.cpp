#include "StdAfx.h"
#include "DnIncreaseItemRecoverHPBlow.h"

CDnIncreaseItemRecoverHPBlow::CDnIncreaseItemRecoverHPBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_272;
	SetValue( szValue );
	m_fValue = (float)atof(szValue);
}

CDnIncreaseItemRecoverHPBlow::~CDnIncreaseItemRecoverHPBlow(void)
{

}

void CDnIncreaseItemRecoverHPBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{

}

void CDnIncreaseItemRecoverHPBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

}