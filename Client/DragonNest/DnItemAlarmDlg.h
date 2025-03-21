#pragma once
#include "DnCustomDlg.h"

class CDnItem;

class CDnItemAlarmDlg : public CDnCustomDlg
{
public:
	enum
	{
		typeAlarmAdd,
		typeAlarmDel,
		typeAlarmModify,
	};

	enum
	{
		PLANT_MAX = 3,
	};

public:
	CDnItemAlarmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnItemAlarmDlg(void);

protected:
	CDnItem *m_pItem;
	CDnItem * m_pPlantItems[PLANT_MAX];
	CDnItemSlotButton *m_pButtonItem;
	CDnItemSlotButton *m_pButtonSymbol;
	CDnItemSlotButton *m_pButtonPlant[PLANT_MAX];
	CDnSkillSlotButton *m_pButtonSkill;
	CEtUIStatic *m_pStaticText;
	CEtUIStatic *m_pStaticPlant[PLANT_MAX];

	SUICoord m_ButtonCoord[2];
	SUICoord m_TextCoord[2];
	float m_fIconHeightDelta;

	float m_fShowTime;
	float m_fShowRatio;
	float m_fElapsedTime;
	float m_fConst;

protected:
	void SetFadeTime( float fFadeTime );
	void SetText( LPCWSTR wszMsg, DWORD dwColor );
	void ResetControl();
	void SetPlant( int nIndex, CDnItem * pItem, DWORD dwColor );

public:
	void SetItemAlarm( LPCWSTR wszMsg, CDnItem *pItem, int nCount, DWORD dwColor, float fFadeTime );
	void SetSymbolAlarm( LPCWSTR wszMsg, CDnItem *pItem, int nCount, DWORD dwColor, float fFadeTime );
	void SetSkillAlarm( LPCWSTR wszMsg, DnSkillHandle hSkill, DWORD dwColor, float fFadeTime );
	void SetPlantAlarm( std::vector<CDnItem *> & vPlant, DWORD dwColor, float fFadeTime );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};
