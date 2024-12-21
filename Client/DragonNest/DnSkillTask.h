#pragma once

#include "Task.h"
//#include "ClientTcpSession.h"
//#include "ClientUdpSession.h"
#include "MessageListener.h"
#include "DNPacket.h"
#include "Singleton.h"
#include "DnSkillTreeSystem.h"
#include "SecondarySkillRepository.h"

#if defined(PRE_FIX_NEXTSKILLINFO)
#include "DnTableDB.h"
#endif // PRE_FIX_NEXTSKILLINFO

//#undef PRE_ADD_SKILL_LEVELUP_RESERVATION

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
class CDnTotalLevelSkillSystem;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

class CDnSkillTask : public CTask, public CTaskListener, public CSingleton<CDnSkillTask>
{
public:

	struct S_SKILLTREE_SLOT_INFO
	{
		DnSkillHandle hSkill;
		CDnSkillTreeSystem::S_NODE_RENDER_INFO NodeRenderInfo;
	};

	struct S_SKILLTREE_INFO
	{
		S_SKILLTREE_SLOT_INFO aSkillTreeSlotInfo[ MAX_SKILL_SLOT_COUNT ];
	};

	CDnSkillTask();
	virtual ~CDnSkillTask();

protected:
	CDnSkillTreeSystem m_SkillTreeSystem;				// ��ų Ʈ�� �ý���
	CSecondarySkillRepository m_LifeSkillRepository;	// ���� ��Ȱ ��ų �����

	bool m_bTempSkillAdded; // �ӽý�ų�� ȹ�����ΰ�?

	// ��ų Ʈ���� �ö� �ִ� ��� ��ų ��ü. ����ִ� ���Ե� �ֱ� ������ �迭�� ó��.
	// �÷��̾��� ���� ������ų�� �ƴ� ���Կ� �����ϰ� ���� ���� ��ų ��ü�� ���õȴ�.
	// ��ų ��ü ��ü�� UI ������ ���� ������ ����ϱ� ������ ��¿ �� ���� �����ϰ� ���� ���� ��ų�� ��ü�� �����Ѵ�..
	map<int, S_SKILLTREE_INFO> m_mapSkillTreeInfoByJobID;		// TODO: ���Ŀ� ������ �߰��Ǹ� JobID ���� Ʈ���� ���� �־�� ��.

	int m_iNeedRefreshSkillPage;
	int m_iSkillTreePage;
	int m_iSkillPageCount;
	int	m_iSkillPoint[DualSkill::Type::MAX];                                         // ��ųƮ�� SP ����Ʈ
	vector<DnSkillHandle> m_vlhUnlockZeroLevelSkills[DualSkill::Type::MAX];          // ����� �� ��ų (���� 0����)
	vector<DnSkillHandle> m_vlhLocalPlayerOpenedSkillList[DualSkill::Type::MAX];	 // ����̸� ��ų���� 1�̻��� ������ �����ϰ� �ִ� ��ų���� ����Ʈ.
	vector<DnSkillHandle> m_vlhGuildWarSkillList;

	struct S_SKILL_LEVELUP_INFO
	{
		DnActorHandle hActor;
		DnSkillHandle hSkill;
		SCSkillLevelUp Packet;

		S_SKILL_LEVELUP_INFO( void )
		{
			ZeroMemory( &Packet, sizeof(SCSkillLevelUp) );
		};
	};

	list<S_SKILL_LEVELUP_INFO*> m_listWaitSkillLevelup;		// ��Ƽ�� ��ų ���� �߿� �������� ���� ��� ť�� �־��ٰ� ������ �� ó��.

	// ������ ó������ ��ų���� ���� ����.
	int m_iSkillResetStartJobDegree;
	int m_iSkillResetEndJobDegree;

	// cheatset ���� addskill ġƮŰ�� ���� ���, �������� ���� ��ų ���� ó���� �ؼ��� �ȵȴ�.
	// ��ų������ db ������ ��ġ�� �Ǿ������Ƿ� ���� ���߿� ��Ŷ�� ���� �Ǵµ� ��� ���� ��ų��
	// ��ų���� ��Ŷ���� ���� Ŭ���̾�Ʈ���� ���µȴ�.
	// ���� SCAddSkill ��Ŷ�� ġƮŰ������ ���̹Ƿ� �� ��Ŷ�� ���� ���� ��ų���� ��Ŷ�� 1ȸ �����ϵ��� �Ѵ�.
	bool m_bAddSkillCheatEnabled;

