#pragma once

#include "DnCustomDlg.h"
#include "DnItem.h"


class CDnPetOptionDlg : public CDnCustomDlg
{
public:
	CDnPetOptionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPetOptionDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void GetPetOption();
	void SetPetFoodItem( int nPetFoodItemID );
	int GetPetFoodItemCount( int nPetFoodItemID );
	void RefreshPetFoodCount();

protected:
	CEtUICheckBox*	m_pCheckBoxGetItem;
	CEtUICheckBox*	m_pCheckBoxNormal;
	CEtUICheckBox*	m_pCheckBoxMagic;
	CEtUICheckBox*	m_pCheckBoxRare;
	CEtUICheckBox*	m_pCheckBoxEpic;
	CEtUICheckBox*	m_pCheckBoxUnique;
	CEtUICheckBox*	m_pCheckBoxLegend;
//	CEtUICheckBox*	m_pCheckBoxAutoUseHPPotion;
//	CEtUICheckBox*	m_pCheckBoxAutoUseMPPotion;
//	CEtUISlider*	m_pSliderAutoUseHPPotion;
//	CEtUISlider*	m_pSliderAutoUseMPPotion;
//	CEtUIStatic*	m_pStaticAutoUseHPPotion;
//	CEtUIStatic*	m_pStaticAutoUseMPPotion;
	CDnItem*			m_pFoodItem;
	CDnItemSlotButton*	m_pFoodItemSlot;
	CEtUICheckBox*		m_pCheckBoxAutoFeed;
	CEtUISlider*		m_pSliderAutoFeed;
	CEtUIStatic*		m_pStaticAutoFeedPercent;
};

