
#include "stdafx.h"

#ifdef PRE_ADD_DRAGON_GEM
#include "DnDragonGemTooltipDlg.h"
#include "DnTableDB.h"
/*
Dragon Jewel Type
1= Stats (defense)
2= Attack
3= Skill ?
4= NOT USED fuck it.
*/
CDnDragonGemTooltipDlg::CDnDragonGemTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_pTextBox = NULL;
	m_pStaticBase = NULL;
	m_pStaticEquip = NULL;
	for (int i = 0; i < DRAGON_GEM_SLOT_MAX; ++i)
	{
		m_pStaticBtnCover[i] = NULL;
		m_pItemSlot[i] = NULL;
		for (int j = 0; j < DRAGON_GEM_SLOT_MAX; ++j)
		{
			m_pJewelSlot[i][j] = NULL;
		}
	}
}

CDnDragonGemTooltipDlg::~CDnDragonGemTooltipDlg()
{
}

void CDnDragonGemTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("TooltipJewelDlg.ui").c_str(), bShow);
}

void CDnDragonGemTooltipDlg::InitialUpdate()
{
	m_pTextBox = GetControl<CEtUITextBox>("ID_TEXTBOX");

	m_pStaticBase = GetControl<CEtUIStatic>("ID_STATIC_BASE");
	m_pStaticEquip = GetControl<CEtUIStatic>("ID_STATIC_EQUIP");
	m_pStaticEquip->Show(false); //hide and show when show EQUIPED ITEM!

	//	v5 = sub_C09140("ID_LINE_VERTICAL%d", v3 + 2);
	for (int i = 0; i < 2; i++)
	{
		m_pStaticVertialLines[i] = GetControl<CEtUIStatic>(FormatA("ID_LINE_VERTICAL%d", i + 2).c_str());
	}

	for (int i = 0; i < DRAGON_GEM_SLOT_MAX; ++i)
	{
		m_pStaticBtnCover[i] = GetControl<CEtUIStatic>(FormatA("ID_STATIC_SLOT%d", i).c_str());
		m_pStaticBtnCover[i]->Show(false);

		m_pItemSlot[i] = GetControl<CDnItemSlotButton>(FormatA("ID_BT_ITEM%d", i).c_str());
		m_pItemSlot[i]->Show(false);

		//fucking koreans..
		if (i == 0)
		{
			for (int j = 0; j < 4; ++j)
			{
				m_pJewelSlot[0][j] = GetControl<CEtUIStatic>(FormatA("ID_STATIC_JEWEL0%d", j).c_str());
				m_pJewelSlot[0][j]->Show(false);
			}
		}

		if(i > 0){
			for (int j = 0; j < DRAGON_GEM_SLOT_MAX; ++j)
			{
				m_pJewelSlot[i][j] = GetControl<CEtUIStatic>(FormatA("ID_STATIC_JEWEL%d%d", i, j+1).c_str());
				m_pJewelSlot[i][j]->Show(false);
			}
		}
	}	



}

void CDnDragonGemTooltipDlg::InitCustomControl(CEtUIControl *pControl)
{
	if( pControl == NULL ) 
		return;

	if( !strstr(pControl->GetControlName() , "ID_BT_ITEM") ) 
		return;

	CDnItemSlotButton* pSlotBtn = NULL;
	pSlotBtn = static_cast<CDnItemSlotButton*>(pControl);
	pSlotBtn->SetSlotType(ST_INVENTORY);
	pSlotBtn->SetSlotIndex(0);
	
	if(pSlotBtn)
		m_vSlotButtonList.push_back(pSlotBtn);
}

bool CDnDragonGemTooltipDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsShow())
		return false;

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnDragonGemTooltipDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process(fElapsedTime);
}

void CDnDragonGemTooltipDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName(pControl->GetControlName());

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnDragonGemTooltipDlg::ShowDragonGemToolTip(CDnItem* pItem)//int nJewelTypeID)
{
	CEtUIDialog* pParentToolTip = this->GetParentDialog();
	if(!pParentToolTip) 
		return;

	InitTooltipDragonGem(pItem->GetDragonGemType());

	SUICoord sParentToolTipCoord, sGemToolTipCoord;
	pParentToolTip->GetDlgCoord(sParentToolTipCoord);
	this->GetDlgCoord(sGemToolTipCoord);

	// 우선 강제로 우측으로 간다.
	//sParentToolTipCoord.fX -= sGemToolTipCoord.fWidth - 0.018f;
	//this->SetPosition(sParentToolTipCoord.fX, sParentToolTipCoord.fY);

	// 아래로 보낸다
	sParentToolTipCoord.fY = (sParentToolTipCoord.fY + sParentToolTipCoord.fHeight);// + 0.018f;
	this->SetPosition(sParentToolTipCoord.fX, sParentToolTipCoord.fY);

	// 화면 밖으로 나가는지 체크.
	float fDlgXCoordRel = .0f;
	float fDlgYCoordRel = .0f;
	this->GetDlgCoord( sGemToolTipCoord );
	if( (sGemToolTipCoord.Bottom()) > GetScreenHeightRatio() )
	{
		fDlgYCoordRel = sGemToolTipCoord.Bottom() - GetScreenHeightRatio();
		sGemToolTipCoord.fY -= fDlgYCoordRel;
	}
	if( (sGemToolTipCoord.Right()) > GetScreenWidthRatio() )
	{
		fDlgXCoordRel = sGemToolTipCoord.Right() - GetScreenWidthRatio();
		sGemToolTipCoord.fX -= fDlgXCoordRel;
	}
	if( ( sGemToolTipCoord.fX ) < 0.f )
	{
		fDlgXCoordRel = sGemToolTipCoord.fX;
		sGemToolTipCoord.fX -= fDlgXCoordRel;
	}
	this->SetDlgCoord( sGemToolTipCoord );


	// 용옥툴팁이 좌측으로 이동했다면, 부모에도 같은 값을 적용.
	if (fDlgXCoordRel != 0.0f)
	{
		// 두번째 비교 툴팁의 경우 한단계 상위(첫번째 툴팁) 및 두번째 상위(기본 툴팁) 둘다 이동시켜야한다.
		CEtUIDialog *pParentDialog = pParentToolTip;
		while( pParentDialog = pParentDialog->GetParentDialog() )
		{
			pParentDialog->GetDlgCoord( sParentToolTipCoord );
			
			sParentToolTipCoord.fX -= fDlgXCoordRel;
			sParentToolTipCoord.fY -= fDlgYCoordRel;
			pParentDialog->SetDlgCoord( sParentToolTipCoord );
		}
	}
	
	// Show!
	Show(true);
}

void CDnDragonGemTooltipDlg::CloseDragonGemToolTip()
{
	Show(false);
}


void CDnDragonGemTooltipDlg::ResetSlots()
{
	for (int i = 0; i < DRAGON_GEM_SLOT_MAX; ++i)
	{
		m_pStaticBtnCover[i]->Show(false);
		m_pItemSlot[i]->Show(false);
		for (int j = 0; j < DRAGON_GEM_SLOT_MAX; ++j)
		{
			m_pJewelSlot[i][j]->Show(false);
		}
	}
}

void CDnDragonGemTooltipDlg::InitTooltipDragonGem(int nJewelTypeID)
{	
	ResetSlots();
	std::vector<int> vecTemp = CDragonJewelSlotTable::GetInstance().GetValue(nJewelTypeID);
	//if first item is 0 then dont show anything!
	if (vecTemp[0] == 0)
		return;

	for (int i = 0; i < vecTemp.size(); i++)
	{
		if (vecTemp[i] > 0 && vecTemp[i] < 4)
		{
			m_pStaticBtnCover[i]->Show(true);
			m_pItemSlot[i]->Show(true);
			m_pJewelSlot[i][vecTemp[i]-1]->Show(true);
		}
	}

}
#endif // PRE_ADD_DRAGON_GEM