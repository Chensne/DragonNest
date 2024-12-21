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
	CManufactureSkill*		m_pCookingSkill;	// 요리스킬 저장소
	CSecondarySkillRecipe*	m_pCookRecipe;		// 제작중인 요리레시피
	bool					m_bStartManufacture;// 요리 시작 Send 다중처리 안된도록 막음
	bool					m_bRequestCooking;
	bool					m_bManufactureLoop;	// 연속 제작
	bool					m_bRecieveResult;	// 결과를 받았을 경우 프로세스에서 2초 뒤 재시작 여부 결정
	bool					m_bCooking;			// 현제 요리 중인지
	float					m_fTimer;
	CDnItem*				m_pRequestRecipe;	// 레시피 추가요청 아이템 임시 포인터
};

#define GetLifeSkillCookingTask()		CDnLifeSkillCookingTask::GetInstance()

#endif // PRE_ADD_COOKING_SYSTEM
