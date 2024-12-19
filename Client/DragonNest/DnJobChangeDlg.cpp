#include "StdAfx.h"
#include "DnJobChangeDlg.h"
#include "DnWorld.h"
#include "CustomSendPacket.h"
#include "ItemSendPacket.h"
#include "DnTableDB.h"
#include "EtUIXML.h"
#include "DnInterface.h"
#include "SmartPtrDef.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 


CDnJobChangeDlg::CDnJobChangeDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	for (int i = 0; i < 2; i++)
	{
		m_pSelectedClass[i] = NULL;
		m_pSelectButton[i] = NULL;
		m_pClassName[i] = NULL;
		m_pJobIcon[i] = NULL;
		m_pClassInfoText[i] = NULL;
		m_pSkillButtonA[i] = NULL;
		m_pSkillButtonB[i] = NULL;
		m_pMovieFile[i] = NULL;
		m_pMovieControl[i] = NULL;
	}
	m_pButtonExClass = NULL;
	m_pButtonTakeClass = NULL;
	m_pCover = NULL;
}

CDnJobChangeDlg::~CDnJobChangeDlg(void)
{

}

void CDnJobChangeDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("ClassChoiceDlg.ui").c_str(), bShow);
}

void CDnJobChangeDlg::InitialUpdate()
{
	//GetControl<CEtUIButton>("ID_BT_APPELLATION");
	for (int i = 0; i < 2; i++)
	{
		//	m_pMovieControl[i] = GetControl<CDnMovieControl>(FormatA("ID_MOVIE_CLASS%d", i).c_str());

		m_pSelectedClass[i] = GetControl<CEtUIStatic>(FormatA("ID_STATIC_SELECT%d", i).c_str());
		m_pSelectedClass[i]->Show(false);

		m_pSelectButton[i] = GetControl<CEtUIButton>(FormatA("ID_BT_SELECT%d", i).c_str());

		m_pClassName[i] = GetControl<CEtUIStatic>(FormatA("ID_TEXT_CLASS%d", i).c_str());
		m_pJobIcon[i] = GetControl<CDnJobIconStatic>(FormatA("ID_STATIC_CLASS%d", i).c_str());
		m_pClassInfoText[i] = GetControl<CEtUIStatic>(FormatA("ID_TEXT_CLASSINFO%d", i).c_str());

		m_pSkillButtonA[i] = GetControl<CDnSkillSlotButton>(FormatA("ID_BT_SKILL_A%d", i).c_str());
		m_pSkillButtonB[i] = GetControl<CDnSkillSlotButton>(FormatA("ID_BT_SKILL_B%d", i).c_str());

		//ID_TEXT_CLASS0 //static
		//ID_STATIC_CLASS0 // jobicon
		//ID_TEXT_CLASSINFO0 //staic

		//ID_BT_SKILL_A0 //button
		//ID_BT_SKILL_B0 //button

		GetControl<CEtUIStatic>(FormatA("ID_STATIC_FLAG%d", i).c_str())->Show(false);
	}
	m_pButtonExClass = GetControl<CEtUIButton>("ID_BT_EXCLASS");
	m_pButtonTakeClass = GetControl<CEtUIButton>("ID_BT_TAKECLASS");
	m_pButtonExClass->Enable(false);
	m_pButtonTakeClass->Enable(false);
	//ID_BT_EXCLASS
	//ID_BT_TAKECLASS
	m_pCover = GetControl<CEtUIStatic>("ID_STATIC_COVER");
	m_pCover->Show(false);
	//ID_STATIC_COVER //
}

void CDnJobChangeDlg::InitCustomControl(CEtUIControl *pControl)
{
	// 이렇게 static_cast<classtype*>(GetControl 만 할거라면 InitialUpdate에서 해도 충분하다.
	for (int i = 0; i < 2; i++)
		m_pMovieControl[i] = GetControl<CDnMovieControl>(FormatA("ID_MOVIE_CLASS%d", i).c_str());
}

void CDnJobChangeDlg::Process(float fElapsedTime)
{

	CDnCustomDlg::Process(fElapsedTime);
}

void CDnJobChangeDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (strcmp(pControl->GetControlName(), "ID_BT_CLOSE") == 0)
		{
			this->Reset();
			this->Show(false);
		}
		if (strcmp(pControl->GetControlName(), "ID_BT_SELECT0") == 0)
		{
			this->BtnSelectClass(0);
		}
		if (strcmp(pControl->GetControlName(), "ID_BT_SELECT1") == 0)
		{
			this->BtnSelectClass(1);
		}
		if (strcmp(pControl->GetControlName(), "ID_BT_EXCLASS") == 0)
		{
			//place holder.
		}
		if (strcmp(pControl->GetControlName(), "ID_BT_TAKECLASS") == 0)
		{
			SendSpecializePacket(m_pClassID[nSelectedIndex]);
			this->Reset();
			this->Show(false);
			GetInterface().ShowInstantSpecializationBtn(false);
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnJobChangeDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	for (int i = 0; i<2; i++)
	{
		if (m_pMovieControl[i])
		{
			m_pMovieControl[i]->Show(bShow);

			if (bShow)
				if (m_pMovieControl[i] && m_pMovieFile[i])
					m_pMovieControl[i]->Play(m_pMovieFile[i], true);

			if (!bShow)
				m_pMovieControl[i]->Stop();
		}
	}

	CEtUIDialog::Show(bShow);
}

