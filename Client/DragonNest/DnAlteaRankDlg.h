#pragma once
#include "DnCustomDlg.h"

#if defined( PRE_ALTEIAWORLD_EXPLORE )

class CDnAlteaRankDlg : public CDnCustomDlg
{
public:
	enum{
		E_PERSONAL_TYPE_DIALOG = 0,
		E_GUILD_TYPE_DIALOG = 1,

		E_GOLDKEY_RANK = 0,
		E_PLAYTIME_RANK = 1,
	};

public:
	CDnAlteaRankDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAlteaRankDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	void SetGuildRankDlg();

	void SetMyGoldKeyCount( const int nCount );
	void SetMyPlayTime( const UINT nPlayTime );

	void SetPersonalGoldKeyRank( std::vector<AlteiaWorld::GoldKeyRankMemberInfo> & vPersonalGoldKey );
	void SetPersonalPlayTimeRank( std::vector<AlteiaWorld::PlayTimeRankMemberInfo> & vPersonalPlayTime );
	void SetGuildGoldKeyRank( std::vector<AlteiaWorld::GuildGoldKeyRankInfo> & vGuildGoldKey );

protected:
	void RefreshMyInfo();
	void RefreshNotice( int nRankType );

protected:
	CEtUIRadioButton * m_pGlodKey_Button;
	CEtUIRadioButton * m_pTime_Button;

	CDnJobIconStatic * m_pClass_Static;
	CEtUITextureControl * m_pGuildMark;
	CEtUIStatic * m_pTitleStatic;
	CEtUIStatic * m_pName_Static;
	CEtUIStatic * m_pGuildName_Static;
	CEtUIStatic * m_pKeyCount_Static;
	CEtUIStatic * m_pNotice_Static;

	CEtUIListBoxEx * m_pListBox;

	int m_eDialogType;
};

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )