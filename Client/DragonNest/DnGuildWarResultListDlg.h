#pragma once
#include "EtUIDialog.h"
#include "DnGuildWarResultDlg.h"

class CDnGuildWarResultListDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnGuildWarResultListDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnGuildWarResultListDlg(void);

protected:
	CDnGuildWarResultDlg::SUserInfo m_Data;

	enum{
		eTeam_My = 0,
		eTeam_Blue,
		eTeam_Red,
		eTeam_Max,
	};

	struct SUIDataSet
	{
		CEtUIStatic * m_pStaticLevel;
		CEtUIStatic * m_pStaticGuildName;
		CEtUIStatic * m_pStaticPlayerName;
		CEtUIStatic * m_pStaticKillCount;
		CEtUIStatic * m_pStaticDeathCount;
		CEtUIStatic * m_pStaticAssistCount;
		CEtUIStatic * m_pStaticScore;
		CEtUIStatic * m_pStaticBasePoint;
		CEtUIStatic * m_pStaticMedal;

		CDnJobIconStatic * m_pStaticJob;

		CEtUITextureControl * m_pTextureRank;
		CEtUITextureControl * m_pTextureMark;

		SUIDataSet() : m_pStaticLevel( NULL ), m_pStaticGuildName( NULL ), m_pStaticPlayerName( NULL ), m_pStaticKillCount( NULL ), m_pStaticDeathCount( NULL )
			, m_pStaticAssistCount( NULL ), m_pStaticScore( NULL ), m_pStaticBasePoint( NULL ), m_pStaticMedal( NULL ), m_pStaticJob( NULL )
			, m_pTextureRank( NULL ), m_pTextureMark( NULL )
		{}

		void Show( bool bShow )
		{
			m_pStaticLevel->Show( bShow );
			m_pStaticGuildName->Show( bShow );
			m_pStaticPlayerName->Show( bShow );
			m_pStaticKillCount->Show( bShow );
			m_pStaticDeathCount->Show( bShow );
			m_pStaticAssistCount->Show( bShow );
			m_pStaticScore->Show( bShow );
			m_pStaticBasePoint->Show( bShow );
			m_pStaticMedal->Show( bShow );

			m_pStaticJob->Show( bShow );

			m_pTextureRank->Show( bShow );
			m_pTextureMark->Show( bShow );
		}
	};

	CEtUIStatic * m_pStaticRedBar;
	CEtUIStatic * m_pStaticBlueBar;

	SUIDataSet m_UIDataSet[eTeam_Max];

public:
	void SetList( CDnGuildWarResultDlg::SUserInfo & data );
	UINT GetSessionID()	{ return m_Data.nSessionID; }

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
};
