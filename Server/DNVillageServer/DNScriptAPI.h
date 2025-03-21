#pragma once

// VillageServer Script API
//  [4/1/2008 nextome]

class CDNUserBase;

int CheckInvenForQuestReward(CDNUserBase* pUser, const TQuestReward& QuestReward, const bool bSelectedArray[]);

// 모든 함수를 루아에 등록한다.
void DefAllAPIFunc(lua_State* pLuaState);

/*
desc   : 다음 대사로 연결한다.
param  : 유저인덱스, NPC인덱스, 토크인덱스, 토크파일이름
return : void
*/
void api_npc_NextTalk(UINT nUserObjectID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

/*
desc   : 다음 스크립트로 연결한다.
param  : 유저인덱스, NPC인덱스, 토크인덱스, 토크파일이름
return : void
*/
void api_npc_NextScript(UINT nUserObjectID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

/*
desc   : 대사 파라메터를 세팅한다(스트링).  
param  : 파라메터키, 파라메터문자의 인덱스
return : -1 유저를 찾을수 없음, -2 파라메터 갯수가 넘어서서 더이상 세팅할수 없음. 1 이면 성공
sample : 
			저는 {%color} 색을 좋아합니다.  라는 문장을 만들때.
			api_SetParamString("{%color}, math.random(2) );  이런식으로 만들어준다.
			1은 TalkParamTable.xls 에 1 : 빨강, 2: 파랑 따로 기록 되어있어야한다.

		주의: api_SetParamString 으로 세팅한 파라메터는 api_NextTalk 호출후 clear 되므로 api_NextTalk 호출전에 세팅한다.
*/
int api_npc_SetParamString(UINT nUserObjectID, const char* szParamKey, int nParamID);

/*
desc   : 대사 파라메터를 세팅한다(정수).  
param  : 파라메터키, 정수값
return : -1 유저를 찾을수 없음, -2 파라메터 갯수가 넘어서서 더이상 세팅할수 없음. 1 이면 성공
sample : 
			저는 {%age} 살쯤에 숨지고 싶어요.  라는 문장을 만들때.
			api_SetParamInt("{%age}, math.random(30)+40 );  이런식으로 만들어준다.

		주의: api_SetParamInt 으로 세팅한 파라메터는 api_NextTalk 호출후 clear 되므로 api_NextTalk 호출전에 세팅한다.
*/
int api_npc_SetParamInt(UINT nUserObjectID, const char* szParamKey, int nValue);


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
void api_log_UserLog(UINT nUserObjectID, const char* szLog);

/*
desc   : 유저에게 퀘스트 관련 정보를 채팅으로 보내준다.
param  : 
return : void
*/
void api_quest_DumpQuest(UINT nUserObjectID);


/*
desc   : 유저에게 퀘스트를 부여 한다.
param  : 유저인덱스, 퀘스트인덱스, 퀘스트 타입 QuestType_SubQuest = 1, QuestType_MainQuest = 2,
return : 1 : 성공, -1 : 유저를 찾을수 없음, -2 : 유저가 퀘스트를 가지고 있지 않음
*/
int api_quest_AddHuntingQuest(UINT nUserObjectID, int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt);
int api_quest_AddQuest(UINT nUserObjectID, int nQuestID, int nQuestType);

/*
desc   : 유저에게 퀘스트를 완료 시킨다.
param  : 유저인덱스, 퀘스트인덱스, 현재 가지고 있는 퀘스트에서 지울것인지 (안지우면 플레이 횟수만 증가 시킨다.), 반복 가능 여부
return : 1 : 성공, -1 : 유저를 찾을수 없음, -2 : 유저가 퀘스트를 가지고 있지 않음
*/
int api_quest_CompleteQuest(UINT nUserObjectID, int nQuestID, bool bDelPlayList, bool bRepeat);

/*
desc   : 유저에게 퀘스트를 완료목록에 추가 시킨다. ( DB 에 기록!! ) 
param  : 유저인덱스, 퀘스트인덱스
return : -1 유저를 찾을수 없음, -2 퀘스트가 진행중이 아님 
*/
int api_quest_MarkingCompleteQuest(UINT nUserObjectID, int nQuestID);

/*
desc   : 완료목록에 완료마킹이 되어있는 퀘스트인지?
param  : 유저인덱스, 퀘스트인덱스
return : -1 유저를 찾을수 없음, 1 이면 완료마킹된 퀘스트, 0 이면 완료마킹 안됨
*/
int api_quest_IsMarkingCompleteQuest(UINT nUserObjectID, int nQuestID);




/*
desc   : 유저가 퀘스트를 가지고 있는지 확인한다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 유저를 찾을수 없음, -2 유저가 퀘스트를 가지고 있지 않음, -1 보다 크면 퀘스트를 가지고 잇음 ( 퀘스트 스텝을 리턴 )
*/
int api_quest_UserHasQuest(UINT nUserObjectID, int nQuestID);

/*
desc   : 유저가 몇개의 퀘스트를 진행중인지 확인한다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 유저를 찾을수 없음, 그외에 현재 진행중인 퀘스트 갯수 리턴..
*/
int api_quest_GetPlayingQuestCnt(UINT nUserObjectID);


/*
desc   : npc 인덱스를 리턴한다.
param  : 유저인덱스, NpcObjId ( npc 인덱스와 npcobject 인덱스는 다른 것. )
return : -1 유저를 찾을수 없음, -2 npc 를 찾을수 없음, 그외에는 npc인덱스
*/
int api_npc_GetNpcIndex(UINT nUserObjectID, UINT nNpcObjID);

/*
desc   : 유저가 가지고 있는 퀘스트의 스텝, 저널 스텝을 세팅한다.
param  : 유저인덱스, 퀘스트인덱스, 저널 스텝
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음.  -3 저널 스텝 범위를 벗어남. 성공일 경우 1을 리턴
*/
int api_quest_SetQuestStepAndJournalStep(UINT nUserObjectID, int nQuestID, short nQuestStep, int nJournalStep);

/*
desc   : 유저가 가지고 있는 퀘스트의 퀘스트 스텝을 세팅한다.
param  : 유저인덱스, 퀘스트인덱스, 퀘스트 스텝
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음. 성공일 경우 1을 리턴
*/
int api_quest_SetQuestStep(UINT nUserObjectID, int nQuestID, short nQuestStep);

/*
desc   : 유저가 가지고 있는 퀘스트의 퀘스트 스텝을 얻는다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음., -1 보다 큰경우 현재 퀘스트의 스텝
*/
int api_quest_GetQuestStep(UINT nUserObjectID, int nQuestID);

/*
desc   : 유저가 가지고 있는 퀘스트의 저널 스텝을 세팅한다.
param  : 유저인덱스, 퀘스트인덱스, 저널 스텝
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음.  -3 저널 스텝 범위를 벗어남. 성공일 경우 1을 리턴
*/
int api_quest_SetJournalStep(UINT nUserObjectID, int nQuestID, int nJournalStep);

/*
desc   : 유저가 가지고 있는 퀘스트의 저널 스텝을 얻는다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 => 유저를 찾을수 없음 -2 => 유저가 퀘스트를 가지고 있지 않음., -1 보다 큰경우 현재 저널 스텝
*/
int api_quest_GetJournalStep(UINT nUserObjectID, int nQuestID);

/*
desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스에 값을 세팅한다.
param  : 유저인덱스, 퀘스트번호, 메모인덱스, 메모값
return : -1 => 유저를 찾을수 없음. -2 => 유저가 퀘스트를 가지고 있지 않음. -3 메모 인덱스 범위를 벗어남. 성공일 경우 1을 리턴
*/
int api_quest_SetQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex, TP_QUESTMEMO nVal);

/*
desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스의 값을 얻는다.
param  : 유저인덱스, 퀘스트번호, 메모인덱스
return : -1 => 유저를 찾을수 없음. -2 => 유저가 퀘스트를 가지고 있지 않음. -3 메모 인덱스 범위를 벗어남. -1 보다 큰경우 특정 메모 인덱스의 값
*/
int api_quest_GetQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : 몬스터 잡거나, 아이템 얻는 갯수 카운팅 정보를 세팅하는 함수 이다.
param  : 유저인덱스, 
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
int api_quest_SetCountingInfo(UINT nUserObjectID, int nQuestID,  int nSlot, int nCountingType, int nCountingIndex, int nTargetCnt); 


/*
desc   : 세팅된 카운팅 정보를 클리어 한다. 
param  : 유저인덱스, 퀘스트 인덱스
return : -1 => 유저를 찾을수 없음 -2 => 퀘스트를 찾을수 없음,
*/
int api_quest_ClearCountingInfo(UINT nUserObjectID, int nQuestID);

/*
desc   : 세팅된 카운팅들이 모두 완료 되었는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 퀘스트를 찾을수 없음,  -3이면 완료 안됨, 1 이면 모두 완료
*/
int api_quest_IsAllCompleteCounting(UINT nUserObjectID, int nQuestID);

/*
desc   : 유저퀘스트를 캔슬시킨다.
param  : 유저인덱스,  퀘스트 아이디
return : -1 => 유저를 찾을수 없음, -2면 실패 1이면 성공
 */
int api_quest_CancelQuest(UINT nUserObjectID, int nQuestID);


/*
desc   : 유저에게 아이템을 넣을만한 인벤여유가 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족
*/
int api_user_CheckInvenForAddItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

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
int api_user_CheckInvenForAddItemList(UINT nUserObjectID, lua_tinker::table ItemTable);

/*
desc   : 유저에게 아이템을 지급한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트, 퀘스트 아이디
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족
*/
int api_user_AddItem(UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID);

/*
desc   : 유저에게서 아이템을 지운다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트, 퀘스트 아이디
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족
*/
int api_user_DelItem(UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID);

/*
desc   : 유저에게서 아이템을 모두 지운다.
param  : 유저인덱스, 아이템인덱스
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음
*/
int api_user_AllDelItem(UINT nUserObjectID, int nItemIndex);

/*
desc   : 유저가 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_user_HasItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);


/*
desc   : 유저의 클래스 ID 를 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 ClasssID
*/
int api_user_GetUserClassID(UINT nUserObjectID);


/*
desc   : 유저의 Job ID 를 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 Job ID
*/
int api_user_GetUserJobID(UINT nUserObjectID);


/*
desc   : 유저의 level을 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 Level
*/
int api_user_GetUserLevel(UINT nUserObjectID);

/*
desc   : 유저 인벤토리의 빈칸수를 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 빈칸수
*/
int api_user_GetUserInvenBlankCount(UINT nUserObjectID);

/*
desc   : 유저 퀘스트 인벤토리의 빈칸수를 얻는다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 빈칸수
*/
int api_quest_GetUserQuestInvenBlankCount(UINT nUserObjectID);

/*
desc   : 유저에게 경험치를 추가한다.
param  : 유저인덱스, 경험치
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_user_AddExp(UINT nUserObjectID, int nQuestID, int nAddExp);

/*
desc   : 유저에게 돈을 추가한다.
param  : 유저인덱스, 돈
return : -1 => 유저를 찾을수 없음,  1이면 성공
*/
int api_user_AddCoin(UINT nUserObjectID, int nAddCoin, int nQuestID);

/*
desc   : 유저에게 동영상을 재생 시킨다.
param  : 유저인덱스, 동영상파일이름
return : -1 => 유저를 찾을수 없음,  1이면 성공
*/
int api_user_PlayCutScene(UINT nUserObjectID, UINT nNpcObjectID, int nCutSceneTableID, bool bIgnoreFadeIn);

/*
desc   : 유저에게 창고를 열라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음,  1이면 성공
*/
int api_ui_OpenWareHouse(UINT nUserObjectID, int iItemID/*=0*/ );

/*
desc   : 유저에게 상점를 열라고 알린다.
param  : 유저인덱스, 상점 ID
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenShop(UINT nUserObjectID, int nShopID, Shop::Type::eCode Type=Shop::Type::Normal );

/*
desc   : 유저에게 스킬 상점를 열라고 알린다.
param  : 유저인덱스, 스킬 상점 ID
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenSkillShop(UINT nUserObjectID);

/*
desc   : 유저에게 무인상점을 열라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenMarket(UINT nUserObjectID);

/*
desc   : 유저에게 문장보옥 합성 창을 열라고 한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenCompoundEmblem(UINT nUserObjectID);


/*
desc   : 유저에게 보옥 업그레이드 창을 열라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenUpgradeJewel(UINT nUserObjectID);


/*
desc   : 유저에게 우편함을 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenMailBox(UINT nUserObjectID);

/*
desc   : 유저에게 문장 해제를 위해 문장차을 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenGlyphLift(UINT nUserObjectID);

/*
desc   : 유저에게 아이템 강화창을 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenUpgradeItem(UINT nUserObjectID,int iItemID=0);

#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
/*
desc   : 텍스쳐 다이얼로그를 연다.
param  : 유저인덱스, 파일인덱스, 화면에서의 위치, 보여질 시간, 이미지 중앙점,
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenTextureDialog(UINT nUserObjectID, int nFileIndex, float fX, float fY, int nPos, int nTime, int nDialogIndex, bool bAutoCloseDialog);

/*
desc   : 텍스쳐 다이얼로그를 닫는다.
param  : 유저인덱스, 다이얼로그 ID
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_CloseTextureDialog(UINT nUserObjectID, int nDlgID);
#endif


int api_ui_OpenDisjointItem(UINT nUserObjectID);


/*
desc   : 유저에게 아이템 합성창을 열라고 시킨다.
param  : 유저인덱스, 아이템 합성 상점 ID
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenCompoundItem(UINT nUserObjectID, int nCompoundShopID);

/*
desc   : 유저에게 아이템 합성창2을 열라고 시킨다.
param  : 유저인덱스, 아이템 합성 상점 ID
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/

int api_ui_OpenCompound2Item(UINT nUserObjectID, int nCompoundGroupID, int iItemID/*=0*/ );

