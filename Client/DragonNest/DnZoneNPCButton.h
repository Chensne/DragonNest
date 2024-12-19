#pragma once

class CDnZoneNPCButton : public CEtUIButton
{
public:

	// CDnZoneNPCButton �� �߰������� ������ ���ø��� ���ǵǾ� �ִ� ������ Ÿ���� ���� 
	// ZoneButton ���� �������� �پ��ϱ⶧���� / NPC / �Խ��� Ÿ�� ��� ���ø������鼭 �ε����� �� Ȯ���ؾ� �մϴ�.
	// ��δ� �������ΰ� �� �ƴ��� ����

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

 //		buttonAdventureQuestAvailable = 12,  // ������ ��� Ÿ���� ZoneButton
 //		buttonAdventureQuestPlaying = 13,    // 
		buttonAdventureQuestRecompense = 14, // ������ ����Ʈ�� �Ϸᰡ���Ҷ��� ǥ������ , ���ҽ��� 3������ ������ , 1���� ���
		
		buttonQuestGlobalAvailable = 12,     // �۷ι� �̺�Ʈ < �Ϲ� NPC Ÿ���� ZoneButton >
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
