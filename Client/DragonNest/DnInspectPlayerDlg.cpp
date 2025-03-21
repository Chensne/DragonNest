#include "StdAfx.h"
#include "DnInspectPlayerDlg.h"
#include "DnInterfaceString.h"
#include "EtUIControl.h"
#include "DnItemSlotButton.h"
#include "DnActor.h"
#include "MAPartsBody.h"
#include "DnCharStatusDlg.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "DnNameLinkMng.h"
#include "DnChatTabDlg.h"
#include "DnAppellationTask.h"

#include "DNProtocol.h"
#include "DnInspectGuildInfoDlg.h"
#include "DnGuildTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInspectPlayerDlg::CDnInspectPlayerDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_dwSessionID = 0;
	m_pStaticName = NULL;
	m_pStaticLevel = NULL;
	m_pStaticJob = NULL;
	m_pStaticAppellation = NULL;
	m_pInspectPlateDlg = NULL;
	m_fMouseX = 0.f;
	m_fMouseY = 0.f;

	m_pPVPIconTexCon = NULL;
	m_pPVPRankNameStCon = NULL;	
	m_bShowCashEquip = false;

	m_pSourceItemSlot = NULL;
	m_pSourceItem = NULL;

#ifdef PRE_ADD_BESTFRIEND
	m_pBestfriendName = NULL;
#endif

	m_pInspectGuildInfoDlg = NULL;
	m_pRotate = NULL;
	m_pRotateBack = NULL;
	m_pRotateFront = NULL;
	m_pRotateLeft = NULL;
	m_pRotateRight = NULL;
#ifdef PRE_ADD_TALISMAN_SYSTEM
	m_pInspectTalismanDlg = NULL;
#endif
#ifdef PRE_ADD_COSTUME_SKILL
	m_pCostumeSkillSlotButton = NULL;
#endif

}

CDnInspectPlayerDlg::~CDnInspectPlayerDlg(void)
{	
	m_vecSlotButton.clear();
	m_vecCashSlotButton.clear();

#ifdef PRE_ADD_TALISMAN_SYSTEM
	SAFE_DELETE( m_pInspectTalismanDlg );
#endif

	SAFE_DELETE( m_pInspectPlateDlg );

	for(DWORD n=0; n<m_vecInspectItemList.size(); n++ )
		SAFE_DELETE( m_vecInspectItemList[n] );
	m_vecInspectItemList.clear();

	SAFE_DELETE(m_pInspectGuildInfoDlg);
}

void CDnInspectPlayerDlg::Initialize(bool bShow)
{
	// CDnPlayerInfoDlg 는 캐릭생성시 이미 쓰이고 있어서 클래스 이름을 CDnInspectPlayerDlg 라고 지었다. 혼동 주의.
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "PlayerInfoDlg.ui" ).c_str(), bShow );	
}

void CDnInspectPlayerDlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>("ID_NAME");
	m_pStaticLevel = GetControl<CEtUIStatic>("ID_LEVEL");
	m_pStaticJob = GetControl<CEtUIStatic>("ID_JOB");

	m_pStaticAppellation = GetControl<CEtUIStatic>( "ID_APP" );

	m_pInspectPlateDlg = new CDnInspectPlateDlg( UI_TYPE_CHILD, this );
	m_pInspectPlateDlg->Initialize( false );

	m_pRotate = GetControl<CEtUIButton>("ID_ROTATE");
	m_pRotateLeft = GetControl<CEtUIButton>("ID_ROTATE_LEFT");
	m_pRotateRight = GetControl<CEtUIButton>("ID_ROTATE_RIGHT");	

	m_pRotateFront = GetControl<CEtUIButton>("ID_ROTATE_CENTER");
	m_pRotateBack = GetControl<CEtUIButton>("ID_ROTATE_BACK");

	SUICoord CharViewCoord = GetControl<CEtUIButton>("ID_ROTATE")->GetProperty()->UICoord;
	
	const int nTextureSize = 512;

	SCameraInfo CameraInfo;
	//CameraInfo.fWidth = (float)viewport.Width;	// 직교-렌더타겟에선 생략
	//CameraInfo.fHeight = (float)viewport.Height;
	CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	CameraInfo.fNear = 10.f;
	CameraInfo.Type = CT_ORTHOGONAL;	// 직교로 그리는게 나아보인다. 근데 이걸로 하면 Aspect안먹어서 직접ui비율 맞춰줘야한다.
	//CameraInfo.Type = CT_PERSPECTIVE;	// 원근하면 AspectRatio 먹일 수 있다.

	// 160, 120 정도 주면, 캐릭터 화면에 꽉 안차고 잘 보일정도의 크기다.
	m_RenderAvatar.Initialize( nTextureSize, nTextureSize, CameraInfo, 160, 120, FMT_A8R8G8B8, true, false );

	// 직교에선 직접 UV계산하니 AspectRatio는 설정안해도 된다.
	//m_RenderAvatar.SetAspectRatio( (CharViewCoord.fWidth * DEFAULT_UI_SCREEN_WIDTH) / (CharViewCoord.fHeight * DEFAULT_UI_SCREEN_HEIGHT) );
	// 직교투영으로 그렸으니 시저렉을 쓰던 uv를 계산해서 ui에 맞게 그려줘야하는데, 시저렉은 디바이스건드리는거라 별로고, uv계산해서 주겠다.
	m_RenderAvatar.CalcUVCoord( CharViewCoord.fWidth, CharViewCoord.fHeight );

	m_pPVPIconTexCon = GetControl<CEtUITextureControl>("ID_TEXTURE_PVP");
	m_pPVPRankNameStCon = GetControl<CEtUIStatic>("ID_PVPRANK");

	m_pInspectGuildInfoDlg = new CDnInspectGuildInfoDlg(UI_TYPE_CHILD, this);
	m_pInspectGuildInfoDlg->Initialize(false);
	
#ifdef PRE_ADD_TALISMAN_SYSTEM
	m_pInspectTalismanDlg = new CDnInspectTalismanDlg(UI_TYPE_CHILD, this);
	m_pInspectTalismanDlg->Initialize(false);
#endif

#ifdef PRE_ADD_COSTUME_SKILL
	m_pCostumeSkillSlotButton = GetControl<CDnQuickSlotButton>("ID_ITEM_CASH_SKILL");
#endif

	SwapEquipButtons( false );
}

void CDnInspectPlayerDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnItemSlotButton *pItemSlotButton(NULL);
	pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);

	if( strstr( pControl->GetControlName(), "ID_ITEM_CASH" ) ) {
		pItemSlotButton->SetSlotIndex((int)m_vecCashSlotButton.size());
		pItemSlotButton->SetSlotType(ST_INSPECT);
		m_vecCashSlotButton.push_back( pItemSlotButton );
	}
	else {
		pItemSlotButton->SetSlotIndex((int)m_vecSlotButton.size());
		pItemSlotButton->SetSlotType(ST_INSPECT);
		m_vecSlotButton.push_back( pItemSlotButton );

		if( strcmp( pControl->GetControlName(), "ID_ITEM_CREST" ) == 0 )
			m_pSourceItemSlot = pItemSlotButton;
	}
}

void CDnInspectPlayerDlg::SwapEquipButtons( bool bShowCashEquip )
{
	SwapPlateDlg( false );
#ifdef PRE_ADD_TALISMAN_SYSTEM
	SwapTalismanDlg( false );
#endif

	CDnItemSlotButton *pSlot = NULL;
	m_bShowCashEquip = bShowCashEquip;
	if( m_bShowCashEquip ) {
		CONTROL( Static, ID_BASE_NORMAL )->Show( false );
		CONTROL( Static, ID_BASE_CASH )->Show( true );		
		for( DWORD i=0; i<m_vecSlotButton.size(); i++ ) {
			pSlot = m_vecSlotButton[ i ];
			pSlot->Show( false );
		}
		for( DWORD i=0; i<m_vecCashSlotButton.size(); i++ ) {
			pSlot = m_vecCashSlotButton[ i ];
			pSlot->Show( true );
		}
	}
	else {
		CONTROL( Static, ID_BASE_CASH )->Show( false );
		CONTROL( Static, ID_BASE_NORMAL )->Show( true );
		for( DWORD i=0; i<m_vecCashSlotButton.size(); i++ ) {
			pSlot = m_vecCashSlotButton[ i ];
			pSlot->Show( false );			
		}
		for( DWORD i=0; i<m_vecSlotButton.size(); i++ ) {
			pSlot = m_vecSlotButton[i];
			pSlot->Show( true );			
		}
	}
	RefreshOnepieceBlock();
}