void CDnJobChangeDlg::BtnSelectClass(int id)
{
	if (!m_pButtonTakeClass->IsEnable())
		m_pButtonTakeClass->Enable(true);

	nSelectedIndex = id;

	//Set Button State
	for (int i = 0; i<2; i++)
	{
		m_pSelectButton[i]->Enable(true);
		m_pSelectedClass[i]->Show(false);
	}

	//
	m_pSelectButton[id]->Enable(false);
	m_pSelectedClass[id]->Show(true);
}

void CDnJobChangeDlg::InitClass(int ClassID)
{
	std::vector<JobData> stdClassVec = this->CalcNextClass(ClassID);

	//	ASSERT(stdClassVec.size() == 0);
	//	ASSERT(stdClassVec.size() > 2);


	if (stdClassVec.size() == 1)
	{
		GetControl<CEtUIStatic>("ID_STATIC_FLAG1")->Show(false);
		m_pCover->Show(true);
	}


	DnSkillHandle hNewSkill[2]; //ok
	for (DWORD i = 0; i < stdClassVec.size(); i++)
	{
		m_pClassName[i]->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, stdClassVec[i].nJobName));
		m_pClassInfoText[i]->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, stdClassVec[i].nJobDescriptionID));

		m_pJobIcon[i]->SetIconID(stdClassVec[i].nJobID);

		m_pClassID[i] = stdClassVec[i].nJobID;

		m_pMovieFile[i] = stdClassVec[i].nJobVideo;

		std::vector<std::string> tokens;
		TokenizeA(stdClassVec[i].nSkill, tokens, ";");
		if (i == 0) //A
		{
			for (int j = 0; j<2; j++)
			{
				hNewSkill[j] = CDnSkill::CreateSkill(CDnActor::Identity(), atoi(tokens[j].c_str()), 1);

				m_pSkillButtonA[j]->SetSlotType(ST_SKILL);
				m_pSkillButtonA[j]->SetSlotState(SKILLSLOT_ENABLE);
				m_pSkillButtonA[j]->SetItem(hNewSkill[j], 1);
				m_pSkillButtonA[j]->Show(true);
			}
		}
		if (i == 1) //B
		{
			for (int j = 0; j < 2; j++)
			{
				hNewSkill[j] = CDnSkill::CreateSkill(CDnActor::Identity(), atoi(tokens[j].c_str()), 1);

				m_pSkillButtonB[j]->SetSlotType(ST_SKILL);
				m_pSkillButtonB[j]->SetSlotState(SKILLSLOT_ENABLE);
				m_pSkillButtonB[j]->SetItem(hNewSkill[j], 1);
				m_pSkillButtonB[j]->Show(true);
			}
		}


		if (stdClassVec[i].nRecomm == 1)
			GetControl<CEtUIStatic>(FormatA("ID_STATIC_FLAG%d", i).c_str())->Show(true);
	}

}

std::vector<CDnJobChangeDlg::JobData> CDnJobChangeDlg::CalcNextClass(int nClassID)
{
	int nSearchForClass = nClassID;
	std::vector<JobData> stdClassVec;// = new std::vector();
	DNTableFileFormat * pTable = GetDNTable(CDnTableDB::TJOB);
	if (!pTable)
		return stdClassVec;

	for (int i = 0; i < pTable->GetItemCount(); i++)
	{
		int nItem = pTable->GetItemID(i);
		int nParentJob = pTable->GetFieldFromLablePtr(nItem, "_ParentJob")->GetInteger();
		int nService = pTable->GetFieldFromLablePtr(nItem, "_Service")->GetInteger();
		if (nParentJob == nSearchForClass && nService == 1)
		{
			JobData data;
			data.nJobID = nItem;
			data.nJobDescriptionID = pTable->GetFieldFromLablePtr(nItem, "_JobDescriptionID")->GetInteger();
			data.nJobName = pTable->GetFieldFromLablePtr(nItem, "_JobName")->GetInteger();
			data.nJobVideo = pTable->GetFieldFromLablePtr(nItem, "_JobMv")->GetString();
			data.nSkill = pTable->GetFieldFromLablePtr(nItem, "_ExSkill")->GetString();
			data.nRecomm = pTable->GetFieldFromLablePtr(nItem, "_Recom")->GetInteger();
			stdClassVec.push_back(data);
		}
	}

	return stdClassVec;

}

void CDnJobChangeDlg::Reset()
{
	m_pCover->Show(false);
	m_pButtonTakeClass->Enable(false);
	for (int i = 0; i < 2; i++)
	{
		if (m_pMovieControl[i])
			m_pMovieControl[i]->Stop();

		m_pSelectedClass[i]->Show(false);
		m_pSelectButton[i]->Enable(true);
	}
}