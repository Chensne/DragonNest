#include "StdAfx.h"

#include "DnNestRebirthTooltipDlg.h"
#include "DnTableDB.h"
#include "TaskManager.h"
#include "DnQuestTask.h"
#include "DnInterface.h"
#include "DnCommonUtil.h"
#include "DnUIString.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnNestRebirthTooltipDlg::CDnNestRebirthTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) : CEtUIDialog( dialogType, pParentDialog, nID, pCallback ), 
																																						 m_pContents( NULL )
{
}

CDnNestRebirthTooltipDlg::~CDnNestRebirthTooltipDlg(void)
{
}

void CDnNestRebirthTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SimpleTooltipDlg.ui" ).c_str(), bShow );
}

void CDnNestRebirthTooltipDlg::InitialUpdate()
{
	m_pContents = GetControl<CEtUITextBox>( "ID_TEXTBOX" );
}

bool CDnNestRebirthTooltipDlg::AdjustPosition(float orgX, float orgY)
{
	float fRight = orgX + m_pContents->GetUICoord().fWidth;
	if (fRight > GetScreenWidthRatio())
		orgX -= m_pContents->GetUICoord().fWidth;

	if( m_pContents->GetLineSize() > 0)
		orgY -= m_pContents->GetUICoord().fHeight * GetScreenHeightRatio();

	SetPosition(orgX, orgY);

	return true;
}

bool CDnNestRebirthTooltipDlg::SetNestRebirthTooltip(int nUsableRebirthCount, int nMaxRebirthCount, int nUsableRebirthItemCount)
{
	m_pContents->ClearText();

	std::wstring result;
	result = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130260), 
		nUsableRebirthCount, nMaxRebirthCount, nUsableRebirthCount, nMaxRebirthCount, nUsableRebirthItemCount); // UISTRING : L"#g[부활 가능 횟수:%d/%d]\n#w(직접 부활가능 횟수 #g%d #w/ 전제 부활가능횟수 #g%d #w)\n\n#y[부활의 사과주스 부활 횟수:%d]"

	m_pContents->AddColorText(result.c_str());

	return true;
}

bool CDnNestRebirthTooltipDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_bShow == false)
		return false;

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			float fMouseX = 0;
			float fMouseY = 0;
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			if (m_pContents && m_pContents->IsInside(fMouseX, fMouseY))
			{
				Show(false);
			}
		}
	}

	return bRet;
}