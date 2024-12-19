#include "StdAfx.h"

#if defined(PRE_ADD_TALISMAN_SYSTEM)
#include "DnCharTalismanListDlg.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnFadeInOutDlg.h"
#include "DnTooltipTalismanDlg.h"
#include "DnCharTalismanDlg.h"
#include "DnLocalPlayerActor.h"

#include "TaskManager.h"

CDnCharTalismanListDlg::CDnCharTalismanListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
, m_pRemoveItem(NULL)
, m_pToolTipDlg(NULL)
, m_pParentDlg(NULL)
, m_pStaticControl(NULL)
, m_bIsTalismanToTalisman(false)
, m_bIsEditMode(false)
{
}

CDnCharTalismanListDlg::~CDnCharTalismanListDlg()
{
	SAFE_DELETE(m_pToolTipDlg);
}

void CDnCharTalismanListDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharTalismanListDlg.ui" ).c_str(), bShow );

	SetElementDialog( true );
}

void CDnCharTalismanListDlg::InitialUpdate()
{
	MakeSlotData();

	m_pToolTipDlg = new CDnTooltipTalismanDlg(UI_TYPE_MOST_TOP);
	m_pToolTipDlg->Initialize(false);

	m_pParentDlg = static_cast<CDnCharTalismanDlg*>(this->GetParentDialog());
}

void CDnCharTalismanListDlg::MakeSlotData()
{
	m_vItemSlotBtnList.reserve(TALISMAN_MAX);
	m_vItemSlotBtnList.clear();

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TTALISMANSLOT);
	if(!pSox) return;

	sTalismanSlotData D;
	for(int i = 0 ; i < pSox->GetItemCount() ; ++i)
	{		
		int nID = pSox->GetItemID(i);

		D.nSlotIndex		= nID;
		D.bIsServiceSlot	= pSox->GetFieldFromLablePtr(nID, "_Service")->GetBool();
		D.nSlotOpenLevel	= pSox->GetFieldFromLablePtr(nID, "_Level")->GetInteger();
		D.nSlotOpenAmount	= pSox->GetFieldFromLablePtr(nID, "_Amount")->GetInteger();
		D.fEfficiency		= pSox->GetFieldFromLablePtr(nID, "_Efficiency")->GetFloat();
		D.nToolTipStringMid = pSox->GetFieldFromLablePtr(nID, "_Description")->GetInteger();
		D.nSlotType			= (eTalismanSlotType)pSox->GetFieldFromLablePtr(nID, "_Type")->GetInteger();
		D.nSlotOpenNeedItemID		 = pSox->GetFieldFromLablePtr(nID, "_item")->GetInteger();
		D.strSlotHoldTextureFileName = pSox->GetFieldFromLablePtr(nID, "_Texture")->GetString();		
		D.bIsOpenSlot		= (D.nSlotType == eTalismanSlotType::TALISMAN_BASIC);

		// UI Controls
		bool bIsShow = (D.bIsServiceSlot == true);

		int nUIIndex = nID-1;
		D.pBackImage = GetControl<CEtUIStatic>( FormatA("ID_STATIC_SLOT%d", nUIIndex).c_str() );
		D.pBackImage->Show(true);

		D.pSlotButton = GetControl<CDnItemSlotButton>( FormatA("ID_BT_ITEM%d", nUIIndex).c_str() );
		D.pSlotButton->SetSlotIndex(i);
		D.pSlotButton->SetSlotType(ST_TALISMAN);
		D.pSlotButton->Show(true);

		D.pPercentText = GetControl<CEtUIStatic>( FormatA("ID_TEXT_OPTION%d", nUIIndex).c_str() );

		std::wstring wszString = FormatW(L"%.f%%", D.fEfficiency*100.0f);
		D.pPercentText->SetText(wszString.c_str(), false, .5f, .5f);
		D.pPercentText->Show(false);
		D.pPercentText->Enable(false);

		D.pCoverStatic = GetControl<CEtUIStatic>( FormatA("ID_STATIC_COVER%d", nUIIndex).c_str() );
		D.pCoverStatic->Show( (D.nSlotType == TALISMAN_EXPANSION) );
		//D.pCoverStatic->Enable( D.bIsServiceSlot );
		D.pCoverStatic->Enable( true );
		D.pCoverStatic->SetID(i);

		D.pCashCoverStatic = GetControl<CEtUIStatic>( FormatA("ID_STATIC_CASHCOVER%d", nUIIndex).c_str() );
		D.pCashCoverStatic->Show( (D.nSlotType == TALISMAN_CASH_EXPANSION) );
		D.pCashCoverStatic->Enable( D.pCashCoverStatic->IsShow() );
		D.pCashCoverStatic->SetID(i);

		m_vItemSlotBtnList.push_back(D);
	}

	for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
	{
		// 슬롯을 UI맨앞으로 이동
		if(m_vItemSlotBtnList[i].bIsOpenSlot)
		{
			MoveToTail(m_vItemSlotBtnList[i].pSlotButton);
		}
	}
}

