#include "Stdafx.h"

#if defined(PRE_ADD_MAINQUEST_UI)
#include "DnMainQuestDlg.h"
#include "DnMainQuestAnswerDlg.h"
#include "DnMainQuestRewardDlg.h"
#include "DnImageLeftBlindNpcDlg.h"
#include "DnImageRightBlindNpcDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnChatTabDlg.h"
#include "TaskManager.h"
#include "DnCommonTask.h"
#include "DnItemTask.h"
#include "DnMasterTask.h"
//#include "DnTooltipDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "DnInterface.h"
#include "NpcSendPacket.h"
#include "DnNpcTalkReturnDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMainQuestDlg::CDnMainQuestDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true)
, m_pAnswerDlg(NULL)
, m_pRewardDlg(NULL)
, m_pTextBoxMain(NULL)
, m_pStaticName(NULL)
, m_pLeftNpcImgDlg(NULL)
, m_pRightNpcImgDlg(NULL)
, m_pSelectButton(NULL)
, m_pBlackBG(NULL)
, m_pSlotBtn(NULL)
, m_pItemEffectTexCtrl(NULL)
, m_pItem(NULL)
, m_bIsOpenMainQuestDlg(false)
, m_SelectBtnType(QuestRenewal::SelectType::eType::Select)
{
}

CDnMainQuestDlg::~CDnMainQuestDlg(void)
{
	SAFE_DELETE(m_pRightNpcImgDlg);
	SAFE_DELETE(m_pLeftNpcImgDlg);
	SAFE_DELETE(m_pRewardDlg);
	SAFE_DELETE(m_pAnswerDlg);
}

void CDnMainQuestDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "NpcBTDlg.ui" ).c_str(), bShow );

	m_pAnswerDlg = new CDnMainQuestAnswerDlg(UI_TYPE_CHILD, this);
	m_pAnswerDlg->Initialize(false);

	m_pRewardDlg = new CDnMainQuestRewardDlg(UI_TYPE_CHILD, this);
	m_pRewardDlg->Initialize(false);

	m_pLeftNpcImgDlg = new CDnImageLeftBlindNpcDlg(UI_TYPE_CHILD, this);
	m_pLeftNpcImgDlg->Initialize(false);

	m_pRightNpcImgDlg = new CDnImageRightBlindNpcDlg(UI_TYPE_CHILD, this);
	m_pRightNpcImgDlg ->Initialize(false);
}

void CDnMainQuestDlg::InitialUpdate()
{
	m_pTextBoxMain  = GetControl<CEtUIHtmlTextBox>("ID_TEXTBOX_MAIN");
	m_pStaticName   = GetControl<CEtUIStatic>("ID_STATIC_NAME");
	m_pSelectButton = GetControl<CEtUIButton>("ID_BT_OK");
	m_pBlackBG		= GetControl<CEtUIStatic>("ID_STATIC0");
	m_pItemEffectTexCtrl = GetControl<CEtUITextureControl>("ID_BACK");
}

void CDnMainQuestDlg::InitCustomControl(CEtUIControl *pControl)
{
	if (pControl == NULL || !strstr(pControl->GetControlName(), "ID_SLOT_BUTTON"))
		return;

	m_pSlotBtn = static_cast<CDnItemSlotButton*>(pControl);
	m_pSlotBtn->SetSlotType(ST_QUEST); // Type를 추가해야할지 말지는 나중에 체크
	m_pSlotBtn->SetSlotIndex(0);
}