	std::map<int, std::string> m_SkillMovieDataList;
	bool m_bRequestWait;

public:
	bool Initialize( void );
	void InitializeLocalPlayerSkills( void );
	void InitializeEnchantPassiveSkills( void );		// 2�� �������� ���� ��ȭ ��ų ����.
	int QueryPVEOrPVP( void );
	bool InitializeSkillMovieTable();
	const char* GetSkillMovieName(int skillId) const;

	void AddSkill( int nSkillID, int iLevel = 1 , int nSkillPage = DualSkill::Type::None );
	void DelSkill( int nSkillID , int nSkillPage = DualSkill::Type::None );

	void ErrorMessage_ReservationSkill( const int nError );
	void Unlock_ReservationSkill( const int nSkillID );
	void Acquire_ReservationSkill( const int nSkillID, const bool bRefreshDialog = true );
	void LevelUpSkill( int nSkillID, int nLevel, const bool bRefreshDialog = true );

	void InitializeSkillTree();
	void RefreshSkillTree();

	void AddJobSkillTree( int iJobID );
	void RemoveJobSkillTree( int iJobID );
protected:
	virtual void _OnRecvSkillMessage( int nSubCmd, char *pData, int nSize );
	virtual void _OnRecvLifeSkillMessage( int nSubCmd, char *pData, int nSize );
	virtual void _OnRecvPVPSkillMessage( int nSubCmd, char *pData, int nSize );

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	virtual void OnRecvSkillList( SCSkillList *pPacket );
	virtual void OnRecvSkillAdd( SCAddSkill *pPacket );
	virtual void OnRecvSkillDel( SCDelSkill *pPacket );
	virtual void OnRecvSkillLevelUp( SCSkillLevelUp *pPacket );
	virtual void OnRecvOtherPlayerSkillLevelUp( SCOtherPlayerSkillLevelUp* pPacket );
	virtual void OnRecvPushSkillPoint( SCPushSkillPoint* pPacket );		// setsp ġƮŰ������ ����.
	virtual void OnRecvUnlockSkill( SCUnlockSkill* pPacket );
	virtual void OnRecvAcquireSkill( SCAcquireSkill* pPacket );
	virtual void OnRecvSkillReset( SCSkillReset* pPacket );
	virtual void OnRecvCanUseSkillResetCashItem( SCCanUseSkillResetCashItem* pPacket );
	virtual void OnRecvUseSkillResetCashItemRes( SCUseSkillResetCashItemRes* pPacket );
#ifdef PRE_ADD_PRESET_SKILLTREE
	virtual void OnRecvSkillSetList(SCSKillSetList * packet);
	virtual void OnRecvSkillSetSaveResult(SCSKillSaveResult * pPacket);
	virtual void OnRecvSkillSetDeleteResult(SCSKillDeleteResult * pPacket);
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE
	
	virtual void SendSkillLevelUpReq( int iSkillID, int iUsePoint );
	virtual void SendAcquireSkillReq( int iSkillID );
	virtual void SendUnlockSkillByMoneyReq( int iSkillID );
#ifdef PRE_MOD_SKILLRESETITEM
	virtual void SendUseSkillResetCashItemReq(const INT64 biItemSerial, const int iStartJobDegree, const int iEndJobDegree, const int iInvenType, const int iInvenIndex);
#else
	virtual void SendUseSkillResetCashItemReq( INT64 biItemSerial, int iStartJobDegree, int iEndJobDegree );
#endif

	void ApplySkillPage(int nSkillPage);
	virtual void OnRecvSkillPageCount( SCSkillPageCount *pPacket );
	virtual void OnRecvChangeSkillPageRes(SCChangeSkillPage* pPacket);
	virtual void SendChangeSkillPage(int nSkillPage , bool bForce = false);

