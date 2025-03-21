#include "StdAfx.h"
#include "DnPartsMonsterActor.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

extern float g_fTotalElapsedTime;

CDnPartsMonsterActor::CDnPartsMonsterActor( int nClassID, bool bProcess )
: TDnMonsterActor( nClassID, bProcess )
{
	m_bUseDamagePartsColor = false;

	m_nAmbientParamIndex = -1;
	ClearMeshAmbientColor();
	m_fDamageBlinkColorDelta = 0.f;
	m_fDamageBlinkColorTime	 = 0.f;
	m_fDamageShakeDelta		 = 0.f;
	m_fDamageShakeTime		 = 0.f;
	m_fDamageScaleFactor	 = 5.f;
	m_bUseDamagePartsShake	 = true;
#ifdef PRE_FIX_CHANGE_EMISSIVE
	m_nChangeEmissiveStep	 = ChangeEmissiveStep::None;
	m_fEmissivePower		 = 0.0f;
	m_fEmissivePowerRange	 = 0.0f;
	m_fEmissiveAniSpeed		 = 0.0f;
	m_bExistEmissive		 = false;
	m_fCurrentEmissive = 0.0f;
	m_fDeltaTime = 0.0f;
#else
	m_bChangeEmissive		 = false;
#endif
	m_bImmediatelyChangeEmissive = false;		
	m_nDiffuseParamIndex = 0;


#ifdef PRE_ADD_DAMAGEDPARTS
	// #60073
	m_nDamagedEmissiveParamIndex = -1;
	m_bUseDamagePartsEmissiveColor = false;
	m_nHPPercentEmissiveParamIndex = -1;
	m_bUseHPPercentEmissiveColor = false;
#endif

#ifdef PRE_ADD_CHECK_PART_MONSTER
	m_vEnablePartsGaugeList.clear();
	m_bIsEnablePartsHPDisplay = false;
#endif

}

CDnPartsMonsterActor::~CDnPartsMonsterActor()
{
#ifdef PRE_ADD_DAMAGEDPARTS
	// #60073.
	m_mapDamagedEmissivePower.clear(); 
#endif

}

void CDnPartsMonsterActor::SetMeshAmbientColor(const char* szMeshName, EtColor& clr)
{
	std::map<std::string, EtColor>::iterator it = m_MeshAmbientColorMap.find(std::string(szMeshName));

	if (it != m_MeshAmbientColorMap.end())
	{
		EtColor& _clr = (it->second);
		_clr = clr;
	}
	else
	{
		m_MeshAmbientColorMap.insert(std::pair<std::string, EtColor>(std::string(szMeshName), clr));
	}

}

void CDnPartsMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnMonsterActor< MASingleBody, MAMultiDamage >::Process( LocalTime, fDelta );

	if (m_bUseDamagePartsColor)
	{
		if (m_fDamageBlinkColorDelta > 0.f)
		{
			m_fDamageBlinkColorDelta -= fDelta;
			if (m_fDamageBlinkColorDelta <= 0.f)
				m_fDamageBlinkColorDelta = 0.f;
		}
		else
		{
			static EtColor DefaultColor( 0.682f, 0.682f, 0.682f, 1.0f );
			m_hObject->SetCustomParam( m_nAmbientParamIndex, &DefaultColor );
			ClearMeshAmbientColor();
		}

		//	note by kalliste : mesh ambient color setting을 현재 데미지 컬러에만 사용하기 때문에 퍼포먼스 고려하여 flag로 막음. 이후 용도 확장되면 밖으로 뺄 것.
		std::map<std::string, EtColor>::iterator it = m_MeshAmbientColorMap.begin();
		for (; it != m_MeshAmbientColorMap.end() ; ++it)
		{
			EtColor clr = it->second;

			int nSubMeshIdx = m_hObject->GetSubMeshIndex(it->first.c_str());
			m_hObject->SetCustomParam(m_nAmbientParamIndex, &clr, nSubMeshIdx);
		}
	}

	if (m_bUseDamagePartsShake)
	{
		float fValue = 1.f;
		if (m_fDamageShakeDelta > 0.f)
		{
			m_fDamageShakeDelta -= fDelta;
			if (m_fDamageShakeDelta <= 0.f)
				m_fDamageShakeDelta = 0.f;

			fValue = 1.f - ( ( _rand()%100 ) * (m_fDamageScaleFactor * 0.0001f) );
		}

		MAActorRenderBase::SetScale( EtVector3( GetScale() * fValue, GetScale(), GetScale() * fValue ) );
	}

