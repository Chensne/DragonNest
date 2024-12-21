#include "StdAfx.h"
#include "DnGuildMarkCreateDlg.h"
#include "DnGuildTask.h"
#include "DnInterface.h"
#include "DnCustomControlCommon.h"
#include "DnTableDB.h"
#include "DnPlayerActor.h"
#include "DnMainFrame.h"
#include "DnGuildMarkInfoDlg.h"
#include "ItemSendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildMarkCreateDlg::CDnGuildMarkCreateDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pGuildMarkItem(NULL)
, m_pGuildMarkInfoDlg(NULL)
, m_pButtonInfo(NULL)
, m_pButtonOK(NULL)
, m_pStaticGuildName(NULL)
, m_pTextureGuildMark0(0)
, m_pTextureGuildMark1(0)
{
	memset(m_sMarkSelectCtrl, 0, sizeof(m_sMarkSelectCtrl));
}

CDnGuildMarkCreateDlg::~CDnGuildMarkCreateDlg(void)
{
	SAFE_DELETE( m_pGuildMarkInfoDlg );
}

void CDnGuildMarkCreateDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildMarkDlg.ui" ).c_str(), bShow );
}

void CDnGuildMarkCreateDlg::InitialUpdate()
{
	m_pTextureGuildMark0 = GetControl<CEtUITextureControl>("ID_TEXTUREL_GUILDMARK0");
	m_pTextureGuildMark1 = GetControl<CEtUITextureControl>("ID_TEXTUREL_GUILDMARK1");
	m_pStaticGuildName = GetControl<CEtUIStatic>("ID_TEXT_GUILDNAME");

	char szControlName[32];
	for( int i = 0; i < NUM_SELECT; ++i )
	{
		sprintf_s(szControlName, _countof(szControlName), "ID_STATIC_EMPTY%d", i);
		m_sMarkSelectCtrl[i].m_pStaticEmpty = GetControl<CEtUIStatic>(szControlName);
		m_sMarkSelectCtrl[i].m_pStaticEmpty->Show( false );

		for( int j = 0; j < NUM_MARK; ++j )
		{
			sprintf_s(szControlName, _countof(szControlName), "ID_STATIC_SLOT%d_%d", i, j);
			m_sMarkSelectCtrl[i].m_pStaticSlot[j] = GetControl<CEtUIStatic>(szControlName);

			sprintf_s(szControlName, _countof(szControlName), "ID_TEXTUREL_MARK%d_%d", i, j);
			m_sMarkSelectCtrl[i].m_pTextureMark[j] = GetControl<CEtUITextureControl>(szControlName);

			sprintf_s(szControlName, _countof(szControlName), "ID_MARKCASH%d_%d", i, j);
			m_sMarkSelectCtrl[i].m_pStaticCash[j] = GetControl<CEtUIStatic>(szControlName);

			sprintf_s(szControlName, _countof(szControlName), "ID_STATIC_SELECT%d_%d", i, j);
			m_sMarkSelectCtrl[i].m_pStaticSelect[j] = GetControl<CEtUIStatic>(szControlName);
		}

		sprintf_s(szControlName, _countof(szControlName), "ID_BT_PRIOR%d", i);
		m_sMarkSelectCtrl[i].m_pButtonPrior = GetControl<CEtUIButton>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_BT_NEXT%d", i);
		m_sMarkSelectCtrl[i].m_pButtonNext = GetControl<CEtUIButton>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_TEXT_PAGE%d", i);
		m_sMarkSelectCtrl[i].m_pStaticPage = GetControl<CEtUIStatic>(szControlName);
	}
	m_sMarkSelectCtrl[0].m_pStaticBoard = GetControl<CEtUIStatic>("ID_STATIC6");
	m_sMarkSelectCtrl[1].m_pStaticBoard = GetControl<CEtUIStatic>("ID_STATIC7");
	m_sMarkSelectCtrl[2].m_pStaticBoard = GetControl<CEtUIStatic>("ID_STATIC8");

	m_pButtonInfo = GetControl<CEtUIButton>("ID_BUTTON0");
	m_pButtonOK = GetControl<CEtUIButton>("ID_BT_OK");

	m_pGuildMarkInfoDlg = new CDnGuildMarkInfoDlg( UI_TYPE_CHILD_MODAL, this );
	m_pGuildMarkInfoDlg->Initialize( false );

	LoadGuildMark();
}

void CDnGuildMarkCreateDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		if( CDnActor::s_hLocalActor )
		{
			CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			WCHAR wszText[128] = {0,};
			swprintf_s( wszText, _countof(wszText), L"<%s>", pPlayerActor->GetGuildSelfView().wszGuildName );
			m_pStaticGuildName->SetText( wszText );
		}
		for( int i = 0; i < NUM_SELECT; ++i )
			m_nCurPage[i] = 0;
		UpdateGuildMarkList();

		// 처음 창 열릴때 편의성으로 기본선택 해준다.
		for( int i = 0; i < NUM_SELECT; ++i )
		{
			if( m_vecMarkInfo[i].empty() ) continue;
			m_sMarkSelectCtrl[i].m_pStaticSelect[0]->Show( true );
			m_nCurSelect[i] = 0;
		}
		OnChangeSelect();
	}
	else
	{
		m_pGuildMarkItem = NULL;
		for( int i = 0; i < NUM_SELECT; ++i )
			m_nCurSelect[i] = -1;
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildMarkCreateDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_OK") )
		{
			// 선택 안했는지 한번 더 검사
			if( m_nCurSelect[0] == -1 && m_nCurSelect[1] == -1 && m_nCurSelect[2] == -1 ) return;

			// 캐시 검사
			if( !m_pGuildMarkItem->IsCashItem() )
			{
				for( int i = 0; i < NUM_SELECT; ++i )
				{
					if( m_nCurSelect[i] != -1 )
					{
						SMarkInfo Info;
						Info = GetMarkInfo( m_sMarkSelectCtrl[i].m_nMarkID[m_nCurSelect[i]] );
						if( Info.nIconIndex && Info.bCash )
						{
							GetInterface().MessageBox( 3987 );
							return;
						}
					}
				}
			}
			GetInterface().MessageBox( 3978, MB_YESNO, 1, this );
			return;
		}
		else if( IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE") )
		{
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_BUTTON0") )
		{
			ShowChildDialog( m_pGuildMarkInfoDlg, true );
			return;
		}

		if( strstr( pControl->GetControlName(), "ID_BT_NEXT" ) )
		{
			for( int i = 0; i < NUM_SELECT; ++i )
			{
				if( m_sMarkSelectCtrl[i].m_pButtonNext == pControl )
				{
					m_nCurPage[i] += 1;
					if( m_nCurPage[i] > m_nMaxPage[i] ) m_nCurPage[i] = m_nMaxPage[i];
					UpdatePage( i );
					UpdateMarkSlot( i );
					return;
				}
			}
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_PRIOR" ) )
		{
			for( int i = 0; i < NUM_SELECT; ++i )
			{
				if( m_sMarkSelectCtrl[i].m_pButtonPrior == pControl )
				{
					m_nCurPage[i] -= 1;
					if( m_nCurPage[i] < 0 ) m_nCurPage[i] = 0;
					UpdatePage( i );
					UpdateMarkSlot( i );
					return;
				}
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildMarkCreateDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		// UICallbackProc 하나밖에 없을테니.. nID검사 생략.
		if( IsCmdControl("ID_YES") )
		{
			// 기본값은 기획자랑 정한 테이블 맨 위에거로 하겠다.
			short wGuildMark = 3;
			short wGuildMarkBG = 1;
			short wGuildMarkBorder = 2;
			if( m_nCurSelect[2] != -1 ) wGuildMark = m_sMarkSelectCtrl[2].m_nMarkID[m_nCurSelect[2]];
			if( m_nCurSelect[0] != -1 ) wGuildMarkBG = m_sMarkSelectCtrl[0].m_nMarkID[m_nCurSelect[0]];
			if( m_nCurSelect[1] != -1 ) wGuildMarkBorder = m_sMarkSelectCtrl[1].m_nMarkID[m_nCurSelect[1]];
			SendGuildMark( m_pGuildMarkItem->GetClassID(), m_pGuildMarkItem->GetSerialID(), m_pGuildMarkItem->GetSlotIndex(), wGuildMark, wGuildMarkBG, wGuildMarkBorder );
			m_pButtonOK->Enable( false );
		}
	}
}

bool CDnGuildMarkCreateDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	bool bRet;
	bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	bool bProc = true;
	if( !m_pGuildMarkInfoDlg ) bProc = false;
	if( m_pGuildMarkInfoDlg && m_pGuildMarkInfoDlg->IsShow() ) bProc = false;
	if( !CDnMouseCursor::GetInstance().IsShowCursor() ) bProc = false;

	if( bProc )
	{
		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );
		PointToFloat( MousePoint, fMouseX, fMouseY );

		switch( uMsg )
		{
		case WM_LBUTTONDOWN:
			{
				SUICoord uiCoord;
				for( int i = 0; i < NUM_SELECT; ++i )
				{
					for( int j = 0; j < NUM_MARK; ++j )
					{
						m_sMarkSelectCtrl[i].m_pTextureMark[j]->GetUICoord(uiCoord);
						if( uiCoord.IsInside(fMouseX, fMouseY) )
						{
							// 마크온리 예외처리.
							SMarkInfo Info;
							Info = GetMarkInfo( m_sMarkSelectCtrl[i].m_nMarkID[j] );
							if( Info.nIconIndex )
							{
								if( i == 2 && Info.bMarkOnly )
								{
									if( m_nCurSelect[0] != -1 || m_nCurSelect[1] != -1 )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3986 ) );
										if( m_nCurSelect[0] != -1 ) { m_sMarkSelectCtrl[0].m_pStaticSelect[m_nCurSelect[0]]->Show( false ); m_nCurSelect[0] = -1; }
										if( m_nCurSelect[1] != -1 ) { m_sMarkSelectCtrl[1].m_pStaticSelect[m_nCurSelect[1]]->Show( false ); m_nCurSelect[1] = -1; }
									}
								}

								if( i == 0 || i == 1 && m_nCurSelect[2] != -1 )
								{
									// 마크온리 선택한 상태에서 배경이나 테두리 선택한거라면, 마크 선택된거를 자동으로 해제한다.
									Info = GetMarkInfo( m_sMarkSelectCtrl[2].m_nMarkID[m_nCurSelect[2]] );
									if( Info.nIconIndex && Info.bMarkOnly ) { m_sMarkSelectCtrl[2].m_pStaticSelect[m_nCurSelect[2]]->Show( false ); m_nCurSelect[2] = -1; }
								}
							}

							if( m_nCurSelect[i] != -1 ) m_sMarkSelectCtrl[i].m_pStaticSelect[m_nCurSelect[i]]->Show( false );
							if( m_sMarkSelectCtrl[i].m_pTextureMark[j]->IsShow() )
							{
								m_sMarkSelectCtrl[i].m_pStaticSelect[j]->Show( true );
								m_nCurSelect[i] = j;
							}
							else
							{
								m_nCurSelect[i] = -1;
							}
							OnChangeSelect();
						}
					}
				}
			}
			break;
		case WM_MOUSEMOVE:
			{
				SUICoord uiCoord;
				for( int i = 0; i < NUM_SELECT; ++i )
				{
					for( int j = 0; j < NUM_MARK; ++j )
					{
						m_sMarkSelectCtrl[i].m_pTextureMark[j]->GetUICoord(uiCoord);
						if( uiCoord.IsInside(fMouseX, fMouseY) )
						{
							SMarkInfo Info;
							Info = GetMarkInfo( m_sMarkSelectCtrl[i].m_nMarkID[j] );
							if( Info.nIconIndex && Info.bMarkOnly && i == 2 )
								ShowTooltipDlg( m_sMarkSelectCtrl[i].m_pTextureMark[j], true, 3988, 0xFFFFFFFF, true );
						}
					}
				}
			}
			break;
		case WM_MOUSEWHEEL:
			{
				SUICoord uiCoord;
				for( int i = 0; i < NUM_SELECT; ++i )
				{
					m_sMarkSelectCtrl[i].m_pStaticBoard->GetUICoord(uiCoord);
					if( uiCoord.IsInside(fMouseX, fMouseY) )
					{
						UINT uLines;
						SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
						int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
						if( nScrollAmount > 0 )
						{
							if( m_nCurPage[i] > 0 )
								ProcessCommand( EVENT_BUTTON_CLICKED, false, m_sMarkSelectCtrl[i].m_pButtonPrior, 0 );
						}
						else if( nScrollAmount < 0 )
						{
							if( m_nCurPage[i] < m_nMaxPage[i] )
								ProcessCommand( EVENT_BUTTON_CLICKED, false, m_sMarkSelectCtrl[i].m_pButtonNext, 0 );
						}
						return true;
					}
				}
			}
			break;
		}
	}

	return bRet;
}

