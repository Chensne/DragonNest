#include "StdAfx.h"
#include "DnFireBurnExplosionBlow.h"
#include "DnMonsterActor.h"
#include "EtActionSignal.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define FIREBURN_EXPLOSION_ACTION_NAME "Hit_Burn"

CDnFireBurnExplosionBlow::CDnFireBurnExplosionBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																								  m_pActionElement( NULL ),
																								  m_fFrame( 0.0f )
#ifdef _GAMESERVER
																								,m_iOriAttackMMin( 0 ),
																								 m_iOriAttackMMax( 0 )
#endif
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_116;

	SetValue( szValue );
	m_fValue = (float)atof(szValue);

	m_pActionElement = m_hActor->GetElement( FIREBURN_EXPLOSION_ACTION_NAME );
	_ASSERT( m_pActionElement && "ȭ�� ���� �׼� ������ ã�� �� ����." );

	m_ActionDurationTime = 0;
	if (m_pActionElement)
	{
		m_ActionDurationTime = m_pActionElement->dwLength;
		m_vlpSignalList = m_pActionElement->pVecSignalList;
	}
}

CDnFireBurnExplosionBlow::~CDnFireBurnExplosionBlow(void)
{

}

bool CDnFireBurnExplosionBlow::CanBegin( void )
{
	// ȭ�� ����ȿ���� �ɷ��ִ� ���°� �ƴ϶�� ���۵� �� ����.
	bool bCanBegin = m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_042 );

	if( false == bCanBegin )
		OutputDebug( "CDnFireBurnExplosionBlow::CanBegin - ȭ�� ����ȿ���� �ɷ����� �����Ƿ� ȭ�� ���� ����ȿ�� �ߵ� ����.\n" );

	// ���� �׼� ���� �� �ߵ���Ű�� �ʴ´�.
	if( NULL == m_pActionElement )
	{
		bCanBegin = false;
		OutputDebug( "CDnFireBurnExplosionBlow::CanBegin - ȭ�� ���� �׼��� �����Ƿ� ȭ�� ���� ����ȿ�� �ߵ� ����.\n" );
	}

	return bCanBegin;
}

void CDnFireBurnExplosionBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// ȭ�� ����ȿ�� ����
#ifdef _GAMESERVER
	m_hActor->CmdRemoveStateEffect( STATE_BLOW::BLOW_042 );
#endif


	// �� ����ȿ���� �����ִ� ��ų�� �������� ���� ���ݷ����� ����ȿ���� ����� ������ �������ݷ��� ���ӽð� ���� ��ü��Ŵ.
#ifdef _GAMESERVER
	_ASSERT( m_ParentSkillInfo.hSkillUser );
	if( !m_ParentSkillInfo.hSkillUser )
	{
		g_Log.Log( LogType::_ERROR, L"CDnFireBurnExplosionBlow::OnBegin - ȭ�� ���� ����ȿ�� ���Ͱ� ��ȿ���� �ʾƼ� �ߵ� ����..\n" );
		SetState( STATE_BLOW::STATE_END );
		return;
	}

	m_iOriAttackMMin = m_hActor->GetAttackMMin();
	m_iOriAttackMMax = m_hActor->GetAttackMMax();

	m_hActor->SetAttackMMin( m_ParentSkillInfo.hSkillUser->GetAttackMMin() );
	m_hActor->SetAttackMMax( m_ParentSkillInfo.hSkillUser->GetAttackMMax() );

	// ���Ͷ�� Ŭ�󿡼� ������ ǥ�ø� ���� ����.
	if( m_hActor && CDnActorState::Reserved6 < m_hActor->GetActorType() )
		static_cast<CDnMonsterActor*>(m_hActor.GetPointer())->SlaveOf( m_ParentSkillInfo.hSkillUser->GetUniqueID() );
#endif

	// ���ӽð��� ���� �׼��� ���ӽð����� ó���Ѵ�.
	float fFPS = 60.0f;
	//m_StateBlow.fDurationTime = (float)m_pActionElement->dwLength / fFPS;
	m_StateBlow.fDurationTime = m_ActionDurationTime != 0 ? (float)m_ActionDurationTime / fFPS : 0.0f;

	// �ٷ� ���ᰡ �� �� �����Ƿ� ���� �����϶��� �߰�
	if (STATE_BLOW::STATE_BEGIN == GetBlowState())
		m_hActor->AddStateBlowSignal(this->GetMySmartPtr());

	OutputDebug( "CDnFireBurnExplosionBlow::OnBegin\n" );
}


void CDnFireBurnExplosionBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

}


void CDnFireBurnExplosionBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	m_hActor->SetAttackMMin( m_iOriAttackMMin );
	m_hActor->SetAttackMMax( m_iOriAttackMMax );

	// ���Ͷ�� Ŭ�󿡼� ������ ǥ�ø� ���� ������ �� ���� ����.
	//if( CDnActorState::Reserved6 < m_hActor->GetActorType() )
	if( m_hActor && m_hActor->IsMonsterActor() )
	{
		DWORD dwInvalidMasterActorID = UINT_MAX;
		static_cast<CDnMonsterActor*>(m_hActor.GetPointer())->SlaveOf( dwInvalidMasterActorID );
	}
#endif

	m_hActor->RemoveStateBlowSignal(this->GetMySmartPtr());

	OutputDebug( "CDnFireBurnExplosionBlow::OnEnd\n" );
}

//#ifdef _GAMESERVER
//
//#endif


void CDnFireBurnExplosionBlow::SignalProcess( LOCAL_TIME LocalTime, float fDelta )
{
	// ���� �׼��� �������� �ʰ� �ñ׳ε鸸 �����Ű���� �Ѵ�.
	float fFPS = 60.0f;
	float fPrevFrame = m_fFrame;
	m_fFrame += (fDelta * fFPS);

	//if(m_pActionElement->dwLength < (DWORD)m_fFrame )
	if (m_ActionDurationTime < (DWORD)m_fFrame)
	{
		m_fFrame = 1.0f;
		fPrevFrame = -0.1f;

		// �׼��� �� �����ߴٸ� �� �̻� �ñ׳� ó���� ���� �ʰ� ����ȿ���� ������. 
		// �׼� ���� ��ŭ ���ӽð��� ���������Ƿ� �� ����ȿ�� �����.
		return;
	}

	if( 0.0f == fPrevFrame )
		fPrevFrame = -(m_fFrame * 0.1f);

	CEtActionSignal *pSignal = NULL;

#if defined(PRE_FIX_55378)
	m_hActor->SetChargerAction(FIREBURN_EXPLOSION_ACTION_NAME);
#endif // PRE_FIX_55378

#if defined(PRE_FIX_62309)
#if defined(_GAMESERVER)
	//#62309
	//�ñ׳� ���μ��� ���鼭 OnSignal�� Hit�� �Ӽ��� �������� ���� �� �ִ�.
	//�ñ׳� ���μ��� ȣ������ �����Ӽ��� ���� �� ����, Hit�ñ׳� ó���ÿ� ������ �Ӽ�������
	//�������� �ϵ��� �Ѵ�..
	m_hActor->PushForceHitElement(m_ParentSkillInfo.eSkillElement);
#endif // _GAMESERVER
#endif // PRE_FIX_62309

	//for( DWORD i=0; i < m_pActionElement->pVecSignalList.size(); i++ )
	for( DWORD i=0; i < m_vlpSignalList.size(); i++ )
	{
		//pSignal = m_pActionElement->pVecSignalList[i];
		pSignal = m_vlpSignalList[i];
		if( pSignal->CheckSignal( fPrevFrame, m_fFrame ) == true ) 
		{
			LOCAL_TIME StartTime = LocalTime - (LOCAL_TIME)( 1000.f / fFPS * ( m_fFrame - pSignal->GetStartFrame() ) );
			LOCAL_TIME EndTime = LocalTime + (LOCAL_TIME)( 1000.f / fFPS * ( pSignal->GetEndFrame() - m_fFrame ) );

			m_hActor->OnSignal( (SignalTypeEnum)pSignal->GetSignalIndex(), pSignal->GetData(), LocalTime, 
				StartTime, EndTime, pSignal->GetSignalListArrayIndex() );
		}
	}

#if defined(PRE_FIX_62309)
#if defined(_GAMESERVER)
	m_hActor->PopForceHitElement();
#endif // _GAMESERVER
#endif // PRE_FIX_62309

#if defined(PRE_FIX_55378)
	m_hActor->SetChargerAction(NULL);
#endif // PRE_FIX_55378
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFireBurnExplosionBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnFireBurnExplosionBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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