#include "stdafx.h"
#include "DnAlteaBoardDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnTableDB.h"
#include "SystemSendPacket.h"
#include "DnItemTask.h"
#include "DnWorld.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined( PRE_ALTEIAWORLD_EXPLORE )

#define RENDER_TARGET_TEXTURE_SIZE	1024
#define PLAYER_ICON_MOVE_DELAY		0.8f
#define DUNGEON_ENTER_DELAY			3
#define DICE_ACTION_TIME			3
#define QUIT_ALTEA_BOARD			10001

CDnAlteaBoardDlg::CDnAlteaBoardDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
, m_pKeyCount_Static( NULL )
, m_pNotice_Static( NULL )
, m_pAutoDiceCount_Static( NULL )
, m_pDice_Button( NULL )
, m_pClose_Button( NULL )
, m_pDrawFrameBtn( NULL )
, m_nNowPosition( 0 )
, m_nTargetPosition( 0 )
, m_nMoveSound( 0 )
, m_nDiceSound( 0 )
, m_nBGM( 0 )
, m_bProcessIcon( false )
, m_bProcessDice( false )
, m_bProcessEnter( false )
, m_bAutoDice( false)
, m_bReadyDice( false )
, m_fIconTime( 0.0f )
, m_fDiceTime( 0.0f )
, m_fEnterTime( 0.0f )
, m_fAutoDiceTime( 0.0f )
{

}

CDnAlteaBoardDlg::~CDnAlteaBoardDlg(void)
{
	SAFE_RELEASE_SPTR( m_hClassTexture );
	SAFE_RELEASE_SPTR( m_hBgTexture );

	CEtSoundEngine::GetInstance().RemoveSound( m_nMoveSound );
	CEtSoundEngine::GetInstance().RemoveSound( m_nDiceSound );

	SAFE_RELEASE_SPTR(m_hBGM);
	CEtSoundEngine::GetInstance().RemoveSound( m_nBGM );
}

void CDnAlteaBoardDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AlteaBoardDlg.ui" ).c_str(), bShow );
}

void CDnAlteaBoardDlg::InitialUpdate()
{
	m_nMoveSound = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( "UI_Alteia_ShipMove.ogg" ).c_str(), false, false );
	m_nDiceSound = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( "UI_Alteia_DiceRolling.ogg" ).c_str(), false, false );
	m_nBGM = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( "DRAGON NEST_BGM_Field_Alteia.ogg" ).c_str(), false, false );

	m_hClassTexture = LoadResource("AlteaClassMark.dds", RT_TEXTURE, true);
	m_hBgTexture = LoadResource("AlteaBG.dds", RT_TEXTURE, true);

	char szName[64] = {0,};
	for( int itr = 0; itr < E_ALTEA_BOARD_COUNT; ++itr )
	{
		sprintf_s( szName, "ID_TEXTUREL_CLASS%d", itr );
		m_pSlotData[itr].m_pClass_TC = GetControl<CEtUITextureControl>( szName );

		sprintf_s( szName, "ID_RBT_MAP%d", itr );
		m_pSlotData[itr].m_pBaseButton = GetControl<CEtUIRadioButton>( szName );

		int nElementCount = m_pSlotData[itr].m_pBaseButton->GetElementCount();
		for( int element_itr = 0; element_itr < nElementCount; ++element_itr )
		{
			SUIElement * pElement = m_pSlotData[itr].m_pBaseButton->GetElement( element_itr );
			pElement->TextureColor.dwColor[UI_STATE_DISABLED] = pElement->TextureColor.dwColor[UI_STATE_NORMAL];
		}
	}

	m_pKeyCount_Static = GetControl<CEtUIStatic>( "ID_TEXT_KEYCOUNT" );
	m_pNotice_Static = GetControl<CEtUIStatic>( "ID_TEXT_NOTICE" );
	m_pAutoDiceCount_Static = GetControl<CEtUIStatic>( "ID_TEXT_COUNT" );

	m_pDice_Button = GetControl<CEtUIButton>( "ID_BT_DICE" );
	m_pClose_Button = GetControl<CEtUIButton>( "ID_BT_CLOSE" );
	m_pDrawFrameBtn = GetControl<CEtUIButton>( "ID_BT_DICEPREVIEW" );

	Initialize_Board();
	Initialize_RenderEtc();
}

void CDnAlteaBoardDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( true == bShow )
	{
		m_pDice_Button->Enable( true );
		m_pDice_Button->SetBlink( true );
		m_pClose_Button->Enable( true );
		m_pNotice_Static->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8417) );

		m_bProcessIcon = false;
		m_bProcessDice = false;
		m_bProcessEnter = false;
		m_bAutoDice = true;
		m_bReadyDice = true;

		Initialize_Dice();
		SetGoldKeyCount();
		SetPlayerIcon();
		SetAutoDiceCount();

		SetSound( true );
	}
	else
		SetSound( false );

	CDnCustomDlg::Show( bShow );
}

void CDnAlteaBoardDlg::Process( float fElapsedTime )
{
	if( false == CDnMouseCursor::GetInstance().IsShowCursor() )
		CDnMouseCursor::GetInstance().ShowCursor( m_bShow );

	if( false == CDnLocalPlayerActor::IsLockInput() )
		CDnLocalPlayerActor::LockInput( m_bShow );

	if( m_bShow == false )
		return;

	m_RenderEtc.Process( fElapsedTime );

	ProcessAutoDiceCount( fElapsedTime );
	ProcessDice( fElapsedTime );
	ProcessPlayerIcon( fElapsedTime );
	ProcessEnter( fElapsedTime );

	CDnCustomDlg::Process( fElapsedTime );
}

void CDnAlteaBoardDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_CLOSE" ) )
		{
			GetInterface().MessageBox( 8416, MB_YESNO, QUIT_ALTEA_BOARD, this );
		}
		else if( IsCmdControl("ID_BT_DICE" ) )
		{
			GetAlteaTask().RequestAlteaDice();
		}
		else if( IsCmdControl("ID_BT_DICEPREVIEW" ) )
			focus::ReleaseControl();
	}
	if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		if( true == bTriggeredByUser )
			SelectSlot( pControl->GetControlName() );
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnAlteaBoardDlg::Render( float fElapsedTime )
{
	if( false == m_bShow )
		return;

	if( m_hBgTexture)
	{
		SUICoord ScreenCoord( 0.0f, 0.0f, 1.0f, 1.0f );
		SUICoord UVCoord;

		int nWidth = CEtDevice::GetInstance().Width();
		int nHeight = CEtDevice::GetInstance().Height();

		float fTemp = ( ( nHeight / 9.f ) * 16.f - nWidth ) / nWidth;
		UVCoord.fX = fTemp / 2.0f;
		UVCoord.fY = 0.0f;
		UVCoord.fWidth = 1.0f - fTemp;
		UVCoord.fHeight = 1.0f;

		CEtSprite::GetInstance().DrawSprite( (EtTexture *)m_hBgTexture->GetTexturePtr(),
			m_hBgTexture->Width(), m_hBgTexture->Height(), UVCoord, 0xFFFFFFFF, ScreenCoord, 0.0f );
	}

	CDnCustomDlg::Render( fElapsedTime );

	if( true == m_bReadyDice )
		return;

	if( 0.0f < m_fReadyDice )
	{
		m_fReadyDice -= fElapsedTime;
		return;
	}

	SUICoord diceViewCoord;
	diceViewCoord = m_pDrawFrameBtn->GetProperty()->UICoord;
	DrawSprite( m_RenderEtc.GetRTT(), m_RenderEtc.GetUVCoord(), 0xFFFFFFFF, diceViewCoord );
}

void CDnAlteaBoardDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	switch( nID ) 
	{
		case QUIT_ALTEA_BOARD:
			{
				if( nCommand == EVENT_BUTTON_CLICKED )
				{
					if( strcmp( pControl->GetControlName(), "ID_YES" ) == 0 ) 
						SendAbandonStage(false, false);
				}
			}
			break;
	}
}

void CDnAlteaBoardDlg::Initialize_RenderEtc()
{
	SUICoord diceViewCoord;
	diceViewCoord = m_pDrawFrameBtn->GetProperty()->UICoord;

	SCameraInfo CameraInfo;
	CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	CameraInfo.fNear = 10.f;
	CameraInfo.Type = CT_ORTHOGONAL;

	m_RenderEtc.Initialize( RENDER_TARGET_TEXTURE_SIZE, RENDER_TARGET_TEXTURE_SIZE, CameraInfo, 185, 192, FMT_A8R8G8B8, true, true );
	m_RenderEtc.CalcUVCoord( diceViewCoord.fWidth, diceViewCoord.fHeight );
	m_RenderEtc.CameraLookAt( EtVector3(100.0f, 250.0f, 100.0f), EtVector3(0.0f, 0.0f, 0.0f), EtVector3(0.0f, 1.0f, 0.0f) );
}

