#include "stdafx.h"
#include "MAMultiDamage.h"
#include "DnPartsMonsterActor.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MonsterParts::MonsterParts()
: m_fDestroyDelta( 0.0f )
{
	m_hSkill.Identity();
}

MonsterParts::~MonsterParts()
{
	SAFE_RELEASE_SPTR( m_hSkill );
}

bool MonsterParts::Process( const float fDelta )
{
	// 재생시간이 0보다 작다는 것은 무한이란뜻!
	if( GetPartsOriginalInfo().nHPRefreshTimems < 0 )
		return false;

	m_fDestroyDelta -= fDelta;
	if( m_fDestroyDelta <= 0.f )
		return true;

	return false;
}

bool MonsterParts::Create( const _Info& info)
{ 
	m_OriginalInfo = m_Info = info;
	return true;
}

bool MonsterParts::CreateSkill( DnActorHandle hActor )
{ 
	if( m_Info.nPassiveSkillIndex > 0 && m_Info.nPassiveSkillLevel > 0 )
	{
		m_hSkill = CDnSkill::CreateSkill( hActor, m_Info.nPassiveSkillIndex, m_Info.nPassiveSkillLevel );
		if( !m_hSkill )
		{
			//g_Log.LogA( "MonsterParts::Create() SkillIndex=%d Level=%d 스킬생성실패!\r\n", m_Info.nPassiveSkillIndex, m_Info.nPassiveSkillLevel );
			return false;
		}

		if( !hActor->AddSkill( m_hSkill ) )
			return false;

		SetRefresh( hActor );
	}

	return true;
}

bool MonsterParts::HasBone( const char* pszBoneName )
{
	for( size_t i=0 ; i <m_Info.vParts.size() ; ++i )
	{
		if ( strcmp( m_Info.vParts[i].szBoneName.c_str(), pszBoneName ) == 0 )
			return true;
	}

	return false;
}

bool MonsterParts::IsLimitAction( const char* pszActionName )
{
	for( size_t i=0 ; i<m_OriginalInfo.vLimitAction.size() ; ++i )
	{
		if( strcmp( m_OriginalInfo.vLimitAction[i].c_str(), pszActionName ) == 0 )
			return true;
	}

	return false;
}

void MonsterParts::SetRefresh( DnActorHandle hActor )
{
	m_Info.nHP = m_OriginalInfo.nHP;
	if( m_hSkill && hActor )
	{
		if( m_Info.bIsRefreshUseSkill )
		{
#if defined( _GAMESERVER )
			hActor->UseSkill( m_hSkill->GetClassID() );
#endif
		}
	}
}

void MonsterParts::SetDestroy( DnActorHandle hActor )
{
	m_Info.nHP		= 0;
	m_fDestroyDelta = m_OriginalInfo.nHPRefreshTimems/1000.0f;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void MAMultiDamage::LoadMultiDamageInfo()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );
	if ( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"MonsterTable.ext failed\r\n");
		return;
	}

	CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>(this);
	if ( !pMonsterActor )
		return;

	CDnPartsMonsterActor* pPartsMonsterActor = dynamic_cast<CDnPartsMonsterActor*>(this);

	int nItemID = pMonsterActor->GetMonsterClassID();
	if( !pSox->IsExistItem( nItemID ) ) return;

	for ( int i=1 ; i<=5 ; ++i )
	{
		char szLabel[MAX_PATH];
		sprintf_s( szLabel, "_MonsterPartsIndex%d", i );
		int nMultiDamageIdx = pSox->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger();
		if ( nMultiDamageIdx < 1 )
			continue;

		DNTableFileFormat* pPartsSox = GetDNTable( CDnTableDB::TMONSTERPARTS );
		if ( !pPartsSox )
			return;

		MonsterParts::_Info info;

		info.uiMonsterPartsTableID	= nMultiDamageIdx;
		info.nHP					= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_HP" )->GetInteger();
		info.nHPRefreshTimems		= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_HPRefreshTimems" )->GetInteger();
		info.fMainDamageRate		= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_MainDamageRate" )->GetFloat();
		info.fDefenseRate			= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_DefenseRate" )->GetFloat();
		info.nPassiveSkillIndex		= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_PassiveSkillIndex" )->GetInteger();
		info.nPassiveSkillLevel		= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_PassiveSkillLevel" )->GetInteger();
		info.nDestroySkillIndex		= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_DeadSkillIndex" )->GetInteger();
		info.bIsRefreshUseSkill		= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_PassiveSkillLink")->GetInteger() ? true : false;

#if defined( _GAMESERVER )
		char* pszRebirthParts = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_PartsHPLink" )->GetString();
		if( pszRebirthParts && strlen(pszRebirthParts) > 0 )
		{
			std::string				strString(pszRebirthParts);
			DNVector(std::string)	vSplit;

			boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(",") );
			info.vRebirthPartsTableID.reserve( vSplit.size() );

			for( UINT j=0 ; j<vSplit.size() ; ++j )
			{
				UINT uiTableID = boost::lexical_cast<UINT>(vSplit[j].c_str());
				if( uiTableID )
					info.vRebirthPartsTableID.push_back( uiTableID );
			}
		}
