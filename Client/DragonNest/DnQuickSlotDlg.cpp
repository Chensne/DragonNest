#include "StdAfx.h"
#include "DnQuickSlotDlg.h"
#include "DnQuickSlotButton.h"
#include "DnActor.h"
#include "DnSkill.h"
#include "DnLocalPlayerActor.h"
#include "DnItemTask.h"
#include "DnPlayerActor.h"
#include "DnMainDlg.h"
#include "DnTableDB.h"
#include "DnSkillTask.h"
#include "SmartPtrDef.h"
#include "DnStateBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnQuickSlotDlg::CDnQuickSlotDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
	, m_nSkillDragSoundIndex(-1)
	, m_nSkillLaydownSoundIndex(-1)
{
}

CDnQuickSlotDlg::~CDnQuickSlotDlg(void)
{
	if( m_nSkillDragSoundIndex == -1 ) 
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_nSkillDragSoundIndex );
	}
	if( m_nSkillLaydownSoundIndex == -1 ) 
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_nSkillLaydownSoundIndex );
	}
}

void CDnQuickSlotDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "QuickSlotDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10005 );
	if( strlen( szFileName ) > 0 )
	{
		m_nSkillDragSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}

	szFileName = CDnTableDB::GetInstance().GetFileName( 10006 );
	if( strlen( szFileName ) > 0 )
	{
		m_nSkillLaydownSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}
}

void CDnQuickSlotDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnQuickSlotButton *pQuickSlotButton(NULL);
	pQuickSlotButton = static_cast<CDnQuickSlotButton*>(pControl);

	SUIControlProperty *pProperty(NULL);
	pProperty = pQuickSlotButton->GetProperty();

	int nOrder;
	nOrder = pProperty->szUIName[ strlen( pProperty->szUIName ) - 1 ] - '1';

	if( nOrder == -1 )
	{
		nOrder = 9;
	}

	pQuickSlotButton->SetButtonOrder( nOrder );
	pQuickSlotButton->SetSlotType( ST_QUICKSLOT );
	pQuickSlotButton->SetTabIndex( GetDialogID() );
	pQuickSlotButton->SetSlotIndex( (int)m_vecSlotButton.size() );

	m_vecSlotButton.push_back( pQuickSlotButton );
}

void CDnQuickSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch( uMsg )
		{
		case WM_LBUTTONDOWN:
			{
				if( strstr( pControl->GetControlName(), "ID_QUICKSLOT" ) )
				{
					CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
					CDnSlotButton *pPressedButton = static_cast<CDnSlotButton*>(pControl);

					int nReqSlotIndex = ((CDnQuickSlotButton*)pPressedButton)->GetItemSlotIndex();
					int nTabIndex = nReqSlotIndex / CDnMainDlg::QUICKSLOT_BUTTON_MAX;
					if(!(nTabIndex < CDnMainDlg::QUICKSLOT_DLG_MAX && nTabIndex >= CDnMainDlg::QUICKSLOT_DLG_00) )
						return;
					
					if( pDragButton == NULL )
					{
						if( pPressedButton->IsEmptySlot() )
							return;

						pPressedButton->EnableSplitMode(0);
						drag::SetControl(pPressedButton);

						// 퀵슬롯에서 선택해도 소리가 나게 하자.
						MIInventoryItem *pItem = pPressedButton->GetItem();
						if( !pItem ) return;

						switch( pPressedButton->GetItemType() )
						{
						case MIInventoryItem::Item:
							CEtSoundEngine::GetInstance().PlaySound( "2D", ((CDnItem*)pItem)->GetDragSoundIndex() );
							break;
						case MIInventoryItem::Skill:
							if( m_nSkillDragSoundIndex != -1 ) CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillDragSoundIndex );
							break;
						}
					}
					else
					{
						if( pDragButton == pPressedButton )
						{
							pDragButton->DisableSplitMode(true);
							drag::ReleaseControl();

							// 같은 자리로 이동되는 것도 소리가 나게 하자.
							MIInventoryItem *pItem = pDragButton->GetItem();
							if( !pItem ) break;

							switch( pDragButton->GetItemType() )
							{
							case MIInventoryItem::Item:
								CEtSoundEngine::GetInstance().PlaySound( "2D", ((CDnItem*)pItem)->GetDragSoundIndex() );
								break;
							case MIInventoryItem::Skill:
								if( m_nSkillLaydownSoundIndex != -1 ) CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillLaydownSoundIndex );
								break;
							}
						}
						else
						{
							ITEM_SLOT_TYPE soltType = pDragButton->GetSlotType();
							switch( soltType )
							{
							case ST_INVENTORY:
								{
									ResetSlot( FindQuickSlotItem( pDragButton->GetItem() ) );
									SetSlot( pDragButton, pPressedButton );
								}
								break;
							case ST_SKILL:
								{
									if( GetSkillTask().IsGuildWarSkill( pDragButton->GetItem()->GetClassID() ) )	// 길드전 스킬은 등록 안되게 함.
										break;
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
									if (GetSkillTask().IsTotalLevelSkill(pDragButton->GetItem()->GetClassID()))
										break;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
									CDnMainDlg *pMainDlg = GetInterface().GetMainBarDialog();
									if (pMainDlg)
									{
										MIInventoryItem *pItem = pDragButton->GetItem();
										if (!pItem) break;

										if (CDnActor::s_hLocalActor)
										{
											if (CDnActor::s_hLocalActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_345))
											{
												DNVector(DnBlowHandle) vlBlows;
												CDnActor::s_hLocalActor->GetStateBlow()->GetStateBlowFromBlowIndex(STATE_BLOW::BLOW_345, vlBlows);

												int nBlowCount = (int)vlBlows.size();
												for (int i = 0; i < nBlowCount; ++i)
												{
													DnBlowHandle hBlow = vlBlows[i];
													if (hBlow)
													{
														int nNewSkillID = hBlow->GetReplaceSkill(pItem->GetClassID());
														if (nNewSkillID > 0)
														{
															CDnSkill * pSkill = (CDnSkill *)(pItem);
															GetSkillTask().AddSkill(nNewSkillID, pSkill->GetLevel(), pSkill->GetApplyType());
															DnSkillHandle hNewSkill = CDnSkill::CreateSkill(CDnActor::Identity(), nNewSkillID, pSkill->GetLevel());

															if (CDnActor::s_hLocalActor)
															{
																hNewSkill->SetHasActor(CDnActor::s_hLocalActor);
																CDnActor::s_hLocalActor->AddSkill(hNewSkill);
															}
															pDragButton->ResetSlot();
															pDragButton->SetItem(hNewSkill, pItem->GetOverlapCount());
														}
													}
												}
											}
										}
									}

									ResetSlot( FindQuickSlotItem( pDragButton->GetItem() ) );
									SetSlot( pDragButton, pPressedButton );



									if( m_nSkillLaydownSoundIndex != -1 )
										CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillLaydownSoundIndex );
								}
								break;
							case ST_QUICKSLOT:
								{
									if( GetSkillTask().IsGuildWarSkill( pDragButton->GetItem()->GetClassID() ) )	// 길드전 스킬은 등록 안되게 함.
										break;
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
									if (GetSkillTask().IsTotalLevelSkill(pDragButton->GetItem()->GetClassID()))
										break;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

									CDnMainDlg *pMainDlg = GetInterface().GetMainBarDialog();
									if( pMainDlg )
									{
										MIInventoryItem *pItem = pDragButton->GetItem();
										if( !pItem ) break;

										if(CDnActor::s_hLocalActor)
										{
											if (CDnActor::s_hLocalActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_345))
											{
												DNVector(DnBlowHandle) vlBlows;
												CDnActor::s_hLocalActor->GetStateBlow()->GetStateBlowFromBlowIndex(STATE_BLOW::BLOW_345, vlBlows);

												int nBlowCount = (int)vlBlows.size();
												for (int i = 0; i < nBlowCount; ++i)
												{
													DnBlowHandle hBlow = vlBlows[i];
													if (hBlow)
													{
														int nNewSkillID = hBlow->GetReplaceSkill(pItem->GetClassID());
														if (nNewSkillID > 0)
														{
															CDnSkill * pSkill = (CDnSkill *)(pItem);
															GetSkillTask().AddSkill(nNewSkillID, pSkill->GetLevel(), pSkill->GetApplyType());
															DnSkillHandle hNewSkill = CDnSkill::CreateSkill(CDnActor::Identity(), nNewSkillID, pSkill->GetLevel());

															if (CDnActor::s_hLocalActor)
															{
																hNewSkill->SetHasActor(CDnActor::s_hLocalActor);
																CDnActor::s_hLocalActor->AddSkill(hNewSkill);
															}
															pDragButton->ResetSlot();
															pDragButton->SetItem(hNewSkill, pItem->GetOverlapCount());
														}
													}
												}
											}
										}
										// 퀵슬롯 전환
										pMainDlg->ChangeQuickSlotButton( pDragButton, pPressedButton );
									}
								}
							    break;
							case ST_INVENTORY_CASH:
								{
									ResetSlot( FindQuickSlotItem( pDragButton->GetItem() ) );
									SetSlot( pDragButton, pPressedButton );
								}
								break;
							}
						}
					}

					return;
				}
			}
			break;
		case WM_RBUTTONUP:
		case WM_KEYDOWN:
			{
				if( !CDnActor::s_hLocalActor ) break;
				if( CDnLocalPlayerActor::IsLockInput() ) break;

				CDnQuickSlotButton *pButton = static_cast<CDnQuickSlotButton*>(pControl);
				MIInventoryItem *pItem = pButton->GetItem();
				if( !pItem ) break;

				switch( pButton->GetItemType() )
				{
				case MIInventoryItem::Item:
					{
						char cType = 0;
						cType = ( CDnItem::IsCashItem( pItem->GetClassID() ) ) ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;
						CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
						if( pPlayerActor->UseItemFromItemID( pItem->GetClassID(), cType ) )
						{
							// 서버에서 응답 오면 사운드 나게 한다.
							//CEtSoundEngine::GetInstance().PlaySound( "2D", ((CDnItem*)pItem)->GetUseSoundIndex() );
						}
					}
					break;
				case MIInventoryItem::Skill:
					{
						DnSkillHandle hSkill = CDnActor::s_hLocalActor->FindSkill(pItem->GetClassID());
						if( !hSkill ) break;
						if( hSkill->GetSkillType() != CDnSkill::Active ) break;
						CDnActor::s_hLocalActor->UseSkill( pItem->GetClassID() );
					}
					break;
				}
			}
			break;
		}
	}
}

void CDnQuickSlotDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !drag::IsValid() || !CDnActor::s_hLocalActor )
		return;

	// 제스처 슬롯안에 마우스 들어왔는지 검사
	bool bMouseEnter = false;
	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i] && m_vecSlotButton[i]->IsMouseEnter() )
		{
			bMouseEnter = true;
			break;
		}
	}

	if( !bMouseEnter )
		return;

	// 들어온 슬롯이 아이템 중 슬롯에 넣을 수 있는 아이템(물약류)인지 검사.
	CDnSlotButton* pDragButton = (CDnSlotButton*)drag::GetControl();
	if( pDragButton->GetItemType() != MIInventoryItem::Item )
		return;

	// 여긴 일반적인 경우엔 안걸린다.
	// 상점에 팔거나 해서 리프레쉬 인벤을 통해 아이템은 삭제되었는데도,
	// drag::GetControl에 해당 슬롯버튼이 설정되있거나 했을때 들어온다.
	if( !pDragButton->GetItem() )
		return;

	if( pDragButton->GetSlotType() != ST_INVENTORY_VEHICLE )
		return;

	CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
	if( !pActor->IsBattleMode() )
		return;

	if( !pActor->IsDie() && ( pActor->IsStay() || pActor->IsMove() ) )
	{
		if( pActor->IsMove() ) pActor->CmdStop( "Stand" );
		pActor->CmdToggleBattle(false);
	}
}

