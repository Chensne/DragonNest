#pragma once

#include "Singleton.h"
#include "DnPacket.h"

/*
	전역변수 모아논 클래스. 	이클래스에 있는 값을은 최종 빌드 버전에는 제외되게 된다.  
	디버깅/치트에 관련된 값말고 전역적으로 저장되어야 하는 변수들은 CGlobalInfo 에다가 세팅하도록 한다.
*/

class CFrameSync;
class CGlobalValue : public CSingleton<CGlobalValue> {
public:
	CGlobalValue();
	~CGlobalValue();

public:
	tstring m_szID;
	tstring m_szPass;
	tstring m_szServerName;
	tstring m_szSecondPass;
	int m_nChannelIndex;

	std::string m_szNation;
	std::string m_szResourcePath;
	bool m_bSingleTest;
	bool m_bNoDamage;
	int m_nLocalPlayerClassID;
	
	tstring m_szSingleMapName;
	tstring m_szSingleMapEnviName;
	int m_nSingleMapArrayIndex;

	bool m_bSoundOff;
	int m_nPlayerLevel;
	int m_nMapLevel;
	bool m_bIgnoreCoolTime;

	int m_nNation;

	std::string m_szRequestTime;
	std::string m_szKeyID;
	
	bool m_bShowDamage;
	bool m_bShowCP;
	bool m_bShowCurrentAction;
	bool m_bShowFPS;
	bool m_bShowDPS;
	bool m_bShowSkillDPS;
	UINT m_nSumDPS;
	float m_fDPSTime;
	DWORD m_dwBeginFPSTime;
	DWORD m_dwEndFPSTime;
	float m_fMinFPS;
	float m_fMaxFPS;
	float m_fAverageFPS;
	DWORD m_dwFPSCount;

	// 디버깅용 피킹 위치
	EtVector3 m_vOutPos;
	bool	  m_bPicked;
	//---------------------
	// 개발용 유틸 코드;
	bool m_bUseLogWnd;
	int m_nDumpLevel;

	int m_iLogLevel;

	bool m_bPVPGameIgnoreCondition;

#ifdef _SHADOW_TEST
	bool m_bEnableShadow;
#endif
	bool m_bEnableWinKey;
#ifdef PRE_ADD_RENEWUI
	bool m_bReNewUI;
#endif

	//90cap new ui Ping timer.
	LARGE_INTEGER m_pPingFreq;        // ticks per second
	LARGE_INTEGER m_pPing1,m_pPing2;           // ticks
	double m_pPing;
	//

public:
	bool IsPermitLogin();
	void ProcessFPSLog( CFrameSync *pSync );

#ifdef PRE_FIX_SHOWVER
#else
	std::string MakeVersionString();
#endif

	//blondy 
	//디버깅용 리스폰 포인트 정보를 찍는다
	bool m_bEnableRespawnInfo;	
	std::vector<SCPVP_RESPAWN_POINT> m_vRespawnInfo;	
	//blondy end

	bool m_bDebugging;

	bool m_bDrawHitSignal;
	float m_fDrawHitSignalMinDelta;
//#ifndef _FINAL_BUILD
	bool IsComputerName( char *szName );
//#endif

	bool m_bMessageBoxWithMsgID;

	DWORD   m_dwSetMonsterID;
	bool	m_bShowMonsterAction;
	bool	m_bSetMonster;
	

	// #39851 저사양시 지형 텍스쳐 테스트용 2장 지정 관련.
	int m_aiSelectedMapLayerTexture[ 2 ];
};

// 전역적으로 저장되어야할 변수들은 여기에다가 세팅하도록 한다.
class CGlobalInfo : public CSingleton<CGlobalInfo>
{
public:
	CGlobalInfo();
	virtual ~CGlobalInfo();

	struct LoginConnectInfo {
		std::string szIP;
		short nPort;
	};
	std::vector<LoginConnectInfo> m_VecLoginConnectList;
	std::string m_szIP;
	std::string m_szPort;
#ifdef PRE_ADD_MULTILANGUAGE
	std::string m_szLanguage;
	MultiLanguage::SupportLanguage::eSupportLanguage m_eLanguage;
#endif // PRE_ADD_MULTILANGUAGE
#ifdef PRE_ADD_STEAMWORKS
	bool m_bUseSteamworks;
#endif // PRE_ADD_STEAMWORKS
//	short m_nPort;

	// 현재 내가 있는 맵의 인덱스
	int m_nCurrentMapIndex;	
	float m_fFadeDelta;
	LOCAL_TIME m_LocalTime;
	float	   m_fDeltaTime;

	std::wstring	 m_wszBuildVersion;

	bool m_bEnableHShield;
	char m_cLocalAccountLevel;

	int m_iGlobalMessageCode; //글로벌하게 메세지 처리 하기 위해 추가
	bool m_bUsePacking;

	bool m_bPlayingCutScene;
	bool m_bThemida_hsbgen;
	bool m_bShowVersion;

	int m_nClientCharNameLenMax;

	bool m_bUITest;
	bool m_bUseHShieldLog;

#ifdef PRE_MOD_SELECT_CHAR
	std::map<int, std::wstring>		m_mapServerList;			// 서버 리스트
	int								m_nSelectedServerIndex;		// 선택한 서버인덱스
#endif // PRE_MOD_SELECT_CHAR

public:
	void SetGlobalMessageCode( int lMessageCode ){m_iGlobalMessageCode = lMessageCode;};
	int GetGlobalMessageCode(){return m_iGlobalMessageCode;};

	DWORD GetLoginConnectInfoCount();
	LoginConnectInfo *GetLoginConnectInfo( DWORD dwIndex );
	void CalcLoginConnectInfo();

	void SetPlayingCutScene( bool bPlaying ) { m_bPlayingCutScene = bPlaying; };
	bool IsPlayingCutScene( void ) { return m_bPlayingCutScene; };

#ifdef PRE_MOD_SELECT_CHAR
	std::wstring GetServerName( int nIndex );
	int GetServerIndex( LPCWSTR szServerName );
#endif // PRE_MOD_SELECT_CHAR

#ifdef PRE_FIX_SHOWVER
	std::string MakeVersionString();
#endif
};