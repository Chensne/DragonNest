#pragma once

#ifdef _USE_VOICECHAT
#include "Singleton.h"
#include "./DolbyAxon/ClientLib/iceclientlib.h"
#include <MMSystem.h>

enum VOICE_CHAT_CODEC
{
	VOICE_CHAR_8BIT = 21,
	VOICE_CHAR_16BIT = 22,
};

class CVoiceChatClient : public CSingleton< CVoiceChatClient >
{
public:
	CVoiceChatClient(void);
	virtual ~CVoiceChatClient(void);

	enum eVoiceFontType
	{
		VoiceFontNone,
		VoiceFontW2M,
		VoiceFontM2W,
		VoiceFontELF,
		VoiceFontCustom,
	};

protected:
	ICECLIENT *m_pClient;

	// 검색된 장치들.
	unsigned int m_nNumDevice;
	ICEClient_device *m_Devices;

	// 클라이언트 단에서 자체적으로 할 수 있는 일은 볼륨조절, 변조
	float m_fVoiceChatVolume;

	// 마이크 관련해서 적당한 클래스가 없어서 보이스챗 클라이언트가 가지고 있도록 하겠다.
	HMIXER m_hMixer;
	MIXERLINE m_mxl;
	float m_fMicVolume;

	// 부스트 컨트롤이 있는지.
	bool m_bEnableMicBoost;
	void CheckMicBoostControl();

	// 윈도우 설정을 바꾸는 것들이라 초기화되었을때 값을 기억해뒀다가 끝날때 복구해둔다.
	float m_fOrigMicVolume;
	//bool m_bOrigMicBoost;	// 부스트의 경우엔 하나의 컨트롤만 얻어올 수 있는게 아니라서 그냥 안하기로 한다.
	void CheckOrigMicConfig();
	void RestoreOrigMicConfig();

public:
	bool Initialize( const char *pServerIP, unsigned short nServerPort, const char *pUserName, UINT nPrivateID, VOICE_CHAT_CODEC Codec = VOICE_CHAR_8BIT );
	static void Finalize();

	bool InitializeMicControl( ICEClient_device *pCaptureDevice );
	void FinalizeMicControl();

	int GetNumDevice() { return m_nNumDevice; }
	ICEClient_device *GetDevice( unsigned int nIndex );
	bool SetDevice( unsigned int nIndex, bool bMic );
	bool GetCaptureDeviceName( std::wstring &wszName );

	// 돌비에선 마이크를 탐색했는지.
	bool IsExistMic();

	// 마이크컨트롤이 있는지 체크
	bool IsExistMicControl() { return (m_hMixer != NULL); }
	// 자신이 말하고 있는지를 체크
	BYTE GetSpeaking();


	// 클라의 SetVolume
	void SetVoiceChatVolume( float fValue );
	float GetVoiceChatVolume();

	// 클라의 자신 마이크 뮤트
	void MuteMyMic( bool bMute );

	// 클라의 Rotation 설정 0~359 사이값.
	void SetRotation( int nRotation );

	// 보이스폰트
	// 커스텀일때 Pitch(음높이), Timbre(음색)사용하며, 0.5~2.0 범위가 유효범위.
	// Pitch : 0.5로 가면 낮은 남자 목소리. 2.0으로 가면 높은 여자 목소리
	// Timbre : 0.5로 가면 늘어지면서 무거운 느낌(TV음성변조 낮은음) 2.0으로 가면 테이프 빨리 돌릴때 느낌(TV음성변조 높은음)
	void SetVoiceFont( eVoiceFontType eVoiceFont, float fPitch = 0.0f, float fTimbre = 0.0f );

	// 마이크 녹음 테스트
	void StartMicTestPhase1();
	void StartMicTestPhase2( void (*callback)(void*) );
	void StopMicTest();

	// 제어판의 마이크 볼륨
	void SetMicVolume( float fValue );
	float GetMicVolume();

	// 마이크 부스트 컨트롤
	bool IsEnableMicBoost() { return m_bEnableMicBoost; }
	void SetMicBoost( bool bBoost );

//#define DOLBY_CLIENT_LOG
#if defined(DOLBY_CLIENT_LOG)
	FILE* logout;
	static void logging(unsigned int level, const char *msg, void *rock);
#endif
};
#endif