bool CDnQuickSlotDlg::SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton )
{
	ASSERT(pDragButton&&"CDnQuickSlotDlg::SetSlot, pDragButton is NULL!");
	ASSERT(pPressedButton&&"CDnQuickSlotDlg::SetSlot, pPressedButton is NULL");

	int nReqSlotIndex = ((CDnQuickSlotButton*)pPressedButton)->GetItemSlotIndex();
	int nTabIndex = nReqSlotIndex / CDnMainDlg::QUICKSLOT_BUTTON_MAX;

	if(!(nTabIndex < CDnMainDlg::QUICKSLOT_DLG_MAX && nTabIndex >= CDnMainDlg::QUICKSLOT_DLG_00) )
	{
		drag::Command(UI_DRAG_CMD_CANCEL);
		drag::ReleaseControl();
		return false;
	}

	MIInventoryItem *pInvenItem = pDragButton->GetItem();
	if( !pInvenItem ) return false;

	if( !pInvenItem->IsQuickSlotItem() )
		return false;

	if(nTabIndex < CDnMainDlg::QUICKSLOT_DLG_MAX && nTabIndex >= CDnMainDlg::QUICKSLOT_DLG_00 )
		((CDnQuickSlotButton*)pPressedButton)->SetQuickItem( pInvenItem );

	pDragButton->DisableSplitMode(true);
	drag::ReleaseControl();

	if(nTabIndex < CDnMainDlg::QUICKSLOT_DLG_MAX && nTabIndex >= CDnMainDlg::QUICKSLOT_DLG_00 )
		GetItemTask().RequestAddQuickSlot( nReqSlotIndex , pInvenItem->GetType(), pInvenItem->GetClassID() );

	CDnItem *pItem = dynamic_cast<CDnItem *>(pInvenItem);
	if( pItem ) CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	return true;
}

bool CDnQuickSlotDlg::InitSlot( int nIndex, MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnQuickSlotDlg::InitSlot");
	if( !pItem ) return false;

	if( !pItem->IsQuickSlotItem() )
		return false;

	if( nIndex < 0 || nIndex >= static_cast<int>( m_vecSlotButton.size() ) )
		return false;

	if( m_vecSlotButton[nIndex] && !m_vecSlotButton[nIndex]->IsEmptySlot() )
	{
		ASSERT(0&&"빈 슬롯이 아닙니다." );
		return false;
	}

	m_vecSlotButton[nIndex]->SetQuickItem( pItem );
	return true;
}

bool CDnQuickSlotDlg::SetSlot( int nIndex, MIInventoryItem *pItem )
{
	if( nIndex < 0 || nIndex >= static_cast<int>( m_vecSlotButton.size() ) )
		return false;

	if( m_vecSlotButton[nIndex] == NULL )
		return false;

	int nReqSlotIndex = m_vecSlotButton[nIndex]->GetItemSlotIndex();
	int nTabIndex = nReqSlotIndex / CDnMainDlg::QUICKSLOT_BUTTON_MAX;

	if(!(nTabIndex < CDnMainDlg::QUICKSLOT_DLG_MAX && nTabIndex >= CDnMainDlg::QUICKSLOT_DLG_00))
		return false;

	ASSERT(pItem&&"CDnQuickSlotDlg::SetSlot");
	if( !pItem ) false;

	if( !pItem->IsQuickSlotItem() )
		return false;

	ResetSlot( FindQuickSlotItem( pItem ) );

	GetItemTask().RequestDelQuickSlot( m_vecSlotButton[nIndex]->GetItemSlotIndex() );
	m_vecSlotButton[nIndex]->ResetSlot();
 	m_vecSlotButton[nIndex]->SetQuickItem( pItem );

	if(nTabIndex < CDnMainDlg::QUICKSLOT_DLG_MAX && nTabIndex >= CDnMainDlg::QUICKSLOT_DLG_00 )
		GetItemTask().RequestAddQuickSlot( nReqSlotIndex, pItem->GetType(), pItem->GetClassID() );

	CDnItem *pSetItem = dynamic_cast<CDnItem *>(pItem);
	if( pSetItem ) CEtSoundEngine::GetInstance().PlaySound( "2D", pSetItem->GetDragSoundIndex() );

	return true;
}

