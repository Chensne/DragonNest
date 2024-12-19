#pragma once
#include "EtUIDialog.h"
#include "DnIsolate.h"

class CDnNestDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnNestDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnNestDlg(void);

	enum eStageTryCount
	{
		eStage_TryCount_Enable,
		eStage_TryCount_Disable
	};

	enum eStageLevelCondition
	{
		eStage_Level_Enable,
		eStage_Level_Min_Disable,
		eStage_Level_Max_Disable
	};

	struct sProcessData
	{
		int m_nStageType;

		int m_eStageTryCount;
		int m_eStageLevelCondition;

		int m_nMaxLevel;
		int m_nMinLevel;

		int m_nMaxTryCount;
		int m_nTryCount;

		tstring m_szMapName;
	};

	struct sStageValue
	{
		int m_nSortNumber;
		int m_nIconIndex;
		DWORD m_dwColor;
		std::wstring m_wszString;
	};

protected:
	CEtUITreeCtl*	m_pTreeList;

	bool IsStageLevelEnable( int nEnterTableID );

	bool InitializeData( const DWORD nIndex, sProcessData & sData, sStageValue & sValue );
	void ProcessData( sProcessData & sData, sStageValue & sValue );
	void SetStageTryCount( sProcessData & sData );
#if defined( PRE_ADD_TSCLEARCOUNTEX )
	void SetAddStageTryCount( CDnNestDlg::sProcessData & sData );
#endif	// #if defined( PRE_ADD_TSCLEARCOUNTEX )
	void SetStageLevelCondition( sProcessData & sData );
	void SetString( sProcessData & sData, sStageValue & sValue );
	void AddTreeItem( CTreeItem * pParent, std::vector< sStageValue > & vEnableStage, std::vector< sStageValue > & vLaterEnableStage, std::vector< sStageValue > & vDisableStage );

	static bool CompareStage( const sStageValue & s1, const sStageValue & s2 );
	void SetPCBangNestInfo();

public:
	virtual void	Initialize( bool bShow );
	virtual void	InitialUpdate();
	virtual void	Show( bool bShow );
	virtual void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void RefreshList();

	void RefreshStageList();
};