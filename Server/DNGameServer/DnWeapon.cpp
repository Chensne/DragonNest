#include "StdAfx.h"
#include "DnWeapon.h"
#include "DnTableDB.h"
#include "MAActorRenderBase.h"
#include "DnProjectile.h"
#include "DnMonsterActor.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DNGameDataManager.h"
#endif

#if defined(PRE_FIX_65287)
#include "DnBlow.h"
#endif // PRE_FIX_65287

DECL_DN_SMART_PTR_STATIC( CDnWeapon, MAX_SESSION_COUNT, 100 )

STATIC_DECL_INIT( CDnWeapon, DWORD, s_dwUniqueCount ) = { 0, };

CDnWeapon::CDnWeapon( CMultiRoom *pRoom, bool bProcess, bool bIncreaseUniqueID )
: CDnUnknownRenderObject( pRoom, bProcess )
, CDnItem( pRoom )
{
	SetWeaponType( WeaponTypeEnum::Weapon );

	m_pParentWeapon = NULL;
	EtMatrixIdentity( &m_matWorld );
	CDnActionBase::Initialize( this );
	m_nLength = 0;
	m_nOriginalLength = 0;
	m_bLinkBone = false;
	m_dwUniqueID = -1;

	m_nDurability = m_nMaxDurability = 0;
	m_bDestroyThis = false;
	m_bCreateObject = false;

	// 문제점
	// 나중에 무기나 기타등등 변경하거나 할시에 다른 클라이언트들과 UniqueID 가 틀려지는 경우가 생길것이다.
	// (특히 Projectile쪽) 그때는 여기서 UniqueID 를 결정하지 말고 UniqueID로 검색해야 되는 객체들( 지금은 Projectile 밖엔없다. 혹은 아이템 던지기)
	// 을 생성할때 UniqueID 를 늘려주면서 셋팅해주고 생성시점에 UniqueID 를 가치 보내줘서 꼬이는 경우를 고치자.
	if( bIncreaseUniqueID ) {
		SetUniqueID( STATIC_INSTANCE(s_dwUniqueCount) );
		STATIC_INSTANCE(s_dwUniqueCount)++;
	}
	else SetUniqueID( -1 );

	m_nAniIndex = -1;
	m_nBlendAniIndex = -1;
	m_fBlendAniFrame = 0.f;
	m_fBlendFrame = 0.f;
	m_fBlendStartFrame = 0.f;

	m_nSetItemID = 0;
#if defined( PRE_ADD_OVERLAP_SETEFFECT )	
	m_bOverLapSet = false;
#endif

	m_eElement = ElementEnum_Amount;

	m_RecreateCashEquipType = (EquipTypeEnum)-1;
}

CDnWeapon::~CDnWeapon()
{
	FreeObject();
}

bool CDnWeapon::Initialize( int nWeaponTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound )
{
	m_nClassID = nWeaponTableID;
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TWEAPON );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"WeaponTable.ext failed\r\n");
		return false;
	}
	if( !pSox->IsExistItem( m_nClassID ) ) return false;

	m_nLength = pSox->GetFieldFromLablePtr( nWeaponTableID, "_Length" )->GetInteger();
	m_nDurability = m_nMaxDurability = pSox->GetFieldFromLablePtr( nWeaponTableID, "_Durability" )->GetInteger();
	m_EquipType = (EquipTypeEnum)pSox->GetFieldFromLablePtr( nWeaponTableID, "_EquipType" )->GetInteger();
	m_nOriginalLength = m_nLength;
	m_nSetItemID = pSox->GetFieldFromLablePtr( nWeaponTableID, "_SetItemID" )->GetInteger();
#if defined( PRE_ADD_OVERLAP_SETEFFECT )	
	m_bOverLapSet = pSox->GetFieldFromLablePtr( nWeaponTableID, "_SetOverlap" )->GetInteger() ? true : false;
