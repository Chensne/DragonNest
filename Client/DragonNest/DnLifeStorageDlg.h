#pragma once

#include "DnCustomDlg.h"
#include "DnItem.h"


class CDnLifeStorageDlg : public CDnCustomDlg
{
public:
	CDnLifeStorageDlg ( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeStorageDlg (void);

	struct SStorageSlotInfo
	{
		CDnItem * m_pItem;
		int m_nExpireTime;

		SStorageSlotInfo()
			: m_pItem( NULL )
			, m_nExpireTime( 0 )
		{}

		~SStorageSlotInfo()
		{
			SAFE_DELETE( m_pItem );
		}
	};

protected:

	typedef std::map<INT64, SStorageSlotInfo *>	STORAGE_MAP;
	typedef STORAGE_MAP::iterator		STORAGE_MAP_ITOR;

	std::vector<CDnItemSlotButton*> m_vecSlotButton;
	CDnItemSlotButton *m_pDragButton;
	CDnItemSlotButton *m_pPressedButton;
	CEtUIButton *m_pButtonPrev;
	CEtUIButton *m_pButtonNext;

	bool	m_bNextPage;
	int		m_iSelectPage;
	std::vector< STORAGE_MAP > m_vItemsPerPage;

public :
	void RefreshLifeStorage(SCFarmWareHouseList * pData);
	void ResetAllStorage();
	void ResetSlot();
	void ChangePage( bool bNext );
	void UpdateSelectPage();
	void RequestTakeWareHouseItem( int nSlotIndex );
	void OnRecvTakeWareHouseItem( INT64 biUniqueID );
	
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};

