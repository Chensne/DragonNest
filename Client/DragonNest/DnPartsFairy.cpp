#include "StdAfx.h"
#include "DnPartsFairy.h"
#include "DnTableDB.h"
#include "DnActor.h"
#include "DnPlayerActor.h"


CDnPartsFairy::CDnPartsFairy()
{
	m_EffectType = FairyEffectType::None;
#ifdef PRE_MOD_CASHFAIRY
	m_nLinkBoneIndexForCalc = -1;
#endif
}

CDnPartsFairy::~CDnPartsFairy()
{
}

bool CDnPartsFairy::Initialize( int nPartsTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID )
{
	if( CDnParts::Initialize( nPartsTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID ) == false ) return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TFAIRY );
	if( !pSox ) return false;

	const DNTableCell* pFairyTypeField = pSox->GetFieldFromLablePtr( nPartsTableID, "_FairyType" );
	const DNTableCell* pWeaponEffectActionField = pSox->GetFieldFromLablePtr( nPartsTableID, "_WeaponEffectAction" );
	if (pFairyTypeField == NULL || pWeaponEffectActionField == NULL)
		return false;

	m_EffectType = (FairyEffectType)pFairyTypeField->GetInteger();
	m_szChangeWeaponActionStr = pWeaponEffectActionField->GetString();

	return true;
}

bool CDnPartsFairy::CreateObject( DnActorHandle hActor, const char *szAniName )
{
	bool bResult = CDnParts::CreateObject( hActor, szAniName );

	switch( m_EffectType ) {
		case FairyEffectType::ChangeWeaponAction:
			// 원래는 CDnWeapon::GetWeaponMatchAction 쪽에서 Player 이고 Fairy 파츠가 있을 경우 m_szChangeWeaponActionStr 얻어서
			// 붙여주면 깔끔하지만, 그쪽 나름대로 플레이어일 경우, 파츠 체크 등등 어떻게보면 오히려 지저분하고 얻어오는 퍼포먼스가
			// 별루 안좋을듯하여 그냥 범용적으로 사용가능하도록 CDnActor 에 AddionalWeaponAction 맴버를 얻어다가 쓰는 식으로 구현합니다.
			if( !m_szChangeWeaponActionStr.empty() && hActor ) {
				hActor->SetAdditionalWeaponAction( m_szChangeWeaponActionStr );
				// 무기쪽에 ReleaseSignalImp 를 해줘야 남아있는 이펙트들이 삭제됩니다.
				hActor->ReleaseWeaponSignalImp();
			}
			break;
	}

	return bResult;
}

bool CDnPartsFairy::FreeObject()
{
	switch( m_EffectType ) {
#ifdef PRE_MOD_CASHFAIRY
		case FairyEffectType::None:
			m_nLinkBoneIndexForCalc = -1;
			break;
#endif
		case FairyEffectType::ChangeWeaponAction:
			if( !m_szChangeWeaponActionStr.empty() && m_hActor ) {

				DnActorHandle hActor = m_hActor;
				if( m_hActor->IsVehicleActor() )
				{
					CDnVehicleActor* pVehicle = static_cast<CDnVehicleActor*>(m_hActor.GetPointer());
					if( pVehicle->GetMyPlayerActor() )
						hActor = pVehicle->GetMyPlayerActor()->GetActorHandle();
				}

				hActor->SetAdditionalWeaponAction( std::string("") );
				// 무기쪽에 ReleaseSignalImp 를 해줘야 남아있는 이펙트들이 삭제됩니다.
				hActor->ReleaseWeaponSignalImp();
			}
			break;
	}

	return CDnParts::FreeObject();
}

#ifdef PRE_MOD_CASHFAIRY
void CDnPartsFairy::LinkFairy( DnActorHandle hActor )
{
	switch( m_EffectType ) {
		case FairyEffectType::None:
			{
				DnActorHandle hTempActor = hActor;
				CDnPlayerActor *pPlayerActor = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
				if( pPlayerActor && pPlayerActor->IsVehicleMode() && pPlayerActor->GetMyVehicleActor() )
					hTempActor = pPlayerActor->GetMyVehicleActor()->GetMySmartPtr();

				// 어차피 캐시템은 더미에 붙이는걸 알고 있으니 더미 기준으로 처리한다.
				m_nLinkBoneIndexForCalc = -1;
				MAActorRenderBase *pRender = dynamic_cast<MAActorRenderBase *>(hTempActor.GetPointer());
				if( pRender ) {
					int nBoneIndex = pRender->GetDummyBoneIndex( "#Cash_Spirit" );
					if( nBoneIndex != -1 ) {
						if( m_hObject ) {

							// 예외처리. 렌더아바타에선 예전처럼 처리한다.
							if( m_bRTTRenderMode )
							{
								m_hObject->SetParent( hTempActor->GetObjectHandle(), "#Cash_Spirit" );
								break;
							}

							m_nLinkBoneIndexForCalc = nBoneIndex;
							m_hParentObjectForCalc = hTempActor->GetObjectHandle();
							EtMatrixIdentity( &m_matCalculated );
							EtMatrixIdentity( &m_matCurrent );
							m_matCurrent._42 = -100000.0f;
						}
					}
				}
			}
			break;
	}
}

void CDnPartsFairy::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnParts::Process( LocalTime, fDelta );

	switch( m_EffectType ) {
		case FairyEffectType::None:
			{
				if( m_nLinkBoneIndexForCalc == -1 ) break;
				if( !m_hParentObjectForCalc )
				{
					if( m_hParentObjectForCalc != CEtObject::Identity() )
					{
						if( m_hActor && m_hActor->GetObjectHandle() )
							m_hParentObjectForCalc = m_hActor->GetObjectHandle();
					}
				}
				if( !m_hParentObjectForCalc ) break;

				// 원래 있어야할 포지션을 구하고,
				EtMatrix *pBoneMat;
				pBoneMat = m_hParentObjectForCalc->GetDummyTransMat( m_nLinkBoneIndexForCalc );
				if( pBoneMat )
				{
					EtMatrixMultiply( &m_matCalculated, pBoneMat, m_hParentObjectForCalc->GetWorldMat() );
				}

				// 현재 포지션은 그대로 유지한채 회전값은 항상 넘겨받고,
				memcpy_s( &m_matCurrent, sizeof(float)*12, &m_matCalculated, sizeof(float)*12 );

				// 이동값은
				EtVector3 vCalcuated = *(EtVector3*)&m_matCalculated._41;
				EtVector3 vCurrent = *(EtVector3*)&m_matCurrent._41;

				// 1000.0f 이상 차이나면 그대로 가져오고
				if( EtVec3Length( &( vCurrent - vCalcuated ) ) > 1000.0f )
				{
					memcpy_s( &m_matCurrent._41, sizeof(float)*4, &m_matCalculated._41, sizeof(float)*4 );
				}
				else
				{
					// 그 이하면 천천히 따라오게 한다.
					EtVector3 vLerp;
					EtVec3Lerp( &vLerp, &vCurrent, &vCalcuated, fDelta * 1.75f );
					memcpy_s( &m_matCurrent._41, sizeof(float)*3, &vLerp, sizeof(float)*3 );
				}

				// 마지막에 Update함수 강제 호출.
				m_hObject->Update( &m_matCurrent );
			}
			break;
	}
}
#endif