#endif

	// Item 정보 Initialize
	if( CDnItem::Initialize( nWeaponTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound ) == false ) return false;

	// 무기는 기본 능력치에서 속성이 있을경우 기본속성으로 설정된다.
	for( int i=0; i<ElementEnum_Amount; i++ ) {
		if( GetElementAttack( (ElementEnum)i ) > 0 ) {
			m_eElement = (ElementEnum)i;
			break;
		}
	}

	return true;
}

void CDnWeapon::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bDestroyThis ) {
		if( m_hActor ) {
			for( int i=0; i<2; i++ ) {
				if( m_hActor->GetWeapon(i) == this ) {
					m_hActor->DetachWeapon(i);
					return;
				}
			}
		}
		else {
			delete this;
		}
		return;
	}
	if( m_hObject ) {
		if( m_bLinkBone ) {
			m_matWorld = *m_hObject->GetWorldMat();
		}
		else {
			m_hObject->Update( &m_matWorld );
		}
	}

	CDnActionBase::ProcessAction( LocalTime, fDelta );
	Process( &m_matWorld, LocalTime, fDelta );
}

void CDnWeapon::Update( EtMatrix *matWorld )
{
	m_matWorld = *matWorld;
}

void CDnWeapon::ChangeAnimation( int nAniIndex, float fFrame, float fBlendFrame )
{
	m_nBlendAniIndex = m_nAniIndex;
	m_fBlendStartFrame = fFrame;
	m_fBlendFrame = fBlendFrame;
	if( m_fBlendFrame == 0.f ) m_nBlendAniIndex = -1;

	if( m_nBlendAniIndex != -1 ) {
		m_fBlendAniFrame = m_fFrame;

		float fAniLength = (float)m_hObject->GetLastFrame( m_nBlendAniIndex );
		if( m_fBlendAniFrame >= fAniLength ) {
			m_fBlendAniFrame = fAniLength;
		}
	}

	m_nAniIndex = nAniIndex;
}

void CDnWeapon::Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hObject ) return;
	if( m_nActionIndex == -1 ) return;
	if( m_nActionIndex >= (int)m_nVecAniIndexList.size() ) return;
	if( m_nAniIndex == -1 ) return;
	m_hObject->SetAniFrame( m_nAniIndex, m_fFrame );
}

int CDnWeapon::GetAniIndex( const char *szAniName )
{
	if( !m_hObject ) return -1;
	if( !m_hObject->GetAniHandle() ) return -1;
	for( int i=0; i<m_hObject->GetAniCount(); i++ ) {
		if( strcmp( m_hObject->GetAniName(i), szAniName ) == NULL ) return i;
	}
	return -1;
}

int CDnWeapon::GetBoneIndex( const char *szBoneName )
{
	if( !m_hObject ) return -1;
	EtAniHandle handle = m_hObject->GetAniHandle();
	return handle->GetBoneIndex( szBoneName );
}

void CDnWeapon::ShowWeapon( bool bShow )
{
}