/*
desc   : 유저에게 캐쉬 아이템 상점을 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenCashShop(UINT nUserObjectID);


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
int api_ui_OpenGuildMgrBox(UINT nUserObjectID, int nGuildMgrNo);

/*
desc   : 일본 가챠폰 다이얼로그를 연다.
param  : 사용될 가챠폰 인덱스
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공, -3 이면 디파인에 걸러짐..
*/
int api_ui_OpenGacha_JP( UINT nUserObjectID, int nGachaShopID );

/*
desc   : NPC 선물하기 다이얼로그를 연다.
param  : 대화중인 Npc ID 
return : -1 => 유저를 찾을수 없음. 1 이면 성공.
*/
int api_ui_OpenGiveNpcPresent( UINT nUserObjectID, int nNpcID );

/*
desc   : 다크레어랭킹게시판 다이얼로그를 연다.
param  : 대화중인 Npc ID 
return : -1 => 유저를 찾을수 없음. 1 이면 성공.
*/
int api_ui_OpenDarkLairRankBoard( UINT nUserObjectID, int nNpcID, int iMapIndex, int iPlayerCount );


/*
desc   : PvP래더랭킹게시판 다이얼로그를 연다.
param  : 대화중인 Npc ID 
return : -1 => 유저를 찾을수 없음. 1 이면 성공.
*/
int api_ui_OpenPvPLadderRankBoard( UINT nUserObjectID, int nNpcID, int iPvPLadderCode );
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

