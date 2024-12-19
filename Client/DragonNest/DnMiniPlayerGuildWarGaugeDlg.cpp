#include "stdafx.h"
#include "DnMiniPlayerGuildWarGaugeDlg.h"
#include "DnInterface.h"
#include "DnBuffUIMng.h"
#include "DnGuildWarSituationMng.h"
#include "TaskManager.h"
#include "DnPvPGameTask.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnMiniPlayerGuildWarGaugeDlg::CDnMiniPlayerGuildWarGaugeDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
, m_pBuffUIMng( NULL )
, m_pGuildWarSituationMng( NULL )
, m_uiUserState( 0 )
, m_bSelect( false )
, m_bSelectMode( false )
, m_bUser( false )
, m_pHP(NULL)
, m_pMP(NULL)
, m_pStaticBase(NULL)
, m_pStaticBlueMaster(NULL)
, m_pStaticName(NULL)
, m_pStaticRedMaster(NULL)
, m_pStaticSelectBar(NULL)
, m_pTextureClass(NULL)
, m_pTextureRank(NULL)
, m_pStaticSubMaster(NULL)
{
	for( int itr =0; itr < eMax_Icon; ++itr )
		m_pIconPosition[itr] = itr;
}

CDnMiniPlayerGuildWarGaugeDlg::~CDnMiniPlayerGuildWarGaugeDlg()
{
	for( int i=0; i<BUFF_TEXTURE_COUNT; i++ )
	{
		SAFE_RELEASE_SPTR( m_hSkillIconList[i] );
	}

	SAFE_DELETE(m_pBuffUIMng);
}

void CDnMiniPlayerGuildWarGaugeDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MiniPlayerGuildWarGauge.ui" ).c_str(), bShow );
}

void CDnMiniPlayerGuildWarGaugeDlg::InitialUpdate()
{
	m_pStaticBase = GetControl<CEtUIStatic>( "ID_STATIC_BASE0" );

	m_pStaticName = GetControl<CEtUIStatic>( "ID_PLAYER_NAME" );
	m_pStaticRedMaster = GetControl<CEtUIStatic>( "ID_STATIC_REDMASTER" );
	m_pStaticBlueMaster = GetControl<CEtUIStatic>( "ID_STATIC_BLUEMASTER" );
	m_pStaticSubMaster = GetControl<CEtUIStatic>( "ID_STATIC_SUBMASTER" );;
	m_pStaticSelectBar = GetControl<CEtUIStatic>( "ID_STATIC_BASE2" );;

	m_pStaticRedMaster->Show( false );
	m_pStaticBlueMaster->Show( false );
	m_pStaticSubMaster->Show( false );
	m_pStaticSelectBar->Show( false );

	m_pHP = GetControl<CEtUIProgressBar>( "ID_HP_GAUGE" );
	m_pMP = GetControl<CEtUIProgressBar>( "ID_MP_GAUGE" );
	m_pHP->SetProgress( 0.0f );
	m_pMP->SetProgress( 0.0f );

	m_pTextureRank = GetControl<CEtUITextureControl>( "ID_TEXTUREL_RANK" );
	m_pTextureClass = GetControl<CDnJobIconStatic>( "ID_STATIC_CLASS" );

	char szControlName[256];
	for( int itr = 0; itr < MAX_BUFF_SLOT; ++itr )
	{
		sprintf_s( szControlName, _countof(szControlName), "ID_BUFF_SLOT%d", itr );
		m_pBuffTextureCtrl[itr] = GetControl<CEtUITextureControl>( szControlName );

		m_pIconCoord[eBuff_Icon1 + itr] = m_pBuffTextureCtrl[itr]->GetBaseUICoord();
	}

	m_hSkillIconList[0] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "BuffIcon01.dds" ).c_str(), RT_TEXTURE );

	m_pBuffUIMng = new CDnBuffUIMng(MAX_BUFF_ONLY_SLOT, MAX_BUBBLE_ONLY_SLOT, CDnBuffUIMng::ePartyMember);
}

void CDnMiniPlayerGuildWarGaugeDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnMiniPlayerGuildWarGaugeDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnMiniPlayerGuildWarGaugeDlg::Render( float fElapsedTime )
{
	if( GetInterface().IsOpenBlind() )
		return;

	if( IsShow() )
	{
		RefreshIcon();		
	}

	CEtUIDialog::Render( fElapsedTime );
}