void CDnWeapon::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_Projectile:
			{
				//// 스킬로 나갔던 이 발사체에서 또 발사체를 쏜다면(디스토션 애로우, 레볼루셔너리 바리스타) 쏜 액터의 능력치를 계승시켜 준다.
				ProjectileStruct *pStruct = (ProjectileStruct *)pPtr;
				DnActorHandle hActor;

				// Note: 한기 - 서버쪽에선 무기의 월드 매트릭스가 단위 행렬값임.... 계산 따로 안하는 듯. 
				// 그래서 겜 서버쪽 무기 발사체 시그널에선 액터의 월드 매트릭스 값을 넘겨줌. 
				// 중력값 먹는 Projectile/TargetPosition/Accell 타입의 발사체를 설정할 시엔 이 부분이 고려되어야 함..

				// 몬스터가 발사체에서 발사체를 쏘는 경우가 생김. (#15557)
				// 그런 경우엔 날아가고 있는 발사체의 월드 행렬을 기점으로 발사시킨다. 상태효과도 계승.
				CDnProjectile *pProjectile = NULL;
				MatrixEx LocalCross;
				bool bMonsterProjectileShootProjectile = false;
				if( (WeaponTypeEnum::Projectile == GetWeaponType()) )
				{
					CDnProjectile* pMyselfProjectile = static_cast<CDnProjectile*>(this);
					hActor = pMyselfProjectile->GetShooterActor();

					if( hActor )
					{
						if( hActor->IsMonsterActor() )
						{
							LocalCross = *pMyselfProjectile->GetMatEx();
							pProjectile = CDnProjectile::CreateProjectile( GetRoom(), hActor, LocalCross, pStruct );
							if( pProjectile == NULL ) 
								break;
							pProjectile->SetShooterType( GetMySmartPtr(), m_nActionIndex, nSignalIndex );

							if( pMyselfProjectile->GetShooterStateSnapshot() )
								pProjectile->SetShooterStateSnapshot( pMyselfProjectile->GetShooterStateSnapshot() );

#if defined(PRE_FIX_65287)

							float fFinalDamageRate = 0.0f;
							if (hActor && hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_050))
							{
								DNVector(DnBlowHandle) vlhBlows;
								hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_050, vlhBlows );
								int iNumBlow = (int)vlhBlows.size();
								for( int i = 0; i < iNumBlow; ++i )
								{
									fFinalDamageRate += vlhBlows[i]->GetFloatValue();
								}
							}

							pProjectile->SetShooterFinalDamageRate(fFinalDamageRate);
#endif // PRE_FIX_65287

#if defined(PRE_ADD_50903)
							//발사체에서 발사체를 쏘는 경우 부모 발사체에 있던 상태효과들을 등록한다..
							pProjectile->ApplyParentProjectile(pMyselfProjectile);
							//발사체 -> 발사체 발사 할때 스킬에 의한 발사체인지 부모 발사체에서 정보를 받아서 설정 해줘야 함.
							pProjectile->FromSkill(pMyselfProjectile->IsFromSkill());

							if (pMyselfProjectile->GetParentSkill())
								pProjectile->SetParentSkill(pMyselfProjectile->GetParentSkill());
#endif // PRE_ADD_50903
						
							bMonsterProjectileShootProjectile = true;

							CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
							pMonsterActor->SendProjectileFromProjectile( pProjectile, nSignalIndex );
						}
					}
					else
						// 몬스터 액터가 쏜 발사체에서 발사체를 쏘는 건데 몬스터 객체가 유효하지 않은 경우임.. 
						// 플레이어일 때 처리되는 밑에 루틴 돌지 않도록 플래그 켜준다.
						bMonsterProjectileShootProjectile = true;
				}
				
				if( false == bMonsterProjectileShootProjectile )
				{
					hActor = m_hActor;
					if( !m_hActor ) 
						break;

					LocalCross = *hActor->GetMatEx();
					pProjectile = CDnProjectile::CreateProjectile( GetRoom(), hActor, LocalCross, pStruct );
					if( pProjectile == NULL ) 
						break;

					pProjectile->SetShooterType( GetMySmartPtr(), m_nActionIndex, nSignalIndex );
					hActor->OnProjectile( pProjectile, pStruct, LocalCross, nSignalIndex );
					hActor->OnSkillProjectile( pProjectile );
				}
			}
			break;
		case STE_Destroy:
			if( IsProcess() ) SetDestroy();
			else m_bDestroyThis = true;
			break;
	}
}

void CDnWeapon::OnHitSuccess( DnActorHandle hTarget, HitTypeEnum HitType, EtVector3 &vHitPosition, bool bPlaySound )
{
}

void CDnWeapon::LinkWeapon( DnActorHandle hActor, int nEquipIndex )
{
	m_hActor = hActor;

	if( !m_hObject ) return;
	m_bLinkBone = true;

	if( GetElementIndex( "Idle" ) != -1 )
		SetAction( "Idle", 0.f, 0.f );
}

