#pragma once

#include "DnCustomDlg.h"
#include "DnItem.h"


class CDnLifeConditionDlg : public CDnCustomDlg
{
public:
	CDnLifeConditionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeConditionDlg();

	struct SPlantInfo
	{
		int m_nAreaIndex;
		int m_nItemID;
		int m_eAreaState;
		float m_fTime;
		float m_fWater;
		bool m_bShowCheck;
		char m_szHarvestIconName[255];

		SPlantInfo(){}
		SPlantInfo( int nItemID, int eAreaState, float fTime, float fWater, int nAreaIndex, bool bShowCheck, char * szHarvestIconName )
			: m_nItemID( nItemID )
			, m_eAreaState( eAreaState )
			, m_fTime( fTime )
			, m_fWater( fWater )
			, m_nAreaIndex( nAreaIndex )
			, m_bShowCheck( bShowCheck )
		{
			memset( m_szHarvestIconName, NULL, sizeof(m_szHarvestIconName) );
			_strcpy( m_szHarvestIconName, _countof(m_szHarvestIconName), szHarvestIconName, (int)strlen(szHarvestIconName) );
		}
	};

	struct SPlantInfoList
	{
		CDnItem * m_pItem;
		CEtUIStatic * m_pStaticBar;
		CEtUIStatic * m_pStaticName;
		CEtUIProgressBar * m_pProgressTime;
		CEtUIProgressBar * m_pProgressWater;
		CEtUICheckBox * m_pCheckBox;
		CDnItemSlotButton * m_pButtonSlot;
		SPlantInfo * m_pPlantInfo;

		SPlantInfoList()
			: m_pItem( NULL )
			, m_pStaticBar( NULL )
			, m_pStaticName( NULL )
			, m_pProgressTime( NULL )
			, m_pProgressWater( NULL )
			, m_pCheckBox( NULL )
			, m_pButtonSlot( NULL )
			, m_pPlantInfo( NULL )
		{}

		~SPlantInfoList()
		{
			SAFE_DELETE( m_pItem );
		}

		void Process()
		{
			if( NULL != m_pPlantInfo )
			{
				m_pProgressTime->SetProgress( m_pPlantInfo->m_fTime );
				m_pProgressWater->SetProgress( m_pPlantInfo->m_fWater );
			}
		}

		void Clear()
		{
			SAFE_DELETE( m_pItem );
			m_pStaticBar->Show( false );
			m_pProgressTime->Show( false );
			m_pProgressWater->Show( false );
			m_pStaticName->SetText(L"");
			m_pCheckBox->Show( false );
			m_pButtonSlot->ResetSlot();
			m_pPlantInfo = NULL;
		}

		bool IsInsideItem( float fX, float fY )
		{
			SUICoord uiCoords;
			m_pButtonSlot->GetUICoord(uiCoords);
			return uiCoords.IsInside(fX, fY);
		}
	};

	enum{
		ALL = -1,
		PLANT_MAX = 5
	};

private: 

	typedef std::map<int, SPlantInfo>	PLANT_MAP;
	typedef PLANT_MAP::iterator			PLANT_MAP_ITOR;

	SPlantInfoList m_PlantInfoList[PLANT_MAX];
	std::map<int, SPlantInfo>	m_mPlantInfo;

	bool m_bShowButton;
	int m_nSelectPage;
	int m_nFindSelect;
	CEtUIStatic * m_pStaticPage;
	CEtUIStatic * m_pStaticPossibleNum;
	CEtUIStatic * m_pStaticEmptyNum;
	CEtUICheckBox * m_pCheckAll;
	CEtUICheckBox * m_pCheckEmpty;
	CEtUIButton *m_pButtonNext;
	CEtUIButton *m_pButtonPrev;


	int FindInsideItem( float fX, float fY );

public :

	void ChangeState( int nAreaIndex, int nItemID, int eAreaState, float fTime, float fWater, char * szHarvestIconName );
	void RefreshPage();
	void RefreshMinimap(int nAreaIndex, bool bCheck);
	void RefreshEmptyAreaInfo( bool bShow );
	void PageCount();

	void SetShow( bool bShow )	{ m_bShowButton = bShow; }

	void GetMyFarmInfo( std::vector< std::pair<int, int> > & vecMyFarmInfo );
	void GetEmptyAreaInfo( std::vector< std::pair<int, EtVector3> > & vecEmptyAreaInfo );
	void SetPossibleCount( int nPossibleCount, int nMax );
	void SetEmptyCount( int nEmpty, int nMax );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

