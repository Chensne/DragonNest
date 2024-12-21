#pragma once

#include "DnActor.h"
#include "MAWalkMovement.h"
#include "MASingleBody.h"

class CEtWorldEventArea;

class CDnNPCActor : public CDnActor, public MASingleBody
{
public:
	enum _StateMark
	{
		MarkTypeNone = -1,
		/*
		Available  수락 가능 - 느낌표 
		Playing    플레이중  - 물음표(회색) 
		Recompense 보상      - 물음표

		뒤에 숫자는 느낌표(물음표) 갯수
		Main -> 주황색
		Sub  -> 파란색
		*/

		QuestAvailable_Main = 1,
		QuestPlaying_Main = 2,
		QuestRecompense_Main = 3,

		QuestAvailable_Sub = 10,
		QuestPlaying_Sub = 11,
		QuestRecompense_Sub = 12,

		CrossFavorGrant    =  15,
		CrossFavorNow      =  16,
		CrossFavorComplete =  17,

		CrossReQuestGrant = 19,
		CrossReQuestNow = 20,
		CrossReQuestComplete = 21,

		MailReceiveNew = 22,

		CrossQuestGlobalGrant = 23,
		CrossQuestGlobalNow = 24, 
		CrossQuestGlobalComplete = 25,

		CrossNoticeWarpNpc = 26, // 워프엔피씨 강제로 풍선 띄워주고 싶답니다.
		CrossNoticePcBang = 27,  // PcBang 알림
		CrossNoticeRepair = 28,  // 수리 알림
		//added new!
		CrossNoticePresent1 = 29,  // 수리 알림
		CrossNoticePresent2 = 30,  // 수리 알림
		CrossNoticeEventNPC1 = 31,  // 수리 알림
		CrossNoticeEventNPC2 = 32,  // 수리 알림
		CrossNoticeLife = 33,  // 수리 알림



		MaxCount,
	}; // 이 타입들이 퀘스트 우선순위및 진행상황과 연관이 있습니다. 이부분이 변경되면 GetQuestStateFromMark / CalcStateMarkRank 부분도 수정해주세요

	enum emJobType
	{
		typeNone,			// 특별히 없음
		typeWeapon,			// 무기상인
		typeParts,			// 방어구상인
		typeNormal,			// 잡화상인
		typeStorage,		// 창고
		typeMarket,			// 무인거래소
		typeMail,			// 우편함
		typeGuild,			// 길드 관리
		typeWarriorSkill,	// 워리어스킬
		typeArcherSkill,	// 아처스킬
		typeSoceressSkill,	// 소서리스스킬
		typeClericSkill,	// 클레릭스킬
		typePlate,	        // 문장보옥
		typeCompleteQuest,  // 모험자 게시판 퀘스트 보상 전용 엔피시
		typeAdventureBoard, // 일반 모험자 게시판 타입
		typeVehicle,        // 탈것 조련사
		typeTicket,         // 비행선 티켓 엔피씨
		typeAcademicSkill,  // 아카데믹 스킬
		typeKaliSkill,      // 칼리 스킬
		typeAssassinSkill,
		typePcBang,         // PcBang 
		typeRepair,         // 수리 엔피씨
		//new!
		typePresent,
		typeEvent,
		typeWebToon,
		typeLenceaSkill,
		typeMachinaSkill,
		typeWarp,
		typeAmount,	        // 총개수
	}; // 이부분이 추가되고 미니맵, 존맵에 아이콘 추가할시 관련 부분 찾아서 설정해 주셔야 합니다.

	enum eSoundType
	{
		BeginTalk = 0, 
		FinishTalk,
		OpenShop,
		CloseShop
	};

	struct NpcData 
	{
		UINT nNpcID;
		int nParam[2];
		std::wstring wszNpcName;
		std::wstring wszTalkFileName;

		std::vector<int>	BeginTalkSound;
		std::vector<int>	FinishTalkSound;
		std::vector<int>	OpenShopSound;
		std::vector<int>	CloseShopSound;
		EtSoundChannelHandle	m_hCurPlaySound;
		NpcData():nNpcID(0xffffffff) { memset(nParam, 0, sizeof(nParam)); }
	};

	CDnNPCActor( int nClassID, bool bProcess = true );
	virtual ~CDnNPCActor();

#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	virtual bool Initialize();
#endif
	virtual bool IsNpcActor(){ return true; }

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void Initialize(NpcData& data);
	virtual void RenderCustom( float fElapsedTime );	
	virtual bool LoadSkin( const char *szSkinName, const char *szAniName, bool bTwoPassAlpha = true );

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new MAWalkMovement();
		return pMovement;
	}

	// ActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void OnChangeAction( const char *szPrevAction );
	virtual void OnChangeActionQueue( const char *szPrevAction );

	// TransAction Message
	virtual void OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket );

	
protected:

private:
	EtTextureHandle m_hStateMarkTex[ MaxCount ];
	NpcData m_NpcData;
	SOBB m_SafeZone;
	bool m_bExistSafeZone;
	
	int m_nStateMark;
	LOCAL_TIME m_nLastUpdateTime;

	emJobType m_emNPCJobType;
	bool m_bEnableOperator;
	DnPropHandle m_hPropHandle;
	std::string m_ForceNextActionName;

	EtSoundChannelHandle m_hScriptSoundChannel;

public:

	static CDnNPCActor* FindNpcActorFromID( int nNpcID );
	static void GetAvailableQuestList(std::vector<int> &vecAvailableQuestList , int nNpcID );
	static void GetAvailableQuestName(std::vector<std::wstring> &vecAvailableQuestName , int nNpcID );

	void RefreshSafeZone();
	void UpdateStateMark();
	void OnSoundPlay(eSoundType type);

	emJobType GetNPCJobType() { return m_emNPCJobType; }
	void SetNPCJobType( emJobType jobType ) { m_emNPCJobType = jobType; }
	void SetPropHandle( DnPropHandle hProp ) { m_hPropHandle = hProp; }
	DnPropHandle GetPropHandle() { return m_hPropHandle; }

	void EnableOperator( bool bEnable ) { m_bEnableOperator = bEnable; }
	bool IsEnableOperator() { return m_bEnableOperator; }

	bool CanTalk();
	bool IsNpcProp() { return ( m_hPropHandle ) ? true : false; }

	NpcData& GetNpcData() { return m_NpcData; }
	
	int GetNPCStateMark() { return m_nStateMark; }
	static int CalcStateMark( int nNpcID );
	static int GetQuestStateFromMark( int nStateMark ); // 스테이트 마크를 이용하여 진행중인지 , 수락가능 상태인지 , 완료가능인지를 알수 있도록 합니다
	// 0 = 수락가능 , 1 = 진행중 , 2 = 완료 : 현재상태가 완료인지 아는 방법은 스테이트 마크를 통해서뿐입니다.

	static int CalcStateMarkRank ( int nStateMark );    // 퀘스트 마크에 따른 중요도를 채점합니다.

	virtual void OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time);
	void SetForceActionOnFinishCurrentAction(const std::string& actionName);

	void SetScriptSoundChannel( EtSoundChannelHandle hScriptSoundChannel ) { m_hScriptSoundChannel = hScriptSoundChannel; }
	void RemoveScriptSoundChannel() { CEtSoundEngine::GetInstance().RemoveChannel( m_hScriptSoundChannel ); }
};

DnActorHandle CreateNpcActor(UINT uUniqueID, UINT nNpcID, EtVector3 Position, float fRotate);