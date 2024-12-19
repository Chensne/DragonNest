#pragma once
#include "EtUIDialog.h"

#if defined(PRE_ADD_DWC)

struct sDwcMemberListData
{
};

class CDnDwcMemberListDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnDwcMemberListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnDwcMemberListDlg();
	
protected:
	CEtUIStatic* m_pStaticTitleName;// 직책
	CEtUIStatic* m_pStaticName;		// 이름
	CEtUIStatic* m_pStaticClass;		// 클래스
	CEtUIStatic* m_pStaticCurrentPlace;	// 현재 위치
	CEtUIStatic* m_pStaticLevel;		// 레벨

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );

	void SetData(TDWCTeamMember data);
};

#endif // PRE_ADD_DWC