#pragma once

#include "DnLuaAPIDefine.h"
// GameServer Script API
//  [4/1/2008 nextome]

class CEtTriggerElement;
class CDNGameRoom;
class CDNUserBase;

int CheckInvenForQuestReward(CDNUserBase* pUser, const TQuestReward& QuestReward, const bool bSelectedArray[]);

// 모든 함수를 루아에 등록한다.
void DefAllAPIFunc(lua_State* pLuaState);

/*
desc   : 다음 대사로 연결한다.
param  : 유저인덱스, NPC인덱스, 토크인덱스, 토크파일이름
return : void
*/
void api_npc_NextTalk(CDNGameRoom *pRoom, UINT nUserSessionID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

/*
desc   : 다음 스크립트로 연결한다.
param  : 유저인덱스, NPC인덱스, 토크인덱스, 토크파일이름
return : void
*/
void api_npc_NextScript(CDNGameRoom *pRoom, UINT nUserSessionID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);


/*
desc   : 대사 파라메터를 세팅한다(스트링).  
param  : 파라메터키, 파라메터문자의 인덱스
return : -1 유저를 찾을수 없음, -2 파라메터 갯수가 넘어서서 더이상 세팅할수 없음. 1 이면 성공
sample : 
저는 {%color} 색을 좋아합니다.  라는 문장을 만들때.
api_npc_SetParamString("{%color}, math.random(2) );  이런식으로 만들어준다.
1은 TalkParamTable.xls 에 1 : 빨강, 2: 파랑 따로 기록 되어있어야한다.

주의: api_npc_SetParamString 으로 세팅한 파라메터는 api_npc_NextTalk 호출후 clear 되므로 api_npc_NextTalk 호출전에 세팅한다.
*/
int api_npc_SetParamString(CDNGameRoom *pRoom, UINT nUserObjectID, const char* szParamKey, int nParamID);

/*
desc   : 대사 파라메터를 세팅한다(정수).  
param  : 파라메터키, 정수값
return : -1 유저를 찾을수 없음, -2 파라메터 갯수가 넘어서서 더이상 세팅할수 없음. 1 이면 성공
sample : 
저는 {%age} 살쯤에 숨지고 싶어요.  라는 문장을 만들때.
api_npc_SetParamInt("{%age}, math.random(30)+40 );  이런식으로 만들어준다.

주의: api_npc_SetParamInt 으로 세팅한 파라메터는 api_npc_NextTalk 호출후 clear 되므로 api_npc_NextTalk 호출전에 세팅한다.
*/
int api_npc_SetParamInt(CDNGameRoom *pRoom, UINT nUserObjectID, const char* szParamKey, int nValue);


/*
desc   : 로그를 남긴다.
param  : 로그 스트링 줄바꿈을 위해 \n 을 포함할것.
return : void
*/
void api_log_AddLog(CDNGameRoom *pRoom, const char* szLog);

/*
desc   : 유저에게 로그를 남긴다. 유저채팅메세지로 전송해줌.
param  : 로그 스트링 줄바꿈을 위해 \n 을 포함할것.
return : void
*/
void api_log_UserLog(CDNGameRoom *pRoom, UINT nUserObjectID, const char* szLog);


/*
desc   : 유저에게 퀘스트 관련 정보를 채팅으로 보내준다.
param  : 
return : void
*/
void api_quest_DumpQuest(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저에게 퀘스트를 부여 한다.
param  : 유저인덱스, 퀘스트인덱스, 퀘스트 타입 QuestType_SubQuest = 1, QuestType_MainQuest = 2,
return : 1 : 성공, -1 : 유저를 찾을수 없음, -2 : 유저가 퀘스트를 가지고 있지 않음
*/
int api_quest_AddHuntingQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt);
int api_quest_AddQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nQuestType);

/*
desc   : 유저에게 퀘스트를 완료 시킨다.
param  : 유저인덱스, 퀘스트인덱스, 현재 가지고 있는 퀘스트에서 지울것인지 (안지우면 플레이 횟수만 증가 시킨다.)
return : -1 유저를 찾을수 없음, -2 퀘스트가 진행중이 아님 
*/
int api_quest_CompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, bool bDelPlayList);

/*
desc   : 유저에게 퀘스트를 완료목록에 추가 시킨다. ( DB 에 기록!! ) 
param  : 유저인덱스, 퀘스트인덱스
return : -1 유저를 찾을수 없음, -2 퀘스트가 진행중이 아님 
*/
int api_quest_MarkingCompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);


/*
desc   : 완료목록에 완료마킹이 되어있는 퀘스트인지?
param  : 유저인덱스, 퀘스트인덱스
return : -1 유저를 찾을수 없음, 1 이면 완료마킹된 퀘스트, 0 이면 완료마킹 안됨
*/
int api_quest_IsMarkingCompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);




/*
desc   : 유저가 퀘스트를 가지고 있는지 확인한다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 유저를 찾을수 없음, -2 유저가 퀘스트를 가지고 있지 않음, -1 보다 크면 퀘스트를 가지고 잇음 ( 퀘스트 스텝을 리턴 )
*/
int api_quest_UserHasQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : 유저가 몇개의 퀘스트를 진행중인지 확인한다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 유저를 찾을수 없음, 그외에 현재 진행중인 퀘스트 갯수 리턴..
*/
int api_quest_GetPlayingQuestCnt(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : npc 인덱스를 리턴한다.
param  : 유저인덱스, NpcObjId ( npc 인덱스와 npcobject 인덱스는 다른 것. )
return : -1 유저를 찾을수 없음, -2 npc 를 찾을수 없음, 그외에는 npc인덱스
*/
int api_npc_GetNpcIndex(CDNGameRoom *pRoom, UINT nUserObjectID, UINT nNpcObjID);

/*
desc   : 유저가 가지고 있는 퀘스트의 스텝, 저널 스텝을 세팅한다.
param  : 유저인덱스, 퀘스트인덱스, 저널 스텝
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음.  -3 저널 스텝 범위를 벗어남. 성공일 경우 1을 리턴
*/
int api_quest_SetQuestStepAndJournalStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, short nQuestStep, int nJournalStep);

