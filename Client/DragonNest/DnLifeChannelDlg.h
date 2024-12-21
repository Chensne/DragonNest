#pragma once

#include "DnCustomDlg.h"


class CDnLifeChannelDlg : public CDnCustomDlg
{
public :
	CDnLifeChannelDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeChannelDlg();

private :

	struct SFarmInfo
	{
		UINT m_nFarmDBID;
		int m_nMapID;
		int m_nCongestion;
#if defined( PRE_ADD_FARM_DOWNSCALE )
		int	iAttr;
#elif defined(PRE_ADD_VIP_FARM)
		Farm::Attr::eType	Attr;
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

		SFarmInfo( UINT nFarmDBID
					, int nMapID
					, int nCongestion
#if defined( PRE_ADD_FARM_DOWNSCALE )
					, int iAttr 
#elif defined(PRE_ADD_VIP_FARM)
					, Farm::Attr::eType nAttr 
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

					)
			: m_nFarmDBID( nFarmDBID )
			, m_nMapID( nMapID )
			, m_nCongestion( nCongestion )
#if defined( PRE_ADD_FARM_DOWNSCALE )
			, iAttr( iAttr )
#elif defined(PRE_ADD_VIP_FARM)
			, Attr( nAttr )
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

		{}
	};

	struct SPlantInfo
	{
		int m_nFarmDBID;
		int m_nFieldIndex;
		int m_nItemID;
		int m_nRemainTime;
		bool m_bHarvest;
		SUICoord m_vButtonCrood;
	};

	enum
	{
		NO_CONGESTION,
		LOW_CONGESTION,
		MID_CONGESTION,	
		HIGH_CONGESTION,
	};

	CEtUIButton * m_pBaseCheck;
	CEtUIButton * m_pStaticCheck;
	CEtUIButton * m_pHarvestBtn;
	CEtUIButton * m_pGrowingBtn;
	CEtUIButton * m_pEmptyBtn;
	CEtUIButton * m_pEmptyBtn2;
	CEtUIListBox * m_pZoneList;
	CEtUIListBoxEx * m_pPlantList;
	CEtUITextureControl * m_pZoneMap;
	EtTextureHandle m_hMinimap;

	CEtUIButton * m_pButtonOK;
	CEtUIStatic * m_pStaticMapName;

	CEtUIStatic * m_pStaticEmptyText;

	std::vector<SPlantInfo>	m_vPlantedInfo;
	std::vector<CEtUIButton *>	m_vPlantIcon;

	std::vector< std::pair<TFarmItem,int> > m_vecChannelInfo;

	float m_fElapsedTime;

	int m_nFarmDBID;
	bool m_bRefreshGate;

	void RemoveList();
	void RemoveIcon();
	void RefreshPlantedText( SFarmInfo * pFarmInfo );
	void RefreshPlantIcon( int iFarmDBID, int iMapIndex );
	void AddSortChannelList();
	void PositionCancel();
	void InitializeMinimap( int nMapID );
	
	static bool CompareFarmChannelInfo( const std::pair<TFarmItem, int> &s1, const std::pair<TFarmItem, int> &s2 );

public : 

	void AddChannelList( SCFarmInfo * pFarmInfo );
	void UpdateFarmPlantedInfo( SCFarmPlantedInfo * pPlantedInfo );

	void SelectChannel();

	int GetFarmDBID()	{ return m_nFarmDBID; }

#if defined(_WORK)
	bool m_bQAZone;
	void ToggleQAZone();
#endif

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	//virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	//virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

