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
	CEtUIStatic* m_pStaticTitleName;// ��å
	CEtUIStatic* m_pStaticName;		// �̸�
	CEtUIStatic* m_pStaticClass;		// Ŭ����
	CEtUIStatic* m_pStaticCurrentPlace;	// ���� ��ġ
	CEtUIStatic* m_pStaticLevel;		// ����

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );

	void SetData(TDWCTeamMember data);
};

#endif // PRE_ADD_DWC