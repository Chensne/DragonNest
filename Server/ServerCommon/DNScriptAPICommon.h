#pragma once

class CDNUserBase;
class CDnNpc;

namespace DNScriptAPI
{
	int CheckInvenForQuestReward(CDNUserBase* pUser, const TQuestReward& QuestReward, const bool bSelectedArray[]);

	/*
	desc   : 다음 대사로 연결한다.
	param  : 유저(꼭 CDNUserSession*), NPC인덱스, 토크인덱스, 토크파일이름
	return : void
	*/
	void api_npc_NextTalk(CDNUserBase* pUser, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

	/*
	desc   : 다음 스크립트로 연결한다.
	param  : 유저(꼭 CDNUserSession*), NPC, 토크인덱스, 토크파일이름
	return : void
	*/
	void api_npc_NextScript(CDNUserBase* pUser, CDnNpc* pNpc, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

	/*
	desc   : 대사 파라메터를 세팅한다(스트링).  
	param  : 유저, 파라메터키, 파라메터문자의 인덱스
	return : -1 유저를 찾을수 없음, -2 파라메터 갯수가 넘어서서 더이상 세팅할수 없음. 1 이면 성공
	sample : 
	저는 {%color} 색을 좋아합니다.  라는 문장을 만들때.
	api_npc_SetParamString("{%color}, math.random(2) );  이런식으로 만들어준다.
	1은 TalkParamTable.xls 에 1 : 빨강, 2: 파랑 따로 기록 되어있어야한다.

	주의: api_npc_SetParamString 으로 세팅한 파라메터는 api_npc_NextTalk 호출후 clear 되므로 api_npc_NextTalk 호출전에 세팅한다.
	*/
	int api_npc_SetParamString(CDNUserBase* pUser, const char* szParamKey, int nParamID);

	/*
	desc   : 대사 파라메터를 세팅한다(정수).  
	param  : 유저, 파라메터키, 정수값
	return : -1 유저를 찾을수 없음, -2 파라메터 갯수가 넘어서서 더이상 세팅할수 없음. 1 이면 성공
	sample : 
	저는 {%age} 살쯤에 숨지고 싶어요.  라는 문장을 만들때.
	api_npc_SetParamInt("{%age}, math.random(30)+40 );  이런식으로 만들어준다.

	주의: api_npc_SetParamInt 으로 세팅한 파라메터는 api_npc_NextTalk 호출후 clear 되므로 api_npc_NextTalk 호출전에 세팅한다.
	*/
	int api_npc_SetParamInt(CDNUserBase* pUser, const char* szParamKey, int nValue);

	/*
	desc   : 로그를 남긴다.
	param  : 로그 스트링 줄바꿈을 위해 \n 을 포함할것.
	return : void
	*/
	void api_log_AddLog(const char* szLog);

	/*
	desc   : 유저에게 로그를 남긴다. 유저채팅메세지로 전송해줌.
	param  : 로그 스트링 줄바꿈을 위해 \n 을 포함할것.
	return : void
	*/
	void api_log_UserLog(CDNUserBase* pUser, const char* szLog);

	/*
	desc   : 유저에게 퀘스트 관련 정보를 채팅으로 보내준다.
	param  : 
	return : void
	*/
	void api_quest_DumpQuest(CDNUserBase* pUser);

	/*
	desc   : 유저에게 퀘스트를 부여 한다.
	param  : 유저, 퀘스트인덱스, 퀘스트 타입 QuestType_SubQuest = 1, QuestType_MainQuest = 2,
	return : 1 : 성공, -1 : 유저를 찾을수 없음, -2 : 유저가 퀘스트를 가지고 있지 않음
	*/
	int api_quest_AddHuntingQuest(CDNUserBase* pUser, int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt);
	int api_quest_AddQuest(CDNUserBase* pUser, int nQuestID, int nQuestType);

	/*
	desc   : 유저에게 퀘스트를 완료 시킨다.
	param  : 유저, 퀘스트인덱스, 현재 가지고 있는 퀘스트에서 지울것인지 (안지우면 플레이 횟수만 증가 시킨다.), 반복 가능 여부
	return : -1 유저를 찾을수 없음, -2 퀘스트가 진행중이 아님 
	*/
	int api_quest_CompleteQuest(CDNUserBase* pUser, int nQuestID, bool bDelPlayList);

	/*
	desc   : 완료목록에 완료마킹이 되어있는 퀘스트인지?
	param  : 유저, 퀘스트인덱스
	return : -1 유저를 찾을수 없음, 1 이면 완료마킹된 퀘스트, 0 이면 완료마킹 안됨
	*/
	int api_quest_IsMarkingCompleteQuest(CDNUserBase* pUser, int nQuestID);

	/*
	desc   : 유저가 퀘스트를 가지고 있는지 확인한다.
	param  : 유저, 퀘스트인덱스
	return : -1 유저를 찾을수 없음, -2 유저가 퀘스트를 가지고 있지 않음, -1 보다 크면 퀘스트를 가지고 잇음 ( 퀘스트 스텝을 리턴 )
	*/
	int api_quest_UserHasQuest(CDNUserBase* pUser, int nQuestID);

	/*
	desc   : 유저가 몇개의 퀘스트를 진행중인지 확인한다.
	param  : 유저
	return : -1 유저를 찾을수 없음, 그외에 현재 진행중인 퀘스트 갯수 리턴..
	*/
	int api_quest_GetPlayingQuestCnt(CDNUserBase* pUser);
	
	/*
	desc   : npc 인덱스를 리턴한다.
	param  : Npc
	return : -2 npc 를 찾을수 없음, 그외에는 npc인덱스
	*/
	int api_npc_GetNpcIndex(CDnNpc* pNpc);

	/*
	desc   : 유저가 가지고 있는 퀘스트의 스텝, 저널 스텝을 세팅한다.
	param  : 유저, 퀘스트인덱스, 저널 스텝
	return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음.  -3 저널 스텝 범위를 벗어남. 성공일 경우 1을 리턴
	*/
	int api_quest_SetQuestStepAndJournalStep(CDNUserBase* pUser, int nQuestID, short nQuestStep, int nJournalStep);
	
	/*
	desc   : 유저가 가지고 있는 퀘스트의 퀘스트 스텝을 세팅한다.
	param  : 유저, 퀘스트인덱스, 퀘스트 스텝
	return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음. 성공일 경우 1을 리턴
	*/
	int api_quest_SetQuestStep(CDNUserBase* pUser, int nQuestID, short nQuestStep);
	
	/*
	desc   : 유저가 가지고 있는 퀘스트의 퀘스트 스텝을 얻는다.
	param  : 유저, 퀘스트인덱스
	return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음., -1 보다 큰경우 현재 퀘스트의 스텝
	*/
	int api_quest_GetQuestStep(CDNUserBase* pUser, int nQuestID);

	/*
	desc   : 유저가 가지고 있는 퀘스트의 저널 스텝을 세팅한다.
	param  : 유저, 퀘스트인덱스, 저널 스텝
	return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음.  -3 저널 스텝 범위를 벗어남. 성공일 경우 1을 리턴
	*/
	int api_quest_SetJournalStep(CDNUserBase* pUser, int nQuestID, int nJournalStep);

	/*
	desc   : 유저가 가지고 있는 퀘스트의 저널 스텝을 얻는다.
	param  : 유저, 퀘스트인덱스
	return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음., -1 보다 큰경우 현재 저널 스텝
	*/
	int api_quest_GetJournalStep(CDNUserBase* pUser, int nQuestID);
	
	/*
	desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스에 값을 세팅한다.
	param  : 유저, 메모인덱스, 메모값
	return : -1 => 유저를 찾을수 없음. -2 => 유저가 퀘스트를 가지고 있지 않음. -3 메모 인덱스 범위를 벗어남. 성공일 경우 1을 리턴
	*/
	int api_quest_SetQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex, int iVal);

	/*
	desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스의 값을 얻는다.
	param  : 유저, 메모인덱스
	return : -1 => 유저를 찾을수 없음. -2 => 유저가 퀘스트를 가지고 있지 않음. -3 메모 인덱스 범위를 벗어남. -1 보다 큰경우 특정 메모 인덱스의 값
	*/
	int api_quest_GetQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex);
	