#ifdef PRE_FIX_CHANGE_EMISSIVE
	switch( m_nChangeEmissiveStep )
	{
	case ChangeEmissiveStep::EmissiveDecrease:
		{
			if( m_fCurrentEmissive <= 0.0f || m_bImmediatelyChangeEmissive )
			{
				m_nChangeEmissiveStep = ChangeEmissiveStep::EmissiveIncrease;

				TDnMonsterActor< MASingleBody, MAMultiDamage >::ChangeEmissiveTexture( m_szChangeEmissiveTexture );
				m_szChangeEmissiveTexture = "";

				// 즉시 바꾸는거라면 텍스처 바꾸고 상수도 모두 복구시켜두고 끝낸다.
				if( m_bImmediatelyChangeEmissive )
				{
					TDnMonsterActor< MASingleBody, MAMultiDamage >::SetEmissiveConstants( m_fEmissivePower, m_fEmissivePowerRange, m_fEmissiveAniSpeed );
					m_nChangeEmissiveStep = ChangeEmissiveStep::None;
					break;
				}
			}

			// 매프레임 Power값 조절로만 줄여나간다.
			TDnMonsterActor< MASingleBody, MAMultiDamage >::SetEmissiveConstants( m_fCurrentEmissive, 0.0f, 0.0f );

			// 강제로 0.5초동안 0으로 만들 감속값을 구해 뺀다.
			m_fCurrentEmissive -= m_fDeltaTime * fDelta / 0.5f;
		}
		break;
	case ChangeEmissiveStep::EmissiveIncrease:
		{
			// 매프레임 Power값 조절로만 줄여나간다.
			TDnMonsterActor< MASingleBody, MAMultiDamage >::SetEmissiveConstants( m_fCurrentEmissive, 0.0f, 0.0f );

			// 강제로 0.5초동안 맥스값으로 만들어놓는다.
			m_fCurrentEmissive += (m_fEmissivePower + m_fEmissivePowerRange) * fDelta / 0.5f;
			if( m_fCurrentEmissive > m_fEmissivePower + m_fEmissivePowerRange )
			{
				m_nChangeEmissiveStep = ChangeEmissiveStep::EmissiveStandBy;
			}
		}
		break;
	case ChangeEmissiveStep::EmissiveStandBy:
		{
			// 맥스치로 유지하다가 상수들 정상값으로 복구
			if( cos( g_fTotalElapsedTime * m_fEmissiveAniSpeed ) > 0.95f )
			{
				TDnMonsterActor< MASingleBody, MAMultiDamage >::SetEmissiveConstants( m_fEmissivePower, m_fEmissivePowerRange, m_fEmissiveAniSpeed );
				m_nChangeEmissiveStep = ChangeEmissiveStep::None;
			}
		}
		break;
	}
#else
	if( m_bChangeEmissive )
	{
		float fEmissive = m_fEmissivePower + m_fEmissivePowerRange * cos( g_fTotalElapsedTime * m_fEmissiveAniSpeed );
		if( fEmissive <= 0.05f || m_bImmediatelyChangeEmissive )
		{
			TDnMonsterActor< MASingleBody, MAMultiDamage >::ChangeEmissiveTexture( m_szChangeEmissiveTexture );
			m_szChangeEmissiveTexture = "";
			m_bImmediatelyChangeEmissive = false;
			m_bChangeEmissive = false;
		}
	}
#endif
	

#ifdef PRE_ADD_DAMAGEDPARTS
	// #60073
	if( m_bUseDamagePartsEmissiveColor )
	{		
		int i = 0;		
		std::map<std::string, float>::iterator it = m_mapDamagedEmissivePower.begin();
		for( ; it != m_mapDamagedEmissivePower.end(); ++it, ++i )
		{
			// Power값 다운.
			float & rPower = it->second;
			if( rPower > 0.0f )
				rPower -= fDelta *0.5f;
			if( rPower < 0.0f )
				rPower = 0.0f;
			
			int nSubMeshIdx = m_hObject->GetSubMeshIndex( it->first.c_str() );			
			m_hObject->SetCustomParam( m_nDamagedEmissiveParamIndex, &rPower, nSubMeshIdx );
		}
	}

	if( m_bUseHPPercentEmissiveColor )
	{			
		// HP.
		int nSize = GetPartsSize();
		for( int k=0; k<nSize; ++k )
		{
			MonsterParts * pParts = GetPartsByIndex( k );
			if( pParts->GetPartsInfo().vParts.size() > 0 )
			{
				int nSubMeshIdx = m_hObject->GetSubMeshIndex( pParts->GetPartsInfo().vParts[ 0 ].szMeshName.c_str() );

				float fMaxHP = static_cast<float>( pParts->GetMaxHP() );
				float fCurHP = static_cast<float>( pParts->GetHP() );

				if( fMaxHP > 0.f )
				{			
					float fHPRatio = 1.0f - ( fCurHP / fMaxHP );
					m_hObject->SetCustomParam( m_nHPPercentEmissiveParamIndex, &fHPRatio, nSubMeshIdx );
				}	
			}			
		}// for.				
	}
