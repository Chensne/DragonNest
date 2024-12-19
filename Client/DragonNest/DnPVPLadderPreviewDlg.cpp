#include "StdAfx.h"
#include "DnPVPLadderPreviewDlg.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"
#include "DnCommonUtil.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "DnPVPLobbyVillageTask.h"
#include "TaskManager.h"
#include "DnItemTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPLadderPreviewDlg::CDnPVPLadderPreviewDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pReduceBtn		= NULL;
	m_pRotateLeftBtn	= NULL;
	m_pRotateRightBtn	= NULL;
	m_pEnlargeBtn		= NULL;

	m_pDrawFrameBtn = NULL;
	m_pRotateFrontBtn	= NULL;
	m_pRotateBackBtn	= NULL;

	m_fMouseX = m_fMouseY = 0.0f;
}

CDnPVPLadderPreviewDlg::~CDnPVPLadderPreviewDlg(void)
{
}

void CDnPVPLadderPreviewDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("PvpLadderPreviewDlg.ui").c_str(), bShow);
}

void CDnPVPLadderPreviewDlg::InitialUpdate()
{
	m_pRotateLeftBtn	= GetControl<CEtUIButton>("ID_ROTATE_LEFT");
	m_pRotateRightBtn	= GetControl<CEtUIButton>("ID_ROTATE_RIGHT");

	m_pRotateFrontBtn	= GetControl<CEtUIButton>("ID_BUTTON_FRONT");
	m_pRotateBackBtn	= GetControl<CEtUIButton>("ID_BUTTON_BACK");

	m_pDrawFrameBtn		= GetControl<CEtUIButton>("ID_BUTTON_PREVIEW");

	if (m_pDrawFrameBtn == NULL)
	{
		_ASSERT(0);
		return;
	}

	SUICoord charViewCoord;
	if (m_pDrawFrameBtn->GetProperty() != NULL)
		charViewCoord = m_pDrawFrameBtn->GetProperty()->UICoord;

	const int nTextureSize = 512;
	SCameraInfo CameraInfo;
	CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	CameraInfo.fNear = 10.f;
	CameraInfo.Type = CT_ORTHOGONAL;
	m_RenderAvatar.Initialize( nTextureSize, nTextureSize, CameraInfo, 160, 120, FMT_A8R8G8B8, true, true );
	m_RenderAvatar.CalcUVCoord( charViewCoord.fWidth, charViewCoord.fHeight );

}

// PVP 로비상태에서는 CDnActor::s_hLocalActor 가 존재하지 않습니다.
// 대신 pvpLobbyTask 에서 입장시 SCEnter 형태로 유저의 패킷을 가지고 잇으며
// 아이템 테스크에서 현재 유저의 아이템에 대한 정보를 쥐고 있기때문에 해당부분의 정보를 이용해서 아이템을 조립해줍니다.