/*
desc   : 유저가 가지고 있는 퀘스트의 퀘스트 스텝을 세팅한다.
param  : 유저인덱스, 퀘스트인덱스, 퀘스트 스텝
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음. 성공일 경우 1을 리턴
*/
int api_quest_SetQuestStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, short nQuestStep);

/*
desc   : 유저가 가지고 있는 퀘스트의 퀘스트 스텝을 얻는다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음., -1 보다 큰경우 현재 퀘스트의 스텝
*/
int api_quest_GetQuestStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : 유저가 가지고 있는 퀘스트의 저널 스텝을 세팅한다.
param  : 유저인덱스, 퀘스트인덱스, 저널 스텝
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음.  -3 저널 스텝 범위를 벗어남. 성공일 경우 1을 리턴
*/
int api_quest_SetJournalStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nJournalStep);

/*
desc   : 유저가 가지고 있는 퀘스트의 저널 스텝을 얻는다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음., -1 보다 큰경우 현재 저널 스텝
*/
int api_quest_GetJournalStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스에 값을 세팅한다.
param  : 유저인덱스, 메모인덱스, 메모값
return : -1 => 유저를 찾을수 없음. -2 => 유저가 퀘스트를 가지고 있지 않음. -3 메모 인덱스 범위를 벗어남. 성공일 경우 1을 리턴
*/
int api_quest_SetQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex, int iVal);

/*
desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스의 값을 얻는다.
param  : 유저인덱스, 메모인덱스
return : -1 => 유저를 찾을수 없음. -2 => 유저가 퀘스트를 가지고 있지 않음. -3 메모 인덱스 범위를 벗어남. -1 보다 큰경우 특정 메모 인덱스의 값
*/
int api_quest_GetQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : 몬스터 잡거나, 아이템 얻는 갯수 카운팅 정보를 세팅하는 함수 이다.
param  : 유저인덱스, 
		 퀘스트인덱스, 
		 카운팅정보 세팅 슬롯(0부터 4까지 5개의 세팅슬롯이 있음) , 
		 카운팅할 타입(	CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3, CountingType_SymbolItem = 4),
		 카운팅할 것의 인덱스, 
		 카운팅할 갯수 
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음. -3 => 카운팅정보세팅슬롯 범위 초과 -4 => 카운팅타입 or 타겟카운트 범위 초과

example: 

api_quest_SetCountingInfo(유저아이디, 퀘스트아이디, 0, CountingType_MyMonsterKill, 10001, 10);
api_quest_SetCountingInfo(유저아이디, 퀘스트아이디, 1, CountingType_MyMonsterKill, 10002,  5);
api_quest_SetCountingInfo(유저아이디, 퀘스트아이디, 2, CountingType_MyMonsterKill, 10003, 20);
api_quest_SetCountingInfo(유저아이디, 퀘스트아이디, 3, Item,	   1001,  5);
api_quest_SetCountingInfo(유저아이디, 퀘스트아이디, 4, Item,     1002,  1);

위와 같이 스크립트에서 호출 해줬을 경우 5개의 카운팅 조건이 세팅 되게 된다. 
맨 윗줄의 뜻은 0번 슬롯에 10001번 몬스터를 10마리 잡으면 조건이 만족한다고 세팅한다.

위와같이 5개의 슬롯에 10001번 몬스터 10마리, 10002번 몬스터 5마리, 10003번 몬스터 20마리, 1001번 아이템 5개, 1002번 아이템 1개 라고 세팅 하였고
위 5개의 조건을 만족하게 되면. 퀘스트 스크립트의 OnComplete() 함수를 호출하게 된다.

*** 카운팅 갯수는 최대 30000 개이다.
*/
int api_quest_SetCountingInfo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID,  int nSlot, int nCountingType, int nCountingIndex, int nTargetCnt); 


/*
desc   : 세팅된 카운팅 정보를 클리어 한다. 
param  : 유저인덱스, 퀘스트 인덱스
return : -1 => 유저를 찾을수 없음 -2 => 퀘스트를 찾을수 없음,
*/
int api_quest_ClearCountingInfo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : 세팅된 카운팅들이 모두 완료 되었는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 퀘스트를 찾을수 없음,  -3이면 완료 안됨, 1 이면 모두 완료
*/
int api_quest_IsAllCompleteCounting(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : 유저에게 아이템을 넣을만한 인벤여유가 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족
*/
int api_user_CheckInvenForAddItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

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
int api_user_CheckInvenForAddItemList(CDNGameRoom *pRoom, UINT nUserObjectID, lua_tinker::table ItemTable);

/*
desc   : 유저에게 아이템을 지급한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트, 퀘스트 아이디
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족
*/
int api_user_AddItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID);

/*
desc   : 유저에게서 아이템을 지운다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트, 퀘스트 아이디
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족
*/
int api_user_DelItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID);

/*
desc   : 유저에게서 아이템을 모두 지운다.
param  : 유저인덱스, 아이템인덱스
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음
*/
int api_user_AllDelItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex);

