#include "StdAfx.h"
#include "DnDissolveBufBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDissolveBufBlow::CDnDissolveBufBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_083;
	SetValue( szValue );
	//m_nValue = atoi( szValue );
	m_fValue = (float)atof(szValue);
}

CDnDissolveBufBlow::~CDnDissolveBufBlow(void)
{
}


void CDnDissolveBufBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// ���� ������ ����ȿ�� ������ �ƴ϶� ����� ��ų ���� ����.
	multimap<int, DnBlowHandle> mmapBuffBlowsBySkill;

	int iDissolveCount = 0;
	int iNumAppliedBlow = m_hActor->GetNumAppliedStateBlow();
	vector<int> vlBuffSkillID;
	for( int iBlow = 0; iBlow < iNumAppliedBlow; ++iBlow )
	{
		DnBlowHandle hBlow = m_hActor->GetAppliedStateBlow( iBlow );
		if( !hBlow )
			continue;

		const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();

		// ���Ƿ� �־��ִ� ���� ����ȿ���� ���� ����� �ƴ�.
		if( hBlow->IsPermanent() )
			continue;

		if( pSkillInfo && pSkillInfo->iSkillID )
		{
			// ���� ����ȿ��. ������ ������ ��ų���� �����Ѵ�.
			// ��, �нú� ������ �Ǿ��ִ� �͵��� ����.
			if( (0 == pSkillInfo->iDissolvable) && 
				(CDnSkill::Passive != pSkillInfo->eSkillType ) &&
				(CDnSkill::Buff == pSkillInfo->eDurationType) )
			{
				// �ٿ� �����ۿ��� ����ϴ� Ư�� ��ų ȸ�� ����ȿ���� ���� ���� ����ȿ���� �������� �ʴ´�.
				// ���Ŀ� �Ϲ������� ���� ��� ���� ó�����־�� ��.
				if( hBlow->IsFromSourceItem() )
					continue;

				if( hBlow->IsEternity() )
					continue;

				mmapBuffBlowsBySkill.insert( make_pair(pSkillInfo->iSkillID, hBlow) );

				// �ϳ��� ��ų�� �������� ����ȿ���� ���� �� �ִ�. ����ũ�� ��ų ���̵� �־��ش�.
				if( vlBuffSkillID.end() == find( vlBuffSkillID.begin(), vlBuffSkillID.end(), pSkillInfo->iSkillID ) )
					vlBuffSkillID.push_back( pSkillInfo->iSkillID );
			}
		}
	}

	DNVector(int) vlAppliedBuffBlowIDs;
	for( int i = 0; i < (int)vlBuffSkillID.size(); ++i )
	{
		if( iDissolveCount >= (int)m_fValue )
			break;

		pair<multimap<int, DnBlowHandle>::iterator, multimap<int, DnBlowHandle>::iterator> iter_pair = 
			mmapBuffBlowsBySkill.equal_range( vlBuffSkillID.at(i) );

		// ���� ��ų���� ��Ե� ����� ����ȿ������ ��Ƶд�.
		multimap<int, DnBlowHandle>::iterator iter = iter_pair.first;
		for( iter; iter != iter_pair.second; ++iter )
			vlAppliedBuffBlowIDs.push_back( iter->second->GetBlowID() );

		++iDissolveCount;
	}


	// ���� ������ �нú� ��ų�� �ƴ� ���� ����ȿ���� ����
	int iNumBlow = (int)vlAppliedBuffBlowIDs.size();
	for( int iBlow = 0; iBlow < iNumBlow; ++iBlow )
	{
		int iBlowID = vlAppliedBuffBlowIDs.at( iBlow );
		m_hActor->RemoveStateBlowFromID( iBlowID );

		//// Note: ���ǵ� ����ȿ�� �ε����� �����ϹǷ� ���� Ÿ���� 1�� �̻��� �� �ִ�.
		//// ���� hBlow �� ���� �������� �̹� �����Ǿ��� ���� �����Ƿ� ��ȿ�� ������ �Ѵ�.
		//if( hBlow )
		//{
		//	m_hActor->RemoveStateBlowByBlowDefineIndex( hBlow->GetBlowIndex() );
		//}
	}

	OutputDebug( "CDnDissolveBufBlow::OnBegin, Value:%d \n", (int)m_fValue );
}


void CDnDissolveBufBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnDissolveBufBlow::OnEnd\n");
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnDissolveBufBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnDissolveBufBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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
