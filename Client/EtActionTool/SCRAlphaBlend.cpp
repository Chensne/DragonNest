#include "StdAfx.h"
#include "SCRAlphaBlend.h"
#include "ActionSignal.h"
#include "ActionObject.h"
#include "InputReceiver.h"
#include "RenderBase.h"
#include "resource.h"
#include "MainFrm.h"
#include "UserMessage.h"
#include "PaneDefine.h"


CSCRAlphaBlend::CSCRAlphaBlend( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
}

CSCRAlphaBlend::~CSCRAlphaBlend()
{
}

void CSCRAlphaBlend::Initialize()
{
	m_bActivate = true;
	OnModify();
}

void CSCRAlphaBlend::OnModify()
{
}

void CSCRAlphaBlend::OnSelect( bool bFirst )
{
}

void CSCRAlphaBlend::OnUnSelect()
{
}

void CSCRAlphaBlend::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	float fStartAlpha = m_pSignal->GetProperty(0)->GetVariableFloat();
	float fEndAlpha = m_pSignal->GetProperty(1)->GetVariableFloat();

	CActionObject *pObject = (CActionObject *)m_pSignal->GetParent()->GetParent();
	EtAniObjectHandle hHandle = pObject->GetObjectHandle();
	if( !hHandle ) return;

	float fFrame = pObject->GetCurFrame();
	float fValue = 1.f / ( m_pSignal->GetEndFrame() - m_pSignal->GetStartFrame() ) * ( fFrame - m_pSignal->GetStartFrame() );
	float fCurAlpha = fStartAlpha + ( ( fEndAlpha - fStartAlpha ) * fValue );
	hHandle->SetObjectAlpha( fCurAlpha );
}
