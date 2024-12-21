#pragma once
#include "DnCustomDlg.h"
#include "DnWorld.h"
#include "DnBlindDlg.h"

class CDnDungeonEnterLevelDlg;
class CDnDungeonExpectDlg;
class CDnDungeonNestEnterLevelDlg;

class CDnDungeonEnterDlg : public CDnCustomDlg, public CDnBlindCallBack
{
public:
	struct SDungeonGateInfo 
	{
		CDnStageEnterButton *m_pButtonGate;

		char m_cCanDifficult[5];
		std::vector<int> m_vecRecommandLevel;
		std::vector<int> m_vecRecommandPartyCount;
		int m_nMapIndex;
		int m_nMinLevel;
		int m_nMaxLevel;
		int m_nNeedItemID;
		int m_nNeedItemCount;
		int m_nAbyssNeedQuestID;
		int m_nAbyssMinLevel;
		int m_nAbyssMaxLevel;
		char m_cDungeonEnterPermit;
		std::vector<DnActorHandle> m_hVecNeedItemActorList;
		int m_nMaxUsableCoinCount;
		int m_nMinPartyCount;
		int m_nMaxPartyCount;

		SDungeonGateInfo()
			: m_pButtonGate(NULL)
			, m_nMapIndex(0)
			, m_nMinLevel(-1)
			, m_nMaxLevel(-1)
			, m_cDungeonEnterPermit(0)
			, m_nAbyssMinLevel(-1)
			, m_nAbyssMaxLevel(-1)
			, m_nAbyssNeedQuestID(-1)
			, m_nMaxUsableCoinCount(-1)
			, m_nMinPartyCount(-1)
			, m_nMaxPartyCount(-1)
			, m_nNeedItemID( 0 )
			, m_nNeedItemCount( 0 )
		{
			memset( m_cCanDifficult, 0, sizeof(m_cCanDifficult) );
		}

		void Clear()
		{
			m_nMinLevel = -1;
			m_nMaxLevel = -1;
			m_nMapIndex = 0;
			m_pButtonGate->Clear();
			memset( m_cCanDifficult, 0, sizeof(m_cCanDifficult) );
			m_vecRecommandLevel.clear();
			m_vecRecommandPartyCount.clear();
			m_cDungeonEnterPermit = 0;
			m_nAbyssMinLevel = -1;
			m_nAbyssMaxLevel = -1;
			m_nAbyssNeedQuestID = -1;
			m_nMaxUsableCoinCount = -1;
			m_nMinPartyCount = -1;
			m_nMaxPartyCount = -1;
			m_nNeedItemID = 0;
			m_nNeedItemCount = 0;
		}

		void SetUserLevel( int nMinLevel, int nMaxLevel )
		{
			m_nMinLevel = nMinLevel;
			m_nMaxLevel = nMaxLevel;
		}

		void SetInfo( const wchar_t *wszDungeonName, const wchar_t *wszDungeonDesc, int nMinPartyCount, int nMaxPartyCount, int nMapIndex, int nMapType, int nMinLevel, int nMaxLevel, char cPermit = CDnWorld::PermitEnter )
		{
			m_pButtonGate->SetInfo( wszDungeonName, wszDungeonDesc, nMinPartyCount, nMaxPartyCount, nMapType, nMinLevel, nMaxLevel, nMapIndex, cPermit );
			m_nMapIndex = nMapIndex;
		}

		void Show( bool bShow ) { m_pButtonGate->Show( bShow ); }
		bool IsShow() { return m_pButtonGate->IsShow(); }
		void Enable( bool bEnable )	{ m_pButtonGate->Enable( bEnable ); }
		bool IsEnable()	{ return m_pButtonGate->IsEnable(); }
	};

	enum
	{
		DUNGEON_GATE_COUNT = 5,
	};

	enum eDungeonEnterType { DUNGEONENTER_TYPE_NORMAL = 0, DUNGEONENTER_TYPE_NEST };

public:
	CDnDungeonEnterDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDungeonEnterDlg(void);

protected:
	CEtUITextureControl *m_pTextureDungeonImage;
	CEtUITextureControl *m_pTextureDungeonMap;
	EtTextureHandle m_hIntroImage;
	EtTextureHandle m_hIntroMap;

	CEtUIStatic *m_pStaticDungeonName;
	CEtUIStatic *m_pStaticBottomBack;

	std::vector<SDungeonGateInfo> m_vecDungeonGateInfo;
	std::vector<int> m_vecEnableGateIndex;

	int m_nGateIndex;
	SUICoord m_uiTextureImageBack;
	SUICoord m_uiBottomBlackBack;

	CDnDungeonEnterLevelDlg *m_pDungeonEnterLevelDlg;
	CDnDungeonExpectDlg *m_pDungeonExpectDlg;

	CDnDungeonNestEnterLevelDlg *m_pDungeonNestEnterLevelDlg;
	eDungeonEnterType m_eDungeonEnterType;

protected:
	void SetDungeonImage( const std::string &DungeonImageName );

	void UpdateButtonState();
	void UpdateControlPos();

	virtual void InitializeEnterLevelDlg();
public:
	void SetGateName( std::wstring &strGateName );
	void SetGateInfo( CDnWorld::DungeonGateStruct *pGateInfo );
	int GetDungeonLevel();
	int GetDungeonGateIndex() { return m_nGateIndex; }

	virtual void SetSelectDungeonInfo( int nMapIndex, int nDifficult );
	void UpdateDungeonExpectReward( int nMapIndex, int nDifficult );
	void SetDungeonEnterType( eDungeonEnterType eEnterType ) { m_eDungeonEnterType = eEnterType; }

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void OnBlindOpen();
	virtual void OnBlindOpened();
	virtual void OnBlindClose();
	virtual void OnBlindClosed();
};