#endif


	CmdMixDestroyPartsAction();
	RefreshStateColor();
}

void CDnPartsMonsterActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_ChangeEmissiveTexture:
			{
#ifdef PRE_FIX_CHANGE_EMISSIVE
				// 에미시브를 제대로 구할 수 있는(에미시브가 적용된) 스킨인지 확인 후
				if( m_fEmissivePower == 0.0f && m_fEmissivePowerRange == 0.0f && m_fEmissiveAniSpeed == 0.0f )
				{
					// 에미시브 상수값 원본을 기억해둔다.
					if( TDnMonsterActor< MASingleBody, MAMultiDamage >::GetEmissiveConstants( m_fEmissivePower, m_fEmissivePowerRange, m_fEmissiveAniSpeed ) )
					{
						m_bExistEmissive = true;
					}
				}

				// 에미시브가 있을때만 아래를 수행.
				if( m_bExistEmissive )
				{
					ChangeEmissiveTextureStruct* pStruct = static_cast<ChangeEmissiveTextureStruct*>(pPtr);
					m_szChangeEmissiveTexture = pStruct->szEmissiveTexture;
					m_bImmediatelyChangeEmissive = (pStruct->bImmediately != 0);
					m_bImmediatelyChangeEmissive = false;

					// 현재값을 기억했다가 감소시키는데 사용.
					// 스탭이 None일때는 현재 렌더링되고 있는 에미시브값을 계산해서 구하고,
					// 진행중인 스탭이 있을 경우엔 m_fCurrentEmissive 값 그대로 유지해서 스탭을 진행한다.
					if( m_nChangeEmissiveStep == ChangeEmissiveStep::None )
						m_fCurrentEmissive = m_fEmissivePower + m_fEmissivePowerRange * cos( g_fTotalElapsedTime * m_fEmissiveAniSpeed );
					m_fDeltaTime = max(m_fCurrentEmissive, 0.0f);

					m_nChangeEmissiveStep = ChangeEmissiveStep::EmissiveDecrease;
				}
#else
				m_fEmissivePower = m_fEmissivePowerRange = m_fEmissiveAniSpeed = 0.0f;
				if( TDnMonsterActor< MASingleBody, MAMultiDamage >::GetEmissiveConstants( m_fEmissivePower, m_fEmissivePowerRange, m_fEmissiveAniSpeed ) )
				{
					ChangeEmissiveTextureStruct* pStruct = static_cast<ChangeEmissiveTextureStruct*>(pPtr);
					m_szChangeEmissiveTexture = pStruct->szEmissiveTexture;
					m_bImmediatelyChangeEmissive = (pStruct->bImmediately != 0);
					m_bChangeEmissive = true;
				}
#endif
				break;
			}
	}

	TDnMonsterActor< MASingleBody, MAMultiDamage >::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

void CDnPartsMonsterActor::CmdMixDestroyPartsAction()
{
	std::map<int, SDestroyActorActionInfo>::iterator iter = m_DestroyActionInfoList.begin();
	for (; iter != m_DestroyActionInfoList.end(); ++iter)
	{
		SDestroyActorActionInfo& info = (*iter).second;
		if (info.curState != DPS_None)
		{
			bool bDestroyed = (info.curState == DPS_Destroy);
			if (info.names[ActionName].empty() == false)
				CmdMixedAction(info.names[ActionBoneName].c_str(), info.names[FixBoneName].c_str(), info.names[ActionName].c_str(), bDestroyed ? 1 : 0, 0.f, 0.f);
			if (info.curState == DPS_Regeneration)
				info.curState = DPS_None;
		}
	}
}