	/*
	desc   : 몬스터 잡거나, 아이템 얻는 갯수 카운팅 정보를 세팅하는 함수 이다.
	param  : 유저, 
			퀘스트인덱스, 
			카운팅정보 세팅 슬롯(0부터 49까지 50개의 세팅슬롯이 있음) , 
			카운팅할 타입(	CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3),
			카운팅할 것의 인덱스 0 ~ 60000 까지의 범위, 
			카운팅할 갯수 
	return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음. -3 => 카운팅정보세팅슬롯 범위 초과 -4 => 카운팅타입 or 타겟카운트 범위 초과

	example: 

		api_SetCountingInfo(유저아이디, 퀘스트아이디, 0, CountingType_MyMonsterKill, 10001, 10);
		api_SetCountingInfo(유저아이디, 퀘스트아이디, 1, CountingType_MyMonsterKill, 10002,  5);
		api_SetCountingInfo(유저아이디, 퀘스트아이디, 2, CountingType_MyMonsterKill, 10003, 20);
		api_SetCountingInfo(유저아이디, 퀘스트아이디, 3, Item,	   1001,  5);
		api_SetCountingInfo(유저아이디, 퀘스트아이디, 4, Item,     1002,  1);

		위와 같이 스크립트에서 호출 해줬을 경우 5개의 카운팅 조건이 세팅 되게 된다. 
		맨 윗줄의 뜻은 0번 슬롯에 10001번 몬스터를 10마리 잡으면 조건이 만족한다고 세팅한다.

		위와같이 5개의 슬롯에 10001번 몬스터 10마리, 10002번 몬스터 5마리, 10003번 몬스터 20마리, 1001번 아이템 5개, 1002번 아이템 1개 라고 세팅 하였고
		위 5개의 조건을 만족하게 되면. 퀘스트 스크립트의 OnComplete() 함수를 호출하게 된다.

	*** 카운팅 갯수는 최대 30000 개이다.
	*/
	int api_quest_SetCountingInfo(CDNUserBase* pUser, int nQuestID,  int nSlot, int nCountingType, int nCountingIndex, int nTargetCnt); 


