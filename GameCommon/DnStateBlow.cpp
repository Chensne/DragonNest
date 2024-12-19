#include "StdAfx.h"
#include "DnStateBlow.h"
#include "DnCreateBlow.h"
#include "DnBlow.h"
#include "DnImmuneBlow.h"
#include "DnAllImmuneBlow.h"
#include "DnUsingSkillWhenDieBlow.h"
#include "DnTableDB.h"
#include "DnPlayerActor.h"
#include "DnProbInvincibleAtBlow.h"
#include "DnIgnoreEffectBlow.h"
#include "DnComboDamageLimitBlow.h"
#include "DnPileAddEffectBlow.h"
#ifdef PRE_ADD_DECREASE_EFFECT
#include "DnPuppetBlow.h"
#endif // PRE_ADD_DECREASE_EFFECT

#ifdef PRE_FIX_MEMOPT_EXT
#ifndef _GAMESERVER
#include "DnCommonUtil.h"
#endif
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifndef _GAMESERVER
std::map<int, EffectOutputInfo*> CDnStateBlow::s_nMapEffectOutputInfo;
bool CDnStateBlow::s_bStopProcess = false;
#else
STATIC_DECL_INIT( CDnStateBlow, bool, s_bStopProcess ) = { false, };
#endif

#ifndef _GAMESERVER
int CDnStateBlow::s_iStateBlowIDCount = 0;
#else
STATIC_DECL_INIT( CDnStateBlow, int, s_iStateBlowIDCount ) = { 0, };
#endif

CDnStateBlow::CDnStateBlow( DnActorHandle hActor )
{
	m_hActor = hActor;

	ZeroMemory( m_aiApplied, sizeof(m_aiApplied) );

#ifndef _GAMESERVER
	m_bAllowDiffuseVariation = true;
	_LoadStateEffectOutputInfo();
#endif

	m_bLockStateBlowList = false;
	m_bLockRemoveReservedList = false;
}

CDnStateBlow::~CDnStateBlow(void)
{
	DelAllStateBlow();
}

bool CDnStateBlow::InitializeClass()
{
#ifndef _GAMESERVER
	_LoadStateEffectOutputInfo();
#endif
	return true;
}

void CDnStateBlow::ReleaseClass()
{
#ifndef _GAMESERVER
	_ReleaseStateEffectOutputInfo();
#endif
}


#ifndef _GAMESERVER
void CDnStateBlow::_LoadStateEffectOutputInfo( void )
{
	if( !s_nMapEffectOutputInfo.empty() )
		return;

	DNTableFileFormat* pStateEffectTable = CDnTableDB::GetInstancePtr()->GetTable( CDnTableDB::TSTATEEFFECT );

#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat* pFileNameSox = CDnTableDB::GetInstancePtr()->GetTable(CDnTableDB::TFILE);
	if (pFileNameSox == NULL)
		return;
#endif
	
	int iNumItem = pStateEffectTable->GetItemCount();
	for( int i=0; i<iNumItem; i++ ) 
	{
		int iItem = pStateEffectTable->GetItemID(i);

		EffectOutputInfo* pNewInfo = new EffectOutputInfo;

		int iStateEffectIndex = pStateEffectTable->GetFieldFromLablePtr( iItem, "_StateEffectIndex" )->GetInteger(); 
		_ASSERT( pNewInfo->iStateEffectIndex < STATE_BLOW::BLOW_MAX );

		pNewInfo->iStateEffectIndex = iStateEffectIndex;
		pNewInfo->iShowTimingType = pStateEffectTable->GetFieldFromLablePtr( iItem, "_ShowTimingType" )->GetInteger();
		pNewInfo->iOutputType = pStateEffectTable->GetFieldFromLablePtr( iItem, "_EffectOutputType" )->GetInteger();
		pNewInfo->iPlayType = pStateEffectTable->GetFieldFromLablePtr( iItem, "_EffectPlayType" )->GetInteger();
#ifdef PRE_FIX_MEMOPT_EXT
		CommonUtil::GetFileNameFromFileEXT(pNewInfo->strSkinFileName, pStateEffectTable, iItem, "_EffectSkinFileName", pFileNameSox);
		CommonUtil::GetFileNameFromFileEXT(pNewInfo->strAniFileName, pStateEffectTable, iItem, "_EffectAniFileName", pFileNameSox);
		CommonUtil::GetFileNameFromFileEXT(pNewInfo->strActFileName, pStateEffectTable, iItem, "_EffectActFileName", pFileNameSox);
#else
		pNewInfo->strSkinFileName = pStateEffectTable->GetFieldFromLablePtr( iItem, "_EffectSkinFileName" )->GetString();
		pNewInfo->strAniFileName = pStateEffectTable->GetFieldFromLablePtr( iItem, "_EffectAniFileName" )->GetString();
		pNewInfo->strActFileName = pStateEffectTable->GetFieldFromLablePtr( iItem, "_EffectActFileName" )->GetString();
#endif
		pNewInfo->strActorActionName = pStateEffectTable->GetFieldFromLablePtr( iItem, "_ActorActionName" )->GetString();
		pNewInfo->iUseDiffuseVariationMethod = pStateEffectTable->GetFieldFromLablePtr( iItem, "_UseDiffuseVariation" )->GetInteger();
		pNewInfo->afDiffuseRed[ 0 ] = (float)pStateEffectTable->GetFieldFromLablePtr( iItem, "_DiffuseR1" )->GetFloat();
		pNewInfo->afDiffuseBlue[ 0 ] = (float)pStateEffectTable->GetFieldFromLablePtr( iItem, "_DiffuseG1" )->GetFloat();
		pNewInfo->afDiffuseGreen[ 0 ] = (float)pStateEffectTable->GetFieldFromLablePtr( iItem, "_DiffuseB1" )->GetFloat();
		pNewInfo->afDiffuseRed[ 1 ] = (float)pStateEffectTable->GetFieldFromLablePtr( iItem, "_DiffuseR2" )->GetFloat();
		pNewInfo->afDiffuseBlue[ 1 ] = (float)pStateEffectTable->GetFieldFromLablePtr( iItem, "_DiffuseG2" )->GetFloat();
		pNewInfo->afDiffuseGreen[ 1 ] = (float)pStateEffectTable->GetFieldFromLablePtr( iItem, "_DiffuseB2" )->GetFloat();
		pNewInfo->fDiffuseChangeSpeed = (float)pStateEffectTable->GetFieldFromLablePtr( iItem, "_DiffuseChangeSpeed" )->GetFloat();

		// ; 로 구분된 인덱스 값들을 받아옴
		const char* pIndices = pStateEffectTable->GetFieldFromLablePtr( iItem, "_LinkBoneIndex" )->GetString();
		string strIndices( pIndices );
		if( (int)strIndices.length() > 0 )
		{
			int nFoundPos = -1;
			if( strIndices.at(strIndices.length()-1) != ';' )
				strIndices.push_back( ';' );
			while( true )
			{
				int nStartPos = nFoundPos + 1; 
				nFoundPos = (int)strIndices.find_first_of( ';', nStartPos );
				if( nFoundPos != (int)string::npos )
				{
					string strIndex( strIndices.substr(nStartPos, nFoundPos-nStartPos) );
					pNewInfo->vlDummyBoneIndices.push_back( atoi(strIndex.c_str()) );
				}
				else
					break;
			} 
		}
		s_nMapEffectOutputInfo.insert( make_pair( iItem, pNewInfo ) );
	}
}

void CDnStateBlow::_ReleaseStateEffectOutputInfo( void )
{
	SAFE_DELETE_PMAP( TMapEffectOutputInfo, s_nMapEffectOutputInfo );
}

void CDnStateBlow::_ProcessDuplicateEffectBoneRotateShow( LOCAL_TIME LocalTime, float fDelta )
{
	// 프로세스를 하기 전에 유효하지 않은 이펙트는 제거.
	for( int i = 0; i < CDnActor::Max_FX_Dummy_Bone; ++i )
	{
		list<S_BONE_EFFECT>& listBoneEffect = m_aListBoneEffectStatus[ i ];

		list<S_BONE_EFFECT>::iterator iter = listBoneEffect.begin();
		for( iter; iter != listBoneEffect.end();  )
		{
			if( !iter->hEffect )
			{
				bool bNextShow = iter->bShow;
				iter = listBoneEffect.erase( iter );

				if( listBoneEffect.end() != iter )
					if( bNextShow )
						iter->Show( bNextShow );

				continue;
			}

			++iter;
		}
	}


	for( int i = 0; i < CDnActor::Max_FX_Dummy_Bone; ++i )
	{
		list<S_BONE_EFFECT>& listBoneEffect = m_aListBoneEffectStatus[ i ];

		// 복수개가 겹쳐있는 본은 교차출력 처리를 해준다.
		if( 1 < (int)listBoneEffect.size() )
		{
			list<S_BONE_EFFECT>::iterator iter = listBoneEffect.begin();
			for( iter; iter != listBoneEffect.end(); ++iter )
			{
				if( iter->bShow )
				{
					iter->fOutputElapsedTime += fDelta;

					if( 1.0f < iter->fOutputElapsedTime )
					{
						list<S_BONE_EFFECT>::iterator iterNext = iter;
						iterNext++;
						if( listBoneEffect.end() == iterNext )
							iterNext = listBoneEffect.begin();
						
						// 기존에 출력되던 이펙트는 숨기고.
						iter->Show( false );
						iter->fOutputElapsedTime = 0.0f;

						// 새로 보여줄 이펙트를 보여준다.
						iterNext->Show( true );
						iterNext->fOutputElapsedTime = 0.f;
						break;
					}
				}
			}
		}
	}
}
#endif