int api_user_UserMessage(UINT nUserObjectID, int nType, int nBaseStringIdx, lua_tinker::table ParamTable);

/*
desc   : 유저에게 심볼아이템을 추가한다.
param  : 유저인덱스,  심볼아이템아이디, 갯수
return : -1 => 유저를 찾을수 없음, -2면 실패 1이면 성공
*/
int api_quest_AddSymbolItem(UINT nUserObjectID, int nItemID, short wCount);

/*
desc   : 유저에게 심볼아이템을 삭제한다.
param  : 유저인덱스,  심볼아이템아이디, 갯수
return : -1 => 유저를 찾을수 없음, -2 면 실패, 1이면 성공
*/
int api_quest_DelSymbolItem(UINT nUserObjectID, int nItemID, short wCount);

/*
desc   : 유저가 심볼 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_quest_HasSymbolItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : 유저에게 퀘스트 아이템을 넣을만한 인벤여유가 있는지 확인한다.
param  : 유저인덱스, 퀘스트 아이템 인덱스, 퀘스트 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 인벤 부족
*/
int api_quest_CheckQuestInvenForAddItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

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
int api_quest_CheckQuestInvenForAddItemList(UINT nUserObjectID, lua_tinker::table ItemTable);

/*
desc   : 유저에게 퀘스트 아이템을 추가한다.
param  : 유저인덱스,  퀘스트아이템아이디, 갯수, 퀘스트 아이디
return : -1 => 유저를 찾을수 없음, -2면 실패 1이면 성공
*/
int api_quest_AddQuestItem(UINT nUserObjectID, int nItemID, short wCount, int nQuestID);

