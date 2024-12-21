#include "stdafx.h"
#include "DnTableDB.h"
#include "DnPetActor.h"
#include "DnPlayerActor.h"
#include "DnDropItem.h"
#include "DnPetTask.h"


CDnPetActor::CDnPetActor( int nClassID, bool bProcess )
: CDnActor( nClassID, bProcess )
, m_fPetRandomStandTime( PET_RANDOM_STAND_TIME )
, m_pMaster( NULL )
{
	CDnActionBase::Initialize( this );
	CDnActorState::Initialize( nClassID );

	for( int i=0; i<2; i++ )
		m_hSimpleParts[i].Identity();

	memset( &m_sPetCompact, 0, sizeof( m_sPetCompact ) );
}

CDnPetActor::~CDnPetActor()
{
	for( int i=0; i<2; i++ )
		SAFE_RELEASE_SPTR( m_hSimpleParts[i] );
	m_DoNotGetItemList.clear();
	ClearNotEatItemList();
}

bool CDnPetActor::Initialize()
{
	CDnActor::Initialize();
	return true;
}

void CDnPetActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnActor::ProcessActor( LocalTime, fDelta );
	PreProcess( LocalTime, fDelta );

	GetMovement()->Process( LocalTime, fDelta );
	MASingleBody::Process( m_matexWorld, LocalTime, fDelta );

	if( !IsMove() )
	{
		if( CDnPetTask::IsActive() )
		{
			if( ( m_sPetCompact.nType & Pet::Type::ePETTYPE_SATIETY && GetPetTask().GetSatietyPercent() > 0.0f )
				|| !( m_sPetCompact.nType & Pet::Type::ePETTYPE_SATIETY ) )	// 만복도 펫은 만복도값 0일 때는 노멀챗 사용안함
				GetPetTask().DoPetChat( PET_CHAT_NORMAL );
		}
		m_fPetRandomStandTime -= fDelta;
		if( m_fPetRandomStandTime <= 0.0f && strcmp( GetCurrentAction(), "Stand" ) == 0 )
		{
			int nRandom = _rand();
			if( nRandom % 3 == 0 )
				SetActionQueue( "Stand1", 0, 3.0f, 0.0f, true, false );
			else if( nRandom % 3 == 1 )
				SetActionQueue( "Stand2", 0, 3.0f, 0.0f, true, false );

			m_fPetRandomStandTime = PET_RANDOM_STAND_TIME;
		}
	}
	else
	{
		m_fPetRandomStandTime = PET_RANDOM_STAND_TIME;
	}

	if( m_pMaster )
	{
		if( m_pMaster->IsShow() )
			Show( true );
		else
			Show( false );
	}

	//펫 캐릭터 바라보기 기능 추가
	ProcessHeadLook(m_matexWorld, fDelta, IsSignalRange(STE_HeadLook));
	ProcessVisual( LocalTime, fDelta );
}
/*
void CDnPetActor::EquipItem( int nPartIndex )
{
	if(!GetObjectHandle() || IsDestroy() ) return;
	DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
	DNTableFileFormat* pItem = GetDNTable( CDnTableDB::TITEM );

	if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(nPartIndex) || !pItem || !pItem->IsExistItem( nPartIndex ) )
		return;

	int nPartsType = pVehiclePartsTable->GetFieldFromLablePtr( nPartIndex, "_VehiclePartsType" )->GetInteger();
	std::string szSkinName = pVehiclePartsTable->GetFieldFromLablePtr( nPartIndex, "_SkinName" )->GetString();
	int nSelectMesh = pVehiclePartsTable->GetFieldFromLablePtr( nPartIndex, "_SelectMesh" )->GetInteger();

	if( nPartsType == Pet::Parts::Default || !((nPartsType == Pet::Parts::PetAccessory1) || (nPartsType == Pet::Parts::PetAccessory2)) )
		return;

	if( m_hSimpleParts[nPartsType] )
	{
		m_hSimpleParts[nPartsType]->FreeObject();
		m_hSimpleParts[nPartsType]->LinkPartsToObject( szSkinName.c_str(),m_szAniFileName.c_str(),m_hObject );
	}
	else if( m_hSimpleParts[nPartsType] == NULL )
	{
		m_hSimpleParts[nPartsType] =(new CDnSimpleParts)->GetMySmartPtr();
		m_hSimpleParts[nPartsType]->LinkPartsToObject( szSkinName.c_str(),m_szAniFileName.c_str(),m_hObject );
	}

	if( GetObjectHandle()->GetSubMeshCount() == 3 )
	{
		if( nSelectMesh == 1 )
		{
			GetObjectHandle()->ShowSubmesh( 1, true );
			GetObjectHandle()->ShowSubmesh( 2, false );
		}
		else if( nSelectMesh == 2 )
		{
			GetObjectHandle()->ShowSubmesh( 1, false );
			GetObjectHandle()->ShowSubmesh( 2, true );
		}
	}
}
*/
void CDnPetActor::EquipItem( TVehicleItem tInfo )
{
	if( !GetObjectHandle() || IsDestroy() ) return;

	DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );

	if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem( tInfo.nItemID ) )
		return;

	int nPartsType = pVehiclePartsTable->GetFieldFromLablePtr( tInfo.nItemID, "_VehiclePartsType" )->GetInteger();
	std::string szSkinName = pVehiclePartsTable->GetFieldFromLablePtr( tInfo.nItemID, "_SkinName" )->GetString();
	int nSelectMesh = pVehiclePartsTable->GetFieldFromLablePtr( tInfo.nItemID, "_SelectMesh" )->GetInteger();

	if( nPartsType == Pet::Parts::Default || !((nPartsType == Pet::Parts::PetAccessory1) || (nPartsType == Pet::Parts::PetAccessory2)) )
		return;

	if( m_hSimpleParts[nPartsType] )
	{
#ifdef PRE_FIX_PET_UNEQUIP
		UnEquipItem((Pet::Parts::ePetParts)nPartsType);
#else
		m_hSimpleParts[nPartsType]->FreeObject();
#endif

		m_hSimpleParts[nPartsType]->LinkPartsToObject( szSkinName.c_str(),m_szAniFileName.c_str(),m_hObject );

		// 아이템 정보 설정
		m_hSimpleParts[nPartsType]->InitializeItem( tInfo.nItemID, 0 );
		m_hSimpleParts[nPartsType]->SetExpireDate( tInfo.tExpireDate );
		m_hSimpleParts[nPartsType]->SetEternityItem( tInfo.bEternity );
		m_hSimpleParts[nPartsType]->SetSerialID( tInfo.nSerial );
		m_hSimpleParts[nPartsType]->SetOverlapCount( tInfo.wCount );
	}
	else if( m_hSimpleParts[nPartsType] == NULL )
	{
		m_hSimpleParts[nPartsType] =(new CDnSimpleParts)->GetMySmartPtr();
		m_hSimpleParts[nPartsType]->LinkPartsToObject( szSkinName.c_str(),m_szAniFileName.c_str(),m_hObject );

		// 아이템 정보 설정
		m_hSimpleParts[nPartsType]->InitializeItem( tInfo.nItemID, 0 );
		m_hSimpleParts[nPartsType]->SetExpireDate( tInfo.tExpireDate );
		m_hSimpleParts[nPartsType]->SetEternityItem( tInfo.bEternity );
		m_hSimpleParts[nPartsType]->SetSerialID( tInfo.nSerial );
		m_hSimpleParts[nPartsType]->SetOverlapCount( tInfo.wCount );
	}

	if( nSelectMesh == 1 && nSelectMesh < GetObjectHandle()->GetSubMeshCount() )
	{
		for( int i=0; i<GetObjectHandle()->GetSubMeshCount(); i++ )
		{
			if( strstr( GetObjectHandle()->GetSubMeshName( i ), "Head_0" ) )
			{
				GetObjectHandle()->ShowSubmesh( i, false );
			}
			else if( strstr( GetObjectHandle()->GetSubMeshName( i ), "Head_1" ) )
			{
				GetObjectHandle()->ShowSubmesh( i, true );
			}
		}
	}
}