void CDnWeapon::LinkWeapon( DnActorHandle hActor, CDnWeapon *pWeapon )
{
	m_hActor = hActor;
	m_pParentWeapon = pWeapon;
	m_bLinkBone = true;

	if( GetElementIndex( "Idle" ) != -1 )
		SetAction( "Idle", 0.f, 0.f );
}

void CDnWeapon::UnlinkWeapon()
{
	m_bLinkBone = false;
	m_pParentWeapon = NULL;
}

CDnWeapon &CDnWeapon::operator = ( CDnWeapon &e )
{
	m_WeaponType = e.m_WeaponType;
	m_nClassID = e.m_nClassID;
	m_nLength = e.m_nLength;
	m_EquipType = e.m_EquipType;
	m_szFileName = e.m_szFileName;
	if( e.m_hObject ) {
		m_hObject = EternityEngine::CreateAniObject( GetRoom(), e.m_hObject->GetSkin()->GetFullName(), ( e.m_hObject->GetAniHandle() ) ? e.m_hObject->GetAniHandle()->GetFullName() : NULL );
	}
	LoadAction( CDnActionBase::m_szFileName.c_str() );



	return *this;
}

bool CDnWeapon::InitializeClass( CMultiRoom *pRoom )
{
	STATIC_INSTANCE_(s_dwUniqueCount) = 0;
	return true;
}

void CDnWeapon::ProcessClass( CMultiRoom *pRoom, LOCAL_TIME LocalTime, float fDelta )
{
	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) {
		STATIC_INSTANCE_(s_pVecProcessList)[i]->Process( LocalTime, fDelta );
		if( STATIC_INSTANCE_(s_pVecProcessList)[i]->IsDestroy() ) 
		{
			CDnWeapon* pWeapon = STATIC_INSTANCE_(s_pVecProcessList)[i];
			SAFE_DELETE(pWeapon);
			i--;
		}
	}
}

void CDnWeapon::ReleaseClass( CMultiRoom *pRoom )
{
	DeleteAllObject( pRoom );
	STATIC_INSTANCE_(s_pVecProcessList).clear();
}

DnWeaponHandle CDnWeapon::FindWeaponFromUniqueID( CMultiRoom *pRoom, DWORD dwUniqueID )
{
	if( dwUniqueID == -1 ) return CDnWeapon::Identity();	
	DnWeaponHandle hWeapon;
	for( int i=0; i<GetItemCount(pRoom); i++ ) {
		hWeapon = GetItem(pRoom, i);
		if( !hWeapon ) continue;
		if( hWeapon->GetUniqueID() == dwUniqueID ) return hWeapon;
	}
	return CDnWeapon::Identity();
}

DnWeaponHandle CDnWeapon::FindWeaponFromUniqueIDAndShooterUniqueID( CMultiRoom *pRoom, DWORD dwUniqueID, DWORD dwShooterUniqueID )
{
	if( dwUniqueID == -1 ) return CDnWeapon::Identity();	
	DnWeaponHandle hWeapon;
	for( int i=0; i<GetItemCount(pRoom); i++ ) {
		if( !IsValidItem( pRoom, i ) )
		{
			g_Log.Log( LogType::_WEAPONTYPE, L"IsValidItem() 이상!!!\r\n" );
			continue;
		}
		hWeapon = GetItem(pRoom, i);
		if( !hWeapon ) continue;
		if( hWeapon->GetUniqueID() == dwUniqueID ) {
			if( !( hWeapon->GetWeaponType() & CDnWeapon::Projectile ) ) continue;
			if( ((CDnProjectile*)hWeapon.GetPointer())->GetShooterActor() &&
				((CDnProjectile*)hWeapon.GetPointer())->GetShooterActor()->GetUniqueID() == dwShooterUniqueID ) return hWeapon;
		}
	}
	return CDnWeapon::Identity();
}