/*
desc   : 유저에게 퀘스트 아이템을 삭제한다.
param  : 유저인덱스,  퀘스트아이템아이디, 갯수, 퀘스트 아이디
return : -1 => 유저를 찾을수 없음, -2 면 실패, 1이면 성공
*/
int api_quest_DelQuestItem(UINT nUserObjectID, int nItemID, short wCount, int nQuestID);

/*
desc   : 유저에게 퀘스트 아이템을 모두 삭제한다.
param  : 유저인덱스,  퀘스트아이템아이디
return : -1 => 유저를 찾을수 없음, -2 면 실패, 1이면 성공
*/
int api_quest_AllDelQuestItem(UINT nUserObjectID, int nItemID);

/*
desc   : 유저가 퀘스트 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_quest_HasQuestItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : 유저에게 길드 생성창을 띄우라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenGuildCreate(UINT nUserObjectID);

/*
desc   : 유저에게 퀘스트 보상창을 띄우라고 알린다.
param  : 유저인덱스, 보상테이블, bActivate 가 true 면 고를수 있고 이면 그냥 보는 용도는 false로 쓴다.
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenQuestReward(UINT nUserObjectID, int nRewardTableIndex, bool bActivate);

/*
desc   : 유저에게 퀘스트 보상을 한다. 
param  : 유저인덱스, 보상테이블, 퀘스트 아이디, 보상체크번호
return : -1 => 유저를 찾을수 없음, -2 유저아이템객체 오류, -3 인벤 부족, -4 이면 테이블을 찾을수 없음, -5 보상 조건 체크 오류, 1 이면 성공
*/
int api_quest_RewardQuestUser(UINT nUserObjectID, int nRewardTableIndext, int nQuestID, int nRewardCheck);