#endif

#ifndef _GAMESERVER
		info.szDeadSkinName = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_DeadSkinName" )->GetString();
		info.szDeadAniName = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_DeadAniName" )->GetString();
		info.szDeadActName = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_DeadActionName" )->GetString();
		info.szAction = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_DeadAction" )->GetString();
#endif //_GAMESERVER

		info.szDeadActorActName		= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_DeadActorAction" )->GetString();
		info.szDeadActorActBoneName = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_DeadActorActionBoneName" )->GetString();
		info.szDeadActorFixBoneName	= pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_DeadActorActionFixBoneName" )->GetString();

#ifndef _GAMESERVER
#if defined(PRE_ADD_MULTILANGUAGE)
		info.wszPartsName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_NameID" )->GetInteger(), MultiLanguage::eDefaultLanguage );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		info.wszPartsName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_NameID" )->GetInteger() );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		info.m_bEnalbeUI =  pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_PartsHPDisplay")->GetInteger() ? true : false;
#endif

		for( int j=1 ; j<=5 ; ++j )
		{
			sprintf_s( szLabel, "_LimitAction%d", j );

			char* pszLimitAction = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, szLabel )->GetString();
			if( pszLimitAction && strlen(pszLimitAction) )
				info.vLimitAction.push_back( pszLimitAction );
		}
		for ( int j=1 ; j<=20 ; ++j )
		{
			char szLocalLabel[64];

			sprintf_s( szLocalLabel, "_MeshName%d", j );
			char* pszMeshName = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, szLocalLabel )->GetString();
			sprintf_s( szLocalLabel, "_BoneName%d", j );
			char* pszBoneName = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, szLocalLabel )->GetString();
			sprintf_s( szLocalLabel, "_EffectType%d", j );
			bool bApplyEffect = ( pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, szLocalLabel )->GetInteger() ) ? true : false;

			if( pszMeshName && pszBoneName && strlen(pszMeshName) && strlen(pszBoneName) )
				info.vParts.push_back( MonsterParts::_PartsInfo( pszMeshName, pszBoneName, bApplyEffect ) );
		}

		char* pszRequiredPartsID = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_RequiredPartsID" )->GetString();
		char* pszRequiredPartsHP = pPartsSox->GetFieldFromLablePtr( nMultiDamageIdx, "_RequiredPartsHP" )->GetString();

		if( pszRequiredPartsID && pszRequiredPartsHP && strlen(pszRequiredPartsID) && strlen(pszRequiredPartsHP) )
		{
			std::string					strRequiredPartsID(pszRequiredPartsID);
			std::string					strRequiredPartsHP(pszRequiredPartsHP);
			std::vector<std::string>	vSplitID,vSplitHP;

			boost::algorithm::split( vSplitID, strRequiredPartsID, boost::algorithm::is_any_of(";") );
			boost::algorithm::split( vSplitHP, strRequiredPartsHP, boost::algorithm::is_any_of(";") );

			if( vSplitID.size() == vSplitHP.size() )
			{
				MonsterParts::_HitCondition HitCondition;
				for( UINT j=0 ; j<vSplitID.size() ; ++j )
				{
					HitCondition.iRequiredPartsID		= atoi(vSplitID[j].c_str());
					HitCondition.iRequiredPartsUnderHP	= atoi(vSplitHP[j].c_str());
					
					info.vHitCondition.push_back( HitCondition );
				}
			}
			else
			{
				_ASSERT(0);
			}
		}

		_AddMonsterParts( info );

	#ifdef _GAMESERVER
		if (pPartsMonsterActor)
			pPartsMonsterActor->_OnAddMonsterParts(info);
	#else
		if (pPartsMonsterActor)
			pPartsMonsterActor->OnAddMonsterParts(info);
	#endif
	}

	for( size_t i=0 ; i<m_Parts.size() ; ++i )
		m_Parts[i].CreateSkill( pMonsterActor->GetActorHandle() );
}

bool MAMultiDamage::_AddMonsterParts( const MonsterParts::_Info& info )
{
	MonsterParts parts;
	parts.Create( info );
	m_Parts.push_back(parts);

	return true;
}