void CDnInspectPlayerDlg::SwapPlateDlg( bool bPlate )
{
	if( bPlate )
	{
		ShowEquipModeUIControl( false );
		ShowChildDialog( m_pInspectPlateDlg, true );

		ShowChildDialog( m_pInspectGuildInfoDlg, false );
	}
	else
	{
		ShowEquipModeUIControl( true );
		ShowChildDialog( m_pInspectPlateDlg, false );

		ShowChildDialog( m_pInspectGuildInfoDlg, false );
	}	
}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
void CDnInspectPlayerDlg::SwapTalismanDlg( bool bShow )
{
	if( bShow )
	{
		ShowEquipModeUIControl( false );
		ShowChildDialog( m_pInspectTalismanDlg, true );
		ShowChildDialog( m_pInspectGuildInfoDlg, false );
	}
	else
	{
		ShowEquipModeUIControl( true );
		ShowChildDialog( m_pInspectTalismanDlg, false );
		ShowChildDialog( m_pInspectGuildInfoDlg, false );
	}	
}
#endif // PRE_ADD_TALISMAN_SYSTEM

void CDnInspectPlayerDlg::ShowEquipModeUIControl( bool bShow )
{
	GetControl<CEtUIButton>( "ID_ROTATE_LEFT" )->Show( bShow );
	GetControl<CEtUIButton>( "ID_ROTATE_RIGHT" )->Show( bShow );
	GetControl<CEtUIButton>( "ID_ROTATE_CENTER" )->Show( bShow );
	GetControl<CEtUIButton>( "ID_ROTATE_BACK" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_BASE_NORMAL" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_BASE_CASH" )->Show( bShow );

	GetControl<CEtUIStatic>( "ID_NAME" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_LEVEL" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_JOB" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_TEXT" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_APP" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_PVPRANK" )->Show( bShow );

	for( DWORD i=0; i<m_vecSlotButton.size(); i++ ) {
		m_vecSlotButton[i]->Show( bShow );
	}

	for( DWORD i=0; i<m_vecCashSlotButton.size(); i++ ) {
		m_vecCashSlotButton[i]->Show( bShow );
	}
}

bool CDnInspectPlayerDlg::CheckValidActor()
{
	bool bValid = true;
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( m_dwSessionID );
	if( !hActor || hActor->IsDestroy() ) {
		bValid = false;
	}
	else {
		const float fDistanceLimit = 500.0f;
		if( CDnActor::GetLocalActor() && EtVec3Length( &(*hActor->GetPosition() - *CDnActor::GetLocalActor()->GetPosition() )) > fDistanceLimit  ) {
			bValid = false;
		}
	}
	return bValid;
}

void CDnInspectPlayerDlg::RefreshItemSlots()
{
	m_bShowCashEquip = false;
	for( int i = 0; i < (int)m_vecSlotButton.size(); i++) {
		m_vecSlotButton[ i ]->ResetSlot();
		m_vecSlotButton[ i ]->OnRefreshTooltip();
	}	
	
	for( int i = 0; i < (int)m_vecCashSlotButton.size(); i++) {
		m_vecCashSlotButton[ i ]->ResetSlot();
		m_vecCashSlotButton[ i ]->OnRefreshTooltip();
	}

	if( !CheckValidActor() ) {
		Show( false );
		return;
	}	

	for(DWORD n=0; n<m_vecInspectItemList.size(); n++ )
		SAFE_DELETE( m_vecInspectItemList[n] );

	m_vecInspectItemList.clear();
	
	m_RenderAvatar.ResetActor();
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( m_dwSessionID );
	m_RenderAvatar.SetActor( hActor, true, false );

	CDnPlayerActor *pDnPlayerActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
	if( !pDnPlayerActor ) return;	

	int nNudeParts[ CDnParts::DefaultPartsTypeEnum_Amount ];
	int nDefaultParts[ CDnParts::DefaultPartsTypeEnum_Amount ];
	for( int i = 0; i < CDnParts::DefaultPartsTypeEnum_Amount; i++) {
		nNudeParts[ i ] = pDnPlayerActor->GetNudePartsInfo( (CDnParts::PartsTypeEnum)(CDnParts::Body + i) );
		nDefaultParts[ i ] = pDnPlayerActor->GetDefaultPartsInfo( (CDnParts::PartsTypeEnum)(CDnParts::Body + i) );
	}

	wchar_t wszTemp[ 256 ] = { 0 };
	m_pStaticName->SetText( hActor->GetName() );
	swprintf_s( wszTemp, 256, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), hActor->GetLevel() );
	m_pStaticLevel->SetText( wszTemp );	
	
#ifdef PRE_ADD_BESTFRIEND
	m_BFserial = hActor->GetBFserial();
	m_pBestfriendName = hActor->GetBestfriendName();
#endif

	m_pStaticJob->SetText( pDnPlayerActor->GetJobName() );
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );
	int nItemID = pSox->GetItemID( pDnPlayerActor->GetAppellationIndex() );

	if( nItemID != -1 )
	{
#ifdef PRE_ADD_BESTFRIEND
		DNTableCell * pCell = pSox->GetFieldFromLablePtr( nItemID, "_Type" );	
		if( pCell && pCell->GetInteger() == AppellationType::Type::eCode::BestFriend )	
		{			
			wchar_t buf[256] = {0,};
			swprintf_s( buf, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger() ), m_pBestfriendName );
			m_pStaticAppellation->SetText( buf );
		}