/*
desc   : 유저를 채널이동시킨다.
param  : 유저인덱스, 채널타입
return : -1 => 유저를 찾을수 없음, -2 없는 채널타입 -3 없는 채널
*/
int api_user_ChangeChannel(UINT nUserObjectID, int ChannelType );

/*
desc   : 유저를 이동시킨다.
param  : 유저인덱스, 이동할 맵 인덱스, 이동할 게이트 인덱스
return : -1 => 유저를 찾을수 없음, -2 없는 맵임
*/
int api_user_ChangeMap(UINT nUserObjectID, int nMapIndex, int nGateNo);

/*
desc   : 해당 유저의 파티원수를 리턴한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 파티원수 (주의 !!!> 파티에 가입되지 않은 사용자도 1 을 반환)
*/
int api_user_GetPartymemberCount(UINT nUserObjectID);

/*
desc   : 해당 유저가 파티에 가입된 상태인지 체크한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 0 => 파티에 가입 않되어있음, 1 => 파티에 가입 되어있음
*/
int api_user_IsPartymember(UINT nUserObjectID);

//blondy
/*
desc   : 해당유저를 PVP로비오 안내.
param  : 유저인덱스
return : 1 성공 -1 => 유저를 찾을수 없음
*/
int api_user_RequestEnterPVP( UINT nUserObjectID );

/*
desc   : 해당 유저의 스테이지 난이도를 구함
param  : 유저인덱스
return : 0 ~ 4 => 던전 난이도 (0:쉬움 / 1:보통 / 2:어려움 / 3:마스터 : 4:어비스), 그 외는 실패 (기본 -1)
*/
int api_user_GetStageConstructionLevel(UINT nUserObjectID);

/*
desc   : 해당 유저의 현재 맵번호를 구함
param  : 유저인덱스
return : 맵번호 (1 이상), 그 외는 실패 ? (0 이하)
*/
int api_user_GetMapIndex(UINT nUserObjectID);

/*
desc   : 해당 유저의 최근 스테이지 완료 랭크를 구함
param  : 유저인덱스
return : 스테이지 완료 랭크 (SSS : 0 / SS : 1 / S : 2 / A : 3 / B : 4 / C : 5 / D : 6), 그 외는 실패 ? (기본 -1)
*/
int api_user_GetLastStageClearRank(UINT nUserObjectID);

/*
desc   : 유저의 현재 소지금을 확인한다.
param  : 유저인덱스, 체크할 값
return : -1 => 유저를 찾을 수 없음, -2 => 소지금이 부족함, 1 => 소지금이 충분함
*/
int api_user_EnoughCoin(UINT nUserObjectID, int nCoin );

/*
desc   : 유저의 현재 소지금을 확인한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 0 이상 => 현재 소지금
*/
int api_user_GetCoin(UINT nUserObjectID);

/*
desc   : 유저에게 돈을 감소한다.
param  : 유저인덱스, 돈
return : -1 => 유저를 찾을수 없음, 1 => 성공
*/
int api_user_DelCoin(UINT nUserObjectID, int nDelCoin);

/*
desc   : 유저의 특정 미션 취득 여부를 반환
param  : 유저인덱스, 미션인덱스 (MISSIONMAX (현재 5000 → 0 ~ 4999))
return : -1 => 유저를 찾을수 없음, 0 => 해당 미션 없음, 1 포함 이외 => 해당 미션 존재
*/
int api_user_IsMissionGained(UINT nUserObjectID, int nMissionIndex);

/*
desc   : 유저의 특정 미션 완료 여부를 반환
param  : 유저인덱스, 미션인덱스 (MISSIONMAX (현재 5000 → 0 ~ 4999))
return : -1 => 유저를 찾을수 없음, 0 => 해당 미션 없음, 1 포함 이외 => 해당 미션 존재
*/
int api_user_IsMissionAchieved(UINT nUserObjectID, int nMissionIndex);

