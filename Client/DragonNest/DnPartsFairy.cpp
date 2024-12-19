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
			// ������ CDnWeapon::GetWeaponMatchAction �ʿ��� Player �̰� Fairy ������ ���� ��� m_szChangeWeaponActionStr ��
			// �ٿ��ָ� ���������, ���� ������� �÷��̾��� ���, ���� üũ ��� ��Ժ��� ������ �������ϰ� ������ �����ս���
			// ���� ���������Ͽ� �׳� ���������� ��밡���ϵ��� CDnActor �� AddionalWeaponAction �ɹ��� ���ٰ� ���� ������ �����մϴ�.
			if( !m_szChangeWeaponActionStr.empty() && hActor ) {
				hActor->SetAdditionalWeaponAction( m_szChangeWeaponActionStr );
				// �����ʿ� ReleaseSignalImp �� ����� �����ִ� ����Ʈ���� �����˴ϴ�.
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
				// �����ʿ� ReleaseSignalImp �� ����� �����ִ� ����Ʈ���� �����˴ϴ�.
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

				// ������ ĳ������ ���̿� ���̴°� �˰� ������ ���� �������� ó���Ѵ�.
				m_nLinkBoneIndexForCalc = -1;
				MAActorRenderBase *pRender = dynamic_cast<MAActorRenderBase *>(hTempActor.GetPointer());
				if( pRender ) {
					int nBoneIndex = pRender->GetDummyBoneIndex( "#Cash_Spirit" );
					if( nBoneIndex != -1 ) {
						if( m_hObject ) {

							// ����ó��. �����ƹ�Ÿ���� ����ó�� ó���Ѵ�.
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

				// ���� �־���� �������� ���ϰ�,
				EtMatrix *pBoneMat;
				pBoneMat = m_hParentObjectForCalc->GetDummyTransMat( m_nLinkBoneIndexForCalc );
				if( pBoneMat )
				{
					EtMatrixMultiply( &m_matCalculated, pBoneMat, m_hParentObjectForCalc->GetWorldMat() );
				}

				// ���� �������� �״�� ������ä ȸ������ �׻� �Ѱܹް�,
				memcpy_s( &m_matCurrent, sizeof(float)*12, &m_matCalculated, sizeof(float)*12 );

				// �̵�����
				EtVector3 vCalcuated = *(EtVector3*)&m_matCalculated._41;
				EtVector3 vCurrent = *(EtVector3*)&m_matCurrent._41;

				// 1000.0f �̻� ���̳��� �״�� ��������
				if( EtVec3Length( &( vCurrent - vCalcuated ) ) > 1000.0f )
				{
					memcpy_s( &m_matCurrent._41, sizeof(float)*4, &m_matCalculated._41, sizeof(float)*4 );
				}
				else
				{
					// �� ���ϸ� õõ�� ������� �Ѵ�.
					EtVector3 vLerp;
					EtVec3Lerp( &vLerp, &vCurrent, &vCalcuated, fDelta * 1.75f );
					memcpy_s( &m_matCurrent._41, sizeof(float)*3, &vLerp, sizeof(float)*3 );
				}

				// �������� Update�Լ� ���� ȣ��.
				m_hObject->Update( &m_matCurrent );
			}
			break;
	}
}
#endif