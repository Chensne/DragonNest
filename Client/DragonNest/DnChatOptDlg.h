#pragma once

#include "EtUIDialog.h"
#include "DnChatOption.h"

class CEtUICheckBox;

class CDnChatOptDlg : public CEtUIDialog, public CDnChatOption
{
public:
	CDnChatOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatOptDlg(void);
	
protected:
	CEtUICheckBox *m_pCheckBoxNormal;
	CEtUICheckBox *m_pCheckBoxParty;
	CEtUICheckBox *m_pCheckBoxGuild;
	CEtUICheckBox *m_pCheckBoxPrivate;
	CEtUICheckBox *m_pCheckBoxSystem;
#ifdef PRE_MOD_CHATBG
	CEtUICheckBox* m_pCheckBoxBackground;
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	CEtUICheckBox* m_pCheckBoxPrivateChannel;
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	int m_nChatTabType;
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	//CEtUICheckBox *m_pCheckBoxFriendLog;
	//CEtUICheckBox *m_pCheckBoxGuildLog;

	//CEtUICheckBox *m_pCheckBoxQuest;
	//CEtUICheckBox *m_pCheckBoxDamage;
	//CEtUICheckBox *m_pCheckBoxMonster;
	//CEtUICheckBox *m_pCheckBoxLevel;
	//CEtUICheckBox *m_pCheckBoxBuff;
	//CEtUICheckBox *m_pCheckBoxSkill;
	//CEtUICheckBox *m_pCheckBoxPartyDeath;
	//CEtUICheckBox *m_pCheckBoxItem;

public:
	void InitOption( int nChatType );

	void SetCheckNomal( bool bCheck );
	void SetCheckParty( bool bCheck );
	void SetCheckGuild( bool bCheck );
	void SetCheckPrivate( bool bCheck );
	void SetCheckSystem( bool bCheck );
#ifdef PRE_MOD_CHATBG
	void SetCheckBackground(bool bCheck, bool bOnlyCheckFlag);
	bool IsCheckBackground() const { return m_pCheckBoxBackground->IsChecked(); }
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	void SetCheckPrivateChannel( bool bCheck );
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	int GetChatTabType() { return m_nChatTabType; }
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
	//void SetCheckChannel( bool bCheck );

	//void SetCheckFriendLog( bool bCheck );	
	//void SetCheckGuildLog( bool bCheck );	

	//void SetCheckQuest( bool bCheck );	
	//void SetCheckDamage( bool bCheck );		
	//void SetCheckMonster( bool bCheck );
	//void SetCheckLevel( bool bCheck );
	//void SetCheckBuff( bool bCheck );		
	//void SetCheckSkill( bool bCheck );		
	//void SetCheckPartyDeath( bool bCheck );
	//void SetCheckItem( bool bCheck );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
