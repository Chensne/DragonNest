#pragma once 
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
#include "EtUITabDialog.h"
#include "DnCompoundBase.h"

class CDnArtficeCompoundDlg : public CDnCompoundBase
{
public:
	CDnArtficeCompoundDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnArtficeCompoundDlg();

public:


protected:
	CEtUICheckBox*  m_pCheckBoxRare;
	CEtUICheckBox*  m_pCheckBoxEpic;
	CEtUICheckBox*  m_pCheckBoxUnique;
	CEtUICheckBox*  m_pCheckBoxLegend;

	CEtUIComboBox*	m_pComboBoxJob;

	CEtUIStatic*	m_pStaticTextAccesariesList;

	std::vector< CEtUICheckBox* > m_vecCheckBoxLevel;
	std::vector< CEtUIStatic* >	  m_vecStaticLevel;

	CEtUITreeCtl*	m_pTreeCtrlAccessories;
	CEtUITreeCtl*   m_pTreeCtrlSkillList;
	

	std::vector< FilterTypeStruct > m_VecSkillList;
	std::vector< int > m_nVecLevelList;

	std::vector< SetItemStruct* > m_VecSetList;


	std::vector< CDnCompoundBase::ItemGroupStruct* > m_pVecResultList;
	CDnCompoundBase::ItemGroupStruct m_RemoteCompoundItem;
	
	CTreeItem* m_pAccesaries;

	int m_nSelectJobType;

	RankStruct m_Rank;
	bool	m_bInitialize;
	bool	m_bRemote;

public:

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	virtual void SetCompoundTable( int nType );
	virtual void UpdateGroupList( std::vector< CDnCompoundBase::ItemGroupStruct* >& pVecResultList  );
	virtual void SetRemoteCompoundDlg( CDnCompoundBase::ItemGroupStruct& CompoundGroupData );
	virtual void Reset();

	void CompoundGroupListClear();

	bool CheckCompoundItemLevel( const int& nLevel );
	int  GetTypeFromPartsID( const int& nPartsID );

	bool IsExistItem( const int& nExistitem);

	bool SetAccessoriesList();
	bool RefreshAccessoriesList();
	bool RefreshSkillList();
	static bool CompareTreeItem( CTreeItem* pLside, CTreeItem* pRside );
	void RefreshCheckBoxLevel( int nLevel, bool bRemote );
		
	void SetEquipmentPartsControl( int nJob, int nPartsID );
	void SetAccessoriesControl( int nEquippedItemID );	
};
#endif 