bool CDnMainQuestDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsShow()) 
		return false;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			if(wParam == VK_ESCAPE)
			{
				End();
				return false;
			}
			else if(wParam == VK_SPACE)
			{
				// 선택지가 안나와 있으면, 선택버튼 클릭이 되도록
				if( m_pAnswerDlg->IsShowAnswerTextBox() == false ) {
					ProcessCommand(EVENT_BUTTON_CLICKED, true, m_pSelectButton, uMsg);
					return false;
				}
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMainQuestDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if(IsCmdControl("ID_BT_OK"))
		{
			if(m_pRewardDlg && m_pRewardDlg->IsShow())
			{
				switch(m_SelectBtnType)
				{
				case QuestRenewal::SelectType::eType::Complete: // 보상
					{
						TakeRecompenseItem();
					}
					break;

				case QuestRenewal::SelectType::eType::Return: // 돌아가기
					{
						End();
						CDnNpcTalkReturnDlg* pNpcTalkReturnDlg = GetInterface().GetNpcTalkReturnDlg();
						if(pNpcTalkReturnDlg) 
						{
							pNpcTalkReturnDlg->ProcessCommand(nCommand, true, pNpcTalkReturnDlg->GetControl("ID_BUTTON_RETURN"), uMsg);
							CDnCommonTask*  pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
							if(pCommonTask) pCommonTask->SetNpcTalkCamera(CDnNpcTalkCamera::WorkState::BeginTalk);
						}
					}
					break;
				}
			}
			//rlkt_test
			//GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", L"ID_BT_OK가 눌렸습니다");
		}
		else if(IsCmdControl("ID_BT_CLOSE")) // 닫기(ESC)
		{
			End();
			return;
		}

		if( m_pCallback )
			m_pCallback->OnUICallbackProc( GetDialogID(), EVENT_BUTTON_CLICKED, pControl);
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMainQuestDlg::TakeRecompenseItem()
{
	if( m_pRewardDlg->IsShow() )
	{
		if( m_pRewardDlg->IsRecompense() )
		{
			if( !m_pRewardDlg->IsCompleteCheck() )
			{
				int nCnt = m_pRewardDlg->GetAvailableCount();
				std::wstring strMsg;
				strMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1199999 ), nCnt );

				GetInterface().MessageBox( strMsg.c_str(), MB_OK );
				return;
			}

			bool bItemArray[QUESTREWARD_INVENTORYITEMMAX] = {0,};
			bool bCashItemArray[MAILATTACHITEMMAX] = {0,};
			m_pRewardDlg->GetRecompenseItemArray( bItemArray, bCashItemArray );

			int nItemCount = 0, nCashItemCount = 0;
			for (int nIndex = 0 ; _countof(bItemArray) > nIndex ; ++nIndex) {
				if (!bItemArray[nIndex]) {
					continue;
				}
				++nItemCount;
			}
			for (int nIndex = 0 ; _countof(bCashItemArray) > nIndex ; ++nIndex) {
				if (!bCashItemArray[nIndex]) {
					continue;
				}
				++nCashItemCount;
			}

			if( ( (0 < nItemCount) || ( (0 < nCashItemCount) && (nItemCount != 0) ) ) )
			{
				CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
				if( pInvenDlg ) 
				{
					if( pInvenDlg->GetEmptySlotCount() < nItemCount )
					{
						GetInterface().MessageBox( 1925, MB_OK );
						return;
					}
				}
			}
			SendSelectQuestReward( m_pRewardDlg->GetRecompenseID(), bItemArray, bCashItemArray );
		}

		ShowChildDialog( m_pRewardDlg, false );
	}
}

void CDnMainQuestDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnMainQuestDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnMainQuestDlg::Start()
{
	SetOpenMainQuestDlgFlag(true);

	CDnLocalPlayerActor::LockInput(true);
	GetMasterTask().CloseAcceptDlg();
	GetInterface().CloseNpcDialog();
	GetInterface().ShowMiniMap(false);
	
	CDnChatTabDlg*  pChatTabDlg = GetInterface().GetChatTabDialog();
	if(pChatTabDlg) pChatTabDlg->ShowEx(false);
	
	CEtUIDialogBase::CloseAllDialog();
	CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
	if(pLocalActor) pLocalActor->SetHideNpc(true, 1.0f);
	CDnLocalPlayerActor::HideAnotherPlayers(true, 1.0f);
	
	Show(true);
	ShowChildDialog(m_pAnswerDlg, true);
}

void CDnMainQuestDlg::End()
{
//	CDnTooltipDlg *pDialog = GetInterface().GetTooltipDialog();
//	if(pDialog) pDialog->HideMainQuestTooltip();

	SetOpenMainQuestDlgFlag(false);
	CDnLocalPlayerActor::LockInput(false);
	GetInterface().OpenBaseDialog();

	CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
	if(pCommonTask) pCommonTask->SetNpcTalkCamera(CDnNpcTalkCamera::WorkState::EndTalk);
	
	CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
	if(pLocalActor) pLocalActor->SetHideNpc(false, 1.0f);
	
	CDnLocalPlayerActor::HideAnotherPlayers(false, 1.0f);
	
	Show(false);
	ShowChildDialog(m_pAnswerDlg, false);
	ShowChildDialog(m_pRewardDlg, false);
}