#else
		m_pStaticAppellation->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger() ) );
#endif
	}
	else
		m_pStaticAppellation->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 93 ) );

	m_vecEquipInfo.clear();

	SEquipInfo Info;
	MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody*>(hActor.GetPointer());
	for( int nIndex = CDnParts::Face; nIndex < CDnParts::PartsTypeEnum_Amount; nIndex++) {
		DnPartsHandle hParts = pPartsBody->GetParts( (CDnParts::PartsTypeEnum)nIndex);
		if( !hParts ) continue;
		CDnItem *pItem = (CDnItem *)hParts.GetPointer();
		/*
		if( nIndex == CDnParts::Hair ) {
			dwHairColor = pItem->GetColor();
		}
		*/
		if( pDnPlayerActor->IsNudeParts( hParts ) ) continue;

		if( nIndex >= CDnParts::Body && nIndex <= CDnParts::Foot ) {
			if( hParts->GetClassID() == nNudeParts[ nIndex - CDnParts::Body ] ) continue;
		}

		int nSlotIndex = CDnCharStatusDlg::EQUIPINDEX_2_SLOTINDEX( nIndex );
		if( nSlotIndex != -1 ) {
			TItemInfo ItemInfo;
			pItem->GetTItemInfo( ItemInfo );
			ItemInfo.Item.nSerial = pItem->GetSerialID();
			CDnItem *pNewItem = GetItemTask().CreateItem( ItemInfo );
			m_vecInspectItemList.push_back(pNewItem);

			if (pNewItem)
				pNewItem->SetEternityItem(true);
#ifdef PRE_MOD_INSPECT_DURABILITY
			CDnParts *pNewParts = dynamic_cast<CDnParts *>(pNewItem);
			if (pNewParts) pNewParts->SetDurability( pNewParts->GetMaxDurability() );
#endif

			m_vecSlotButton[nSlotIndex]->SetItem( pNewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
			m_vecSlotButton[nSlotIndex]->OnRefreshTooltip();
			Info.nClassID = pItem->GetClassID();
			Info.nSetItemID = hParts->GetSetItemID();
			m_vecEquipInfo.push_back( Info );
		}
	}

	for( int nIndex = CDnParts::CashHelmet; nIndex < CDnParts::CashPartsTypeEnum_Amount; nIndex++) {	
		DnPartsHandle hParts = pPartsBody->GetCashParts( (CDnParts::PartsTypeEnum)nIndex );
		if( !hParts ) continue;
		CDnItem *pItem = (CDnItem *)hParts.GetPointer();
		if( pDnPlayerActor->IsNudeParts( hParts ) ) continue;
		int nSlotIndex = CDnCharStatusDlg::CASH_EQUIPINDEX_2_SLOTINDEX( nIndex );
		if( nSlotIndex != -1 ) {
			TItemInfo ItemInfo;
			pItem->GetTItemInfo( ItemInfo );
			ItemInfo.Item.nSerial = pItem->GetSerialID();
			CDnItem *pNewItem = GetItemTask().CreateItem( ItemInfo );
			m_vecInspectItemList.push_back(pNewItem);

			if (pNewItem)
				pNewItem->SetEternityItem(true);
#ifdef PRE_MOD_INSPECT_DURABILITY
			CDnParts *pNewParts = dynamic_cast<CDnParts *>(pNewItem);
			if (pNewParts) pNewParts->SetDurability( pNewParts->GetMaxDurability() );
#endif


			m_vecCashSlotButton[ nSlotIndex ]->SetItem( pNewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
			m_vecCashSlotButton[ nSlotIndex ]->OnRefreshTooltip();
			Info.nClassID = pItem->GetClassID();
			Info.nSetItemID = hParts->GetSetItemID();
			m_vecEquipInfo.push_back( Info );
		}
	}

	for( int i = 0; i < 2; i++) {
		if( hActor->GetWeapon( i ) ) {
			CDnWeapon *pWeapon = hActor->GetWeapon( i ).GetPointer();
			int nSlotIndex = CDnCharStatusDlg::EQUIPINDEX_2_SLOTINDEX(  CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() ) );
			if( nSlotIndex != -1 ) {

				TItemInfo ItemInfo;
				((CDnItem*)pWeapon)->GetTItemInfo(ItemInfo);
				ItemInfo.Item.nSerial = pWeapon->GetSerialID();
				CDnItem *pNewItem = GetItemTask().CreateItem( ItemInfo );
				m_vecInspectItemList.push_back(pNewItem);

				if (pNewItem)
					pNewItem->SetEternityItem(true);
#ifdef PRE_MOD_INSPECT_DURABILITY
				CDnWeapon *pNewWeapon = dynamic_cast<CDnWeapon *>(pNewItem);
				if (pNewWeapon) pNewWeapon->SetDurability( pNewWeapon->GetMaxDurability() );
#endif


				m_vecSlotButton[ nSlotIndex ]->SetItem((CDnItem*)pNewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
				m_vecSlotButton[ nSlotIndex ]->OnRefreshTooltip();
				Info.nClassID = pWeapon->GetClassID();
				Info.nSetItemID = pWeapon->GetSetItemID();
				m_vecEquipInfo.push_back( Info );
			}
		}
	}

	for( int i = 0; i < 2; i++) {
		CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
		if( pActor && pActor->GetCashWeapon( i ) ) {
			CDnWeapon *pWeapon = pActor->GetCashWeapon( i ).GetPointer();
			int nSlotIndex = CDnCharStatusDlg::CASH_EQUIPINDEX_2_SLOTINDEX( CDnCharStatusDlg::CASH_EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() ) );
			if( nSlotIndex != -1 ) {

				TItemInfo ItemInfo;
				((CDnItem*)pWeapon)->GetTItemInfo(ItemInfo);
				ItemInfo.Item.nSerial = pWeapon->GetSerialID();
				CDnItem *pNewItem = GetItemTask().CreateItem( ItemInfo );
				m_vecInspectItemList.push_back(pNewItem);

				if (pNewItem)
					pNewItem->SetEternityItem(true);
#ifdef PRE_MOD_INSPECT_DURABILITY
				CDnWeapon *pNewWeapon = dynamic_cast<CDnWeapon *>(pNewItem);
				if (pNewWeapon) pNewWeapon->SetDurability( pNewWeapon->GetMaxDurability() );
#endif

				m_vecCashSlotButton[ nSlotIndex ]->SetItem( pNewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
				m_vecCashSlotButton[ nSlotIndex ]->OnRefreshTooltip();
				Info.nClassID = pWeapon->GetClassID();
				Info.nSetItemID = pWeapon->GetSetItemID();
				m_vecEquipInfo.push_back( Info );
			}
		}
	}

	//blondy
	char cPVPRank = pDnPlayerActor->GetPvPLevel();
	if( pDnPlayerActor )
	{
		//아이콘 찍기
		if( GetInterface().GetPVPIconTex() )
		{
			if( pDnPlayerActor )
			{		
				int iIconW,iIconH;
				int iU,iV;

				iIconW = GetInterface().GeticonWidth();
				iIconH = GetInterface().GeticonHeight();

				if( GetInterface().ConvertPVPGradeToUV( cPVPRank, iU, iV ) )
				{
					m_pPVPIconTexCon->SetTexture(GetInterface().GetPVPIconTex(),iU, iV ,iIconW,iIconH );
					m_pPVPIconTexCon->Show(true);
				}
			}
		}

		//등급명 찍기 
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
		if ( pSox )
		{
			if( pSox->IsExistItem( cPVPRank ))
			{
				int iUIString = pSox->GetFieldFromLablePtr( cPVPRank, "PvPRankUIString" )->GetInteger();
				m_pPVPRankNameStCon->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iUIString ));
			}
		}
	}
	//blondy end

	SAFE_DELETE( m_pSourceItem );
	m_pSourceItemSlot->SetItem( NULL, CDnSlotButton::ITEM_ORIGINAL_COUNT );
	int iNumSourceItem = pDnPlayerActor->GetNumEffectSkill();

	if( 0 < iNumSourceItem ) 	// 현재는 한개만 있음.
	{
		const CDnPlayerActor::S_EFFECT_SKILL* pUsedItem = pDnPlayerActor->GetEffectSkillFromIndex( 0 );
		if( pUsedItem )
		{
			DNTableFileFormat* pItemSox = GetDNTable( CDnTableDB::TITEM );
			if( (pItemSox && pItemSox->IsExistItem( pUsedItem->iItemID )) ) 
			{
				eItemTypeEnum eType = (eItemTypeEnum)pItemSox->GetFieldFromLablePtr( pUsedItem->iItemID, "_Type" )->GetInteger();
				if( eType == ITEMTYPE_SOURCE )
				{
					// 할당된 아이템 객체는 이쪽 루틴으로 새로 오면 delete 되며 여기서 다시 할당.
					// 최종적으로는 이 다이얼로그 객체가 소멸될 때 각 퀵슬롯으 돌면서 아이템 객체를 delete 하는 루틴에 의해서 제거된다.
					m_pSourceItem = CDnItem::CreateItem( pUsedItem->iItemID, 1 );
					if( m_pSourceItem )
					{
						// 장착시에는 스킬 데이터에 있는 아이콘 인덱스로 교체. (아이템 아이콘 인덱스 기준으로 저장되어있음 그냥 바꿔주면 됨)
						m_pSourceItem->ChangeToSkillIconIndex();
						m_pSourceItemSlot->SetItem( m_pSourceItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
					}
				}
			}
		}
	}

#ifdef PRE_ADD_COSTUME_SKILL
	if( pDnPlayerActor && pDnPlayerActor->GetCostumeSkillIndex() )
	{
		DnSkillHandle hSkill = pDnPlayerActor->FindSkill( pDnPlayerActor->GetCostumeSkillIndex() );
		if( hSkill )
			m_pCostumeSkillSlotButton->SetQuickItem( hSkill.GetPointer() );
	}
#endif

	RefreshOnepieceBlock();
}

