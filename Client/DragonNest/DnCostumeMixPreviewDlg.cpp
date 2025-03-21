#include "StdAfx.h"

#include "DnCostumeMixPreviewDlg.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"
#include "DnCommonUtil.h"
#include "DnTableDB.h"
#include "DnItemTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCostumeMixPreviewDlg::CDnCostumeMixPreviewDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
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
	m_Type = eCOSMIX;
}

CDnCostumeMixPreviewDlg::~CDnCostumeMixPreviewDlg(void)
{
}

void CDnCostumeMixPreviewDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("Com_Preview.ui").c_str(), bShow);
}

void CDnCostumeMixPreviewDlg::InitialUpdate()
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


void CDnCostumeMixPreviewDlg::MakeAvatarToRender(int classId)
{
	CDnPlayerActor* pLocalActor = (CDnActor::s_hLocalActor.GetPointer()->GetClassID() == classId) ? static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer()) : NULL;

	m_RenderAvatar.ResetActor( m_RenderAvatar.GetActorID() != classId );

	int i = 0;
	//	NORMAL PARTS
	if (pLocalActor)
	{
		m_RenderAvatar.SetActor( CDnActor::s_hLocalActor, true, true );
	}
	else
	{
		m_RenderAvatar.SetActor( classId );

//#ifdef PRE_ADD_CASH_COSTUME_AURA
//		CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(m_RenderAvatar.GetActor().GetPointer());
//		if(pPlayerActor)
//			pPlayerActor->ComputeRTTModeCostumeAura();
//#endif
	}
}

void CDnCostumeMixPreviewDlg::AttachParts(ITEMCLSID itemId)
{
	m_RenderAvatar.AttachItem(itemId);
}

bool CDnCostumeMixPreviewDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

void CDnCostumeMixPreviewDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	if (bShow)
	{
		int classId = GetCurrentClassId();
		if (CommonUtil::IsValidCharacterClassId(classId) == false)
		{
			_ASSERT(0);
			return;
		}

		MakeAvatarToRender(classId);
	}
	else {
		m_RenderAvatar.ResetActor();
	}

	CDnCustomDlg::Show(bShow);
}

void CDnCostumeMixPreviewDlg::Render( float fElapsedTime )
{
	CDnCustomDlg::Render( fElapsedTime );

	if( IsShow() ) {
		SUICoord charViewCoord;
		if (m_pDrawFrameBtn->GetProperty() != NULL)
			charViewCoord = m_pDrawFrameBtn->GetProperty()->UICoord;

		DrawSprite( m_RenderAvatar.GetRTT(), m_RenderAvatar.GetUVCoord(), 0xFFFFFFFF, charViewCoord );
	}
}

void CDnCostumeMixPreviewDlg::Process( float fElapsedTime )
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

void CDnCostumeMixPreviewDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
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

void CDnCostumeMixPreviewDlg::ClearParts(int classId)
{
	MakeAvatarToRender(classId);
}

int	CDnCostumeMixPreviewDlg::GetCurrentClassId() const
{
	if (CDnItemTask::IsActive() == false)
		return -1;

	if (m_Type == eCOSMIX)
	{
		const CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
		return dataMgr.GetCurrentClassId();
	}
#ifdef PRE_ADD_COSRANDMIX
	else if (m_Type == eCOS_DMIX || m_Type == eCOS_RANDOMMIX)
#else
	else if (m_Type == eCOS_DMIX)
#endif
	{
		DnActorHandle hLocalActor = CDnActor::GetLocalActor();
		if (!hLocalActor)
			return -1;
		return hLocalActor->GetClassID();
	}

	return -1;
}

bool CDnCostumeMixPreviewDlg::IsInside(float fMouseX, float fMouseY) const
{
	return (GetDlgCoord().IsInside(fMouseX, fMouseY));
}