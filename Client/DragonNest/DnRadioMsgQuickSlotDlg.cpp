#include "StdAfx.h"
#include "DnRadioMsgQuickSlotDlg.h"
#include "DnRadioMsgButton.h"
//#include "DnActor.h"
//#include "DnLocalPlayerActor.h"
//#include "DnItemTask.h"
//#include "DnPlayerActor.h"
#include "DnMainDlg.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnRadioMsgQuickSlotDlg::CDnRadioMsgQuickSlotDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_nSkillDragSoundIndex(-1)
, m_nSkillLaydownSoundIndex(-1)
{
}

CDnRadioMsgQuickSlotDlg::~CDnRadioMsgQuickSlotDlg(void)
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

void CDnRadioMsgQuickSlotDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "RadioMsgQuickSlotDlg.ui" ).c_str(), bShow );

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

void CDnRadioMsgQuickSlotDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnRadioMsgButton *pQuickSlotButton(NULL);
	pQuickSlotButton = static_cast<CDnRadioMsgButton*>(pControl);

	SUIControlProperty *pProperty(NULL);
	pProperty = pQuickSlotButton->GetProperty();

	int nOrder;
	nOrder = pProperty->szUIName[ strlen( pProperty->szUIName ) - 1 ] - '1';

	if( nOrder == -1 )
	{
		nOrder = 9;
	}

	pQuickSlotButton->SetSlotType( ST_RADIOMSG_QUICKSLOT );
	pQuickSlotButton->SetSlotIndex( (int)m_vecSlotButton.size() );

	m_vecSlotButton.push_back( pQuickSlotButton );
}

void CDnRadioMsgQuickSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
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

					if( pDragButton == NULL )
					{
						if( !pPressedButton->GetItemID() )
							return;

						pPressedButton->EnableSplitMode(CDnSlotButton::ITEM_ORIGINAL_COUNT);
						drag::SetControl(pPressedButton);

						// 퀵슬롯에서 선택해도 소리가 나게 하자.
						switch( pPressedButton->GetItemType() )
						{
						case MIInventoryItem::Item:
						case MIInventoryItem::Skill:
							_ASSERT(0&&"Radio퀵슬롯에선 Item, Skill 집을 수 없다.");
							break;
						case MIInventoryItem::RadioMsg:
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
							switch( pDragButton->GetItemType() )
							{
							case MIInventoryItem::Item:
							case MIInventoryItem::Skill:
								break;
							case MIInventoryItem::RadioMsg:
								if( m_nSkillDragSoundIndex != -1 ) CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillDragSoundIndex );
								break;
							}
						}
						else
						{
							ITEM_SLOT_TYPE soltType = pDragButton->GetSlotType();
							switch( soltType )
							{
							case ST_RADIOMSG:
								{
									ResetSlot( FindRadioMsgSlotItem( pDragButton->GetItemID() ) );
									SetSlot( pDragButton, pPressedButton );

									if( m_nSkillLaydownSoundIndex != -1 )
										CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillLaydownSoundIndex );
								}
								break;
							case ST_INVENTORY:
							case ST_SKILL:
							case ST_QUICKSLOT:
								break;
							case ST_RADIOMSG_QUICKSLOT:
								{
									// 퀵슬롯 전환
									ChangeQuickSlotButton( pDragButton, pPressedButton );

									if( m_nSkillLaydownSoundIndex != -1 )
										CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillLaydownSoundIndex );
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
				/*
				if( !CDnActor::s_hLocalActor ) break;
				if( CDnLocalPlayerActor::IsLockInput() ) break;

				CDnQuickSlotButton *pButton = static_cast<CDnQuickSlotButton*>(pControl);
				MIInventoryItem *pItem = pButton->GetItem();
				if( !pItem ) break;

				switch( pButton->GetItemType() )
				{
				case MIInventoryItem::Item:
					{
						CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
						if( pPlayerActor->UseItemFromItemID( pItem->GetClassID() ) )
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
				*/
			}
			break;
		}
	}
}