/*
desc   : 유저가 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_user_HasItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);


/*
desc   : 유저의 클래스 ID 를 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 ClasssID
*/
int api_user_GetUserClassID(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : 유저의 Job ID 를 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 Job ID
*/
int api_user_GetUserJobID(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : 유저의 level을 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 Level
*/
int api_user_GetUserLevel(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저 인벤토리의 빈칸수를 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 빈칸수
*/
int api_user_GetUserInvenBlankCount(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저 퀘스트 인벤토리의 빈칸수를 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 빈칸수
*/
int api_quest_GetUserQuestInvenBlankCount(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저에게 경험치를 추가한다.
param  : 유저인덱스, 경험치
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_user_AddExp(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nAddExp);

/*
desc   : 유저에게 돈을 추가한다.
param  : 유저인덱스, 돈
return : -1 => 유저를 찾을수 없음,  1이면 성공
*/
int api_user_AddCoin(CDNGameRoom *pRoom, UINT nUserObjectID, int nAddCoin, int nQuestID);


/*
desc   : 유저에게 동영상을 재생 시킨다.
param  : 유저인덱스, 컷씬테이블ID
return : -1 => 유저를 찾을수 없음,  1이면 성공
*/
int api_user_PlayCutScene(CDNGameRoom *pRoom, UINT nUserObjectID, UINT nNpcObjectID, int nCutSceneTableID, bool bIgnoreFadeIn);


/*
desc   : 유저에게 창고를 열라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음,  1이면 성공
*/
int api_ui_OpenWareHouse(CDNGameRoom *pRoom, UINT nUserObjectID, int iItemID=0 );

/*
desc   : 유저에게 상점를 열라고 알린다.
param  : 유저인덱스, 상점 ID
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenShop(CDNGameRoom *pRoom, UINT nUserObjectID, int nShopID, Shop::Type::eCode Type = Shop::Type::Normal);

/*
desc   : 유저에게 스킬 상점를 열라고 알린다.
param  : 유저인덱스, 스킬 상점 ID
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenSkillShop(CDNGameRoom *pRoom, UINT nUserObjectID, int nSkillShopID);


/*
desc   : 유저에게 무인상점을 열라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenMarket(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저에게 문장보옥 합성 창을 열라고 한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenCompoundEmblem(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : 유저에게 보옥 업그레이드 창을 열라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenUpgradeJewel(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : 유저에게 우편함을 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenMailBox(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : 유저에게 분해창을 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenDisjointItem(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저에게 아이템 합성창을 열라고 시킨다.
param  : 유저인덱스, 아이템 합성 상점 ID
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenCompoundItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nCompoundShopID);

int api_ui_OpenCompound2Item(CDNGameRoom *pRoom, UINT nUserObjectID, int nCompoundShopID, int iItemID/*=0*/ );

/*
desc   : 유저에게 캐쉬 아이템 상점을 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenCashShop(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저에게 길드 관리 UI 대화상자를 열라고 시킨다.
param  : 유저인덱스, 길드관리번호
return : -1 => 유저를 찾을수 없음, 1이면 성공

길드관리번호>
	0 : 길드창설
	1 : 길드해산
	2 : 길드탈퇴
	3 : 길드포인트
	4 : 길드레벨업
*/
int api_ui_OpenGuildMgrBox(CDNGameRoom *pRoom, UINT nUserObjectID, int nGuildMgrNo);

/*
desc   : 일본 가챠폰 다이얼로그를 연다.
param  : 사용될 가챠폰 인덱스
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공, -3 이면 디파인에 걸러짐..
*/
int api_ui_OpenGacha_JP(CDNGameRoom *pRoom,  UINT nUserObjectID, int nGachaShopID);

/*
desc   : NPC 선물하기 다이얼로그를 연다.
param  : 대화중인 Npc ID 
return : -1 => 유저를 찾을수 없음. 1 이면 성공.
*/
int api_ui_OpenGiveNpcPresent(CDNGameRoom* pRoom, UINT nUserObjectID, int nNpcID);


/*
desc   : 유저에게 농장창고를 열라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음,  1이면 성공
*/
int api_ui_OpenFarmWareHouse(CDNGameRoom *pRoom, UINT nUserObjectID);


#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
/*
desc   : 텍스쳐 다이얼로그를 연다.
param  : 유저인덱스, 파일인덱스, 화면에서의 위치, 보여질 시간, 이미지 중앙점,
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenTextureDialog(CDNGameRoom* pRoom, UINT nUserObjectID, int nFileIndex, float fX, float fY, int nPos, int nTime, int nDialogIndex, bool bAutoCloseDialog);


/*
desc   : 텍스쳐 다이얼로그를 닫는다.
param  : 유저인덱스, 다이얼로그 ID
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_CloseTextureDialog(CDNGameRoom* pRoom, UINT nUserObjectID, int nDlgID);
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

로그 스트링 줄바꿈을 위해 \n 을 포함할것.
return : -1 => 유저를 찾을수 없음, -2 타입이 없음 -3 메세지가 너무 김
*/
int api_user_UserMessage(CDNGameRoom *pRoom, UINT nUserObjectID, int nType, int nBaseStringIdx, lua_tinker::table ParamTable);

/*
desc   : 유저에게 심볼아이템을 추가한다.
param  : 유저인덱스,  심볼아이템아이디, 갯수
return : -1 => 유저를 찾을수 없음, -2면 실패 1이면 성공
*/
int api_quest_AddSymbolItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount);

/*
desc   : 유저에게 심볼아이템을 삭제한다.
param  : 유저인덱스,  심볼아이템아이디, 갯수
return : -1 => 유저를 찾을수 없음, -2 면 실패, 1이면 성공
*/
int api_quest_DelSymbolItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount);

/*
desc   : 유저가 심볼 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_quest_HasSymbolItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : 유저에게 퀘스트 아이템을 넣을만한 인벤여유가 있는지 확인한다.
param  : 유저인덱스, 퀘스트 아이템 인덱스, 퀘스트 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족
*/
int api_quest_CheckQuestInvenForAddItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

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

	if (api_quest_CheckQuestInvenForAddItemList(userObjID, TableParam) == 1) then
		-- 성공
	else
		-- 실패
	end
end
*/
int api_quest_CheckQuestInvenForAddItemList(CDNGameRoom *pRoom, UINT nUserObjectID, lua_tinker::table ItemTable);

/*
desc   : 유저에게 퀘스트 아이템을 추가한다.
param  : 유저인덱스,  퀘스트아이템아이디, 갯수, 퀘스트 아이디
return : -1 => 유저를 찾을수 없음, -2면 실패 1이면 성공
*/
int api_quest_AddQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount, int nQuestID);

/*
desc   : 유저에게 퀘스트 아이템을 삭제한다.
param  : 유저인덱스,  퀘스트아이템아이디, 갯수, 퀘스트 아이디
return : -1 => 유저를 찾을수 없음, -2 면 실패, 1이면 성공
*/
int api_quest_DelQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount, int nQuestID);

