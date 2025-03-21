#pragma once

#include "DnChatBalloonDlg.h"

/*
	기존의 방식은 기본 텍스쳐들을 불러와서 렌더링 해주는 방식이나 이는 확장성이나 여러가지 불편한 사항이 존재한다.
	그래서 UI형식을 추가하였다.
	하지만 기존의 방식을 그대로 두는 이유는 이전에 작업해놓은 것들이 너무 많아 수정 하기가 애매 해서이다.
	추후에 말풍선에 관련된 작업은 UI로 한다.
*/
class MAChatBalloon
{
public:
	MAChatBalloon();
	virtual ~MAChatBalloon();

	static int m_siTotalBalloon;
private:

	enum{
		DEFAULT_TIME = 10000
	};

	enum{
		NUM_CHATBALLOON_TEX = 6,
	};
	EtTextureHandle	m_hChatBalloonTexture[NUM_CHATBALLOON_TEX];
	EtTextureHandle	m_hChatBalloonTailTexture[NUM_CHATBALLOON_TEX];

	std::vector< std::wstring > m_wszTalkText;
	DWORD					m_dwTalkStartTime;
	DWORD					m_dwDuringTime;

	float					m_fTalkWidth;
	float					m_fTalkTailPos;
	float					m_fTalkTailPosPrev;
	int						m_nChatType;
	bool					m_bConutBalloon;
	bool                    m_bEnableCustomChatBalloon;

	float					m_fAdditionalYOffset;

	float					m_fTalkWidthForWordBreak;	// 위에 TalkWidth가 가끔 원하는 값을 들고있지 않게 설계되어있어서 올바른 wordbreak를 위해 별도로 변수하나 만듭니다.

	CDnChatBalloonDlg *		m_Dialog;

	std::vector< std::wstring > m_vecCustomChatBalloonText;
	bool m_bForceShow;

	void ProcessCustomChatBalloonText( float fElapsedTime );
	int m_nCustomTextIndex;
	float m_fCustomTextDelayTime;

	bool CheckDistance( EtVector3 vPos, bool bNormalChatBalloon );

public:

	void RenderChatBalloon( EtVector3 vPos, EtVector3 vHeadPos, float fElapsedTime );
	void SetChatBalloonCustomText( const char * wszFileName, LPCWSTR wszMessage, DWORD dwTextColor, DWORD dwStartTime, int nChatType, DWORD dwTalkTime = DEFAULT_TIME ); // 일반적인 유져가 채팅하듯이 커스텀 말풍선을 사용.
	void SetChatBalloonText( LPCWSTR wszMessage, DWORD dwStartTime, int nChatType, DWORD dwTalkTime = DEFAULT_TIME );
	void SetChatBalloonCustom( const char * wszFileName, LPCWSTR wszMessage, DWORD dwTextColor, DWORD dwStartTime, int nChatType, DWORD dwTalkTime = DEFAULT_TIME );
	
	bool IsEnableCustomChatBallon(){return m_bEnableCustomChatBalloon;}
	bool IsRenderChatBalloon();
	void StopRenderChatBalloon();

	void StopChatBalloonCustom();
	void AddCustomChatBalloonText( LPCWSTR wszMessage );
	void ClearCustomChatBalloonText() { m_vecCustomChatBalloonText.clear(); }
	void ForceShowState( bool bForceShow ) { m_bForceShow = bForceShow; }

	// 투영공간에서의 단위이기때문에 픽셀단위로 조절할 수는 없다. #20162 이슈처럼 특별한 경우에만 사용하려고 만든거다.
	void SetAdditionalYOffset( float fValue ) { m_fAdditionalYOffset = fValue; }

	void IncreaseBalloon()	{ ++m_siTotalBalloon; m_bConutBalloon = true; }
	void DecreaseBalloon()	{ --m_siTotalBalloon; m_bConutBalloon = false; }
	bool IsCountBalloon()	{ return m_bConutBalloon; }
	static void ClearCount()	{ m_siTotalBalloon = 0; }
};