void CDnPetActor::UnEquipItem( Pet::Parts::ePetParts Type )
{
	if( Type == Pet::Parts::Default || !((Type == Pet::Parts::PetAccessory1) || (Type == Pet::Parts::PetAccessory2)) )
		return;

	if( m_hSimpleParts[Type] == NULL )
		return;

	if( Type == Pet::Parts::PetAccessory1 || Type == Pet::Parts::PetAccessory2 )
	{
		DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );

		if( !pVehiclePartsTable )
			return;

		int nSelectMesh = pVehiclePartsTable->GetFieldFromLablePtr( m_hSimpleParts[Type]->GetClassID(), "_SelectMesh" )->GetInteger();
		if( nSelectMesh == 1 && nSelectMesh < GetObjectHandle()->GetSubMeshCount() )
		{
			for( int i=0; i<GetObjectHandle()->GetSubMeshCount(); i++ )
			{
				if( strstr( GetObjectHandle()->GetSubMeshName( i ), "Head_0" ) )
				{
					GetObjectHandle()->ShowSubmesh( i, true );
				}
				else if( strstr( GetObjectHandle()->GetSubMeshName( i ), "Head_1" ) )
				{
					GetObjectHandle()->ShowSubmesh( i, false );
				}
			}
		}
	}

	m_hSimpleParts[Type]->FreeObject();
}

