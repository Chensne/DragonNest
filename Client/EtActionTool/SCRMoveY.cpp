#include "StdAfx.h"
#include "SCRMoveY.h"
#include "ActionSignal.h"
#include "ActionObject.h"


CSCRMoveY::CSCRMoveY( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
}

CSCRMoveY::~CSCRMoveY()
{
}

void CSCRMoveY::Initialize()
{
	m_bActivate = true;
	OnModify();

}

void CSCRMoveY::OnModify()
{
}

void CSCRMoveY::OnSelect( bool bFirst )
{
}

void CSCRMoveY::OnUnSelect()
{
}

void CSCRMoveY::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	int iFrameLength = m_pSignal->GetEndFrame() - m_pSignal->GetStartFrame();

	float fVelocityY = m_pSignal->GetProperty(0)->GetVariableFloat();
	float fWholeMoveYDistance = fVelocityY * ((float)iFrameLength / 60.0f);
	fWholeMoveYDistance = (0.0f < fWholeMoveYDistance) ? fWholeMoveYDistance : -fWholeMoveYDistance;
	((CActionObject*)m_pSignal->GetParent()->GetParent())->SetMoveYDistancePerSec( fVelocityY, fWholeMoveYDistance );

	//float fYDelta = pStruct->fVelocityY * ((float(SignalEndTime - SignalStartTime) / 1000.0f));
	//float fWholeMoveYDistance = (0.0f < fYDelta) ? fYDelta : -fYDelta;
	//m_pMovement->SetMoveYDistancePerSec( pStruct->fVelocityY, fWholeMoveYDistance, (pStruct->bMaintainYPos == TRUE) ? true : false );

	//float fVelocity = m_pSignal->GetProperty(0)->GetVariableFloat();
	//float fResistance = m_pSignal->GetProperty(1)->GetVariableFloat();
	//((CActionObject*)m_pSignal->GetParent()->GetParent())->SetVelocity( EtVector3( 0.f, fVelocity, 0.f ) );
	//((CActionObject*)m_pSignal->GetParent()->GetParent())->SetResistance( EtVector3( 0.f, fResistance, 0.f ) );
}