void CDnInspectPlayerDlg::SetSessionID( DWORD  dwSessionID )
{
	m_dwSessionID = dwSessionID;
	RefreshItemSlots();
}

int CDnInspectPlayerDlg::GetSessionID()
{
	return m_dwSessionID;
}

void CDnInspectPlayerDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;
	BaseClass::Show( bShow );
	
	if( bShow ) {
		GetControl<CEtUIRadioButton>("ID_RBT_NORMAL")->SetChecked( true );
		GetInterface().CloseAllMainMenuDialog();
		if( GetInterface().GetNameLinkToolTipDlg() ) GetInterface().GetNameLinkToolTipDlg()->Show(false);
		SetRenderPriority( this, true );
	}
	else {
		m_RenderAvatar.ResetActor();
		m_dwSessionID = 0;
		m_pStaticName->SetText( L"" );
		m_pStaticLevel->SetText( L"" );
		m_pStaticJob->SetText( L"" );
		m_pStaticAppellation->SetText( L"" );
		m_pPVPIconTexCon->Show(false);
		m_pPVPRankNameStCon->SetText( L"" );

#ifdef PRE_ADD_BESTFRIEND
		m_pBestfriendName = NULL;
#endif
		SwapEquipButtons( false );
	}
}

void CDnInspectPlayerDlg::Process( float fElapsedTime )
{
	BaseClass::Process( fElapsedTime );

	if( IsShow() && !CDnMouseCursor::GetInstance().IsShowCursor() ) {
		CDnMouseCursor::GetInstance().ShowCursor( true, true );
	}

	if( !CheckValidActor() ) {
		Show( false );
		return;
	}

	m_RenderAvatar.Process( fElapsedTime );

	static float fRotScale = 200.f;
	if( m_pRotateLeft->IsPressed()  )
	{
		m_RenderAvatar.AddRotateYaw( fElapsedTime * fRotScale );
	}
	else if( m_pRotateRight->IsPressed()  )
	{
		m_RenderAvatar.AddRotateYaw( -fElapsedTime * fRotScale );
	}

	if( m_pRotate->IsPressed()  )
	{
		float fX = m_pRotate->GetMouseCoord().fX;
		float fY = m_pRotate->GetMouseCoord().fY;
		if( m_fMouseX != 0.f && m_fMouseY != 0.f ) {
			static float fMoveScale = 500.f;
			float fAddAngle = fMoveScale * sqrtf( (m_fMouseX-fX)*(m_fMouseX-fX)+(m_fMouseY-fY)*(m_fMouseY-fY) ) *  (((m_fMouseX-fX)>0.f)? 1.f : -1.f) ;
			m_RenderAvatar.AddRotateYaw( fAddAngle );
		}
		m_fMouseX = fX;
		m_fMouseY = fY;
	}
	else {
		m_fMouseX = 0.f;
		m_fMouseY = 0.f;
	}

	if( m_pInspectPlateDlg->IsShow() || m_pInspectGuildInfoDlg->IsShow() ) 
		return;

	if( m_RenderAvatar.IsFrontView() ) {
		m_pRotateFront->Show( false );
		m_pRotateBack->Show( true );
	}
	else {
		m_pRotateFront->Show( true );
		m_pRotateBack->Show( false );
	}
}

void CDnInspectPlayerDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_DOUBLE_CLICKED )
	{
		if( IsCmdControl("ID_ROTATE")) {
			m_RenderAvatar.SetFrontView();
		}
	}
	else if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_ROTATE_CENTER") )
		{
			m_RenderAvatar.SetFrontView();
		}
		else if( IsCmdControl("ID_ROTATE_BACK") )
		{
			m_RenderAvatar.SetRearView();
		}
		else if( IsCmdControl("ID_BUTTON_CLOSE"))
		{
			Show( false );			
		}
		else if( strstr( pControl->GetControlName(), "ID_ITEM_" ) )
		{
			CDnSlotButton *pDragButton;
			CDnItemSlotButton *pPressedButton;
			pDragButton = ( CDnSlotButton * )drag::GetControl();
			pPressedButton = ( CDnItemSlotButton * )pControl;
			if( pDragButton == NULL )
			{
				if( !pPressedButton->GetItem() ) 
					return;
				CDnItem *pItem = (CDnItem*)pPressedButton->GetItem();
				if (uMsg & VK_SHIFT)
				{
					if (GetInterface().SetNameLinkChat(*pItem))
						return;
				}
			}
		}
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED ) {
		if( IsCmdControl( "ID_RBT_NORMAL" ) ) {
			SwapEquipButtons( false );
		}
		else if( IsCmdControl( "ID_RBT_CASH" ) ) {
			SwapEquipButtons( true );
		}
		else if( IsCmdControl( "ID_RBT_PLATE" ) ) {
			SwapPlateDlg( true );
		}
		else if( IsCmdControl( "ID_RBT_GUILD" ) ) 
		{
			bool bValid = true;		
			DnActorHandle hActor = CDnActor::FindActorFromUniqueID( m_dwSessionID );
			if( !hActor || hActor->IsDestroy() ) {
				bValid = false;
			}		
			if (bValid)
				GetGuildTask().RequestPlayerGuildInfo(m_dwSessionID);
			
			//SwapGuildInfoDlg( true );
		}
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		else if( IsCmdControl( "ID_RBT_TALISMAN" ) ) 
		{
			SwapTalismanDlg(true);
		}
#endif // PRE_ADD_TALISMAN_SYSTEM
	}
}

