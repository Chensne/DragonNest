#include "StdAfx.h"

#if defined(PRE_ADD_TALISMAN_SYSTEM) 
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnMainMenuDlg.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif
#include "DnCharTalismanDlg.h"
#include "DnCharTalismanInfoDlg.h"
#include "DnTableDB.h"
#include "DnLocalPlayerActor.h"
#include "DnFadeInOutDlg.h"
#include "DnCharTalismanListDlg.h"
#include "DnTooltipTalismanDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const int	TALISMAN_TREE_SCROLL_TRACK_PAGESIZE = 25;
const float TALISMAN_TREE_SCROLL_TRACK_RANGE	= 40.0f;
const int	TALISMAN_SLOT_OFFSET	= 70;
const int	TALISMAN_SLOT_WIDTH_CNT = 4;

CDnCharTalismanDlg::CDnCharTalismanDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback)
, m_pDetailInfoDlg(NULL)
, m_pChangeApplyBtn(NULL)
, m_pDetailInfoBtn(NULL)
, m_pCombineBtn(NULL)
, m_pSlotDlg(NULL)
, m_pScrollBar(NULL)
, m_pInfomationStaticText(NULL)
, m_fOriginalPosX(.0f)
, m_fOriginalPosY(.0f)
, m_nScrollPos(-1)
, m_nSlotMaxCount(0)
, m_nUseMoneySoundIndex(-1)
, m_bIsEditMode(false)
, m_bIsEquipStart(false)
, m_nCurrentSlotType(ST_ITEM_NONE)
, m_nSlotChangeCount(0)
{
}

CDnCharTalismanDlg::~CDnCharTalismanDlg()
{
	SAFE_DELETE(m_pSlotDlg);
	SAFE_DELETE(m_pDetailInfoDlg);	
}

void CDnCharTalismanDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharTalismanDlg.ui" ).c_str(), bShow );
}

void CDnCharTalismanDlg::InitialUpdate()
{	
	m_pSlotDlg = new CDnCharTalismanListDlg(UI_TYPE_CHILD, this);
	m_pSlotDlg->Initialize(false);

	m_pDetailInfoDlg = new CDnCharTalismanInfoDlg(UI_TYPE_CHILD, this);
	m_pDetailInfoDlg->Initialize(false);

	m_pChangeApplyBtn = GetControl<CEtUIButton>("ID_BT_OK");
	m_pDetailInfoBtn  = GetControl<CEtUIButton>("ID_BT_INFO");
	m_pCombineBtn	  = GetControl<CEtUIButton>("ID_BT_COMPOSE");
	m_pListBoxEx	  = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_TALISMAN");
	m_pInfomationStaticText = GetControl<CEtUIStatic>("ID_TEXT_OPTION");
	m_pInfomationStaticText->SetTextColor(textcolor::GOLD);
	m_pInfomationStaticText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8368)); // mid: ���� ��ġ�� ���� �������� �ɷ�ġ�� �����˴ϴ�.
	
	m_pScrollBar = m_pListBoxEx->GetScrollBar();
	m_pScrollBar->SetPageSize( TALISMAN_TREE_SCROLL_TRACK_PAGESIZE );
	m_pScrollBar->SetTrackRange( 0, (int)TALISMAN_TREE_SCROLL_TRACK_RANGE );	

	// ������ġ�� ���ų�, ���� ������ ��ư ��Ȱ��ȭ.
	GetControl<CEtUIButton>("ID_BT_SMALLHELP")->Enable(false);
	GetControl<CEtUIButton>("UI_BT_BUYSHORTCUT")->Enable(false);
	m_pCombineBtn->Enable(false); // ���� ��ġ�� �ռ� ��.

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TTALISMANSLOT);
	if(pSox) 
	{
		for(int i = 0 ; i < pSox->GetItemCount() ; ++i)
		{
			int ID = pSox->GetItemID(i);
			if(pSox->GetFieldFromLablePtr(ID, "_Service")->GetBool())
				++m_nSlotMaxCount;
		}
	}

	const char* szFileName = CDnTableDB::GetInstance().GetFileName(10003);
	if(strlen(szFileName) > 0) {
		m_nUseMoneySoundIndex = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}
}

