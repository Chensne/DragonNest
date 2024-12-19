#include "StdAfx.h"
#include "DnTotalLevelSkillActivateListDlg.h"
#include "DnSkillTask.h"
#include "TotalLevelSkillSystem.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const int CASH_SLOT_INDEX = 3;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
CDnTotalLevelSlotToolTipDlg::CDnTotalLevelSlotToolTipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pContents = NULL;
}

CDnTotalLevelSlotToolTipDlg::~CDnTotalLevelSlotToolTipDlg()
{

}

void CDnTotalLevelSlotToolTipDlg::AddText( LPCWSTR szText, const D3DCOLOR TextColor, DWORD dwFormat, const D3DCOLOR BgColor )
{
	if (m_pContents)
		m_pContents->SetText(szText, TextColor);
}

void CDnTotalLevelSlotToolTipDlg::ClearText()
{
	if (m_pContents)
		m_pContents->ClearText();
}

void CDnTotalLevelSlotToolTipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SimpleTooltipDlg.ui" ).c_str(), bShow );
}

void CDnTotalLevelSlotToolTipDlg::InitialUpdate()
{
	m_pContents = GetControl<CEtUITextBox>( "ID_TEXTBOX" );
}

//////////////////////////////////////////////////////////////////////////

CDnTotalLevelSkillActivateListDlg::CDnTotalLevelSkillActivateListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pCashSlotCover = NULL;
	m_pCashButton = NULL;
	m_pTooltipDlg = NULL;
}

CDnTotalLevelSkillActivateListDlg::~CDnTotalLevelSkillActivateListDlg(void)
{
	SAFE_DELETE(m_pTooltipDlg);
}

void CDnTotalLevelSkillActivateListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillTreeSubSkillDlg.ui" ).c_str(), bShow );

	m_pTooltipDlg = new CDnTotalLevelSlotToolTipDlg(UI_TYPE_MOST_TOP, NULL);
	if (m_pTooltipDlg)
		m_pTooltipDlg->Initialize(false);
}

void CDnTotalLevelSkillActivateListDlg::InitialUpdate()
{
	m_pCashSlotCover = GetControl<CEtUIStatic>("ID_STATIC_COVER");
	m_pCashButton = GetControl<CEtUIStatic>("ID_STATIC_CASH");

	if (m_pCashButton)
		m_pCashButton->Show(false);
}

void CDnTotalLevelSkillActivateListDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}
	else
	{
		if (m_pTooltipDlg)
			m_pTooltipDlg->Show(false);
	}

	CEtUIDialog::Show( bShow );
}

bool CDnTotalLevelSkillActivateListDlg::SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton )
{
	ASSERT(pDragButton&&"CDnQuickSlotDlg::SetSlot, pDragButton is NULL!");
	ASSERT(pPressedButton&&"CDnQuickSlotDlg::SetSlot, pPressedButton is NULL");

	int nReqSlotIndex =pPressedButton->GetSlotIndex();

	MIInventoryItem *pInvenItem = pDragButton->GetItem();
	if( !pInvenItem ) return false;

	pDragButton->DisableSplitMode(true);
	drag::ReleaseControl();

	if (pInvenItem->GetType() == MIInventoryItem::Skill)
	{
		CDnSkill* pSkill = static_cast<CDnSkill*>(pInvenItem);

		GetSkillTask().RequestAddTotalLevelSkill(nReqSlotIndex, pSkill->GetClassID());
	}

	return true;
}

void CDnTotalLevelSkillActivateListDlg::SetSlot( int nSlotIndex, DnSkillHandle hSkill )
{
	if (nSlotIndex < 0 || nSlotIndex >= (int)m_vecSlotButton.size())
		return;

	CDnSkill* pSkill = hSkill.GetPointer();
	m_vecSlotButton[nSlotIndex]->SetQuickItem(pSkill);

// 	if (pSkill)
// 		m_vecSlotButton[nSlotIndex]->SetForceUsable(true);
// 	else
// 		m_vecSlotButton[nSlotIndex]->SetForceUnUsable(true);
		
	return;
}