void CDnPVPLadderPreviewDlg::RefreshPreview()
{
	CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	if(pPVPLobbyTask)
	{
		vector<int> jobHistory;
		for( int i=0; i<JOBMAX; i++ ) {
			if( pPVPLobbyTask->GetUserInfo().cJobArray[i] != 0 )
				jobHistory.push_back( pPVPLobbyTask->GetUserInfo().cJobArray[i] );
		}

		if( !jobHistory.empty() )
		{
			int nClassID = -1;
			nClassID = jobHistory[0];

			if(!m_RenderAvatar.GetActor())
			{
				m_RenderAvatar.ResetActor();
				m_RenderAvatar.SetActor( nClassID , false);

				CDnItemTask *pItemTask = (CDnItemTask *)CTaskManager::GetInstance().GetTask( "ItemTask" );
				if(pItemTask)
				{
#ifdef PRE_ADD_PVPRANK_INFORM
					const SCEnter& userInfo = pPVPLobbyTask->GetUserInfo();
					char cViewCache[VIEWCASHEQUIPMAX_BITSIZE];
					memcpy_s(cViewCache, sizeof(cViewCache), userInfo.cViewCashEquipBitmap, sizeof(userInfo.cViewCashEquipBitmap));
#endif
					if(m_RenderAvatar.GetActor()->IsPlayerActor())
					{
						CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(m_RenderAvatar.GetActor().GetPointer());

#ifdef PRE_ADD_PVPRANK_INFORM
						int nDefaultPartsInfo[4] = {  
							userInfo.nDefaultBody, 
							userInfo.nDefaultLeg, 
							userInfo.nDefaultHand,  
							userInfo.nDefaultFoot };

						pPlayerActor->SetDefaultPartsInfo( nDefaultPartsInfo );
						if (pPlayerActor->GetObjectHandle())
						{
							pPlayerActor->SetPartsColor( MAPartsBody::HairColor, userInfo.dwHairColor );
							pPlayerActor->SetPartsColor( MAPartsBody::EyeColor, userInfo.dwEyeColor );
							pPlayerActor->SetPartsColor( MAPartsBody::SkinColor, userInfo.dwSkinColor );
						}

						const SCEnter& userInfo = pPVPLobbyTask->GetUserInfo();
						char cViewCache[VIEWCASHEQUIPMAX_BITSIZE];
						memcpy_s(cViewCache, sizeof(cViewCache), userInfo.cViewCashEquipBitmap, sizeof(userInfo.cViewCashEquipBitmap));

						pPlayerActor->SetWeaponViewOrder( 0, GetBitFlag(cViewCache, CASHEQUIP_WEAPON1) );
						pPlayerActor->SetWeaponViewOrder( 1, GetBitFlag(cViewCache, CASHEQUIP_WEAPON2) );

						for( int i = 0; i < CDnParts::SwapParts_Amount; ++i )
							pPlayerActor->SetPartsViewOrder( i, GetBitFlag(cViewCache, i) );

						pPlayerActor->SetHideHelmet( GetBitFlag(cViewCache, HIDEHELMET_BITINDEX) );
#else
						int nDefaultPartsInfo[4] = {  
							pPVPLobbyTask->GetUserInfo().nDefaultBody, 
							pPVPLobbyTask->GetUserInfo().nDefaultLeg, 
							pPVPLobbyTask->GetUserInfo().nDefaultHand,  
							pPVPLobbyTask->GetUserInfo().nDefaultFoot };

							pPlayerActor->SetDefaultPartsInfo( nDefaultPartsInfo );
							if(pPlayerActor->GetObjectHandle())
							{
								pPlayerActor->SetPartsColor( MAPartsBody::HairColor, pPVPLobbyTask->GetUserInfo().dwHairColor );
								pPlayerActor->SetPartsColor( MAPartsBody::EyeColor, pPVPLobbyTask->GetUserInfo().dwEyeColor );
								pPlayerActor->SetPartsColor( MAPartsBody::SkinColor, pPVPLobbyTask->GetUserInfo().dwSkinColor );
							}

						pPlayerActor->SetWeaponViewOrder( 0, GetBitFlag(pPVPLobbyTask->GetUserInfo().cViewCashEquipBitmap, CASHEQUIP_WEAPON1) );
						pPlayerActor->SetWeaponViewOrder( 1, GetBitFlag(pPVPLobbyTask->GetUserInfo().cViewCashEquipBitmap, CASHEQUIP_WEAPON2) );

						for( int i = 0; i < CDnParts::SwapParts_Amount; ++i )
							pPlayerActor->SetPartsViewOrder( i, GetBitFlag(pPVPLobbyTask->GetUserInfo().cViewCashEquipBitmap, i) );

						pPlayerActor->SetHideHelmet( GetBitFlag(pPVPLobbyTask->GetUserInfo().cViewCashEquipBitmap, HIDEHELMET_BITINDEX) );
#endif
					}


					for(int i=0; i<EQUIPMAX; i++)
					{
#ifdef PRE_ADD_PVPRANK_INFORM
						if( (CDnParts::PartsTypeEnum)i == CDnParts::Helmet && GetBitFlag(cViewCache, HIDEHELMET_BITINDEX) == true )
#else
						if( (CDnParts::PartsTypeEnum)i == CDnParts::Helmet && GetBitFlag(pPVPLobbyTask->GetUserInfo().cViewCashEquipBitmap, HIDEHELMET_BITINDEX) == true )
#endif
							continue;
						CDnItem *pItem = pItemTask->GetEquipItem(i);
						if(pItem)
						{
							m_RenderAvatar.AttachItem(pItem->GetClassID());
						}
					}

					for(int i=0; i<CASHEQUIPMAX; i++)
					{
#ifdef PRE_ADD_PVPRANK_INFORM
						if( i < CDnParts::SwapParts_Amount && GetBitFlag(cViewCache, i) == false )
#else
						if( i < CDnParts::SwapParts_Amount && GetBitFlag(pPVPLobbyTask->GetUserInfo().cViewCashEquipBitmap, i) == false )
#endif
							continue;

#ifdef PRE_ADD_PVPRANK_INFORM
						if( (CDnParts::PartsTypeEnum)i == CDnParts::CashHelmet && GetBitFlag(cViewCache, HIDEHELMET_BITINDEX) == true )
#else
						if( (CDnParts::PartsTypeEnum)i == CDnParts::CashHelmet && GetBitFlag(pPVPLobbyTask->GetUserInfo().cViewCashEquipBitmap, HIDEHELMET_BITINDEX) == true )
#endif
							continue;
						CDnItem *pItem = pItemTask->GetCashEquipItem(i);
						if(pItem)
						{
							int itemId = (pItem->GetLookItemID() != ITEMCLSID_NONE) ? pItem->GetLookItemID() : pItem->GetClassID();
							m_RenderAvatar.AttachItem(itemId);
						}
					}

#ifdef PRE_ADD_CASH_COSTUME_AURA
					CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(m_RenderAvatar.GetActor().GetPointer());
					if(pPlayerActor)
						pPlayerActor->ComputeRTTModeCostumeAura();
#endif

					m_RenderAvatar.AddRotateYaw(-25.f);
				}
			}
		}
	}
}