void CDnMiniPlayerGuildWarGaugeDlg::SetUser( CDnPartyTask::PartyStruct * pStruct )
{
	m_bUser = true;
	m_hActor = pStruct->hActor;
	m_cTeam = pStruct->usTeam;
	SetUserState( pStruct->uiPvPUserState );

	int iIconW,iIconH;
	int iU,iV;
	iIconW = GetInterface().GeticonWidth();
	iIconH = GetInterface().GeticonHeight();
	if( GetInterface().ConvertPVPGradeToUV( pStruct->cPvPLevel, iU, iV ))
	{
		m_pTextureRank->SetTexture(GetInterface().GetPVPIconTex(), iU, iV, iIconW, iIconH);
		m_pTextureRank->Show(true);
	}

	BYTE nJob = pStruct->cClassID;
	for( int itr = 0; itr < JOBMAX; ++itr )
	{
		if( 0 == pStruct->cJobArray[itr] )	break;
		nJob = pStruct->cJobArray[itr];
	}

	m_pTextureClass->SetIconID( nJob, true );
	m_pTextureClass->Show( true );

	m_pStaticName->SetText( pStruct->wszCharacterName );
	m_pStaticName->Show( true );
}

void CDnMiniPlayerGuildWarGaugeDlg::SetUserState( UINT uiUserState )
{
	m_uiUserState = uiUserState;

	m_pStaticBlueMaster->Show( false );
	m_pStaticRedMaster->Show( false );
	m_pStaticSubMaster->Show( false );

	if( PvPCommon::UserState::GuildWarCaptain & m_uiUserState )
	{
		if( PvPCommon::Team::A == m_cTeam )
			m_pStaticBlueMaster->Show( true );
		else 
			m_pStaticRedMaster->Show( true );
	}
	else if( PvPCommon::UserState::GuildWarSedcondCaptain & m_uiUserState )
		m_pStaticSubMaster->Show( true );
}

void CDnMiniPlayerGuildWarGaugeDlg::RefreshIcon()
{
	int nIconIndex = 0;

	for( int itr = 0; itr < MAX_BUFF_SLOT; ++itr )
	{
		if( m_pBuffTextureCtrl[itr]->IsShow() )
		{
			if( nIconIndex != m_pIconPosition[eBuff_Icon1 + itr] )
			{
				m_pBuffTextureCtrl[itr]->SetPosition( m_pIconCoord[nIconIndex].fX, m_pBuffTextureCtrl[itr]->GetBaseUICoord().fY );
				m_pIconPosition[eBuff_Icon1 + itr] = nIconIndex;
			}
			++nIconIndex;
		}
	}
}

void CDnMiniPlayerGuildWarGaugeDlg::ProcessGauge()
{
	if( !m_bUser || !m_hActor )
		return;

	ProcessBuffs();

	float fHP, fSP;
	if( m_hActor->GetHP() == 0 )
		fHP = 0.0f;
	else
		fHP = m_hActor->GetHP() / (float)m_hActor->GetMaxHP() * 100.0f;

	if( m_hActor->GetSP() == 0 )
		fSP = 0.0f;
	else
		fSP = m_hActor->GetSP() / (float)m_hActor->GetMaxSP() * 100.0f;

	m_pHP->SetProgress( fHP );
	m_pMP->SetProgress( fSP );
}	