void CDnMainQuestDlg::SetMainQuestParagraph(TALK_PARAGRAPH talk_para)
{
	SetNpcTalk(talk_para.Question.szQuestion.c_str());
	SetNpcPortrait(talk_para.szLeftPortraitName, talk_para.szRightPortraitName);
	bool bIsShowSlotButton = SetSlotItem(talk_para.nItemIndex, talk_para.szImageName);

	// 버튼 Text 변경
	if(talk_para.Answers.empty() == false) {
		SetButtonText(talk_para.Answers[0].cSelectType);
	}
	
	if(m_pAnswerDlg)
	{
		m_pAnswerDlg->SetAnswer(talk_para.Answers);
		if(bIsShowSlotButton == false)// 슬롯이 안떠있으면 배경 이미지를 그리게 한다
			m_pAnswerDlg->SetMainBGImage(talk_para.szImageName);
	}	
}

bool CDnMainQuestDlg::SetSlotItem(int nItemIndex, const std::string strImgName)
{
	bool bResult = true;

	if(nItemIndex > 0)
	{
		TItemInfo itemInfo;
		CDnItem::MakeItemInfo(nItemIndex, 1, itemInfo);
		SAFE_DELETE(m_pItem);
		m_pItem = GetItemTask().CreateItem(itemInfo);
		if(m_pSlotBtn && m_pItem)
		{
			m_pSlotBtn->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);												
			m_pSlotBtn->Show(true);			

			/*
			SUICoord SlotBtnCoord = m_pSlotBtn->GetUICoord();
			float x = SlotBtnCoord.fX + SlotBtnCoord.fWidth;
			float y = SlotBtnCoord.fY;
			CDnTooltipDlg*  pToolTipDlg = GetInterface().GetTooltipDialog();
			if(pToolTipDlg) pToolTipDlg->ShowMainQuestTooltip(m_pSlotBtn, x, y);
			*/
		}
		else
		{
			if(m_pSlotBtn)  m_pSlotBtn->Show(false);
			/*CDnTooltipDlg*  pToolTipDlg = GetInterface().GetTooltipDialog();
			if(pToolTipDlg) pToolTipDlg->HideMainQuestTooltip();*/
			bResult = false;
		}
	}
	else
	{
		if(m_pSlotBtn)  m_pSlotBtn->Show(false);
		/*CDnTooltipDlg*  pToolTipDlg = GetInterface().GetTooltipDialog();
		if(pToolTipDlg) pToolTipDlg->HideMainQuestTooltip();*/
		bResult = false;
	}
	

	if(nItemIndex > 0 && strImgName.empty() == false)
	{
		SAFE_RELEASE_SPTR(m_hItemEffectBGImage);
		m_hItemEffectBGImage = LoadResource(CEtResourceMng::GetInstance().GetFullName(strImgName.c_str() ).c_str(), RT_TEXTURE);
		if(m_hItemEffectBGImage)
		{
			int W = m_hItemEffectBGImage->Width();
			int H = m_hItemEffectBGImage->Height();
			m_pItemEffectTexCtrl->SetTexture(m_hItemEffectBGImage, 0, 0, W, H);
			m_pItemEffectTexCtrl->SetManualControlColor( true );
			m_pItemEffectTexCtrl->SetTextureColor( 0xAAFFFFFF );
			m_pItemEffectTexCtrl->Show( true );
		}
		else 
		{
			SAFE_RELEASE_SPTR(m_hItemEffectBGImage);
			m_pItemEffectTexCtrl->Show( false );
			bResult = false;
		}
	}
	else
	{
		SAFE_RELEASE_SPTR(m_hItemEffectBGImage);
		m_pItemEffectTexCtrl->Show(false);
		bResult = false;
	}

	return bResult;
}

void CDnMainQuestDlg::SetButtonText(BYTE cSelectType)
{
	//					{ 계속 / 수락 / 완료/ 대화종료 / 돌아가기 }
	int nBtnMidArray[] = { 68, 1413, 7117, 8432, 105 };
	int nIndex = cSelectType - 2;
	if( nIndex >= 0 || nIndex < (sizeof(nBtnMidArray) / sizeof(int)) )
		m_pSelectButton->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1 , nBtnMidArray[nIndex]) );

	m_SelectBtnType = static_cast<QuestRenewal::SelectType::eType>(cSelectType);
	m_pSelectButton->Focus(true);
}