	/*
	desc   : 세팅된 카운팅 정보를 클리어 한다. 
	param  : 유저, 퀘스트 인덱스
	return : -1 => 유저를 찾을수 없음 -2 => 퀘스트를 찾을수 없음,
	*/
	int api_quest_ClearCountingInfo(CDNUserBase* pUser, int nQuestID);
	
	
	/*
	desc   : 세팅된 카운팅들이 모두 완료 되었는지 확인한다.
	param  : 유저, 아이템인덱스, 아이템 카운트
	return : -1 => 유저를 찾을수 없음 -2 => 퀘스트를 찾을수 없음,  -3이면 완료 안됨, 1 이면 모두 완료
	*/
	int api_quest_IsAllCompleteCounting(CDNUserBase* pUser, int nQuestID);

	/*
	desc   : 유저에게 아이템을 넣을만한 인벤여유가 있는지 확인한다.
	param  : 유저, 아이템인덱스, 아이템 카운트
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족
	*/
	int api_user_CheckInvenForAddItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);
	
	/*
	desc   : 유저에게 퀘스트 아이템 목록을 넣을만한 인벤여유가 있는지 확인한다.
	param  : 유저인덱스, 퀘스트 아이템 목록 (루아테이블, [<최대개수>]{퀘스트 아이템인덱스, 퀘스트 아이템 카운트} 가 한 쌍으로 <최대개수 (현재 10개)> 만큼 존재)
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족

	example :

	function grandmother_OnTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)

		local TableItem =
		{
			{ 1001, 10 },		-- 아이템 인덱스 (1001), 아이템 개수 (10)
			{ 1002, 1 },		-- 아이템 인덱스 (1002), 아이템 개수 (1)
			{ 1003, 5 },		-- 아이템 인덱스 (1003), 아이템 개수 (5)
		};

		if (api_user_CheckInvenForAddItemList(userObjID, TableParam) == 1) then
			-- 성공
		else
			-- 실패
		end
	end
	*/
	int api_user_CheckInvenForAddItemList(CDNUserBase* pUser, lua_tinker::table ItemTable);
	
	/*
	desc   : 유저에게 아이템을 지급한다.
	param  : 유저, 아이템인덱스, 아이템 카운트, 퀘스트 아이디
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족
	*/
	int api_user_AddItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt, int nQuestID);

	/*
	desc   : 유저에게서 아이템을 지운다.
	param  : 유저, 아이템인덱스, 아이템 카운트, 퀘스트 아이디
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족
	*/
	int api_user_DelItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt, int nQuestID);

	/*
	desc   : 유저에게서 아이템을 모두 지운다.
	param  : 유저, 아이템인덱스, 아이템 카운트, 퀘스트 아이디
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음
	*/
	int api_user_AllDelItem(CDNUserBase* pUser, int nItemIndex);

	/*
	desc   : 유저가 아이템을 가지고 있는지 확인한다.
	param  : 유저, 아이템인덱스, 아이템 카운트
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
	*/
	int api_user_HasItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);

	/*
	desc   : 유저의 클래스 ID 를 얻는다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 그외는 ClasssID
	*/
	int api_user_GetUserClassID(CDNUserBase* pUser);

	/*
	desc   : 유저의 Job ID 를 얻는다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 그외는 Job ID
	*/
	int api_user_GetUserJobID(CDNUserBase* pUser);

	/*
	desc   : 유저의 level을 얻는다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 그외는 Level
	*/
	int api_user_GetUserLevel(CDNUserBase* pUser);

	/*
	desc   : 유저 인벤토리의 빈칸수를 얻는다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 그외는 빈칸수
	*/
	int api_user_GetUserInvenBlankCount(CDNUserBase* pUser);

	/*
	desc   : 유저 퀘스트 인벤토리의 빈칸수를 얻는다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 그외는 빈칸수
	*/
	int api_quest_GetUserQuestInvenBlankCount(CDNUserBase* pUser);
	
	/*
	desc   : 유저에게 동영상을 재생 시킨다.
	param  : 유저, 동영상파일이름
	return : -1 => 유저를 찾을수 없음,  1이면 성공
	*/
	int api_user_PlayCutScene(CDNUserBase* pUser, UINT nNpcObjectID, int nCutSceneTableID, bool bIgnoreFadeIn);

	/*
	desc   : 유저에게 창고를 열라고 알린다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음,  1이면 성공
	*/
	int api_ui_OpenWareHouse(CDNUserBase* pUser, int iItemID=0 );

