#include "StdAfx.h"
#include "DnGhoulTransformBlow.h"
#include "DnPlayerActor.h"

#if defined( _GAMESERVER )
#include "DNPvPGameRoom.h"
#include "DNGameDataManager.h"
#endif

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnGhoulTransformBlow::CDnGhoulTransformBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_217;
	SetValue( szValue );
	m_vecTransformActorList.clear();
	SetTransformActorList();
}

CDnGhoulTransformBlow::~CDnGhoulTransformBlow(void)
{

}

void CDnGhoulTransformBlow::SetTransformActorList()
{
	std::string str = m_StateBlow.szValue;
	
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	TokenizeA(str, tokens, delimiters);

	std::vector<std::string>::iterator iter = tokens.begin();
	for ( ; iter != tokens.end(); ++iter)
		m_vecTransformActorList.push_back(atoi(iter->c_str()));
}

void CDnGhoulTransformBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	if(!m_hActor || !m_hActor->IsPlayerActor())
		return;

#if defined( _GAMESERVER )
	
	int nTransformActorSize = (int)m_vecTransformActorList.size();
	if(m_hActor && m_hActor->IsPlayerActor() && nTransformActorSize > 0)
	{
		CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		int iRandValue = (_rand(GetRoom())%(nTransformActorSize));

		if(iRandValue >= 0 && iRandValue < nTransformActorSize )
		{
			pPlayer->ToggleTransformMode( true, m_vecTransformActorList[iRandValue] , true ); // 이전에 변신중이라도 상관없이 변신 시킴.

			CDNGameRoom* pGameRoom = m_hActor->GetGameRoom();
			
			if(pGameRoom)
			{
				TMonsterMutationData Data;

				if(g_pDataManager->GetMonsterMutationData(m_vecTransformActorList[iRandValue], Data))
				{
					// 랜덤 사이즈
					_fpreset();
					int nScale = 100;
					int nMin = Data.nSizeMin;
					int nMax = Data.nSizeMax;
					if( nMin <= nMax ) {
						nScale = ( nMin + ( _rand(pGameRoom)%( ( nMax + 1 ) - nMin ) ) );
					}
					float fScale = nScale/100.f;
					m_hActor->SetScale( EtVector3( fScale, fScale, fScale ) );

					_ASSERT( pGameRoom->bIsPvPRoom() );
					static_cast<CDNPvPGameRoom*>(pGameRoom)->SendSelectZombie( m_hActor, Data.nMutationID , true, false , nScale );
				}
			}
		}
	}
#endif

}

void CDnGhoulTransformBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}


void CDnGhoulTransformBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnGhoulTransformBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{	
}

void CDnGhoulTransformBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{	
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