bool CDnRadioMsgQuickSlotDlg::SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton )
{
	ASSERT(pDragButton&&"CDnQuickSlotDlg::SetSlot, pDragButton is NULL!");
	ASSERT(pPressedButton&&"CDnQuickSlotDlg::SetSlot, pPressedButton is NULL");

	if( pDragButton->GetItemID() == 0 )
		return false;

	((CDnRadioMsgButton*)pPressedButton)->SetRadioMsgInfo( pDragButton->GetItemID() );

	pDragButton->DisableSplitMode(false);
	drag::ReleaseControl();

	//GetItemTask().RequestAddQuickSlot( ((CDnQuickSlotButton*)pPressedButton)->GetItemSlotIndex(), pInvenItem->GetType(), pInvenItem->GetClassID() );
	//GetRadioMsgTask().RequestAddQuickSlot( m_vecSlotButton[nIndex]->GetItemSlotIndex(), /*pItem->GetType(),*/ pItem->GetClassID() );

	return true;
}

bool CDnRadioMsgQuickSlotDlg::InitSlot( int nIndex, int nRadioMsgID )
{
	// 처음 들어왔을때 서버에서 리스트 받으면 이 함수로 설정한다.

	ASSERT(nRadioMsgID&&"CDnRadioMsgQuickSlotDlg::SetSlot");
	if( !nRadioMsgID ) false;

	if( !m_vecSlotButton[nIndex]->GetItemID() )
	{
		ASSERT(0&&"빈 슬롯이 아닙니다." );
		return false;
	}

	m_vecSlotButton[nIndex]->SetRadioMsgInfo( nRadioMsgID );
	return true;
}

int CDnRadioMsgQuickSlotDlg::FindRadioMsgSlotItem( int nRadioMsgID )
{
	ASSERT( nRadioMsgID&&"CDnRadioMsgQuickSlotDlg::FindQuickSlotItem, ID is 0!" );

	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( !m_vecSlotButton[i]->GetItemID() )
			continue;

		if( m_vecSlotButton[i]->GetItemID() == nRadioMsgID )
			return i;
	}

	return -1;
}

void CDnRadioMsgQuickSlotDlg::ResetSlot( int nIndex )
{
	if( m_vecSlotButton.empty() ) return;
	if( nIndex < 0 ) return;
	if( nIndex >= (int)m_vecSlotButton.size() ) return;

	//GetRadioMsgTask().RequestDelQuickSlot( m_vecSlotButton[nIndex]->GetItemSlotIndex() );
	m_vecSlotButton[nIndex]->ResetSlot();
}

void CDnRadioMsgQuickSlotDlg::ChangeQuickSlotButton( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton )
{
	if( !pDragButton || !pPressedButton )
		return;

	int nDragRadioMsgID = pDragButton->GetItemID();
	int nPressedRadioMsgID = pPressedButton->GetItemID();

	if( !nDragRadioMsgID )
		return;

	int nSlotIndex = pPressedButton->GetSlotIndex();
	SetSlot( nSlotIndex, nDragRadioMsgID );

	if( nPressedRadioMsgID )
	{
		nSlotIndex = pDragButton->GetSlotIndex();
		SetSlot( nSlotIndex, nPressedRadioMsgID );
	}

	pDragButton->DisableSplitMode(false);
	pPressedButton->DisableSplitMode(false);
	drag::ReleaseControl();
}

bool CDnRadioMsgQuickSlotDlg::SetSlot( int nIndex, int nRadioMsgID )
{
	ASSERT(nRadioMsgID&&"CDnRadioMsgQuickSlotDlg::SetSlot");
	if( !nRadioMsgID ) false;

	ResetSlot( FindRadioMsgSlotItem( nRadioMsgID ) );

	//GetRadioMsgTask().RequestDelQuickSlot( m_vecSlotButton[nIndex]->GetItemSlotIndex() );
	m_vecSlotButton[nIndex]->ResetSlot();

	m_vecSlotButton[nIndex]->SetRadioMsgInfo( nRadioMsgID );
	//GetRadioMsgTask().RequestAddQuickSlot( m_vecSlotButton[nIndex]->GetItemSlotIndex(), /*pItem->GetType(),*/ pItem->GetClassID() );

	return true;
}