bool CDnQuickSlotDlg::SetEventSlot( MIInventoryItem *pItem )
{
	int nEmptyIndex = 0;

	for(int i=0; i<(int)m_vecSlotButton.size() ; i++)
	{
		if( m_vecSlotButton[i] == NULL ) continue;
		if(m_vecSlotButton[i]->GetItem() && pItem && (m_vecSlotButton[i]->GetItem()->GetClassID() == pItem->GetClassID()) )
			return false; // 동일한것이 있으면 리턴
	}

	for(int i=0; i<(int)m_vecSlotButton.size() ; i++)
	{
		if( m_vecSlotButton[i] == NULL ) continue;
		if(m_vecSlotButton[i]->GetItem() == NULL )
		{
			nEmptyIndex = i; // 빈슬롯이 있으면 넣는다, 만약에 가득차있다면 들어오지않으므로 0 번 슬롯에 들어간다.
			break;
		}
	}

	ASSERT( pItem && "CDnQuickSlotDlg::SetEventSlot" );
	if( !pItem ) false;
	if( !pItem->IsQuickSlotItem() )
		return false;

	if( m_vecSlotButton[nEmptyIndex] )
	{
		m_vecSlotButton[nEmptyIndex]->ResetSlot();
		m_vecSlotButton[nEmptyIndex]->SetQuickItem( pItem );
	}

	return true;
}


void CDnQuickSlotDlg::ResetAllSlot()
{
	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i] == NULL ) continue;
		m_vecSlotButton[i]->ResetSlot();
	}
}

void CDnQuickSlotDlg::OnRefreshSlot()
{
	if( !CDnActor::s_hLocalActor )
		return;

	int nItemID(0);
	MIInventoryItem *pItem(NULL);

	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i] == NULL ) continue;

		nItemID = m_vecSlotButton[i]->GetItemID();
		if( nItemID == 0 ) continue;

		switch( m_vecSlotButton[i]->GetItemType() )
		{
		case MIInventoryItem::Item:
			if( CDnItem::IsCashItem(nItemID) )
				pItem = GetItemTask().FindItem( nItemID, ITEM_SLOT_TYPE::ST_INVENTORY_CASH );
			else
				pItem = GetItemTask().FindItem( nItemID, ITEM_SLOT_TYPE::ST_INVENTORY );
			break;
		case MIInventoryItem::Skill:

			int nTabIndex =  m_vecSlotButton[i]->GetItemSlotIndex() / CDnMainDlg::QUICKSLOT_BUTTON_MAX;
			int nPageIndex = nTabIndex / DualSkill::Type::MAX;
			pItem = GetSkillTask().FindSkillBySkillPage( nItemID, nPageIndex );

			if(!pItem)
				pItem = CDnActor::s_hLocalActor->FindSkill( nItemID );

			break;
		}

		if( !pItem )
		{
			m_vecSlotButton[i]->ResetSlot();
			m_vecSlotButton[i]->SetItemID( nItemID );
		}
		else
		{
			m_vecSlotButton[i]->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		}
	}
}

int CDnQuickSlotDlg::FindQuickSlotItem( MIInventoryItem *pItem )
{
	ASSERT( pItem&&"CDnQuickSlotDlg::FindQuickSlotItem, pItem is NULL!" );

	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i] == NULL ) continue;
		if( m_vecSlotButton[i]->IsEmptySlot() )
			continue;

		if( pItem->GetType() != m_vecSlotButton[i]->GetItemType() )
			continue;

		if( m_vecSlotButton[i]->GetItemID() == pItem->GetClassID() )
			return i;
	}

	return -1;
}