void CDnStateBlow::Process( LOCAL_TIME LocalTime, float fDelta, bool bForceInitialize )
{
#ifdef _GAMESERVER
	if( m_hActor && true == s_bStopProcess[ m_hActor->GetRoom()->GetRoomID() ] )
		return;
#else
	if( s_bStopProcess )
		return;
#endif

	if( !m_listBlowHandle.empty() ) 
	{
		BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

#ifndef _GAMESERVER
		if( m_bAllowDiffuseVariation )
		{
			// diffuse variation 숫자가 가장 높은 한 놈만 색깔 바뀌는 거 출력
			BLOW_HANDLE_LIST_ITER iterAllowDV = m_listBlowHandle.end();
			int iBiggest = 0;
			for( ; iter != m_listBlowHandle.end(); ++iter )
			{
				if( (*iter) && (*iter)->IsUseTableDefinedGraphicEffect() )
				{
					(*iter)->AllowDiffuseVariation( false );
					const EffectOutputInfo* pEffectInfo = (*iter)->GetEffectOutputInfo();
					if( pEffectInfo && pEffectInfo->iUseDiffuseVariationMethod > 0 )
					{
						if( iBiggest < pEffectInfo->iUseDiffuseVariationMethod )
						{
							iBiggest = pEffectInfo->iUseDiffuseVariationMethod;
							iterAllowDV = iter;
						}
					}
				}
			}

			if( m_listBlowHandle.end() != iterAllowDV )
			{
				(*iterAllowDV)->AllowDiffuseVariation( true );
			}
		}
#endif

#ifndef _FINAL_BUILD
		_CrashIfProcessListLocked();
#endif // #ifdef _FINAL_BUILD
		
		// 상태효과 객체들 루프돌면서 process 중에 리스트가 제거될 경우. 체크.
		// 파이널 릴리즈가 아닌경우 깔금하게 죽어라.
		m_bLockStateBlowList = true;

		Process_StateBlow_New(LocalTime, fDelta, bForceInitialize);
    }
	m_bLockStateBlowList = false;

#ifndef _GAMESERVER

#if !defined(SW_MODIFY_SE_EFFECTOUTPUT_20091119_jhk8211)
	_ProcessDuplicateEffectBoneRotateShow( LocalTime, fDelta );
#endif

#endif

#if defined( _GAMESERVER )
	// 여기서 StateBlow::Process() OnRebith() 가 불리게 되는 경우 내부적으로 자유롭게 StateBlow::Process() 를 돌릴 수가 없어서
	// 구조적으로 다 끝나고 후처리로 돌리게 바꿔놓는다.
	if( m_hActor )
		m_hActor->OnStateBlowProcessAfter();
#endif // #if defined( _GAMESERVER )

	RemoveStateBlowReservedList();
}



int CDnStateBlow::CanAddThisBlow( const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX BlowIndex )
{
	int iResult = ADD_SUCCESS;

	// #35335 무적 상태효과일 땐 화염, 독 상태효과가 적용되지 않음.
	if( IsApplied( STATE_BLOW::BLOW_099 ) )
	{
		if( (STATE_BLOW::BLOW_042 == BlowIndex) ||
			(STATE_BLOW::BLOW_044 == BlowIndex) )
		return ADD_FAIL_BY_INVINCIBLE;
	}

#ifdef _GAMESERVER
	// 226번 특정 스킬 확률 무적 상태효과가 해당 스킬에 대해 발동중인가.
	if( pParentSkill && IsApplied( STATE_BLOW::BLOW_226 ) )
	{
		DNVector( DnBlowHandle ) vlhBlows;
		GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_226, vlhBlows );
		
		for( int i = 0; i < (int)vlhBlows.size(); ++i )
		{
			bool bCanAdd = static_cast<CDnProbInvincibleAtBlow*>(vlhBlows.at(i).GetPointer())->CanAddThisSkillsStateBlow( pParentSkill->iSkillID );
			if( false == bCanAdd )
				return ADD_FAIL_BY_PROB_SKILL_INVINCIBLE;
		}
	}
#endif // #ifdef _GAMESERVER

	if (pParentSkill && pParentSkill->bIgnoreImmune)
		return iResult;

#if defined (_GAMESERVER) 
	//#34452번 이슈와 관련있음 
	bool bCheckGuildPriority = false;	
	if (pParentSkill && pParentSkill->bIsGuildSkill == false)
		bCheckGuildPriority = true;
#endif

	// 현재 면역된 상태효과인지 체크
	// 부활(무적) 상태효과인 경우 데미지 먹는 상태효과에 걸리지 않는다.
	list<DnBlowHandle>::iterator iter = m_listBlowHandle.begin();
	for( iter; iter != m_listBlowHandle.end(); )
	{
		DnBlowHandle hNowBlow = (*iter);
		if( hNowBlow )
		{
			if( (hNowBlow->GetBlowIndex() == STATE_BLOW::BLOW_077) )
			{
				bool bImmuned = static_cast<CDnImmuneBlow*>(hNowBlow.GetPointer())->IsImmuned( pParentSkill, BlowIndex );
				if( bImmuned )
					iResult = ADD_FAIL_BY_IMMUNE;
			}
			else if( (hNowBlow->GetBlowIndex() == STATE_BLOW::BLOW_150) )
			{
				bool bImmuned = static_cast<CDnAllImmuneBlow*>(hNowBlow.GetPointer())->IsImmuned( pParentSkill, BlowIndex );
				if( bImmuned )
					iResult = ADD_FAIL_BY_IMMUNE;
			}
			else if( IsApplied(STATE_BLOW::BLOW_057)  )
			{
				// 부활 중일때는 자기 자신이 사용한 스킬에서 자신에게 부여하는 상태효과.. 혹은, 버프/오라 스킬의 상태효과만 적용시킨다.
				if( pParentSkill )
				{
					CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(pParentSkill);
					if( pSkillInfo->hSkillUser )
					{
						if(	!(pSkillInfo->hSkillUser == m_hActor) &&
							(CDnSkill::Buff != pSkillInfo->eDurationType) && 
							(CDnSkill::Aura != pSkillInfo->eDurationType) &&
							(CDnSkill::StanceChange != pSkillInfo->eDurationType) )
						{
							iResult = ADD_FAIL_BY_REVIVAL;
						}
					}
				}
			}
			else if (hNowBlow->GetBlowIndex() == STATE_BLOW::BLOW_252)
			{
				//#53722 특정 스킬을 면역하는 스킬효과 개발(hit는 되지만 상태효과 추가는 되지 않도록..)
				CDnIngnoreEffectBlow* pIgnoreEffectBlow = static_cast<CDnIngnoreEffectBlow*>(hNowBlow.GetPointer());
				if (pIgnoreEffectBlow && pParentSkill && pIgnoreEffectBlow->IsInvincibleAt(pParentSkill->iSkillID))
					iResult = ADD_FAIL_BY_IMMUNE;
			}
#if defined(_GAMESERVER)
			else if ( hNowBlow->GetBlowIndex() == STATE_BLOW::BLOW_242 && BlowIndex == STATE_BLOW::BLOW_242)
			{
				//242번 상태효과는 같은 스킬 ID이고, 같은 유저 인 경우는 더이상 추가 되지 않도록 한다.
				if (pParentSkill)
				{
					CDnSkill::SkillInfo* pParentSkillInfo = const_cast<CDnSkill::SkillInfo*>(pParentSkill);
					CDnSkill::SkillInfo* pExistSkillInfo = const_cast<CDnSkill::SkillInfo*>(hNowBlow->GetParentSkillInfo());

					// #56880 스킬 시작 시간이 같은지도 확인 해야 한다..
					LOCAL_TIME parentSkillStartTime = 0;
					LOCAL_TIME nowBlowSkillStartTime = 0;

					CDnComboDamageLimitBlow* pNowBlow = static_cast<CDnComboDamageLimitBlow*>(hNowBlow.GetPointer());
					if (pNowBlow)
						nowBlowSkillStartTime = pNowBlow->GetSkillStartTime();

					DnSkillHandle hParentSkill;
					if (pParentSkillInfo->hSkillUser)
						hParentSkill = pParentSkillInfo->hSkillUser->FindSkill(pParentSkillInfo->iSkillID);
					
					if (hParentSkill)
						parentSkillStartTime = pParentSkillInfo->projectileSkillStartTime != 0 ? pParentSkillInfo->projectileSkillStartTime : hParentSkill->GetSkillStartTime();

					if (pExistSkillInfo)
					{
						if (pParentSkillInfo->hSkillUser && 
							pParentSkillInfo->hSkillUser == pExistSkillInfo->hSkillUser &&	//스킬 사용자가 같고
							pParentSkillInfo->iSkillID == pExistSkillInfo->iSkillID &&		//스킬이 같은경우
							nowBlowSkillStartTime == parentSkillStartTime					//스킬 시작 시간이 같은 경우
							)
						{
							iResult = ADD_FAIL_BY_COMBOLIMITBLOW;
						}
					}
				}
			}
			else if (hNowBlow->GetBlowIndex() == STATE_BLOW::BLOW_249)
			{
				//249번 상태효과 중첩 되고 활성화?(중첩카운트 설정 수치에 도달 했을 경우) 더이상 추가 되지 않도록한다..
				CDnPileAddEffectBlow* pPileAddEffectBlow = static_cast<CDnPileAddEffectBlow*>(hNowBlow.GetPointer());
				if (pPileAddEffectBlow && pPileAddEffectBlow->IsActivatedBlow())
					iResult = ADD_FAIL_BY_COMBOLIMITBLOW;	//따로 열거형 추가 안하고 그냥 이 녀석으로 사용함..
			}
#endif // _GAMESERVER


#if defined (_GAMESERVER)
			if (bCheckGuildPriority && hNowBlow->GetBlowIndex() == BlowIndex)
			{
				const CDnSkill::SkillInfo * pNowSkillInfo = static_cast<CDnBlow*>(hNowBlow.GetPointer())->GetParentSkillInfo();
				if (pNowSkillInfo->eDurationType == pParentSkill->eDurationType && pNowSkillInfo->bIsGuildSkill)
				{
					//같은 블로우 효과가 있는데 길드스킬로 사용된것이라면 사용불가!
					iResult = ADD_FAIL_BY_GUILDBLOW_PRIORITY;
				}
			}
#endif
		}

		++iter;
	}

	return iResult;
}

