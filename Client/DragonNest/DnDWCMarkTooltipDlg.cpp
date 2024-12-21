#include "StdAfx.h"

#if defined(PRE_ADD_DWC)
#include "DnDWCMarkTooltipDlg.h"
#include "DnTableDB.h"
#include "TaskManager.h"
#include "DnQuestTask.h"
#include "DnInterface.h"
#include "DnCommonUtil.h"
#include "DnUIString.h"
#include "DnDWCTask.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDWCMarkTooltipDlg::CDnDWCMarkTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) 
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pContents(NULL)
{
}

CDnDWCMarkTooltipDlg::~CDnDWCMarkTooltipDlg()
{
}

void CDnDWCMarkTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SimpleTooltipDlg.ui" ).c_str(), bShow );
}

void CDnDWCMarkTooltipDlg::InitialUpdate()
{
	m_pContents = GetControl<CEtUITextBox>("ID_TEXTBOX");
}

bool CDnDWCMarkTooltipDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
				Show(false);
		}
	}

	return bRet;
}

bool CDnDWCMarkTooltipDlg::AdjustPosition(float orgX, float orgY)
{
	float fRight = orgX + m_pContents->GetUICoord().fWidth;
	if (fRight > GetScreenWidthRatio())
		orgX -= m_pContents->GetUICoord().fWidth;

	SetPosition(orgX, orgY);
	return true;
}

bool CDnDWCMarkTooltipDlg::SetMarkTooltip()
{
	m_pContents->ClearText();

	if (CDnDWCTask::IsActive() == false)
		return false;

	std::wstring wszStartTime;
	std::wstring wszEndTime;
	GetDWCTask().GetDWCSessionPeriod(wszStartTime, wszEndTime);

	std::wstring S1;
	std::wstring E1;
	std::wstring S2;
	std::wstring E2;
	GetDWCTask().GetDWCRemainMatchTime(CDnDWCTask::eMatchTime::eDWC_First_MatchTime , S1, E1);
	GetDWCTask().GetDWCRemainMatchTime(CDnDWCTask::eMatchTime::eDWC_Second_MatchTime, S2, E2);
	
	std::wstring wszResult = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120268), wszStartTime.c_str(), wszEndTime.c_str(), S1.c_str(), E1.c_str(), S2.c_str(), E2.c_str());
	m_pContents->AddColorText(wszResult.c_str());
	return true;
}

#endif // PRE_ADD_DWC