DnWeaponHandle CDnWeapon::CreateWeapon( CMultiRoom *pRoom, int nWeaponTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, bool bProcess, bool bIncreaseUniqueID, char cSealCount, bool bSoulBound )
{
	DnWeaponHandle hWeapon = (new IBoostPoolDnWeapon( pRoom, bProcess, bIncreaseUniqueID ))->GetMySmartPtr();
	hWeapon->Initialize( nWeaponTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound );
	return hWeapon;
}

void CDnWeapon::CreateObject()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
	if( !pSox->IsExistItem( m_nClassID ) ) return;

#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat* pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	if (pFileNameSox == NULL || g_pDataManager == NULL) return;

	std::string szSkinName, szAniName, szActName;
	g_pDataManager->GetFileNameFromFileEXT(szSkinName, pSox, m_nClassID, "_SkinName", pFileNameSox);
	g_pDataManager->GetFileNameFromFileEXT(szAniName, pSox, m_nClassID, "_AniName", pFileNameSox);
	g_pDataManager->GetFileNameFromFileEXT(szActName, pSox, m_nClassID, "_ActName", pFileNameSox);
#else
	std::string szSkinName = pSox->GetFieldFromLablePtr( m_nClassID, "_SkinName" )->GetString();
	std::string szAniName = pSox->GetFieldFromLablePtr( m_nClassID, "_AniName" )->GetString();
	std::string szActName = pSox->GetFieldFromLablePtr( m_nClassID, "_ActName" )->GetString();
#endif

	if( !szSkinName.empty() ) {
		m_hObject = EternityEngine::CreateAniObject( GetRoom(), CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), ( szAniName.empty() ) ? NULL : CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
		if( !m_hObject ) return;
	}

	if( !szActName.empty() ) LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );

	if( !szActName.empty() ) {
		if( GetElementIndex( "Idle" ) != -1 )
			SetAction( "Idle", 0.f, 0.f );
	}
	m_bCreateObject = true;
}

void CDnWeapon::RecreateCashWeapon( DnActorHandle hActor, int nEquipIndex )
{
	if( !IsCreateObject() ) return;
	if( !hActor ) return;
	if( !hActor->IsPlayerActor() ) return;
	if( nEquipIndex < 0 || nEquipIndex > 1 ) return;
	if( !IsCashItem() ) return;

	DnWeaponHandle hMainWeapon = hActor->GetWeapon( nEquipIndex );
	if( !hMainWeapon ) return;
	if( hMainWeapon->GetEquipType() == m_RecreateCashEquipType ) return;
	switch( hMainWeapon->GetEquipType() ) {
		case Gauntlet:
		case Staff:
		case Shield:
		case Arrow:
			return;
	}

	m_RecreateCashEquipType = hMainWeapon->GetEquipType();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
	if( !pSox->IsExistItem( m_nClassID ) ) return;

#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat* pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	if (!pFileNameSox)
	{
		_ASSERT(0);
		return;
	}
#endif

	static char *szSubStr[] = { "", "Sub1", "Sub2" };
	std::string szSkinName;
	std::string szAniName;
	std::string szActName;
	bool bValid = false;

	char szStr[64];
	for( int i=0; i<3; i++ ) {
		sprintf_s( szStr, "_EquipType%s", szSubStr[i] );
		int nEquipType = pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetInteger();
		if( nEquipType == m_RecreateCashEquipType ) {
#ifdef PRE_FIX_MEMOPT_EXT
			sprintf_s( szStr, "_SkinName%s", szSubStr[i] );
			g_pDataManager->GetFileNameFromFileEXT(szSkinName, pSox, m_nClassID, szStr, pFileNameSox);
			sprintf_s( szStr, "_AniName%s", szSubStr[i] );
			g_pDataManager->GetFileNameFromFileEXT(szAniName, pSox, m_nClassID, szStr, pFileNameSox);
			sprintf_s( szStr, "_ActName%s", szSubStr[i] );
			g_pDataManager->GetFileNameFromFileEXT(szActName, pSox, m_nClassID, szStr, pFileNameSox);
#else
			sprintf_s( szStr, "_SkinName%s", szSubStr[i] );
			szSkinName = pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString();
			sprintf_s( szStr, "_AniName%s", szSubStr[i] );
			szAniName = pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString();
			sprintf_s( szStr, "_ActName%s", szSubStr[i] );
			szActName = pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString();
#endif

			bValid = true;
			break;
		}
	}
	if( !bValid ) return;

	SAFE_RELEASE_SPTR( m_hObject );
	FreeAction();

	int nMainWeaponClassID = hMainWeapon->GetClassID();
#ifdef PRE_FIX_MEMOPT_EXT
	if( szSkinName.empty() ) g_pDataManager->GetFileNameFromFileEXT(szSkinName, pSox, nMainWeaponClassID, "_SkinName", pFileNameSox);
	if( szAniName.empty() ) g_pDataManager->GetFileNameFromFileEXT(szAniName, pSox, nMainWeaponClassID, "_AniName", pFileNameSox);
	if( szActName.empty() ) g_pDataManager->GetFileNameFromFileEXT(szActName, pSox, nMainWeaponClassID, "_ActName", pFileNameSox);
#else
	if( szSkinName.empty() ) szSkinName = pSox->GetFieldFromLablePtr( nMainWeaponClassID, "_SkinName" )->GetString();
	if( szAniName.empty() ) szAniName = pSox->GetFieldFromLablePtr( nMainWeaponClassID, "_AniName" )->GetString();
	if( szActName.empty() ) szActName = pSox->GetFieldFromLablePtr( nMainWeaponClassID, "_ActName" )->GetString();
#endif

	if( !szSkinName.empty() ) {
		m_hObject = EternityEngine::CreateAniObject( GetRoom(), CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), ( szAniName.empty() ) ? NULL : CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
	}

	if( !szActName.empty() ) {
		LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );
	}

	if( !szActName.empty() ) {
		if( GetElementIndex( "Idle" ) != -1 )
			SetAction( "Idle", 0.f, 0.f );
	}
}

