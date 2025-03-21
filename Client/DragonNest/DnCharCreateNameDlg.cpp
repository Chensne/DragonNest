#include "StdAfx.h"
#include "DnCharCreateNameDlg.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnMessageBox.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "MAPartsBody.h"
#include "DnInterfaceDlgID.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DNCountryUnicodeSet.h"



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharCreateNameDlg::CDnCharCreateNameDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
, m_pEditBoxName(NULL)
{
}

CDnCharCreateNameDlg::~CDnCharCreateNameDlg(void)
{
}

void CDnCharCreateNameDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("charsetupnamedlg.ui").c_str(), bShow);
}

void CDnCharCreateNameDlg::InitialUpdate()
{
	m_pEditBoxName = GetControl<CEtUIIMEEditBox>("ID_CHARNAME");
	
	m_bCallbackProcessed = false;

	CGlobalInfo::GetInstance().m_nClientCharNameLenMax = m_pEditBoxName->GetMaxChar() - 2;
}

void CDnCharCreateNameDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
	if (!pTask) return;

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
#ifndef PRE_MOD_SELECT_CHAR
		if (IsCmdControl("ID_CHARTURN")) { pTask->TurnCharacter(); return; }
#endif // PRE_MOD_SELECT_CHAR
		if (IsCmdControl("ID_BUTTON_BACK"))
		{
			//rlkt_dark
			if (pTask->GetLastState() == CDnLoginTask::CharCreate_SelectClassDark)
			{
				pTask->ChangeState(CDnLoginTask::CharCreate_SelectClassDark);
			}
			else{
				pTask->ChangeState(CDnLoginTask::CharCreate_SelectClass);
			}
			return;
		}

		if (IsCmdControl("ID_BUTTON_CREATE"))
		{
			tstring szName = m_pEditBoxName->GetText();

			if (szName.empty())
			{
				GetInterface().MessageBox(MESSAGEBOX_13, MB_OK, 0, this);
				return;
			}

			if (szName.size() < CHARNAMEMIN)
			{
				WCHAR wszTemp[80];
				swprintf_s(wszTemp, 80, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100303), CHARNAMEMIN);
				GetInterface().MessageBox(wszTemp, MB_OK, 0, this);
				return;
			}

			DWORD dwCheckType = ALLOW_STRING_DEFAULT;
#if defined (_US)
			dwCheckType = ALLOW_STRING_CHARACTERNAME_ENG;
#endif
			if (g_CountryUnicodeSet.Check(szName.c_str(), dwCheckType) == false)
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 83), MB_OK, 0, this);
				return;
			}

			if (DN_INTERFACE::UTIL::CheckAccount(szName) || DN_INTERFACE::UTIL::CheckChat(szName))
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 84), MB_OK, 0, this);
				return;
			}
#ifdef PRE_MOD_SELECT_CHAR
			//int nValue = -1;
			//m_pComboBoxServerList->GetSelectedValue(nValue);
			pTask->CreateCharacter((TCHAR*)szName.c_str(), GetInterface().GetCharSetupSelectedServerIndex());
#else // PRE_MOD_SELECT_CHAR
			pTask->CreateCharacter((TCHAR*)szName.c_str());
#endif // PRE_MOD_SELECT_CHAR
			m_bRequestWaitCreate = true;
			return;
		}
	}
	else if (nCommand == EVENT_EDITBOX_STRING)	// 임시 엔터 처리.
	{
		if (m_bCallbackProcessed)
		{
			// LoginDlg에 설명있음.
			m_bCallbackProcessed = false;
			return;
		}
		else
		{
			// 잘못된 ID 및 패스워드로 나오는 메세지박스의 핫키 작동하게 하려면 에딧박스에 있는 포커스를 없애야한다.
			focus::ReleaseControl();
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCharCreateNameDlg::Show(bool bShow)
{
	CEtUIDialog::Show(bShow);

	if (bShow)
	{
		m_pEditBoxName->ClearText();
		RequestFocus(m_pEditBoxName);
		m_bRequestWaitCreate = false;
	}

}

void CDnCharCreateNameDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_OK"))
		{
			m_pEditBoxName->ClearText();
			m_bRequestWaitCreate = false;
			RequestFocus(m_pEditBoxName);
		}
	}
}

void CDnCharCreateNameDlg::EnableCharCreateBackDlgControl(bool bEnable)
{
	bool bResultEnable = (!m_bRequestWaitCreate) ? bEnable : false;
	GetControl("ID_CHARNAME")->Enable(bResultEnable);
	GetControl("ID_BUTTON_CREATE")->Enable(bResultEnable);
	//GetControl("ID_BUTTON_BACK")->Enable(bResultEnable);
}