void CDnStateBlow::_CheckImmediatelyBegin( DnBlowHandle hBlow )
{
	if( STATE_BLOW::BLOW_025 == hBlow->GetBlowIndex() )
	{
		// 팀 반전 같은 상태효과도 있어서 팀이 반전되면 유혹 상태효과 이펙트가 
		// 몹과 같은 팀이 되어 붙질 않으므로 먼저 이펙트를 처리한다. #14128
#ifndef _GAMESERVER
		if( hBlow->IsUseTableDefinedGraphicEffect() )
			hBlow->AttachGraphicEffectDefaultType();
#else
		hBlow->CheckAndStartActorActionInEffectInfo();
#endif
		hBlow->OnBegin( 0, 0.0f );
		m_hActor->OnBeginStateBlow( hBlow );

		// 곧바로 end 로 셋팅하는 blow 도 있기 때문에 체크.
		if( STATE_BLOW::STATE_END != hBlow->GetBlowState() )
			hBlow->SetState( STATE_BLOW::STATE_DURATION );
	}
}


int CDnStateBlow::AddStateBlow( DnBlowHandle hBlow )
{
	ASSERT(hBlow&&"CDnStateBlow::AddStateBlow");
	if( !hBlow ) return -1;
	
	if( false == hBlow->IsDuplicated() )
		hBlow->SetBlowID( GenerateStateBlowID() );
	
#ifndef _GAMESERVER
	if( m_bAllowDiffuseVariation )
	{
		const CDnSkill::SkillInfo* pParentSkillInfo = hBlow->GetParentSkillInfo();
		std::string effectOutputIDs = pParentSkillInfo ? pParentSkillInfo->effectOutputIDs : "";

#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF // 해당스킬이 적에게 영향을 미치는경우이며 , Debuff가 설정이 된경우에는 설정된 디버프 정보로 셋팅해준다.
		
		if(pParentSkillInfo && pParentSkillInfo->iSkillUserTeam != hBlow->GetActorHandle()->GetTeam())
		{
			//Debuff 이펙트 설정이 있는지 확인 해서...
			std::vector<std::string> infoTokens;
			std::string delimiters = ";";

			bool isDebuggEffect = false;
			TokenizeA(pParentSkillInfo->debuffEffectOutputIDs, infoTokens, delimiters);
			int nTokenSize = (int)infoTokens.size();
			for (int i = 0; i < nTokenSize; ++i)
			{
				int nEffectOutputID = atoi(infoTokens[i].c_str());
				if (nEffectOutputID != 0)
				{
					isDebuggEffect = true;
					break;
				}
			}

			//정상적인 값이 있는 경우.. debuff이펙트 설정으로 변경한다.
			if (isDebuggEffect)
				effectOutputIDs = pParentSkillInfo->debuffEffectOutputIDs;
		}
#endif

		EffectOutputInfo *pEffectInfo = NULL;

		std::vector<std::string> infoTokens;
		std::string delimiters = ";";

		TokenizeA(effectOutputIDs, infoTokens, delimiters);
		int nTokenSize = (int)infoTokens.size();
		for (int i = 0; i < nTokenSize; ++i)
		{
			int nEffectOutputID = atoi(infoTokens[i].c_str());
			pEffectInfo = GetEffectOutputInfo(nEffectOutputID);
			if (pEffectInfo && hBlow->IsMatchStateEffectIndex(pEffectInfo->iStateEffectIndex))
			{
				hBlow->SetEffectOutputInfo(pEffectInfo);
			}
		}
	}
#endif

	++m_aiApplied[ hBlow->GetBlowIndex() ];

	m_listBlowHandle.push_back( hBlow );

	_CheckImmediatelyBegin( hBlow );

	return hBlow->GetBlowID();
}

int CDnStateBlow::RemoveStateBlowByBlowDefineIndex( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	// 해당 BlowID를 넘겨 받아서 처리 하는 부분이 없음. 그래서 -1로 리턴하도록하는데.. 어떨지...[2010/12/14 semozz]
	AddRemoveStateBlowInfo(RemoveStateBlowInfo(RemoveStateBlowInfo::RSBI_BLOW_INDEX, emBlowIndex));
	return -1;
}



void CDnStateBlow::GetStateBlowFromBlowIndex( STATE_BLOW::emBLOW_INDEX emBlowIndex, /*IN OUT*/ DNVector(DnBlowHandle)& vlhResult )
{
	int nIndex = -1;
	DnBlowHandle hFindedBlow;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

	for( ; iter != m_listBlowHandle.end(); )
	{
		DnBlowHandle hBlow = (*iter);

		if( hBlow && (hBlow->GetBlowIndex() == emBlowIndex) )
		{
			vlhResult.push_back( hBlow );
		}

		++iter;
	}
}

bool CDnStateBlow::IsExistStateBlowFromBlowIndex( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

	for( ; iter != m_listBlowHandle.end(); ++iter)
	{
		if( (*iter) && ((*iter)->GetBlowIndex() == emBlowIndex) )
			return true;
	}

	return false;
}

bool CDnStateBlow::IsExistStateBlowFromBlowID( int nBlowID )
{
	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();
	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		if( (*iter) && ((*iter)->GetBlowID() == nBlowID) )
			return true;
	}
	return false;
}

DnBlowHandle CDnStateBlow::GetStateBlowFromID( int nStateBlowID )
{
	DnBlowHandle hFindedBlow;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		DnBlowHandle hBlow = (*iter);

		if( hBlow && (hBlow->GetBlowID() == nStateBlowID) )
		{
			hFindedBlow = hBlow;
			break;
		}
	}

	return hFindedBlow;
}

#ifndef _GAMESERVER
DnBlowHandle CDnStateBlow::GetStateBlowFromServerID( int nServerID )
{
	DnBlowHandle hFindedBlow;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		DnBlowHandle hBlow = (*iter);
		if( hBlow && (hBlow->GetServerBlowID() == nServerID) )
		{
			hFindedBlow = hBlow;
			break;
		}
	}

	return hFindedBlow;
}
#endif


void CDnStateBlow::RemoveStateBlowFromID( int nStateBlowID )
{
	AddRemoveStateBlowInfo(RemoveStateBlowInfo(RemoveStateBlowInfo::RSBI_BLOW_ID, nStateBlowID));
}

