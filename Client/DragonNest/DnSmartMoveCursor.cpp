#include "StdAfx.h"
#include "DnSmartMoveCursor.h"
#include "GameOption.h"
#include "DnMainFrame.h"
#include "DnLoadingTask.h"
#include "DnInterface.h"
#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
#include "DnNpcDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSmartMoveCursor::CDnSmartMoveCursor()
{
	m_pControl = NULL;
	m_pParentDlg = NULL;
	m_fMovePosX = m_fMovePosY = m_fPrePosX = m_fPrePosY = 0.5f;
}

void CDnSmartMoveCursor::SetControl( CEtUIControl* pControl )
{
	m_pControl = pControl;
	if ( m_pControl ) m_pParentDlg = pControl->GetParent();
}

void CDnSmartMoveCursor::MoveCursor()
{
	if ( !m_pControl || !m_pParentDlg )
		return;

	if( !CGameOption::GetInstance().m_bSmartMove )
		return;

	SUICoord uiCoord;
	m_pControl->GetUICoord( uiCoord );

	m_fMovePosX = m_pParentDlg->GetXCoord() + uiCoord.fX + uiCoord.fWidth / 2.0f;
	m_fMovePosY = m_pParentDlg->GetYCoord() + uiCoord.fY + uiCoord.fHeight / 2.0f;

	// 로그인화면에서 마우스 움직이지 않은채로 입력하면 다이얼로그에 WM_MOUSEMOVE 메세지가 안가서 0.0으로 남아있게된다.
	int nX, nY;
	CDnMouseCursor::GetInstance().GetCursorPos( &nX, &nY );
	m_fPrePosX = nX / m_pParentDlg->GetScreenWidth();
	m_fPrePosY = nY / m_pParentDlg->GetScreenHeight();

	nX = (int)(m_pParentDlg->GetScreenWidth() * m_fMovePosX);
	nY = (int)(m_pParentDlg->GetScreenHeight() * m_fMovePosY);
	CDnMouseCursor::GetInstance().SetCursorPos( nX, nY );

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	if( GetInterface().GetAimKeyState() )
	{
		RECT rcMouse;
		POINT pt;
		pt.x = nX;
		pt.y = nY;
		ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), &pt );
		rcMouse.left = rcMouse.right = pt.x;
		rcMouse.top = rcMouse.bottom = pt.y;
		//CDnMouseCursor::GetInstance().ClipCursor( &rcMouse );
		ClipCursor( &rcMouse );
	}
#endif
}

void CDnSmartMoveCursor::ReturnCursor()
{
	if ( !m_pControl || !m_pParentDlg )
		return;

	if( !CGameOption::GetInstance().m_bSmartMove )
		return;

	if( CDnLoadingTask::IsActive() && CDnLoadingTask::GetInstance().IsEnableBackgroundLoading() == false ) return;

#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
	CDnNpcDlg *pNpcDlg = GetInterface().GetNpcDialog();
	if(pNpcDlg && pNpcDlg->IsShow()) return; // NPC와 대화중 돌아가기 버튼 누를시 마우스 위치 리셋방지.
#endif

	int nX, nY;
	nX = (int)(m_pParentDlg->GetScreenWidth() * m_fPrePosX);
	nY = (int)(m_pParentDlg->GetScreenHeight() * m_fPrePosY);

	CDnMouseCursor::GetInstance().SetCursorPos( nX, nY );

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	if( GetInterface().GetAimKeyState() )
	{
		RECT rcMouse;
		POINT pt;
		pt.x = nX;
		pt.y = nY;
		ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), &pt );
		rcMouse.left = rcMouse.right = pt.x;
		rcMouse.top = rcMouse.bottom = pt.y;
		//CDnMouseCursor::GetInstance().ClipCursor( &rcMouse );
		ClipCursor( &rcMouse );
	}
#endif

	// Note : 커서를 옮기고 난 후 마우스무브 메세지를 다이얼로그에 날려준다.
	//
	LONG lParam;
	lParam = nY;
	lParam = (lParam << 16);
	lParam += nX;

	//EtInterface::StaticMsgProc( CDnMainFrame::GetInstance().GetHWnd(), WM_MOUSEMOVE, 0, (LPARAM)lParam );
	CEtUIDialogBase::StaticMsgProc( CDnMainFrame::GetInstance().GetHWnd(), WM_MOUSEMOVE, 0, (LPARAM)lParam );
}

CDnSmartMoveCursorEx::CDnSmartMoveCursorEx()
{
	m_pControl = NULL;
	m_pParentDlg = NULL;
	m_bReadyMove = false;
}

void CDnSmartMoveCursorEx::SetControl( CEtUIControl* pControl )
{
	m_pControl = pControl;
	if ( m_pControl ) m_pParentDlg = pControl->GetParent();
}

void CDnSmartMoveCursorEx::MoveCursor()
{
	m_bReadyMove = true;
}

void CDnSmartMoveCursorEx::Process()
{
	if( !m_pControl || !m_pParentDlg )
		return;

	if( !CGameOption::GetInstance().m_bSmartMoveMainMenu )
		return;

	if( CDnLoadingTask::IsActive() && CDnLoadingTask::GetInstance().IsEnableBackgroundLoading() == false )
		return;

	if( !m_bReadyMove )
		return;

	if( !m_pControl->IsShow() )
	{
		m_bReadyMove = false;
		return;
	}

	SUICoord uiCoord;
	m_pControl->GetUICoord( uiCoord );
	float fMovePosX = m_pParentDlg->GetXCoord() + uiCoord.fX + uiCoord.fWidth / 2.0f;
	float fMovePosY = m_pParentDlg->GetYCoord() + uiCoord.fY + uiCoord.fHeight / 2.0f;

	int nX, nY;
	nX = (int)(m_pParentDlg->GetScreenWidth() * fMovePosX);
	nY = (int)(m_pParentDlg->GetScreenHeight() * fMovePosY);
	CDnMouseCursor::GetInstance().SetCursorPos( nX, nY );

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	if( GetInterface().GetAimKeyState() )
	{
		RECT rcMouse;
		POINT pt;
		pt.x = nX;
		pt.y = nY;
		ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), &pt );
		rcMouse.left = rcMouse.right = pt.x;
		rcMouse.top = rcMouse.bottom = pt.y;
		//CDnMouseCursor::GetInstance().ClipCursor( &rcMouse );
		ClipCursor( &rcMouse );
	}
#endif

	m_bReadyMove = false;
}