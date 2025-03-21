#include "StdAfx.h"
#include "SCRVelocity.h"
#include "ActionSignal.h"
#include "ActionObject.h"


CSCRVelocity::CSCRVelocity( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
}

CSCRVelocity::~CSCRVelocity()
{
}

void CSCRVelocity::Initialize()
{
	m_bActivate = true;
	OnModify();

}

void CSCRVelocity::OnModify()
{
}

void CSCRVelocity::OnSelect( bool bFirst )
{
}

void CSCRVelocity::OnUnSelect()
{
}

void CSCRVelocity::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	((CActionObject*)m_pSignal->GetParent()->GetParent())->SetVelocity( m_pSignal->GetProperty(0)->GetVariableVector3() );
	((CActionObject*)m_pSignal->GetParent()->GetParent())->SetResistance( m_pSignal->GetProperty(1)->GetVariableVector3() );
}