/*
desc   : 유저가 창고에 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_user_HasItemWarehouse(UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : 유저가 장비에 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, 1 포함 그외 true
*/
int api_user_HasItemEquip(UINT nUserObjectID, int nItemIndex);

/*
desc   : 유저의 현재 길드 직급을 반환
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음 / 0 : 길드 없음 / 1 : 길드장 / 2 : 부길드장 / 3 : 선임길드원 / 4 : 일반길드원 / 5 : 신입길드원
*/
int api_guild_GetGuildMemberRole(UINT nUserObjectID);

/*
desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스에 값을 1 증가시킴
param  : 유저인덱스, 퀘스트번호, 메모인덱스
return : 증가된 퀘스트 메모값 (에러일 경우에는 LONG_MIN 반환)
*/
int api_quest_IncQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : 유저가 가지고 있는 퀘스트의 메모의 특정 인덱스에 값을 1 감소시킴
param  : 유저인덱스, 퀘스트번호, 메모인덱스
return : 감소된 퀘스트 메모값 (에러일 경우에는 LONG_MAX 반환)
*/
int api_quest_DecQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : 유저에게 특정 전직 직업을 지정
param  : 유저인덱스, 변경 전직 직업
return : -1 : 유저를 찾을수 없음 / -2 : 없는 전직 번호임 / -3 : 전직 불가 조건임 / 0 : 이미 같은 전직 직업임 / 1 : 전직 직업 변경 성공
*/
int api_user_SetUserJobID(UINT nUserObjectID, int nJobID);

/*
desc   : 유저의 전직 직업이 특정 전직 계열에 있는지 확인
param  : 유저인덱스, 선행 전직 직업
return : -1 : 유저를 찾을수 없음 / 0 : 같은 계열이 아님 / 1 : 같은 계열이 맞음
*/
int api_user_IsJobInSameLine(UINT nUserObjectID, int nBaseJobID);

/*
desc   : 유저가 캐쉬 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스, 아이템 카운트
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, -3 아이템 갯수 부족, 0 보다 크면 true ( 갯수를 리턴 )
*/
int api_user_HasCashItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : 유저가 장비에 캐쉬 아이템을 가지고 있는지 확인한다.
param  : 유저인덱스, 아이템인덱스
return : -1 => 유저를 찾을수 없음 -2 => 아이템을 찾을수 없음, 1 포함 그외 true
*/
int api_user_HasCashItemEquip(UINT nUserObjectID, int nItemIndex);

/*
desc   : 특정 카운팅 타입, ID 에 대해서 임의로 카운팅을 증가시킨다.
param  : 유저인덱스, 
		카운팅할 타입 (CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3),
		카운팅할 것의 인덱스 (아이템 ID, 몬스터 ID 등등)
return : -1 => 유저를 찾을수 없음, 1 => 성공
*/
int api_quest_IncCounting(UINT nUserObjectID, int nCountingType, int nCountingIndex);

/*
desc   : 유저가 진행 가능 최대 퀘스트 수에 도달하였는지 여부를 반환한다.
param  : 유저인덱스, 퀘스트인덱스
return : -1 : 유저를 찾을수 없음, 1 : 최대 퀘스트 수에 도달 함, 그외 : 아님
*/
int api_quest_IsPlayingQuestMaximum(UINT nUserObjectID);

/*
desc   : 특정 퀘스트를 강제 완료 시킨다. (진행 중이 아닌 퀘스트는 추가하여 완료)
param  : 유저인덱스, 퀘스트인덱스, 퀘스트 코드 (0:일반 / 1:시간제), 마킹여부 (1:마킹/0:안함), 삭제여부 (1:삭제/0:않함), 반복여부 (1:반복/0:안함)
return : -1 : 유저를 찾을수 없음, 1 : 최대 퀘스트 수에 도달 함, 그외 : 아님
*/
int api_quest_ForceCompleteQuest(UINT nUserObjectID, int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat);


/*
desc   : 스킬을 초기화한다
param  : 유저인덱스, 퀘스트인덱스, 
return : -1 : 유저를 찾을수 없음, -2 : 퀘스트를 찾을수 없음, -3: 이미 완료한 퀘스트임, 1 : 스킬초기화 성공
*/
int api_quest_ResetSkill(UINT nUserObjectID, int nQuestID);

/*
desc   : 월드이벤트 완료스텝 알려준다.
param  : 유저인덱스, 스케쥴ID
return : -1 : 에러, 0: 완료, 1 : 시작안내, 2: 수집기간, 3: 보상안내, 4: 보상기간
*/
int api_quest_GetWorldEventStep(UINT nUserObjectID, int nScheduleID);

