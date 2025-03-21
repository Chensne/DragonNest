#pragma once

class CDnBonusBoxButton : public CEtUIRadioButton
{
	enum
	{
		BonusBox,
		BonusSelect,
		BronzeBox,
		BronzeBoxSelect,
		BronzeBoxOpen,
		BronzeBoxOpenSelect,
		SilverBox,
		SilverBoxSelect,
		SilverBoxOpen,
		SilverBoxOpenSelect,
		GoldBox,
		GoldBoxSelect,
		GoldBoxOpen,
		GoldBoxOpenSelect,
		PlatinumBox,
		PlatinumBoxSelect,
		PlatinumBoxOpen,
		PlatinumBoxOpenSelect,
		ItemLight,
	};

public:
	enum emBonusBoxType
	{
		typeQuestion,
		typeBronze,
		typeBronzeOpen,
		typeSilver,
		typeSilverOpen,
		typeGold,
		typeGoldOpen,
		typePlatinum,
		typePlatinumOpen,
	};

public:
	CDnBonusBoxButton( CEtUIDialog *pParent );
	virtual ~CDnBonusBoxButton(void);

protected:
	bool m_bRPressed;

	emBonusBoxType m_emBonusBoxType;
	bool m_bItemLight;
	bool m_bHandle;

public:
	void SetItemLight( bool bLight ) { m_bItemLight = bLight; }

	void SetQuestionBox()
	{
		m_emBonusBoxType = typeQuestion;
	}

	void SetBonusBox( int nBoxType )
	{
		switch( nBoxType )
		{
		case 0: m_emBonusBoxType = typeBronze;		break;
		case 1: m_emBonusBoxType = typeSilver;		break;
		case 2: m_emBonusBoxType = typeGold;		break;
		case 3: m_emBonusBoxType = typePlatinum;	break;
		}
	}

	void SetOpenBonusBox( int nBoxType )
	{
		switch( nBoxType )
		{
		case 0: m_emBonusBoxType = typeBronzeOpen;		break;
		case 1: m_emBonusBoxType = typeSilverOpen;		break;
		case 2: m_emBonusBoxType = typeGoldOpen;		break;
		case 3: m_emBonusBoxType = typePlatinumOpen;	break;
		}
	}

	emBonusBoxType GetBonusBoxButtonType() { return m_emBonusBoxType; }

public:
	void SetUseHandle( bool bHandle );

public:
	virtual void Render( float fElapsedTime );
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
};
