#include "StdAfx.h"
#include "DnCharCreateSetupGestureDlg.h"
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

CDnCharCreateSetupGestureDlg::CDnCharCreateSetupGestureDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
, m_bRequestWaitCreate(false)
, m_bCallbackProcessed(false)
, m_bCharCreate(false)
{
	memset(m_pGestureSlotButton, NULL, sizeof(CDnLifeSkillButton*)* MAX_GESTURE);
	memset(m_pGestureText, NULL, sizeof(CEtUIStatic*)* MAX_GESTURE);
	memset(m_pGestureButton, NULL, sizeof(CEtUIButton*)* MAX_GESTURE);
#ifdef PRE_MOD_SELECT_CHAR
	memset(m_pCostumeRadioButton, 0, sizeof(m_pCostumeRadioButton));
#endif // PRE_MOD_SELECT_CHAR
}

CDnCharCreateSetupGestureDlg::~CDnCharCreateSetupGestureDlg(void)
{
	m_vecStrGestureActionName.clear();
}

void CDnCharCreateSetupGestureDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CharSetupCostumeDlg.ui").c_str(), bShow);
}

void CDnCharCreateSetupGestureDlg::InitialUpdate()
{
	
	m_bCallbackProcessed = false;

	char szControlName[32];
	for (int i = 0; i<MAX_COSTUME; ++i)
	{
		sprintf_s(szControlName, _countof(szControlName), "ID_RBT_COSTUME%d", i);
		m_pCostumeRadioButton[i] = GetControl<CEtUIRadioButton>(szControlName);
	}

	for (int i = 0; i<MAX_GESTURE; ++i)
	{
		sprintf_s(szControlName, _countof(szControlName), "ID_BT_GESTURE%d", i);
		m_pGestureSlotButton[i] = GetControl<CDnLifeSkillButton>(szControlName);
		sprintf_s(szControlName, _countof(szControlName), "ID_TEXT_GESTURE%d", i);
		m_pGestureText[i] = GetControl<CEtUIStatic>(szControlName);
		sprintf_s(szControlName, _countof(szControlName), "ID_BT_GT%d", i);
		m_pGestureButton[i] = GetControl<CEtUIButton>(szControlName);
		m_pGestureButton[i]->SetButtonID(i);
	}

	SetGestureInfo();
}


void CDnCharCreateSetupGestureDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
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

		if (strstr(pControl->GetControlName(), "ID_BT_GT"))
		{
			int nSelectIndex = static_cast<CEtUIButton*>(pControl)->GetButtonID();
			pTask->CharCreateGestureAction(m_vecStrGestureActionName[nSelectIndex].c_str(), m_bCharCreate);
		}
	}
	else if (nCommand == EVENT_RADIOBUTTON_CHANGED)
	{
		if (strstr(pControl->GetControlName(), "ID_RBT_COSTUME"))
		{
			int nSelectIndex = static_cast<CEtUIRadioButton*>(pControl)->GetTabID();
			pTask->ShowPartsList(nSelectIndex);
		}
	}
	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCharCreateSetupGestureDlg::Show(bool bShow)
{
	CEtUIDialog::Show(bShow);

#ifdef PRE_MOD_SELECT_CHAR
	//GetInterface().ShowCharGestureDlg(bShow, true);
	///? rlkt_test

	if (bShow)
	{
		SetGestureInfo();

		for (int i = 0; i<MAX_COSTUME; ++i)
		{
			if (m_pCostumeRadioButton[i])
				m_pCostumeRadioButton[i]->SetChecked(false);
		}
	}
#endif // PRE_MOD_SELECT_CHAR
}

void CDnCharCreateSetupGestureDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName(pControl->GetControlName());

}

#ifdef PRE_MOD_CREATE_CHAR

void CDnCharCreateSetupGestureDlg::SetCharCreatePartsName(CDnParts::PartsTypeEnum PartsIndex, std::wstring strPartsName)
{
	switch (PartsIndex)
	{
	case CDnParts::Hair:
		GetControl("ID_STATIC_HEAD")->SetText(strPartsName);
		break;
	case CDnParts::Face:
		GetControl("ID_STATIC_FACE")->SetText(strPartsName);
		break;
	case CDnParts::Body:
		GetControl("ID_STATIC_BODY")->SetText(strPartsName);
		break;
	case CDnParts::Leg:
		GetControl("ID_STATIC_LEG")->SetText(strPartsName);
		break;
	case CDnParts::Hand:
		GetControl("ID_STATIC_GLOVE")->SetText(strPartsName);
		break;
	case CDnParts::Foot:
		GetControl("ID_STATIC_BOOTS")->SetText(strPartsName);
		break;
	}
}

#endif // PRE_MOD_CREATE_CHAR


void CDnCharCreateSetupGestureDlg::SetGestureInfo()
{
	m_vecStrGestureActionName.clear();

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TGESTURE);
	int nGestureIndex1 = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Login_Gesture1);
	int nGestureIndex2 = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Login_Gesture2);
	int nGestureIndex3 = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Login_Gesture3);
	int nGestureIndex4 = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Login_Gesture4);
	int nGestureIndex5 = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Login_Gesture5);

	m_vecStrGestureActionName.push_back(pSox->GetFieldFromLablePtr(nGestureIndex1, "_ActionID")->GetString());
	m_vecStrGestureActionName.push_back(pSox->GetFieldFromLablePtr(nGestureIndex2, "_ActionID")->GetString());
	m_vecStrGestureActionName.push_back(pSox->GetFieldFromLablePtr(nGestureIndex3, "_ActionID")->GetString());
	m_vecStrGestureActionName.push_back(pSox->GetFieldFromLablePtr(nGestureIndex4, "_ActionID")->GetString());
	m_vecStrGestureActionName.push_back(pSox->GetFieldFromLablePtr(nGestureIndex5, "_ActionID")->GetString());

	m_pGestureSlotButton[0]->SetGestureIcon(pSox->GetFieldFromLablePtr(nGestureIndex1, "_IconID")->GetInteger());
	m_pGestureSlotButton[1]->SetGestureIcon(pSox->GetFieldFromLablePtr(nGestureIndex2, "_IconID")->GetInteger());
	m_pGestureSlotButton[2]->SetGestureIcon(pSox->GetFieldFromLablePtr(nGestureIndex3, "_IconID")->GetInteger());
	m_pGestureSlotButton[3]->SetGestureIcon(pSox->GetFieldFromLablePtr(nGestureIndex4, "_IconID")->GetInteger());
	m_pGestureSlotButton[4]->SetGestureIcon(pSox->GetFieldFromLablePtr(nGestureIndex5, "_IconID")->GetInteger());

	m_pGestureText[0]->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(nGestureIndex1, "_NameID")->GetInteger()));
	m_pGestureText[1]->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(nGestureIndex2, "_NameID")->GetInteger()));
	m_pGestureText[2]->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(nGestureIndex3, "_NameID")->GetInteger()));
	m_pGestureText[3]->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(nGestureIndex4, "_NameID")->GetInteger()));
	m_pGestureText[4]->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(nGestureIndex5, "_NameID")->GetInteger()));
}