void CDnAlteaBoardDlg::Initialize_Dice()
{
	m_RenderEtc.ResetEtc( false );
	m_RenderEtc.SetEtc( "Boarddice.skn", "Boarddice.ani", "Boarddice.act" );

	DnEtcHandle hEtcObject = m_RenderEtc.GetEtc();
	if( !hEtcObject )
		return;

	CDnEtcObject * pObject = hEtcObject.GetPointer();
	if( NULL == pObject )
		return;

	EtObjectHandle hObject = pObject->GetObjectHandle();
	if( CEtObject::Identity() == hObject )
		return;

	for( int itr = 0; itr < hObject->GetSubMeshCount(); ++itr )
	{
		std::string szSubMesh = hObject->GetSubMeshName( itr );

		ToLowerA( szSubMesh );
		if( strstr( szSubMesh.c_str(), "face" ) )
			m_vecDiceSubMeshIndex.push_back( itr );
	}

	int nDiceNumber = 0;
	const int nMaterialCount = hObject->GetMaterialCount();
	for( int itr = 0; itr < nMaterialCount; ++itr )
	{
		EtMaterialHandle hMaterial = hObject->GetMaterial( itr );

		if( !hMaterial || 0 == strstr( hMaterial->GetFileName(), "emissive" ) || CDnAlteaTask::E_DICE_MAXNUMBER <= nDiceNumber )
			continue;

		int nTextureIndex = 0, nEmissiveIndex = 0;
		EtParameterHandle hParamDiffuseTex = hMaterial->GetParameterByName( "g_DiffuseTex" );
		EtParameterHandle hParamEmissiveTex = hMaterial->GetParameterByName( "g_EmissiveTex" );

		const int nCustomParamCount = hMaterial->GetCustomParamCount();
		for( int jtr = 0; jtr < nCustomParamCount; ++jtr )
		{
			SCustomParam * pParam = hObject->GetCustomParam( itr, jtr );

			if( pParam->hParamHandle == hParamDiffuseTex )
				nTextureIndex = pParam->nTextureIndex;
			else if( pParam->hParamHandle == hParamEmissiveTex )
				nEmissiveIndex = pParam->nTextureIndex;
		}

		m_DiceTextureIndex[nDiceNumber] = std::make_pair( nTextureIndex, nEmissiveIndex );
		++nDiceNumber;
	}
}

void CDnAlteaBoardDlg::Initialize_Board()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TALTEIAWORLDMAP );
	if( NULL == pSox )
		return;

	int nTableCount = pSox->GetItemCount();
	for( int itr = 1; itr < nTableCount; ++itr )
	{
		int nItemID = pSox->GetItemID( itr );

		int nMapType = pSox->GetFieldFromLablePtr( nItemID, "_MapType" )->GetInteger();

		if( CDnAlteaTask::E_MAPTYPE_DUNGEON != nMapType )
			continue;

		int nMapID = pSox->GetFieldFromLablePtr( nItemID, "_MapID" )->GetInteger();
		int nMapName = pSox->GetFieldFromLablePtr( nItemID, "_MapName" )->GetInteger();
		int nMissionName = pSox->GetFieldFromLablePtr( nItemID, "_MissionName" )->GetInteger();

		m_pSlotData[itr].m_nMapIndex = nMapID;
		m_pSlotData[itr].m_wszMapName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nMapName);
		m_pSlotData[itr].m_wszMissionName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nMissionName);
	}
}

void CDnAlteaBoardDlg::SetDiceResult( const int nDice )
{
	m_nTargetPosition += nDice;

	m_bProcessEnter = false;
	m_bProcessIcon = false;
	m_bProcessDice = true;
	m_bAutoDice = false;
	m_bReadyDice = false;
	m_fIconTime = 0.0f;
	m_fDiceTime = 0.0f;
	m_fReadyDice = 0.5f;

	SetDiceObject( nDice );
	DnEtcHandle hEtc = m_RenderEtc.GetEtc();
	if( hEtc )
		hEtc->SetActionQueue( "Action" );

	SetPlayerIcon();
	m_pNotice_Static->ClearText();
	m_pDice_Button->Enable( false );
	m_pClose_Button->Enable( false );
	m_pAutoDiceCount_Static->Show( false );

	CEtSoundEngine::GetInstance().PlaySound__( "2D", m_nDiceSound );
}

void CDnAlteaBoardDlg::SelectSlot( const char * szControlName )
{
	m_pNotice_Static->ClearText();

	if( NULL == szControlName )
		return;

	std::string str( szControlName );
	std::string strNumber = str.substr( strlen("ID_RBT_MAP"), 2 );
	int nArrayIndex = atoi( strNumber.c_str() );

	if( 0 > nArrayIndex || E_ALTEA_BOARD_COUNT <= nArrayIndex )
		return;

	m_pNotice_Static->SetText( m_pSlotData[nArrayIndex].m_wszMapName.c_str() );
}