/*
desc   : 월드이벤트 완료횟수 알려준다.
param  : 유저인덱스, 스케쥴ID
return : -1 : 에러, 그외 현재 카운트
*/
int api_quest_GetWorldEventCount (UINT nUserObjectID, int nScheduleID);


/*
desc   : 아이템을 NPC에게 반납한후 카운트를 퀘스트에 누적한다.
param  : 유저인덱스, 스케쥴ID
return : -1 : 에러, 그외 반납된 카운트
*/
int api_quest_ReturnItemToNpc (UINT nUserObjectID, int nScheduleID);

/*
desc   : 아이템을 NPC에게 반납한 현재 카운트 상황을 알려준다.
param  : 유저인덱스, 스케쥴ID
*/
void api_quest_OpenScoreWorldEvent (UINT nUserObjectID, int nScheduleID);

/*
desc   : 특정 사용자에게 농장구역 리스트를 보여준다
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음, 1 성공
*/
int api_quest_OpenFarmAreaList (UINT nUserObjectID);

/*
desc   : 특정 사용자의 위치를 이동 시킨다.
param  : 유저인덱스, X 좌표, Y 좌표, Z 좌표
return : -1 : 유저를 찾을수 없음, 1 성공
*/
int api_user_ChangePos(UINT nUserObjectID, int nX, int nY, int nZ , int nLookX = 0, int nLookY = 0);

/*
desc   : NPC 호의Point 를 얻어온다.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_GetFavorPoint( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 악의Point 를 얻어온다.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_GetMalicePoint( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 호의Percent 를 얻어온다.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_GetFavorPercent( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 악의Percent 를 얻어온다.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_GetMalicePercent( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 호의Point 를 가감한다.
param  : 유저인덱스, NpcID, 가감값(- 넣으면 감소함)
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_AddFavorPoint( UINT uiUserObjectID, int iNpcID, int val );

/*
desc   : NPC 악의Point 를 가감한다.
param  : 유저인덱스, NpcID, 가감값(- 넣으면 감소함)
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_AddMalicePoint( UINT uiUserObjectID, int iNpcID, int val );

/*
desc   : NPC 에게 보낼 선물 선택한 정보를 서버로 보내라고 클라이언트에게 요청.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_SendSelectedPresent( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 가 분노 이펙트를 출력하도록 클라이언트에게 요청.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_Rage( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC 가 실망 이펙트를 출력하도록 클라이언트에게 요청.
param  : 유저인덱스, NpcID
return : -1 => 유저를 찾을수 없음, -2 => 시스템에러 그외 => 리턴값
*/
int api_npc_Disappoint( UINT uiUserObjectID, int iNpcID );


/*
desc   : 특정 사용자의 전체 스킬을 리셋시킨다.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음, 1 : 성공
*/
int api_user_ResetSkill(UINT nUserObjectID);



/*
desc   : 특정 사용자가 보조 스킬을 새롭게 배운다.
param  : 유저인덱스, 보조스킬ID
return : -1 : 유저를 찾을수 없음, 1 : 성공, 0 : 실패
*/
int api_npc_CreateSecondarySkill( UINT uiObjectUserID, int iSkillID );

/*
desc   : 특정 사용자가 보조 스킬을 배웠는지 체크
param  : 유저인덱스,  보조스킬ID
return : -1 : 유저를 찾을수 없음, 1 : 성공, 0 : 실패
*/
int api_npc_CheckSecondarySkill( UINT uiObjectUserID, int iSkillID );



/*
desc	: 유저에게 길드창고를 열라고 알린다.
param	: 유저인덱스
return  : -1 => 유저를 찾을 수 없음, -2=>길드원이 아님, 1=>성공
*/
int api_ui_OpenGuildWareHouse (UINT nUserObjectID);

/*
desc   : 유저의 현재길드의 창고소유여부를 알려준다.
param  : 유저인덱스
return : -1 : 유저를찾을수없음/ 0 : 길드창고없음/ 1 : 길드창고있음
*/
int api_guild_HasGuildWare(UINT nUserObjectID);


