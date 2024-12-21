#pragma once
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL

#include "EtUITabDialog.h"
#include "DnCompoundBase.h"

class CDnSuffixCompoundDlg : public CDnCompoundBase
{
public:
	CDnSuffixCompoundDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );	
	virtual ~CDnSuffixCompoundDlg();

public:
	struct PartsItemStruct
	{
		int nJob;
		std::multimap<int, int> mutimapEquipList;

		PartsItemStruct( int Job ) : nJob( Job ){}

		void Reset()
		{
			nJob = -1;
			mutimapEquipList.clear();
		}
	};

	struct CheckSuffixStruct
	{
		CEtUICheckBox* pCheckBoxSuffix;
		CEtUIStatic*   pStaticName;
		int nID;						// 접미사 ID 
		int nGroupID;					// 그룹 아이디 
		int nEquipType;					// 무기, 방어구, 악세 
		bool bPVP;	
		CDnCompoundBase::ItemGroupStruct*	pSuffixItem;

		~CheckSuffixStruct()
		{
			pSuffixItem = NULL;
			pStaticName = NULL;
			pCheckBoxSuffix = NULL;
		}

		void Show( bool bShow )
		{
			pCheckBoxSuffix->Show( bShow );	
			pStaticName->Show( bShow );
		}

		void Reset()
		{
			nID = nEquipType = 0;
			bPVP = false;
			pCheckBoxSuffix->SetChecked( false, false );
			pStaticName->ClearText();
			pSuffixItem = NULL;
		}
	};

protected:

	CEtUICheckBox*  m_pCheckBoxRare;
	CEtUICheckBox*  m_pCheckBoxEpic;
	CEtUICheckBox*  m_pCheckBoxUnique;

	CEtUIComboBox*	m_pComboBoxJob;
	CEtUIComboBox*  m_pComboBoxGrade;

	CEtUITreeCtl*	m_pTreeCtrlEquipment;
	
	std::vector<CEtUICheckBox*>	 m_vecCheckBoxLevel;
	std::vector<CEtUIStatic*>	 m_vecStaticLevel;
	
	std::vector< CheckSuffixStruct* > m_VecCheckBoxSuffix;	// 노말, PVP 구분 하지 않는다. 
	std::vector< CEtUIStatic* >	 m_VecStaticSuffixName;
	
	std::vector< int > m_nVecLevelList;
	std::vector< int > m_nVecSuffix;

	std::vector<PartsItemStruct> m_VecPartsItemList;
	std::vector< CDnCompoundBase::ItemGroupStruct* > m_pVecResultCheckBoxList;	

	int m_nSelectSuffixGrade;
	int m_nSelectJobType;
	
	std::vector< int >m_nVecSuffixIDList;
	CDnCompoundBase::ItemGroupStruct m_RemoteCompoundItem;

	// 테이블에서 로드한 세트 아이템 리스트 
	std::vector< SetItemStruct* > m_VecSetList;
	
	RankStruct m_Rank;
	bool	m_bInitialize;
	bool	m_bRemote;


public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	virtual void SetCompoundTable( int nType );
	virtual void UpdateGroupList( std::vector< CDnCompoundBase::ItemGroupStruct* >& pVecResultList );
	virtual void SetRemoteCompoundDlg( CDnCompoundBase::ItemGroupStruct& CompoundGroupData );
	virtual void Reset();
		
	bool SetUpEquipList();	
	bool RefreshEquipList();
	void RefreshSuffixList();
	void RefreshCheckBoxLevel( int nLevel, bool bRemote );
	bool IsExistItem( int nSuffixID );

	int GetTypeFromPartsID( const int& nPartsID );
	bool CheckCompoundItemLevel( const int& nLevel );
	bool CheckCompoundSuffixID( const int& nSuffixID );
	void CompoundGroupListClear();
	
	void DeleteAllCheckBox();
	void ResetCheckBox();
	bool IsExistSuffixCheckBox( int nID );
	void MakeSuffixCheckBox( std::vector< CDnCompoundBase::ItemGroupStruct* >& pVecResultList );
	void SetEquipmentPartsControl( int nJob, int nPartsID );


};

#endif 