#pragma once
#include "DnCustomDlg.h"

class CDnCharStatusDetailInfoDlg : public CDnCustomDlg
{
public:
	enum eDetailInfoAttributes
	{
		CRITICAL_PROB,
		STUN_PROB,
		STIFF_PROB,

		CRITICAL_RESIST,
		STUN_RESIST,
		STIFF_RESIST,

		FIRE_ATTACK,
		ICE_ATTACK,
		LIGHT_ATTACK,
		DARK_ATTACK,

		FIRE_DEFENSE,
		ICE_DEFENSE,
		LIGHT_DEFENSE,
		DARK_DEFENSE,
		MAX,
	};

protected:
	CDnCharStatusStatic *m_pStatic_Attribute[eDetailInfoAttributes::MAX];
	bool m_bActorStateRefresh;

public:
	CDnCharStatusDetailInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharStatusDetailInfoDlg(void);
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Render( float fElapsedTime );
	virtual void Show( bool bShow );

	void RefreshStatus();
	void OnRefreshPlayerStatus();
};
