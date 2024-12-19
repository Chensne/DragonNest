#pragma once

#include "Task.h"
#include "MessageListener.h"

#ifdef PRE_ADD_COOKING_SYSTEM

class CSecondarySkillRecipe;
class CManufactureSkill;
class CDnItem;

class CDnLifeSkillCookingTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDnLifeSkillCookingTask>
{
public:
	CDnLifeSkillCookingTask();
	virtual ~CDnLifeSkillCookingTask();

public:
	bool Initialize();
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

public:
	CManufactureSkill* GetCookingSkill() { return m_pCookingSkill; }

	void SendAddRecipe( CDnItem* pRecipeItem );
	void SendDeleteRecipe( int nSkillID, int nItemID );
	void SendExtractRecipe( int nSkillID, int nItemID );
	void SendManufacture( int nSkillID, int nItemID, bool bStart );
	void StartManufacture( CSecondarySkillRecipe* pCookRecipe, bool bManufactureLoop );

	void OnRecvRecipeList( SecondarySkill::SCRecipeList* pPacket );
	void OnRecvAddRecipe( SecondarySkill::SCAddRecipe* pPacket );
	void OnRecvUpdateRecipeExp( SecondarySkill::SCUpdateRecipeExp* pPacket );
	void OnRecvDeleteRecipe( SecondarySkill::SCDeleteRecipe* pPacket );
	void OnRecvExtractRecipe( SecondarySkill::SCExtractRecipe* pPacket );
	void OnRecvManufacture( SecondarySkill::SCManufacture* pPacket );
	void OnRecvCancelManufacture( SecondarySkill::SCCancelManufacture* pPacket );

	void RequestAddRecipe( CDnItem* pRecipeItem );
	bool IsNowCooking() { return m_bCooking; }
	bool IsRequestCooking() { return m_bRequestCooking; }
	void CancelManufacture( bool bReleaseLockInput = true );

private:
	bool Manufacture( CSecondarySkillRecipe* pCookRecipe );
	void CheckRecieveResult();

private:
	enum { MANUFACTURE_PROGRESS = 0, MSGBOX_REQUEST_ADD_RECIPE  };
	CManufactureSkill*		m_pCookingSkill;	// �丮��ų �����
	CSecondarySkillRecipe*	m_pCookRecipe;		// �������� �丮������
	bool					m_bStartManufacture;// �丮 ���� Send ����ó�� �ȵȵ��� ����
	bool					m_bRequestCooking;
	bool					m_bManufactureLoop;	// ���� ����
	bool					m_bRecieveResult;	// ����� �޾��� ��� ���μ������� 2�� �� ����� ���� ����
	bool					m_bCooking;			// ���� �丮 ������
	float					m_fTimer;
	CDnItem*				m_pRequestRecipe;	// ������ �߰���û ������ �ӽ� ������
};

#define GetLifeSkillCookingTask()		CDnLifeSkillCookingTask::GetInstance()

#endif // PRE_ADD_COOKING_SYSTEM