void CDnCharTalismanListDlg::Process( float fElapsedTime )
{
	CDnItemTask* pItemTask = static_cast<CDnItemTask*>(CTaskManager::GetInstance().GetTask("ItemTask"));
	if(!pItemTask) return;
	CDnItemTask::SCashTalisman& sCashTalismanSlotData = pItemTask->GetCashTalismanSlot();
	if(sCashTalismanSlotData.bRecvExpirePacket)
		RefreshTalismanCashSlot();

	CDnCustomDlg::Process(fElapsedTime);
}

void CDnCharTalismanListDlg::SetWeableSlotBlank()
{
	if(m_vItemSlotBtnList.empty()) return;
	if(!m_pParentDlg) return;

	if(!CDnActor::s_hLocalActor)
		return;

	CDnLocalPlayerActor* pActor = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if(!pActor)
		return;

	if( drag::IsValid() )
	{
		CDnSlotButton *pDragButton = (CDnSlotButton*)drag::GetControl();
		switch( pDragButton->GetSlotType() ) 
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
		CDnCharStatusDlg::eRetWearable eResult = m_pParentDlg->ValidWearableTalismanItem(pItem, &wszErrorMessage);
		if( CDnCharStatusDlg::eWEAR_ENABLE != eResult )			 return;
		if( !m_pParentDlg->CheckLevel(pItem, &wszErrorMessage) ) return;
		if( !m_pParentDlg->CheckEmptySlot(&wszErrorMessage) )	 return;

		switch( pItem->GetItemType() )
		{
		case ITEMTYPE_TALISMAN:
			{
				for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
				{
					if(!m_vItemSlotBtnList[i].bIsOpenSlot)
						continue;

					if(m_vItemSlotBtnList[i].nSlotOpenLevel > pActor->GetLevel())
						continue;

					if(m_vItemSlotBtnList[i].pSlotButton->IsEmptySlot())
						m_vItemSlotBtnList[i].pSlotButton->SetWearable(true);
				}
			}
			break;
		}
	}
	else
	{
		for( int i=0; i<(int)m_vItemSlotBtnList.size(); i++ )
			m_vItemSlotBtnList[i].pSlotButton->SetWearable( false );
	}
}