/*
desc   : 유저에게 퀘스트 아이템을 모두 삭제한다.
param  : 유저인덱스,  퀘스트아이템아이디
return : -1 => 유저를 찾을수 없음, -2 면 실패, 1이면 성공
*/
int api_quest_AllDelQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID );

/*
desc   : 유저가 퀘스트 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_quest_HasQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : 유저에게 길드 생성창을 띄우라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenGuildCreate(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저에게 퀘스트 보상창을 띄우라고 알린다.
param  : 유저인덱스, 보상테이블, bActivate 가 true 면 고를수 있고 이면 그냥 보는 용도는 false로 쓴다.
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenQuestReward(CDNGameRoom *pRoom, UINT nUserObjectID, int nRewardTableIndex, bool bActivate);

/*
desc   : 유저에게 퀘스트 보상을 한다. 
param  : 유저인덱스, 보상테이블, 퀘스트 아이디, 보상체크번호
return : -1 => 유저를 찾을수 없음, -2 유저아이템객체 오류, -3 인벤 부족, -4 이면 테이블을 찾을수 없음, -5 보상 조건 체크 오류, 1 이면 성공
*/
int api_quest_RewardQuestUser(CDNGameRoom *pRoom, UINT nUserObjectID, int nRewardTableIndext, int nQuestID, int nRewardCheck);

