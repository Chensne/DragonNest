#include "StdAfx.h"
#include "MAPartsBody.h"
#include "DnTableDB.h"
#include "DnActor.h"
#include "DnParts.h"
#include "DnWeapon.h"
#include "DnPartsHair.h"
#include "DnPartsHelmet.h"
#include "DnPartsFairy.h"
#include "DnPartsFace.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "EtMergedSkin.h"
#include "EtOptionController.h"
#include "DnItemTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CSyncLock MAPartsBody::s_PartsLock;

MAPartsBody::MAPartsBody()
{
	memset( m_bSelfDelete, 0, sizeof(m_bSelfDelete) );
	memset( m_bCashSelfDelete, 0, sizeof(m_bSelfDelete) );
	memset( m_bCombineParts, 0, sizeof(m_bCombineParts) );
	memset( m_bCombineCashParts, 0, sizeof(m_bCombineCashParts) );
	for( int i=0; i<PartsColorAmount; i++ )
		m_dwPartsColor[i] = 0xFFFFFFFF;
	m_bIgnoreDeleteWaitOnepiece = false;
	m_nActorClassID = 0;
	m_bIsShowPVPHelmet = false; // 기본으로는 안보여 줍니다.
	m_nCurrentHairLevel = 0;
	m_nCombinePartsState = 0;

	memset( m_bPartsViewOrder, 1, sizeof(m_bPartsViewOrder) );
	m_bHideHelmet = false;
	m_bIgnoreCombine = false;
}

MAPartsBody::~MAPartsBody()
{
	FreeSkin();
}

void MAPartsBody::LoadSkin( const char *szDummySkinName, const char *szAniName )
{
	if( m_hObject ) return;

	m_szAniFileName = szAniName;

	m_hObject = EternityEngine::CreateAniObject( szDummySkinName, szAniName );
	if( !m_hObject ) return;
	m_hObject->SetCalcPositionFlag( CALC_POSITION_Y );

	if( m_MergedObject ) m_MergedObject->SetParent( m_hObject, -1 );
	for( int i=0; i < CDnParts::PartsTypeEnum_Amount; i++) {
		if( m_hPartsObject[i] && m_hPartsObject[i]->GetObjectHandle() ) 
			m_hPartsObject[i]->GetObjectHandle()->SetParent( m_hObject, -1 );
	}
	CDnActor *pActor = dynamic_cast<CDnActor *>(this);
	if( pActor ) m_nActorClassID = pActor->GetClassID();
}

void MAPartsBody::FreeSkin()
{
	for( DWORD i=0; i<CDnParts::PartsTypeEnum_Amount; i++ ) {
		DetachParts( (CDnParts::PartsTypeEnum)i );
	}
	for( DWORD i=0; i<CDnParts::CashPartsTypeEnum_Amount; i++ ) {
		DetachCashParts( (CDnParts::PartsTypeEnum)i );
	}
	SAFE_RELEASE_SPTR( m_MergedObject );
	SAFE_RELEASE_SPTR( m_hObject );
	SAFE_RELEASE_SPTRVEC( m_NeedReleasePartsList );
	SAFE_RELEASE_SPTR( m_hFaceDecoTexture );
}

bool MAPartsBody::AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete , bool bUseCombine )
{
	if( !hParts ) return false;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTS );
	if( !pSox ) return false;
	CDnActor *pActor = dynamic_cast<CDnActor *>(this);
	if( pActor == NULL ) return false;

	CDnParts::PartsTypeEnum SlotIndex = ( Index == -1 ) ? hParts->GetPartsType() : Index;
	if( SlotIndex < 0 )
		return false;

	if( m_hPartsObject[SlotIndex] && m_hPartsObject[SlotIndex] != hParts ) {
		MAPartsBody::DetachParts( SlotIndex );
	}
	m_hPartsObject[SlotIndex] = hParts;
	m_bSelfDelete[SlotIndex] = bDelete;

	hParts->SetActor( ((CDnActor*)dynamic_cast<CDnActor *>(this))->GetMySmartPtr() );
	const char *szSkinName = hParts->GetSkinName( pActor->GetClassID() );
	if( szSkinName ) m_szSkinName[SlotIndex] = szSkinName;
	switch( SlotIndex ){
		case CDnParts::Face:
		case CDnParts::Hair:
			bUseCombine = false;
			break;
		default:
			if( hParts->IsIgnoreCombine() )
				bUseCombine = false;
			break;
	}
	if( SlotIndex >= CDnParts::Helmet && SlotIndex <= CDnParts::Foot ) 
		m_bCombineParts[SlotIndex - CDnParts::Helmet] = bUseCombine;

	switch( SlotIndex ){
		case CDnParts::Helmet:
			{
				CDnPartsHelmet *pNormalHelmet = dynamic_cast<CDnPartsHelmet *>(m_hPartsObject[CDnParts::Helmet].GetPointer());
				if( pNormalHelmet && pNormalHelmet->IsIgnoreCombine() ) 
					bUseCombine = false;

				CDnPartsHelmet *pHelmet = dynamic_cast<CDnPartsHelmet *>(m_hCashPartsObject[CDnParts::CashHelmet].GetPointer());
				if( pHelmet && pHelmet->IsIgnoreCombine() ) 
					bUseCombine = false;
			}
			break;
	}

	if( !bUseCombine ) {
		hParts->CreateObject( pActor->GetMySmartPtr(), m_szAniFileName.c_str() );
		if( SlotIndex >= CDnParts::Helmet && SlotIndex <= CDnParts::Foot ) {
			if( m_hCashPartsObject[SlotIndex-2]) m_hPartsObject[SlotIndex]->FreeObject();

			if(SlotIndex == CDnParts::Helmet)
			{
				CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
				if( ( pGameTask ) && ( pGameTask->GetGameTaskType() == GameTaskType::PvP ) )
				{
					if(!m_bIsShowPVPHelmet && m_hPartsObject[SlotIndex])
						m_hPartsObject[SlotIndex]->FreeObject();
				}
			}
		}
	}

	switch( SlotIndex ) {
		case CDnParts::Helmet:
		case CDnParts::Hair:
			{
				if( !m_hPartsObject[CDnParts::Hair] ) break;
				int nLevel = 0;

#ifdef PRE_ADD_TRANSPARENCY_COSTUME
				if( m_hCashPartsObject[CDnParts::CashHelmet] && !CDnParts::IsTransparentSkin( m_hCashPartsObject[CDnParts::CashHelmet]->GetClassID(), m_nActorClassID ) ) {
#else
				if( m_hCashPartsObject[CDnParts::CashHelmet] ) {
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
					nLevel = GetHairLevel( m_hCashPartsObject[CDnParts::CashHelmet] );
				}
				else if( m_hPartsObject[CDnParts::Helmet] ) {
					nLevel = GetHairLevel( m_hPartsObject[CDnParts::Helmet] );
				}

				RefreshHairLevel( nLevel );
			}
			break;
		case CDnParts::Face:
			UpdateColorParam( EyeColor );
			SetFaceAction( "Flicker" );
			break;
		default:
			break;
	}
	if( SlotIndex >= CDnParts::Face && SlotIndex <= CDnParts::Foot )
		UpdateColorParam( SkinColor );

	// 예외 상황이 많아서 위에다 일일이 하는것보단 RefreshPartsViewOrder 한번 호출해주는게 제일 깔끔하다.
	int nPartsViewOrderIndex = SlotIndex-2;
	if( 0 <= nPartsViewOrderIndex && nPartsViewOrderIndex < CDnParts::SwapParts_Amount && m_bPartsViewOrder[nPartsViewOrderIndex] == false )
		RefreshPartsViewOrder( nPartsViewOrderIndex, true );
	if( nPartsViewOrderIndex == -1 && m_bPartsViewOrder[CDnParts::CashHelmet] == false )	// 헤어스타일 변경이면 머리 리프레쉬를 다시 한다. 머리스타일이니 컴바인은 무시.
		RefreshPartsViewOrder( CDnParts::CashHelmet, true );

	return true;
}