void CDnCharTalismanListDlg::RefreshTalismanCashSlot()
{
	if(m_vItemSlotBtnList.empty())
		return;

	if(!CDnActor::s_hLocalActor)
		return;

	CDnLocalPlayerActor* pPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if(!pPlayer)
		return;

	CDnItemTask* pItemTask = static_cast<CDnItemTask*>(CTaskManager::GetInstance().GetTask("ItemTask"));
	if(!pItemTask)
		return;

	CDnItemTask::SCashTalisman& sCashTalismanSlotData = pItemTask->GetCashTalismanSlot();
	if(sCashTalismanSlotData.bRecvExpirePacket)
	{
		sCashTalismanSlotData.bRecvExpirePacket = false;

		for(int i = 0 ; i < TALISMAN_CASH_MAX+1 ; ++i)
		{
			bool bIsOpen = sCashTalismanSlotData.bOpenSlot;
			m_vItemSlotBtnList[i].bIsOpenSlot = bIsOpen;

			if(bIsOpen)
			{
				m_vItemSlotBtnList[i].pCashCoverStatic->Show(false);
				m_vItemSlotBtnList[i].pCashCoverStatic->Enable(false);
				m_vItemSlotBtnList[i].pSlotButton->SetIconColor(itemcolor::NORMAL);
				MoveToTail(m_vItemSlotBtnList[i].pSlotButton);

				if(pPlayer) {
					//if( MIInventoryItem* pInvenItem = m_vItemSlotBtnList[i].pSlotButton->GetItem() ) 
					{
						// 막혔던 슬롯에 아이템이 들어가있으면, 다시 장착시켜준다.
						CDnItem* pTalisman = GetItemTask().GetTalismanItem(i);
						//if( pTalisman && pTalisman->GetSerialID() == ((CDnItem*)pInvenItem)->GetSerialID() ) 
						if(pTalisman)
						{
							DnTalismanHandle hTalisman = ((CDnTalisman*)pTalisman)->GetMySmartPtr();
							float fRatio = GetItemTask().GetTalismanSlotRatio(i);
							pPlayer->AttachTalisman(hTalisman, i, fRatio);
						}
					}
				}
			}
			else
			{
				// 만기된 슬롯인데, 아이템이 박혀있다면...
				//MIInventoryItem* pCurrentItem = m_vItemSlotBtnList[i].pSlotButton->GetItem();
				CDnItem* pTalismanItem = pItemTask->GetTalismanItem(i);
				
				bool bHaveItem = true;
				if(!pTalismanItem) bHaveItem = false;
				
				if(bHaveItem) {
					m_vItemSlotBtnList[i].pCashCoverStatic->Show(false);
					m_vItemSlotBtnList[i].pCashCoverStatic->Enable(false);
					m_vItemSlotBtnList[i].pSlotButton->SetIconColor(itemcolor::EXPIRE);
					MoveToTail(m_vItemSlotBtnList[i].pSlotButton);
				}
				else {
					m_vItemSlotBtnList[i].pCashCoverStatic->Show(true);
					m_vItemSlotBtnList[i].pCashCoverStatic->Enable(true);
					MoveToTail(m_vItemSlotBtnList[i].pCashCoverStatic);
				}

				if(pPlayer) {
					// ItemTask에 있는 탈리스만은 UI에서 사용하므로, 탈착시키지 않는다.
					// 능력치가 반영되지 않도록 Actor에 있는 탈리스만은 벗긴다.
					pPlayer->DetachTalisman(i);
				}
			}
		} // end of for
		pPlayer->RefreshState();
	} // end of if
}

void CDnCharTalismanListDlg::RefreshTalismanSlot()
{
	if(m_vItemSlotBtnList.empty())
		return;
	
	for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
	{
		/*if(!m_vItemSlotBtnList[i].bIsServiceSlot) continue;*/

		m_vItemSlotBtnList[i].pBackImage->Show(true);
		m_vItemSlotBtnList[i].pSlotButton->Show(true);
		
		m_vItemSlotBtnList[i].pPercentText->Show(m_bIsEditMode);
		m_vItemSlotBtnList[i].pPercentText->Enable(m_bIsEditMode);
				
		if(m_vItemSlotBtnList[i].nSlotType != eTalismanSlotType::TALISMAN_BASIC)
		{			
			if(m_vItemSlotBtnList[i].nSlotType == eTalismanSlotType::TALISMAN_CASH_EXPANSION)
			{
				bool bIsOpen = GetItemTask().IsOpenCashTalismanSlot();
				if( GetItemTask().GetTalismanItem(i) ) 
				{
					m_vItemSlotBtnList[i].pCashCoverStatic->Show(false);
					
					if(bIsOpen)
						m_vItemSlotBtnList[i].pSlotButton->SetIconColor(itemcolor::NORMAL);
					else
						m_vItemSlotBtnList[i].pSlotButton->SetIconColor(itemcolor::EXPIRE);
				}
				else 
				{
					m_vItemSlotBtnList[i].pCashCoverStatic->Show(!bIsOpen);
					m_vItemSlotBtnList[i].pCashCoverStatic->Enable(!bIsOpen);
				}
			}
			else
			{
				// 일반슬롯 처리
				bool bIsOpen = m_vItemSlotBtnList[i].bIsOpenSlot;
				m_vItemSlotBtnList[i].pCoverStatic->Show(!bIsOpen);
			}
		}
	}
}

