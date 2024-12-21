#pragma once

#include "EtUIDialog.h"
#include "DnCommonTask.h"
#include "DnWorld.h"

class CDnDungeonOpenNoticeDlg : public CEtUIDialog
{
public:
	CDnDungeonOpenNoticeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDungeonOpenNoticeDlg(void);

protected:
	CEtUIStatic *m_pStaticTitle;
	CEtUIStatic *m_pStaticTitleName;
	CEtUIStatic *m_pStaticBack[6];
	CEtUIStatic *m_pStaticAbyssBack[6];
	CEtUIStatic *m_pStaticName[6];
	CEtUIStatic *m_pStaticAbyssName[6];
	CEtUIStatic *m_pStaticMapType[6][CDnWorld::LandEnvironment_Amount-1];

	float m_fDelta;
	float m_fHideDeltaOffset;
	float m_fMaxDelta;
	SUICoord m_CoordTitle;
	SUICoord m_CoordTitleName;
	SUICoord m_CoordBack[6];
	SUICoord m_CoordAbyssBack[6];
	SUICoord m_CoordName[6];
	SUICoord m_CoordAbyssName[6];
	SUICoord m_CoordMapType[6][CDnWorld::LandEnvironment_Amount-1];

	int m_nShowElementCount;

	struct DungeonOpenStruct {
		CDnCommonTask::DungeonOpenNoticeTypeEnum Type;
		std::vector<int> nVecMapID;
	};
	std::vector<DungeonOpenStruct *> m_pVecDungeonOpenList;

protected:
	bool RefreshInfo();
	void UpdateInfo( DungeonOpenStruct *pInfo );
	float GetWeightValue( float fStartDelta, float fEndDelta, float fCurDelta );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );
};