/*
desc   : 해당 유저의 파티원수를 리턴한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 파티원수 (주의 !!!> 파티에 가입되지 않은 사용자도 1 을 반환)
*/
int api_user_GetPartymemberCount(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 해당 유저가 파티에 가입된 상태인지 체크한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 0 => 파티에 가입 않되어있음, 1 => 파티에 가입 되어있음
*/
int api_user_IsPartymember(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저를 이동시킨다.
param  : 유저인덱스, 이동할 맵 인덱스, 이동할 게이트 인덱스
return : -1 => 유저를 찾을수 없음, -2 없는 맵임
*/
void api_user_ChangeMap(CDNGameRoom *pRoom, UINT nUserObjectID, int nMapIndex, int nGateNo);

/*
desc   : 해당 유저의 스테이지 난이도를 구함
param  : 유저인덱스
return : 0 ~ 4 => 던전 난이도 (0:쉬움 / 1:보통 / 2:어려움 / 3:마스터 : 4:어비스), 그 외는 실패 (기본 -1)
*/
int api_user_GetStageConstructionLevel(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 해당 유저의 현재 맵번호를 구함
param  : 유저인덱스
return : 맵번호 (1 이상), 그 외는 실패 ? (0 이하)
*/
int api_user_GetMapIndex(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 해당 유저의 최근 스테이지 완료 랭크를 구함
param  : 유저인덱스
return : 스테이지 완료 랭크 (SSS : 0 / SS : 1 / S : 2 / A : 3 / B : 4 / C : 5 / D : 6), 그 외는 실패 ? (기본 -1)
*/
int api_user_GetLastStageClearRank(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저의 현재 소지금을 확인한다.
param  : 유저인덱스, 체크할 값
return : -1 => 유저를 찾을 수 없음, -2 => 소지금이 부족함, 1 => 소지금이 충분함
*/
int api_user_EnoughCoin(CDNGameRoom *pRoom, UINT nUserObjectID, int nCoin );

/*
desc   : 유저의 현재 소지금을 확인한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 0 이상 => 현재 소지금
*/
int api_user_GetCoin(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저에게 돈을 감소한다.
param  : 유저인덱스, 돈 (0 초과)
return : -1 => 유저를 찾을수 없음, 1 => 성공
*/
int api_user_DelCoin(CDNGameRoom *pRoom, UINT nUserObjectID, int nDelCoin);

/*
desc   : 유저의 특정 미션 취득 여부를 반환
param  : 유저인덱스, 미션인덱스 (MISSIONMAX (현재 5000 → 0 ~ 4999))
return : -1 => 유저를 찾을수 없음, 0 => 해당 미션 없음, 1 포함 이외 => 해당 미션 존재
*/
int api_user_IsMissionGained(CDNGameRoom *pRoom, UINT nUserObjectID, int nMissionIndex);

/*
desc   : 유저의 특정 미션 완료 여부를 반환
param  : 유저인덱스, 미션인덱스 (MISSIONMAX (현재 5000 → 0 ~ 4999))
return : -1 => 유저를 찾을수 없음, 0 => 해당 미션 없음, 1 포함 이외 => 해당 미션 존재
*/
int api_user_IsMissionAchieved(CDNGameRoom *pRoom, UINT nUserObjectID, int nMissionIndex);

/*
desc   : 유저가 창고에 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_user_HasItemWarehouse(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : 유저가 장비에 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, 1 포함 그외 true
*/
int api_user_HasItemEquip(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex);

/*
desc   : 유저의 현재 길드 직급을 반환
param  : 유저인덱스
return : 0 : 길드 없음 / 1 : 길드장 / 2 : 부길드장 / 3 : 선임길드원 / 4 : 일반길드원 / 5 : 신입길드원
*/
int api_guild_GetGuildMemberRole(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스에 값을 1 증가시킴
param  : 유저인덱스, 퀘스트번호, 메모인덱스
return : 증가된 퀘스트 메모값 (에러일 경우에는 LONG_MIN 반환)
*/
int api_quest_IncQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스에 값을 1 감소시킴
param  : 유저인덱스, 퀘스트번호, 메모인덱스
return : 증가된 퀘스트 메모값 (에러일 경우에는 LONG_MAX 반환)
*/
int api_quest_DecQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : 유저에게 특정 전직 직업을 지정
param  : 유저인덱스, 변경 전직 직업
return : -1 : 유저를 찾을수 없음 / -2 : 없는 전직 번호임 / -3 : 전직 불가 조건임 / 0 : 이미 같은 전직 직업임 / 1 : 전직 직업 변경 성공
*/
int api_user_SetUserJobID(CDNGameRoom *pRoom, UINT nUserObjectID, int nJobID);

/*
desc   : 유저의 전직 직업이 특정 전직 계열에 있는지 확인
param  : 유저인덱스, 선행 전직 직업
return : -1 : 유저를 찾을수 없음 / 0 : 같은 계열이 아님 / 1 : 같은 계열이 맞음
*/
int api_user_IsJobInSameLine(CDNGameRoom *pRoom, UINT nUserObjectID, int nBaseJobID);

/*
desc   : 유저가 캐쉬 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 캐쉬아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_user_HasCashItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : 유저가 장비에 캐쉬 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 캐쉬아이템인덱스
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, 1 포함 그외 true
*/
int api_user_HasCashItemEquip(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex);

/*
desc   : 특정 카운팅 타입, ID 에 대해서 임의로 카운팅을 증가시킨다.
param  : 유저인덱스, 
		퀘스트인덱스, 
		카운팅할 타입 (CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3),
		카운팅할 것의 인덱스 (아이템 ID, 몬스터 ID 등등)
return : -1 => 유저를 찾을수 없음, 1 => 성공
*/
int api_quest_IncCounting(CDNGameRoom *pRoom, UINT nUserObjectID, int nCountingType, int nCountingIndex);

#if defined(PRE_ADD_DAILYQUESTDATEAPI_01) && defined(PRE_ADD_QUEST_DAILYCHECK_01)
/*
desc   : 일일 퀘스트 기준으로 현재 날짜를 얻어온다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1 => 성공
*/
int api_quest_GetDailyQuestDay(CDNGameRoom *pRoom, UINT nUserObjectID);
#endif	// #if defined(PRE_ADD_DAILYQUESTDATEAPI_01) && defined(PRE_ADD_QUEST_DAILYCHECK_01)

/*
desc   : 유저가 진행 가능 최대 퀘스트 수에 도달하였는지 여부를 반환한다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 : 유저를 찾을수 없음, 1 : 최대 퀘스트 수에 도달 함, 그외 : 아님
*/
int api_quest_IsPlayingQuestMaximum(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 특정 퀘스트를 강제 완료 시킨다. (진행 중이 아닌 퀘스트는 추가하여 완료)
param  : 유저인덱스, 퀘스트인덱스, 퀘스트 코드 (0:일반 / 1:시간제), 마킹여부 (1:마킹/0:안함), 삭제여부 (1:삭제/0:않함), 반복여부 (1:반복/0:안함)
return : -1 : 유저를 찾을수 없음, 1 : 최대 퀘스트 수에 도달 함, 그외 : 아님
*/
int api_quest_ForceCompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat);

/*
desc   : 특정 사용자의 위치를 이동 시킨다.
param  : 유저인덱스, X 좌표, Y 좌표, Z 좌표
return : -1 : 유저를 찾을수 없음, 1 성공
*/
int api_user_ChangePos(CDNGameRoom *pRoom, UINT nUserObjectID, int nX, int nY, int nZ, int nLookX = 0, int nLookY = 0);

int api_ui_OpenUpgradeItem(CDNGameRoom *pRoom, UINT nUserObjectID,int iItemID=0);

int api_user_RequestEnterPVP(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : NPC 호의Point 를 얻어온다.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_GetFavorPoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 악의Point 를 얻어온다.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_GetMalicePoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 호의Percent 를 얻어온다.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_GetFavorPercent( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 악의Percent 를 얻어온다.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_GetMalicePercent( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 호의Point 를 가감한다.
param  : 유저인덱스, NpcID, 가감값(- 넣으면 감소함)
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_AddFavorPoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID, int val );

/*
desc   : NPC 악의Point 를 가감한다.
param  : 유저인덱스, NpcID, 가감값(- 넣으면 감소함)
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_AddMalicePoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID, int val );

/*
desc   : NPC 에게 보낼 선물 선택한 정보를 서버로 보내라고 클라이언트에게 요청.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_SendSelectedPresent( CDNGameRoom* pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 에게 보낼 선물 선택한 정보를 서버로 보내라고 클라이언트에게 요청.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_Rage( CDNGameRoom* pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 에게 보낼 선물 선택한 정보를 서버로 보내라고 클라이언트에게 요청.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_Disappoint( CDNGameRoom* pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : 특정 사용자의 전체 스킬을 리셋시킨다.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음, 1 : 성공
*/
int api_user_ResetSkill(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : 특정 사용자를 2차 전직 스킬을 사용할 수 있는 상태로 임시로 만들어준다.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음, 1 : 성공
*/
int api_user_SetSecondJobSkill( CDNGameRoom *pRoom, UINT nUserObjectID, int iSecondChangeJobID );

/*
desc   : 임시로 2차 전직을 사용할 수 있는 상태로 만들어둔 유저를 원래 상태로 돌려놓는다. 
		 이 api 를 호출하지 않아도 맵을 이동해도 리셋된다.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음, 1 : 성공
*/
int api_user_ClearSecondJobSkill( CDNGameRoom *pRoom, UINT nUserObjectID );

/*
desc   : 유저가 가진 장비 아이템을 수리하고, 수리비용을 차감합니다.
param  : 유저인덱스, 전체수리 유무(1-전체수리, 0-장착한아이템)
return : -1 : 유저를 찾을수 없음 / 0 : 수리할 아이템 없음 / 1 : 소지금 부족 / 2 : 아이템 수리 완료
*/
int api_user_RepairItem(CDNGameRoom *pRoom, UINT nUserObjectID, int iAllRepair);


/*
desc   : 아이템의 강화값을 다른 아이템으로 이동시킵니다.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음, 1이면 성공 
*/
int api_ui_OpenExchangeEnchant(CDNGameRoom *pRoom, UINT nUserObjectID);

//------------------------------------------------------------------------------------------------
// trigger

int api_trigger_GetPartyCount( CDNGameRoom *pRoom );
int api_trigger_GetPartyActor( CDNGameRoom *pRoom, int nPartyIndex );
bool api_trigger_IsInsidePlayerToEventArea( CDNGameRoom *pRoom, int nActorHandle, int nEventAreaHandle );
void api_trigger_SetActionToProp( CDNGameRoom *pRoom, int nPropHandle, const char *szAction );
void api_trigger_ShowDungeonClear( CDNGameRoom *pRoom, bool bClear = true, bool bIgnoreRewardItem = false );
void api_trigger_SetPermitGate( CDNGameRoom *pRoom, int nGateIndex, int nOpenClose );
int api_trigger_GetMonsterAreaLiveCount( CDNGameRoom *pRoom, int nEventAreaHandle );
int api_trigger_GetMonsterAreaTotalCount( CDNGameRoom *pRoom, int nEventAreaHandle );
void api_trigger_RebirthMonster( CDNGameRoom *pRoom, int nEventAreaHandle );
void api_trigger_GenerationMonster( CDNGameRoom *pRoom, int nEventAreaHandle, int nMonsterTableID, int nMin, int nMax, int nTeamSetting );
void api_trigger_GenerationMonsterGroup( CDNGameRoom *pRoom, int nEventAreaHandle, int nMonsterGroupID, int nMin, int nMax, int nTeamSetting );
void api_trigger_CmdMoveMonster( CDNGameRoom* pRoom, int nEventAreaHandle, int nTargetEventAreaHandle );
void api_trigger_CmdRandMoveMonster( CDNGameRoom* pRoom, int nEventAreaHandle, int nTargetEventAreaHandle, UINT uiMoveFrontRate );
bool api_trigger_CheckMonsterIsInsideArea( CDNGameRoom* pRoom, int nEventAreaHandle, int nTargetEventAreaHandle );
bool api_trigger_CheckMonsterLessHP( CDNGameRoom* pRoom, int iMonsterID, int iHP );
bool api_trigger_CheckInsideAreaMonsterLessHP( CDNGameRoom* pRoom, int iAreaUniqueID, int iHP );
void api_trigger_GenerationMonsterSet( CDNGameRoom *pRoom, int nEventAreaHandle, int nSetID, int nPosID, int nTeamSetting, bool bResetReference );
void api_trigger_GenerationMonsterSetInEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nChangeSetID, int nChangePosID, int nTeamSetting, bool bResetReference );
/*
desc   : EventArea 에 NPC 를 생성한다.
param  : 이벤트영역 id , npc id 
return : -1 => 이벤트영역 찾을수 없음,  1이면 성공
*/
int api_trigger_GenerationNpc(CDNGameRoom *pRoom, int nEventAreaHandle, int nNpcTableID);

/*
desc   : EventArea 에 NPC 를 제거한다.
param  : 이벤트영역 id
return : -1 => 이벤트영역 찾을수 없음,  1이면 성공
*/
int api_trigger_DestroyNpc(CDNGameRoom *pRoom, int nEventAreaHandle);

void api_trigger_SetGameSpeed( CDNGameRoom *pRoom, float fPlaySpeed, DWORD dwDelay );
void api_trigger_ShowProp( CDNGameRoom *pRoom, int nPropHandle, bool bShow );
bool api_trigger_IsAllDieMonster( CDNGameRoom *pRoom, bool bCheckTeam = false );
int api_trigger_GetLastAimPropHandle( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
bool api_trigger_IsProp( CDNGameRoom *pRoom, int nPropHandle );
bool api_trigger_IsBrokenProp( CDNGameRoom *pRoom, int nPropHandle );
void api_trigger_DestroyMonsterArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nDropType );
void api_trigger_DestroyAllMonster( CDNGameRoom *pRoom, bool bDropItem, int nTeam );
void api_trigger_SetMonsterAIState( CDNGameRoom *pRoom, int nEventAreaHandle, int nValue );
void api_trigger_PlayCutSceneByTrigger( CDNGameRoom *pRoom, int nCutSceneTableID, bool bFadeIn, int nQuestIndex , int nQuestStep);
int api_trigger_GetGameTime( CDNGameRoom *pRoom );
bool api_trigger_Delay( CDNGameRoom *pRoom, CEtTriggerElement *pElement, int nDelay );
int api_trigger_GetBattleModeCount( CDNGameRoom *pRoom, int nEventAreaHandle );
char *api_trigger_GetActionToProp( CDNGameRoom *pRoom, int nPropHandle );
void api_trigger_ChangeMapByTrigger( CDNGameRoom *pRoom, int nMapIndex, int nGateNo );
int api_trigger_GetTalkNpcClassID( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
int api_trigger_GetTalkNpcActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
int api_trigger_GetTalkNpcTargetActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
void api_trigger_EnableOperatorProp( CDNGameRoom *pRoom, int nPropHandle, int nActorHandle, bool bEnable );
void api_trigger_AllUserEnableOperatorProp( CDNGameRoom *pRoom, int nPropHandle, bool bEnable );
void api_trigger_SetActionToPropActor( CDNGameRoom *pRoom, int nPropHandle, int nActorHandle, const char *szAction );
int api_trigger_GetLastAimPropActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
int api_trigger_GetLastBrokenPropActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
void api_trigger_EnableTriggerElement( CDNGameRoom *pRoom, CEtTriggerElement *pElement, bool bEnable );
void api_trigger_SetActionToActor( CDNGameRoom *pRoom, int nEventAreaHandle, const char *szAction, int nLoopCount, float fBlendFrame );
void api_trigger_EnableOperatorNpc( CDNGameRoom *pRoom, int nEventAreaHandle, int nActorHandle, bool bEnable );
void api_trigger_EnableOperatorNpcFromPropNpc( CDNGameRoom *pRoom, int nPropHandle, int nActorHandle, bool bEnable );
void api_trigger_EnableTriggerObject( CDNGameRoom *pRoom, const char *szTriggerObjectName, bool bEnable );
void api_trigger_EnableMonsterNoDamage( CDNGameRoom *pRoom, int nEventAreaHandle, bool bEnable );
void api_trigger_WarpActor( CDNGameRoom *pRoom, int nActorHandle, int nTargetEventAreaHandle );
void api_trigger_WarpActorFromEventArea( CDNGameRoom *pRoom, int nMonsterEventAreaHandle, int nTargetEventAreaHandle );
bool api_trigger_IsInsidePropToEventArea( CDNGameRoom* pRoom, int nPropHandle, int nTargetEventAreaHandle );
void api_trigger_ChangeTeamFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nTeam );
bool api_trigger_IsDieActor( CDNGameRoom *pRoom, int nActorHandle );
int api_trigger_GetUserLevelByTrigger( CDNGameRoom *pRoom, int nActorHandle );
int api_trigger_GetMonsterActorFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nIndex );
void api_trigger_DropItemToEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nItemID, int nCount, DWORD dwOwnerActorHandle = 0 );
void api_trigger_DropItemToActor( CDNGameRoom *pRoom, int nActorHandle, int nItemID, int nCount, DWORD dwOwnerActorHandle = 0 );
void api_trigger_ShowChatBalloonToMonsterArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nUIStringIndex );
void api_trigger_ShowChatBalloonToActor( CDNGameRoom *pRoom, int nActorHandle, int nUIStringIndex );
void api_trigger_ShowChatBalloonToProp( CDNGameRoom *pRoom, int nPropHandle, int nUIStringIndex );
void api_trigger_HideChatBalloonToMonsterArea( CDNGameRoom *pRoom, int nEventAreaHandle );
void api_trigger_HideChatBalloonToProp( CDNGameRoom *pRoom, int nPropHandle );
void api_trigger_SetForceAggroToMonsterArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nTargetEventArea, int nValue );
void api_trigger_SetForceAggroToProp( CDNGameRoom *pRoom, int nEventAreaHandle, int nPropHandle, int nValue );
void api_trigger_ShowExposureInfo( CDNGameRoom *pRoom, int nEventAreaHandle, bool bShow );
void api_trigger_DLChangeRound( CDNGameRoom *pRoom, bool bBoss, int iNextTotalRound=0 );
int api_trigger_DLGetRound( CDNGameRoom *pRoom );
int api_trigger_DLGetTotalRound( CDNGameRoom *pRoom );
int api_trigger_DLGetStartFloor( CDNGameRoom* pRoom );
int api_trigger_DLRequestChallenge( CDNGameRoom* pRoom );
int api_trigger_DLResponseChallenge( CDNGameRoom* pRoom );
void api_trigger_DLSetStartRound( CDNGameRoom* pRoom, int iRound );
void api_trigger_EnableDungeonClearWarpQuestFlag(CDNGameRoom* pRoom);
bool api_trigger_IsEnableDungeonClearWarp(CDNGameRoom* pRoom);
bool api_trigger_IsExistNotAchieveMissionPartyActor( CDNGameRoom *pRoom, int nMissionArrayIndex );
int api_trigger_GetChangeActionPlayerActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
char *api_trigger_GetActionToPlayerActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
void api_trigger_EnablePropNoDamage( CDNGameRoom *pRoom, int nPropHandle, bool bEnable );
int api_trigger_GetTriggerEventSignalEventAreaHandle( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
int api_trigger_GetTriggerEventSignalActorHandle( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
int api_trigger_GetTriggerEventSignalEventID( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
int api_trigger_GetTriggerEventSignalTeam( CDNGameRoom *pRoom, CEtTriggerElement *pElement );
void api_trigger_HoldPartyDice(CDNGameRoom* pRoom);
void api_trigger_ReleasePartyDice(CDNGameRoom* pRoom);
void api_trigger_UpdateNestTryCount(CDNGameRoom *pRoom,int iForceMapIndex=0 );
void api_trigger_UpdateUserNestTryCount(CDNGameRoom *pRoom, int nActorHandle );
int api_trigger_GetMonsterClassID( CDNGameRoom *pRoom, int nActorHandle );
void api_trigger_EnableDungeonClearWarpAlarm( CDNGameRoom *pRoom, bool bEnable );
void api_trigger_SetHPFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nValue );
void api_trigger_SetHP( CDNGameRoom *pRoom, int nActorHandle, int nValue );
void api_trigger_AddHPAbsoluteFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nValue );
void api_trigger_AddHPRatioFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, float fValue );
void api_trigger_AddHPAbsolute( CDNGameRoom *pRoom, int nActorHandle, int nValue );
void api_trigger_AddHPRatio( CDNGameRoom *pRoom, int nActorHandle, float fValue );
int api_trigger_GetTeamFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle );
int api_trigger_GetTeam( CDNGameRoom *pRoom, int nActorHandle );
void api_trigger_SetHPRatioFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, float fValue );
void api_trigger_SetHPRatio( CDNGameRoom *pRoom, int nActorHandle, float fValue );
void api_trigger_IgnoreAggroFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nValue );
int api_trigger_GetPropDurability( CDNGameRoom *pRoom, int nPropHandle );
int api_trigger_LastAimPropActorMessage( CDNGameRoom *pRoom, int nType, int nActorHandle, int nBaseStringIdx, lua_tinker::table ParamTable );
void api_trigger_BuffFromEventArea( CDNGameRoom *pRoom, int nActorHandle, int nSkillID, int nSkillLevel, int nDelay);
bool api_trigger_NoDuplicateBuffFromEventArea( CDNGameRoom *pRoom, int nActorHandle, int nSkillID, int nSkillLevel, int nDelay);
void api_trigger_Start_DragonNest( CDNGameRoom* pRoom, int nType, int nRebirthCount );
void api_trigger_SetUsableRebirthCoin( CDNGameRoom* pRoom, int nActorHandle, int nUsableRebirthCoin );
int api_trigger_GetMonsterHPPercent_FromEventArea( CDNGameRoom* pRoom, int nEventAreaHandle );
void api_trigger_UseSkillFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nSkillID );
void api_trigger_UseSkill( CDNGameRoom *pRoom, int nActorHandle, int nSkillID );
void api_trigger_NotUseManaSkill( CDNGameRoom *pRoom, int nActorHandle );
void api_trigger_AllUser_AddVehicleItem(CDNGameRoom *pRoom, char* pszStr, bool bDBSave );
void api_trigger_PlayerNameShowBlindCaption( CDNGameRoom *pRoom, int nUIStringIndex, int nDelay, int nActorHandle );
void api_trigger_InvenToEquip( CDNGameRoom* pRoom, UINT nUserObjectID, int iItemID, bool bShootMode, bool bBackup=false );
void api_trigger_UnRideVehicle( CDNGameRoom* pRoom, UINT nUserObjectID );
void api_trigger_ChangeMyBGM( CDNGameRoom * pRoom, int nActorHandle, int nBGM, int nFadeDelta = 0 );
void api_trigger_FileTableBGMOff( CDNGameRoom * pRoom, int nActorHandle );
void api_trigger_RadioMsg( CDNGameRoom * pRoom, int nActorHandle, int nRadioID );
void api_trigger_RadioImage( CDNGameRoom * pRoom, int nActorHandle, int nFileID, DWORD nTime );
int api_trigger_GetPvPGameModePlayTime( CDNGameRoom * pRoom );
void api_trigger_DropGroupItemToEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nItemID, DWORD dwOwnerActorHandle = 0 );
void api_trigger_DropGroupItemToActor( CDNGameRoom *pRoom, int nActorHandle, int nItemID, DWORD dwOwnerActorHandle = 0 );
void api_trigger_TargetActorShowNpc( CDNGameRoom * pRoom, int nActorHandle, int nEventAreaHandle, bool bShow );
void api_trigger_TargetActorShowProp( CDNGameRoom * pRoom, int nActorHandle, int nPropHandle, bool bShow );
int  api_trigger_TriggerVariableTable( CDNGameRoom * pRoom, int nItemID );
int  api_trigger_GetPvPGameMode( CDNGameRoom * pRoom );
void api_trigger_CmdKeepOperationProp( CDNGameRoom * pRoom, int nPropHandle, int nActorHandle, int nOperationTime, int nStringID, const char *szAction );
void api_trigger_ActivateMission( CDNGameRoom * pRoom, int nActorHandle, int nMissionID);
void api_trigger_SetDungeonClearRound( CDNGameRoom * pRoom, int iRound );
bool api_trigger_BreakIntoUserCheck( CDNGameRoom* pRoom );
bool api_trigger_DungeonStartTimeAttack( CDNGameRoom* pRoom, int iMin, int iSec );
int api_trigger_DungeonCheckTimeAttack( CDNGameRoom* pRoom );
void api_trigger_DungeonStopTimeAttack( CDNGameRoom* pRoom );
void api_trigger_RequestDungeonFail( CDNGameRoom* pRoom, bool bTimeOut=false );
void api_trigger_Special_RebirthItem( CDNGameRoom* pRoom, int nType, int nITemID, int nRebirthItemCount );
void api_trigger_GameWarpDungeonClearToLeader(CDNGameRoom* pRoom, bool bIgnoreClear = false);
void api_trigger_ActorBuffFromEventArea( CDNGameRoom *pRoom, int nActorHandle, int nEventAreaHandle, int nSkillID, int nSkillLevel, int nDelay, bool bSummonInclude );
void api_trigger_console_WriteLine( char * szOutputString, const char * s0, const char * s1, const char * s2, const char * s3, const char * s4, const char * s5,
								   const char * s6, const char * s7, const char * s8, const char * s9 );
