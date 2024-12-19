#include "StdAfx.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnMainDlg.h"
#include "DnTimeEventTask.h"
#include "DnSystemDlg.h"
#include "DnFadeInOutDlg.h"
#include "DnMainBar_MinimapDlg.h"
#include "DnMinimap.h"
#include "GameOption.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMainBar_MinimapDlg::CDnMainBar_MinimapDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
{
	m_pStaticBackLight = NULL;
	m_pStaticBackBlack = NULL;
}

CDnMainBar_MinimapDlg::~CDnMainBar_MinimapDlg(void)
{

}



void CDnMainBar_MinimapDlg::InitialUpdate()
{

	//hide lucky ID_STATIC_LUCKY
	CEtUIStatic *notused1 = GetControl<CEtUIStatic>("ID_STATIC_LUCKY");
	notused1->Show(false);
	//

	m_hBackLight = LoadResource(CEtResourceMng::GetInstance().GetFullName("MainBarLight.dds").c_str(), RT_TEXTURE);
	m_hBackBlack = LoadResource(CEtResourceMng::GetInstance().GetFullName("MainBarBlack.dds").c_str(), RT_TEXTURE);

	m_pStaticBackLight = GetControl<CEtUIStatic>("ID_BACK_LIGHT");
	m_pStaticBackLight->Show(false);
	m_pStaticBackBlack = GetControl<CEtUIStatic>("ID_BACK_BLACK");
	m_pStaticBackBlack->Show(false);

	m_bShowOption = true;
	ShowMinimapOption(false);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->SetChecked(true);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->SetChecked(true);
	if (CDnMinimap::IsActive()) {
		GetMiniMap().ShowFunctionalNPC(true);
		GetMiniMap().ShowOtherNPC(true);
		GetMiniMap().Zoom(+3.0f); // set default zoom!
								  //GetInterface().MessageBoxTextBox(L"Minimap is active set functio nand other np.c");
	}


	m_bShowDiceOption = true;
	GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->SetChecked(CGameOption::GetInstance().m_bEnableAutoDice);
	ShowAutoDiceOption(false);


}

void CDnMainBar_MinimapDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("mainbar_minimapdlg.ui").c_str(), bShow);
}

void CDnMainBar_MinimapDlg::Show(bool bShow)
{
	CEtUIDialog::Show(bShow);
}

void CDnMainBar_MinimapDlg::Render(float fElapsedTime)
{
	DrawSprite(m_hBackBlack, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0x7fffffff, m_pStaticBackBlack->GetUICoord());
	DrawSprite(m_hBackLight, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0xffffffff, m_pStaticBackLight->GetUICoord());

	if (CDnMinimap::IsActive())
	{
		SUICoord MinimapCoord, UVCoord;
		MinimapCoord.SetPosition((m_DlgInfo.DlgCoord.fWidth * 0.5f) - (78.0f / DEFAULT_UI_SCREEN_WIDTH), (6.0f / DEFAULT_UI_SCREEN_HEIGHT));
		MinimapCoord.SetSize(156.0f / DEFAULT_UI_SCREEN_WIDTH, 156.0f / DEFAULT_UI_SCREEN_HEIGHT);
		UVCoord.SetCoord(0.0f, 0.0f, 1.0f, 1.0f);
		//DrawSprite( CDnMinimap::GetInstance().GetTexture(), UVCoord, 0xdcffffff, MinimapCoord, -CalcCameraRotateValue() );
		//DrawSprite( CDnMinimap::GetInstance().GetTexture(), UVCoord, 0xdcffffff, MinimapCoord );
		DrawSprite(CDnMinimap::GetInstance().GetTexture(), UVCoord, 0xffffffff, MinimapCoord); // aici era 0xffffffff
	}

	CEtUIDialog::Render(fElapsedTime);

	if (CDnMinimap::IsActive())
	{
		DrawMinimapOutInfo();
	}

	// Draw North Mark rlkt 17.06
	/*	SUICoord NorthCoord;
	NorthCoord.SetPosition( (m_DlgInfo.DlgCoord.fWidth * 0.5f) - (11.0f/DEFAULT_UI_SCREEN_WIDTH), (-3.0f/DEFAULT_UI_SCREEN_HEIGHT) );
	NorthCoord.SetSize( 22.0f/DEFAULT_UI_SCREEN_WIDTH, 22.0f/DEFAULT_UI_SCREEN_HEIGHT );
	DrawSprite( m_hNorthTexture, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0xffffffff, NorthCoord );*/
}

