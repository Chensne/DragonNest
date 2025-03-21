#pragma once
#include "DnCustomDlg.h"

#if defined(PRE_ADD_DWC)
class CDnPVPDWCTabDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	enum eDWCChannel
	{
		eChannel_Normal = 0,
		eChannel_DWC,
		eChannel_DWC_Practice,
		eChannel_None,

		eChannel_MAX,
	};
	void SetDWCRadioButton(eDWCChannel eChannelType);
	void SetDWCRadioButton(LadderSystem::MatchType::eCode eCode);
	int GetSelectedLadderChannel() const { return m_nSelectedLadderChannel; }
	PvPCommon::RoomType::eRoomType GetSelectedGradeChannel( void ) { return m_eSelectedGradeChannel; };
	void PushUIRadioButton(const char* szControlName);
protected:
	int m_nSelectedLadderChannel;
	CEtUIRadioButton* m_pChannelRadioBtn[eChannel_MAX];
	PvPCommon::RoomType::eRoomType m_eSelectedGradeChannel;

public:					 
	CDnPVPDWCTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPDWCTabDlg();

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );
};
#endif // PRE_ADD_DWC