int api_trigger_GetPartyCountbyRoom( CDNGameRoom* pRoom );
int api_trigger_GetPartyActorbyRoom( CDNGameRoom* pRoom, int nFirstPartyIndex );

int api_trigger_GetRacingStartTime( CDNGameRoom * pRoom );
int api_trigger_GetRacingGoalTime( CDNGameRoom * pRoom, const int nActorHandle );
int api_trigger_GetCurrentWeather( CDNGameRoom* pRoom );
void api_trigger_StartDungeonGateTime( CDNGameRoom* pRoom, int nGateNumber );
void api_trigger_EndDungeonGateTime( CDNGameRoom* pRoom, int nGateNumber  );
int api_trigger_StageRemainClearCount( CDNGameRoom* pRoom, UINT nUserObjectID, int nMapIndex );
void api_trigger_EnableRideVehicle( CDNGameRoom * pRoom, UINT nActorHandle, bool bEnableRide );
void api_trigger_AllMonsterUseSkill( CDNGameRoom * pRoom, int nSkillID, int nSkillLevel, int nDelay, bool bEternity );
bool api_trigger_RequireTotalComboCount( CDNGameRoom * pRoom, UINT nActorHandle, int nTotalComboCount );
void api_trigger_RequireTotalComboReset( CDNGameRoom * pRoom, UINT nActorHandle );