DnBlowHandle CDnStateBlow::CreateStateBlow( DnActorHandle hActor, const CDnSkill::SkillInfo* pParentSkill, 
											STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam )
{
	DnBlowHandle hBlow;

	//242번 상태효과는 지속 시간을 무한대로? 자동 설정 하도록 한다..
	//상태효과 자체에서 Process함수에서 조건에 맞으면 자동 종료 하도록 한다(서버쪽에서만 동작)
	if (emBlowIndex == STATE_BLOW::BLOW_242)
		nDurationTime = -1;

	// 이슈 6190 관련.
	//중첩
	//	효과 중첩 개수가 1 이상인 스킬의 경우 레벨의 같은 스킬이 최대 중첩개수까지 중복되어 적용된다.
	//	효과 중첩 개수가 1 이상이나 스킬의 레벨이 틀릴 경우에는 별도로 적용한다.
	//	최대 중첩개수까지 적용되었을때 중첩이 되는 조건의 효과가 추가로 적용되면 지속시간만 다시 초기화 된다.
	// 접두어 스킬의 상태효과인 경우 중첩 처리를 하지 않는다.
	bool bItemPrefixSkill = false;
	if( pParentSkill && pParentSkill->bItemPrefixSkill )
		bItemPrefixSkill = true;

	bool bDuplicated = false;


	float fReduceTimeValue = 1.0f;
	if (pParentSkill && pParentSkill->eDurationType != CDnSkill::DurationTypeEnum::Instantly)
	{
		fReduceTimeValue = GetImmuneReduceTimeValue(emBlowIndex);
		nDurationTime = (int)((float)nDurationTime * fReduceTimeValue);

#ifdef PRE_ADD_DECREASE_EFFECT
#ifdef _GAMESERVER
		if( hActor && emBlowIndex != STATE_BLOW::BLOW_247 && fReduceTimeValue != 1.0f )
		{
			hActor->SendAddSEFail( CDnStateBlow::ADD_DECREASE_EFFECT_BY_IMMUNE, emBlowIndex );
		}
#endif // _GAMESERVER
#endif // PRE_ADD_DECREASE_EFFECT
	}

	//#56880 242번 상태효과는 중첩 처리 안되어야 한다...
	//#52905 253번 상태효과는 중첩 처리 안되어야 한다...
	if( emBlowIndex != STATE_BLOW::BLOW_242 &&
		emBlowIndex != STATE_BLOW::BLOW_253 &&
		false == bItemPrefixSkill && 
		0 < m_aiApplied[ emBlowIndex ] )
	{
		DNVector(DnBlowHandle) vlhResult;
		GetStateBlowFromBlowIndex( emBlowIndex, vlhResult );
		_ASSERT( !vlhResult.empty() );

		int iNumBlow = (int)vlhResult.size();
		for( int iBlow = 0; iBlow < iNumBlow; ++iBlow )
		{
			DnBlowHandle hExistBlow = vlhResult.at( iBlow );

			if( STATE_BLOW::STATE_END != hExistBlow->GetBlowState() )
			{
				const CDnSkill::SkillInfo* pExistParentSkillInfo = hExistBlow->GetParentSkillInfo();
				if( pExistParentSkillInfo && pParentSkill )
				{
					// 스킬과 레벨이 같은경우는 효과를 중첩시킵니다.
					bool bLevelCheck = pExistParentSkillInfo->iSkillLevelID == pParentSkill->iSkillLevelID;
					bool bIndexCheck = pExistParentSkillInfo->iSkillID == pParentSkill->iSkillID;

					// #65533  PileAddEffect 같은경우는 서로 다른 레벨에 의해 생성되어도 중첩시키도록 설정합니다.
					if( emBlowIndex == STATE_BLOW::BLOW_249 ) 
						bLevelCheck = true;

					if( bLevelCheck && bIndexCheck )
					{
						STATE_BLOW BlowInfo;
						BlowInfo.emBlowIndex = emBlowIndex;

						if( nDurationTime != -1 )
							BlowInfo.fDurationTime = (float)nDurationTime * 0.001f;
						else
							BlowInfo.fDurationTime = 0.0f;

						BlowInfo.szValue = szParam;

						//#53448 중첩시 최종 타격자로 SkillUser를 변경 해줘야 한다.
						if (emBlowIndex == STATE_BLOW::BLOW_249)
							hExistBlow->SetParentSkillInfo(pParentSkill);

						hExistBlow->Duplicate( BlowInfo );

						hBlow = hExistBlow;
						bDuplicated = true;

						// Remove List 에 있는 거 빼줍시다.
						REMOVE_STATEBLOW_LIST::iterator removeInfoiter = m_RemoveStateBlowList.begin();
						REMOVE_STATEBLOW_LIST::iterator removeInfoEndIter = m_RemoveStateBlowList.end();

						for( REMOVE_STATEBLOW_LIST::iterator it = m_RemoveStateBlowList.begin(); it != m_RemoveStateBlowList.end(); it++ ) {
							if( it->m_Type == RemoveStateBlowInfo::RSBI_BLOW_ID && hExistBlow->GetBlowID() == it->m_Value ) {
								m_RemoveStateBlowList.erase( it );
								break;
							}
						}
					}
				}
			}
		}
	}
	
	if( false == bDuplicated )
	{	
		static CDnCreateBlow createBlow;
#ifdef PRE_MOD_MAGICALBREEZE_CHANGE_BLOW
		STATE_BLOW::emBLOW_INDEX emOriginalBlowIndex = emBlowIndex;
		emBlowIndex = CheckMagicalBreezeChangeBlow( pParentSkill, emBlowIndex, hActor );
		hBlow = createBlow.CreateBlow( emBlowIndex, hActor, szParam );
		if( !hBlow ) return hBlow;
		hBlow->SetOriginalBlowIndex( emOriginalBlowIndex );
#else // PRE_MOD_MAGICALBREEZE_CHANGE_BLOW
		hBlow = createBlow.CreateBlow( emBlowIndex, hActor, szParam );
		if( !hBlow ) return hBlow;
#endif // PRE_MOD_MAGICALBREEZE_CHANGE_BLOW

		if( nDurationTime == -1 ) 
			hBlow->SetPermanent( true );
		else 
			hBlow->SetDurationTime( nDurationTime * 0.001f );

#ifdef PRE_ADD_DECREASE_EFFECT
#ifdef _GAMESERVER
		if( emBlowIndex == STATE_BLOW::BLOW_247 && fReduceTimeValue != 1.0f )
		{
			CDnPuppetBlow* pDnPuppetBlow = dynamic_cast<CDnPuppetBlow*>( hBlow.GetPointer() );
			if( pDnPuppetBlow )
				pDnPuppetBlow->SetShowReduce( true );
		}
#endif // _GAMESERVER
#endif // PRE_ADD_DECREASE_EFFECT
	}

	return hBlow;
}

#ifdef PRE_MOD_MAGICALBREEZE_CHANGE_BLOW
STATE_BLOW::emBLOW_INDEX CDnStateBlow::CheckMagicalBreezeChangeBlow( const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX emBlowIndex, DnActorHandle hActor )
{
	STATE_BLOW::emBLOW_INDEX emRtnBlowIndex = emBlowIndex;
#ifdef _GAMESERVER
	if( hActor == NULL ) return emRtnBlowIndex;
	if( pParentSkill == NULL ) return emRtnBlowIndex;
	if( pParentSkill->eApplyType != CDnSkill::StateEffectApplyType::ApplySelf || pParentSkill->eSkillType != CDnSkill::SkillTypeEnum::Active ) return emRtnBlowIndex;
	if( pParentSkill->bIsItemSkill ) return emRtnBlowIndex;

	if( hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_180 ) )
	{
		switch( emBlowIndex )
		{
		case STATE_BLOW::BLOW_001:	// 물리 공격력 절대값 변경 -> 마법 공격력 절대값 변경
			emRtnBlowIndex = STATE_BLOW::BLOW_028;
			break;
		case STATE_BLOW::BLOW_002:	// 물리 공격력 절대값 변경 -> 마법 공격력 절대값 변경
			emRtnBlowIndex = STATE_BLOW::BLOW_029;
			break;
		case STATE_BLOW::BLOW_200:	// 물리 공격력 절대값 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
			emRtnBlowIndex = STATE_BLOW::BLOW_202;	// ->마법 공격력 절대값 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
			break;
		case STATE_BLOW::BLOW_201:	// 물리 공격력 비율 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
			emRtnBlowIndex = STATE_BLOW::BLOW_203;	// ->마법 공격력 비율 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
			break;
		}
	}
#endif // _GAMESERVER
	return emRtnBlowIndex;
}
#endif // PRE_MOD_MAGICALBREEZE_CHANGE_BLOW


int CDnStateBlow::GenerateStateBlowID()
{
#ifdef _GAMESERVER
	s_iStateBlowIDCount[ m_hActor->GetRoom()->GetRoomID() ]++;

	if( s_iStateBlowIDCount[ m_hActor->GetRoom()->GetRoomID() ] >= INT_MAX )
		s_iStateBlowIDCount[ m_hActor->GetRoom()->GetRoomID() ] = 0;

	return s_iStateBlowIDCount[ m_hActor->GetRoom()->GetRoomID() ];
#else
	s_iStateBlowIDCount++;

	if( s_iStateBlowIDCount >= INT_MAX )
		s_iStateBlowIDCount = 0;

	return s_iStateBlowIDCount;
#endif
}


void CDnStateBlow::OnChangedWeapon()
{
	DnBlowHandle hBlow;

	m_bLockStateBlowList = true;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();
	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		hBlow = (*iter);

		if( hBlow && (hBlow->GetCallBackType()&SB_ONCHANGEDWEAPON) )
		{
			hBlow->OnChangedWeapon();
		}
	}

	m_bLockStateBlowList = false;
}

bool CDnStateBlow::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	DnBlowHandle hBlow;
	bool bRetDefense(bHitSuccess);

	m_bLockStateBlowList = true;
	BLOW_HANDLE_LIST_ITER iter = m_listBlowHandle.begin();
	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		hBlow = (*iter);

		if( hBlow && (hBlow->GetCallBackType()&SB_ONDEFENSEATTACK) )
		{
			if( hBlow->OnDefenseAttack( hHitter, pAttackerState, HitParam, bHitSuccess ) )
			{
				bRetDefense = false;
			}
		}
	}

	if( bRetDefense )
	{
		iter = m_listBlowHandle.begin();
		for( ; iter != m_listBlowHandle.end(); ++iter )
		{
			hBlow = (*iter);

			if( hBlow && (hBlow->GetCallBackType()&SB_AFTERONDEFENSEATTACK) )
			{
				if( hBlow->OnDefenseAttack( hHitter, pAttackerState, HitParam, bHitSuccess ) )
				{
					bRetDefense = false;
				}
			}
		}
	}

	m_bLockStateBlowList = false;

	return bRetDefense;
}