void CDnCharTalismanListDlg::SetOpenTalismanSlot(int nOpenSlotFlag)
{
	for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
	{
		if( GetBitFlag( (char*)&nOpenSlotFlag , i ) == true ) {
			int nIndex = TALISMAN_EXPANSION_START+i;
			if(nIndex < 0 || nIndex >= (int)m_vItemSlotBtnList.size())
				continue;
			
			m_vItemSlotBtnList[nIndex].bIsOpenSlot = true;
			MoveToTail(m_vItemSlotBtnList[nIndex].pSlotButton);
		}
	}
}

void CDnCharTalismanListDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	if(bShow)
		RefreshTalismanSlot();

	CDnCustomDlg::Show(bShow);
}

void CDnCharTalismanListDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	//SetWeableSlotBlank();
}

void CDnCharTalismanListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());
	
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr(pControl->GetControlName(), "ID_BT_ITEM") )
		{
			if( GetItemTask().IsRequestWait() ) return;
			if( GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd) ) return;
			
			//ShowTalismanSlotEfficiency(false); // 퍼센트율 Off

			CDnSlotButton*		pDragButton		= (CDnSlotButton*)drag::GetControl();
			CDnItemSlotButton*	pPressedButton  = (CDnItemSlotButton*)pControl;		
			
			if(pDragButton == NULL)
			{
				if(!pPressedButton->GetItem())
					return;
				
				if( !GetItemTask().CheckRequestMoveItem(true) ) 
					return;

				CDnItem* pItem = (CDnItem*)pPressedButton->GetItem();
				if(uMsg == WM_RBUTTONUP && pPressedButton->GetItem())
				{
					GetItemTask().RequestMoveItem(	MoveType_TalismanToInven, 
													pItem->GetSlotIndex(),
													pItem->GetSerialID(),	
													GetItemTask().GetCharInventory().FindFirstEmptyIndex(),
													pItem->GetOverlapCount() );
					return;
				}			

				if(uMsg == WM_LBUTTONDOWN && pDragButton == NULL)
				{
					// 탈리스만 아이템 드래그 시작
					drag::SetControl(pPressedButton);
					m_bIsTalismanToTalisman = true;
					m_bIsEditMode = true;
					return;
				}
				
				// todo by kalliste : refactoring with DnInvenSlotDlg.cpp
				if(uMsg & VK_SHIFT)
				{
					if (GetInterface().SetNameLinkChat(*pItem))
						return;
				}
			}
			else
			{
				drag::ReleaseControl();
				pDragButton->DisableSplitMode(true);
				m_bIsEditMode = false;

				CDnItem* pSelectedItem = (CDnItem*)pDragButton->GetItem();
				if(pSelectedItem == pPressedButton->GetItem())
					return; // 같은 아이템이면 스왑안함.

				int nIndex = pPressedButton->GetSlotIndex();
				if( nIndex > 0 || nIndex <= (int)m_vItemSlotBtnList.size() )
				{
					if(!m_vItemSlotBtnList[nIndex].bIsOpenSlot)
						return; // 안뚫린 슬롯에 셋팅 방지
				}

				if( pDragButton->GetSlotIndex() < TALISMAN_BASIC_START && !m_vItemSlotBtnList[pDragButton->GetSlotIndex()].bIsOpenSlot )
				{
					if( pPressedButton->GetItem() )
					{
						// 만료슬롯에 있던 탈리스만은, 빈슬롯으로만 들어갈수있다.
						GetInterface().MessageBox(1971); // 임시mid: 빈슬롯이 없어서 아이템을 첨부할수 없습니다. 
						return;
					}
				}

				if(pSelectedItem)
				{			
					if(m_bIsTalismanToTalisman) // 탈리스만 -> 탈리스만
					{
						GetItemTask().RequestMoveItem(	MoveType_Talisman, 
														pSelectedItem->GetSlotIndex(), pSelectedItem->GetSerialID(), 
														pPressedButton->GetSlotIndex(), pSelectedItem->GetOverlapCount() );
						m_bIsTalismanToTalisman = false;
					}
					else
					{
						GetItemTask().RequestMoveItem(	MoveType_InvenToTalisman, 
														pSelectedItem->GetSlotIndex(), pSelectedItem->GetSerialID(), 
														pPressedButton->GetSlotIndex(), pSelectedItem->GetOverlapCount() );
					}					
				}
			}
		}
		else if(strstr(pControl->GetControlName(), "ID_STATIC_COVER"))	// 일반 커버
		{
			if(uMsg == WM_RBUTTONUP)
			{
				// 안뚫린 슬롯 오픈 제한
				m_pStaticControl = (CEtUIStatic*)pControl;
				if(m_pStaticControl && !m_vItemSlotBtnList.empty())
				{
					for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
					{
						if(m_vItemSlotBtnList[i].pCoverStatic->GetID() == m_pStaticControl->GetID() )
						{
							if(m_vItemSlotBtnList[i].bIsServiceSlot == false)
								return;
						}
					}
				}
				
				if(m_pStaticControl)
				{
					// mid: 슬롯을 확성화 하시겠습니까?
					GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8360), MB_YESNO, 8360, this, false, true );
				}
			}
		}
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnCharTalismanListDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(m_pParentDlg) {
		if(m_pParentDlg && false == m_pParentDlg->IsMouseInDlg())
			return false;
	}

	SUICoord uicoord;
	CEtUIListBoxEx* pListBox = m_pParentDlg->GetListBoxEX();
	if(pListBox) pListBox->GetUICoord(uicoord);

	POINT MousePoint;
	float fMouseX, fMouseY;
	MousePoint.x = short( LOWORD( lParam ) );
	MousePoint.y = short( HIWORD( lParam ) );
	PointToFloat( MousePoint, fMouseX, fMouseY );
	float fScreenHeight = uicoord.fHeight / GetScreenHeightRatio() * GetEtDevice()->Height(); //
	float fScreenMouseY = fMouseY / GetScreenHeightRatio() * GetEtDevice()->Height();
	
	CEtUIScrollBar* pScrollBar = m_pParentDlg->GetScrollBar();
	int nCurrentPos = 0;
	if(pScrollBar) nCurrentPos =  pScrollBar->GetTrackPos();
	float fNowYOffsetRatio  = (float)nCurrentPos / fScreenHeight;
	fNowYOffsetRatio = fNowYOffsetRatio / GetScreenHeightRatio() * GetEtDevice()->Height();
	fScreenHeight += fNowYOffsetRatio;

	if( fScreenHeight > fScreenMouseY )
	{
		switch( uMsg )
		{
		case WM_RBUTTONUP:
			{	
				POINT MousePoint;
				float fMouseX, fMouseY;
				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );
				PointToFloat( MousePoint, fMouseX, fMouseY );

				CEtUIControl* pControl = GetControlAtPoint( fMouseX, fMouseY );
				if( pControl && pControl->IsShow() )
				{
					if(pControl->IsEnable())
						this->ProcessCommand( EVENT_BUTTON_CLICKED, true, pControl, uMsg );
				}
				return true;
			}
			break;

		case WM_MOUSEMOVE:
			{		
				CDnSlotButton *pDragButton = (CDnSlotButton*)drag::GetControl();
				if( pDragButton == NULL )
				{				
					for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i )
					{
						/*if(!m_vItemSlotBtnList[i].bIsServiceSlot)
							continue;*/

						//if(m_vItemSlotBtnList[i].nSlotType == eTalismanSlotType::TALISMAN_CASH_EXPANSION)
						//	continue; // 우선 캐시 슬롯은 툴팁은 뺴둔다.

						if( false == m_vItemSlotBtnList[i].bIsOpenSlot )
						{
							SUICoord uiCoord;
							if(m_vItemSlotBtnList[i].nSlotType == eTalismanSlotType::TALISMAN_CASH_EXPANSION) 
							{
								if(!m_vItemSlotBtnList[i].pCashCoverStatic->IsShow()) 
									continue;

								m_vItemSlotBtnList[i].pCashCoverStatic->GetUICoord( uiCoord );
							}
							else if(m_vItemSlotBtnList[i].nSlotType == eTalismanSlotType::TALISMAN_EXPANSION) 
							{
								if(!m_vItemSlotBtnList[i].pCoverStatic->IsShow()) 
									continue;

								m_vItemSlotBtnList[i].pCoverStatic->GetUICoord( uiCoord );
							}

							POINT	 MousePoint;
							float	 fMouseX,fMouseY;
							MousePoint.x = short(LOWORD( lParam ));
							MousePoint.y = short(HIWORD( lParam ));
							PointToFloat( MousePoint, fMouseX, fMouseY );
							GetScreenMouseMovePoints( fMouseX, fMouseY );
							fMouseX -= GetXCoord();
							fMouseY -= GetYCoord();

							if( uiCoord.IsInside( fMouseX, fMouseY ) )
							{
								// 1. 캐시(Description만 나오도록)
								// 2. _Service가 true인 경우 ( 툴팁내용이 전부 나오도록 )
								// 3. _Service가 false인 경우 ( Description만 나오도록 )
								bool bIsCash = ( (m_vItemSlotBtnList[i].nSlotType == eTalismanSlotType::TALISMAN_CASH_EXPANSION) || !m_vItemSlotBtnList[i].bIsServiceSlot );
								m_pToolTipDlg->SetTalismanSlot(m_vItemSlotBtnList[i].nSlotIndex, bIsCash);
								GetScreenMouseMovePoints( fMouseX, fMouseY );
								m_pToolTipDlg->SetPosition( fMouseX, fMouseY );
								SUICoord DlgCoord;
								m_pToolTipDlg->GetDlgCoord(DlgCoord);
								static float fXRGap(4.0f/DEFAULT_UI_SCREEN_WIDTH);
								if( (DlgCoord.Right()+fXRGap) > GetScreenWidthRatio() ) DlgCoord.fX -= (DlgCoord.Right()+fXRGap - GetScreenWidthRatio());
								m_pToolTipDlg->SetDlgCoord(DlgCoord);
								m_pToolTipDlg->Show( true );
								break;
							}
							else
								m_pToolTipDlg->Show( false );
						}
					}
				}
			}
			break;
		}
	} // end of bool
	else
	{
		m_pToolTipDlg->Show(false);
		return false;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnCharTalismanListDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if (IsCmdControl("ID_YES"))
		{
			switch( nID )
			{
			case 8360:
				{
					if(m_pStaticControl)
					{
						CSOpenTalismanSlot TalismanSlot;
						memset(&TalismanSlot, 0, sizeof(CSOpenTalismanSlot));
						TalismanSlot.nSlotIndex = m_pStaticControl->GetID();
						CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_OPEN_TALISMANSLOT, (char*)&TalismanSlot, sizeof(CSOpenTalismanSlot));
					}
				}
				break;
			}
		}
	}
}

