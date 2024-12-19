#pragma once
#include "DnCustomDlg.h"
#ifdef PRE_ADD_CHAR_SATUS_RENEW
#include "DnCharStatusBasePopDlg.h"
#endif


class CDnCharStatusBaseInfoDlg : public CDnCustomDlg
{
public:
	CDnCharStatusBaseInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharStatusBaseInfoDlg(void);

#ifdef PRE_ADD_CHAR_SATUS_RENEW
	enum ePageType
	{
		STATUS = 0,
		GENERAL = 1,
		SPECIAL_ATTACK = 2,
		SPECIAL_DEFENSE = 3,
		MAGICAL_ATTACK = 4,
		MAGICAL_DEFENSE = 5,
		SIDE_INFO = 6,
		MAX,
	};

	enum eSection
	{
		PRIMARY = 0,
		SECONDARY = 1,
		SECTIONMAX,

		INFOSIZE = 4,
		MAX_ELEMENT_SIZE = INFOSIZE * ePageType::MAX,
	};

	enum eDetailString // XX의 스텟이 [X%] 만큼 증가 합니다. 같은 디테일한 정보를 타입에 따라서 다양하게 표기.
	{
		MOVE_SPEED = 5168,
		FIRE_DAMAGE = 5169, 
		WATER_DAMAGE = 5170,
		LIGHT_DAMAGE = 5171, 
		DARK_DAMAGE = 5172,
		FIRE_DEFENSE = 5179,
		WATER_DEFENSE = 5180, 
		LIGHT_DEFENSE = 5181,
		DARK_DEFENSE = 5182,
		STUN_RATIO = 5173,  // 인자없음
		STIFF_RATIO = 5174, // 인자없음
		CRIT_RATIO = 5175,
		FINAL_DAMAGE = 5176,
		PHYSICAL_DEFENSE = 5177,
		MEGICAL_DEFENSE = 5178, 
		STUN_RESIST = 5185, // 인자없음
		STIFF_RESIT = 5184, // 인자없음
		CRIT_RESIST = 5183,
		MP_RECOVERY = 5196,
	};
#endif

protected:
	CEtUIStatic *m_pStaticClass;
	CEtUIStatic *m_pStaticLevel;
	CDnCharStatusStatic *m_pStaticHP;
	CDnCharStatusStatic *m_pStaticMP;

#if defined(PRE_ADD_CHAR_STATUS_SECOND_RENEW)
	CDnCharStatusStatic *m_pStaticMPRegen;
#else
	CEtUIStatic *m_pStaticGuild;
#endif

	bool m_bActorStateRefresh;

#ifdef PRE_ADD_CHAR_SATUS_RENEW
	int m_nEnablePage[eSection::SECTIONMAX];
	CDnCharStatusStatic *m_pStaticInfo[eSection::SECTIONMAX][eSection::INFOSIZE];
	CEtUIStatic *m_pStaticInfoName[eSection::SECTIONMAX][eSection::INFOSIZE];
	CEtUIStatic *m_pStaticSectionName[eSection::SECTIONMAX];
	CEtUIButton *m_pButtonChangePage[eSection::SECTIONMAX];
	CDnCharStatusBasePopDlg *m_pPopUpDlg;
#endif

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Render( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void OnRefreshPlayerStatus();

#ifdef PRE_ADD_CHAR_SATUS_RENEW
	void RenderStatusInfo( int nType , int nPage , bool bActorStatusRefresh );
	bool CanRefreshStatus( int nType , bool bActorStatusRefresh );
	void EnableSatusPage( int nPage , int nType );

	static int CalcDefenseRate( int nDefense , DnActorHandle hActor );
	static int CalcCriticalRate( int nCrit , DnActorHandle hActor );
	static int CalcStateBlowCriticalRate( DnActorHandle hActor );
	static int CalcCriticalRegistanceRate( int nCrit ,DnActorHandle hActor );
	static int CalcFianlDamageRate( int nFinalDanage , DnActorHandle hActor );
#endif

};