void CDnCharTalismanDlg::ShowTalismanSlotEfficiency(bool bShow)
{
	m_bIsEquipStart = bShow;
	if(m_pSlotDlg)
		m_pSlotDlg->ShowTalismanSlotEfficiency(bShow);
}

#ifdef PRE_ADD_EQUIPLOCK
int CDnCharTalismanDlg::GetWearedSlotIndex(const CDnItem* pItem) const
{
	if(m_pSlotDlg == NULL)
		return -1;
	return m_pSlotDlg->GetWearedSlotIndex(pItem);
}
#else
int CDnCharTalismanDlg::GetWearedSlotIndex(CDnItem* pItem)
{
	if(m_pSlotDlg == NULL)
		return -1;
	return m_pSlotDlg->GetWearedSlotIndex(pItem);
}
#endif

CDnItem* CDnCharTalismanDlg::GetEquipTalisman(int nIndex)
{
	if(m_pSlotDlg == NULL)
		return NULL;
	return m_pSlotDlg->GetEquipTalisman(nIndex);
}

void CDnCharTalismanDlg::Reset()
{	
	SetEditMode(false);
	ShowTalismanSlotEfficiency(false);
	if(m_pSlotDlg) m_pSlotDlg->GetTalismanToolTip()->Show(false);
	m_pDetailInfoDlg->Show(false);
	m_pChangeApplyBtn->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8353)); // mid: Ż������ ��ġ����
	
	m_pInfomationStaticText->SetTextColor(textcolor::GOLD);
	m_pInfomationStaticText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8368)); // mid: ���� ��ġ�� ���� �������� �ɷ�ġ�� �����˴ϴ�.

	m_nSlotChangeCount = 0;
}

void CDnCharTalismanDlg::SetTalismanChangeText()
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TPLAYERCOMMONLEVEL);
	if(!pSox) return;
	if(!CDnActor::s_hLocalActor) return;

	int nMyLevel = CDnActor::s_hLocalActor->GetLevel();
	int nNeedMoney = pSox->GetFieldFromLablePtr(nMyLevel, "_TalismanCost")->GetInteger();

	int nGold	= int(nNeedMoney/10000);
	int nSilver = int((nNeedMoney%10000)/100);
	int nCopper = int(nNeedMoney%100);

	wchar_t  szString[256] = { 0 , };
	wsprintf(szString, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8369), nGold, nSilver, nCopper, ++m_nSlotChangeCount);
	m_pInfomationStaticText->SetTextColor(textcolor::RED);
	m_pInfomationStaticText->SetText(szString);
}

void CDnCharTalismanDlg::SetSlotOffset()
{
	if(m_pSlotDlg)
	{
		SUIDialogInfo info;
		m_pSlotDlg->GetDlgInfo(info);

		if(m_fOriginalPosX == .0f && m_fOriginalPosY == .0f)
		{
			m_fOriginalPosX = info.DlgCoord.fX;
			m_fOriginalPosY = info.DlgCoord.fY;
		}

		const SUICoord& ListBoxCoord = m_pListBoxEx->GetUICoord();
		float fPageSize = ListBoxCoord.fHeight / GetScreenHeightRatio() * GetEtDevice()->Height();
		m_pScrollBar->SetPageSize( (int)fPageSize );

		// UI�� �ִ���̴� Y:720�̴�.
		// OFFSET(70) * ((���̺��� Į������ / 4) - 1)
		// ex) 70 * ( 32�� / 4ĭ ) - 1;
		//	   70 * 7 = 490;				
		float fBGImageHeight = m_pSlotDlg->GetBGImageHeight();
		float fSize = fBGImageHeight / GetScreenHeightRatio() * GetEtDevice()->Height();
		int nEnd = int(fSize * ((m_nSlotMaxCount / TALISMAN_SLOT_WIDTH_CNT) - 1));
		m_pScrollBar->SetTrackRange(0, nEnd);

		int	  nCurrentPos		= m_pScrollBar->GetTrackPos();
		float fNowYOffsetRatio  = (float)nCurrentPos / fPageSize;
		float fHeight			= ListBoxCoord.fHeight / GetScreenHeightRatio();
		m_pSlotDlg->SetPosition( m_fOriginalPosX, m_fOriginalPosY-fNowYOffsetRatio*fHeight );
	}
}

void CDnCharTalismanDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	if(IsShow() && m_pSlotDlg)
	{
		SetSlotOffset();

		RECT oldRect, newRect;
		GetEtDevice()->GetScissorRect( &oldRect );

		const SUICoord& ContentListBoxCoord = m_pListBoxEx->GetUICoord();
		newRect.left	= int((ContentListBoxCoord.fX+GetXCoord()) / GetScreenWidthRatio() * GetEtDevice()->Width());
		newRect.top		= int((ContentListBoxCoord.fY+GetYCoord()) / GetScreenHeightRatio() * GetEtDevice()->Height());
		newRect.right	= newRect.left + int(ContentListBoxCoord.fWidth / GetScreenWidthRatio() * GetEtDevice()->Width() );
		newRect.bottom  = newRect.top + int(ContentListBoxCoord.fHeight / GetScreenHeightRatio() * GetEtDevice()->Height() );

		CEtSprite::GetInstance().Flush();
		GetEtDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE);
		GetEtDevice()->SetScissorRect( &newRect );

		m_pSlotDlg->Show(true);
		m_pSlotDlg->Render( fElapsedTime );
		m_pSlotDlg->Show(false);
		
		CEtSprite::GetInstance().Flush();
		GetEtDevice()->SetScissorRect( &oldRect );
		GetEtDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE);

		SetWeableSlotBlank();
	}
}

void CDnCharTalismanDlg::SetWeableSlotBlank()
{
	const std::vector<CDnCharTalismanListDlg::sTalismanSlotData> vItemList = m_pSlotDlg->GetItemSlotBtnList();
	if(vItemList.empty()) return;

	if(!CDnActor::s_hLocalActor)
		return;

	CDnLocalPlayerActor* pActor = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if(!pActor)
		return;

	if( drag::IsValid() )
	{
		CDnSlotButton *pDragButton = (CDnSlotButton*)drag::GetControl();
		m_nCurrentSlotType = pDragButton->GetSlotType();
		switch(m_nCurrentSlotType) 
		{
		case ST_INVENTORY:
		case ST_INVENTORY_CASH:
		case ST_CHARSTATUS:
		case ST_TALISMAN:
			break;
		default: return;
		}

		CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
		if( !pItem ) return;

		std::wstring wszErrorMessage;
		CDnCharStatusDlg::eRetWearable eResult = GetWearableTalismanEquipType(pItem, &wszErrorMessage);
		if(eResult != CDnCharStatusDlg::eRetWearable::eWEAR_ENABLE)
			return;

		switch( pItem->GetItemType() )
		{
		case ITEMTYPE_TALISMAN:
			{
				for(int i = 0 ; i < (int)vItemList.size() ; ++i)
				{
					if(!vItemList[i].bIsOpenSlot)
						continue;
					
					if(vItemList[i].nSlotOpenLevel > pActor->GetLevel())
						continue;
					
					if(vItemList[i].pSlotButton->IsEmptySlot())
						vItemList[i].pSlotButton->SetWearable(true);
				}
			}
			break;
		}
	}
	else
	{
		for( int i=0; i<(int)vItemList.size(); i++ )
			vItemList[i].pSlotButton->SetWearable( false );
	}
}

void CDnCharTalismanDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;	

	CEtUIDialog::Show(bShow);

	if( m_pSlotDlg == NULL ) 
		return;

	SetSlotOffset();

	if(bShow)
	{
		m_pSlotDlg->SetAcceptInputMsgWhenHide(bShow);
		m_pSlotDlg->SetElementDialogShowState(bShow);
	}
	else
	{
		m_pSlotDlg->SetAcceptInputMsgWhenHide(false);
		m_pSlotDlg->SetElementDialogShowState(false);
		Reset();
	}
}

void CDnCharTalismanDlg::ReleaseTalismanClickFlag()
{
	if(m_pSlotDlg)
		m_pSlotDlg->ReleaseTalismanClickFlag();
}

void CDnCharTalismanDlg::RefreshDetailInfoDlg()
{
	if( m_pDetailInfoDlg->IsShow() ) {
		m_pDetailInfoDlg->SetText();
		m_pDetailInfoDlg->Show(true);
	}
}

void CDnCharTalismanDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if( IsCmdControl("ID_BT_OK") )
		{
			// �������.
			//if(!m_bIsEditMode) {
			//	// mid: Ż������ �������� ��ġ�� ������ ��� �Ź� ������ %d��� %d�ǹ� %d���۸� �Ҹ��մϴ�.\n�̵� �� ��Ұ� �Ұ��� �ϹǷ� �Ҹ�� ���� �������� �� �����Ƿ� ������ ������ �ּ���.
			//	wchar_t  szString[256];
			//	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TPLAYERCOMMONLEVEL);
			//	if(!pSox) return;
			//	int nMyLevel = CDnActor::s_hLocalActor->GetLevel();
			//	int nNeedMoney = pSox->GetFieldFromLablePtr(nMyLevel, "_TalismanCost")->GetInteger();
			//	
			//	int nGold	= int(nNeedMoney/10000);
			//	int nSilver = int((nNeedMoney%10000)/100);
			//	int nCopper = int(nNeedMoney%100);

			//	wsprintf(szString, GetEtUIXML().GetUIString(CEtUIXML::idCategory1,8346), nGold, nSilver, nCopper);
			//	GetInterface().MessageBox(szString, MB_OK, E_MESSAGE_START_EDITMODE, this, false, true);
			//}
			//else {
			//	m_bIsEditMode = false;
			//	m_pChangeApplyBtn->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8353)); // mid: Ż������ ��ġ����
			//}
		}
		else if( IsCmdControl("ID_BT_INFO") )
		{
			// ����������
			if(m_pDetailInfoDlg->IsShow())
				m_pDetailInfoDlg->Show(false);
			else {
				m_pDetailInfoDlg->SetText();
				m_pDetailInfoDlg->Show(true);
			}
		}
		else if( IsCmdControl("ID_BT_COMPOSE") )
		{
			// �ռ���ư
		}
		else if( IsCmdControl("ID_CLOSE_DIALOG") )
		{
			// ����
			CDnCharStatusDlg* pCharStatusDlg = dynamic_cast<CDnCharStatusDlg*>( GetParentDialog() );
			if( pCharStatusDlg && pCharStatusDlg->GetCallBack() )
				pCharStatusDlg->GetCallBack()->OnUICallbackProc( pCharStatusDlg->GetDialogID(), EVENT_BUTTON_CLICKED, GetControl( "ID_CLOSE_DIALOG" ) );

			return;
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_TALISMAN);
		}
#endif
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);	
}

bool CDnCharTalismanDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsShow())
		return false;

	switch(uMsg)
	{
	case WM_MOUSEWHEEL:
		{
			if( IsMouseInDlg() )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int iScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;						

				if( m_pScrollBar->IsEnable() )
				{
					int nPos = m_pScrollBar->GetTrackPos() - iScrollAmount*10;
					m_pScrollBar->SetTrackPos(nPos);
				}
			}
		}
	}

	return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnCharTalismanDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process(fElapsedTime);

	if( !IsShow() )
		return;
}

void CDnCharTalismanDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if(IsCmdControl("ID_YES") || IsCmdControl("ID_OK"))
		{
			//switch(nID)
			//{
			//case E_MESSAGE_START_EDITMODE:
			//	{
			//		m_bIsEditMode ^= true;
			//		if(m_bIsEditMode)	m_pChangeApplyBtn->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8354)); // mid: ���泻�� ����
			//		else				m_pChangeApplyBtn->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8353)); // mid: Ż������ ��ġ����
			//	}
			//	break;
			//}
		}
	}
}

void CDnCharTalismanDlg::PlayTalismanMoveSound()
{
	if( m_nUseMoneySoundIndex != -1 )
		CEtSoundEngine::GetInstance().PlaySound( "2D", m_nUseMoneySoundIndex );

	wchar_t  szString[256];
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TPLAYERCOMMONLEVEL);
	if(!pSox) return;
	int nMyLevel   = CDnActor::s_hLocalActor->GetLevel();
	int nNeedMoney = pSox->GetFieldFromLablePtr(nMyLevel, "_TalismanCost")->GetInteger();

	int nGold	= int(nNeedMoney/10000);
	int nSilver = int((nNeedMoney%10000)/100);
	int nCopper = int(nNeedMoney%100);

	wsprintf(szString, GetEtUIXML().GetUIString(CEtUIXML::idCategory1,8350), nGold, nSilver, nCopper);
	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", szString, false);

	SetTalismanChangeText();
}

bool CDnCharTalismanDlg::CheckEmptySlot(std::wstring *strErrorMsg)
{
	if(!m_pSlotDlg)
		return false;

	bool bResult = false;
	//int  nCnt	 = m_pSlotDlg->GetEmptySlotCount();
	int nCnt = m_pSlotDlg->FindEmptySlotIndex();

	if(nCnt > -1)
		bResult = true;
	else
		if(strErrorMsg) *strErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8361);

	return bResult;
}

bool CDnCharTalismanDlg::CheckLevel(CDnItem* pItem, std::wstring *strErrorMsg)
{
	CDnLocalPlayerActor* pActor = (CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer();
	if(!pActor)
	{
		if(strErrorMsg) *strErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5061); // mid: ���� ���� ���� �����Դϴ�.
		return false;
	}

	int nPlayerLevel = pActor->GetLevel();
	int nItemLevel	 = pItem->GetLevelLimit();
	if( nPlayerLevel < nItemLevel )
	{
		if(strErrorMsg) *strErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 720); // mid: �ʿ䷹���� �����մϴ�.
		return false;
	}

	return true;
}

bool CDnCharTalismanDlg::CheckSameItemEquipped(CDnItem* pItem, std::wstring *strErrorMsg)
{
	if(!pItem) {
		if(strErrorMsg) *strErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5061); // mid: ���� ���� ���� �����Դϴ�.
		return false;
	}

	if(!m_pSlotDlg) return false;

	int nCurrentItemParam = pItem->GetTypeParam(1);
	if(!m_pSlotDlg->CheckParam(nCurrentItemParam))
	{
		if(strErrorMsg) *strErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8362); // mid: ���� ������ Ż�������� �����Ҽ� �����ϴ�.
		return false;
	}

	return true;
}

