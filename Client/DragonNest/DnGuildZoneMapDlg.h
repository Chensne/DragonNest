#pragma once

#include "DnCustomDlg.h"
#include "DnPartyTask.h"

class CDnMutatorGuildWarMode;

class CDnGuildWarZoneMapDlg : public CDnCustomDlg
{	
public:
	CDnGuildWarZoneMapDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWarZoneMapDlg(void);

	enum{
		NONE_FLAG = 0,
		BLUE_PROCESS_FLAG,
		BLUE_SUCCESS_FLAG,
		RED_PROCESS_FLAG,
		RED_SUCCESS_FLAG,
		MAX_FLAG,
	};

	enum{
		DOOR_COUNT = 3,
		ALARM_TIME = 5,	//5초
	};

	struct SZonePartyButtonInfo	//파티원 및 적군 표현
	{
		CEtUIButton *pButton;
		bool bPartyMember;
		UINT nSessionID;

		SZonePartyButtonInfo() : pButton( NULL ), bPartyMember( false ), nSessionID( 0 )
		{}
	};

	struct SFlagButtonInfo	//거점 표현
	{
		CEtUIButton * pButton[MAX_FLAG];
		EtVector3 m_Position;
		int m_nAreaID;
		std::wstring wszName;

		SFlagButtonInfo() : m_nAreaID(0)
		{
			SecureZeroMemory( pButton, sizeof(pButton) );
		}

		void Clear()
		{
			for( int itr = 0; itr < MAX_FLAG; ++itr )
			{
				if( NULL == pButton[itr] )
					continue;

				pButton[itr]->Show( false );
			}
		}
	};

	struct SZoneDoorInfo	//성문 표현
	{
		CEtUIStatic * pStaticDoor;
		CEtUIStatic * pStaticBrokenDoor;
		CEtUIStatic * pStaticText;
		int nTableID;
		int nSessionID;
		int nAreaID;
		TCHAR * szName;
		EtVector3 vPosition;

		SZoneDoorInfo() : pStaticDoor(NULL), pStaticBrokenDoor(NULL), pStaticText(NULL), nTableID(0), nSessionID(0), nAreaID(0), szName( NULL )
		{}
	};

	struct SZoneBossInfo	//보스 표현
	{
		CEtUIStatic * pStaticBoss;
		CEtUIStatic * pStaticText;
		int nTableID;
		int nSessionID;

		SZoneBossInfo() : pStaticBoss(NULL), pStaticText(NULL), nTableID(0), nSessionID(0)
		{}
	};

	struct SZoneAlarmInfo	//경보 표현
	{
		CEtUIStatic * pStaticAlarm;
		int nSessionID;
		float fDurationTime;

		SZoneAlarmInfo() : pStaticAlarm(NULL), nSessionID(0), fDurationTime(0.f)
		{}
	};

protected:

	CEtUITextureControl *m_pTextureMinimap;
	
	CEtUIButton * m_pButtonFlag[MAX_FLAG];
	
	CEtUIButton * m_pButtonParty;
	CEtUIButton * m_pButtonPartyMember;
	CEtUIButton * m_pButtonEnemy;

	CEtUIButton * m_pButtonMousePoint;
	
	CEtUIStatic * m_pStaticPlayer;
	
	CEtUIStatic * m_pStaticBlueDoor;
	CEtUIStatic * m_pStaticBlueBrokenDoor;
	CEtUIStatic * m_pStaticRedDoor;
	CEtUIStatic * m_pStaticRedBrokenDoor;
	
	CEtUIStatic * m_pStaticBlueBoss;
	CEtUIStatic * m_pStaticRedBoss;

	CEtUIStatic * m_pStaticAlarm;

	CEtUIStatic * m_pStaticBlueText;
	CEtUIStatic * m_pStaticRedText;

	bool m_bEnemyShow;

	EtTextureHandle m_hMinimap;
	SUICoord m_TextureCoord;
	float m_fMapWidth;
	float m_fMapHeight;
	SUICoord m_UVCoord;

	float m_fMousePointTime;

	std::list<SZonePartyButtonInfo> m_listButtonParty;
	std::list<SZonePartyButtonInfo> m_listButtonEnemy;
	std::list<SFlagButtonInfo>		m_listButtonFlag;
	std::list<SZoneDoorInfo>		m_listDoor;
	std::list<SZoneBossInfo>		m_listBoss;
	std::list<SZoneAlarmInfo>		m_listAlarm;

	void ControlPosition( CEtUIControl * pControl, const TCHAR * szName, EtVector3 vPosition, CEtUIControl * pTextControl = NULL );
	int FindMonsterUniqueID( int nTableID );

	void AddGuildWarUserButton( DnActorHandle hActor );
	void AddOccupationUserButton( DnActorHandle hActor );
	void AddDoorStatic( int eTeamIndex, int nDoorID, int nAreaID );
	void AddBossStatic( int eTeamIndex, int nBossID );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void ProcessUser();
	void ProcessAlarm( float fElapsedTime );
	void ProcessBoss();
	void ProcessDoor();
	void ProcessBuff();
	void ProcessPosition( float fElapsedTime );
	void ProcessMousePointer( float fElapsedTime );

	void AddUser( DnActorHandle hActor, bool bIsGuildWar );
	void AddAlarm( DnActorHandle hActor );
	void OnMousePointer( EtVector3 vPos );

	void ProcessFlag();
	void CreateFlagControl();
	void CreateDoorBossControl();

	void SetMap();

	void PostInitialize();
};