void CDnAlteaBoardDlg::SetPlayerIcon()
{
	if( 0 > m_nNowPosition || E_ALTEA_BOARD_COUNT < m_nNowPosition )
		return;

	bool bButtonEnable = ( true == m_bProcessDice || true == m_bProcessIcon || true == m_bProcessEnter );

	for( int itr = 0; itr < E_ALTEA_BOARD_COUNT; ++itr )
	{
		m_pSlotData[itr].m_pClass_TC->Show( false );
		m_pSlotData[itr].m_pBaseButton->Enable( !bButtonEnable );
	}

	CEtUITextureControl * pControl = m_pSlotData[m_nNowPosition].m_pClass_TC;
	pControl->SetTexture( m_hClassTexture, 0, 0, 64, 64 );
	pControl->Show( true );

	m_pSlotData[m_nNowPosition].m_pBaseButton->Enable( true );
	m_pSlotData[m_nNowPosition].m_pBaseButton->SetChecked( true );
}

void CDnAlteaBoardDlg::ProcessPlayerIcon( float fElapsedTime )
{
	if( false == m_bProcessIcon || m_nTargetPosition == m_nNowPosition )
		return;

	m_fIconTime += fElapsedTime;

	if( PLAYER_ICON_MOVE_DELAY <= m_fIconTime )
	{
		++m_nNowPosition;
		SetPlayerIcon();
		m_fIconTime = 0.0f;

		CEtSoundEngine::GetInstance().PlaySound__( "2D", m_nMoveSound );

		if( m_nTargetPosition == m_nNowPosition )
		{
			m_bProcessIcon = false;
			m_bProcessEnter = true;
			m_fEnterTime = 0.0f;
		}
	}
}

void CDnAlteaBoardDlg::ProcessEnter( float fElapsedTime )
{
	if( false == m_bProcessEnter )
		return;

	m_fEnterTime += fElapsedTime;

	if( DUNGEON_ENTER_DELAY <= m_fEnterTime )
	{
		m_bProcessEnter = false;
		m_fEnterTime = 0.0f;
		GetAlteaTask().RequestAlteaNextMap();
	}
}

void CDnAlteaBoardDlg::SetGoldKeyCount()
{
	int nGoldkeyItemID = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::AlteiaWorldGoldKeyItemID);
	int nGoldKeyItemCount = GetItemTask().GetCharInventory().GetItemCount( nGoldkeyItemID );

	m_pKeyCount_Static->SetIntToText( nGoldKeyItemCount );
}

void CDnAlteaBoardDlg::SetDiceObject( int nDiceResult )
{
	DnEtcHandle hEtcObject = m_RenderEtc.GetEtc();
	if( !hEtcObject )
		return;

	CDnEtcObject * pObject = hEtcObject.GetPointer();
	if( NULL == pObject )
		return;

	EtObjectHandle hObject = pObject->GetObjectHandle();
	if( CEtObject::Identity() == hObject )
		return;

	if( CDnAlteaTask::E_DICE_UP_NUMBER == nDiceResult )
		return;

	int nDiceNumber = 1;
	const int nMaterialCount = hObject->GetMaterialCount();
	for( int itr = 0; itr < nMaterialCount; ++itr )
	{
		EtMaterialHandle hMaterial = hObject->GetMaterial( itr );

		if( !hMaterial || 0 == strstr( hMaterial->GetFileName(), "emissive" ) )
			continue;

		int nDiffuseCustomParam = hObject->AddCustomParam( "g_DiffuseTex" );
		int nEmissiveCustomParam = hObject->AddCustomParam( "g_EmissiveTex" );

		EtParameterHandle hParamDiffuseTex = hMaterial->GetParameterByName( "g_DiffuseTex" );
		EtParameterHandle hParamEmissiveTex = hMaterial->GetParameterByName( "g_EmissiveTex" );

		const int nCustomParamCount = hMaterial->GetCustomParamCount();
		for( int jtr = 0; jtr < nCustomParamCount; ++jtr )
		{
			SCustomParam * pParam = hObject->GetCustomParam( itr, jtr );

			if( pParam->hParamHandle == hParamDiffuseTex )
			{
				if( nDiceResult == nDiceNumber )
				{
					int nDiceFaceNumber = SetDiceFaceNumber( nDiceNumber );
					int nDiceUpFaceNumber = SetDiceFaceNumber( CDnAlteaTask::E_DICE_UP_NUMBER );

					int nTextureIndex = m_DiceTextureIndex[nDiceFaceNumber].first;
					hObject->SetCustomParam( nDiffuseCustomParam, (&nTextureIndex), m_vecDiceSubMeshIndex[nDiceUpFaceNumber] );

					nTextureIndex = m_DiceTextureIndex[nDiceUpFaceNumber].first;
					hObject->SetCustomParam( nDiffuseCustomParam, (&nTextureIndex), m_vecDiceSubMeshIndex[nDiceFaceNumber] );
				}
			}
			else if( pParam->hParamHandle == hParamEmissiveTex )
			{
				if( nDiceResult == nDiceNumber )
				{
					int nDiceFaceNumber = SetDiceFaceNumber( nDiceNumber );
					int nDiceUpFaceNumber = SetDiceFaceNumber( CDnAlteaTask::E_DICE_UP_NUMBER );

					int nTextureIndex = m_DiceTextureIndex[nDiceFaceNumber].second;
					hObject->SetCustomParam( nEmissiveCustomParam, (&nTextureIndex), m_vecDiceSubMeshIndex[nDiceUpFaceNumber] );

					nTextureIndex = m_DiceTextureIndex[nDiceUpFaceNumber].second;
					hObject->SetCustomParam( nEmissiveCustomParam, (&nTextureIndex), m_vecDiceSubMeshIndex[nDiceFaceNumber] );
				}
			}
		}
		++nDiceNumber;
	}
}

