#include "StdAfx.h"
#include "SCRShaderCustomParameter.h"
#include "ActionSignal.h"
#include "ActionObject.h"
#include "InputReceiver.h"
#include "RenderBase.h"
#include "resource.h"
#include "MainFrm.h"
#include "UserMessage.h"
#include "PaneDefine.h"


CSCRShaderCustomParameter::CSCRShaderCustomParameter( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
}

CSCRShaderCustomParameter::~CSCRShaderCustomParameter()
{
}

void CSCRShaderCustomParameter::Initialize()
{
	m_bActivate = true;
	OnModify();
}

void CSCRShaderCustomParameter::OnModify()
{
}

void CSCRShaderCustomParameter::OnSelect( bool bFirst )
{
}

void CSCRShaderCustomParameter::OnUnSelect()
{
}

void CSCRShaderCustomParameter::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	CActionObject *pObject = (CActionObject *)m_pSignal->GetParent()->GetParent();
	EtAniObjectHandle hHandle = pObject->GetObjectHandle();
	if( !hHandle ) return;

	int nIndex = hHandle->AddCustomParam( m_pSignal->GetProperty(0)->GetVariableString() );
	if( nIndex == -1 ) return;
	int nSubMeshIndex = m_pSignal->GetProperty(6)->GetVariableInt();

	switch( m_pSignal->GetProperty(1)->GetVariableInt() ) {
		case EPT_INT:
			{
				int nValue = m_pSignal->GetProperty(2)->GetVariableInt();
				hHandle->SetCustomParam( nIndex, (void*)&nValue, nSubMeshIndex );
			}
			break;
		case EPT_FLOAT:
			{
				float fValue = m_pSignal->GetProperty(3)->GetVariableFloat();
				hHandle->SetCustomParam( nIndex, (void*)&fValue, nSubMeshIndex );
			}
			break;
		case EPT_VECTOR:
			{
				EtVector3 vValue = m_pSignal->GetProperty(4)->GetVariableVector3();
				hHandle->SetCustomParam( nIndex, (void*)&vValue, nSubMeshIndex );
			}
			break;
		case EPT_TEX:
			break;
	}
}
