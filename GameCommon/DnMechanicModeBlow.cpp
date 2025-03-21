#include "StdAfx.h"
#include "DnMechanicModeBlow.h"
#ifdef _CLIENT
#include "DnInterface.h"
#include "EtUIDialogGroup.h"
#include "DnQuickSlotDlg.h"
#include "SmartPtrDef.h"
#include "DnMainDlg.h"
#include "DnItemTask.h"
#endif
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



CDnMechanicModeBlow::CDnMechanicModeBlow(DnActorHandle hActor, const char* szValue) : CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_345;
	SetValue(szValue);

	DNVector(std::string) m_pVecSkill;
	TokenizeA(szValue, m_pVecSkill, ";");
	for (DWORD i = 0; i < m_pVecSkill.size();i++)
	{
		m_pVecFindSkills.push_back(atoi(m_pVecSkill[i].c_str()));
		i++;
		m_pVecNewSkills.push_back(atoi(m_pVecSkill[i].c_str()));
	}
}

CDnMechanicModeBlow::~CDnMechanicModeBlow(void)
{

}


void CDnMechanicModeBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta)
{
	OutputDebug("CDnMechanicModeBlow::OnBegin");
	OutputDebug("%s elements: %d", __FUNCTION__, m_pVecFindSkills.size());

#ifdef _CLIENT

	CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
	if (pMainBarDlg)
	{
		CEtUIDialogGroup *pQuickSlotDlgGroup = pMainBarDlg->GetQuickSlotDialogGroup();
		if (pQuickSlotDlgGroup)
		{
			for(int i=0;i<2;i++)//tab 1+2
			{
				CDnQuickSlotDlg *pEventQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(pQuickSlotDlgGroup->GetDialog(i));

				for (DWORD i = 0; i<m_pVecFindSkills.size(); i++)
				{
					int nSlot = pEventQuickSlotDlg->FindSkillQuickSlotIndexAndReplace(m_pVecFindSkills[i], m_pVecNewSkills[i]);
				}
			}
		}
	}

#endif

}


void CDnMechanicModeBlow::Process(LOCAL_TIME LocalTime, float fDelta)
{
	CDnBlow::Process(LocalTime, fDelta);
}


void CDnMechanicModeBlow::OnEnd(LOCAL_TIME LocalTime, float fDelta)
{
	OutputDebug("CDnMechanicModeBlow::OnEnd");
#ifdef _CLIENT

	CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
	if (pMainBarDlg)
	{
		CEtUIDialogGroup *pQuickSlotDlgGroup = pMainBarDlg->GetQuickSlotDialogGroup();
		if (pQuickSlotDlgGroup)
		{
			for (int i = 0; i<2; i++)//tab 1+2
			{
				CDnQuickSlotDlg *pEventQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(pQuickSlotDlgGroup->GetDialog(i));

				for (DWORD i = 0; i<m_pVecFindSkills.size(); i++)
				{
					int nSlot = pEventQuickSlotDlg->FindSkillQuickSlotIndexAndReplace(m_pVecNewSkills[i], m_pVecFindSkills[i], true);
				}
			}
		}
	}

#endif
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnMechanicModeBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}

void CDnMechanicModeBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


bool CDnMechanicModeBlow::CanUseSkill(int nSkillID)
{
	for (DWORD i = 0; i < m_pVecNewSkills.size(); i++)
	{
		if (m_pVecNewSkills[i] == nSkillID)
			return true;
	}

	return false;
}

int CDnMechanicModeBlow::GetReplaceSkill(int nSkillID)
{
	for (DWORD i = 0; i < m_pVecFindSkills.size(); i++)
	{
		if (m_pVecFindSkills[i] == nSkillID)
			return m_pVecNewSkills[i];
	}

	return 0;
}