void CDnGuildMarkCreateDlg::SetGuildMarkItem( CDnItem *pItem )
{
	m_pGuildMarkItem = pItem;
}

static bool CompareMarkInfo( CDnGuildMarkCreateDlg::SMarkInfo &Mark1, CDnGuildMarkCreateDlg::SMarkInfo &Mark2 )
{
	if( Mark1.nPriority < Mark2.nPriority ) return true;
	else if( Mark1.nPriority > Mark2.nPriority ) return false;
	return false;
}

void CDnGuildMarkCreateDlg::LoadGuildMark()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGUILDMARK );

	int nNumItem = pSox->GetItemCount();
	for( int nItem = 0; nItem < nNumItem; ++nItem )
	{
		int nItemID = pSox->GetItemID( nItem );
		int nType = pSox->GetFieldFromLablePtr( nItemID, "_Type" )->GetInteger();
		if( nType < 0 || nType > 2 ) continue;
#ifdef PRE_ADD_BEGINNERGUILD
		eMarkViewType markViewType = ShowForUserCreate;
		CDNTableFile::Cell* pCell = pSox->GetFieldFromLablePtr( nItemID, "_MarkView" );
		if (pCell)
		{
			markViewType = (eMarkViewType)pCell->GetInteger();
			if (markViewType == NoUse)
				continue;
		}
#else
		int markView = pSox->GetFieldFromLablePtr( nItemID, "_MarkView" )->GetInteger();
		if (markView != 1)
			continue;
#endif

		SMarkInfo MarkInfo;
		MarkInfo.nMarkID = nItemID;
		MarkInfo.nIconIndex = pSox->GetFieldFromLablePtr( nItemID, "_IconID" )->GetInteger();
		MarkInfo.nPriority = pSox->GetFieldFromLablePtr( nItemID, "_Priority" )->GetInteger();
		MarkInfo.bMarkOnly = ( pSox->GetFieldFromLablePtr( nItemID, "_MarkOnly" )->GetInteger() == TRUE ) ? true : false;
		MarkInfo.bCash = ( pSox->GetFieldFromLablePtr( nItemID, "_IsCash" )->GetInteger() == TRUE ) ? true : false;
#ifdef PRE_ADD_BEGINNERGUILD
		MarkInfo.markViewType = markViewType;
#endif

		m_vecMarkInfo[nType].push_back( MarkInfo );
	}
	for( int i = 0; i < NUM_SELECT; ++i )
	{
		std::sort( m_vecMarkInfo[i].begin(), m_vecMarkInfo[i].end(), CompareMarkInfo );

#ifdef PRE_ADD_BEGINNERGUILD
		int enableCount = GetCreateEnableMarkCount(i);
		if (enableCount > 0)
			m_nMaxPage[i] = enableCount / NUM_MARK;
		else
			m_nMaxPage[i] = 0;
#else
		m_nMaxPage[i] = ((int)m_vecMarkInfo[i].size()-1) / NUM_MARK;
#endif
	}
}