/*
desc   : 유저에게 무비 클립 브라우저를 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenMovieBrowser(UINT nUserObjectID);

/*
desc   : 길드전 신청 여부를 알려준다.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음 / 0 : 길드전 신청불가능 / 1 : 길드전 신청가능
*/
int api_guildwar_IsPreparation(UINT nUserObjectID);
/*
desc   : 길드전 기간 체크.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음 / eGuildWarStepType / 0 : 길드전 기간 아님 / 1 : 길드전 신청 기간 / 2 : 길드전 예선 기간 / 3 : 본선 및 보상기간
*/
int api_guildwar_GetStep(UINT nUserObjectID);
/*
desc   : 길드전 예선 진행 현황 보기
param  : 유저인덱스
return : return : -1 => 유저를 찾을수 없음, -2 => 예선전 기간이 아님, 0 : 성공
*/
int api_guildwar_GetTrialCurrentState(UINT nUserObjectID);
/*
desc   : 길드전 예선 결과
param  : 유저인덱스
return : return : -1 => 유저를 찾을수 없음, -2 => 본선 및 보상기간이 아님, -3 => 아직 예선결과 집계가 완료되지 않음, 0 : 성공
*/
int api_guildWar_GetTrialStats(UINT nUserObjectID);
/*
desc   : 길드전 결승 대진표 보기
param  : 유저인덱스
return : return : -1 => 유저를 찾을수 없음, -2 => 본선 및 보상기간이 아님, -3 => 대진표가 셋팅되지 않았음, 0: 성공
*/
int api_guildWar_GetMatchList(UINT nUserObjectID);
/*
desc   : 길드전 인기 투표 결과
param  : 유저인덱스
return : return : -1 => 유저를 찾을수 없음, -2 => 지난 인기 투표 결과 없음, -3 => 길드전이 진행중임, 0: 성공
*/
int api_guildWar_GetPopularityVote(UINT nUserObjectID);

/*
desc   : 길드전 본선 유무 체크.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음 / 0 : 본선 진행 안함 / 1 : 본선 진행함
*/
int api_guildWar_GetFinalProgress(UINT nUserObjectID);

/*
desc   : 유저에게 브라우저를 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenBrowser(UINT nUserObjectID, const char* szUrl, int nX, int nY, int nWidth, int nHeight);

/*
desc	: 기부 창을 연다.
param	: 
return	: -1 => 유저를 찾을수 없음. 1이면 성공.
*/
int api_ui_OpenDonation(UINT nUserObjectID);


/*
desc	: 인벤토리 창을 연다.
param	: 
return	: -1 => 유저를 찾을수 없음. 1이면 성공.
*/
int api_ui_OpenInventory(UINT nUserObjectID);

/*
desc	: 유저의 PC Cafe등급
param	:
return	: -1 => 유저를 찾을수 없음, 0 => PC방이 아님. 나머지 PC방 등급.
*/
int api_user_GetPCCafe(UINT nUserObjectID);

/*
desc	: PC방 유저에게 무기, 탈것과 같은 대여아이템을 만들어서 넣어준다.
param	:
return	: -1 => 유저를 찾을수 없음, 0 => 성공, 1 => 인벤이 모자름, 2 => 이미 대여아이템을 가지고 있음 3 => 
*/
int api_user_SetPCCafeItem(UINT nUserObjectID);

/*
desc   : 유저가 가진 장비 아이템을 수리하고, 수리비용을 차감합니다.
param  : 유저, 전체수리 유무(1-전체수리, 0-장착한아이템)
return : -1 : 유저를 찾을수 없음 / 0 : 수리할 아이템 없음 / 1 : 소지금 부족 / 2 : 아이템 수리 완료
*/
int api_user_RepairItem(UINT nUserObjectID, int iAllRepair);


/*
desc   : 유저에게 강화이동창을 열라고 시킨다.
param  : 유저
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenExchangeEnchant(UINT nUserObjectID);

/*
desc   : 해당 맵이 닫혀있는지 확인한다.
param  : 유저, 맵 인덱스
return : 1 이면 오픈, 0 이면 클로즈
*/
int api_env_CheckCloseGateByTime(int iMapIndex);

/*
desc   : 코스튬 랜덤 합성창을 연다
param  : 유저
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenCostumeRandomMix(UINT nUserObjectID);

/*
desc   : 월드버프 줄수 있는지 체크
param  : 버프 아이템아이디
return : -1 => 실패, 1이면 성공
*/
int api_quest_WorldBuffCheck(int nItemID);

/*
desc   : 월드버프를 줌
param  : 버프 아이템아이디
return : 
*/
int api_quest_ApplyWorldBuff(UINT nUserObjectID, int nItemID, int nMapIdx);

#if defined(PRE_ADD_DWC)
/*
desc   : DWC 팀 생성 DLg를 연다
param  : 유저인덱스
return : 
*/
void api_ui_OpenCreateDwcTeamDlg(UINT nUserObjectID);
#endif // PRE_ADD_DWC

int api_ui_OpenJobChange(UINT nUserObjectID);