	int GetSkillPageCount() { return m_iSkillPageCount; }
	int GetCurrentSkillPage() { return m_iSkillTreePage; }
	int GetNeedRefreshSkillPage(){ return m_iNeedRefreshSkillPage; }

	void SetSkillPage(int iSkillPage){ m_iSkillTreePage = iSkillPage; }
	void SetSkillPoint(int iSkillPoint , int nSkillPage) 
	{
		if(nSkillPage < DualSkill::Type::MAX && nSkillPage >= DualSkill::Type::Primary)
			m_iSkillPoint[nSkillPage] = iSkillPoint; 
	}
	void AddSkillPoint( int iAddPoint , int nSkillPage )
	{
		if(nSkillPage < DualSkill::Type::MAX && nSkillPage >= DualSkill::Type::Primary)
			m_iSkillPoint[nSkillPage] += iAddPoint; 
	}
	void SetSkillPoint( int iSkillPoint ) { m_iSkillPoint[m_iSkillTreePage] = iSkillPoint; };	
	int GetSkillPoint( void ) { return m_iSkillPoint[m_iSkillTreePage]; };
	void ResetSkillsByJob( int iStartJobDegree, int iEndJobDegree, int iRewardSP ,int nSkillPage);

	DnSkillHandle FindSkillBySkillPage( int nSkillID , int nSkillPage , bool bEntirePage = false );

	DnSkillHandle FindSkill( int nSkillID );
	DnSkillHandle FindZeroLevelSkill( int nSkillID );
	DnSkillHandle FindLockedSkill( int nSkillID );

	void GetSkillTreeSlotInfo( int iJobID, int iSkillSlotIndex, /*OUT*/ S_SKILLTREE_SLOT_INFO* pSkillSlotInfo );

	DnSkillHandle FindSkillBySkillBookItemID( int iSkillBookItemID );
	DnSkillHandle FindZeroLevelSkillBySkillBookItemID( int iSkillBookItemID );

	DnSkillHandle FindEmblemSkill( void );

#ifdef PRE_ADD_LEVELUP_GUIDE
	void GetLevelUpSkillInfo(std::vector<int>& newSkillIdList, int newLevel, int jobId);
	void GetLevelUpSkillInfo(std::vector<int>& newSkillIdList, int newLevel, const std::vector<int>& jobHistoryVec);
#endif

	void ResetNewAcquireSkills( void );

	int GetWholeUsedSkillPoint( void );

	int _GetAccumlatedNeedSkillPoint( DnSkillHandle hSkill, int iSkillLevel );
	int GetUsedSkillPointInThisJob( int iJobID );

	// �� ��ų�� ��� �� �ִ��� ��� �˻�. ���� ��ų�� ���ٴ� ���� ����� ȹ��(1��)�� ���ÿ� ó���Ǵ� �����̴�.
	CDnSkillTreeSystem::RESULT_CODE CanUnlockThis( DnSkillHandle hSkill );

	void RefreshAllSkillsDecreaseMP( void );

	int GetAvailSkillPointForThisJob( int iNeedJobID );

	bool IsExclusiveSkill( int iSkillID, int iExclusiveID );
	void GetSkillLevelList( int iSkillID, /*IN OUT*/ vector<int>& vlSkillLevelList, int iLevelDataType );

	bool IsRequestWait() { return m_bRequestWait; }

	// �ӽ÷� 2�� ���� ��ų�� �� �� �ְ� ���ֱ� ���� �ӽ� ��ų �߰�.
	void AddTempSkill( int iSkillID );
	void RemoveTempSkill( int iSkillID );
	void RemoveAllTempSkill( void );
	bool IsTempSkillAdded( void ) { return m_bTempSkillAdded; };

	void SendLifeSkillDelete( int nSkillID );
	void OnRecvLifeSkillAdd( SecondarySkill::SCCreate* pPacket );
	void OnRecvLifeSkillDelete( SecondarySkill::SCDelete* pPacket );
	void OnRecvLifeSkillUpdateExp( SecondarySkill::SCUpdateExp* pPacket );
	void OnRecvLifeSkillList( SecondarySkill::SCList* pPacket );