CDnCharStatusDlg::eRetWearable CDnCharTalismanDlg::ValidWearableTalismanItem(const MIInventoryItem *pItem, std::wstring *strErrorMsg)
{
	if( strErrorMsg ) strErrorMsg->clear();

	if( !pItem )
	{
		if( strErrorMsg ) *strErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5059 ); // �������� �����ϴ�.
		return CDnCharStatusDlg::eWEAR_UNABLE;
	}

	if( pItem->GetType() != MIInventoryItem::Item )
	{
		if( strErrorMsg ) *strErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 ); // ������ �� ���� �������Դϴ�.
		return CDnCharStatusDlg::eWEAR_UNABLE;
	}

	const CDnItem *pEquipItem = dynamic_cast<const CDnItem*>(pItem);
	if( !pEquipItem )
	{
		if( strErrorMsg ) *strErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 ); // ������ �� ���� �������Դϴ�.
		return CDnCharStatusDlg::eWEAR_UNABLE;
	}

	if (pEquipItem->IsSoulbBound() == false)
		return CDnCharStatusDlg::eWEAR_NEED_UNSEAL;

	switch( pEquipItem->GetItemType() )
	{
	case ITEMTYPE_PARTS:
	case ITEMTYPE_WEAPON:
		if( strErrorMsg ) *strErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 ); // �ش� ���Կ� ������ �Ұ����� �������Դϴ�.
		return CDnCharStatusDlg::eWEAR_UNABLE;
	}

	return CDnCharStatusDlg::eWEAR_ENABLE;
}

CDnCharStatusDlg::eRetWearable CDnCharTalismanDlg::GetWearableTalismanEquipType(CDnItem* pItem, std::wstring *strErrorMsg)
{
	CDnCharStatusDlg::eRetWearable eResult = ValidWearableTalismanItem(pItem, strErrorMsg);

	if( CDnCharStatusDlg::eWEAR_ENABLE == eResult )
	{ 
		if( !CheckLevel(pItem, strErrorMsg) )			return CDnCharStatusDlg::eWEAR_UNABLE;
		if( !CheckEmptySlot(strErrorMsg) )				return CDnCharStatusDlg::eWEAR_UNABLE;
		if( m_nCurrentSlotType != ST_TALISMAN ) 
			if(!CheckSameItemEquipped(pItem, strErrorMsg)) return CDnCharStatusDlg::eWEAR_UNABLE;
	}

	return eResult;
}

bool CDnCharTalismanDlg::RequestEquipTalismanItem( int nSlotIndex, MIInventoryItem* pInvenItem )
{
	// ���콺 ��Ŭ�� -> ������û
	CDnItem* pItem = dynamic_cast<CDnItem*>(pInvenItem);
	if(!pItem) return false;

	if(!m_pSlotDlg)
		return false;

	std::wstring strError;
	if( GetWearableTalismanEquipType( pItem, &strError ) != CDnCharStatusDlg::eWEAR_ENABLE )
	{
		GetInterface().MessageBox(strError.c_str(), MB_OK);
		return false;
	}
	
	int nEmptySlotIndex = m_pSlotDlg->FindEmptySlotIndex();
	return GetItemTask().RequestMoveItem( MoveType_InvenToTalisman, nSlotIndex, pItem->GetSerialID(), nEmptySlotIndex, pInvenItem->GetOverlapCount() );
}

void CDnCharTalismanDlg::SetTalismanItem(int nEquipIndex, MIInventoryItem *pItem)
{
	if(m_pSlotDlg)
		m_pSlotDlg->SetSlotItem(nEquipIndex, pItem);
}

void CDnCharTalismanDlg::RemoveTalismanItem(int nEquipIndex)
{
	if(m_pSlotDlg)
		m_pSlotDlg->RemoveSlotItem(nEquipIndex);
}

void CDnCharTalismanDlg::SetOpenTalismanSlot(int nSlotOpenFlag)
{
	if(m_pSlotDlg)
		m_pSlotDlg->SetOpenTalismanSlot(nSlotOpenFlag);
}

float CDnCharTalismanDlg::GetTalismanSlotRatio(int nSlotIndex)
{
	if(m_pSlotDlg == NULL)
		return .0f;

	return m_pSlotDlg->GetTalismanSlotRatio(nSlotIndex);
}

#endif // PRE_ADD_TALISMAN_SYSTEM