int CDnStateBlow::OnUseMP( int iMPDelta )
{
	int iResult = iMPDelta;
	DnBlowHandle hBlow;

	m_bLockStateBlowList = true;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

	int iDelta = 0;
	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		hBlow = (*iter);

		if( hBlow && (hBlow->GetCallBackType()&SB_ONUSEMP) )
			iDelta += hBlow->OnUseMP( iMPDelta );
	}

	m_bLockStateBlowList = false;

	iResult += iDelta;

	return iResult;
}


void CDnStateBlow::DelAllStateBlow()
{
	DnBlowHandle hBlow;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		hBlow = (*iter);
		if( hBlow )
		{
			if( hBlow->IsDuration() )
			{
				if( !m_hActor )
					g_Log.Log(LogType::_ROOMCRASH, L"[DelAllStateBlow] Invalid ActorHandle\n" );
			}

			SAFE_RELEASE_SPTR( hBlow );
		}
	}

	m_listBlowHandle.clear();
}


DnBlowHandle CDnStateBlow::GetStateBlow( int iIndex )
{
	BLOW_HANDLE_LIST_ITER iter = m_listBlowHandle.begin();

	for( int i = 0; i < iIndex; ++i )
		++iter;

	return *iter;
}



float CDnStateBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	float fResult = 0.0f;
	DnBlowHandle hBlow;

	DnBlowHandle hHighLanderBlow;
	DnBlowHandle hProbInvincibleAtBlow;

	float fStateBlowResult = 0.0f;
	float fHighLanderResult = 0.0f;
	float fProbInvincibleAtBlow = 0.0f;

	m_bLockStateBlowList = true;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();
	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		hBlow = (*iter);
		if( hBlow && (hBlow->GetCallBackType()&SB_ONCALCDAMAGE) )
		{
			fStateBlowResult = hBlow->OnCalcDamage( fOriginalDamage, HitParam );

			if( STATE_BLOW::BLOW_143 == hBlow->GetBlowIndex() )
			{
				hHighLanderBlow = hBlow;
				fHighLanderResult = fStateBlowResult;
			}
			else
			if( STATE_BLOW::BLOW_226 == hBlow->GetBlowIndex() )
			{
				hProbInvincibleAtBlow = hBlow;
				fProbInvincibleAtBlow = fStateBlowResult;
			}

			fResult += fStateBlowResult;
		}
	}

	m_bLockStateBlowList = false;

	// 하이랜더, 특정 스킬 확률 무시 상태효과가 있다면 해당 결과값이 우선. (데미지 없음)
	if( hHighLanderBlow )
		fResult = fHighLanderResult;
	else 	
	if( hProbInvincibleAtBlow && fProbInvincibleAtBlow != 0.0f)	//데미지 보정값이 있을 경우만.
		fResult = fProbInvincibleAtBlow;
	
	return fResult;
}


bool CDnStateBlow::OnDie( DnActorHandle hHitter )
{
	bool bProcessed = false;

	// 현재는 죽을 때 스킬 쓰는 상태효과 밖에 없으므로 루프 돌지 않고 직접 지목해서 처리.
	if( IsApplied( STATE_BLOW::BLOW_137 ) )
	{
		BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();
		for( ; iter != m_listBlowHandle.end(); ++iter )
		{
			DnBlowHandle hBlow = (*iter);
			if( hBlow && hBlow->GetBlowIndex() == STATE_BLOW::BLOW_137 )
			{
				static_cast<CDnUsingSkillWhenDieBlow*>(hBlow.GetPointer())->OnDie();

#if defined(PRE_FIX_44884)
				//UsingSkillWhenDieBlow가 설정된 액터는 OnDie호출시 HP를 1로 설정되어 OnDie호출 시점을 상태효과 처리로 미룬다.
				//실제 Hitter를 알 수 없는 상황이라 여기에서 저장 해놓는다.
				static_cast<CDnUsingSkillWhenDieBlow*>(hBlow.GetPointer())->SetFinalHitterActor(hHitter);
#endif // PRE_FIX_44884

				bProcessed = true;
			}
		}
	}

	return bProcessed;
}



void CDnStateBlow::OnTargetHit( DnActorHandle hTargetActor )
{
	DnBlowHandle hBlow;

	m_bLockStateBlowList = true;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();
	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		hBlow = (*iter);
		if( hBlow && (hBlow->GetCallBackType()&SB_ONTARGETHIT) )
		{
			hBlow->OnTargetHit( hTargetActor );
		}
	}

	m_bLockStateBlowList = false;
}


void CDnStateBlow::ResetStateBlowBySkillType( int nSkillDurationType )
{
	AddRemoveStateBlowInfo(RemoveStateBlowInfo(RemoveStateBlowInfo::RSBI_SKILL_DURATION_TYPE, nSkillDurationType));
}

#ifndef _GAMESERVER
void CDnStateBlow::RestoreAllBlowGraphicEffect()
{
	BLOW_HANDLE_LIST_ITER iter = m_listBlowHandle.begin();
	
	for( ; iter != m_listBlowHandle.end(); ++iter) {
		DnBlowHandle hBlow = *iter;
		if( !hBlow ) continue;
		if( hBlow->IsUseTableDefinedGraphicEffect() ) {
			hBlow->AttachGraphicEffectDefaultType();
		}
	}
}

EffectOutputInfo *CDnStateBlow::GetEffectOutputInfo( int nItemID )
{
	std::map<int, EffectOutputInfo*>::iterator it = s_nMapEffectOutputInfo.find( nItemID );
	if( it == s_nMapEffectOutputInfo.end() ) return NULL;
	return it->second;
}

#ifndef _GAMESERVER
void CDnStateBlow::RemoveStateBlowFromServerID( int nServerBlowID )
{
	AddRemoveStateBlowInfo(RemoveStateBlowInfo(RemoveStateBlowInfo::RSBI_BLOW_SERVER_ID, nServerBlowID));
}
#endif

#if !defined(SW_MODIFY_SE_EFFECTOUTPUT_20091119_jhk8211)
void CDnStateBlow::AddEffectAttachedBone( int iBone, DnEtcHandle hEffectHandle )
{
	// 현재 0 번 본을 제외하고 중첩 교차 출력 하지 않음. 그냥 다 보여줌. #16291
	if( CDnActor::FXDummyBoneEnum::FX_01 != iBone )
		return;

	// 루프 돌면서 겹치는 본은 일정 시간마다 변경해서 뿌려주자.
	_ASSERT( 0 <= iBone && iBone < CDnActor::Max_FX_Dummy_Bone );
	if( 0 <= iBone && iBone < CDnActor::Max_FX_Dummy_Bone )
	{
		S_BONE_EFFECT BoneEffect;
		BoneEffect.hEffect = hEffectHandle;

		// 처음 추가되는 것이라면 show 처리.
		// show( false ) 가 렌더링 되고 난 후 부터는 안 먹는다.
		if( m_aListBoneEffectStatus[ iBone ].empty() )
			BoneEffect.Show( true );
		else
			BoneEffect.Show( false );

		m_aListBoneEffectStatus[ iBone ].push_back( BoneEffect );
	}
}

void CDnStateBlow::DelEffectAttachedBone( int iBone, DnEtcHandle hEffectHandle )
{
	// 현재 0 번 본을 제외하고 중첩 교차 출력 하지 않음. 그냥 다 보여줌. #16291
	if( CDnActor::FXDummyBoneEnum::FX_01 != iBone )
		return;

	_ASSERT( 0 <= iBone && iBone < CDnActor::Max_FX_Dummy_Bone );
	if( 0 <= iBone && iBone < CDnActor::Max_FX_Dummy_Bone )
	{
		list<S_BONE_EFFECT>& listBoneEffect = m_aListBoneEffectStatus[ iBone ];
		list<S_BONE_EFFECT>::iterator iter = find( listBoneEffect.begin(), 
												   listBoneEffect.end(), hEffectHandle );

		if( listBoneEffect.end() != iter )
		{
			// 현재 중첩된 이펙트 본 자리이며 지금 삭제할 녀석이 현재 발동중이라면.. 
			// 다음 본에게 곧장 차례를 넘긴다.
			// 이미 어디선가 이펙트 객체가 삭제되었다면 bone effect status list 에서 삭제만 한다.
			if( 1 < (int)listBoneEffect.size() )
			{
				if( iter->bShow )
				{
					list<S_BONE_EFFECT>::iterator iterNext = iter;
					iterNext++;
					if( listBoneEffect.end() == iterNext )
						iterNext = listBoneEffect.begin();

					iterNext->Show( true );
				}
			}

			m_aListBoneEffectStatus[ iBone ].erase( iter );
		}
	}
}
#endif

#endif

void CDnStateBlow::OnCmdActionFromPacket( const char* pActionName )
{
	BLOW_HANDLE_LIST_ITER iter = m_listBlowHandle.begin();
	BLOW_HANDLE_LIST_ITER iterEnd = m_listBlowHandle.end();

	for( iter; iter != iterEnd; ++iter )
	{
		if( *iter )
			(*iter)->OnCmdActionFromPacket( pActionName );
	}
}