	CSecondarySkillRepository& GetLifeSkillRepository() { return m_LifeSkillRepository; }
	CSecondarySkill * GetSecondarySkill( SecondarySkill::SubType::eType eSecondaryType );

	bool CheckAvailableDeleteLifeSkill( int nSkillID );

	// ������ų ���� ����
	void DoLifeSkill( int nSkillID );
	void DoSkillFishing();
	void DoSkillCooking();
	void DoSkillNone();

	void LoadGuildWarSkillList( PvPCommon::Team::eTeam eTeam );
	DnSkillHandle FindGuildWarSkill( int nSkillTableID );
	bool IsGuildWarSkill( int nSkillTableID );
	vector<DnSkillHandle>& GetGuildWarSkillList() { return m_vlhGuildWarSkillList; }

	void SendAcquireGuildWarSkillReq( int nSkillID, int nSkillLevel, int nReservationIndex = 0 );
	void SendGuildWarSkillInit();
	void SendUseGuildWarSkill( int nSkillID );
	void SendGuildSkillSwapIndex( int nFromIndex, int nToIndex );
	void OnRecvAcquireGuildWarSkillReq( SCPvPTryAcquireSkill* pPacket );
	void OnRecvGuildWarSkillInit( SCPvPInitSkill* pPacket );
	void OnRecvUseGuildWarSkill( SCPvPUseSkill* pPacket );
	void OnRecvGuildWarSkillState( SCPvPOccupationSkillState* pPacket );
	void OnRecvGuildSkillSwapIndex( SCPvPSwapSkillIndex* pPacket );

	//��ų ������ ��� �߰���
	void AddSkill( DnSkillHandle hSkill );
	void RemoveSkill( DnSkillHandle hSkill );

	// AcquireSkill �� ���ؼ� �� �������� ���������� ��ų���� ����ؾ��ϴ� ��찡 �����.
	void ChangeSkillLevelUp( int nSkillID, int nOrigLevel , bool bCurrentPageOnly = false ); 

	bool HasSameGlobalIDSkill( DnSkillHandle hSkill );

#if defined(PRE_FIX_NEXTSKILLINFO)
public:
	SKILL_LEVEL_INFO* GetSkillLevelTableIDList(int nSkillID, int nApplyType);

protected:
	static SKILL_LEVEL_INFO ms_SkillLevelTableInfo;
	SKILL_LEVEL_TABLEID_LIST m_SkillLevelTableIDList[2];

	void InitSkillLevelTableIDList();
	void AddSkillLevelTableID(int nSkillID, int nSkillLevel, int nSkillLevelTableID, int nApplyType);
	
#endif // PRE_FIX_NEXTSKILLINFO


#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
public:
	bool IsAvailableSPByJob(DnActorHandle hActor, vector<int>& needSPValues);
	bool IsAvailableSPByJob(std::vector<int>& jobHistory, vector<int>& needSPValues, int nIndex);
	void GetNeedSPValuesByJob(int nSkillID, std::vector<int>& nNeedSPValues);
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
public:
	bool IsTotalLevelSkill(int nSkillID);
	bool IsAvailableTotalSkill(DnActorHandle hActor, CDnSkill* pSkill);

	CDnTotalLevelSkillSystem* GetTotalLevelSkillSystem();

	void AddTotalLevelSkill(DnSkillHandle hSkill);

	void RequestAddTotalLevelSkill(int nSlotIndex, int nSkillID);
	void RequestRemoveTotalLevelSkill(int nSlotIndex);

	int GetTotalLevel();
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
public:
	enum{
		eNeed_SkillMax_None =		0x00000000,
		eNeed_SkillMax_Acquire =	0x00000001,
		eNeed_SkillMax_Unlock =		0x00000010,
		eNeed_SkillMax_LevelUp =	0x00000100,
	};

	enum eSkillMinus_State
	{
		eSkillMinus_Enable,
		eSkillMinus_Disable,
		eSkillMinus_Delete
	};

	struct SReservationSkillMax
	{
		int m_nSkillID;
		int m_nNeedSkillMax;
		int m_nNowSkillLevel;
		int m_nLevelUpCount;

