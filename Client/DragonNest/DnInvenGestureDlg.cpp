#include "StdAfx.h"
#include "DnInvenGestureDlg.h"
#include "DnGestureButton.h"
#include "DnGestureTask.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "DnPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnInvenGestureDlg::CDnInvenGestureDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallBack )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallBack )
, m_pSkillPage(NULL)
, m_pButtonPagePrev( NULL )
, m_pButtonPageNext( NULL )
, m_nMaxPage(1)
, m_nCurrentPage(1)
, m_nDragSoundIndex(-1)
{
}

CDnInvenGestureDlg::~CDnInvenGestureDlg(void)
{
	if( m_nDragSoundIndex == -1 ) 
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_nDragSoundIndex );
	}
}

void CDnInvenGestureDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemGestureDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10005 );
	if( strlen( szFileName ) > 0 )
	{
		m_nDragSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}
}

void CDnInvenGestureDlg::InitialUpdate()
{
	m_pSkillPage = GetControl<CEtUIStatic>("ID_STATIC_PAGE");

	char szControlName[32]={0};

	for( int i=0; i<(int)m_vecSkillSlot.size(); i++ )
	{
		sprintf_s( szControlName, 32, "ID_STATIC%d", i+2 );
		m_vecSkillSlot[i].m_pSkillBase = GetControl<CEtUIStatic>(szControlName);

		sprintf_s( szControlName, 32, "ID_NAME_%d", i );
		m_vecSkillSlot[i].m_pSkillName = GetControl<CEtUIStatic>(szControlName);

		sprintf_s( szControlName, 32, "ID_STATIC_BAR%d", i );
		m_vecSkillSlot[i].m_pSkillNameBar = GetControl<CEtUIStatic>(szControlName);
	}

	m_pButtonPagePrev = GetControl<CEtUIButton>("ID_PAGE_PREV");
	m_pButtonPageNext = GetControl<CEtUIButton>("ID_PAGE_NEXT");
	m_pButtonPagePrev->Enable(false);
	m_pButtonPageNext->Enable(false);
}

void CDnInvenGestureDlg::InitCustomControl( CEtUIControl *pControl )
{
	if( typeid(*pControl) != typeid(CDnLifeSkillButton) )
		return;

	CDnLifeSkillButton *pSkillSlotButton(NULL);
	pSkillSlotButton = static_cast<CDnLifeSkillButton*>(pControl);

	pSkillSlotButton->SetSlotType(ST_GESTURE);
	pSkillSlotButton->SetSlotIndex( (int)m_vecSkillSlot.size() );

	SKILL_SLOT skillSlot;
	skillSlot.m_pSlotButton = pSkillSlotButton;
	m_vecSkillSlot.push_back( skillSlot );
}

void CDnInvenGestureDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_nCurrentPage = 1;
		UpdateList();
	}
	else
	{
	}

	CEtUIDialog::Show( bShow );
}

void CDnInvenGestureDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_PAGE_PREV" ) )
		{
			// 여기서 스킬아이콘 집은채로 다음 페이지 눌렀을때의
			// drag::ReleaseControl등의 처리를 해줘야한다.
			drag::ReleaseControl();
			PrevPage();
			return;
		}

		if( IsCmdControl("ID_PAGE_NEXT" ) )
		{
			drag::ReleaseControl();
			NextPage();
			return;
		}

		if( strstr( pControl->GetControlName(), "ID_SKILL" ) )
		{
			CDnSlotButton *pDragButton;
			CDnLifeSkillButton *pPressedButton;

			pDragButton = ( CDnSlotButton * )drag::GetControl();
			pPressedButton = ( CDnLifeSkillButton * )pControl;

			if( pDragButton == NULL )
			{
				if ( !pPressedButton->GetItemID() )
					return;

				if( !pPressedButton->IsUsable() )
					return;


				// 라디오메세지의 경우 스킬창에서 바로 못쓰게 한다.
				// 쿨타임 돌아가는 것도 없으니, 쓰게되면 어색할거다.

				pPressedButton->EnableSplitMode(CDnSlotButton::ITEM_ORIGINAL_COUNT);
				drag::SetControl( pPressedButton );

				// 제스처를 집었을땐 비전투모드로 바꿔주자.
				CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				if( pActor->IsBattleMode() )
				{
					if( !pActor->IsDie() && ( pActor->IsStay() || pActor->IsMove() ) )
					{
						if( pActor->IsMove() ) pActor->CmdStop( "Stand" );
						pActor->CmdToggleBattle(false);
					}
				}

				if( m_nDragSoundIndex != -1 )
					CEtSoundEngine::GetInstance().PlaySound( "2D", m_nDragSoundIndex );
			}
			else
			{
				pDragButton->DisableSplitMode(true);
				drag::ReleaseControl();
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnInvenGestureDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{
			if( IsMouseInDlg() )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
				if( nScrollAmount > 0 )
				{
					PrevPage();
				}
				else if( nScrollAmount < 0 )
				{
					NextPage();
				}
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnInvenGestureDlg::UpdateSkillSlot()
{
	InitSkillSlotList();

	int nVisibleGestureCount = (int)GetGestureTask().GetVisibleGestureCount();
	if( nVisibleGestureCount == 0 )
		return;

	// 보여주기 용으로 정렬
	//struct SkillSort : public binary_function<const DnSkillHandle&, const DnSkillHandle&, bool>
	//{
	//	bool operator () ( const DnSkillHandle& hSkill1, const DnSkillHandle& hSkill2 )
	//	{
	//		return hSkill1->operator < ( hSkill2 );
	//	}
	//};
	//sort( m_vecSkillItem.begin(), m_vecSkillItem.end(), SkillSort() );

	int nStartIndex = (m_nCurrentPage-1)*ITEMSLOT_MAX_SIZE;
	int nEndIndex = m_nCurrentPage*ITEMSLOT_MAX_SIZE;

	SKILL_SLOT *pSkillSlot(NULL);
	CDnSkill *pSkillItem(NULL);
	wchar_t wszTemp[80]={0};

	for( int i=nStartIndex; i<nEndIndex; i++ )
	{
		if( i >= nVisibleGestureCount )
			break;

		CDnGestureTask::SGestureInfo *pInfo;
		pInfo = GetGestureTask().GetVisibleGestureInfoFromIndex(i);
		if( pInfo )
		{
			m_vecSkillSlot[i-nStartIndex].SetSkillSlot( pInfo->nID, pInfo->szName.c_str() );
		}
	}
}

void CDnInvenGestureDlg::UpdateMaxPageNum()
{
	int nSkillListSize = (int)GetGestureTask().GetVisibleGestureCount();
	if( nSkillListSize == 0 )
	{
		m_nMaxPage = 1;
	}
	else
	{
		m_nMaxPage = (nSkillListSize / ITEMSLOT_MAX_SIZE);
		if( (nSkillListSize%ITEMSLOT_MAX_SIZE) )
		{
			m_nMaxPage++;
		}
	}

	UpdateSkillPage();
}

void CDnInvenGestureDlg::UpdateSkillPage()
{
	wchar_t wszTemp[80]={0};
	swprintf_s( wszTemp, 80, L"%d/%d", m_nCurrentPage, m_nMaxPage );
	m_pSkillPage->SetText(wszTemp);
}

void CDnInvenGestureDlg::NextPage()
{
	if( GetGestureTask().GetVisibleGestureCount() == 0 )
		return;

	m_nCurrentPage++;
	if( m_nCurrentPage > m_nMaxPage )
	{
		m_nCurrentPage = m_nMaxPage;
	}

	UpdateList();
}

void CDnInvenGestureDlg::PrevPage()
{
	if( GetGestureTask().GetVisibleGestureCount() == 0 )
		return;

	m_nCurrentPage--;
	if( m_nCurrentPage < 1 )
	{
		m_nCurrentPage = 1;
	}

	UpdateList();
}

void CDnInvenGestureDlg::InitSkillSlotList()
{
	for( int i=0; i<(int)m_vecSkillSlot.size(); i++ )
	{
		m_vecSkillSlot[i].InitSkillSlot();
	}
}

void CDnInvenGestureDlg::ResetAllItem()
{
	InitSkillSlotList();
}

void CDnInvenGestureDlg::UpdatePageButton()
{
	if( m_nCurrentPage == m_nMaxPage )
	{
		m_pButtonPageNext->Enable(false);
	}
	else
	{
		m_pButtonPageNext->Enable(true);
	}

	if( m_nCurrentPage == 1 )
	{
		m_pButtonPagePrev->Enable(false);
	}
	else
	{
		m_pButtonPagePrev->Enable(true);
	}
}

void CDnInvenGestureDlg::UpdateList()
{
	UpdateSkillSlot();

	UpdateMaxPageNum();
	UpdatePageButton();
}