void CDnMainBar_MinimapDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (!GetInterface().IsLockMainMenu()) {
		if (nCommand == EVENT_BUTTON_CLICKED)
		{
			if (GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd))
				return;

			if (IsCmdControl("ID_BUTTON_PLUS"))
			{
				if (CDnMinimap::IsActive() == true)
					GetMiniMap().Zoom(+1.0f);
				return;
			}

			if (IsCmdControl("ID_BUTTON_MINUS"))
			{
				if (CDnMinimap::IsActive() == true)
					GetMiniMap().Zoom(-1.0f);
				return;
			}

			if (IsCmdControl("ID_BUTTON_MAP"))
			{
				ShowMinimapOption(!m_bShowOption);
				return;
			}

#ifdef PRE_ADD_AUTO_DICE
			if (IsCmdControl("ID_BUTTON_DICE"))
			{
				ShowAutoDiceOption(!m_bShowDiceOption);
				return;
			}
#endif


		}

		if (nCommand == EVENT_CHECKBOX_CHANGED) {
			if (IsCmdControl("ID_CHECKBOX_FUNCTIONNPC"))
			{
				if (CDnMinimap::IsActive()) {
					bool bChecked = GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->IsChecked();
					GetMiniMap().ShowFunctionalNPC(bChecked);
				}
			}
			if (IsCmdControl("ID_CHECKBOX_NPC"))
			{
				if (CDnMinimap::IsActive()) {
					bool bChecked = GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->IsChecked();
					GetMiniMap().ShowOtherNPC(bChecked);
				}
			}

#ifdef PRE_ADD_AUTO_DICE
			if (IsCmdControl("ID_CHECKBOX_DICE"))
			{
				bool bChecked = GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->IsChecked();
				CGameOption::GetInstance().m_bEnableAutoDice = bChecked;
				GetControl<CDnMenuButton>("ID_BUTTON_DICE")->SetOpen(bChecked);
			}
#endif
			if (IsCmdControl("ID_BT_LADDER"))
			{

				return;
			}


		}
	}

}

void CDnMainBar_MinimapDlg::DrawMinimapOutInfo()
{
	std::vector<CDnMinimap::SOutInfo> &vecOutInfo = GetMiniMap().GetMiniMapOutInfo();
	if (vecOutInfo.empty() == false)
	{
		for (DWORD i = 0; i < vecOutInfo.size(); ++i)
		{
			CEtUIStatic *pIcon = NULL;
			switch (vecOutInfo[i].eIconType)
			{

			case CDnMinimap::emICON_INDEX::indexTraceQuest:
				pIcon = GetControl<CEtUIStatic>("ID_ICON_QUEST");
				break;

			case CDnMinimap::emICON_INDEX::indexAllyCaptain:
				pIcon = GetControl<CEtUIStatic>("ID_MAP_PARTY");
				break;

			case CDnMinimap::emICON_INDEX::indexEnemyCaptain:
				pIcon = GetControl<CEtUIStatic>("ID_MAP_ENEMY");
				break;
			}

			if (pIcon)
				DrawIcon(pIcon, vecOutInfo[i].fDegree, 72.f, 0.0025f);
		}
	}
}

void CDnMainBar_MinimapDlg::ShowMinimapOption(bool bShow)
{
	if (m_bShowOption == bShow) {
		return;
	}
	m_bShowOption = bShow;
	GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->Show(bShow);
	GetControl<CEtUIStatic>("ID_STATIC_FUNCTIONNPC")->Show(bShow);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->Show(bShow);
	GetControl<CEtUIStatic>("ID_STATIC_NPC")->Show(bShow);
	GetControl<CEtUIStatic>("ID_MM_BACK")->Show(bShow);
}


