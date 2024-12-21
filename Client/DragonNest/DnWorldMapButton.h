#pragma once

class CDnWorldMapButton : public CEtUIButton
{
public:
	enum emMapButtonQuestType
	{
		typeNoneQuestButton = 0,
		typeMainQuestButton = 3,
		typeActiveQuestButton = 4,
		typeDynamicQuestButton = 5,
	};

public:
	CDnWorldMapButton( CEtUIDialog *pParent );
	virtual ~CDnWorldMapButton(void);

protected:
	bool m_bOpened;
	bool m_bBlink;
	emMapButtonQuestType m_emMapButtonQuestType;
	float m_fElapsedTime;

public:
	bool IsButtonOpened() { return m_bOpened; }
	void SetButtonState( bool bOpened ) { m_bOpened = bOpened; }

	void SetBlink( bool bBlink ) {m_bBlink = bBlink;}

	emMapButtonQuestType GetButtonQuestType() { return m_emMapButtonQuestType; }
	void SetButtonQuestType( emMapButtonQuestType emType ) { m_emMapButtonQuestType = emType; }

public:
	virtual void Render( float fElapsedTime );
};