/*
desc   : 파티 중 특정 클래스의 인원을 체크
         [Interger1] 유저 클래스의 인원을 반환
param  : 직업(Interger1)1 워리어 2 아쳐, 3 소서리스 4 클래스 5 아카데믹 6 칼리
return : 파티내 [Interger1] 클래스의 인원
*/
int api_trigger_GetPartyInClassCount( CDNGameRoom* pRoom, int nClassID );

void api_trigger_Set_AlteiaWorld( CDNGameRoom* pRoom, bool bFlag );
void api_tirgger_Set_UserHPSP( CDNGameRoom * pRoom, UINT nActorHandle, int nHP_Ratio, int nSP_Ratio );

void api_trigger_Set_ItemUseCount( CDNGameRoom* pRoom, int nItemID, int nUseCount );	// 스테이지 아이템 사용횟수 제한

//rlkt_test
bool api_trigger_CheckJobChangeProcess(CDNGameRoom* pRoom, UINT nActorHandle);
int api_ui_OpenJobChange(CDNGameRoom* pRoom, UINT nUserObjectID);
int api_trigger_Emergence_GetValue(CDNGameRoom* pRoom, UINT nValueType);
int api_trigger_Emergence_Begin(CDNGameRoom* pRoom, int nEventAreaHandle);
bool api_trigger_Emergence_CheckSelectedEvent(CDNGameRoom* pRoom, int nEventAreaHandle);
bool api_trigger_CheckForceMoveComebackRoom(CDNGameRoom* pRoom);