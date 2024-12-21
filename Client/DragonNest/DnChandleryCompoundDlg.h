#pragma once 
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
#include "EtUITabDialog.h"
#include "DnCompoundBase.h"

class CDnChandleryCompoundDlg : public CDnCompoundBase
{
public:
	CDnChandleryCompoundDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnChandleryCompoundDlg();


	enum
	{
		eGEM = 15,
		eGift = 16,
		eExSkillbook = 17,
		eMIX = 18,
		eExchangeEpic = 19,
		eExchangeUnique = 20,
		eMaxChandleryList,
	};

protected:

	CEtUITreeCtl*	m_pTreeCtrlChandlery;
	std::vector< ItemGroupStruct* > m_ResultChandleryList;
	std::map< int, SetItemStruct* > m_MapChandleryList;
	std::map< int, std::vector<CDnCompoundBase::ItemGroupStruct*> > m_MapCategoryList;
	
	bool	m_bRemote;
	CDnCompoundBase::ItemGroupStruct m_RemoteCompoundItem;

public:

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Reset();

	virtual void SetCompoundTable( int nType );
	virtual void UpdateGroupList( std::vector< CDnCompoundBase::ItemGroupStruct* >& pVecResultList  );
	virtual void SetRemoteCompoundDlg( CDnCompoundBase::ItemGroupStruct& CompoundGroupData );

	bool IsExistItem( int nKey, int nExistitem );
	void CompoundGroupListClear();

	bool SetChandleryList();

};

#endif 