#pragma once
#include "EtUIDialog.h"

#if defined(PRE_ADD_DWC)

class CDnDWCCharCreateDlg;
class CDnDWCCharCreateSelectDlg: public CEtUIDialog, public CEtUICallback
{	
public:
	CDnDWCCharCreateSelectDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnDWCCharCreateSelectDlg();
	enum
	{
		DWC_CREATE_PLAYER_MAX = 10,
	};

protected:
	struct sDWCCharCreateData
	{
		int nID;
		int nClassID;
		int nJobID1;
		int nJobID2;
		int nClassMidID; // 클래스 UI Mid값
		std::string strClassImageFileName;

		sDWCCharCreateData()
			: nID(-1)
			, nClassID(0)
			, nJobID1(0)
			, nJobID2(0)
			, nClassMidID(0)
		{
			strClassImageFileName.clear();
		}
	};
	std::vector<sDWCCharCreateData> m_vDWCCharCreateDataList;
	std::vector<CEtUIRadioButton*>	m_vRadioButtonList;
	CEtUIComboBox*		 m_pComboBox;
	INT64				 m_nCharDBID;
	int					 m_nSelectedTabID;
	int					 m_nCharJobID;
	CDnDWCCharCreateDlg* pParentDlg;

protected:
	void SetCharNameComboBox();
	void MakeDWCCharCreateData();
	void Reset();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

	const INT64 GetSelectCharDBID()  { return m_nCharDBID; }
	const int	GetSelectCharJobID() { return m_nCharJobID; }
};

#endif // PRE_ADD_DWC