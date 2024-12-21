#include "StdAfx.h"
#include "SCRJump.h"
#include "ActionSignal.h"
#include "ActionObject.h"


CSCRJump::CSCRJump( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
}

CSCRJump::~CSCRJump()
{
}

void CSCRJump::Initialize()
{
	m_bActivate = true;
	OnModify();

}

void CSCRJump::OnModify()
{
}

void CSCRJump::OnSelect( bool bFirst )
{
}

void CSCRJump::OnUnSelect()
{
}

void CSCRJump::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	float fVelocity = m_pSignal->GetProperty(0)->GetVariableFloat();
	float fResistance = m_pSignal->GetProperty(1)->GetVariableFloat();
	((CActionObject*)m_pSignal->GetParent()->GetParent())->SetVelocity( EtVector3( 0.f, fVelocity, 0.f ) );
	((CActionObject*)m_pSignal->GetParent()->GetParent())->SetResistance( EtVector3( 0.f, fResistance, 0.f ) );
}
