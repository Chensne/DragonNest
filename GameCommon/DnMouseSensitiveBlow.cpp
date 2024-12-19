#include "StdAfx.h"
#include "DnMouseSensitiveBlow.h"
#ifndef _GAMESERVER
#include "DnPlayerCamera.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnMouseSensitiveBlow::CDnMouseSensitiveBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_152;

	SetValue( szValue );
	m_fValue = (float)atof(szValue);
}

CDnMouseSensitiveBlow::~CDnMouseSensitiveBlow(void)
{

}

void CDnMouseSensitiveBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifndef _GAMESERVER
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
		CDnPlayerCamera *pCamera = static_cast<CDnPlayerCamera *>(hCamera.GetPointer());
		if( pCamera && pCamera->GetAttachActor() == CDnActor::s_hLocalActor->GetMySmartPtr() ) {
			// 0���ε����� �ý���, 1���ε����� �׼��̴�, ����ȿ��������� 2������ �Ѵ�.
			pCamera->SetFreeze( m_fValue, 2 );
		}
	}
#endif

	OutputDebug( "CDnMouseSensitiveBlow::OnBegin\n" );
}


void CDnMouseSensitiveBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnMouseSensitiveBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifndef _GAMESERVER
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
		CDnPlayerCamera *pCamera = static_cast<CDnPlayerCamera *>(hCamera.GetPointer());
		if( pCamera && pCamera->GetAttachActor() == CDnActor::s_hLocalActor->GetMySmartPtr() ) {
			pCamera->SetFreeze( false, 2 );
		}
	}
#endif

	OutputDebug( "CDnMouseSensitiveBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnMouseSensitiveBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnMouseSensitiveBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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
