#pragma once

class CDnZoneNPCButton : public CEtUIButton
{
public:

	// CDnZoneNPCButton 에 추가적으로 설정된 탬플릿에 정의되어 있는 순서별 타입의 정의 
	// ZoneButton 에도 종류별로 다양하기때문에 / NPC / 게시판 타입 등등 탬플릿을보면서 인덱스를 잘 확인해야 합니다.
	// 모두다 공통적인것 은 아님을 주의

	enum emQuestState
	{
		buttonQuestNone = 0,
		buttonQuestAvailable = 2,
		buttonQuestPlaying = 3,
		buttonQuestRecompense = 4,
		buttonSubQuestAvailable = 6,
		buttonSubQuestPlaying = 7,
		buttonSubQuestRecompense = 8,
		buttonReputationQuestAvailable = 9,
		buttonReputationQuestPlaying = 10,
		buttonReputationQuestRecompense = 11,

 //		buttonAdventureQuestAvailable = 12,  // 모험자 길드 타입의 ZoneButton
 //		buttonAdventureQuestPlaying = 13,    // 
		buttonAdventureQuestRecompense = 14, // 모험자 퀘스트는 완료가능할때만 표현해줌 , 리소스상 3가지가 있지만 , 1개만 사용
		
		buttonQuestGlobalAvailable = 12,     // 글로벌 이벤트 < 일반 NPC 타입의 ZoneButton >
		buttonQuestGlobalPalying = 13,       // 
		buttonQuestGlobalRecompense = 14,    //
		buttonNoticePcBang = 12,
	};

	enum emMailState
	{
		buttonMailNone = 0,
		buttonMailNew = 2,
	};

public:
	CDnZoneNPCButton( CEtUIDialog *pParent );
	virtual ~CDnZoneNPCButton(void);

protected:
	emQuestState m_emQuestState;
	emMailState	m_emMailState;
	bool m_bPushPin;
	EtVector2 m_vWorldPos;
	int m_nUnitSize;
	EtVector2 m_vDirection;

public:
	bool IsPushPin() { return m_bPushPin; }
	void SetPushPin( bool bPin ) { m_bPushPin = bPin; }

public:
	void SetQuestState( emQuestState questState ) { m_emQuestState = questState; }
	void SetMailState( emMailState mailState )	{ m_emMailState = mailState; }
	void SetWorldPos( EtVector2 &vPos ) { m_vWorldPos = vPos; }
	EtVector2 *GetWorldPos() { return &m_vWorldPos; }
	void SetUnitSize( int nValue ) { m_nUnitSize = nValue; }
	int GetUnitSize() { return m_nUnitSize; }
	void SetDirection( EtVector2 &vDir ) { m_vDirection = vDir; }
	EtVector2 *GetDirection() { return &m_vDirection; }

public:
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void Render( float fElapsedTime );
};