void CDnInspectPlayerDlg::Render( float fElapsedTime )
{
	BaseClass::Render( fElapsedTime );

	if( IsShow() && !m_pInspectPlateDlg->IsShow() 
		&& !m_pInspectGuildInfoDlg->IsShow()
#ifdef PRE_ADD_TALISMAN_SYSTEM
		&& !m_pInspectTalismanDlg->IsShow()
#endif
		)
	{
		SUICoord CharViewCoord = GetControl<CEtUIButton>("ID_ROTATE")->GetProperty()->UICoord;
		DrawSprite( m_RenderAvatar.GetRTT(), m_RenderAvatar.GetUVCoord(), 0xFFFFFFFF, CharViewCoord );
	}
}
 
void CDnInspectPlayerDlg::GetEquipInfo( std::vector<SEquipInfo> &vecEquipInfo )
{
	vecEquipInfo = m_vecEquipInfo;
}

void CDnInspectPlayerDlg::RefreshOnepieceBlock()
{
	for( int i=0; i<(int)m_vecCashSlotButton.size(); i++ )
	{
		m_vecCashSlotButton[i]->SetClosed( false );
	}

	if( m_bShowCashEquip ) {
		for( int i=0; i<CASHEQUIPMAX; i++ ) {

			if(i==CASHEQUIP_EFFECT)
				continue;

			if( i > CDnParts::CashFairy ) continue;
			CDnItem *pItem = (CDnItem *)m_vecCashSlotButton[i]->GetItem();
			if( !pItem ) continue;
			if( pItem->GetItemType() != ITEMTYPE_PARTS ) continue;
			CDnParts *pParts = static_cast<CDnParts *>(pItem);
			if( !pParts->IsExistSubParts() ) continue;
			for( int j=0; j<pParts->GetSubPartsCount(); j++ ) {
				CDnParts::PartsTypeEnum SubPartsType = pParts->GetSubPartsIndex(j);
				m_vecCashSlotButton[SubPartsType]->SetClosed( true );
			}
		}
	}
}

