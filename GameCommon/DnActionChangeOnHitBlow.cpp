#include "StdAfx.h"
#include "DnActionChangeOnHitBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnActionChangeOnHitBlow::CDnActionChangeOnHitBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_179;
	SetValue( szValue );

#if defined(PRE_FIX_55461)
	m_strActionNameToChange = "";
	m_strLimitAction = "";

	std::string str = szValue;//"��ȯ�ɵ���(;����Ʈ ����)";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. ����
	TokenizeA(str, tokens, delimiters);
	int nTokenSize = (int)tokens.size();
	if (nTokenSize == 1)
	{
		m_strActionNameToChange = tokens[0].c_str();
	}
	else if (nTokenSize == 2)
	{
		m_strActionNameToChange = tokens[0].c_str();
		m_strLimitAction = tokens[1].c_str();
	}
	else
	{
		OutputDebug("%s :: %s --> ����ȿ�� ���� ���� �ʿ�!!!\n", __FUNCTION__, szValue);
	}
	
#else
	m_strActionNameToChange = szValue;
#endif // PRE_FIX_55461

	AddCallBackType( SB_ONTARGETHIT );
}

CDnActionChangeOnHitBlow::~CDnActionChangeOnHitBlow(void)
{

}

void CDnActionChangeOnHitBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{

	OutputDebug( "CDnActionChangeOnHitBlow::OnBegin\n" );
}



void CDnActionChangeOnHitBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnActionChangeOnHitBlow::OnEnd\n" );
}

#ifdef _GAMESERVER
void CDnActionChangeOnHitBlow::OnTargetHit( DnActorHandle hTargetActor )
{
	if( m_strActionNameToChange.empty() )
		return;

#if defined(PRE_FIX_55461)
	if (m_strLimitAction.empty() == false)
	{
		//����Ʈ �׼��� ���� �Ǿ� �ְ�, �߻�ü�� ���� ��Ʈ�� �ȰŶ�� ��ŵ �ؾ� �ҵ�..
		CDnDamageBase::SHitParam *pTargetActorHitParam = hTargetActor ? hTargetActor->GetHitParam() : NULL;
		if (pTargetActorHitParam && pTargetActorHitParam->bFromProjectile == true)
			return;

		string strActorAction = m_hActor->GetCurrentAction();
		if (strcmp(m_strLimitAction.c_str(), strActorAction.c_str()) != 0)
		{
			OutputDebug("%s :: ���� ����(%s)�� ���� ���� ����(%s)�� ��ġ���� ����!!!!!\n", __FUNCTION__, m_strLimitAction.c_str(), strActorAction.c_str());
			return;
		}
	}
#endif // PRE_FIX_55461

	// SkillChain �ɼ��� ���ְ� �׼� ����.
	float fBlendFrame = 3.0f;
	bool bSkillChain = true;
	m_hActor->CmdAction( m_strActionNameToChange.c_str(), 0, fBlendFrame, true, false, bSkillChain );

	int iActionIndex = m_hActor->GetElementIndex( m_strActionNameToChange.c_str() );
	DWORD dwGap = 0;
	int iLoopCount = 0;
	char cMovePushKeyFlag = 0;
	bool bFromStateBlow = false;			// �켱 �ܼ��ϰ� �׼� �϶�� Ŭ��� �����ش�.
	
	// Ŭ���̾�Ʈ�� �׼� �����϶�� ��Ŷ ����.
	BYTE pBuffer[ 128 ] = { 0 };
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &iActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &dwGap, sizeof(DWORD) );
	Stream.Write( &iLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
	Stream.Write( m_hActor->GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &EtVec3toVec2( *m_hActor->GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &EtVec3toVec2( *m_hActor->GetLookDir() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &cMovePushKeyFlag, sizeof(char) );
	Stream.Write( &bFromStateBlow, sizeof(bFromStateBlow) );
	Stream.Write( &bSkillChain, sizeof(bSkillChain) );

	m_hActor->Send( eActor::CS_CMDACTION, &Stream );

	// �� ����ȿ���� ����ȴ�.
	SetState( STATE_BLOW::STATE_END );

	if( hTargetActor )
		hTargetActor->ResetDamageRemainTime();

	OutputDebug( "CDnActionChangeOnHitBlow::OnTargetHit\n" );
}
#endif // #ifdef _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnActionChangeOnHitBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnActionChangeOnHitBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