void CDnPetActor::SetDefaultParts()
{
	DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
	if( !pVehicleTable || !pVehicleTable->IsExistItem( m_nItemID ) )
		return;

	int nVehicleACtorDefaultPARTS_A = pVehicleTable->GetFieldFromLablePtr( m_nItemID, "_DefaultPartsA" )->GetInteger();
	int nVehicleACtorDefaultPARTS_B = pVehicleTable->GetFieldFromLablePtr( m_nItemID, "_DefaultPartsB" )->GetInteger();

	// 기본 파츠는 아이템 아이디에 대한 정보 밖에없습니다.
	UnEquipItem( Pet::Parts::PetAccessory1 );
	UnEquipItem( Pet::Parts::PetAccessory2 );

	TVehicleItem tItem;
	memset( &tItem, 0, sizeof(tItem) );

	tItem.nItemID = nVehicleACtorDefaultPARTS_A;
	EquipItem( tItem );

	tItem.nItemID = nVehicleACtorDefaultPARTS_B;
	EquipItem( tItem );

	DNTableFileFormat*  pItemTable = GetDNTable( CDnTableDB::TITEM );
	if( pItemTable )
	{
		DWORD dwPetDefaultColor1 = (DWORD)pItemTable->GetFieldFromLablePtr( m_nItemID , "_TypeParam1" )->GetInteger();
		DWORD dwPetDefaultColor2 = (DWORD)pItemTable->GetFieldFromLablePtr( m_nItemID , "_TypeParam2" )->GetInteger();

		ChangeColor( ePetParts::PET_PARTS_BODY, dwPetDefaultColor1 );
		ChangeColor( ePetParts::PET_PARTS_NOSE, dwPetDefaultColor2 );
	}

	SetDefaultMesh();
}

void CDnPetActor::SetDefaultMesh()
{
	if( !GetObjectHandle() )
		return;

	for( int i=0; i<GetObjectHandle()->GetSubMeshCount(); i++ )
	{
		if( strstr( GetObjectHandle()->GetSubMeshName( i ), "Head_0" ) )
		{
			GetObjectHandle()->ShowSubmesh( i, true );
		}
		else if( strstr( GetObjectHandle()->GetSubMeshName( i ), "Head_1" ) )
		{
			GetObjectHandle()->ShowSubmesh( i, false );
		}
	}
}

void CDnPetActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	CDnActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

bool CDnPetActor::LoadSkin( const char *szSkinName, const char *szAniName, bool bTwoPassAlpha )
{
	bool bResult = MASingleBody::LoadSkin( szSkinName, szAniName, bTwoPassAlpha );
	if( !bResult ) return false;

	m_hObject->SetCalcPositionFlag( CALC_POSITION_Y );
	return true;
}

void CDnPetActor::SetPartsColor( EtAniObjectHandle hObject, char* Type, EtColor Color )
{
	if( !hObject )	return;

	int nCustomColor = hObject->AddCustomParam( Type );

	if( nCustomColor != -1 )
		hObject->SetCustomParam( nCustomColor, &Color );
}

void CDnPetActor::CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	MovePos( vPos, true );
	if( SetActionQueue( szActionName, nLoopCount, fBlendFrame, 0.f, false ) == false ) return;
}