#ifdef _GAMESERVER
CDnSkill::CanApply CDnStateBlow::CanApplySkillStateEffect(const CDnSkill::SkillInfo *pUsingSkillInfo, const CDnSkill::StateEffectStruct &newStateEffect)
{
	// 상태효과 검사할때 리셋리스트 초기화 [2010/12/09 semozz]
	if (m_hActor)
		m_hActor->InitStateBlowIDToRemove();

	CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;

	// 1. 새로운 상태가 지속 시간이 있다면 추가 시킨다.
	if ((CDnSkill::IsNeedCheckApplyStateBlow( (STATE_BLOW::emBLOW_INDEX)newStateEffect.nID)) ||
		(newStateEffect.nDurationTime > 0) )
	{
		// [2010/11/12 semozz]
		// 2. 지속 시간이 있는 StateBlow일 경우 
		// 일단 기존 스킬 사용 여부 체크와 동일한 코드 사용..
		// 이 부분은 검토 필요
		STATE_BLOW::emBLOW_INDEX newBlowIndex = (STATE_BLOW::emBLOW_INDEX)newStateEffect.nID;
		if( IsApplied( newBlowIndex ) )
		{
			DNVector(DnBlowHandle) vlhResult;
			GetStateBlowFromBlowIndex( newBlowIndex, vlhResult );
			_ASSERT( !vlhResult.empty() );

			vector<int> vlStateBlowIDToRemove;
			DNVector( DnBlowHandle ) vlhSameSkillBlows;

			bool bExistingSameSkill = false;
			int iExistingSameSkillCount = 0;

			int iNumBlow = (int)vlhResult.size();
			for( int iBlow = 0; iBlow < iNumBlow; ++iBlow )
			{
				DnBlowHandle hExistingBlow = vlhResult.at( iBlow );
				const CDnSkill::SkillInfo* pExistingParentSkillInfo = hExistingBlow ? hExistingBlow->GetParentSkillInfo() : NULL;

				// 모체 스킬 정보가 없는 상태효과는 스킬로 걸린 것이 아니므로 여기서 다룰 대상이 아님
				if ( NULL == pExistingParentSkillInfo )
					continue;

				// 스킬 중복 카운트 체크
				// 같은 스킬 아이디에 동일한 상태효과가 또 나타난다면 중복된 스킬임.
				bool bIsSameSkillID = false;
#if defined(PRE_FIX_58505)
				//접미사 스킬 타입이 설정 되어 있지 않으면 스킬 아이디로 비교하고,
				//접미사 스킬 타입이 설정 되어 있으면 접미사 스킬 타입이 같은지 비교.
				if ( pUsingSkillInfo->bItemPrefixSkill == true && pUsingSkillInfo->nPrefixSkillType != CDnPrefixSkill::Prefix_Non )
					bIsSameSkillID = pUsingSkillInfo->nPrefixSkillType == pExistingParentSkillInfo->nPrefixSkillType;
				else
					bIsSameSkillID = pUsingSkillInfo->iSkillID == pExistingParentSkillInfo->iSkillID;
#else
				if( pUsingSkillInfo->iSkillID == pExistingParentSkillInfo->iSkillID )
					bIsSameSkillID = true;
#endif // PRE_FIX_58505

				if (bIsSameSkillID == true)
				{
					// 같은 스킬 아이디가 처음 나온다면 처음엔 적용중 스킬 카운트 하나 올려줌.
					iExistingSameSkillCount += hExistingBlow->GetDuplicateCount();
					bExistingSameSkill = true;

					vlhSameSkillBlows.push_back( hExistingBlow );
				}

				if (pUsingSkillInfo->eTargetType == CDnSkill::TargetTypeEnum::All  //지금 사용 하는 스킬의 TargetType이 All이고, 
					&& (0 != pUsingSkillInfo->iSkillDuplicateMethod && (pUsingSkillInfo->iSkillDuplicateMethod == pExistingParentSkillInfo->iSkillDuplicateMethod)) //중첩 처리ID가 설정되어 있고, 기존 스킬과 동일한 ID일때,
					&& (m_hActor == pUsingSkillInfo->hSkillUser) //자기 자신
					)
					continue;

				// 지속효과 구분 인덱스가 0이 아니라면 스킬 효과 중복 관련 처리가 필요함.
				// 지속효과 구분 인덱스가 0이면 그냥 중첩됨.
				// 혹은 같은 스킬이면서 최대 중첩 카운트가 1이하인경우엔 스킬 대체되도록 처리.
#if defined(PRE_FIX_58505)
				if( 0 != pUsingSkillInfo->iSkillDuplicateMethod ||
					(bIsSameSkillID == true && pUsingSkillInfo->iDuplicateCount <= 1) )
#else
				if( 0 != pUsingSkillInfo->iSkillDuplicateMethod ||
					(pUsingSkillInfo->iSkillID == pExistingParentSkillInfo->iSkillID && pUsingSkillInfo->iDuplicateCount <= 1) )
#endif // PRE_FIX_58505
				{
					// 기존에 실행되고 있던 스킬과 같은 지속효과 인덱스인가. 그렇다면 중첩이 가능한지 체크들어간다.
					// 또한 스킬을 쓴 유저의 팀이 같을때만 중첩처리를 하도록 한다.
					// 다른 팀인데 같은 스킬을 쓸 경우 나에게 걸린 디버프 상태효과가 해제되어 버린다. (#19812)
					if( (pUsingSkillInfo->iSkillDuplicateMethod == pExistingParentSkillInfo->iSkillDuplicateMethod) &&
						(pUsingSkillInfo->iSkillUserTeam == pExistingParentSkillInfo->iSkillUserTeam) )
					{
						if( pUsingSkillInfo->iLevel >= pExistingParentSkillInfo->iLevel )
						{
							// 242번 상태효과는 한번 추가가 되면 리셋 되면 안된다.

							// 확률 체크에서 통과한다면 리셋시킬 상태효과에 넣어둠.
							// 실제 확률체크하는 객체는 기존에 적용된 상태효과 객체를 사용하지만 확률만 체크하는 것이기 때문에
							// 그대로 사용한다.
							if( hExistingBlow->GetBlowIndex() != STATE_BLOW::BLOW_242 && 
								hExistingBlow->CanBegin() )
							{
								vlStateBlowIDToRemove.push_back( hExistingBlow->GetBlowID() );
								//mapDuplicateResult[ hExistingBlow->GetBlowIndex() ] = true;

								// 리셋되어야할 상태들을 담아 놓는다. [2010/12/08 semozz]
								// 여기서 리스트에 담긴 상태들은 OnSignal의 STE_ApplyStateEffect 시점에 상태 적용할때
								// 기존의 상태를 제거 한다.
								m_hActor->AddStateBlowIDToRemove(hExistingBlow->GetBlowID());
							}
							else
							{
								eResult = CDnSkill::CanApply::Fail;		// 효과 갱신 시 확률 체크에서 실패했으므로 추가하지 않는다.
							}
						}
						else if( pUsingSkillInfo->iLevel < pExistingParentSkillInfo->iLevel )
						{
							// 효과 적용 안됨.
							eResult = CDnSkill::CanApply::Fail;

							// Note 한기: 추후에 그래픽적으로 뭔가 표시해줘야 한다면 이 곳에서 처리하면 됨.
						}
					}
				}

			}

			if( false == vlStateBlowIDToRemove.empty() )
			{
				// 스킬 효과 대체. 중첩 카운트 세었던 것 처리할 필요 없다.
				bExistingSameSkill = false;
				eResult = CDnSkill::CanApply::ApplyDuplicateSameSkill;
			}

			// 상태 효과를 중첩해서 적용해야하는 경우, 최대 중첩 갯수가 넘으면 안된다.
			// 높은 레벨의 상태효과가 이미 적용중이라면 중첩처리가 되지 않는다.
			if( CDnSkill::CanApply::Fail != eResult )
			{
				if( bExistingSameSkill )
				{
					if( pUsingSkillInfo->iDuplicateCount <= iExistingSameSkillCount )
					{
						// 최대 중첩 갯수를 넘는 경우 기존 상태효과의 시간만 초기화 한다.
						int iNumSameSkillBlowsToResetDurationTime = (int)vlhSameSkillBlows.size();
						for( int iBlow = 0; iBlow < iNumSameSkillBlowsToResetDurationTime; ++iBlow )
						{
							DnBlowHandle hBlow = vlhSameSkillBlows.at( iBlow );

							//#53448 249번 상태효과 경우 중첩 갯수 이상은 추가 될 수 없다.
							//최대 중첩 갯수를 넘은 경우 아래 루프를 돌면서 기존 상태효과 제거 하고, 다시 추가 작업 필요 없음.
							if (hBlow && hBlow->GetBlowIndex() == STATE_BLOW::BLOW_249)
								continue;

							// 아예 삭제했다가 새로 추가하는 것이 명확함.
							int nDurationTime = int(hBlow->GetDurationTime() * 1000.0f);
							hBlow->ResetDurationTime();

							CDnSkill::SkillInfo SkillInfo = *(hBlow->GetParentSkillInfo());
							STATE_BLOW::emBLOW_INDEX BlowIndex = hBlow->GetBlowIndex();
							string strValue( hBlow->GetValue() );
							
							// 이 시점에서 즉시 지워저야 한다. [2011/01/18 semozz]
							int nBlowID = hBlow->GetBlowID();
							// 패킷 보낼꺼 보내고
							m_hActor->SendRemoveStateEffectFromID(nBlowID);
							// 바로 삭제한다..
							if (m_hActor->GetStateBlow())
								m_hActor->GetStateBlow()->RemoveImediatlyStateEffectFromID(nBlowID);

							// 여기에 결빙같은 확률 있는 상태효과를 100% 성공시켜야 하므로 
							// CanBegin 함수 호출하지 않도록 bCheckCanBegin 플래그를 꺼서 호출.
							m_hActor->CmdAddStateEffect( &SkillInfo, BlowIndex, nDurationTime, strValue.c_str(), false, false );
						}

						eResult = CDnSkill::CanApply::Fail;
					}
				}
			}
		}	
	}

	return eResult;
}
#endif