const int CDnCharTalismanListDlg::FindEmptySlotIndex()
{
	if(m_vItemSlotBtnList.empty())
		return -1;

	for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
	{
		if( !m_vItemSlotBtnList[i].bIsServiceSlot)
			continue;

		if( !m_vItemSlotBtnList[i].bIsOpenSlot )
			continue;

		if( m_vItemSlotBtnList[i].pSlotButton->IsEmptySlot() )
			return i;			
	}

	return -1;
}

bool CDnCharTalismanListDlg::CheckParam(int nParam)
{
	if(m_vItemSlotBtnList.empty())
		return false;

	for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
	{
		CDnItem* pItem = (CDnItem*)m_vItemSlotBtnList[i].pSlotButton->GetItem();
		if(!pItem)
			continue;

		int nSlotItemParam = pItem->GetTypeParam(1);
		if( nSlotItemParam == nParam)
			return false;
	}

	return true;
}

void CDnCharTalismanListDlg::SetAllOpenSlot()
{
	if(m_vItemSlotBtnList.empty())
		return;

	for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
	{
		m_vItemSlotBtnList[i].pCoverStatic->Show(false);
		m_vItemSlotBtnList[i].pCoverStatic->Enable(false);

		m_vItemSlotBtnList[i].pCashCoverStatic->Show(false);
		m_vItemSlotBtnList[i].pCashCoverStatic->Enable(false);
	}
}