void CDnMainQuestDlg::SetNpcTalk( LPCWSTR wszNpcTalk )
{
	ASSERT(wszNpcTalk);
	ASSERT(m_pTextBoxMain);

	//m_pTextBoxMain->SetUICoord(m_MainTextBoxDummy);
	//m_pTextBoxAnswer->SetUICoord(m_AnswerTextBoxDummy);

	m_pStaticName->ClearText();
	m_pTextBoxMain->ClearText();

	std::wstring strNpcTalk(wszNpcTalk);
	std::wstring::size_type index_s = strNpcTalk.find_first_of( L"$" );
	std::wstring::size_type index_e = strNpcTalk.find_last_of( L"$" );

	if( index_s == std::wstring::npos )
	{
		m_pTextBoxMain->ReadHtmlString( strNpcTalk.c_str() );
	}
	else
	{
		std::wstring strNpcName = strNpcTalk.substr( index_s+1, index_e-index_s-1 );
		std::wstring strNpcText =strNpcTalk.erase( index_s, index_e-index_s+1 );

		m_pStaticName->SetText( strNpcName );
		m_pTextBoxMain->ReadHtmlString( strNpcText.c_str() );
	}	
}

void CDnMainQuestDlg::SetRecompense( const TQuestRecompense &questRecompense, const bool bRecompense )
{
	if( !m_pRewardDlg ) 
		return;

	m_pRewardDlg->SetRecompense( questRecompense, bRecompense );

	SetLevelCapRecompense( questRecompense );
	ShowChildDialog( m_pRewardDlg, true );
}

void CDnMainQuestDlg::SetLevelCapRecompense( const TQuestRecompense & questRecompese )
{
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor * pLocalActor = dynamic_cast<CDnLocalPlayerActor *>( CDnActor::s_hLocalActor.GetPointer() );
	if( !pLocalActor )
		return;

	if( pLocalActor->GetLevel() < (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PlayerLevelLimit) )
		return;

	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( questRecompese.nQuestIndex );
	if( NULL == pQuestInfo )
		return;

	if( false == pQuestInfo->bLevelCapReward )
		return;

	std::vector<int> vlJobHistory;
	pLocalActor->GetJobHistory( vlJobHistory );
	int iFirstJob = (int)vlJobHistory.front();

	TQuestLevelCapRecompense questLevelCapRecompense;
	if( false == g_DataManager.GetQuestLevelCapRecompense( pQuestInfo->cQuestType, iFirstJob, questLevelCapRecompense ) )
		return;

	m_pRewardDlg->SetLevelCapRecompense( questRecompese, questLevelCapRecompense );
}

void CDnMainQuestDlg::SetNpcPortrait(const std::string strLeft, const std::string strRight)
{
	//GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", L"양옆에 포트레이트는 테스트용으로. 강제로 클라에서 띄우고 있습니다.");
	//std::string aa = "DwcElementallord.dds";
	//std::string bb = "DwcElementallord.dds";

	if(strLeft.empty() == false)
	{
		m_pLeftNpcImgDlg->SetNpcTexture(NULL, strLeft.c_str(), false);		
		m_pLeftNpcImgDlg->Show(true);
	}
	else
		m_pLeftNpcImgDlg->Show(false);

	if( strRight.empty() == false )
	{
		m_pRightNpcImgDlg->SetNpcTexture(NULL, strRight.c_str(), true);
		m_pRightNpcImgDlg->Show(true);
	}
	else
		m_pRightNpcImgDlg->Show(false);
}

bool CDnMainQuestDlg::OnAnswer(int nNpcID)
{
	if(m_pAnswerDlg) 
		return m_pAnswerDlg->OnAnswer(nNpcID);

	return false;
}

bool CDnMainQuestDlg::GetAnswerIndex(OUT std::wstring& szIndex, OUT std::wstring& szTarget, int nCommand, bool bClearTextBoxSelect) const
{
	if(m_pAnswerDlg)
		return m_pAnswerDlg->GetAnswerIndex(szIndex, szTarget, nCommand, bClearTextBoxSelect);

	return false;
}

void CDnMainQuestDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();

	SUICoord CurrentCoord;
	CurrentCoord.fX = -GetScreenWidthBorderSize();
	CurrentCoord.fY = -GetScreenHeightBorderSize();
	CurrentCoord.fWidth = GetScreenWidthRatio();
	CurrentCoord.fHeight = GetScreenHeightRatio();

	if(m_pBlackBG) 
		m_pBlackBG->SetUICoord(CurrentCoord);
}
#endif // PRE_ADD_MAINQUEST_UI