bool RemoveStateBlowInfo::Compare(DnBlowHandle hBlow)
{
	bool result = false;
	
	if (!hBlow)
		return result;

	switch(m_Type)
	{
	case RSBI_BLOW_INDEX:
		{
			if( hBlow->IsEternity() == false && hBlow->IsFromSourceItem() == false )
				result = (hBlow->GetBlowIndex() == m_Value);
		}
		break;
	case RSBI_BLOW_ID:
		{
			result = (hBlow->GetBlowID() == m_Value);
		}
		break;
	case RSBI_SKILL_DURATION_TYPE:
		{
			const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();
			result = ( pSkillInfo && (CDnSkill::DurationTypeEnum)m_Value == pSkillInfo->eDurationType );
		}
		break;
#if !defined(_GAMESERVER)
	case RSBI_BLOW_SERVER_ID:
		{
			result = (hBlow->GetServerBlowID() == m_Value);
		}
		break;
#endif
	}

	return result;
}


void CDnStateBlow::RemoveStateBlowReservedList()
{
	REMOVE_STATEBLOW_LIST::iterator removeInfoiter = m_RemoveStateBlowList.begin();
	
#ifndef _FINAL_BUILD
	_CrashIfRemoveListLocked();
#endif // #ifdef _FINAL_BUILD
	m_bLockRemoveReservedList = true;
	for (; removeInfoiter != m_RemoveStateBlowList.end(); ++removeInfoiter)
	{
		RemoveStateBlowInfo &removeInfo = (*removeInfoiter);

		DnBlowHandle hBlow;

#ifndef _FINAL_BUILD
		_CrashIfProcessListLocked();
#endif // #ifdef _FINAL_BUILD
		m_bLockStateBlowList = true;
		BLOW_HANDLE_LIST_ITER blowListiter =  m_listBlowHandle.begin();
		for( ; blowListiter != m_listBlowHandle.end();  )
		{
			hBlow = (*blowListiter);

			if (removeInfo.Compare(hBlow))
			{

				//ResetStateBlowBySkillType 함수에서 패킷 클라이언트로 보내는 코드 있음.
				if (RemoveStateBlowInfo::RSBI_SKILL_DURATION_TYPE == removeInfo.m_Type)
				{
#if defined(_GAMESERVER)
					STATE_BLOW::emBLOW_INDEX emBlowIndex = hBlow->GetStateBlow().emBlowIndex;
					// 클라에게도 삭제하라고 패킷만 날려줌
					m_hActor->SendRemoveStateEffect(emBlowIndex);
#endif // _GAMESERVER
				}

				_ASSERT( m_aiApplied[ hBlow->GetBlowIndex() ] > 0 );
				--m_aiApplied[ hBlow->GetBlowIndex() ];

				hBlow->OnEnd( 0, 0 );
				m_hActor->OnEndStateBlow( hBlow );

				SAFE_RELEASE_SPTR( hBlow );
				blowListiter = m_listBlowHandle.erase(blowListiter);

				//BlowIndex, DuratioType은 전부 찾아서 지운다.
				if (RemoveStateBlowInfo::RSBI_BLOW_INDEX == removeInfo.m_Type ||
					RemoveStateBlowInfo::RSBI_SKILL_DURATION_TYPE == removeInfo.m_Type)
					continue;
				else
					break;
			}

			++blowListiter;
		}
		m_bLockStateBlowList = false;
	}

	m_RemoveStateBlowList.clear();
	m_bLockRemoveReservedList = false;
}

void CDnStateBlow::AddRemoveStateBlowInfo(RemoveStateBlowInfo info)
{
	//상태효과 제거를 위해 등록은 루프 돌면서도 가능..
	m_RemoveStateBlowList.push_back(info);
}

void CDnStateBlow::RemoveImediatlyStateEffectFromID(int nStateBlowID)
{
#ifndef _FINAL_BUILD
	_CrashIfProcessListLocked();
#endif // #ifdef _FINAL_BUILD

	DnBlowHandle hBlow;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		hBlow = (*iter);

		if( hBlow && (hBlow->GetBlowID() == nStateBlowID) )
		{
			_ASSERT( m_aiApplied[ hBlow->GetBlowIndex() ] > 0 );
			--m_aiApplied[ hBlow->GetBlowIndex() ];

			hBlow->OnEnd( 0, 0 );
			m_hActor->OnEndStateBlow( hBlow );

			OutputDebug("%s EndBlowID: %d Index: %d", __FUNCTION__, hBlow->GetBlowID(), hBlow->GetBlowIndex());

			SAFE_RELEASE_SPTR( hBlow );
			m_listBlowHandle.erase(iter);
			break;
		}
	}
}

void CDnStateBlow::RemoveImediatlyStateEffectByBlowIndex(STATE_BLOW::emBLOW_INDEX emBlowIndex)
{
#ifndef _FINAL_BUILD
	_CrashIfProcessListLocked();
#endif // #ifdef _FINAL_BUILD

	int nIndex = -1;
	DnBlowHandle hBlow;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

	for( ; iter != m_listBlowHandle.end(); )
	{
		hBlow = (*iter);

		if( hBlow && ( ( hBlow->IsEternity() == false && hBlow->IsFromSourceItem() == false ) && hBlow->GetBlowIndex() == emBlowIndex ) )
		{
			_ASSERT( m_aiApplied[ hBlow->GetBlowIndex() ] > 0 );
			--m_aiApplied[ hBlow->GetBlowIndex() ];

			hBlow->OnEnd( 0, 0 );
			m_hActor->OnEndStateBlow( hBlow );

			nIndex = hBlow->GetBlowID();
			SAFE_RELEASE_SPTR( hBlow );
			iter = m_listBlowHandle.erase(iter);

			continue;
		}

		++iter;
	}
}

#if !defined(_GAMESERVER)
void CDnStateBlow::RemoveImediatlyStateEffectByServerID(int nServerBlowID)
{
#ifndef _FINAL_BUILD
	_CrashIfProcessListLocked();
#endif // #ifdef _FINAL_BUILD

	DnBlowHandle hBlow;

	BLOW_HANDLE_LIST_ITER iter =  m_listBlowHandle.begin();

	for( ; iter != m_listBlowHandle.end(); ++iter )
	{
		hBlow = (*iter);

		if( hBlow && (hBlow->GetServerBlowID() == nServerBlowID) )
		{
			_ASSERT( m_aiApplied[ hBlow->GetBlowIndex() ] > 0 );
			--m_aiApplied[ hBlow->GetBlowIndex() ];

			hBlow->OnEnd( 0, 0 );
			m_hActor->OnEndStateBlow( hBlow );

			SAFE_RELEASE_SPTR( hBlow );
			m_listBlowHandle.erase(iter);
			break;
		}
	}
}
#endif // _GAMESERVER


bool CDnStateBlow::IsImmuned(STATE_BLOW::emBLOW_INDEX blowIndex)
{
	// 무적 상태효과 있을 때 버프 상태효과를 제외하곤 전부 제외.
	if( IsApplied( STATE_BLOW::BLOW_099 ) )
	{
		return false;
	}

	// 현재 면역된 상태효과인지 체크
	// 부활(무적) 상태효과인 경우 데미지 먹는 상태효과에 걸리지 않는다.
	list<DnBlowHandle>::iterator iter = m_listBlowHandle.begin();
	for( iter; iter != m_listBlowHandle.end(); )
	{
		DnBlowHandle hNowBlow = (*iter);
		if( hNowBlow )
		{
			if( (hNowBlow->GetBlowIndex() == STATE_BLOW::BLOW_077) )
			{
				bool bImmuned = static_cast<CDnImmuneBlow*>(hNowBlow.GetPointer())->IsImmuned( blowIndex );
				if (bImmuned)
					return true;
			}
			else
			if( (hNowBlow->GetBlowIndex() == STATE_BLOW::BLOW_150) )
			{
				bool bImmuned = static_cast<CDnAllImmuneBlow*>(hNowBlow.GetPointer())->IsImmuned( blowIndex );
				if (bImmuned)
					return true;
			}
		}

		++iter;
	}

	return false;
}

#ifndef _FINAL_BUILD
void CDnStateBlow::_CrashIfProcessListLocked( void )
{
	if( m_bLockStateBlowList )
	{
		_ASSERT( 0 );
		int* pNull = NULL;
		*pNull = 0xffffeeee;
	}
}

void CDnStateBlow::_CrashIfRemoveListLocked( void )
{
	if( m_bLockRemoveReservedList )
	{
		_ASSERT( 0 );
		int* pNull = NULL;
		*pNull = 0xeeeeffff;
	}
}
#endif // #ifdef _FINAL_BUILD


