#pragma once

#include "Singleton.h"

//const byte cOptionNumber = 17;

class CGameOption : public TGameOptions, public CSingleton<CGameOption>
{
public:
	enum emGraphicQuality
	{
		Graphic_High,
		Graphic_Middle,
		Graphic_Low,
	};
#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	enum emTextureQuality
	{
		Texture_High,
		Texture_Low,
	};
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	enum emEffectQuality
	{
		Effect_High,
		Effect_Middle,
		Effect_Low
	};
#endif

	enum emSoundQuality
	{
		Sound_High,
		Sound_Middle,
		Sound_Low,
	};

public:
	CGameOption();
	virtual ~CGameOption();

protected:
	float m_fMouseSensitivity;
	float m_fCursorSensitivity;
	float m_fCameraSensitivity;

protected:
	//blondy
	bool SetOptionValue( FILE *stream , char * szString );	
	//blondy end

	// 채팅매크로 4개는 다른 파일에 저장해달라고 한다.
	bool LoadMacro();
	bool SaveMacro();

public:
	bool Load();
	bool Save();

public:
	int m_nWidth;
	int m_nHeight;
	bool m_bWindow;
	bool m_bVSync;
#ifdef PRE_ADD_MULTILANGUAGE
	int m_nLanguageID;
#endif // PRE_ADD_MULTILANGUAGE
#ifdef PRE_ADD_37745
	bool m_bHideWeaponByBattleMode;
#endif

	bool m_bExtendView;

#ifdef PRE_ADD_CASH_AMULET
	bool m_bShowCashChatBalloon;
#endif

#ifdef 	PRE_ADD_VIEW_OPITION_PARTY_HEAL
	bool m_bShowPartyHealInfo;
#endif

	bool m_bHideQuestMarkByLevel;
	bool m_bEnableQuestNotifyPriority;

#ifdef PRE_ADD_AUTO_DICE
	bool m_bEnableAutoDice; // 던젼 주사위 자동굴림
#endif
#ifdef PRE_ADD_CHAR_SATUS_RENEW
	int m_nCharStatusPrimaryPage;
	int m_nCharStatusSecondaryPage;
#endif
	int m_nGraphicQuality;
	int m_nCurGraphicQuality;

#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	int m_nTextureQuality;
	int m_nCurTextureQuality;
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	int m_nEffectQuality;
	int m_nCurEffectQuality;
#endif

	int m_nBrightness;
	int m_nUISize;
	int m_nScreenShotFormat;
#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
	int m_nMemoryOptimize;
#endif

	float m_fTotalVolume;
	float m_fBGMVolume;
	float m_f3DVolume;
	float m_f2DVolume;
	float m_fVoiceVolume;

	bool m_bTotalMute;
	bool m_bBGMMute;
	bool m_b3DMute;
	bool m_b2DMute;
	bool m_bVoiceMute;

	int m_nSoundQuality;

	bool m_bSmartMove;
	bool m_bSmartMoveMainMenu;

	bool m_bDefaultSmartMove;
	bool m_bDefaultSmartMoveMainMenu;
	float m_fDefaultMouseSensitivity;

	int m_nRecordResolution;
	int m_nRecordFPS;
	int m_nRecordVideoQuality;
	int m_nRecordAudioQuality;

	bool m_bVoiceChat;
	float m_fVoiceChatVolume;
	float m_fMicVolume;
	bool m_bMicTest;
	bool m_bMicBoost;
	bool m_bUseAutoAdjustVolume;
	float m_fAutoAdjustVolume;
	int m_nVoiceFont;
	float m_fVoicePitch;
	float m_fVoiceTimbre;

	int m_nWndPosX;
	int m_nWndPosY;

	bool m_bMouseInvert;

	int m_nInputDevice;

	int m_nMiniSiteVersion;

#ifdef PRE_MOD_CHATBG
	bool m_bChatBackground;
#endif

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	bool m_bEnablePlayerEffect;
#endif

#ifdef PRE_ADD_STAMPSYSTEM
	int m_nWeekDay;
	int m_nShowStampAlram;
#endif // PRE_ADD_STAMPSYSTEM

public:
	// Note : 정보 출력 옵션
	//

	// Note : 커뮤니티 관련 옵션
	//
	bool m_bCommFriendHideOffline;
	bool m_bCommGuildHideOffline;

	enum {
		NUM_COMM_MACROS = 4,
	};
	bool m_bDefaultMacro[ NUM_COMM_MACROS ];
	wchar_t m_wszMacro[ NUM_COMM_MACROS ][ 255 ];

	// Note : 도움말 옵션
	//
	bool m_bHelpCaption;

	// 퀘스트 알리미 출력
public:
	void CheckValidGraphicOption();
	void ApplyGraphicOption();
	void ChangeGraphicQuality();
	void ChangeDisplayMode();

	SGraphicOption GetGraphicOption();

public:
	void ApplyControl();
	void CancelControl();

	void SetMouseSensitivity( float fValue );
	float GetMouseSendsitivity();

	void SetCursorSensitivity( float fValue )	{ m_fCursorSensitivity = fValue; }
	float GetCursorSensitivity()				{ return m_fCursorSensitivity; }

	void SetCameraSensitivity( float fValue )	{ m_fCameraSensitivity = fValue; }
	float GetCameraSensitivity()				{ return m_fCameraSensitivity; }

	int GetCurGraphicQuality() { return m_nCurGraphicQuality; }
#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	int GetCurTextureQuality() { return m_nCurTextureQuality; }
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	int GetCurEffectQuality() { return m_nCurEffectQuality; }
#endif

	void SetMouseInvert( bool bMouseInvert )	{ m_bMouseInvert = bMouseInvert; }
	bool GetMouseInvert()						{ return m_bMouseInvert; }

	void SetInputDevice( int nInputDevice )		{ m_nInputDevice = nInputDevice; }
	int  GetInputDevice()						{ return m_nInputDevice; }
	void ApplyInputDevice();

public:
	void ApplySound();

	float GetVolumeTotal();
	float GetVolumeBGM();
	float GetVolume3D();
	float GetVolume2D();
	float GetVolumeVoice();

	void SetVolumeTotal( float fVolume );
	void SetVolumeBGM( float fVolume );
	void SetVolume3D( float fVolume );
	void SetVolume2D( float fVolume );
	void SetVolumeVoice( float fVolume );

	bool IsMuteTotal();
	bool IsMuteBGM();
	bool IsMute3D();
	bool IsMute2D();
	bool IsMuteVoice();

	void SetMuteTotal( bool bMute );
	void SetMuteBGM( bool bMute );
	void SetMute3D( bool bMute );
	void SetMute2D( bool bMute );
	void SetMuteVoice( bool bMute );

public:
	void ApplyVoiceChat();
};