void CDnCharTalismanListDlg::SetSlotItem(int nIndex, MIInventoryItem* pItem)
{
	if(m_vItemSlotBtnList.empty())
		return;

	if(nIndex >= (int)m_vItemSlotBtnList.size())
		return;

	m_vItemSlotBtnList[nIndex].pSlotButton->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
}

bool CDnCharTalismanListDlg::RemoveSlotItem(int nIndex)
{
	if(m_vItemSlotBtnList.empty())
		return false;

	if(nIndex >= (int)m_vItemSlotBtnList.size())
		return false;

	m_vItemSlotBtnList[nIndex].pSlotButton->ResetSlot();

	return true;
}

float CDnCharTalismanListDlg::GetTalismanSlotRatio(int nSlotIndex)
{
	if(m_vItemSlotBtnList.empty())
		return -1;

	if( nSlotIndex < 0 || nSlotIndex >= (int)m_vItemSlotBtnList.size())
		return -1;

	return m_vItemSlotBtnList[nSlotIndex].fEfficiency;
}

void CDnCharTalismanListDlg::ShowTalismanSlotEfficiency(bool bShow)
{
	if(m_vItemSlotBtnList.empty())
		return;

	for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
	{
		/*if(!m_vItemSlotBtnList[i].bIsServiceSlot)
			continue;*/
		//m_vItemSlotBtnList[i].pPercentText->Show(bShow);
		//m_vItemSlotBtnList[i].pPercentText->Enable(bShow);
	}

	m_bIsEditMode = bShow;
}