void CDnTotalLevelSkillActivateListDlg::ResetSlot(int nSlotIndex)
{
	if (nSlotIndex < 0 || nSlotIndex >= (int)m_vecSlotButton.size())
		return;

	m_vecSlotButton[nSlotIndex]->ResetSlot();
}

void CDnTotalLevelSkillActivateListDlg::InitCustomControl( CEtUIControl *pControl )
{
	if( typeid(*pControl) != typeid(CDnQuickSlotButton) )
		return;

	CDnQuickSlotButton *pQuickSlotButton(NULL);
	pQuickSlotButton = static_cast<CDnQuickSlotButton*>(pControl);

	pQuickSlotButton->SetSlotType( ST_TOTAL_LEVEL_SKILL );
	pQuickSlotButton->SetTabIndex( 0 );
	pQuickSlotButton->SetSlotIndex( (int)m_vecSlotButton.size() );

	m_vecSlotButton.push_back( pQuickSlotButton );
}

void CDnTotalLevelSkillActivateListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch(uMsg)
		{
		case WM_LBUTTONDOWN:
			{
				if (strstr(pControl->GetControlName(), "ID_BT_SUBSKILL"))
				{
					CDnSlotButton* pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
					CDnSlotButton* pPressedButton = static_cast<CDnSlotButton*>(pControl);

					if( pDragButton == NULL )
					{
						if( pPressedButton->IsEmptySlot() )
							return;

						pPressedButton->EnableSplitMode(0);
						drag::SetControl(pPressedButton);
					}
					else
					{
						if( pDragButton == pPressedButton )
						{
							pDragButton->DisableSplitMode(true);
							drag::ReleaseControl();

						}
						else
						{
							ITEM_SLOT_TYPE soltType = pDragButton->GetSlotType();
							switch( soltType )
							{
							case ST_TOTAL_LEVEL_SKILL:
								{
									pDragButton->DisableSplitMode(true);
									drag::ReleaseControl();
								}
								break;
							case ST_SKILL:
								{
									CDnTotalLevelSkillSystem* pTotalLevelSkillSystem = GetSkillTask().GetTotalLevelSkillSystem();
									CDnSkill* pSkill = static_cast<CDnSkill*>(pDragButton->GetItem());

									int nReqSlotIndex = pPressedButton->GetSlotIndex();

									if (pTotalLevelSkillSystem && pSkill)
									{										
										if (pTotalLevelSkillSystem->IsTotalLevelSkill(pSkill->GetClassID()) == false)
											break;

										int nResult = pTotalLevelSkillSystem->CanAddSkill(nReqSlotIndex, pSkill->GetMySmartPtr());
										
										switch(nResult)
										{
										case CDnTotalLevelSkillSystem::TotalLevelSystem_Error_None:
											{
												SetSlot(pDragButton, pPressedButton);
											}
											break;
										case CDnTotalLevelSkillSystem::TotalLevelSystem_Error_SameSkillType:
											{
												//이미 같은 타입의 통합레벨 스킬이 장착되어 있을경우..
												GetInterface().ShowCaptionDialog( CDnInterface::typeCaption1, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1830 ), textcolor::YELLOW, 1.0f );
											}
											break;
										}
									}

									pDragButton->DisableSplitMode(true);
									drag::ReleaseControl();									
								}
								break;
							}
						}
					}
				}
			}
			break;
		case WM_RBUTTONUP:
			{
				if (strstr(pControl->GetControlName(), "ID_BT_SUBSKILL"))
				{
					CDnSlotButton* pPressedButton = static_cast<CDnSlotButton*>(pControl);
					if (pPressedButton)
					{
						ITEM_SLOT_TYPE soltType = pPressedButton->GetSlotType();
						switch( soltType )
						{
						case ST_TOTAL_LEVEL_SKILL:
							{
								CDnTotalLevelSkillSystem* pTotalLevelSkillSystem = GetSkillTask().GetTotalLevelSkillSystem();
								CDnSkill* pSkill = static_cast<CDnSkill*>(pPressedButton->GetItem());

								int nReqSlotIndex = pPressedButton->GetSlotIndex();

								if (pTotalLevelSkillSystem && pSkill)
								{										
									//ResetSlot(nReqSlotIndex);
									GetSkillTask().RequestRemoveTotalLevelSkill(nReqSlotIndex);
								}
							}
							break;
						}
					}
				}
			}
			break;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnTotalLevelSkillActivateListDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_MOUSEMOVE:
		{
			if( !IsShow() )
				break;

			if (m_pTooltipDlg)
				m_pTooltipDlg->ClearText();

			bool bShowTooltip = false;

			CDnSlotButton* pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			if (pDragButton && pDragButton->GetItem())
				break;

			if (m_pTooltipDlg == NULL)
				break;

			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );

			float fMouseX, fMouseY;
			PointToFloat( MousePoint, fMouseX, fMouseY );

			if (m_pCashButton && m_pCashButton->IsShow() && m_pCashButton->IsInside(fMouseX, fMouseY))
			{
				CDnTotalLevelSkillSystem::_TotalLevelSkillSlotInfo* pInfo = NULL;
				CDnTotalLevelSkillSystem* pTotalLevelSystem = GetSkillTask().GetTotalLevelSkillSystem();
				if (pTotalLevelSystem)
					pInfo = pTotalLevelSystem->GetSlotInfo(CASH_SLOT_INDEX);

				if (pInfo && pInfo->m_isCashSlot == true && pInfo->m_ExpireDate != 0)
				{
					WCHAR dateString[256] = {0,};
					WCHAR wszTimeInfo[512] = {0, };

					tm Day;
					if( DnLocalTime_s(&Day, &pInfo->m_ExpireDate) == false) 
						break;
					
					CommonUtil::GetCashRemainDate(pInfo->m_ExpireDate, dateString, 256);

					std::wstring stringProcessed;
					if (CommonUtil::ConvertTimeFormatString(dateString, stringProcessed) != false)
					{
						_snwprintf_s(wszTimeInfo, _countof(wszTimeInfo), _TRUNCATE, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1823 ), stringProcessed.c_str());
						m_pTooltipDlg->AddText(wszTimeInfo);

						float fX, fY;
						GetPosition(fX, fY);
						m_pTooltipDlg->SetPosition(fX + fMouseX, fY + fMouseY - m_pTooltipDlg->GetDlgCoord().fHeight);
						bShowTooltip = true;
					}

					break;
				}
			}

			std::vector< CDnQuickSlotButton* >::iterator iter = m_vecSlotButton.begin();
			std::vector< CDnQuickSlotButton* >::iterator endIter = m_vecSlotButton.end();

			int nCharLevel = CDnActor::s_hLocalActor ? CDnActor::s_hLocalActor->GetLevel() : -1;

			int nSlotIndex = 0;
			for (; iter != endIter; ++iter, ++nSlotIndex)
			{
				CDnQuickSlotButton* pSlotButton = (*iter);

				if (pSlotButton && pSlotButton->IsShow() && pSlotButton->IsInside(fMouseX, fMouseY))
				{
					CDnTotalLevelSkillSystem::_TotalLevelSkillSlotInfo* pInfo = NULL;
					CDnTotalLevelSkillSystem* pTotalLevelSystem = GetSkillTask().GetTotalLevelSkillSystem();
					if (pTotalLevelSystem)
						pInfo = pTotalLevelSystem->GetSlotInfo(nSlotIndex);

					if (pSlotButton->GetItem() == NULL && pInfo)
					{
						bool isLimited = nCharLevel != -1 ? nCharLevel < pInfo->m_LevelLimit : true;

						if (pInfo->m_isCashSlot == true)
						{
							WCHAR wszTotalLevelInfo[256] = {0, };

							//스킬제한 : 모든스킬장착가능
							_snwprintf_s(wszTotalLevelInfo, _countof(wszTotalLevelInfo), _TRUNCATE, L"%s : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1828 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1827 ));
							m_pTooltipDlg->AddText(wszTotalLevelInfo);

							//레벨 설정.
							WCHAR wszLevelInfo[128] = {0, };
							_snwprintf_s(wszLevelInfo, _countof(wszLevelInfo), _TRUNCATE, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 728 ), pInfo->m_LevelLimit);

							//필요 레벨 : Lv. 60
							_snwprintf_s(wszTotalLevelInfo, _countof(wszTotalLevelInfo), _TRUNCATE, L"%s : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9022 ), wszLevelInfo);
							m_pTooltipDlg->AddText(wszTotalLevelInfo, isLimited ? textcolor::RED : textcolor::WHITE);

							//등급
							_snwprintf_s(wszTotalLevelInfo, _countof(wszTotalLevelInfo), _TRUNCATE, L"%s %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2252 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8043 ));
							m_pTooltipDlg->AddText(wszTotalLevelInfo);
						}
						else
						{
							WCHAR wszTotalLevelInfo[256] = {0, };

							//레벨 설정.
							WCHAR wszLevelInfo[128] = {0, };
							_snwprintf_s(wszLevelInfo, _countof(wszLevelInfo), _TRUNCATE, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 728 ), pInfo->m_LevelLimit);
							
							//필요 레벨 : Lv. 60
							_snwprintf_s(wszTotalLevelInfo, _countof(wszTotalLevelInfo), _TRUNCATE, L"%s : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1804 ), wszLevelInfo);
							m_pTooltipDlg->AddText(wszTotalLevelInfo, isLimited ? textcolor::RED : textcolor::WHITE);

							//등급
							_snwprintf_s(wszTotalLevelInfo, _countof(wszTotalLevelInfo), _TRUNCATE, L"%s %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2252 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8042 ));
							m_pTooltipDlg->AddText(wszTotalLevelInfo);
						}

						float fX, fY;
						GetPosition(fX, fY);

						m_pTooltipDlg->SetPosition(fX + fMouseX, fY + fMouseY - m_pTooltipDlg->GetDlgCoord().fHeight);
						bShowTooltip = true;
						break;
					}
				}
			}

			m_pTooltipDlg->Show( bShowTooltip );
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnTotalLevelSkillActivateListDlg::ActivateSkillSlot(int nSlotIndex, bool bActivate)
{
	if (nSlotIndex < 0 || nSlotIndex >= (int)m_vecSlotButton.size())
		return;

	m_vecSlotButton[nSlotIndex]->Enable(bActivate);
	
	if (bActivate)
		m_vecSlotButton[nSlotIndex]->SetForceUsable(true);
	else
		m_vecSlotButton[nSlotIndex]->SetForceUnUsable(true);
}

void CDnTotalLevelSkillActivateListDlg::ActivateSkillCashSlot(int nSlotIndex, bool bActivate, __time64_t tExpireDate)
{
	if (CASH_SLOT_INDEX != nSlotIndex)
	{
		OutputDebug("%s 캐시 슬롯 인덱스가 맞지 않음... %d\n", __FUNCTION__, nSlotIndex);
		return;
	}

	ActivateSkillSlot(nSlotIndex, bActivate);

	if (m_pCashSlotCover)
		m_pCashSlotCover->Show(!bActivate);

	if (m_pCashButton)
	{
		if (bActivate == false)
			m_pCashButton->Show(bActivate);
		else
			m_pCashButton->Show(tExpireDate != 0);
	}
}

#endif // PRE_ADD_TOTAL_LEVEL_SKILL