void CDnStateBlow::Process_StateBlow_Old(LOCAL_TIME LocalTime, float fDelta, bool bForceInitialize)
{
	BLOW_HANDLE_LIST::iterator iter;
	DnBlowHandle hBlow;

	for( iter = m_listBlowHandle.begin(); iter != m_listBlowHandle.end(); )
	{
		hBlow = (*iter);

		if( hBlow )
		{
			// 게임서버 덤프 수정 확인용.
			if( !(hBlow->GetActorHandle()) )
				hBlow->SetActorHandle( m_hActor );
			//////////////////////////////////////////////////////////////////////////

			if( hBlow->IsBegin() )
			{
				// 팀 반전 같은 상태효과도 있어서 팀이 반전되면 유혹 상태효과 이펙트가 
				// 몹과 같은 팀이 되어 붙질 않으므로 먼저 이펙트를 처리한다. #14128
#ifndef _GAMESERVER
				if( hBlow->IsUseTableDefinedGraphicEffect() )
					hBlow->AttachGraphicEffectDefaultType();
#else
				hBlow->CheckAndStartActorActionInEffectInfo();
#endif
				hBlow->OnBegin( LocalTime, fDelta );
				m_hActor->OnBeginStateBlow( hBlow );

				// 곧바로 end 로 셋팅하는 blow 도 있기 때문에 체크.
				if( STATE_BLOW::STATE_END != hBlow->GetBlowState() )
					hBlow->SetState( STATE_BLOW::STATE_DURATION );
			}
			else if( hBlow->IsDuration() )
			{
				hBlow->Process( LocalTime, fDelta );
			}
			else if( hBlow->IsEnd() )
			{
				RemoveStateBlowFromID(hBlow->GetBlowID());
			}
		}

		// 각각의 blow 후에 process 후에 독, 화상 걸려서 interval 데미지에 죽거나 하는 경우엔 패시브 스킬이 바로 다시 붙게 되는데 
		// 죽었을 땐 부활 상태효과만 처리한다.
#ifdef _GAMESERVER
		// 게임 서버일땐 액터가 gm trace 일 때도 아래 부활 처리 루틴으로 들어가기 때문에 조건을 하나 더 걸어준다.
		if( !m_hActor->IsGMTrace() && !bForceInitialize && m_hActor->IsPlayerActor() && m_hActor->IsDie() )
#else
		if( !bForceInitialize && m_hActor->IsPlayerActor() && m_hActor->IsDie() )
#endif
		{
			// 부활 상태효과가 없다면 루프 종료
			bool bExistRebirth = false;
			for( iter = m_listBlowHandle.begin(); m_listBlowHandle.end() != iter; ++iter )
			{
				if( !(*iter) )
					continue;

				if( (*iter)->GetBlowIndex() == STATE_BLOW::BLOW_057 || (*iter)->GetBlowIndex() == STATE_BLOW::BLOW_230 )
				{
					bExistRebirth = true;
					break;
				}
			}
			if( false == bExistRebirth )
				break;
		}
		else
			++iter;
	}
}

void CDnStateBlow::Process_StateBlow_New(LOCAL_TIME LocalTime, float fDelta, bool bForceInitialize)
{
	//죽은상태, Rebirth상태효과 존재 유무
	bool bIsDie = false;
	bool bExistRebirth = false;
	BLOW_HANDLE_LIST::iterator rebirthStateIter = m_listBlowHandle.end();
	BLOW_HANDLE_LIST::iterator spectatorStateIter = m_listBlowHandle.end();

#ifdef _GAMESERVER
	// 게임 서버일땐 액터가 gm trace 일 때도 아래 부활 처리 루틴으로 들어가기 때문에 조건을 하나 더 걸어준다.
	if( !m_hActor->IsGMTrace() && !bForceInitialize && m_hActor->IsPlayerActor() && m_hActor->IsDie() )
#else
	if( !bForceInitialize && m_hActor->IsPlayerActor() && m_hActor->IsDie() )
#endif
	{
		bIsDie = true;
		for( BLOW_HANDLE_LIST::iterator iter = m_listBlowHandle.begin(); m_listBlowHandle.end() != iter; ++iter )
		{
			if( !(*iter) )
				continue;

			if( (*iter)->GetBlowIndex() == STATE_BLOW::BLOW_057 )
			{
				bExistRebirth = true;
				rebirthStateIter = iter;
			}

			if( (*iter)->GetBlowIndex() == STATE_BLOW::BLOW_230 )
			{
				bExistRebirth = true;
				spectatorStateIter = iter;
			}
		}
	}

	//죽은 상태이고,
	if (bIsDie)
	{
		// 죽은 상태효과에서 Rebirth상태효과가 존재 한다면 Rebirth상태효과만 처리
		if (rebirthStateIter != m_listBlowHandle.end())
		{
			DnBlowHandle hRebirthBlow = (*rebirthStateIter);
			Process_BlowHandle(hRebirthBlow, LocalTime, fDelta);
		}
		if (spectatorStateIter != m_listBlowHandle.end())
		{
			DnBlowHandle hSpectatorBlow = (*spectatorStateIter);
			Process_BlowHandle(hSpectatorBlow, LocalTime, fDelta);
		}
		//죽은 상태일경우 다른 상태효과는 처리 안하도록 여기서 리턴...
		return;
	}


	DnBlowHandle hBlow;
	for( BLOW_HANDLE_LIST::iterator iter = m_listBlowHandle.begin(); iter != m_listBlowHandle.end(); )
	{
		hBlow = (*iter);

		//유효하지 않은 상태효과는 리스트에서 바로 제거 해 버린다..
		if (!hBlow)
		{
			iter = m_listBlowHandle.erase(iter);
			continue;
		}

		Process_BlowHandle(hBlow, LocalTime, fDelta);
		
		++iter;
	}
}

void CDnStateBlow::Process_BlowHandle(DnBlowHandle hBlow, LOCAL_TIME LocalTime, float fDelta)
{
	if (!hBlow)
		return;

	if( hBlow->IsBegin() )
	{
		// 팀 반전 같은 상태효과도 있어서 팀이 반전되면 유혹 상태효과 이펙트가 
		// 몹과 같은 팀이 되어 붙질 않으므로 먼저 이펙트를 처리한다. #14128
#ifndef _GAMESERVER
		if( hBlow->IsUseTableDefinedGraphicEffect() )
			hBlow->AttachGraphicEffectDefaultType();
#else
		hBlow->CheckAndStartActorActionInEffectInfo();
#endif
		hBlow->OnBegin( LocalTime, fDelta );
		m_hActor->OnBeginStateBlow( hBlow );

		// 곧바로 end 로 셋팅하는 blow 도 있기 때문에 체크.
		if( STATE_BLOW::STATE_END != hBlow->GetBlowState() )
			hBlow->SetState( STATE_BLOW::STATE_DURATION );
	}
	else if( hBlow->IsDuration() )
	{
		hBlow->Process( LocalTime, fDelta );
	}
	else if( hBlow->IsEnd() )
	{
		RemoveStateBlowFromID(hBlow->GetBlowID());
	}
}


float CDnStateBlow::GetImmuneReduceTimeValue(STATE_BLOW::emBLOW_INDEX blowIndex)
{
	float fReduceTimeValue = 1.0f;

	//면역 상태효과를 돌면서 blowIndex에 면역인 상태효과에서 지속시간 비율 변경 값을 얻어 온다.
	DNVector(DnBlowHandle) vlhResult;
	GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_077, vlhResult );

	int nListCount = (int)vlhResult.size();
	for (int i = 0; i < nListCount; ++i)
	{
		DnBlowHandle hExistBlow = vlhResult.at( i );
		int nBlowIndex = (int)hExistBlow->GetFloatValue();
		if ((STATE_BLOW::emBLOW_INDEX)(nBlowIndex) == blowIndex)
		{
			//blowIndex 지속시간 비율 변경 값중에 제일 작은 값을 얻는다..
			float fValue = static_cast<CDnImmuneBlow*>(hExistBlow.GetPointer())->GetReduceTimeValue();
			if (fReduceTimeValue > fValue)
				fReduceTimeValue = fValue;
		}
	}

	return fReduceTimeValue;
}


#ifdef _GAMESERVER
#ifdef PRE_ADD_PROJECTILE_SE_INFO
void CDnStateBlow::MakeCloneStateBlowList( BLOW_HANDLE_LIST BlowList )
{
	if( BlowList.empty() )
		return;
	static CDnCreateBlow createBlow;
	BLOW_HANDLE_LIST_ITER iter = BlowList.begin();
	for( iter; iter != BlowList.end() ; ++iter )
	{
		DnBlowHandle hBlow = (*iter);
		if( hBlow )
		{
			if( hBlow->GetBlowIndex() > STATE_BLOW::BLOW_NONE && hBlow->GetBlowIndex() < STATE_BLOW::BLOW_MAX )
			{
				DnBlowHandle hBlow = createBlow.CreateBlow( (*iter)->GetBlowIndex() , (*iter)->GetActorHandle(), (*iter)->GetValue() );
				m_listBlowHandle.push_back( hBlow );
				++m_aiApplied[ hBlow->GetBlowIndex() ];
			}
		}
	}
}
#endif
#endif