#ifdef PRE_ADD_BEGINNERGUILD
int CDnGuildMarkCreateDlg::GetCreateEnableMarkCount(int type)
{
	if (type < 0 || type >= NUM_SELECT)
		return -1;

	int i = 0, count = 0;
	for (; i < (int)m_vecMarkInfo[type].size(); ++i)
	{
		const SMarkInfo& info = m_vecMarkInfo[type][i];
		if (info.markViewType == ShowForUserCreate)
			count++;
	}

	return count;
}
#endif

void CDnGuildMarkCreateDlg::UpdateGuildMarkList()
{
	for( int i = 0; i < NUM_SELECT; ++i )
	{
		UpdatePage( i );
		UpdateMarkSlot( i );
	}
}

void CDnGuildMarkCreateDlg::UpdatePage( int nIndex )
{
	wchar_t wszPage[10]={0};
	swprintf_s( wszPage, 10, L"%d/%d", m_nCurPage[nIndex]+1, m_nMaxPage[nIndex]+1 );
	m_sMarkSelectCtrl[nIndex].m_pStaticPage->SetText( wszPage );

	if( m_nCurPage[nIndex] == m_nMaxPage[nIndex] )
		m_sMarkSelectCtrl[nIndex].m_pButtonNext->Enable(false);
	else
		m_sMarkSelectCtrl[nIndex].m_pButtonNext->Enable(true);

	if( m_nCurPage[nIndex] == 0 )
		m_sMarkSelectCtrl[nIndex].m_pButtonPrior->Enable(false);
	else
		m_sMarkSelectCtrl[nIndex].m_pButtonPrior->Enable(true);
}

