#pragma once
#include "EtUIDialog.h"
#include "DnVillageTask.h"
#include "DnInterfaceString.h"

class CDnPartyEnterDlg : public CEtUIDialog
{
public:
#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
	CDnPartyEnterDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
#else
	CDnPartyEnterDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
#endif 
	
	virtual ~CDnPartyEnterDlg(void);

	struct SPartyEnterUnit
	{
		CEtUIStatic* pMasterMark;
		CEtUIStatic* pName;
		CEtUIStatic* pLevel;
		CEtUIStatic* pJob;		
#ifdef PRE_ADD_NEWCOMEBACK
		CEtUIStatic* pComeback;
#endif // PRE_ADD_NEWCOMEBACK

		void Set(const CDnVillageTask::SPartyMemberInfo& info);
		void Clear();
	};

protected:
	CEtUIStatic* m_pStaticPartyName;
	CEtUIStatic* m_pStaticTargetStage;

	std::vector<SPartyEnterUnit> m_PartyMemberInfoList;

	CEtUIButton* m_pConfirmBtn;
	CEtUIButton* m_pCancelBtn;

#ifdef PRE_PARTY_DB
	TPARTYID m_PartyIdx;
#else
	int m_PartyIdx;
#endif
	ePartyType m_Type;

	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow, ePartyType type );
	virtual void Show( bool bShow );
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
#ifdef PRE_PARTY_DB
	void SetPartyInfo(TPARTYID partyIdx, const std::wstring& partyName, const std::wstring& targetStageName);
	virtual int GetPassword() { return Party::Constants::INVALID_PASSWORD; }
#else
	void SetPartyInfo(int partyIdx, const std::wstring& partyName, const std::wstring& targetStageName);
	virtual LPCWSTR GetPassword() { return NULL; }
#endif

#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
	inline void SetPartyType( ePartyType partytype )  { m_Type = partytype; }
#endif 

private:
	void ClearMemberInfo();
	bool DoEnter();
};