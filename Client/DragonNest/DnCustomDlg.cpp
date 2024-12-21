#include "StdAfx.h"
#include "DnCustomDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCustomDlg::CDnCustomDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
}

CDnCustomDlg::~CDnCustomDlg(void)
{
}

CEtUIControl *CDnCustomDlg::CreateControl( SUIControlProperty *pProperty )
{
	CEtUIControl *pControl(NULL);

	switch( pProperty->nCustomControlID )
	{
	case idStatusStatic:		pControl = new CDnCharStatusStatic( this );		break;
	case idGaugeFace:			pControl = new CDnGaugeFace( this );			break;
	case idQuickSlotButton:		pControl = new CDnQuickSlotButton( this );		break;
	case idItemSlotButton:		pControl = new CDnItemSlotButton( this );		break;
	case idSkillSlotButton:		pControl = new CDnSkillSlotButton( this );		break;
	case idGaugeExp:			pControl = new CDnExpGauge( this );				break;
	case idComboCount:			pControl = new CDnComboCount( this );			break;
	case idDamageCount:			pControl = new CDnDamageCount( this );			break;
	case idChainCount:			pControl = new CDnChainCount( this );			break;
	case idZoneGateButton:		pControl = new CDnZoneGateButton( this );		break;
	case idZoneNPCButton:		pControl = new CDnZoneNPCButton( this );		break;
	case idWorldVillageButton:	pControl = new CDnWorldVillageButton( this );	break;
	case idWorldGameButton:		pControl = new CDnWorldGameButton( this );		break;
	case idQuestTree:			pControl = new CDnQuestTree( this );			break;
	case idMenuButton:			pControl = new CDnMenuButton( this );			break;
	case idMessageStatic:		pControl = new CDnMessageStatic( this );		break;
	case idSkillUpButton:		pControl = new CDnSkillUpButton( this );		break;
	case idStageEnterButton:	pControl = new CDnStageEnterButton( this );		break;
	case idBonusBoxButton:		pControl = new CDnBonusBoxButton( this );		break;
	case idMovieControl:		pControl = new CDnMovieControl( this );			break;
	case idColorButton:			pControl = new CDnColorButton( this );			break;
	case idCpCount:				pControl = new CDnCpCount( this );				break;
	case idNpcAnswerHtmlTextBox:pControl = new CDnNpcAnswerHtmlTextBox( this );	break;
	case idMissionIconStatic:	pControl = new CDnMissionIconStatic( this );	break;
	case idCpJudgeControl:		pControl = new CDnCpJudgeControl(this);			break;
	case idRadioMsgButton:		pControl = new CDnRadioMsgButton(this);			break;
	case idGestureButton:		pControl = new CDnLifeSkillButton(this);			break;
	case idJobIconStatic:		pControl = new CDnJobIconStatic(this);			break;
	case idSkillAcquireButton:	pControl = new CDnSkillAcquireButton(this);		break;
	case idFishingProgressBar:	pControl = new CDnFishingProgressBar(this);		break;
	case idFarmAreaButton:		pControl = new CDnFarmAreaButton(this);		break;
	case idLoadingButton:		pControl = new CDnLoadingButton(this);		break;
	case idRotateToggleButton:  pControl = new CDnRotateToggleButton(this); break;
	case idMenuListControl:     pControl = new CDnMenuList( this );				break;
	}

	if( pControl )
	{
		pControl->Initialize( pProperty );
		m_vecControl.push_back( pControl );
		InitCustomControl( pControl );
		return pControl;
	}
	else
	{
		return CEtUIDialog::CreateControl( pProperty );
	}
}