int CDnQuickSlotDlg::FindEmptyQuickSlotIndex()
{
	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i] == NULL ) continue;
		if( m_vecSlotButton[i]->IsEmptySlot() )
			return m_vecSlotButton[i]->GetItemSlotIndex();
	}
	return -1;
}

int CDnQuickSlotDlg::FindSkillQuickSlotIndex( int nSkillID )
{
	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i] == NULL || m_vecSlotButton[i]->IsEmptySlot() )
			continue;

		CDnSkill * pSkill = (CDnSkill *)(m_vecSlotButton[i]->GetItem());
		if( NULL == pSkill )
			continue;

		if( pSkill->GetClassID() == nSkillID )
			return m_vecSlotButton[i]->GetItemSlotIndex();
	}
	return -1;
}


int CDnQuickSlotDlg::FindSkillQuickSlotIndexAndReplace(int nSkillID,int nSkillIDReplace,bool bRevert)
{
	for (int i = 0; i<(int)m_vecSlotButton.size(); i++)
	{
		if (m_vecSlotButton[i] == NULL || m_vecSlotButton[i]->IsEmptySlot())
			continue;

		CDnSkill * pSkill = (CDnSkill *)(m_vecSlotButton[i]->GetItem());
		if (NULL == pSkill)
			continue;

	
		if (pSkill->GetClassID() == nSkillID)
		{
			DnSkillHandle hNewSkill = CDnActor::s_hLocalActor->FindSkill(nSkillIDReplace);
			if (!hNewSkill){
				OutputDebug("%s  SkillNotFound: %d", __FUNCTION__, nSkillIDReplace);
				GetSkillTask().AddSkill(nSkillIDReplace, pSkill->GetLevel(), pSkill->GetApplyType());
				hNewSkill = CDnSkill::CreateSkill(CDnActor::Identity(), nSkillIDReplace, pSkill->GetLevel());
			}
			
			if(CDnActor::s_hLocalActor)
			{
				hNewSkill->SetHasActor(CDnActor::s_hLocalActor);
				CDnActor::s_hLocalActor->AddSkill(hNewSkill);
			}

			hNewSkill->AcquireByBlow(!bRevert);
			
			hNewSkill->ResetCoolTime();

			ResetSlot(i);
			SetSlot(i, hNewSkill);



			return m_vecSlotButton[i]->GetItemSlotIndex();
		}
	}
	return -1;
}

void CDnQuickSlotDlg::ResetSlot( int nIndex )
{
	if( m_vecSlotButton.empty() ) return;
	if( nIndex < 0 ) return;
	if( nIndex >= (int)m_vecSlotButton.size() ) return;

	int nReqSlotIndex =  m_vecSlotButton[nIndex]->GetItemSlotIndex();
	int nTabIndex = nReqSlotIndex / CDnMainDlg::QUICKSLOT_BUTTON_MAX;
	if(!(nTabIndex < CDnMainDlg::QUICKSLOT_DLG_MAX && nTabIndex >= CDnMainDlg::QUICKSLOT_DLG_00))
		return;

	GetItemTask().RequestDelQuickSlot( m_vecSlotButton[nIndex]->GetItemSlotIndex() );
	m_vecSlotButton[nIndex]->ResetSlot();
}

void CDnQuickSlotDlg::EnableQuickSlot(bool bEnable)
{
	std::vector<CDnQuickSlotButton*>::iterator iter = m_vecSlotButton.begin();
	for( ; iter != m_vecSlotButton.end(); ++iter )
	{
		CDnQuickSlotButton* pBtn = (*iter);
		if( pBtn && pBtn->GetItem() != NULL )
			pBtn->Enable( bEnable );
	}
}

void CDnQuickSlotDlg::GetItemList(std::vector<int> &vecList)
{
	vecList.clear();

	for( DWORD i=0; i<m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i] == NULL ) continue;
		vecList.push_back( m_vecSlotButton[i]->GetItemID() );
	}
}