		SReservationSkillMax() : m_nSkillID( 0 ), m_nNeedSkillMax( eNeed_SkillMax_None ), m_nNowSkillLevel( 0 ), m_nLevelUpCount( 0 )
		{}
	};
protected:
	std::vector< TReservationSkillReq > m_vecReservationSkillData;
	std::map< int, DnSkillHandle > m_mapReservationSkillHandle;
	std::map< int, DnSkillHandle > m_mapReservationGuildSkillHandle;
	int m_nReservationSkillNeedSP;
	INT64 m_nReservationSkillNeedMoney;

	int GetUsedReservationSkillPointInThisJob( const int iJobID );
	void GetPossessReservationSkill( vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> & vlPossessedSkillInfos );

public:
	virtual void OnRecvReservationSkillList( const SCReservationSkillListAck * pData );
	void SendReservationSkillList();

	bool IsJobSP_SkillReservation( const int nJobID );

	bool IsClear_SkillLevelUp();
	void Clear_SkillLevelUp();

	int GetReservationSkillNeedSP();
	INT64 GetReservationSkillNeedMoney();
	DnSkillHandle GetReservationSkill( const int nSkillID );

	void GetReservationSkillLevelMax( SReservationSkillMax & sData );
	void Change_SkillLevelUpMax( const int nSkillID );
	void Change_SkillLevelUp( const int nSkillID, const int nSkillLevel, const char cState );

	bool IsGlobalSkillGroupDown( DnSkillHandle hSkill );
	int IsReservationSkillDown( const int nSkillID );
	void Change_SkillLevelDownMax( const int nSkillID );
	void Change_SkillLevelDown( const int nSkillID, const int nSkillLevel );

	void SetReservationSkill_Item( const int nSkillID );

	DnSkillHandle GetReservationGuildSkill( const int nSkillID );
	void Change_GuildSkillLevelUp( const int nSkillID );
	void Change_GuildSkillLevelDown( const int nSkillID );
	bool IsReservationGuildSkillDown( const int nSkillID );
	int GetReservationGuildSkillCount();
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

#if defined( PRE_ADD_PRESET_SKILLTREE )
public:
	struct SPresetSkillTree
	{
		std::vector< std::pair<int, BYTE> > m_Data;
		std::wstring m_wszTreeName;
		BYTE m_cIndex;
		bool m_bAdvice;
		int m_nLevelLimit;

		SPresetSkillTree() : m_cIndex(0), m_bAdvice(false), m_nLevelLimit(0)
		{}

		void Clear()
		{
			m_Data.clear();
			m_wszTreeName.clear();
			m_cIndex = 0;
			m_bAdvice = true;
			m_nLevelLimit = 0;
		}
	};

	enum{
		E_DEFAULT_PRESET_ID = 255,
		E_ADVICE_INDEX_START = 100,
	};

protected:
	int m_nPresetSkillDeleteIndex;
	SPresetSkillTree m_SavePresetSkillTree;
	std::vector< SPresetSkillTree > m_vecPresetSkillTree;

public:
	void SendPresetSkillTreeList();
	void SendPresetSkillTreeSave( std::wstring wszName );
	void SendPresetSkillTreeDel( const BYTE cIndex );

	void SavePresetSkill( const int nJobID, const int nSlot );
	bool IsMySkillTreeSkill( const vector<int> & vlJobHistory, const int nSkillID );

	void AdviceSkillTreeSort( std::vector< std::pair<int, BYTE> > & inData, std::vector< std::pair<int, BYTE> > & outData );
	void AdviceSkillTreePush( std::vector< std::pair<int, BYTE> > & inData, std::vector< std::pair<int, BYTE> > & outData, const int nJobID, const int nSlot );
	void InitAdviceSkillTreeTable();
	void SetPresetSkillTree();

	std::vector< SPresetSkillTree > & GetPresetSkillTree();
	bool IsPresetSkillTreeSave();
	int GetEmptyEnablePresetIndex();
	void PresetSkillLevelUp( const int nSkillID, const BYTE cLevel );
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )
};

#define GetSkillTask()	CDnSkillTask::GetInstance()