bool MAPartsBody::DetachParts( CDnParts::PartsTypeEnum Index )
{
	if( !m_hPartsObject[Index] ) return true;

	m_szSkinName[Index].clear();

	m_hPartsObject[Index]->FreeObject();
	if( m_bSelfDelete[Index] ) {
		SAFE_RELEASE_SPTR( m_hPartsObject[Index] );
		m_bSelfDelete[Index] = false;
	}
	m_hPartsObject[Index].Identity();
	if( Index >= CDnParts::Helmet && Index <= CDnParts::Foot ) 
		m_bCombineParts[Index - CDnParts::Helmet] = true;

	switch( Index ) {
		case CDnParts::Face:
			SetFaceAction( "Flicker" );
			break;
		case CDnParts::Helmet:
			{
				int nLevel = 0;
				if( m_bPartsViewOrder[CDnParts::CashHelmet] && m_hCashPartsObject[CDnParts::CashHelmet] )
					nLevel = GetHairLevel( m_hCashPartsObject[CDnParts::CashHelmet] );

				RefreshHairLevel( nLevel );
			}
			break;
		default:
			break;
	}

	return true;
}

bool MAPartsBody::DetachCashParts( CDnParts::PartsTypeEnum Index )
{
	if( !m_hCashPartsObject[Index] ) return true;

	// 한벌옷 체크
	bool bOnepiece = false;
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
	if( m_hCashPartsObject[Index]->IsExistSubParts() && !CDnParts::IsTransparentSkin( m_hCashPartsObject[Index]->GetClassID(), m_nActorClassID ) ) {
#else
	if( m_hCashPartsObject[Index]->IsExistSubParts() ) {
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
		for( int j=0; j<m_hCashPartsObject[Index]->GetSubPartsCount(); j++ ) {
			CDnParts::PartsTypeEnum SubParts = m_hCashPartsObject[Index]->GetSubPartsIndex(j);
			if( SubParts >= CDnParts::CashHelmet && SubParts <= CDnParts::CashFoot ) {
				if( m_hPartsObject[SubParts+2] ) {
					AttachParts( m_hPartsObject[SubParts+2], (CDnParts::PartsTypeEnum)(SubParts+2), m_bSelfDelete[SubParts+2] );
					bOnepiece = true;
				}
			}
		}
	}

	m_szCashSkinName[Index].clear();

	if( bOnepiece && m_bIgnoreDeleteWaitOnepiece == false )
	{
		// CombineParts와 겹치면 안된다.
		ScopeLock< CSyncLock > Lock( m_PartsLock );

		// 원피스에서 원피스 갈아입을때 m_hDeleteWaitOnepieceCashPartsObject 들어있는 상태로 또 들어오게되니 체크.
		SAFE_RELEASE_SPTR( m_hDeleteWaitOnepieceCashPartsObject );

		// 여기서는 핸들복사가 아니라 아예 전부를 복사해야한다.
		// 기존의 코드에서 캐시오브젝트 아이템객체를 삭제하면서 파츠오브젝트도 전부 삭제하기때문에 별도로 로딩해서 들고있는 것이다.
		if( m_hCashPartsObject[Index]->GetObjectHandle() )
		{
			m_hDeleteWaitOnepieceCashPartsObject = EternityEngine::CreateAniObject( m_hCashPartsObject[Index]->GetObjectHandle()->GetSkinFileName(), m_hCashPartsObject[Index]->GetObjectHandle()->GetAniHandle()->GetFileName(), true );
			m_hDeleteWaitOnepieceCashPartsObject->SetParent( m_hObject, -1 );
			m_hDeleteWaitOnepieceCashPartsObject->Update( *m_hCashPartsObject[Index]->GetObjectCross() );
			m_hDeleteWaitOnepieceCashPartsObject->EnableShadowCast( true );
			m_hDeleteWaitOnepieceCashPartsObject->EnableShadowReceive( false );
			m_hDeleteWaitOnepieceCashPartsObject->GetSkinInstance()->SetRenderAlphaTwoPass( true );

			DWORD dwColor = m_dwPartsColor[SkinColor];
			int nCustomColor = m_hDeleteWaitOnepieceCashPartsObject->AddCustomParam( "g_SkinColor" );
			if( nCustomColor != -1 )  {
				EtColor Color = dwColor;
				m_hDeleteWaitOnepieceCashPartsObject->SetCustomParam( nCustomColor, &Color );
			}
		}
	}
	m_hCashPartsObject[Index]->FreeObject();
	if( m_bCashSelfDelete[Index] ) {
		SAFE_RELEASE_SPTR( m_hCashPartsObject[Index] );
		m_bCashSelfDelete[Index] = false;
	}
	m_hCashPartsObject[Index].Identity();	
	if( Index >= CDnParts::CashHelmet && Index <= CDnParts::CashFoot ) 
		m_bCombineCashParts[Index] = true;

	switch( Index ) {
		case CDnParts::CashHelmet:
			{
				int nLevel = 0;
				if( m_hPartsObject[CDnParts::Helmet] )
				{
					CDnPartsHelmet *pNormalHelmet = dynamic_cast<CDnPartsHelmet *>(m_hPartsObject[CDnParts::Helmet].GetPointer());
					if( pNormalHelmet && ( pNormalHelmet->IsAttachHeadBone() || pNormalHelmet->IsIgnoreCombine() ) ) // 컴바인하지않는 일반 Show타입의 투구가 존재한다
					{
						if(!m_hPartsObject[CDnParts::Helmet]->GetObjectHandle()) // 캐쉬장착하면서 지운경우
						{
							AttachParts(m_hPartsObject[CDnParts::Helmet],CDnParts::Helmet);
						}
					}

					nLevel = GetHairLevel( m_hPartsObject[CDnParts::Helmet] );
				}

				RefreshHairLevel( nLevel );
			}
			break;
		case CDnParts::CashFaceDeco:
			SAFE_RELEASE_SPTR( m_hFaceDecoTexture );
			UpdateFaceDeco();
		default:
			break;
	}

	return true;
}

bool MAPartsBody::AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete, bool bUseCombine )
{
	if( !hParts ) return false;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTS );
	if( !pSox ) return false;
	CDnActor *pActor = dynamic_cast<CDnActor *>(this);
	if( pActor == NULL ) return false;

	CDnParts::PartsTypeEnum SlotIndex = ( Index == -1 ) ? hParts->GetPartsType() : Index;

	if( m_hCashPartsObject[SlotIndex] ) {
		MAPartsBody::DetachCashParts( SlotIndex );
	}
	m_hCashPartsObject[SlotIndex] = hParts;
	m_bCashSelfDelete[SlotIndex] = bDelete;

	// 한벌옷 체크
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
	bool bTransparentSkin = CDnParts::IsTransparentSkin( hParts->GetClassID(), m_nActorClassID );
	if( hParts->IsExistSubParts() && !bTransparentSkin ) {
#else
	if( hParts->IsExistSubParts() ) {
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
		for( int j=0; j<hParts->GetSubPartsCount(); j++ ) {
			CDnParts::PartsTypeEnum SubParts = hParts->GetSubPartsIndex(j);
			if( m_hPartsObject[SubParts+2] ) 
				m_hPartsObject[SubParts+2]->FreeObject();
		}

		if( hParts->IsIgnoreCombine() ) {
			// CombineParts와 겹치면 안된다.
			ScopeLock< CSyncLock > Lock( m_PartsLock );
			// 원피스에서 원피스 갈아입을때 m_hDeleteWaitOnepieceCashPartsObject 는 보일필요 없으니 하이드.
			if( m_hDeleteWaitOnepieceCashPartsObject )
				m_hDeleteWaitOnepieceCashPartsObject->ShowObject( false );
		}
	}

	hParts->SetActor( ((CDnActor*)dynamic_cast<CDnActor *>(this))->GetMySmartPtr() );
	const char *szSkinName = hParts->GetSkinName( pActor->GetClassID() );
	if( szSkinName ) m_szCashSkinName[SlotIndex] = szSkinName;
	switch( SlotIndex ) {
		case CDnParts::CashHelmet:
			{
				CDnPartsHelmet *pHelmet = dynamic_cast<CDnPartsHelmet *>(m_hCashPartsObject[CDnParts::CashHelmet].GetPointer());
				if( pHelmet && ( pHelmet->IsAttachHeadBone() || pHelmet->IsIgnoreCombine() ) )
					bUseCombine = false;

				CDnPartsHelmet *pNormalHelmet = dynamic_cast<CDnPartsHelmet *>(m_hPartsObject[CDnParts::Helmet].GetPointer());
				if( pHelmet && pNormalHelmet && ( pNormalHelmet->IsAttachHeadBone() || pNormalHelmet->IsIgnoreCombine() ) )
					if( m_hPartsObject[CDnParts::Helmet] ) m_hPartsObject[CDnParts::Helmet]->FreeObject();

			}
			break;
//		case CDnParts::CashNecklace:
		case CDnParts::CashEarring:
		case CDnParts::CashRing:
		case CDnParts::CashRing2:
		case CDnParts::CashWing:
		case CDnParts::CashTail:
		case CDnParts::CashFairy:		
			bUseCombine = false;
			break;
		case CDnParts::CashFaceDeco:
			{
				bUseCombine = false;
				const char *szDecalName = hParts->GetDecalName( pActor->GetClassID() );
				SAFE_RELEASE_SPTR( m_hFaceDecoTexture );
				if( szDecalName && szDecalName[0] != '\0' ) {
					m_hFaceDecoTexture = EternityEngine::LoadTexture( szDecalName );
				}
				UpdateFaceDeco();
			}
			break;
		default:
			if( hParts->IsIgnoreCombine() )
				bUseCombine = false;
			break;
	}
	if( SlotIndex >= CDnParts::CashHelmet && SlotIndex <= CDnParts::CashFoot ) 
		m_bCombineCashParts[SlotIndex] = bUseCombine;

	if( !bUseCombine ) {
		// 잠깐 보였다 다시 사라지는거 방지하기 위해선 이렇게 처리해야한다.(컴바인 관련 파츠들만 해당사항)
		bool bCreateObject = true;
		if( SlotIndex >= CDnParts::CashHelmet && SlotIndex <= CDnParts::CashFoot && m_bPartsViewOrder[SlotIndex] == false ) bCreateObject = false;
		if( SlotIndex == CDnParts::CashHelmet && m_bHideHelmet ) bCreateObject = false;
		if( bCreateObject ) hParts->CreateObject( pActor->GetMySmartPtr(), m_szAniFileName.c_str() );
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
		if( SlotIndex >= CDnParts::CashHelmet && SlotIndex <= CDnParts::CashFoot && !bTransparentSkin ) {
#else
		if( SlotIndex >= CDnParts::CashHelmet && SlotIndex <= CDnParts::CashFoot ) {
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
			if( m_hPartsObject[SlotIndex+2] ) m_hPartsObject[SlotIndex+2]->FreeObject();
		}

		if(SlotIndex == CDnParts::CashHelmet)
		{
			CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( ( pGameTask ) && ( pGameTask->GetGameTaskType() == GameTaskType::PvP ) )
			{
				if(!m_bIsShowPVPHelmet && m_hCashPartsObject[SlotIndex])
					m_hCashPartsObject[SlotIndex]->FreeObject();
			}
		}
	}
	LinkCashParts( SlotIndex );

	if( SlotIndex >= CDnParts::CashBody && SlotIndex <= CDnParts::CashFoot )
		UpdateColorParam( SkinColor );

	// 예외 상황이 많아서 위에다 일일이 하는것보단 RefreshPartsViewOrder 한번 호출해주는게 제일 깔끔하다.
	if( 0 <= SlotIndex && SlotIndex < CDnParts::SwapParts_Amount && m_bPartsViewOrder[SlotIndex] == false )
	{
		// 새로 액터 만들어서 파츠정보 셋팅할때만 이쪽으로 들어온다.(빌리지 입장, 겜서버 입장 등등)
		// 캐시아이템이 보이는 걸로 설정되어있다면 따로 처리할거 없으니 안한다.
		// 보통 새 액터 만들때 컴바인 마지막에 별도로 해주니까, 컴바인은 무시하기로 한다.
		RefreshPartsViewOrder( SlotIndex, true );
	}

	return true;
}

void MAPartsBody::LinkCashParts( CDnParts::PartsTypeEnum Index )
{
	CDnActor *pActor = dynamic_cast<CDnActor *>(this);
	if( pActor == NULL ) return;
	if( m_hCashPartsObject[Index] ) {
		switch( Index ) {
			case CDnParts::CashBody:
			case CDnParts::CashLeg:
			case CDnParts::CashHand:
			case CDnParts::CashFoot:
				if( !m_hCashPartsObject[Index]->IsLoadPartsAni() ) {
					m_hCashPartsObject[Index]->LinkParts( pActor->GetMySmartPtr(), NULL );
				}
				break;
			case CDnParts::CashHelmet:
				{
					int nLevel = 0;
					if( m_hCashPartsObject[CDnParts::CashHelmet] ) {
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
						if( !CDnParts::IsTransparentSkin( m_hCashPartsObject[CDnParts::CashHelmet]->GetClassID(), m_nActorClassID ) )
							nLevel = GetHairLevel( m_hCashPartsObject[CDnParts::CashHelmet] );
						else if( m_hPartsObject[CDnParts::Helmet] )
							nLevel = GetHairLevel( m_hPartsObject[CDnParts::Helmet] );
#else
						nLevel = GetHairLevel( m_hCashPartsObject[CDnParts::CashHelmet] );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME

						CDnPartsHelmet *pHelmet = dynamic_cast<CDnPartsHelmet *>(m_hCashPartsObject[CDnParts::CashHelmet].GetPointer());
						if( pHelmet ) {
							if( pHelmet->IsAttachHeadBone() ) 
								m_hCashPartsObject[Index]->LinkParts( pActor->GetMySmartPtr(), "Bip01 Head" );
							else m_hCashPartsObject[Index]->LinkParts( pActor->GetMySmartPtr(), NULL );
						}
					}

					RefreshHairLevel( nLevel );
				}
				break;
			case CDnParts::CashEarring: m_hCashPartsObject[Index]->LinkParts( pActor->GetMySmartPtr(), "#Cash_EarRing" ); break;
			case CDnParts::CashRing: m_hCashPartsObject[Index]->LinkParts( pActor->GetMySmartPtr(), "#Cash_Ring_L" ); break;
			case CDnParts::CashRing2: m_hCashPartsObject[Index]->LinkParts( pActor->GetMySmartPtr(), "#Cash_Ring_R" ); break;
			case CDnParts::CashWing: m_hCashPartsObject[Index]->LinkParts( pActor->GetMySmartPtr(), "#Cash_Wing" ); break;
			case CDnParts::CashTail: m_hCashPartsObject[Index]->LinkParts( pActor->GetMySmartPtr(), "#Cash_Tail" ); break;
#ifdef PRE_MOD_CASHFAIRY
			case CDnParts::CashFairy:
				{
					CDnPartsFairy *pFairy = dynamic_cast<CDnPartsFairy *>(m_hCashPartsObject[CDnParts::CashFairy].GetPointer());
					if( pFairy ) pFairy->LinkFairy( pActor->GetMySmartPtr() );
				}
				break;
#else
			case CDnParts::CashFairy:
				{
					CDnPlayerActor *pPlayerActor = dynamic_cast<CDnPlayerActor *>(this);
					if( pPlayerActor && pPlayerActor->IsVehicleMode() && pPlayerActor->GetMyVehicleActor() )
						m_hCashPartsObject[Index]->LinkParts( pPlayerActor->GetMyVehicleActor()->GetMySmartPtr(), "#Cash_Spirit" );
					else
						m_hCashPartsObject[Index]->LinkParts( pActor->GetMySmartPtr(), "#Cash_Spirit" );
				}
				break;
#endif
			default:
				break;
		}

		if (Index == CDnParts::CashWing)
		{
			if(m_hCashPartsObject[Index]->IsExistAction("CostumeAction_On"))
			{
					m_hCashPartsObject[Index]->SetAction("CostumeAction_On", 0.0f, 0.0f, true);
					OutputDebug("SetCostumACtionIdle CostumeAction_On");
			}
		}
	}
}

void MAPartsBody::RefreshHairLevel( int nLevel )
{
	if( !m_hPartsObject[CDnParts::Hair] ) return;
	EtAniObjectHandle hHandle = m_hPartsObject[CDnParts::Hair]->GetObjectHandle();
	if( !hHandle || !hHandle->GetMesh() ) return;
	char szSubMeshTag[4];
	sprintf_s( szSubMeshTag, "_%d", nLevel );
	bool bExistSubMesh = false;
	for( int i=0; i<hHandle->GetSubMeshCount(); i++ ) {
		CEtSubMesh *pSubMesh = hHandle->GetSubMesh(i);
		if( !pSubMesh ) continue;
		if( strstr( pSubMesh->GetSubMeshName(), szSubMeshTag ) ) {
			hHandle->ShowSubmesh( i, true );
			bExistSubMesh = true;
		}
		else hHandle->ShowSubmesh( i, false );
	}
	if( !bExistSubMesh ) {
		hHandle->ShowSubmesh( 0, true );
	}
	CDnPartsHair *pHairParts = static_cast<CDnPartsHair *>(m_hPartsObject[CDnParts::Hair].GetPointer());
	pHairParts->UpdateColorParam( GetPartsColor( HairColor ) );

	m_nCurrentHairLevel = nLevel; // Refresh 해 줄때 정보를 담아둡니다.
}

void MAPartsBody::CombineParts()
{
	if( m_bIgnoreCombine ) return;
	InterlockedIncrement( &m_nCombinePartsState );

	// 두군데에서 동시에 불리는 경우 있다..
	ScopeLock< CSyncLock > Lock( s_PartsLock );

	// 멤버는 멤버락대로 따로 건다.
	ScopeLock< CSyncLock > MemberLock( m_PartsLock );

	bool bShow = true;
	EtAniObjectHandle hTempObject;
	if( m_MergedObject ) {
		hTempObject = m_MergedObject;
		bShow = m_MergedObject->IsShow();
	}

	m_MergedObject.Identity();
//	SAFE_RELEASE_SPTR( m_MergedObject );

	// 헬멧이 에니가 있으면 컴바인을 하면 안되기땜에 미리 계산합니다.
	/*
	bool bCombineFlag[7] = { true, };
	memset( bCombineFlag, 1, sizeof(bCombineFlag) );
	for( int i = CDnParts::Helmet; i <= CDnParts::Foot; i++) {
		if( !m_hPartsObject[i] ) continue;
	}
	if( !m_szSkinName[CDnParts::Helmet].empty() || !m_szCashSkinName[CDnParts::CashHelmet].empty() ) {
		DnPartsHandle hHelmet = m_hPartsObject[CDnParts::Helmet];
		if( !m_szCashSkinName[CDnParts::CashHelmet].empty() )
			hHelmet = m_hCashPartsObject[CDnParts::CashHelmet];

		if( hHelmet ) {
			CDnPartsHelmet *pHelmet = dynamic_cast<CDnPartsHelmet *>(hHelmet.GetPointer());
			if( pHelmet && ( pHelmet->IsAttachHeadBone() || pHelmet->IsIgnoreCombine() ) ) {
				bCombineFlag[CDnParts::Helmet] = false;
			}
		}
	}
	*/

	bool bCombineFlag[5];
	for( int i=0; i<5; i++ ) {
		if( m_hPartsObject[i+2] ) bCombineFlag[i] = m_bCombineParts[i];
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
		if( m_hCashPartsObject[i] && !CDnParts::IsTransparentSkin( m_hCashPartsObject[i]->GetClassID(), m_nActorClassID ) && m_bPartsViewOrder[i] ) bCombineFlag[i] = m_bCombineCashParts[i];
#else
		if( m_hCashPartsObject[i] && m_bPartsViewOrder[i] ) bCombineFlag[i] = m_bCombineCashParts[i];
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
	}
//	memcpy( bCombineFlag, m_bCombineParts, sizeof(bCombineFlag) );

	if( m_bHideHelmet )
		bCombineFlag[CDnParts::Helmet-2] = false;

	// PvP에서 헬멧 제외
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( ( pGameTask ) && ( pGameTask->GetGameTaskType() == GameTaskType::PvP ) )
	{
		if( !m_bIsShowPVPHelmet ) {
			bCombineFlag[CDnParts::Helmet-2] = false;
		}
	}
	
	// 한벌옷 체크
	bool bOnepiece = false;
	for( int i = CDnParts::CashHelmet; i <= CDnParts::CashFoot; i++) {
//		if( !bCombineFlag[i] ) continue;
		if( !m_hCashPartsObject[i] ) continue;
		if( !m_hCashPartsObject[i]->IsExistSubParts() ) continue;
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
		if( CDnParts::IsTransparentSkin( m_hCashPartsObject[i]->GetClassID(), m_nActorClassID ) ) continue;
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
		if( m_bPartsViewOrder[i] == false ) continue;
		for( int j=0; j<m_hCashPartsObject[i]->GetSubPartsCount(); j++ ) {
			CDnParts::PartsTypeEnum SubParts = m_hCashPartsObject[i]->GetSubPartsIndex(j);
			if( SubParts >= 0 && SubParts < 5 )
			{
				bCombineFlag[SubParts] = false;
				bOnepiece = true;
			}
		}
	}

	std::vector< std::string > vecAvatarSkin;
	for( int i = CDnParts::Helmet; i <= CDnParts::Foot; i++) {
		if( !bCombineFlag[i-2] ) continue;
		std::string szSkinName = m_szSkinName[i];
		if( m_hCashPartsObject[i-2] && !m_szCashSkinName[i-2].empty() && m_bPartsViewOrder[i-2] ) {
			szSkinName = m_szCashSkinName[i-2];
		}
		if( szSkinName.empty() ) continue;
		vecAvatarSkin.push_back( szSkinName );
	}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
	// 워리어가 건틀렛 착용시 컴바인 안되는 장갑이라면 서브메시를 Hide시켜준다.
	OnCombineParts();
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

	int nMergeWidth, nMergeHeight, nHalfWidth, nHalfHeight;
	std::vector< RECT > vecAvatarTextureRect;
	vecAvatarTextureRect.resize( vecAvatarSkin.size() );

	nMergeWidth = nMergeHeight = 512;
	// 저사양 버전에서는 반사이즈..
	if( GetEtOptionController()->GetGraphicQuality() >= 2 )
	{
		nMergeWidth = nMergeHeight = 256;
	}
	// 머지텍스쳐 기본크기는 512 쓰는데 아바타 6개 넘어가면 가로사이즈 1024로 확장
	if( vecAvatarSkin.size() > 6 )
	{
		nMergeWidth *= 2;
	}
	nHalfWidth = nMergeWidth / 2;
	nHalfHeight = nMergeHeight / 2;
	for( int i = 0; i < 2; i++ )
	{
		int j;
		for( j = 0; j < 2; j++ )
		{
			int nIndex = i * 2 + j;
			if( nIndex >= ( int )vecAvatarSkin.size() )
			{
				break;
			}
			vecAvatarTextureRect[ nIndex ].left = j * nHalfWidth;
			vecAvatarTextureRect[ nIndex ].right = vecAvatarTextureRect[ nIndex ].left + nHalfWidth;
			vecAvatarTextureRect[ nIndex ].top = i * nHalfHeight;
			vecAvatarTextureRect[ nIndex ].bottom = vecAvatarTextureRect[ nIndex ].top + nHalfHeight;
		}
	}
	int nReduceBegin = 3;
	for( int i = 4; i < ( int )vecAvatarSkin.size(); i++ )
	{
		vecAvatarTextureRect[ nReduceBegin ].right = vecAvatarTextureRect[ nReduceBegin ].left + ( nHalfWidth / 2 );

		vecAvatarTextureRect[ i ] = vecAvatarTextureRect[ nReduceBegin ];
		vecAvatarTextureRect[ i ].left = vecAvatarTextureRect[ nReduceBegin ].right;
		vecAvatarTextureRect[ i ].right = vecAvatarTextureRect[ i ].left + ( nHalfWidth / 2 );
		nReduceBegin--;
		if( nReduceBegin < 0 )
		{
			nReduceBegin = 0;
		}
	}

	EtMergedSkinHandle hMergedSkin;
	for( int i = 0; i < ( int )vecAvatarSkin.size(); i++)
	{
		EtSkinHandle hSkin = EternityEngine::LoadSkin( vecAvatarSkin[ i ].c_str() );
		if( !hSkin ) continue;
		EtMaterialHandle hMaterial = hSkin->GetMaterialHandle( 0 );
		if( ( hMaterial ) && ( stricmp( hMaterial->GetFileName(), "DiffuseAvatar.fx" ) ) )
		{
			continue;
		}
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( hSkin->GetMeshHandle() ) hSkin->GetMeshHandle()->SetDeleteImmediate( true );
#endif

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		CEtSkinInstance SkinInstance;
		SkinInstance.CopySkinInfo( hSkin );
		CEtSkinInstance *pSkinInstance = &SkinInstance;
		OnCombineParts( hSkin, &pSkinInstance );
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

		if( hMergedSkin ) {
#ifdef PRE_MOD_WARRIOR_RIGHTHAND
			hMergedSkin->Merge( hSkin, vecAvatarTextureRect[ i ], pSkinInstance );
#else
			hMergedSkin->Merge( hSkin, vecAvatarTextureRect[ i ] );
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
		}
		else {
			hMergedSkin = (new CEtMergedSkin)->GetMySmartPtr();
			hMergedSkin->SetTextureSize( nMergeWidth, nMergeHeight );
			if( !hMergedSkin->Assign( hSkin, vecAvatarTextureRect[ i ] ) ) {
				SAFE_RELEASE_SPTR( hSkin );
				SAFE_RELEASE_SPTR( hMergedSkin );
				SAFE_RELEASE_SPTR( m_hDeleteWaitOnepieceCashPartsObject );
				SAFE_RELEASE_SPTR( hTempObject );
				InterlockedDecrement( &m_nCombinePartsState );
				return;
			}
		}
		m_NeedReleasePartsList.push_back( hSkin );
		ThreadDelay();
	}
	if( !hMergedSkin ) {
		SAFE_RELEASE_SPTR( hTempObject );
		// 원피스에서 다른 옷을 갈아입을때 패킷 두번처리되는것 때문에 이쪽으로 올 가능성 있다.
		// 이때는 지우지 말고 다음번 Insert된 CombineParts에서 지우도록 한다.(이렇게 해야 간헐적으로 몸메시 없는것처럼 보이는걸 피할 수 있다.)
		if( bOnepiece == false ) SAFE_RELEASE_SPTR( m_hDeleteWaitOnepieceCashPartsObject );
		InterlockedDecrement( &m_nCombinePartsState );
		return;
	}
	hMergedSkin->BuildMipmap();
	m_MergedObject = EternityEngine::CreateAniObject( hMergedSkin, m_szAniFileName.c_str() );
	if( !m_MergedObject ) {
		SAFE_RELEASE_SPTR( hTempObject );
		SAFE_RELEASE_SPTR( m_hDeleteWaitOnepieceCashPartsObject );
		InterlockedDecrement( &m_nCombinePartsState );
		return;
	}
	m_MergedObject->GetSkinInstance()->SetRenderAlphaTwoPass( true );
	m_MergedObject->SetParent( m_hObject, -1);
	m_MergedObject->ShowObject( bShow );
	SAFE_RELEASE_SPTR( hTempObject );
	SAFE_RELEASE_SPTR( m_hDeleteWaitOnepieceCashPartsObject );

	InterlockedDecrement( &m_nCombinePartsState );

	UpdateColorParam( SkinColor );
}

bool MAPartsBody::ChangeSocialTexture( int nSocialIndex, int nFrameIndex )
{
	DnPartsHandle hParts = GetParts( CDnParts::Face );
	if( !hParts ) return false;

	EtAniObjectHandle hObject = hParts->GetObjectHandle();
	if( !hObject ) return false;

	bool bResult[2] = { false, };
	EtTextureHandle hTexture = m_pVecSocialList[nSocialIndex]->pVecList[nFrameIndex]->hTexture;
	int nTextureIndex = -1;
	if( hTexture ) {
		nTextureIndex = hTexture.GetIndex();
		int nCustomParam = hObject->AddCustomParam( "g_DiffuseTex" );
		bResult[0] = hObject->SetCustomParam( nCustomParam, &nTextureIndex );
	}

	hTexture = m_pVecSocialList[nSocialIndex]->pVecList[nFrameIndex]->hMaskTexture;
	nTextureIndex = -1;
	if( hTexture ) {
		nTextureIndex = hTexture.GetIndex();
		int nCustomParam = hObject->AddCustomParam( "g_MaskTex" );
		bResult[1] = hObject->SetCustomParam( nCustomParam, &nTextureIndex );
	}

	UpdateFaceDeco();

	return ( bResult[0] && bResult[1] ) ? true : false;
}

void MAPartsBody::ShowParts( bool bShow )
{
	for( int i=0; i<CDnParts::PartsTypeEnum_Amount; i++ ) {
		if( m_hPartsObject[i] ) {
			m_hPartsObject[i]->ShowRenderBase( bShow );
		}
	}
	for( int i=0; i<CDnParts::CashPartsTypeEnum_Amount; i++ ) {
		if( m_hCashPartsObject[i] ) {
			if( i < CDnParts::SwapParts_Amount && IsViewPartsOrder(i) == false )
				continue;
			m_hCashPartsObject[i]->ShowRenderBase( bShow );
			LinkCashParts( (CDnParts::PartsTypeEnum)i );
		}
	}

	if( m_MergedObject )
	{
		m_MergedObject->ShowObject( bShow );
	}
}

void MAPartsBody::SetPartsColor( PartsColorEnum ColorType, DWORD dwColor )
{
	m_dwPartsColor[ColorType] = dwColor;

	UpdateColorParam( ColorType );
}

void MAPartsBody::UpdateColorParam( PartsColorEnum ColorType )
{
	ScopeLock< CSyncLock > Lock( m_PartsLock );

	DWORD dwColor = m_dwPartsColor[ColorType];
	switch( ColorType )
	{
	case HairColor:
		{
			DnPartsHandle hParts = GetParts( CDnParts::PartsTypeEnum::Hair );
			if( hParts ) {
				CDnPartsHair *pPartsHair = dynamic_cast<CDnPartsHair*>(hParts.GetPointer());
				if( pPartsHair ) {
					pPartsHair->UpdateColorParam( dwColor );
				}
			}
		}
		break;
	case EyeColor:
		{
			DnPartsHandle hParts = GetParts( CDnParts::PartsTypeEnum::Face );
			if( hParts ) {
				hParts->UpdateEyeColorParam( dwColor );
			}
		}
		break;
	case SkinColor:
		{
			for( int nParts = CDnParts::Face; nParts <= CDnParts::Foot; nParts++ ) {
				CDnParts *pParts = dynamic_cast<CDnParts*>(GetParts( (CDnParts::PartsTypeEnum)nParts ).GetPointer());
				if( pParts ) {
					pParts->UpdateSkinColorParam( dwColor );
				}
			}
			for( int nParts = CDnParts::CashHelmet; nParts <= CDnParts::CashFoot; nParts++ ) {
				CDnParts *pParts = dynamic_cast<CDnParts*>(GetCashParts( (CDnParts::PartsTypeEnum)nParts ).GetPointer());
				if( pParts ) {
					pParts->UpdateSkinColorParam( dwColor );
				}
			}
			if( !IsCombiningParts() )
			{
				if( m_MergedObject ) {
					int nCustomColor = m_MergedObject->AddCustomParam( "g_SkinColor" );
					if( nCustomColor != -1 )  {
						EtColor Color = dwColor;
						m_MergedObject->SetCustomParam( nCustomColor, &Color );
					}
				}
			}
		}
		break;
	}
}

void MAPartsBody::Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta )
//void MAPartsBody::Process( LOCAL_TIME LocalTime, float fDelta )
{
	MAActorRenderBase::Process( pmatWorld, LocalTime, fDelta );

	for( int i=0; i<CDnParts::PartsTypeEnum_Amount; i++ ) {
		if( m_hPartsObject[i] ) m_hPartsObject[i]->Process( LocalTime, fDelta );
	}

	for( int i=0; i<CDnParts::CashPartsTypeEnum_Amount; i++ ) {
		if( m_hCashPartsObject[i] ) m_hCashPartsObject[i]->Process( LocalTime, fDelta );
	}
}