void MAMultiDamage::_OnRefreshParts( DnActorHandle hActor, MonsterParts* pParts, int nPartsIndex )
{
	// 파츠가 다시 살아남. 파괴 이펙트 제거부탁용~
#ifndef _GAMESERVER
	if( !pParts->GetPartsInfo().szDeadSkinName.empty() || !pParts->GetPartsInfo().szDeadActName.empty() ) {
		for( DWORD i=0; i<pParts->GetPartsInfo().vParts.size(); i++ ) {
			if( pParts->GetPartsInfo().vParts[i].bApplyDeadEffect ) {
				hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( -10000, ( nPartsIndex * 20 ) + i );
			}
		}
	}

	CDnPartsMonsterActor* pPartsActor = dynamic_cast<CDnPartsMonsterActor*>(this);
	if (pPartsActor != NULL && pParts != NULL)
		pPartsActor->OnRefreshParts(*pParts, nPartsIndex);

	pParts->SetRefresh( hActor );
#endif
}

void MAMultiDamage::_OnDestroyParts( DnActorHandle hActor, MonsterParts* pParts, int nPartsIndex )
{
	// 파츠 파괴. 파괴 이펙트 뿌려주세용~
#ifndef _GAMESERVER
	if( !pParts->GetPartsInfo().szDeadSkinName.empty() || !pParts->GetPartsInfo().szDeadActName.empty() ) {
		for( DWORD i=0; i<pParts->GetPartsInfo().vParts.size(); i++ ) {
			if( pParts->GetPartsInfo().vParts[i].bApplyDeadEffect ) {
				if( hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::IsExistSignalHandle( -10000, ( nPartsIndex * 20 ) + i ) ) {
					hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( -10000, ( nPartsIndex * 20 ) + i );
				}
				DnEtcHandle hObject = (new CDnEtcObject)->GetMySmartPtr();
				if( !hObject->Initialize( pParts->GetPartsInfo().szDeadSkinName.c_str(), pParts->GetPartsInfo().szDeadAniName.c_str(), pParts->GetPartsInfo().szDeadActName.c_str() ) ) {
					SAFE_RELEASE_SPTR( hObject );
					return;
				}
				hObject->SetActionQueue( pParts->GetPartsInfo().szAction.c_str() );
				EtcObjectSignalStruct *pResult = hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -10000, ( nPartsIndex * 20 ) + i, hObject );

				pResult->bDefendenceParent = true;
				pResult->bLinkObject = true;
				pResult->vOffset = EtVector3( 0.f, 0.f, 0.f );
				pResult->vRotate = EtVector3( 0.f, 0.f, 0.f );
				_strcpy( pResult->szBoneName, _countof(pResult->szBoneName), pParts->GetPartsInfo().vParts[i].szBoneName.c_str(), (int)strlen(pParts->GetPartsInfo().vParts[i].szBoneName.c_str()) );
			}
		}
	}

	CDnPartsMonsterActor* pPartsActor = dynamic_cast<CDnPartsMonsterActor*>(this);
	if (pPartsActor != NULL && pParts != NULL)
		pPartsActor->OnDestroyParts(*pParts, nPartsIndex);

	pParts->SetDestroy( hActor );
#endif	
}

void MAMultiDamage::OnSetPartsHP( DnActorHandle hActor, const int iPartsTableID, const int iCurHP, const int iHitterID )
{
	CDnPartsMonsterActor* pMonsterActor = dynamic_cast<CDnPartsMonsterActor*>(this);
	if ( !pMonsterActor )
		return;

	for( size_t i=0 ; i<m_Parts.size() ; ++i )
	{
		if( m_Parts[i].GetPartsTableID() == iPartsTableID )
		{
			// 파츠에 원래 들어가는 damage 를 주고 각 파츠의 추가 방어 감쇄율을 적용해서 데미지 값을 돌려준다.
			pMonsterActor->OnMultiDamage( m_Parts[i] );

			int iPrevHP = m_Parts[i].GetHP();
			m_Parts[i].SetHP( iCurHP );

			//OutputDebug("[PARTSDAMAGE] id:%d : prev HP:%d current HP:%d\n", m_Parts[i].GetPartsTableID(), iPrevHP, iCurHP);

#ifndef _GAMESERVER
			if( iPrevHP > 0 && iCurHP <= 0 )
				_OnDestroyParts( hActor, &m_Parts[i], (int)i );
			else if( iPrevHP <= 0 && iCurHP > 0 )
				_OnRefreshParts( hActor, &m_Parts[i], (int)i );
#endif
			break;
		}
	}
}

#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
int MAMultiDamage::GetPartsIndexFromPartsID( const UINT uiTableID )
{
	for( UINT i=0 ; i<m_Parts.size() ; ++i )
	{
		if( m_Parts[i].GetPartsTableID() == uiTableID )
			return i;
	}
	
	return -1;
}
#endif 

MonsterParts* MAMultiDamage::GetParts( const UINT uiTableID )
{
	for( UINT i=0 ; i<m_Parts.size() ; ++i )
	{
		if( m_Parts[i].GetPartsTableID() == uiTableID )
			return &m_Parts[i];
	}

	return NULL;
}

MonsterParts* MAMultiDamage::GetPartsByIndex(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_Parts.size())
		return NULL;

	return &m_Parts[nIndex];
}