void CDnGuildMarkCreateDlg::UpdateMarkSlot( int nIndex )
{
	m_sMarkSelectCtrl[nIndex].ClearSlot();

	int nOffset = m_nCurPage[nIndex] * NUM_MARK;
	for( int i = nOffset; i < (int)m_vecMarkInfo[nIndex].size(); ++i )
	{
		if( i >= nOffset + NUM_MARK )
			break;

		SMarkInfo Info = m_vecMarkInfo[nIndex][i];
#ifdef PRE_ADD_BEGINNERGUILD
		if (Info.markViewType != ShowForUserCreate)
			continue;
#endif

		EtTextureHandle hIconTexture;
		int nTexIndex = Info.nIconIndex / GUILDMARK_TEXTURE_ICON_COUNT;
		hIconTexture = GetInterface().GetGuildMarkIconTex( nTexIndex );
		if( hIconTexture )
		{
			int nX = 0, nY = 0;
			int nIconIndex = Info.nIconIndex % GUILDMARK_TEXTURE_ICON_COUNT;
			CalcButtonUV( nIconIndex, hIconTexture, nX, nY, GUILDMARK_ICON_XSIZE, GUILDMARK_ICON_YSIZE );
			m_sMarkSelectCtrl[nIndex].m_pTextureMark[i-nOffset]->SetTexture( hIconTexture, nX, nY, GUILDMARK_ICON_XSIZE, GUILDMARK_ICON_YSIZE );
			m_sMarkSelectCtrl[nIndex].m_pTextureMark[i-nOffset]->Show( true );
			m_sMarkSelectCtrl[nIndex].m_nMarkID[i-nOffset] = Info.nMarkID;
			if( Info.bCash ) m_sMarkSelectCtrl[nIndex].m_pStaticCash[i-nOffset]->Show( true );
			m_sMarkSelectCtrl[nIndex].m_nIconIndex[i-nOffset] = Info.nIconIndex;
		}

		// 기획자분이랑 미리 맞춰둔 규약이다. 첫번째 자리에 빈슬롯이 있으면 표시해주기.
		if( nOffset == 0 && i == 0 && Info.nIconIndex == 0 )
			m_sMarkSelectCtrl[nIndex].m_pStaticEmpty->Show( true );
	}

	// 슬롯이 업데이트 되면, 선택란도 초기화한다.
	m_nCurSelect[nIndex] = -1;
	OnChangeSelect();
}

