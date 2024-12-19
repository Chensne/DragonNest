#include "StdAfx.h"
#include "DnCameraRoarBlow.h"
#ifndef _GAMESERVER
#include "DnPlayerCamera.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCameraRoarBlow::CDnCameraRoarBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_151;

	SetValue( szValue );
	m_fValue = (float)atof(szValue);
}

CDnCameraRoarBlow::~CDnCameraRoarBlow(void)
{

}

void CDnCameraRoarBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifndef _GAMESERVER
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
		CDnPlayerCamera *pCamera = static_cast<CDnPlayerCamera *>(hCamera.GetPointer());
		if( pCamera && pCamera->GetAttachActor() == CDnActor::s_hLocalActor->GetMySmartPtr() ) {
			pCamera->SetCameraBlur( LocalTime, (DWORD)(m_StateBlow.fDurationTime * 1000.0f), m_fValue );
		}
	}
#endif

	OutputDebug( "CDnCameraRoarBlow::OnBegin\n" );
}


void CDnCameraRoarBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnCameraRoarBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

	OutputDebug( "CDnCameraRoarBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCameraRoarBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnCameraRoarBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
