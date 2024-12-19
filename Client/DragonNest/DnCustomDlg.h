#pragma once
#include "EtUIDialog.h"
#include "DnCustomControlCommon.h"
#include "DnCharStatusStatic.h"
#include "DnItemSlotButton.h"
#include "DnGaugeFace.h"
#include "DnQuickSlotButton.h"
#include "DnSkillSlotButton.h"
#include "DnExpGauge.h"
#include "DnDamageCount.h"
#include "DnComboCount.h"
#include "DnChainCount.h"
#include "DnCpCount.h"
#include "DnZoneGateButton.h"
#include "DnZoneNPCButton.h"
#include "DnWorldGameButton.h"
#include "DnWorldVillageButton.h"
#include "DnQuestTree.h"
#include "DnMenuButton.h"
#include "DnMessageStatic.h"
#include "DnSkillUpButton.h"
#include "DnStageEnterButton.h"
#include "DnBonusBoxButton.h"
#include "DnMovieControl.h"
#include "DnColorButton.h"
#include "DnNpcAnswerHtmlTextBox.h"
#include "DnMissionIconStatic.h"
#include "DnCpJudgeControl.h"
#include "DnRadioMsgButton.h"
#include "DnGestureButton.h"
#include "DnJobIconStatic.h"
#include "DnSkillAcquireButton.h"
#include "DnFishingProgressBar.h"
#include "DnFarmAreaButton.h"
#include "DnLoadingButton.h"
#include "DnRotateToggleButton.h"
#include "DnMenuList.h"

class CDnCustomDlg : public CEtUIDialog
{
public:
	CDnCustomDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnCustomDlg(void);

public:
	virtual void InitCustomControl( CEtUIControl *pControl ) {}
	virtual CEtUIControl *CreateControl( SUIControlProperty *pProperty );
};