void CDnGuildMarkCreateDlg::OnChangeSelect()
{
	bool bOK = false;
	for( int i = 0; i < NUM_SELECT; ++i )
	{
		if( m_sMarkSelectCtrl[i].m_nIconIndex[0] == 0 )
		{
			if( m_nCurSelect[i] != -1 && m_nCurSelect[i] != 0 )
				bOK = true;
		}
		else if( m_nCurSelect[i] != -1 )
			bOK = true;
	}
	m_pButtonOK->Enable( bOK );
	UpdateGuildMark();
}

void CDnGuildMarkCreateDlg::UpdateGuildMark()
{
	if( !CDnGuildTask::IsActive() ) return;
	int n1 = (m_nCurSelect[0] != -1) ? m_sMarkSelectCtrl[0].m_nMarkID[m_nCurSelect[0]] : 0;
	int n2 = (m_nCurSelect[1] != -1) ? m_sMarkSelectCtrl[1].m_nMarkID[m_nCurSelect[1]] : 0;
	int n3 = (m_nCurSelect[2] != -1) ? m_sMarkSelectCtrl[2].m_nMarkID[m_nCurSelect[2]] : 0;
	TGuildSelfView View;
	View.wGuildMarkBG = n1;
	View.wGuildMarkBorder = n2;
	View.wGuildMark = n3;
	EtTextureHandle hGuildMark = GetGuildTask().GetGuildMarkTexture( View );
	if( hGuildMark )
	{
		m_pTextureGuildMark0->SetTexture( hGuildMark, 0, 0, hGuildMark->Width(), hGuildMark->Height() );
		m_pTextureGuildMark1->SetTexture( hGuildMark, 0, 0, hGuildMark->Width(), hGuildMark->Height() );
	}
}

CDnGuildMarkCreateDlg::SMarkInfo CDnGuildMarkCreateDlg::GetMarkInfo( int nMarkID )
{
	// IconIndex로 구하는거라 배경, 테두리, 마크 vector의 제일 처음값들은 다 겹치게 된다.(0번 IconIndex를 가지고 있으니..)
	// 그러나, 이 함수 자체가 0번 IconIndex를 구하는 용도로 사용되지 않을거라서 그냥 넘어가기로 한다.
	SMarkInfo Info;
	memset( &Info, 0, sizeof(SMarkInfo) );
	for( int i = 0; i < NUM_SELECT; ++i )
	{
		for( int j = 0; j < (int)m_vecMarkInfo[i].size(); ++j )
		{
			if( m_vecMarkInfo[i][j].nMarkID == nMarkID )
			{
				Info = m_vecMarkInfo[i][j];
				return Info;
			}
		}
	}
	return Info;
}

void CDnGuildMarkCreateDlg::OnRecvGuildMark( short wGuildMarkBG, short wGuildMarkBorder, short wGuildMark )
{
	if( CDnActor::s_hLocalActor )
	{
		// 생성 결과 메세지는 CDnGuildTask::OnRecvChangeGuildMark 여기서 처리하니까, 이펙트만 보여주고 끝낸다.
		if( !m_hGuildMarkCreateEffect )
		{
			m_hGuildMarkCreateEffect = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
			if( m_hGuildMarkCreateEffect )
			{
				m_hGuildMarkCreateEffect->SetPosition( *CDnActor::s_hLocalActor->GetPosition() );
				m_hGuildMarkCreateEffect->SetActionQueue( "MissionAchieve" );
			}
		}
	}
	Show( false );
}