void MAPartsBody::UpdateFaceDeco()
{
	DnPartsHandle hParts = GetParts( CDnParts::Face );
	if( !hParts ) return;

	EtAniObjectHandle hObject = hParts->GetObjectHandle();
	if( !hObject ) return;

	int nCustomParam = hObject->AddCustomParam( "g_TatooTex" );
	if( nCustomParam >= 0 ) {
		int nTextureIndex = m_hFaceDecoTexture ? m_hFaceDecoTexture->GetMyIndex() : CEtTexture::GetBlankTexture()->GetMyIndex();
		hObject->SetCustomParam( nCustomParam, &nTextureIndex );
	}
}

int MAPartsBody::GetHairLevel( DnPartsHandle hParts )
{
	int nLevel = 0;
	CDnPartsHelmet *pHelmet = dynamic_cast<CDnPartsHelmet *>(hParts.GetPointer());
	if( pHelmet ) nLevel = pHelmet->GetHairShowLevel();

	if( m_bHideHelmet )
		nLevel = 0;

	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask ) {
		switch( pGameTask->GetGameTaskType() ) {
			case GameTaskType::PvP:
				if(!m_bIsShowPVPHelmet)
					nLevel = 0;
				break;
		}
	}
	return nLevel;
}

void MAPartsBody::RefreshPartsViewOrder( int nEquipIndex, bool bSkipCombine )
{
	if( nEquipIndex < 0 || nEquipIndex >= CDnParts::SwapParts_Amount ) return;
	CDnActor *pActor = dynamic_cast<CDnActor *>(this);
	if( pActor == NULL ) return;
	if( !m_hCashPartsObject[nEquipIndex] ) return;

	if( nEquipIndex >= CDnParts::CashHelmet && nEquipIndex <= CDnParts::CashFoot )
	{
		// 방어구는 여기
		bool bNormalPartsIgnoreCombine = false;
		bool bCashPartsIgnoreCombine = false;
		if( nEquipIndex == CDnParts::CashHelmet )
		{
			CDnPartsHelmet *pNormalHelmet = dynamic_cast<CDnPartsHelmet *>(m_hPartsObject[nEquipIndex+2].GetPointer());
			if( pNormalHelmet && ( pNormalHelmet->IsAttachHeadBone() || pNormalHelmet->IsIgnoreCombine() ) ) bNormalPartsIgnoreCombine = true;
			CDnPartsHelmet *pCashHelmet = dynamic_cast<CDnPartsHelmet *>(m_hCashPartsObject[nEquipIndex].GetPointer());
			if( pCashHelmet && ( pCashHelmet->IsAttachHeadBone() || pCashHelmet->IsIgnoreCombine() ) ) bCashPartsIgnoreCombine = true;

			CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( ( pGameTask ) && ( pGameTask->GetGameTaskType() == GameTaskType::PvP ) )
			{
				if( !m_bIsShowPVPHelmet )
				{
					bNormalPartsIgnoreCombine = false;
					bCashPartsIgnoreCombine = false;
				}
			}

			if( m_bHideHelmet )
			{
				bNormalPartsIgnoreCombine = false;
				bCashPartsIgnoreCombine = false;
				bSkipCombine = true;
			}
		}
		else
		{
			if( m_hPartsObject[nEquipIndex+2] && m_hPartsObject[nEquipIndex+2]->IsIgnoreCombine() ) bNormalPartsIgnoreCombine = true;
			if( m_hCashPartsObject[nEquipIndex] && m_hCashPartsObject[nEquipIndex]->IsIgnoreCombine() ) bCashPartsIgnoreCombine = true;
		}

		if( m_bPartsViewOrder[nEquipIndex] )
		{
			// IgnoreCombine이 true라면 ObjectHandle이 있을것이다.
			if( bCashPartsIgnoreCombine )
			{
				if( !m_hCashPartsObject[nEquipIndex]->GetObjectHandle() )
				{
					m_hCashPartsObject[nEquipIndex]->CreateObject( pActor->GetMySmartPtr(), m_szAniFileName.c_str() );
					LinkCashParts( (CDnParts::PartsTypeEnum)nEquipIndex );
					UpdateColorParam( SkinColor );
				}
			}

			if( bNormalPartsIgnoreCombine )
			{
				if( m_hPartsObject[nEquipIndex+2]->GetObjectHandle() )
				{
					m_hPartsObject[nEquipIndex+2]->FreeObject();
				}
			}

			if( nEquipIndex == CDnParts::CashHelmet )
			{
				int nLevel = GetHairLevel( m_hCashPartsObject[CDnParts::CashHelmet] );
				RefreshHairLevel( nLevel );
			}
		}
		else
		{
			if( bNormalPartsIgnoreCombine )
			{
				if( !m_hPartsObject[nEquipIndex+2]->GetObjectHandle() )
				{
					m_hPartsObject[nEquipIndex+2]->CreateObject( pActor->GetMySmartPtr(), m_szAniFileName.c_str() );
				}
			}

			// 한벌캐시옷에서 일반옷으로 스위칭 하는지 확인
			bool bOnepiece = false;
			CDnParts::PartsTypeEnum Index = (CDnParts::PartsTypeEnum)nEquipIndex;
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
			if( m_hCashPartsObject[Index]->IsExistSubParts() && !CDnParts::IsTransparentSkin( m_hCashPartsObject[Index]->GetClassID(), m_nActorClassID ) ) {
#else
			if( m_hCashPartsObject[Index]->IsExistSubParts() ) {
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
				for( int j=0; j<m_hCashPartsObject[Index]->GetSubPartsCount(); j++ ) {
					CDnParts::PartsTypeEnum SubParts = m_hCashPartsObject[Index]->GetSubPartsIndex(j);
					if( SubParts >= CDnParts::CashHelmet && SubParts <= CDnParts::CashFoot ) {
						if( m_hPartsObject[SubParts+2] ) bOnepiece = true;
					}
				}
			}

			if( bOnepiece && m_bIgnoreDeleteWaitOnepiece == false )
			{
				// CombineParts와 겹치면 안된다.
				ScopeLock< CSyncLock > Lock( m_PartsLock );

				// 원피스에서 원피스 갈아입을때 m_hDeleteWaitOnepieceCashPartsObject 들어있는 상태로 또 들어오게되니 체크.
				SAFE_RELEASE_SPTR( m_hDeleteWaitOnepieceCashPartsObject );

				// 여기서는 핸들복사가 아니라 아예 전부를 복사해야한다.
				// 기존의 코드에서 캐시오브젝트 아이템객체를 삭제하면서 파츠오브젝트도 전부 삭제하기때문에 별도로 로딩해서 들고있는 것이다.
				if( m_hCashPartsObject[Index]->GetObjectHandle() )
				{
					m_hDeleteWaitOnepieceCashPartsObject = EternityEngine::CreateAniObject( m_hCashPartsObject[Index]->GetObjectHandle()->GetSkinFileName(), m_hCashPartsObject[Index]->GetObjectHandle()->GetAniHandle()->GetFileName(), true );
					m_hDeleteWaitOnepieceCashPartsObject->SetParent( m_hObject, -1 );
					m_hDeleteWaitOnepieceCashPartsObject->Update( *m_hCashPartsObject[Index]->GetObjectCross() );
					m_hDeleteWaitOnepieceCashPartsObject->EnableShadowCast( true );
					m_hDeleteWaitOnepieceCashPartsObject->EnableShadowReceive( false );
					m_hDeleteWaitOnepieceCashPartsObject->GetSkinInstance()->SetRenderAlphaTwoPass( true );

					DWORD dwColor = m_dwPartsColor[SkinColor];
					int nCustomColor = m_hDeleteWaitOnepieceCashPartsObject->AddCustomParam( "g_SkinColor" );
					if( nCustomColor != -1 )  {
						EtColor Color = dwColor;
						m_hDeleteWaitOnepieceCashPartsObject->SetCustomParam( nCustomColor, &Color );
					}
				}
			}

			if( bCashPartsIgnoreCombine )
			{
				if( m_hCashPartsObject[nEquipIndex]->GetObjectHandle() )
				{
					m_hCashPartsObject[nEquipIndex]->FreeObject();
				}
			}

			if( nEquipIndex == CDnParts::CashHelmet )
			{
				int nLevel = GetHairLevel( m_hPartsObject[CDnParts::Helmet] );
				RefreshHairLevel( nLevel );
			}
		}

		if( bSkipCombine == false )
		{
			if( bNormalPartsIgnoreCombine == false || bCashPartsIgnoreCombine == false )
			{
				// 하나라도 컴바인 되는 거라면 다시 컴바인 해야한다.
				// 액터메세지로 오기때문에 OnCheckLoadingPacket 할필요 없이 그냥 Insert만 해도 된다.
				GetItemTask().InsertChangePartsThread( pActor->GetMySmartPtr() );
			}
		}
	}
	else if( ( nEquipIndex >= CDnParts::CashNecklace && nEquipIndex <= CDnParts::CashEarring ) ||
			 ( nEquipIndex >= CDnParts::CashWing && nEquipIndex <= CDnParts::CashTail ) )
	{
		// 일반아이템 목걸이, 귀걸이에는 메시가 없기 때문에 날개나 꼬리처럼 처리해도 무방하다.
		if( m_bPartsViewOrder[nEquipIndex] )
		{
			if( !m_hCashPartsObject[nEquipIndex]->GetObjectHandle() )
			{
				m_hCashPartsObject[nEquipIndex]->CreateObject( pActor->GetMySmartPtr(), m_szAniFileName.c_str() );
				LinkCashParts( (CDnParts::PartsTypeEnum)nEquipIndex );
			}
		}
		else
		{
			if( m_hCashPartsObject[nEquipIndex]->GetObjectHandle() )
			{
				m_hCashPartsObject[nEquipIndex]->FreeObject();
			}
		}
	}
	else if( nEquipIndex >= CDnParts::CashRing && nEquipIndex <= CDnParts::CashRing2 )
	{
		// 무기와 달리 기본 Idle모션에 파티클을 넣고 act로 돌리는거라,
		//m_hCashWeapon[nEquipIndex]->ReleaseSignalImp();
		//m_hCashWeapon[nEquipIndex]->ResetDefaultAction( nEquipIndex );
		// 위 방법으로는 안보이게 할 수 없다.
		// 그래서 그냥 Show-Hide하는걸로 하겠다.
		if( m_bPartsViewOrder[nEquipIndex] )
		{
			m_hCashPartsObject[nEquipIndex]->ShowRenderBase( true );
		}
		else
		{
			m_hCashPartsObject[nEquipIndex]->ShowRenderBase( false );
		}
	}
	else if( nEquipIndex == CDnParts::CashFaceDeco )
	{
		// 데칼은 원래 루틴 타게 하는게 제일 낫다.
		if( m_bPartsViewOrder[nEquipIndex] )
		{
			if( !m_hFaceDecoTexture )
			{
				const char *szDecalName = m_hCashPartsObject[nEquipIndex]->GetDecalName( pActor->GetClassID() );
				if( szDecalName && szDecalName[0] != '\0' ) {
					m_hFaceDecoTexture = EternityEngine::LoadTexture( szDecalName );
				}
				UpdateFaceDeco();
			}
		}
		else
		{
			if( m_hFaceDecoTexture )
			{
				SAFE_RELEASE_SPTR( m_hFaceDecoTexture );
				UpdateFaceDeco();
			}
		}
	}
}

void MAPartsBody::RefreshHideHelmet( bool bChangeParts )
{
	CDnActor *pActor = dynamic_cast<CDnActor *>(this);
	if( pActor == NULL ) return;

	bool bNormalPartsIgnoreCombine = false;
	bool bCashPartsIgnoreCombine = false;

	CDnPartsHelmet *pNormalHelmet = dynamic_cast<CDnPartsHelmet *>(m_hPartsObject[CDnParts::Helmet].GetPointer());
	if( pNormalHelmet && ( pNormalHelmet->IsAttachHeadBone() || pNormalHelmet->IsIgnoreCombine() ) ) bNormalPartsIgnoreCombine = true;
	CDnPartsHelmet *pCashHelmet = dynamic_cast<CDnPartsHelmet *>(m_hCashPartsObject[CDnParts::CashHelmet].GetPointer());
	if( pCashHelmet && ( pCashHelmet->IsAttachHeadBone() || pCashHelmet->IsIgnoreCombine() ) ) bCashPartsIgnoreCombine = true;

	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( ( pGameTask ) && ( pGameTask->GetGameTaskType() == GameTaskType::PvP ) )
	{
		if( !m_bIsShowPVPHelmet )
		{
			bNormalPartsIgnoreCombine = false;
			bCashPartsIgnoreCombine = false;
		}
	}

	if( m_bHideHelmet )
	{
		bNormalPartsIgnoreCombine = false;
		bCashPartsIgnoreCombine = false;
	}

	DnPartsHandle hParts;
	if( m_bPartsViewOrder[CDnParts::CashHelmet] )
	{
		hParts = m_hCashPartsObject[CDnParts::CashHelmet];
		if( bCashPartsIgnoreCombine && !hParts->GetObjectHandle() )
		{
			hParts->CreateObject( pActor->GetMySmartPtr(), m_szAniFileName.c_str() );
			LinkCashParts( CDnParts::CashHelmet );
			UpdateColorParam( SkinColor );
		}
	}
	else
	{
		hParts = m_hPartsObject[CDnParts::Helmet];
		if( bNormalPartsIgnoreCombine && !hParts->GetObjectHandle() )
		{
			hParts->CreateObject( pActor->GetMySmartPtr(), m_szAniFileName.c_str() );
		}
	}

	int nLevel = 0;
	if( hParts ) nLevel = GetHairLevel( hParts );
	RefreshHairLevel( nLevel );

	if( m_bHideHelmet )
	{
		if( hParts && hParts->GetObjectHandle() )
			hParts->FreeObject();

		if( m_bPartsViewOrder[CDnParts::CashHelmet] ) hParts = m_hPartsObject[CDnParts::Helmet];
		else hParts = m_hCashPartsObject[CDnParts::CashHelmet];
		if( hParts && hParts->GetObjectHandle() )
			hParts->FreeObject();
	}

	if( bChangeParts && bNormalPartsIgnoreCombine == false || 
		bChangeParts && bCashPartsIgnoreCombine == false )
		GetItemTask().InsertChangePartsThread( pActor->GetMySmartPtr() );
}