#if defined (PRE_MOD_GAMESERVERSHOP)
	/*
	desc   : 유저에게 상점를 열라고 알린다.
	param  : 유저인덱스, 상점 ID
	return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
	*/
	int api_ui_OpenShop(CDNUserBase* pUser, int nShopID, Shop::Type::eCode Type);

	/*
	desc   : 유저에게 스킬 상점를 열라고 알린다.
	param  : 유저인덱스, 스킬 상점 ID
	return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
	*/
	int api_ui_OpenSkillShop(CDNUserBase* pUser);
#endif
	
	/*
	desc   : 유저에게 문장보옥 합성 창을 열라고 한다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenCompoundEmblem(CDNUserBase* pUser);
	
	/*
	desc   : 유저에게 보옥 업그레이드 창을 열라고 알린다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenUpgradeJewel(CDNUserBase* pUser);

	/*
	desc   : 유저에게 우편함을 열라고 시킨다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenMailBox(CDNUserBase* pUser);

	/*
	desc   : 유저에게 문장 해제를 위해 문장차을 열라고 시킨다.
	param  : 유저인덱스
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenGlyphLift(CDNUserBase* pUser);
	
	/*
	desc   : 유저에게 분해창을 열라고 시킨다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenDisjointItem(CDNUserBase* pUser);
	
	/*
	desc   : 유저에게 아이템 합성창을 열라고 시킨다.
	param  : 유저, 아이템 합성 상점 ID
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenCompoundItem(CDNUserBase* pUser, int nCompoundShopID);
	
	/*
	desc   : 유저에게 캐쉬 아이템 상점을 열라고 시킨다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenCashShop(CDNUserBase* pUser);

	/*
	desc   : 유저에게 길드 관리 UI 대화상자를 열라고 시킨다.
	param  : 유저, 길드관리번호
	return : -1 => 유저를 찾을수 없음, 1이면 성공

	길드관리번호>
		0 : 길드창설
		1 : 길드해산
		2 : 길드탈퇴
		3 : 길드포인트
		4 : 길드레벨업
	*/
	int api_ui_OpenGuildMgrBox(CDNUserBase* pUser, int nGuildMgrNo);
	
	/*
	desc   : 일본 가챠폰 다이얼로그를 연다.
	param  : 사용될 가챠폰 인덱스
	return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공, -3 이면 디파인에 걸러짐..
	*/
	int api_ui_OpenGacha_JP(CDNUserBase* pUser, int nGachaShopID);
	
	/*
	desc   : NPC 선물하기 다이얼로그를 연다.
	param  : 대화중인 Npc ID 
	return : -1 => 유저를 찾을수 없음. 1 이면 성공.
	*/
	int api_ui_OpenGiveNpcPresent(CDNUserBase* pUser, int nNpcID);

	/*
	desc	: 기부 창을 연다.
	param	: 
	return	: -1 => 유저를 찾을수 없음. 1이면 성공.
	*/
	int api_ui_OpenDonation(CDNUserBase* pUser);


	/*
	desc	: 인벤토리 창을 연다.
	param	: 
	return	: -1 => 유저를 찾을수 없음. 1이면 성공.
	*/
	int api_ui_OpenInventory(CDNUserBase* pUser);