#ifdef PRE_ADD_EQUIPLOCK
int CDnCharTalismanListDlg::GetWearedSlotIndex(const CDnItem* pItem) const
{
	if(!pItem)
		return -1;

	const CDnTalisman* pTalisman = static_cast<const CDnTalisman*>(pItem);
	if(!pTalisman)
		return -1;

	if(m_vItemSlotBtnList.empty())
		return -1;

	for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
	{
		const CDnItem* pSrcItem = static_cast<const CDnItem*>(m_vItemSlotBtnList[i].pSlotButton->GetItem());
		if(!pSrcItem) continue;
		if( pSrcItem->GetClassID() == pTalisman->GetClassID() )
			return i;
	}

	return -1;
}
#else
int CDnCharTalismanListDlg::GetWearedSlotIndex(CDnItem* pItem)
{
	if(!pItem)
		return -1;

	CDnTalisman* pTalisman = (CDnTalisman*)pItem;
	if(!pTalisman)
		return -1;

	if(m_vItemSlotBtnList.empty())
		return -1;

	for(int i = 0 ; i < (int)m_vItemSlotBtnList.size() ; ++i)
	{
		CDnItem* pSrcItem = (CDnItem*)m_vItemSlotBtnList[i].pSlotButton->GetItem();
		if(!pSrcItem) continue;
		if( pSrcItem->GetClassID() == pTalisman->GetClassID() )
			return i;
	}

	return -1;
}
#endif

CDnItem* CDnCharTalismanListDlg::GetEquipTalisman(int nIndex)
{
	if(m_vItemSlotBtnList.empty())
		return NULL;

	if(nIndex < 0 || nIndex >= (int)m_vItemSlotBtnList.size())
		return NULL;

	return (CDnItem*)m_vItemSlotBtnList[nIndex].pSlotButton->GetItem();
}
#endif // PRE_ADD_TALISMAN_SYSTEM