void CDnPartsMonsterActor::RefreshStateColor()
{
	std::map<int, SDestroyActorActionInfo>::const_iterator iter = m_DestroyActionInfoList.begin();
	for (; iter != m_DestroyActionInfoList.end(); ++iter)
	{
		const SDestroyActorActionInfo& info = (*iter).second;
		if (info.curState == DPS_Destroy)
		{
			if (info.meshName.empty() == false)
			{
				EtColor DieDiffuse = info.destroyDiffuse;
				EtColor DieAmbient = info.destroyAmbient;
				int nSubMeshIdx = m_hObject->GetSubMeshIndex(info.meshName.c_str());
				m_hObject->SetCustomParam( m_nDiffuseParamIndex, &DieDiffuse, nSubMeshIdx );
				m_hObject->SetCustomParam( m_nAmbientParamIndex, &DieAmbient, nSubMeshIdx );
			}
		}
	}
}

bool CDnPartsMonsterActor::Initialize()
{
	TDnMonsterActor< MASingleBody, MAMultiDamage >::Initialize();
	LoadMultiDamageInfo();

	m_nDiffuseParamIndex = m_hObject->AddCustomParam( "g_MaterialDiffuse" );
	m_nAmbientParamIndex = m_hObject->AddCustomParam( "g_MaterialAmbient" );

#ifdef PRE_ADD_DAMAGEDPARTS
	// #60073
	m_nDamagedEmissiveParamIndex = m_hObject->AddCustomParam( "g_EmissiveDamagePower" ); 
	m_nHPPercentEmissiveParamIndex = m_hObject->AddCustomParam( "g_EmissiveHPPower" );
#endif

#ifdef PRE_ADD_CHECK_PART_MONSTER
	CheckEnablePartsHpDisplay();
#endif

	return true;
}

#ifdef PRE_ADD_CHECK_PART_MONSTER
void CDnPartsMonsterActor::CheckEnablePartsHpDisplay()
{
	if(!m_vEnablePartsGaugeList.empty())
	{
		for(int i = 0 ; i < (int)m_vEnablePartsGaugeList.size() ; ++i)
		{
			if(m_vEnablePartsGaugeList[i] == 1)
			{
				// Parts중 한개라도 Enable이면
				m_bIsEnablePartsHPDisplay = true;
				break;
			}
		}
		m_vEnablePartsGaugeList.clear();
		std::vector<int>().swap(m_vEnablePartsGaugeList);
	}
}
#endif

void CDnPartsMonsterActor::OnMultiDamage(MonsterParts& Parts)
{
	DNVector(MonsterParts::_PartsInfo)& vecParts = const_cast<MonsterParts::_Info&>(Parts.GetPartsInfo()).vParts;
	for (UINT j = 0 ; j < vecParts.size() ; ++j)
	{
		SetMeshAmbientColor(vecParts[j].szMeshName.c_str(), m_DamageBlinkColor);
	
#ifdef PRE_ADD_DAMAGEDPARTS
		// #60073		
		std::map<std::string, float>::iterator it = m_mapDamagedEmissivePower.find( vecParts[j].szMeshName.c_str() ); // #60073
		if( it != m_mapDamagedEmissivePower.end() )
		{
			float & rPower = (it->second);
			rPower = 1.0f;
		}
		else
		{			
			m_mapDamagedEmissivePower.insert( std::map<std::string, float>::value_type(vecParts[j].szMeshName, 1.0f) );
		}
#endif

	}	

	m_fDamageBlinkColorDelta = m_fDamageBlinkColorTime;
}

void CDnPartsMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
#ifdef PRE_ADD_LOTUSGOLEM
	if (HitParam.bIgnoreShowDamage == false)
#endif
	m_fDamageShakeDelta = m_fDamageShakeTime;
	TDnMonsterActor< MASingleBody, MAMultiDamage >::OnDamage(pHitter, HitParam);
}