void CDnMiniPlayerGuildWarGaugeDlg::ProcessBuffs()
{
	if( !m_bUser || !m_hActor )
		return;

	if (m_pBuffUIMng == NULL)
	{
		_ASSERT(0);
		return;
	}

	int iNumAppliedStateBlow = m_hActor->GetNumAppliedStateBlow();
	if (m_hActor->IsDie() || iNumAppliedStateBlow <= 0)
	{
		m_pBuffUIMng->ClearLists();

		int i = 0;
		for (; i < MAX_BUFF_SLOT; ++i)
			m_pBuffTextureCtrl[i]->Show(false);
	}

	m_pBuffUIMng->Process(m_hActor);

	int j = 0;
	for (; j < MAX_BUFF_SLOT; ++j)
		m_pBuffTextureCtrl[j]->Show(false);

	int slotIdx = 0;

	CDnBuffUIMng::BUFFLIST& buffSlotList = m_pBuffUIMng->GetBuffSlotList();
	CDnBuffUIMng::BUFFLIST::iterator iter = buffSlotList.begin();
	CDnBuffUIMng::BUFFLIST::iterator nextIter;
	for (; iter != buffSlotList.end();)
	{
		CDnBuffUIMng::SBuffUnit& unit = *iter;
		nextIter = iter;
		++nextIter;

		if (unit.bUpdated)
		{
			if (unit.texturePageIdx >= BUFF_TEXTURE_COUNT)
			{
				_ASSERT(0);
				return;
			}

			m_pBuffTextureCtrl[slotIdx]->SetTexture(m_hSkillIconList[unit.texturePageIdx], int(unit.textureUV.fX), 
				int(unit.textureUV.fY), int(unit.textureUV.fWidth), int(unit.textureUV.fHeight));
			m_pBuffTextureCtrl[slotIdx]->Show(true);

			unit.bUpdated = false;
			++slotIdx;
		}
		else
		{
			buffSlotList.erase(iter);
			m_pBuffTextureCtrl[slotIdx]->Show(false);
		}

		iter = nextIter;
	}
}

void CDnMiniPlayerGuildWarGaugeDlg::SetGuildWarSituationMng( CDnGuildWarSituationMng * pGuildWarSituationMng )
{
	m_pGuildWarSituationMng = pGuildWarSituationMng;
}

bool CDnMiniPlayerGuildWarGaugeDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_LBUTTONDOWN :
		{
			if( m_bSelectMode )
			{
				POINT MousePoint;
				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );
				float fMouseX, fMouseY;
				fMouseX = MousePoint.x / GetScreenWidth();
				fMouseY = MousePoint.y / GetScreenHeight();

				SUICoord uiCoord;
				GetDlgCoord( uiCoord );

				if( uiCoord.IsInside( fMouseX, fMouseY ) )
				{
					m_bSelect = !m_bSelect;
					m_pStaticSelectBar->Show( m_bSelect );

					m_pGuildWarSituationMng->SelectUser( this );
				}
			}
		}
		break;
	case WM_RBUTTONDOWN :
		{
			if( !m_bUser )
				break;

			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			float fMouseX, fMouseY;
			fMouseX = MousePoint.x / GetScreenWidth();
			fMouseY = MousePoint.y / GetScreenHeight();

			SUICoord uiCoord;
			GetDlgCoord( uiCoord );

			if( uiCoord.IsInside( fMouseX, fMouseY ) )
			{
				m_pGuildWarSituationMng->ShowPopupDlg( fMouseX, fMouseY, m_uiUserState, m_hActor->GetUniqueID(), true );
			}
		}
		break;
	}

	return bRet;
}

void CDnMiniPlayerGuildWarGaugeDlg::SetSelectMode( bool bSelectMode )
{
	m_bSelectMode = bSelectMode;

	if( !m_bSelectMode )
	{
		m_bSelect = false;
		m_pStaticSelectBar->Show( false );
	}
}

void CDnMiniPlayerGuildWarGaugeDlg::Select( bool bSelect )
{
	if( m_bSelectMode )
	{
		m_bSelect = bSelect;
		m_pStaticSelectBar->Show( bSelect );
	}
}

void CDnMiniPlayerGuildWarGaugeDlg::ClearUser()
{
	m_pBuffUIMng->ClearLists();

	for(int i = 0; i < MAX_BUFF_SLOT; ++i)
		m_pBuffTextureCtrl[i]->Show(false);

	m_cTeam = 0;
	m_uiUserState = 0;
	m_bSelect = false;
	m_bUser = false;

	m_pTextureRank->Show( false );
	m_pTextureClass->Show( false );
	m_pStaticName->Show( false );

	m_pHP->SetProgress( 0.f );
	m_pMP->SetProgress( 0.f );

	SetUserState( m_uiUserState );
}

void CDnMiniPlayerGuildWarGaugeDlg::SetPartyShow()
{
	m_pStaticBase->Show( false );
}