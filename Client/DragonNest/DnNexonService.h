#pragma once

#if ( defined(_KR) || defined(_US) ) && defined(_AUTH)

#include "DnServiceSetup.h"
#include "DnPacket.h"


class DnNexonService : public IServiceSetup
{
public:
	enum ErrorLogDefine{
		Game_Crash = 1,							//비정상 종료
		Server_Disconnected = 2,				//서버와 디스커넥트
		SID_Not_Found = 3,						//SID 미발급
		GRAPHIC_DEVICE_ERROR =  4,				//다이렉트 엑스미설치나 그래픽드라이버 미설치
		INPUT_DEVICE_ERROR = 5,					//인풋 디바이스가 없음
		APP_DUPLICATED_ERROR = 6,				//중복 실행
		RESOURCE_PACKING_CORRUPTED_ERROR = 7,	//리소스 파일이 손상됨
		INVALID_PATH	 = 8,					//비정상적인 경로에서 실행
		INSUFFICIENT_MEMORY	 = 9,				//메모리 부족
		UNKNOWN_ERROR = 99,						//이유 알수없음
	};
	
	enum eGameState
	{
		eNone,
		eStart,
		eEnd,
	};

	DnNexonService();
	virtual ~DnNexonService();

	virtual int	PreInitialize(void* pCustomData);
	virtual int	Initialize(void* pCustomData);
	virtual LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual int Release();

	virtual int OnEvent(int nType, void* pExtra);
	virtual void OnDispatchMessage(int iMainCmd, int iSubCmd, char * pData, int iLen);

	virtual bool WriteStageLog_( INT32 nStage , LPCTSTR szComment );
	virtual bool WriteErrorLog_( INT32 nStage , LPCTSTR szComment );
	int DeleteChracter( int SelectIndex );
	int ReviveCharacter( int nSlotIndex );

	BOOL IsStandAloneMode() { return m_bStandAloneMode; }
	BOOL NGMAuthStandAloneMode( std::wstring strNexonID, std::wstring strPassword, std::wstring& szMsg );
	std::wstring GetNexonPassPort() { return m_szNexonPassPort; }
	void LogOutAuth();

protected:
	bool ParsingByString( LPCTSTR szComment , std::wstring wsString );
	BOOL NGMAuthWebLaunchingMode( NMLOCALEID uLocaleId, NMGameCode uGameCode, std::wstring& szMsg );
	
#ifdef _HSHIELD
	BOOL InitHackShield();
#endif // _HSHIELD
#if defined(_US)
	void ShowNexonWebpage();
#endif // _US

private:
	// 캐릭터 리스트 정보 넥슨 메신저 로그인에 사용
	SCCharList		m_CharListData;
	int				m_nSelectCharIndex;
	int				m_nServerIndex;

	eGameState		m_eGameState; 
	
	std::wstring	m_szPassPort;
	std::wstring	m_szNexonPassPort;
	std::wstring	m_wszCharacterName;
	__int64			m_UniqueID;
	bool			m_bStandAloneMode;
};

#endif // _KR, _US