void CDnPartsMonsterActor::OnAddMonsterParts(const MonsterParts::_Info& partsInfo)
{
	DNTableFileFormat* pPartsSox = GetDNTable( CDnTableDB::TMONSTERPARTS );
	if (!pPartsSox)
		return;

	SDestroyActorActionInfo destroyPartsActionInfo;
	if (partsInfo.szDeadActorActName.empty() == false &&
		partsInfo.szDeadActorActBoneName.empty() == false&&
		partsInfo.szDeadActorFixBoneName.empty() == false)
	{
		destroyPartsActionInfo.Set(partsInfo.szDeadActorActName, partsInfo.szDeadActorActBoneName, partsInfo.szDeadActorFixBoneName);
	}
	destroyPartsActionInfo.meshName = pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_MeshName1" )->GetString();
	destroyPartsActionInfo.destroyAmbient = EtColor(pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_DeadActorAmbient_R")->GetFloat(), 
		pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_DeadActorAmbient_G")->GetFloat(), 
		pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_DeadActorAmbient_B")->GetFloat(),
		1.f);
	destroyPartsActionInfo.destroyDiffuse = EtColor(pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_DeadActorDiffuse_R")->GetFloat(), 
		pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_DeadActorDiffuse_G")->GetFloat(), 
		pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_DeadActorDiffuse_B")->GetFloat(),
		1.f);
	m_DestroyActionInfoList.insert(std::make_pair(partsInfo.uiMonsterPartsTableID, destroyPartsActionInfo));

	//	note by kalliste : 몬스터 당 데미지 컬러 셋팅은 하나지만, 기획팀 관리 편의상 파츠테이블에 넣기로 하여 다음과 같이 셋팅함.
	m_fDamageBlinkColorTime		= pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_ToHitBlinkTime")->GetFloat();
	m_fDamageShakeTime			= pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_ToHitShakeTime")->GetFloat();

	m_fDamageScaleFactor		= pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_Scaling")->GetFloat();

	m_DamageBlinkColor = EtColor(pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_ToHitBlinkRed")->GetFloat(), 
		pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_ToHitBlinkGreen")->GetFloat(), 
		pPartsSox->GetFieldFromLablePtr( partsInfo.uiMonsterPartsTableID, "_ToHitBlinkBlue")->GetFloat(),
		1.f);

#ifdef PRE_ADD_CHECK_PART_MONSTER
	int nResult = 0;
	if( partsInfo.m_bEnalbeUI ) nResult = 1;
	m_vEnablePartsGaugeList.push_back(nResult);
#endif
}

void CDnPartsMonsterActor::OnDestroyParts(const MonsterParts& partsInfo, int nPartsIndex)
{
	std::map<int, SDestroyActorActionInfo>::iterator iter = m_DestroyActionInfoList.find(partsInfo.GetPartsTableID());
	if (iter != m_DestroyActionInfoList.end())
	{
		SDestroyActorActionInfo& info = (*iter).second;
		info.curState = DPS_Destroy;

		if (partsInfo.GetPartsInfo().szDeadActorActName.empty() == false)
			SetActionQueue(partsInfo.GetPartsInfo().szDeadActorActName.c_str());
	}
}

void CDnPartsMonsterActor::OnRefreshParts(const MonsterParts& partsInfo, int nPartsIndex)
{
	std::map<int, SDestroyActorActionInfo>::iterator iter = m_DestroyActionInfoList.find(partsInfo.GetPartsTableID());
	if (iter != m_DestroyActionInfoList.end())
	{
		SDestroyActorActionInfo& info = (*iter).second;
		info.curState = DPS_Regeneration;

		if (partsInfo.GetPartsInfo().szDeadActorActName.empty() == false)
			SetActionQueue("Stand");
	}
}

void CDnPartsMonsterActor::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
{
	switch( dwActorProtocol ) {
		case eActor::SC_MONSTERPARTS_STATE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int iPartsTableID	= 0;
				int iState			= 0;		
				int nIndex = 0;

				Stream.Read( &nIndex, sizeof(int) );	// Parts 인덱스 추가 [2011/05/11 semozz]
				Stream.Read( &iPartsTableID, sizeof(int) );
				Stream.Read( &iState, sizeof(int) );

				if (nIndex < 0 || nIndex >= (int)m_Parts.size())
				{
					//인덱스 범위 벗어남..
					OutputDebug("SC_MONSTERPARTS_STATE Index 범위 이상...!!!\n");
				}
				else
				{
#if defined( PRE_ADD_LOTUSGOLEM )
					MonsterParts::_Info& PartsInfo = const_cast<MonsterParts::_Info&>(m_Parts[nIndex].GetPartsInfo());
					PartsInfo.PartsState = iState;
#endif	// #if defined( PRE_ADD_LOTUSGOLEM )
				}
			}
			return;
	}

	__super::OnDispatchMessage(dwActorProtocol, pPacket);
}