void CDnPetActor::CmdStop( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bResetStop, bool bForce )
{
	if( SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame, false ) == false ) 
		return;

	ResetMove();
	ResetLook();
}

void CDnPetActor::CmdLook( EtVector2 &vVec, bool bForce )
{
	Look( vVec, bForce );
}

void CDnPetActor::RenderCustom( float fElapsedTime )
{
	if( m_bShowHeadName && IsShow() && m_pMaster )
	{
		DWORD dwFontColor;
		DWORD dwFontShadowColor;

		GetHeadIconFontColor( m_pMaster, MAHeadNameRender::Normal, dwFontColor, dwFontShadowColor );
			
		CalcCustomRenderDepth();
		AddHeadNameElement( 1, GetName(), s_nFontIndex, 16, dwFontColor, dwFontShadowColor );
		WCHAR wszCharName[64];
		swprintf_s( wszCharName, 64, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9237 ), m_pMaster->GetName() );
		AddHeadNameElement( 0, wszCharName, s_nFontIndex, 16, dwFontColor, dwFontShadowColor );
	}

	CDnActor::RenderCustom( fElapsedTime );
}

void CDnPetActor::ChangeColor( ePetParts parts, DWORD dwColor )
{
	if( !m_hObject ) return;

	if( parts == PET_PARTS_BODY )
	{
		int nCustomColor = m_hObject->AddCustomParam( "g_CustomColor" );
		if( nCustomColor != -1 )  
		{
			float fHairColor[3]={0,};
			CDnParts::ConvertR10G10B10ToFloat( fHairColor, dwColor );
			EtColor Color( fHairColor[0], fHairColor[1], fHairColor[2], 1.0f );

			m_hObject->SetCustomParam( nCustomColor, &Color );
		}
	}
	else if( parts == PET_PARTS_NOSE )
	{
		int nCustomColor = m_hObject->AddCustomParam( "g_CustomColor2" );
		if( nCustomColor != -1 )  
		{
			float fHairColor[3]={0,};
			CDnParts::ConvertR10G10B10ToFloat( fHairColor, dwColor );
			EtColor Color( fHairColor[0], fHairColor[1], fHairColor[2], 1.0f );

			m_hObject->SetCustomParam( nCustomColor, &Color );
		}
	}
}

bool CDnPetActor::HaveAnyDoNotGetItem( const DNVector(DnDropItemHandle)& vecItemList )
{
	if( vecItemList.size() == 0 )
		return false;
	
	int nDoNetGetItemListCount = static_cast<int>( m_DoNotGetItemList.size() );
	int nCount = static_cast<int>( vecItemList.size() );
	DnDropItemHandle hDropItem;
	for( int i=0; i<nCount; i++ )
	{
		hDropItem = vecItemList[i];
		if( !hDropItem ) continue;
		if( hDropItem->GetItemID() == 0 ) continue;
		std::set<INT64>::const_iterator iter = m_DoNotGetItemList.find( hDropItem->GetUniqueID() );
		if( iter == m_DoNotGetItemList.end() )
			m_DoNotGetItemList.insert( hDropItem->GetUniqueID() );
	}

	if( static_cast<int>( m_DoNotGetItemList.size() ) > nDoNetGetItemListCount )
		return true;

	return false;
}

void CDnPetActor::AddNotEatItemList(const DWORD& dropItemUniqueID)
{
	if (dropItemUniqueID > 0)
		m_NotEatItemList.insert(dropItemUniqueID);
}

bool CDnPetActor::IsNotEatItemList(const DWORD& dropItemUniqueID) const
{
	std::set<INT64>::const_iterator iter = m_NotEatItemList.find(dropItemUniqueID);
	return (iter != m_NotEatItemList.end());
}

void CDnPetActor::DoPetGesture( int nGestureID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGESTURE );

	string szPetActionName;
	int nNumItem = pSox->GetItemCount();
	for( int nItem = 0; nItem < nNumItem; ++nItem )
	{
		int nItemID = pSox->GetItemID( nItem );
		if( nItemID == nGestureID )
		{
			szPetActionName = pSox->GetFieldFromLablePtr( nItemID, "_PetActionID" )->GetString();
			break;
		}
	}
	
	if( szPetActionName.length() > 0 )
	{
		CmdAction( szPetActionName.c_str() );
	}
}