int CDnAlteaBoardDlg::SetDiceFaceNumber( int nDice )
{
	switch( nDice )
	{
		case 1 : return 0;
		case 2 : return 2;
		case 3 : return 3;
		case 4 : return 5;
		case 5 : return 1;
		case 6 : return 4;
	}

	return -1;
}

void CDnAlteaBoardDlg::ProcessDice( float fElapsedTime )
{
	if( false == m_bProcessDice )
		return;

	m_fDiceTime += fElapsedTime;

	if( DICE_ACTION_TIME <= m_fDiceTime )
	{
		m_bProcessDice = false;
		m_bProcessIcon = true;
		m_fDiceTime = 0.0f;
		m_pNotice_Static->SetText( m_pSlotData[m_nTargetPosition].m_wszMapName.c_str() );
	}
}

void CDnAlteaBoardDlg::SetAutoDiceCount()
{
	m_fAutoDiceTime = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldAutoDiceTimer );
	m_pAutoDiceCount_Static->Show( true );
	m_pAutoDiceCount_Static->SetIntToText( (int)m_fAutoDiceTime );
}

void CDnAlteaBoardDlg::ProcessAutoDiceCount( float fElapsedTime )
{
	if( false == m_bAutoDice )
		return;

	m_fAutoDiceTime -= fElapsedTime;
	
	if( 0.0f >= m_fAutoDiceTime )
		GetAlteaTask().RequestAlteaDice();
	else
		m_pAutoDiceCount_Static->SetIntToText( (int)m_fAutoDiceTime );
}

void CDnAlteaBoardDlg::SetSound( bool bAlteaSount )
{
	if( true == bAlteaSount )
	{
		CDnWorld::GetInstance().SetProcessBattleBGM( true );
		CEtSoundEngine::GetInstance().FadeVolume( NULL, 0.0f, CGlobalInfo::GetInstance().m_fFadeDelta, false );

		if( CEtSoundChannel::Identity() == m_hBGM )
			m_hBGM = CEtSoundEngine::GetInstance().PlaySound__( "BGM", m_nBGM, true, true );

		if( m_hBGM )
		{
			m_hBGM->SetVolume( CEtSoundEngine::GetInstance().GetMasterVolume( "BGM" ) );
			m_hBGM->Resume();
		}

		CEtSoundEngine::GetInstance().FadeVolume("BGM", 1.f, CGlobalInfo::GetInstance().m_fFadeDelta, false);
		CEtSoundEngine::GetInstance().FadeVolume( "2D", 1.0f, CGlobalInfo::GetInstance().m_fFadeDelta, false );

		CDnWorldEnvironment * pEnvi = CDnWorld::GetInstance().GetEnvironment();
		if( pEnvi )
			pEnvi->SetEnviBGMVolume( 0.0f );
	}
	else
	{
		if( m_hBGM )
			m_hBGM->Pause();

		CEtSoundEngine::GetInstance().FadeVolume( NULL, 1.0f, CGlobalInfo::GetInstance().m_fFadeDelta, false );
	}
}

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )