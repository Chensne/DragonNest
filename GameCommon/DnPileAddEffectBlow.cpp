#include "StdAfx.h"
#include "DnPileAddEffectBlow.h"
#include "DnTableDB.h"
#include "EtActionSignal.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

/*
�ش� ȿ���� ������ Ƚ����ŭ(��ų���̺� ����) ��ø�� ��� Ư�� �׼��� �����Ѵ�.
��ø �ɶ� ����Ʈ ������ �����Ѵ�.
Ư�� �׼� ����� �� ����ȿ���� ����� ������ ��ġ�� ��ų ����� ��ġ�� ��� �̵� ��Ŵ.
*/
CDnPileAddEffectBlow::CDnPileAddEffectBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_249;

	SetValue( szValue );
	m_fValue = 0.0f;
	
	m_szTargetAction = szValue;
	m_pActionElement = NULL;
	m_fFrame = 0.0f;
	m_ActionDurationTime = 0;

	m_bActivatedBlow = false;
}

CDnPileAddEffectBlow::~CDnPileAddEffectBlow(void)
{

}

void CDnPileAddEffectBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug( "%s\n", __FUNCTION__ );
}


void CDnPileAddEffectBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

}


void CDnPileAddEffectBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	m_hActor->RemoveStateBlowSignal(this->GetMySmartPtr());

	OutputDebug( "%s\n", __FUNCTION__);
}

void CDnPileAddEffectBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// �ð��� �Ѿ�� ������ ��ü��
	m_StateBlow.fDurationTime = StateBlowInfo.fDurationTime;

	//�ߺ��ɶ� ����Ʈ�� ������ ���� ��Ų��..
#ifndef _GAMESERVER
	std::string szAction = "Idle_";
	char buffer[65];
	_itoa_s(m_iDuplicateCount, buffer, 65, 10 );
	szAction += buffer;

	if (m_hEtcObjectEffect)
		m_hEtcObjectEffect->SetActionQueue(szAction.c_str());
#endif // _GAMESERVER

	//������ �ߺ� ī��Ʈ�� �Ǹ�
	DNTableFileFormat* pSkillTable = NULL;
	int iDuplicateCount = 0;
	pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	if (pSkillTable)
		iDuplicateCount = pSkillTable->GetFieldFromLablePtr( m_ParentSkillInfo.iSkillID, "_EffectAmassCount" )->GetInteger();

	if (iDuplicateCount != 0 && iDuplicateCount == m_iDuplicateCount)
	{
		//��ø ī��Ʈ ��ġ�� ���� �ϸ� Ȱ��ȭ �����Ѵ�.
		SetActivateBlow(true);

		m_hActor->AddStateBlowSignal(this->GetMySmartPtr());
	}
}

void CDnPileAddEffectBlow::OnSetParentSkillInfo()
{
	if (m_ParentSkillInfo.hSkillUser)
	{
		m_pActionElement = m_ParentSkillInfo.hSkillUser->GetElement( m_szTargetAction.c_str() );
		
		m_ActionDurationTime = 0;

		if (m_pActionElement)
		{
			m_ActionDurationTime = m_pActionElement->dwLength;
			m_vlpSignalList = m_pActionElement->pVecSignalList;
		}
	}
}

void CDnPileAddEffectBlow::SignalProcess( LOCAL_TIME LocalTime, float fDelta )
{
	if (IsEnd() == true)
		return;

	//#ifdef _GAMESERVER
	if( m_ParentSkillInfo.hSkillUser &&
		m_hActor )
	{
		if( m_pActionElement )
		{
			float fFPS = 60.0f;
			float fPrevFrame = m_fFrame - 1.0f;

			if( m_pActionElement->dwLength < (DWORD)m_fFrame )
			{
				m_fFrame = 1.0f;
				fPrevFrame = -0.1f;

				//������ �� ������..����ȿ�� ��??
				SetState(STATE_BLOW::STATE_END);

				return;
			}

			// ��ġ�� ���� ���ͷ� �ٲ㼭 �ñ׳ε����� �ٽ� ������Ŵ.
			MatrixEx OriginalCross = *(m_ParentSkillInfo.hSkillUser->GetMatEx());
			MatrixEx ChargerActorCross = *(m_hActor->GetMatEx());
			*(m_ParentSkillInfo.hSkillUser->GetMatEx()) = ChargerActorCross;

#ifdef _GAMESERVER
			m_ParentSkillInfo.hSkillUser->PushForceHitElement( m_ParentSkillInfo.eSkillElement );
#endif

			CEtActionSignal *pSignal = NULL;

#if defined(PRE_FIX_55378)
			m_ParentSkillInfo.hSkillUser->SetChargerAction(m_szTargetAction.c_str());
#endif // PRE_FIX_55378

			for( DWORD i=0; i < m_vlpSignalList.size(); i++ )
			{
				pSignal = m_vlpSignalList[i];
				if( pSignal->CheckSignal( fPrevFrame, m_fFrame ) == true ) 
				{
					LOCAL_TIME StartTime = LocalTime - (LOCAL_TIME)( 1000.f / fFPS * ( m_fFrame - pSignal->GetStartFrame() ) );
					LOCAL_TIME EndTime = LocalTime + (LOCAL_TIME)( 1000.f / fFPS * ( pSignal->GetEndFrame() - m_fFrame ) );

					m_ParentSkillInfo.hSkillUser->OnSignal( (SignalTypeEnum)pSignal->GetSignalIndex(), pSignal->GetData(), LocalTime, 
						StartTime, EndTime, pSignal->GetSignalListArrayIndex() );
				}
			}

#if defined(PRE_FIX_55378)
			m_ParentSkillInfo.hSkillUser->SetChargerAction(NULL);
#endif // PRE_FIX_55378

			*(m_ParentSkillInfo.hSkillUser->GetMatEx()) = OriginalCross;

#ifdef _GAMESERVER
			m_ParentSkillInfo.hSkillUser->PopForceHitElement();
#endif

			m_fFrame += (fDelta * fFPS);
		}
	}
}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnPileAddEffectBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnPileAddEffectBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW