#pragma once

#if defined(_VILLAGESERVER) || defined(_GAMESERVER)
class CDNGameDataManager;
#else
#include "DNTableFile.h"
#endif

//------------------------------------------------------------------------------
/**
    Note 한기:

	스킬 트리 관련 테이블에서 데이터를 로드해서 쥐고 있다가 
	데이터 확인 요청/데이터 조회 요청을 처리합니다.

	데이터가 구성된 후에 무언가를 새로 생성하거나 데이터를 읽어놓고 변경을 
	가하지 않기 때문에 멀티스레드 고려가 되어있지 않습니다.
*/
//------------------------------------------------------------------------------


// 스킬 슬롯 최대 갯수입니다. 트리를 어떻게 꾸미느냐에 따라 달라지기 때문에 
// 만약 최대 스킬 슬롯을 늘려야 한다면 이 상수값을 변경하고 
// R:/GameRes/Resource/UI/Skill/SkillTreeContentDlg.ui 파일에서도 같이 슬롯 갯수를 맞춰줘야 합니다.
const int MAX_SKILL_SLOT_COUNT = 28;

const int MAX_PARENT_SKILL_COUNT = 3;
const int MAX_SKILL_PER_LINE = 4;


class CDnSkillTreeSystem
{
private:
	struct S_UNLOCK_CONDITION
	{
		int iUnlockSkillBookItemID;

		S_UNLOCK_CONDITION( void ) : iUnlockSkillBookItemID( 0 )
		{

		};
	};

	struct S_ACQUIRE_CONDITION
	{
		// 스킬을 언락하기 위한 필요 캐릭터 레벨. 각 스킬마다 스킬레벨테이블에 1렙에 정의되어있음.
		int iNeedLevel;

		// 요구되는 부모 스킬 레벨. NodeInfo 에 있는 순서와 일치하며 또 그래야만 함.
		vector<int> vlParentSkillNeedLevel;

		S_ACQUIRE_CONDITION( void ) : iNeedLevel( 0 )
		{

		};
	};

public:
	// UI 표시용 구조체.
	struct S_NODE_RENDER_INFO
	{
		int iSkillID;
		int iSlotIndex;
		int iJobID;
		const S_UNLOCK_CONDITION* pUnLockCondition;
		const S_ACQUIRE_CONDITION* pAcquireCondition;
		vector<int> vlChildSlotIndexInJob;		// 왼쪽부터 오른쪽으로 나가는 트리 슬롯 인덱스. 같은 직업 트리 안에서만 있는 자식.

		// 다른 직업의 스킬이 부모 스킬이 될 수 있음.
		vector<int> vlParentSkillJobIDs;
		vector<int> vlParentSkillIDs;

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
		vector<int> vlNeedJobSP;							//직업군 별 필요 sp값
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

		S_NODE_RENDER_INFO() : iSkillID( 0 ), iSlotIndex( 0 ), iJobID( 0 ), pUnLockCondition( NULL ) , pAcquireCondition( NULL ){};
		S_NODE_RENDER_INFO( int argiSkillID, int argiSlotIndex, int argiJobID, 
							const S_UNLOCK_CONDITION* argpUnLockCondition, const S_ACQUIRE_CONDITION* argpAcquireCondition ) : 
							iSkillID( argiSkillID ), iSlotIndex( argiSlotIndex ), iJobID( argiJobID ), 
							pUnLockCondition( argpUnLockCondition ), pAcquireCondition( argpAcquireCondition ) {};
	};

private:
	// 스킬 트리 노드 정보.
	struct S_NODE_INFO
	{
		int iJobID;
		int iSkillID;
		int iNeedSkillPointToAcquire;		// 획득하기 위해 필요한 스킬 포인트

		//------------------------------------------------------------------------------
		/** 
			트리 슬롯 ID 는 왼쪽부터 오른쪽으로 0부터 순차적으로 나갑니다.
			현재는 4 칸이 한 줄 기준임. 
		*/
		//------------------------------------------------------------------------------
		int iTreeSlotIndex;
		bool bDefaultLocked;

		// 이 노드의 스킬을 언락, 획득하기 위한 조건
		S_UNLOCK_CONDITION UnLockCondition;
		S_ACQUIRE_CONDITION AcquireCondition;

		vector<S_NODE_INFO*> vlpChildNodeInfo;
		vector<S_NODE_INFO*> vlpParentNodeInfo;		// 부모 스킬노드는 최대 3개가 될 수 있음. 모두 있어야 스킬 획득 가능.

#if defined( PRE_ADD_PRESET_SKILLTREE )
		vector<int> vecAdviceSkillLevel;
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )

#if defined( PRE_ADD_ONLY_SKILLBOOK )
		bool bNeedSkillBook;
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )

		// 노드 렌더링 정보
		S_NODE_RENDER_INFO* pNodeRenderInfo;

		S_NODE_INFO() : iTreeSlotIndex( 0 ), iJobID( 0 ), iSkillID( 0 ), iNeedSkillPointToAcquire( 0 ), 
						bDefaultLocked( false ), pNodeRenderInfo( NULL )
#if defined( PRE_ADD_ONLY_SKILLBOOK )
						, bNeedSkillBook( false )
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )
		{
			
		};
	};

	struct S_SKILLTREE_INFO
	{
		int iJobID;
		
		vector<S_NODE_INFO*> vlpNodeList;				// 리스트로 스킬 노드 접근.
		map<int, S_NODE_INFO*> mapNodeBySkillID;		// 스킬 ID를 키 값으로 스킬 노드에 접근.

		vector<S_NODE_RENDER_INFO> vlNodeRenderInfo;	// 노드 렌더용 정보.

		~S_SKILLTREE_INFO()
		{
			SAFE_DELETE_PVEC( vlpNodeList );
		}
	};
	
	vector<S_SKILLTREE_INFO*> m_vlpJobSkillTreeList;
	map<int, S_SKILLTREE_INFO*> m_mapSkillTreeByJobID;	// 직업(잡) ID 로 스킬 트리에 접근.
	map<int, S_NODE_INFO*> m_mapNodeListBySkillBookID;	// 스킬북 아이템 ID로 노드 정보를 찾을 수 있음.

protected:
	S_NODE_INFO* _FindNode( int iSkillID );
	void _MakeupParentSkillLinks( DNTableFileFormat* pSkillTreeTable, DNTableFileFormat* pSkillTable );

public:
	CDnSkillTreeSystem(void);
	~CDnSkillTreeSystem(void);

	// 테이블 로드해서 필요한 정보 초기화.
	// 서버에서는 DataManager 에서 정리해놓은 데이터가 있기 때문에 이걸 사용해서 초기화 한다.
#if defined(_VILLAGESERVER) || defined(_GAMESERVER)
	bool InitializeTableUsingDataManager( CDNGameDataManager* pDataManager );
#endif
	bool InitializeTable( void );

	struct S_POSSESSED_SKILL_INFO
	{
		int iSkillID;
		int iSkillLevel;		// 레벨테이블 ID 아님. 순수 스킬 현재 레벨
		bool bCurrentLock;		// 현재 락이 된 상태인지 아닌지.

		S_POSSESSED_SKILL_INFO() : iSkillID( 0 ), iSkillLevel( 0 ), bCurrentLock( true )
		{

		};

		S_POSSESSED_SKILL_INFO( int argiSkillID, int argiSkillLevel, bool argbCurrentLock ) : iSkillID(argiSkillID), 
																					iSkillLevel(argiSkillLevel), bCurrentLock(argbCurrentLock)
		{

		};
	};

	// 애초부터 호출되면 안되는 케이스.
	enum ERROR_CODE
	{
		E_NONE = 0,					// 에러 아님
		E_CANT_FIND_SKILLNODE,		// 스킬 트리노드를 찾을 수 없음. (직업이 다른 경우도 포함)
		E_MISMATCH_JOB,
		E_INVALID_CALL,				// 잘못된 호출
	};

	// 
	enum RESULT_CODE
	{
		R_SUCCESS,
		R_NOT_ENOUGH_CHAR_LEVEL,			// 캐릭터 요구레벨이 모자람.
		R_INVALID_SKILLBOOK_ITEMID,			// 스킬북 아이템 ID 가 맞지 않음.
		R_DONT_HAVE_PARENT_SKILL,			// 선행(부모) 스킬이 없음.
		R_LOCKED_PARENTSKILL,				// 부모 스킬이 락이 되어있음.
		R_NOT_ENOUGH_PARENT_SKILL_LEVEL,	// 부모스킬의 레벨이 충족되지 않음.
		R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE,	// 스킬 포인트가 모자라서 스킬을 획득할 수 없음.
		R_EXCLUSIVE_SKILL,
		R_ONLY_SKILL_BOOK,					// 스킬북으로만 배울 수 있는 스킬.
		R_FAIL,
		R_ERROR,
	};

#if defined( PRE_ADD_PRESET_SKILLTREE )
	enum
	{
		E_ADVICE_SKILL_LEVEL_COUNT = 3
	};
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )

	// 현재 획득 스킬, 스킬북 사용 정보.
	struct S_TRY_UNLOCK
	{
		int iCurrentCharLevel;
		int iTryUnlockSkillID;					// 얻고자 하는 스킬 ID
		int iSkillBookItemID;					// 사용하는 스킬 북 ItemID
		int iJobID;								// 현재 직업. 전직했을 때 이전 스킬 언락할때 어찌할진 생각해보자.
		//const vector<S_POSSESSED_SKILL_INFO>& vlPossessedSkillInfo;	// 보유하고 있는 스킬 정보. (쓸데없는 복사 방지)
	
		//S_CURRENT_USERSKILL_INFO( const vector<S_POSSESSED_SKILL_INFO>& _vlSkillInfo ) : 
		//							 vlPossessedSkillInfo( _vlSkillInfo ),
		//							 iCurrentCharLevel( 0 ), iSkillBookItemID( 0 ), iTryUnlockSkillID( 0 ), iJobID( 0 )
		//{

		//};

		S_TRY_UNLOCK( void ) : iCurrentCharLevel( 0 ), iTryUnlockSkillID( 0 ), 
										   iSkillBookItemID( 0 ), iJobID( 0 )
		{

		}
	};

	struct S_TRY_ACQUIRE
	{
		int iCurrentCharLevel;
		int iTryAcquireSkillID;
		int iHasSkillPoint;
		int iJobID;

		const vector<S_POSSESSED_SKILL_INFO>& vlPossessedSkillInfo;

		S_TRY_ACQUIRE( const vector<S_POSSESSED_SKILL_INFO>& _vlSkillInfo ) : 
							vlPossessedSkillInfo( _vlSkillInfo ), iCurrentCharLevel( 0 ),
																  iTryAcquireSkillID( 0 ), 
																  iHasSkillPoint( 0 ), iJobID( 0 )
		{

		};
	};

	// 요청 결과.
	struct S_OUTPUT
	{
		RESULT_CODE eResult;
		ERROR_CODE eErrorCode;

		S_OUTPUT() : eResult( R_FAIL ), eErrorCode( E_NONE )
		{
			
		};
	};

#ifdef PRE_ADD_ONLY_SKILLBOOK
	bool IsNeedSkillBook( int iSkillID, bool& bExistSkill );
#endif // PRE_ADD_ONLY_SKILLBOOK

	// 스킬 언락 시도. 이 함수를 호출하기 전에 이미 언락된 스킬은 호출하지 않도록 확인해주세요!
	void TryUnLockSkill( /*IN*/ const S_TRY_UNLOCK& PresentInfo, /*OUT*/ S_OUTPUT* pResult );
	void TryAcquireSkill( /**/ const S_TRY_ACQUIRE& TryAcquire, /*OUT*/ S_OUTPUT* pResult );
	int FindSkillBySkillBook( int iSkillBookItemID );
#ifdef PRE_ADD_LEVELUP_GUIDE
	void GetLevelUpSkillInfo(std::vector<int>& newSkillIdList, int newLevel, int jobId);
	void GetLevelUpSkillInfo(std::vector<int>& newSkillIdList, int newLevel, const std::vector<int>& jobHistoryVec);
#endif

//#ifndef (_GAMESERVER)&(_VILLAGESERVER)
	// 클라이언트에서만 사용하는 트리 타기.
	int GetNodeCount( int iJobID ) const;		// 유효한 스킬 노드 갯수.
	int GetSlotCount( int iJobID ) const;		// 사용하는 슬롯 갯수. (중간에 빈칸 존재할 수 있음)

	void GetNodeRenderInfo( int iSlotIndex, int iJobID, /*OUT*/S_NODE_RENDER_INFO* pNodeRenderInfo ) const;
//#endif

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
	void GetNeedSPValuesByJob(int nSkillID, std::vector<int>& nNeedSPValues);
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

	int IsParentSkill( const int nParentSkillID, const int nChildSkillID );

	void GetParentSkillData( const int nChildSkillID, std::vector< std::pair<int, int> > & vecParentSkillData );

#if defined( PRE_ADD_PRESET_SKILLTREE )
	void GetAdviceSkillTree( const int nAdviceSkill_Index, const std::vector<int> & vecJobList, std::vector< std::pair<int, BYTE> > & vecData );
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )
};
