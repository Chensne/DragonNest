#pragma once
#include "EtUIDialog.h"

class CDnGuildWantedListItemDlg : public CEtUIDialog
{
public:
	CDnGuildWantedListItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWantedListItemDlg(void);

protected:
	CEtUITextureControl *m_pTextureGuildMark;
	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticCondition;
	CEtUIStatic *m_pStaticLevel;
	CEtUIStatic *m_pStaticCount;
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CEtUIStatic *m_pStaticCategory;
	CEtUIStatic *m_pStaticIntroduce;
#endif

	TGuildUID m_GuildUID;
	int m_nLevel;
	int m_nCount;
	int m_nCountMax;
	std::wstring m_wszNotice;
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	std::wstring m_wszHomepage;
	std::wstring m_wszGuildMasterName;
#endif

public:
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	void SetInfo( TGuildUID GuildUID, EtTextureHandle hTexture, const WCHAR *wszText1, const WCHAR *wszText2, int nLevel, int nCount, int nCountMax, bool bWaitRequest, BYTE cPurposeCode, const WCHAR *wszHomepage, const WCHAR *wszGuildMasterName );
#else
	void SetInfo( TGuildUID GuildUID, EtTextureHandle hTexture, const WCHAR *wszText1, const WCHAR *wszText2, int nLevel, int nCount, int nCountMax, bool bWaitRequest );
#endif
	void SetOnWaitRequest();

	LPCWSTR GetGuildName() { return m_pStaticName->GetText(); }
	TGuildUID GetGuildUID() { return m_GuildUID; }
	int GetLevel() { return m_nLevel; }
	int GetCount() { return m_nCount; }
	int GetCountMax() { return m_nCountMax; }
	LPCWSTR GetNotice() { return m_wszNotice.c_str(); }
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	LPCWSTR GetHomepage() { return m_wszHomepage.c_str(); }
	LPCWSTR GetGuildMasterName() { return m_wszGuildMasterName.c_str(); }
#endif

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};