#pragma once
#include "DnCustomDlg.h"
#include "DnRenderAvatar.h"
#include "DnAlteaTask.h"

#if defined( PRE_ALTEIAWORLD_EXPLORE )

class CDnAlteaBoardDlg : public CDnCustomDlg, public CEtUICallback
{
public :
	struct SSlotData
	{
		int m_nMapIndex;
		std::wstring m_wszMapName;
		std::wstring m_wszMissionName;

		CEtUITextureControl * m_pClass_TC;
		CEtUIRadioButton * m_pBaseButton;

		SSlotData() : m_nMapIndex( 0 ), m_pClass_TC( NULL ), m_pBaseButton( NULL )
		{}
	};

	enum{
		E_ALTEA_BOARD_COUNT = 24,
	};

public:
	CDnAlteaBoardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAlteaBoardDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Render( float fElapsedTime );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

public:
	void Initialize_RenderEtc();
	void Initialize_Dice();
	void Initialize_Board();
	void SetDiceResult( const int nDice );

protected:
	void SelectSlot( const char * szControlName );
	void SetPlayerIcon();
	void ProcessPlayerIcon( float fElapsedTime );
	void ProcessEnter( float fElapsedTime );
	void SetGoldKeyCount();

	void SetDiceObject( int nDice );
	int SetDiceFaceNumber( int nDice );
	void ProcessDice( float fElapsedTime );

	void SetAutoDiceCount();
	void ProcessAutoDiceCount( float fElapsedTime );

	void SetSound( bool bAlteaSount );

protected:
	SSlotData m_pSlotData[ E_ALTEA_BOARD_COUNT ];

	CEtUIStatic * m_pKeyCount_Static;
	CEtUIStatic * m_pNotice_Static;
	CEtUIStatic * m_pAutoDiceCount_Static;

	CEtUIButton * m_pDice_Button;
	CEtUIButton * m_pClose_Button;
	CEtUIButton * m_pDrawFrameBtn;

	EtTextureHandle m_hClassTexture;
	EtTextureHandle m_hBgTexture;

	CDnRenderAvatarNew m_RenderEtc;

	int m_nNowPosition;
	int m_nTargetPosition;

	int m_nMoveSound;
	int m_nDiceSound;
	int m_nBGM;

	EtSoundChannelHandle m_hBGM;

	bool m_bProcessIcon;
	bool m_bProcessDice;
	bool m_bProcessEnter;
	bool m_bAutoDice;
	bool m_bReadyDice;
	float m_fIconTime;
	float m_fDiceTime;
	float m_fEnterTime;
	float m_fAutoDiceTime;
	float m_fReadyDice;

	std::pair<int, int> m_DiceTextureIndex[CDnAlteaTask::E_DICE_MAXNUMBER];
	std::vector<int> m_vecDiceSubMeshIndex;
};

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )