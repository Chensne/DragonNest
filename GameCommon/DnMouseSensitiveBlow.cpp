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
			// 0번인덱스는 시스템, 1번인덱스는 액션이니, 상태효과디버프는 2번으로 한다.
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

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnMouseSensitiveBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//값 계산
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