void CDnWeapon::FreeObject()
{
	UnlinkWeapon();
	m_bCreateObject = false;
	m_RecreateCashEquipType = (EquipTypeEnum)-1;
	SAFE_RELEASE_SPTR( m_hObject );
	FreeAction();
}

void CDnWeapon::ReleaseClass( CMultiRoom *pRoom, WeaponTypeEnum WeaponType )
{
	CDnWeapon *pWeapon;
	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) {
		pWeapon = STATIC_INSTANCE_(s_pVecProcessList)[i];
		if( pWeapon->GetWeaponType() & WeaponType ) {
			SAFE_DELETE(pWeapon);
			i--;
		}
	}
}

CDnItem &CDnWeapon::operator = ( TItem &e )
{
	CDnItem::operator = (e);

	m_nDurability = e.wDur;

	return *this;
}

bool CDnWeapon::IsSubWeapon( CDnWeapon::EquipTypeEnum emType )
{
	switch( emType )
	{
	case CDnWeapon::Sword:
	case CDnWeapon::Axe:
	case CDnWeapon::Hammer:
	case CDnWeapon::SmallBow:
	case CDnWeapon::BigBow:
	case CDnWeapon::CrossBow:
	case CDnWeapon::Staff:
	case CDnWeapon::Mace:
	case CDnWeapon::Flail:
	case CDnWeapon::Wand:		
	case CDnWeapon::Cannon:
	case CDnWeapon::BubbleGun:	
	case CDnWeapon::Chakram:
	case CDnWeapon::Fan:
		return false;
	case CDnWeapon::Shield:
	case CDnWeapon::Arrow:
	case CDnWeapon::Book:
	case CDnWeapon::Orb:
	case CDnWeapon::Puppet:
	case CDnWeapon::Gauntlet:	
	case CDnWeapon::Glove:		
	case CDnWeapon::Charm:
		return true;
	default:
		ASSERT(0&&"CDnWeapon::IsSubWeapon");
		break;
	}
	return false;
}