#pragma once
#include "EtUIDialog.h"

class CDnDwcInviteReqDlg : public CEtUIDialog
{
public:
	CDnDwcInviteReqDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDwcInviteReqDlg(void);

protected:
	CEtUIProgressBar *m_pProgressBarTime;

	CEtUIStatic *m_pTitleName;
	CEtUIStatic *m_pCharacterName;
	CEtUIButton *m_pButtonOK;		// ���� ����
	CEtUIButton *m_pButtonCancel;	// ����

	float m_fTotalTime;				// ��ü�ð�
	float m_fElapsedTime;			// �����ð�(���� ó�� Show�ɶ��� ���� ���� �����ϰ� ����)

public:
	void SetInfo( LPCWSTR pwszGuildName, LPCWSTR wszCharacterName, float fTotalTime, int nID = -1, CEtUICallback *pCall = NULL );

	// ���� ���α׷����ٸ� �����Ϸ��� fElapsedTime���� ���� �־��ָ� �ȴ�.
	void SetElapsedTime( float fElapsedTime );

	// Send
	void SendReject();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};