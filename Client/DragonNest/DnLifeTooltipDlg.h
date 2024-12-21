#pragma once

#include "EtUIDialog.h"
#include "DnLifeSkillPlantTask.h"


class CDnLifeTooltipDlg : public CEtUIDialog
{
public:
	CDnLifeTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeTooltipDlg();

private:
	CEtUIStatic * m_pStaticName;
	CEtUIStatic * m_pStaticState;
	CEtUIStatic * m_pStaticTime;
	CEtUIStatic * m_pStaticWater;
	CEtUIStatic * m_pStaticWaterCount;
	CEtUIStatic * m_pStaticWaterTime;
	CEtUIStatic * m_pStaticCharName;

	int m_nRenderCount;

public :

	void SetInfo( CDnLifeSkillPlantTask::SPlantTooltip & info );
	void UpdateTooltip( CEtUIDialog * pParent, float fX, float fY );
	void CenterTooltip();
	void AddRenderCount()	{ ++m_nRenderCount; }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Render(float fElapsedTime );
};