void CDnInspectPlayerDlg::SwapGuildInfoDlg(bool bShow)
{
	if( bShow )
	{
		ShowEquipModeUIControl( true );
		ShowBaseItem(false);
		ShowChildDialog( m_pInspectPlateDlg, false);
		ShowChildDialog( m_pInspectGuildInfoDlg, true );
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		ShowChildDialog( m_pInspectTalismanDlg, false);
#endif
		for( DWORD i=0; i<m_vecCashSlotButton.size(); i++ ) 
		{
			m_vecCashSlotButton[i]->Show( false );
		}
	}
	else
	{
		ShowEquipModeUIControl( true );
		ShowChildDialog( m_pInspectPlateDlg, false );
		ShowChildDialog( m_pInspectGuildInfoDlg, false );
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		ShowChildDialog( m_pInspectTalismanDlg, false);
#endif
	}
}

void CDnInspectPlayerDlg::ShowBaseItem(bool bShow)
{
	GetControl<CEtUIButton>( "ID_ROTATE_LEFT" )->Show( bShow );
	GetControl<CEtUIButton>( "ID_ROTATE_RIGHT" )->Show( bShow );
	GetControl<CEtUIButton>( "ID_ROTATE_CENTER" )->Show( bShow );
	GetControl<CEtUIButton>( "ID_ROTATE_BACK" )->Show( bShow );
	//GetControl<CEtUIStatic>( "ID_BASE_NORMAL" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_BASE_CASH" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_STATIC1" )->Show( bShow );
	

	GetControl<CEtUIStatic>( "ID_NAME" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_LEVEL" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_JOB" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_TEXT" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_APP" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_PVPRANK" )->Show( bShow );
	if (m_pPVPIconTexCon)
		m_pPVPIconTexCon->Show(bShow);
}