void CDnPVPLadderPreviewDlg::SetJobAction()
{
	CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	if(!pPVPLobbyTask)
		return;

	DNTableFileFormat*  pSox = GetDNTable(CDnTableDB::TJOB);
	if (!pSox) return;

	vector<int> jobHistory;
	int nJobIndex = 0;
	std::string strActionName;

	for( int i=0; i<JOBMAX; i++ ) {
		if( pPVPLobbyTask->GetUserInfo().cJobArray[i] != 0 )
			jobHistory.push_back( pPVPLobbyTask->GetUserInfo().cJobArray[i] );

	}

	if( jobHistory.empty() )
		return;
	
	nJobIndex = jobHistory[jobHistory.size()-1];
	
	if (pSox->IsExistItem(nJobIndex))
	{
		DNTableCell* pField = pSox->GetFieldFromLablePtr(nJobIndex, "_IdleMotionName");

		if (pField)
			strActionName = pField->GetString();
	}
	m_RenderAvatar.GetActor()->SetActionQueue(strActionName.c_str());

}

bool CDnPVPLadderPreviewDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
				if( nScrollAmount < 0 ) {
					m_RenderAvatar.ZoomOut();
				}
				else if( nScrollAmount > 0 ) {
					m_RenderAvatar.ZoomIn();
				}
			}
		}
		break;
	}

	return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}




void CDnPVPLadderPreviewDlg::Render( float fElapsedTime )
{
	CDnCustomDlg::Render( fElapsedTime );

	if( IsShow() ) {
		SUICoord charViewCoord;
		if (m_pDrawFrameBtn->GetProperty() != NULL)
			charViewCoord = m_pDrawFrameBtn->GetProperty()->UICoord;

		DrawSprite( m_RenderAvatar.GetRTT(), m_RenderAvatar.GetUVCoord(), 0xFFFFFFFF, charViewCoord );
	}
}

void CDnPVPLadderPreviewDlg::Process( float fElapsedTime )
{
	if (IsShow() == false) return;

	m_RenderAvatar.Process( fElapsedTime );

	static float fRotScale = 200.f;
	if( m_pRotateLeftBtn->IsPressed() )
	{
		m_RenderAvatar.AddRotateYaw( fElapsedTime * fRotScale );
	}
	else if( m_pRotateRightBtn->IsPressed() )
	{
		m_RenderAvatar.AddRotateYaw( -fElapsedTime * fRotScale );
	}

	if( m_pDrawFrameBtn->IsPressed()  )
	{
		float fX = m_pDrawFrameBtn->GetMouseCoord().fX;
		float fY = m_pDrawFrameBtn->GetMouseCoord().fY;
		if( m_fMouseX != 0.f && m_fMouseY != 0.f ) {
			static float fMoveScale = 500.f;
			float fAddAngle = fMoveScale * sqrtf( (m_fMouseX-fX)*(m_fMouseX-fX)+(m_fMouseY-fY)*(m_fMouseY-fY) ) *  (((m_fMouseX-fX)>0.f)? 1.f : -1.f) ;
			m_RenderAvatar.AddRotateYaw( fAddAngle );
		}
		m_fMouseX = fX;
		m_fMouseY = fY;

		focus::SetFocus( m_pDrawFrameBtn );
	}
	else if( m_pDrawFrameBtn->IsRightPressed()  )
	{
		float fX = m_pDrawFrameBtn->GetMouseCoord().fX;
		float fY = m_pDrawFrameBtn->GetMouseCoord().fY;

		if( m_fMouseX != 0.f && m_fMouseY != 0.f ) {
			float fDistX = m_fMouseX - fX;
			float fDistY = m_fMouseY - fY;
			m_RenderAvatar.Panning( fDistX * GetScreenWidth(), fDistY * GetScreenHeight() );
		}

		m_fMouseX = fX;
		m_fMouseY = fY;

		focus::SetFocus( m_pDrawFrameBtn );
	}
	else {
		m_fMouseX = 0.f;
		m_fMouseY = 0.f;
	}


	if( m_RenderAvatar.IsFrontView() ) {
		m_pRotateFrontBtn->Show( false );
		m_pRotateBackBtn->Show( true );
	}
	else {
		m_pRotateFrontBtn->Show( true );
		m_pRotateBackBtn->Show( false );
	}

	CDnCustomDlg::Process(fElapsedTime);
}

void CDnPVPLadderPreviewDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_PREVIEW"))	
		{
			focus::ReleaseControl();
		}
		if( IsCmdControl("ID_BUTTON_FRONT") )
		{
			m_RenderAvatar.SetFrontView();
		}
		if( IsCmdControl("ID_BUTTON_BACK") )
		{
			m_RenderAvatar.SetRearView();
		}
		if( IsCmdControl("ID_BUTTON_ZOOMIN") )
		{
			m_RenderAvatar.ZoomIn();
		}
		if( IsCmdControl("ID_BUTTON_ZOOMOUT") )
		{
			m_RenderAvatar.ZoomOut();
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