#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
	/*
	desc   : 텍스쳐 다이얼로그를 연다
	param  : 유저인덱스, 다이얼로그 데이터
	return : -1 => 유저를 찾을수 없음. 1이면 성공.
	*/
	int api_Open_Texture_Dialog(CDNUserBase* pUser, SCOpenTextureDialog data);


	/*
	desc   : 텍스쳐 다이얼로그를 닫는다
	param  : 유저인덱스, 다이얼로그 데이터
	return : -1 => 유저를 찾을수 없음. 1이면 성공.
	*/
	int api_Close_Texture_Dialog(CDNUserBase* pUser, int nDlgID);
#endif

	

	/*
	desc   : 유저에게 메세지를 출력하게 한다.
	param  : nType 은 메세지 출력 타입, 
			CHATTYPE_NORMAL   => 1,	// 일반적인 대화
			CHATTYPE_PARTY    => 2, // 파티 대화
			CHATTYPE_PRIVATE  => 3,	// 귓속말
			CHATTYPE_GUILD    => 4,	// 길드 대화
			CHATTYPE_CHANNEL  => 5,	// 거래/채널 대화
			CHATTYPE_SYSTEM   => 6, // 시스템 메세지
			nBaseStringIdx 는 기본 스트링
			ParamTable 은 각 키워드의 치환 정보가 담겨있는 루아 테이블
	return : -1 => 유저를 찾을수 없음, -2 타입이 없음 -3 메세지가 너무 김

	example :

	{%Hitter} 가 {item:2}를 장착한채 {%Target}를 때렸습니다.  <-- uistring 6000번
	"히터_사람"	 <-- uistring 6001번
	"타겟_사마귀"<-- uistring 6002번

	function grandmother_OnTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)

		
		local TableParam =
		{
			// 아래와 같이 {%Hitter} 라는 키워드는 uistring.xml 의 6001번으로 치환 된다. 
			{ "{%Hitter}", 1, 6001 },
			{ "{%Target}", 1, 6002 },
		};

		// 만약  { "{%Hitter}", 2, 6001 }, 라면 6001 이란 정수가 문자열 "6001" 로 되어 치환된다.

		api_UserMessage(userObjID, 1, 6000, TableParam);
	end
	*/
	int api_user_UserMessage(CDNUserBase* pUser, int nType, int nBaseStringIdx, lua_tinker::table ParamTable);
	
	/*
	desc   : 유저에게 심볼아이템을 추가한다.
	param  : 유저,  심볼아이템아이디, 갯수
	return : -1 => 유저를 찾을수 없음, -2면 실패 1이면 성공
	*/
	int api_quest_AddSymbolItem(CDNUserBase* pUser, int nItemID, short wCount);

	/*
	desc   : 유저에게 심볼아이템을 삭제한다.
	param  : 유저,  심볼아이템아이디, 갯수
	return : -1 => 유저를 찾을수 없음, -2 면 실패, 1이면 성공
	*/
	int api_quest_DelSymbolItem(CDNUserBase* pUser, int nItemID, short wCount);

	/*
	desc   : 유저가 심볼 아이템을 가지고 있는지 확인한다.
	param  : 유저, 아이템인덱스, 아이템 카운트
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
	*/
	int api_quest_HasSymbolItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);
	
	/*
	desc   : 유저에게 퀘스트 아이템을 넣을만한 인벤여유가 있는지 확인한다.
	param  : 유저, 퀘스트 아이템 인덱스, 퀘스트 아이템 카운트
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족
	*/
	int api_quest_CheckQuestInvenForAddItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);
	
	/*
	desc   : 유저에게 퀘스트 아이템 목록을 넣을만한 인벤여유가 있는지 확인한다.
	param  : 유저, 퀘스트 아이템 목록 (루아테이블, [<최대개수>]{퀘스트 아이템인덱스, 퀘스트 아이템 카운트} 가 한 쌍으로 <최대개수 (현재 10개)> 만큼 존재)
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족

	example :

	function grandmother_OnTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)

		local TableItem =
		{
			{ 1001, 10 },		-- 아이템 인덱스 (1001), 아이템 개수 (10)
			{ 1002, 1 },		-- 아이템 인덱스 (1002), 아이템 개수 (1)
			{ 1003, 5 },		-- 아이템 인덱스 (1003), 아이템 개수 (5)
		};

		if (api_quest_CheckQuestInvenForAddItemList(userObjID, TableParam) == 1) then
			-- 성공
		else
			-- 실패
		end
	end
	*/
	int api_quest_CheckQuestInvenForAddItemList(CDNUserBase* pUser, lua_tinker::table ItemTable);

	/*
	desc   : 유저에게 퀘스트 아이템을 추가한다.
	param  : 유저,  퀘스트아이템아이디, 갯수, 퀘스트 아이디
	return : -1 => 유저를 찾을수 없음, -2면 실패 1이면 성공
	*/
	int api_quest_AddQuestItem(CDNUserBase* pUser, int nItemID, short wCount, int nQuestID);

	/*
	desc   : 유저에게 퀘스트 아이템을 삭제한다.
	param  : 유저,  퀘스트아이템아이디, 갯수, 퀘스트 아이디
	return : -1 => 유저를 찾을수 없음, -2 면 실패, 1이면 성공
	*/
	int api_quest_DelQuestItem(CDNUserBase* pUser, int nItemID, short wCount, int nQuestID);

	/*
	desc   : 유저에게 퀘스트 아이템을 모두 삭제한다.
	param  : 유저,  퀘스트아이템아이디
	return : -1 => 유저를 찾을수 없음, -2 면 실패, 1이면 성공
	*/
	int api_quest_AllDelQuestItem(CDNUserBase* pUser, int nItemID);

	/*
	desc   : 유저가 퀘스트 아이템을 가지고 있는지 확인한다.
	param  : 유저, 아이템인덱스, 아이템 카운트
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
	*/
	int api_quest_HasQuestItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);
	
	/*
	desc   : 유저에게 길드 생성창을 띄우라고 알린다.
	param  : 유저인덱스
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenGuildCreate(CDNUserBase* pUser);

	/*
	desc   : 유저에게 퀘스트 보상창을 띄우라고 알린다.
	param  : 유저인덱스, 보상테이블, bActivate 가 true 면 고를수 있고 이면 그냥 보는 용도는 false로 쓴다.
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenQuestReward(CDNUserBase* pUser, int nRewardTableIndex, bool bActivate);
	
	/*
	desc   : 유저에게 퀘스트 보상을 한다. 
	param  : 유저, 보상테이블, 퀘스트 아이디, 보상체크번호
	return : -1 => 유저를 찾을수 없음, -2 유저아이템객체 오류, -3 인벤 부족, -4 이면 테이블을 찾을수 없음, -5 보상 조건 체크 오류, 1 이면 성공
	*/
	int api_quest_RewardQuestUser(CDNUserBase* pUser, int nRewardTableIndext, int nQuestID, int nRewardCheck);

	/*
	desc   : 해당 유저의 현재 맵번호를 구함
	param  : 유저
	return : 맵번호 (1 이상), 그 외는 실패 ? (0 이하)
	*/
	int api_user_GetMapIndex(CDNUserBase* pUser);
	
	/*
	desc   : 해당 유저의 최근 스테이지 완료 랭크를 구함
	param  : 유저인덱스
	return : 스테이지 완료 랭크 (SSS : 0 / SS : 1 / S : 2 / A : 3 / B : 4 / C : 5 / D : 6), 그 외는 실패 ? (기본 -1)
	*/
	int api_user_GetLastStageClearRank(CDNUserBase* pUser);
	
	/*
	desc   : 유저의 현재 소지금을 확인한다.
	param  : 유저, 체크할 값
	return : -1 => 유저를 찾을 수 없음, -2 => 소지금이 부족함, 1 => 소지금이 충분함
	*/
	int api_user_EnoughCoin(CDNUserBase* pUser, int nCoin);
	
	/*
	desc   : 유저의 현재 소지금을 확인한다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 0 이상 => 현재 소지금
	*/
	int api_user_GetCoin(CDNUserBase* pUser);
	
	/*
	desc   : 유저의 특정 미션 취득 여부를 반환
	param  : 유저, 미션인덱스 (MISSIONMAX (현재 5000 → 0 ~ 4999))
	return : -1 => 유저를 찾을수 없음, 0 => 해당 미션 없음, 1 포함 이외 => 해당 미션 존재
	*/
	int api_user_IsMissionGained(CDNUserBase* pUser, int nMissionIndex);
	
	/*
	desc   : 유저의 특정 미션 완료 여부를 반환
	param  : 유저, 미션인덱스 (MISSIONMAX (현재 5000 → 0 ~ 4999))
	return : -1 => 유저를 찾을수 없음, 0 => 해당 미션 없음, 1 포함 이외 => 해당 미션 존재
	*/
	int api_user_IsMissionAchieved(CDNUserBase* pUser, int nMissionIndex);
	
	/*
	desc   : 유저가 창고에 아이템을 가지고 있는지 확인한다.
	param  : 유저, 아이템인덱스, 아이템 카운트
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
	*/
	int api_user_HasItemWarehouse(CDNUserBase* pUser, int nItemIndex, int nItemCnt);

	/*
	desc   : 유저가 장비에 아이템을 가지고 있는지 확인한다.
	param  : 유저, 아이템인덱스
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, 1 포함 그외 true
	*/
	int api_user_HasItemEquip(CDNUserBase* pUser, int nItemIndex);
	
	/*
	desc   : 유저의 현재 길드 직급을 반환
	param  : 유저
	return : 0 : 길드 없음 / 1 : 길드장 / 2 : 부길드장 / 3 : 선임길드원 / 4 : 일반길드원 / 5 : 신입길드원
	*/
	int api_guild_GetGuildMemberRole(CDNUserBase* pUser);
	
	/*
	desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스에 값을 1 증가시킴
	param  : 유저, 퀘스트번호, 메모인덱스
	return : 증가된 퀘스트 메모값 (에러일 경우에는 LONG_MIN 반환)
	*/
	int api_quest_IncQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex);

	/*
	desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스에 값을 1 감소시킴
	param  : 유저, 퀘스트번호, 메모인덱스
	return : 증가된 퀘스트 메모값 (에러일 경우에는 LONG_MAX 반환)
	*/
	int api_quest_DecQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex);
	
	/*
	desc   : 유저에게 특정 전직 직업을 지정
	param  : 유저, 변경 전직 직업
	return : -1 : 유저를 찾을수 없음 / -2 : 없는 전직 번호임 / -3 : 전직 불가 조건임 / 0 : 이미 같은 전직 직업임 / 1 : 전직 직업 변경 성공
	*/
	int api_user_SetUserJobID(CDNUserBase* pUser, int nJobID);
	
	/*
	desc   : 유저의 전직 직업이 특정 전직 계열에 있는지 확인
	param  : 유저, 선행 전직 직업
	return : -1 : 유저를 찾을수 없음 / 0 : 같은 계열이 아님 / 1 : 같은 계열이 맞음
	*/
	int api_user_IsJobInSameLine(CDNUserBase* pUser, int nBaseJobID);
	
	/*
	desc   : 유저가 캐쉬 아이템을 가지고 있는지 확인한다.
	param  : 유저, 캐쉬아이템인덱스, 아이템 카운트
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
	*/
	int api_user_HasCashItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);

	/*
	desc   : 유저가 장비에 캐쉬 아이템을 가지고 있는지 확인한다.
	param  : 유저, 캐쉬아이템인덱스
	return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, 1 포함 그외 true
	*/
	int api_user_HasCashItemEquip(CDNUserBase* pUser, int nItemIndex);
	
	/*
	desc   : 특정 카운팅 타입, ID 에 대해서 임의로 카운팅을 증가시킨다.
	param  : 유저, 
			카운팅할 타입 (CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3),
			카운팅할 것의 인덱스 (아이템 ID, 몬스터 ID 등등)
	return : -1 => 유저를 찾을수 없음, 1 => 성공
	*/
	int api_quest_IncCounting(CDNUserBase* pUser, int nCountingType, int nCountingIndex);
	
	/*
	desc   : 유저가 진행 가능 최대 퀘스트 수에 도달하였는지 여부를 반환한다.
	param  : 유저, 퀘스트인덱스
	return : -1 : 유저를 찾을수 없음, 1 : 최대 퀘스트 수에 도달 함, 그외 : 아님
	*/
	int api_quest_IsPlayingQuestMaximum(CDNUserBase* pUser);
	
	/*
	desc   : 특정 퀘스트를 강제 완료 시킨다. (진행 중이 아닌 퀘스트는 추가하여 완료)
	param  : 유저, 퀘스트인덱스, 퀘스트 코드 (0:일반 / 1:시간제), 마킹여부 (1:마킹/0:안함), 삭제여부 (1:삭제/0:않함), 반복여부 (1:반복/0:안함)
	return : -1 : 유저를 찾을수 없음, 1 : 최대 퀘스트 수에 도달 함, 그외 : 아님
	*/
	int api_quest_ForceCompleteQuest(CDNUserBase* pUser, int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat);
	
	/*
	desc   : NPC 호의Point 를 얻어온다.
	param  : 유저, NpcID
	return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
	*/
	int api_npc_GetFavorPoint(CDNUserBase* pUser, int iNpcID);
	
	/*
	desc   : NPC 악의Point 를 얻어온다.
	param  : 유저, NpcID
	return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
	*/
	int api_npc_GetMalicePoint(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : NPC 호의Percent 를 얻어온다.
	param  : 유저, NpcID
	return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
	*/
	int api_npc_GetFavorPercent(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : NPC 악의Percent 를 얻어온다.
	param  : 유저, NpcID
	return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
	*/
	int api_npc_GetMalicePercent(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : NPC 호의Point 를 가감한다.
	param  : 유저, NpcID, 가감값(- 넣으면 감소함)
	return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
	*/
	int api_npc_AddFavorPoint(CDNUserBase* pUser, int iNpcID, int val);

	/*
	desc   : NPC 악의Point 를 가감한다.
	param  : 유저, NpcID, 가감값(- 넣으면 감소함)
	return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
	*/
	int api_npc_AddMalicePoint(CDNUserBase* pUser, int iNpcID, int val);

	/*
	desc   : NPC 에게 보낼 선물 선택한 정보를 서버로 보내라고 클라이언트에게 요청.
	param  : 유저, NpcID
	return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
	*/
	int api_npc_SendSelectedPresent(CDNUserBase* pUser, int iNpcID);
	
	/*
	desc   : NPC 에게 보낼 선물 선택한 정보를 서버로 보내라고 클라이언트에게 요청.
	param  : 유저, NpcID
	return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
	*/
	int api_npc_Rage(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : NPC 에게 보낼 선물 선택한 정보를 서버로 보내라고 클라이언트에게 요청.
	param  : 유저, NpcID
	return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
	*/
	int api_npc_Disappoint(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : 특정 사용자의 전체 스킬을 리셋시킨다.
	param  : 유저
	return : -1 : 유저를 찾을수 없음, 1 : 성공
	*/
	int api_user_ResetSkill(CDNUserBase* pUser);

	/*
	desc   : 유저가 가진 장비 아이템을 수리하고, 수리비용을 차감합니다.
	param  : 유저, 전체수리 유무(1-전체수리, 0-장착한아이템)
	return : -1 : 유저를 찾을수 없음 / 0 : 수리할 아이템 없음 / 1 : 소지금 부족 / 2 : 아이템 수리 완료
	*/
	int api_user_RepairItem(CDNUserBase* pUser, int iAllRepair);

	/*
	desc   : 유저에게 강화이동창을 열라고 시킨다.
	param  : 유저
	return : -1 => 유저를 찾을수 없음, 1이면 성공
	*/
	int api_ui_OpenExchangeEnchant(CDNUserBase* pUser);

	/*
	desc   : 해당 맵이 닫혀있는지 확인한다.
	param  : 유저, 맵 인덱스
	return : 1 이면 오픈, 0 이면 클로즈
	*/
	int api_env_CheckCloseGateByTime(int iMapID);

	/*
	desc   : 원격 퀘스트 완료 스텝을 등록 한다
	param  : 퀘스트인덱스, 스텝 인덱스
	*/
	void api_quest_SetRemoteComplete (int nQuestID, int nStep);

	/*
	desc   : 월드버프 줄수 있는지 체크
	param  : 버프 아이템아이디
	*/
	int api_quest_WorldBuffCheck(int nItemID);

	/*
	desc   : 월드버프를 줌
	param  : 버프 아이템아이디
	*/
	int api_quest_ApplyWorldBuff(CDNUserBase* pUser, int nItemID, int nMapIdx);


	//rlkt_test
	int api_Open_ChangeJobDialog(CDNUserBase* pUser);
};