#ifdef PRE_ADD_AUTO_DICE
void CDnMainBar_MinimapDlg::ShowAutoDiceOption(bool bShow)
{
	if (m_bShowDiceOption == bShow) {
		return;
	}

	m_bShowDiceOption = bShow;
	GetControl<CEtUIStatic>("ID_TEXT_DICE")->Show(bShow);
	GetControl<CEtUIStatic>("ID_STATIC0")->Show(bShow);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->Show(bShow);
}

void CDnMainBar_MinimapDlg::UpateAutoDiceOption(bool bReset)
{
	if (bReset)
		CGameOption::GetInstance().m_bEnableAutoDice = false;

	GetControl<CDnMenuButton>("ID_BUTTON_DICE")->SetOpen(CGameOption::GetInstance().m_bEnableAutoDice);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->SetChecked(CGameOption::GetInstance().m_bEnableAutoDice);
}
#endif


void CDnMainBar_MinimapDlg::UpdateMinimapOption()
{
	bool bShowFunctionNpc = GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->IsChecked();
	bool bShowStaticNpc = GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->IsChecked();

	if (CDnMinimap::IsActive()) {
		GetMiniMap().ShowFunctionalNPC(bShowFunctionNpc);
		GetMiniMap().ShowOtherNPC(bShowStaticNpc);
	}
}


void CDnMainBar_MinimapDlg::DrawIcon(CEtUIControl *pControl, float fAngle, float fRadius, float fValue)
{
	SUIElement *pElement = pControl->GetElement(0);
	if (pElement)
	{
		SUICoord uiCoord;
		pControl->GetUICoord(uiCoord);

		float fCenX, fCenY;
		fCenX = (m_DlgInfo.DlgCoord.fWidth) * 0.5f;
		fCenY = 78.0f / DEFAULT_UI_SCREEN_HEIGHT;

		float fModifiedAngle = fAngle - 1.5707964f;

		EtVector2 vTemp;
		vTemp.x = cos(fModifiedAngle) * (fRadius / DEFAULT_UI_SCREEN_WIDTH);

		if (fModifiedAngle > RIGHT_DEGREE_MAX || fModifiedAngle < LEFT_DEGREE_MAX)
		{
			uiCoord.fY = m_DlgInfo.DlgCoord.fHeight - uiCoord.fHeight + fValue - (20.0f / DEFAULT_UI_SCREEN_HEIGHT);
		}
		else
		{
			vTemp.y = sin(fModifiedAngle) * (fRadius / DEFAULT_UI_SCREEN_HEIGHT);
			uiCoord.fY = vTemp.y + (fCenY - (uiCoord.fHeight*0.5f));
		}

		uiCoord.fX = vTemp.x + (fCenX - (uiCoord.fWidth*0.5f));
		uiCoord.SetSize(pElement->fTextureWidth * 1.25f, pElement->fTextureHeight * 1.25f);

		DrawSprite(pControl->GetTemplate().m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, uiCoord, EtToDegree(fAngle));
	}
}


bool CDnMainBar_MinimapDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!IsShow())
	{
		return false;
	}

	bool bRet;
	bRet = CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short(LOWORD(lParam));
		MousePoint.y = short(HIWORD(lParam));
		PointToFloat(MousePoint, fMouseX, fMouseY);

		if ((GetControl<CEtUIButton>("ID_BUTTON_MAP")->IsInside(fMouseX, fMouseY) == false) &&
			(GetControl<CEtUIStatic>("ID_MM_BACK")->IsInside(fMouseX, fMouseY) == false)) {
			if (m_bShowOption) {
				ShowMinimapOption(false);
			}
		}
#ifdef PRE_ADD_AUTO_DICE
		if ((GetControl<CEtUIButton>("ID_BUTTON_DICE")->IsInside(fMouseX, fMouseY) == false) &&
			(GetControl<CEtUIStatic>("ID_STATIC0")->IsInside(fMouseX, fMouseY) == false)) {
			if (m_bShowDiceOption) {
				ShowAutoDiceOption(false);
			}
		}
#endif
	}
	break;
	}

	return bRet;
}