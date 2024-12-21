#pragma once

#include "DnLuaAPIDefine.h"
// GameServer Script API
//  [4/1/2008 nextome]

class CEtTriggerElement;
class CDNGameRoom;
class CDNUserBase;

int CheckInvenForQuestReward(CDNUserBase* pUser, const TQuestReward& QuestReward, const bool bSelectedArray[]);

// ��� �Լ��� ��ƿ� ����Ѵ�.
void DefAllAPIFunc(lua_State* pLuaState);

/*
desc   : ���� ���� �����Ѵ�.
param  : �����ε���, NPC�ε���, ��ũ�ε���, ��ũ�����̸�
return : void
*/
void api_npc_NextTalk(CDNGameRoom *pRoom, UINT nUserSessionID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

/*
desc   : ���� ��ũ��Ʈ�� �����Ѵ�.
param  : �����ε���, NPC�ε���, ��ũ�ε���, ��ũ�����̸�
return : void
*/
void api_npc_NextScript(CDNGameRoom *pRoom, UINT nUserSessionID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);


/*
desc   : ��� �Ķ���͸� �����Ѵ�(��Ʈ��).  
param  : �Ķ����Ű, �Ķ���͹����� �ε���
return : -1 ������ ã���� ����, -2 �Ķ���� ������ �Ѿ�� ���̻� �����Ҽ� ����. 1 �̸� ����
sample : 
���� {%color} ���� �����մϴ�.  ��� ������ ���鶧.
api_npc_SetParamString("{%color}, math.random(2) );  �̷������� ������ش�.
1�� TalkParamTable.xls �� 1 : ����, 2: �Ķ� ���� ��� �Ǿ��־���Ѵ�.

����: api_npc_SetParamString ���� ������ �Ķ���ʹ� api_npc_NextTalk ȣ���� clear �ǹǷ� api_npc_NextTalk ȣ������ �����Ѵ�.
*/
int api_npc_SetParamString(CDNGameRoom *pRoom, UINT nUserObjectID, const char* szParamKey, int nParamID);

/*
desc   : ��� �Ķ���͸� �����Ѵ�(����).  
param  : �Ķ����Ű, ������
return : -1 ������ ã���� ����, -2 �Ķ���� ������ �Ѿ�� ���̻� �����Ҽ� ����. 1 �̸� ����
sample : 
���� {%age} ���뿡 ������ �;��.  ��� ������ ���鶧.
api_npc_SetParamInt("{%age}, math.random(30)+40 );  �̷������� ������ش�.

����: api_npc_SetParamInt ���� ������ �Ķ���ʹ� api_npc_NextTalk ȣ���� clear �ǹǷ� api_npc_NextTalk ȣ������ �����Ѵ�.
*/
int api_npc_SetParamInt(CDNGameRoom *pRoom, UINT nUserObjectID, const char* szParamKey, int nValue);


/*
desc   : �α׸� �����.
param  : �α� ��Ʈ�� �ٹٲ��� ���� \n �� �����Ұ�.
return : void
*/
void api_log_AddLog(CDNGameRoom *pRoom, const char* szLog);

/*
desc   : �������� �α׸� �����. ����ä�ø޼����� ��������.
param  : �α� ��Ʈ�� �ٹٲ��� ���� \n �� �����Ұ�.
return : void
*/
void api_log_UserLog(CDNGameRoom *pRoom, UINT nUserObjectID, const char* szLog);


/*
desc   : �������� ����Ʈ ���� ������ ä������ �����ش�.
param  : 
return : void
*/
void api_quest_DumpQuest(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �������� ����Ʈ�� �ο� �Ѵ�.
param  : �����ε���, ����Ʈ�ε���, ����Ʈ Ÿ�� QuestType_SubQuest = 1, QuestType_MainQuest = 2,
return : 1 : ����, -1 : ������ ã���� ����, -2 : ������ ����Ʈ�� ������ ���� ����
*/
int api_quest_AddHuntingQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt);
int api_quest_AddQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nQuestType);

/*
desc   : �������� ����Ʈ�� �Ϸ� ��Ų��.
param  : �����ε���, ����Ʈ�ε���, ���� ������ �ִ� ����Ʈ���� ��������� (������� �÷��� Ƚ���� ���� ��Ų��.)
return : -1 ������ ã���� ����, -2 ����Ʈ�� �������� �ƴ� 
*/
int api_quest_CompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, bool bDelPlayList);

/*
desc   : �������� ����Ʈ�� �Ϸ��Ͽ� �߰� ��Ų��. ( DB �� ���!! ) 
param  : �����ε���, ����Ʈ�ε���
return : -1 ������ ã���� ����, -2 ����Ʈ�� �������� �ƴ� 
*/
int api_quest_MarkingCompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);


/*
desc   : �Ϸ��Ͽ� �ϷḶŷ�� �Ǿ��ִ� ����Ʈ����?
param  : �����ε���, ����Ʈ�ε���
return : -1 ������ ã���� ����, 1 �̸� �ϷḶŷ�� ����Ʈ, 0 �̸� �ϷḶŷ �ȵ�
*/
int api_quest_IsMarkingCompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);




/*
desc   : ������ ����Ʈ�� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, ����Ʈ�ε���
return : -1 ������ ã���� ����, -2 ������ ����Ʈ�� ������ ���� ����, -1 ���� ũ�� ����Ʈ�� ������ ���� ( ����Ʈ ������ ���� )
*/
int api_quest_UserHasQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : ������ ��� ����Ʈ�� ���������� Ȯ���Ѵ�.
param  : �����ε���, ����Ʈ�ε���
return : -1 ������ ã���� ����, �׿ܿ� ���� �������� ����Ʈ ���� ����..
*/
int api_quest_GetPlayingQuestCnt(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : npc �ε����� �����Ѵ�.
param  : �����ε���, NpcObjId ( npc �ε����� npcobject �ε����� �ٸ� ��. )
return : -1 ������ ã���� ����, -2 npc �� ã���� ����, �׿ܿ��� npc�ε���
*/
int api_npc_GetNpcIndex(CDNGameRoom *pRoom, UINT nUserObjectID, UINT nNpcObjID);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ����, ���� ������ �����Ѵ�.
param  : �����ε���, ����Ʈ�ε���, ���� ����
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����.  -3 ���� ���� ������ ���. ������ ��� 1�� ����
*/
int api_quest_SetQuestStepAndJournalStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, short nQuestStep, int nJournalStep);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ����Ʈ ������ �����Ѵ�.
param  : �����ε���, ����Ʈ�ε���, ����Ʈ ����
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����. ������ ��� 1�� ����
*/
int api_quest_SetQuestStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, short nQuestStep);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ����Ʈ ������ ��´�.
param  : �����ε���, ����Ʈ�ε���
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����., -1 ���� ū��� ���� ����Ʈ�� ����
*/
int api_quest_GetQuestStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ���� ������ �����Ѵ�.
param  : �����ε���, ����Ʈ�ε���, ���� ����
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����.  -3 ���� ���� ������ ���. ������ ��� 1�� ����
*/
int api_quest_SetJournalStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nJournalStep);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ���� ������ ��´�.
param  : �����ε���, ����Ʈ�ε���
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����., -1 ���� ū��� ���� ���� ����
*/
int api_quest_GetJournalStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� �����Ѵ�.
param  : �����ε���, �޸��ε���, �޸�
return : -1 => ������ ã���� ����. -2 => ������ ����Ʈ�� ������ ���� ����. -3 �޸� �ε��� ������ ���. ������ ��� 1�� ����
*/
int api_quest_SetQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex, int iVal);

/*
desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� ��´�.
param  : �����ε���, �޸��ε���
return : -1 => ������ ã���� ����. -2 => ������ ����Ʈ�� ������ ���� ����. -3 �޸� �ε��� ������ ���. -1 ���� ū��� Ư�� �޸� �ε����� ��
*/
int api_quest_GetQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : ���� ��ų�, ������ ��� ���� ī���� ������ �����ϴ� �Լ� �̴�.
param  : �����ε���, 
		 ����Ʈ�ε���, 
		 ī�������� ���� ����(0���� 4���� 5���� ���ý����� ����) , 
		 ī������ Ÿ��(	CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3, CountingType_SymbolItem = 4),
		 ī������ ���� �ε���, 
		 ī������ ���� 
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����. -3 => ī�����������ý��� ���� �ʰ� -4 => ī����Ÿ�� or Ÿ��ī��Ʈ ���� �ʰ�

example: 

api_quest_SetCountingInfo(�������̵�, ����Ʈ���̵�, 0, CountingType_MyMonsterKill, 10001, 10);
api_quest_SetCountingInfo(�������̵�, ����Ʈ���̵�, 1, CountingType_MyMonsterKill, 10002,  5);
api_quest_SetCountingInfo(�������̵�, ����Ʈ���̵�, 2, CountingType_MyMonsterKill, 10003, 20);
api_quest_SetCountingInfo(�������̵�, ����Ʈ���̵�, 3, Item,	   1001,  5);
api_quest_SetCountingInfo(�������̵�, ����Ʈ���̵�, 4, Item,     1002,  1);

���� ���� ��ũ��Ʈ���� ȣ�� ������ ��� 5���� ī���� ������ ���� �ǰ� �ȴ�. 
�� ������ ���� 0�� ���Կ� 10001�� ���͸� 10���� ������ ������ �����Ѵٰ� �����Ѵ�.

���Ͱ��� 5���� ���Կ� 10001�� ���� 10����, 10002�� ���� 5����, 10003�� ���� 20����, 1001�� ������ 5��, 1002�� ������ 1�� ��� ���� �Ͽ���
�� 5���� ������ �����ϰ� �Ǹ�. ����Ʈ ��ũ��Ʈ�� OnComplete() �Լ��� ȣ���ϰ� �ȴ�.

*** ī���� ������ �ִ� 30000 ���̴�.
*/
int api_quest_SetCountingInfo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID,  int nSlot, int nCountingType, int nCountingIndex, int nTargetCnt); 


/*
desc   : ���õ� ī���� ������ Ŭ���� �Ѵ�. 
param  : �����ε���, ����Ʈ �ε���
return : -1 => ������ ã���� ���� -2 => ����Ʈ�� ã���� ����,
*/
int api_quest_ClearCountingInfo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : ���õ� ī���õ��� ��� �Ϸ� �Ǿ����� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => ����Ʈ�� ã���� ����,  -3�̸� �Ϸ� �ȵ�, 1 �̸� ��� �Ϸ�
*/
int api_quest_IsAllCompleteCounting(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID);

/*
desc   : �������� �������� �������� �κ������� �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����
*/
int api_user_CheckInvenForAddItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : �������� ����Ʈ ������ ����� �������� �κ������� �ִ��� Ȯ���Ѵ�.
param  : �����ε���, ����Ʈ ������ ��� (������̺�, [<�ִ밳��>]{����Ʈ �������ε���, ����Ʈ ������ ī��Ʈ} �� �� ������ <�ִ밳�� (���� 10��)> ��ŭ ����)
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����

example :

function grandmother_OnTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)

	local TableItem =
	{
		{ 1001, 10 },		-- ������ �ε��� (1001), ������ ���� (10)
		{ 1002, 1 },		-- ������ �ε��� (1002), ������ ���� (1)
		{ 1003, 5 },		-- ������ �ε��� (1003), ������ ���� (5)
	};

	if (api_user_CheckInvenForAddItemList(userObjID, TableParam) == 1) then
		-- ����
	else
		-- ����
	end
end
*/
int api_user_CheckInvenForAddItemList(CDNGameRoom *pRoom, UINT nUserObjectID, lua_tinker::table ItemTable);

/*
desc   : �������� �������� �����Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ, ����Ʈ ���̵�
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����
*/
int api_user_AddItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID);

/*
desc   : �������Լ� �������� �����.
param  : �����ε���, �������ε���, ������ ī��Ʈ, ����Ʈ ���̵�
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����
*/
int api_user_DelItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID);

/*
desc   : �������Լ� �������� ��� �����.
param  : �����ε���, �������ε���
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����
*/
int api_user_AllDelItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex);

/*
desc   : ������ �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_user_HasItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);


/*
desc   : ������ Ŭ���� ID �� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� ClasssID
*/
int api_user_GetUserClassID(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : ������ Job ID �� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� Job ID
*/
int api_user_GetUserJobID(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : ������ level�� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� Level
*/
int api_user_GetUserLevel(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : ���� �κ��丮�� ��ĭ���� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� ��ĭ��
*/
int api_user_GetUserInvenBlankCount(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : ���� ����Ʈ �κ��丮�� ��ĭ���� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� ��ĭ��
*/
int api_quest_GetUserQuestInvenBlankCount(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �������� ����ġ�� �߰��Ѵ�.
param  : �����ε���, ����ġ
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_user_AddExp(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nAddExp);

/*
desc   : �������� ���� �߰��Ѵ�.
param  : �����ε���, ��
return : -1 => ������ ã���� ����,  1�̸� ����
*/
int api_user_AddCoin(CDNGameRoom *pRoom, UINT nUserObjectID, int nAddCoin, int nQuestID);


/*
desc   : �������� �������� ��� ��Ų��.
param  : �����ε���, �ƾ����̺�ID
return : -1 => ������ ã���� ����,  1�̸� ����
*/
int api_user_PlayCutScene(CDNGameRoom *pRoom, UINT nUserObjectID, UINT nNpcObjectID, int nCutSceneTableID, bool bIgnoreFadeIn);


/*
desc   : �������� â�� ����� �˸���.
param  : �����ε���
return : -1 => ������ ã���� ����,  1�̸� ����
*/
int api_ui_OpenWareHouse(CDNGameRoom *pRoom, UINT nUserObjectID, int iItemID=0 );

/*
desc   : �������� ������ ����� �˸���.
param  : �����ε���, ���� ID
return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����
*/
int api_ui_OpenShop(CDNGameRoom *pRoom, UINT nUserObjectID, int nShopID, Shop::Type::eCode Type = Shop::Type::Normal);

/*
desc   : �������� ��ų ������ ����� �˸���.
param  : �����ε���, ��ų ���� ID
return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����
*/
int api_ui_OpenSkillShop(CDNGameRoom *pRoom, UINT nUserObjectID, int nSkillShopID);


/*
desc   : �������� ���λ����� ����� �˸���.
param  : �����ε���
return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����
*/
int api_ui_OpenMarket(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �������� ���庸�� �ռ� â�� ����� �Ѵ�.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenCompoundEmblem(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : �������� ���� ���׷��̵� â�� ����� �˸���.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenUpgradeJewel(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : �������� �������� ����� ��Ų��.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenMailBox(CDNGameRoom *pRoom, UINT nUserObjectID);


/*
desc   : �������� ����â�� ����� ��Ų��.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenDisjointItem(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �������� ������ �ռ�â�� ����� ��Ų��.
param  : �����ε���, ������ �ռ� ���� ID
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenCompoundItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nCompoundShopID);

int api_ui_OpenCompound2Item(CDNGameRoom *pRoom, UINT nUserObjectID, int nCompoundShopID, int iItemID/*=0*/ );

/*
desc   : �������� ĳ�� ������ ������ ����� ��Ų��.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenCashShop(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �������� ��� ���� UI ��ȭ���ڸ� ����� ��Ų��.
param  : �����ε���, ��������ȣ
return : -1 => ������ ã���� ����, 1�̸� ����

��������ȣ>
	0 : ���â��
	1 : ����ػ�
	2 : ���Ż��
	3 : �������Ʈ
	4 : ��巹����
*/
int api_ui_OpenGuildMgrBox(CDNGameRoom *pRoom, UINT nUserObjectID, int nGuildMgrNo);

/*
desc   : �Ϻ� ��í�� ���̾�α׸� ����.
param  : ���� ��í�� �ε���
return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����, -3 �̸� �����ο� �ɷ���..
*/
int api_ui_OpenGacha_JP(CDNGameRoom *pRoom,  UINT nUserObjectID, int nGachaShopID);

/*
desc   : NPC �����ϱ� ���̾�α׸� ����.
param  : ��ȭ���� Npc ID 
return : -1 => ������ ã���� ����. 1 �̸� ����.
*/
int api_ui_OpenGiveNpcPresent(CDNGameRoom* pRoom, UINT nUserObjectID, int nNpcID);


/*
desc   : �������� ����â�� ����� �˸���.
param  : �����ε���
return : -1 => ������ ã���� ����,  1�̸� ����
*/
int api_ui_OpenFarmWareHouse(CDNGameRoom *pRoom, UINT nUserObjectID);


#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
/*
desc   : �ؽ��� ���̾�α׸� ����.
param  : �����ε���, �����ε���, ȭ�鿡���� ��ġ, ������ �ð�, �̹��� �߾���,
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenTextureDialog(CDNGameRoom* pRoom, UINT nUserObjectID, int nFileIndex, float fX, float fY, int nPos, int nTime, int nDialogIndex, bool bAutoCloseDialog);


/*
desc   : �ؽ��� ���̾�α׸� �ݴ´�.
param  : �����ε���, ���̾�α� ID
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_CloseTextureDialog(CDNGameRoom* pRoom, UINT nUserObjectID, int nDlgID);
#endif


/*
desc   : �������� �޼����� ����ϰ� �Ѵ�.
param  : nType �� �޼��� ��� Ÿ��, 
CHATTYPE_NORMAL   => 1,	// �Ϲ����� ��ȭ
CHATTYPE_PARTY    => 2, // ��Ƽ ��ȭ
CHATTYPE_PRIVATE  => 3,	// �ӼӸ�
CHATTYPE_GUILD    => 4,	// ��� ��ȭ
CHATTYPE_CHANNEL  => 5,	// �ŷ�/ä�� ��ȭ
CHATTYPE_SYSTEM   => 6, // �ý��� �޼���

�α� ��Ʈ�� �ٹٲ��� ���� \n �� �����Ұ�.
return : -1 => ������ ã���� ����, -2 Ÿ���� ���� -3 �޼����� �ʹ� ��
*/
int api_user_UserMessage(CDNGameRoom *pRoom, UINT nUserObjectID, int nType, int nBaseStringIdx, lua_tinker::table ParamTable);

/*
desc   : �������� �ɺ��������� �߰��Ѵ�.
param  : �����ε���,  �ɺ������۾��̵�, ����
return : -1 => ������ ã���� ����, -2�� ���� 1�̸� ����
*/
int api_quest_AddSymbolItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount);

/*
desc   : �������� �ɺ��������� �����Ѵ�.
param  : �����ε���,  �ɺ������۾��̵�, ����
return : -1 => ������ ã���� ����, -2 �� ����, 1�̸� ����
*/
int api_quest_DelSymbolItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount);

/*
desc   : ������ �ɺ� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_quest_HasSymbolItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : �������� ����Ʈ �������� �������� �κ������� �ִ��� Ȯ���Ѵ�.
param  : �����ε���, ����Ʈ ������ �ε���, ����Ʈ ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����
*/
int api_quest_CheckQuestInvenForAddItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : �������� ����Ʈ ������ ����� �������� �κ������� �ִ��� Ȯ���Ѵ�.
param  : �����ε���, ����Ʈ ������ ��� (������̺�, [<�ִ밳��>]{����Ʈ �������ε���, ����Ʈ ������ ī��Ʈ} �� �� ������ <�ִ밳�� (���� 10��)> ��ŭ ����)
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����

example :

function grandmother_OnTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)

	local TableItem =
	{
		{ 1001, 10 },		-- ������ �ε��� (1001), ������ ���� (10)
		{ 1002, 1 },		-- ������ �ε��� (1002), ������ ���� (1)
		{ 1003, 5 },		-- ������ �ε��� (1003), ������ ���� (5)
	};

	if (api_quest_CheckQuestInvenForAddItemList(userObjID, TableParam) == 1) then
		-- ����
	else
		-- ����
	end
end
*/
int api_quest_CheckQuestInvenForAddItemList(CDNGameRoom *pRoom, UINT nUserObjectID, lua_tinker::table ItemTable);

/*
desc   : �������� ����Ʈ �������� �߰��Ѵ�.
param  : �����ε���,  ����Ʈ�����۾��̵�, ����, ����Ʈ ���̵�
return : -1 => ������ ã���� ����, -2�� ���� 1�̸� ����
*/
int api_quest_AddQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount, int nQuestID);

/*
desc   : �������� ����Ʈ �������� �����Ѵ�.
param  : �����ε���,  ����Ʈ�����۾��̵�, ����, ����Ʈ ���̵�
return : -1 => ������ ã���� ����, -2 �� ����, 1�̸� ����
*/
int api_quest_DelQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount, int nQuestID);

/*
desc   : �������� ����Ʈ �������� ��� �����Ѵ�.
param  : �����ε���,  ����Ʈ�����۾��̵�
return : -1 => ������ ã���� ����, -2 �� ����, 1�̸� ����
*/
int api_quest_AllDelQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID );

/*
desc   : ������ ����Ʈ �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_quest_HasQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : �������� ��� ����â�� ����� �˸���.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenGuildCreate(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �������� ����Ʈ ����â�� ����� �˸���.
param  : �����ε���, �������̺�, bActivate �� true �� ���� �ְ� �̸� �׳� ���� �뵵�� false�� ����.
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenQuestReward(CDNGameRoom *pRoom, UINT nUserObjectID, int nRewardTableIndex, bool bActivate);

/*
desc   : �������� ����Ʈ ������ �Ѵ�. 
param  : �����ε���, �������̺�, ����Ʈ ���̵�, ����üũ��ȣ
return : -1 => ������ ã���� ����, -2 ���������۰�ü ����, -3 �κ� ����, -4 �̸� ���̺��� ã���� ����, -5 ���� ���� üũ ����, 1 �̸� ����
*/
int api_quest_RewardQuestUser(CDNGameRoom *pRoom, UINT nUserObjectID, int nRewardTableIndext, int nQuestID, int nRewardCheck);

/*
desc   : �ش� ������ ��Ƽ������ �����Ѵ�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� ��Ƽ���� (���� !!!> ��Ƽ�� ���Ե��� ���� ����ڵ� 1 �� ��ȯ)
*/
int api_user_GetPartymemberCount(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �ش� ������ ��Ƽ�� ���Ե� �������� üũ�Ѵ�.
param  : �����ε���
return : -1 => ������ ã���� ����, 0 => ��Ƽ�� ���� �ʵǾ�����, 1 => ��Ƽ�� ���� �Ǿ�����
*/
int api_user_IsPartymember(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : ������ �̵���Ų��.
param  : �����ε���, �̵��� �� �ε���, �̵��� ����Ʈ �ε���
return : -1 => ������ ã���� ����, -2 ���� ����
*/
void api_user_ChangeMap(CDNGameRoom *pRoom, UINT nUserObjectID, int nMapIndex, int nGateNo);

/*
desc   : �ش� ������ �������� ���̵��� ����
param  : �����ε���
return : 0 ~ 4 => ���� ���̵� (0:���� / 1:���� / 2:����� / 3:������ : 4:���), �� �ܴ� ���� (�⺻ -1)
*/
int api_user_GetStageConstructionLevel(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �ش� ������ ���� �ʹ�ȣ�� ����
param  : �����ε���
return : �ʹ�ȣ (1 �̻�), �� �ܴ� ���� ? (0 ����)
*/
int api_user_GetMapIndex(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �ش� ������ �ֱ� �������� �Ϸ� ��ũ�� ����
param  : �����ε���
return : �������� �Ϸ� ��ũ (SSS : 0 / SS : 1 / S : 2 / A : 3 / B : 4 / C : 5 / D : 6), �� �ܴ� ���� ? (�⺻ -1)
*/
int api_user_GetLastStageClearRank(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : ������ ���� �������� Ȯ���Ѵ�.
param  : �����ε���, üũ�� ��
return : -1 => ������ ã�� �� ����, -2 => �������� ������, 1 => �������� �����
*/
int api_user_EnoughCoin(CDNGameRoom *pRoom, UINT nUserObjectID, int nCoin );

/*
desc   : ������ ���� �������� Ȯ���Ѵ�.
param  : �����ε���
return : -1 => ������ ã���� ����, 0 �̻� => ���� ������
*/
int api_user_GetCoin(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : �������� ���� �����Ѵ�.
param  : �����ε���, �� (0 �ʰ�)
return : -1 => ������ ã���� ����, 1 => ����
*/
int api_user_DelCoin(CDNGameRoom *pRoom, UINT nUserObjectID, int nDelCoin);

/*
desc   : ������ Ư�� �̼� ��� ���θ� ��ȯ
param  : �����ε���, �̼��ε��� (MISSIONMAX (���� 5000 �� 0 ~ 4999))
return : -1 => ������ ã���� ����, 0 => �ش� �̼� ����, 1 ���� �̿� => �ش� �̼� ����
*/
int api_user_IsMissionGained(CDNGameRoom *pRoom, UINT nUserObjectID, int nMissionIndex);

/*
desc   : ������ Ư�� �̼� �Ϸ� ���θ� ��ȯ
param  : �����ε���, �̼��ε��� (MISSIONMAX (���� 5000 �� 0 ~ 4999))
return : -1 => ������ ã���� ����, 0 => �ش� �̼� ����, 1 ���� �̿� => �ش� �̼� ����
*/
int api_user_IsMissionAchieved(CDNGameRoom *pRoom, UINT nUserObjectID, int nMissionIndex);

/*
desc   : ������ â�� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_user_HasItemWarehouse(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : ������ ��� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, 1 ���� �׿� true
*/
int api_user_HasItemEquip(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex);

/*
desc   : ������ ���� ��� ������ ��ȯ
param  : �����ε���
return : 0 : ��� ���� / 1 : ����� / 2 : �α���� / 3 : ���ӱ��� / 4 : �Ϲݱ��� / 5 : ���Ա���
*/
int api_guild_GetGuildMemberRole(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� 1 ������Ŵ
param  : �����ε���, ����Ʈ��ȣ, �޸��ε���
return : ������ ����Ʈ �޸� (������ ��쿡�� LONG_MIN ��ȯ)
*/
int api_quest_IncQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� 1 ���ҽ�Ŵ
param  : �����ε���, ����Ʈ��ȣ, �޸��ε���
return : ������ ����Ʈ �޸� (������ ��쿡�� LONG_MAX ��ȯ)
*/
int api_quest_DecQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : �������� Ư�� ���� ������ ����
param  : �����ε���, ���� ���� ����
return : -1 : ������ ã���� ���� / -2 : ���� ���� ��ȣ�� / -3 : ���� �Ұ� ������ / 0 : �̹� ���� ���� ������ / 1 : ���� ���� ���� ����
*/
int api_user_SetUserJobID(CDNGameRoom *pRoom, UINT nUserObjectID, int nJobID);

/*
desc   : ������ ���� ������ Ư�� ���� �迭�� �ִ��� Ȯ��
param  : �����ε���, ���� ���� ����
return : -1 : ������ ã���� ���� / 0 : ���� �迭�� �ƴ� / 1 : ���� �迭�� ����
*/
int api_user_IsJobInSameLine(CDNGameRoom *pRoom, UINT nUserObjectID, int nBaseJobID);

/*
desc   : ������ ĳ�� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, ĳ���������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_user_HasCashItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : ������ ��� ĳ�� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, ĳ���������ε���
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, 1 ���� �׿� true
*/
int api_user_HasCashItemEquip(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex);

/*
desc   : Ư�� ī���� Ÿ��, ID �� ���ؼ� ���Ƿ� ī������ ������Ų��.
param  : �����ε���, 
		����Ʈ�ε���, 
		ī������ Ÿ�� (CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3),
		ī������ ���� �ε��� (������ ID, ���� ID ���)
return : -1 => ������ ã���� ����, 1 => ����
*/
int api_quest_IncCounting(CDNGameRoom *pRoom, UINT nUserObjectID, int nCountingType, int nCountingIndex);

#if defined(PRE_ADD_DAILYQUESTDATEAPI_01) && defined(PRE_ADD_QUEST_DAILYCHECK_01)
/*
desc   : ���� ����Ʈ �������� ���� ��¥�� ���´�.
param  : �����ε���
return : -1 => ������ ã���� ����, 1 => ����
*/
int api_quest_GetDailyQuestDay(CDNGameRoom *pRoom, UINT nUserObjectID);
#endif	// #if defined(PRE_ADD_DAILYQUESTDATEAPI_01) && defined(PRE_ADD_QUEST_DAILYCHECK_01)

/*
desc   : ������ ���� ���� �ִ� ����Ʈ ���� �����Ͽ����� ���θ� ��ȯ�Ѵ�.
param  : �����ε���, ����Ʈ�ε���
return : -1 : ������ ã���� ����, 1 : �ִ� ����Ʈ ���� ���� ��, �׿� : �ƴ�
*/
int api_quest_IsPlayingQuestMaximum(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : Ư�� ����Ʈ�� ���� �Ϸ� ��Ų��. (���� ���� �ƴ� ����Ʈ�� �߰��Ͽ� �Ϸ�)
param  : �����ε���, ����Ʈ�ε���, ����Ʈ �ڵ� (0:�Ϲ� / 1:�ð���), ��ŷ���� (1:��ŷ/0:����), �������� (1:����/0:����), �ݺ����� (1:�ݺ�/0:����)
return : -1 : ������ ã���� ����, 1 : �ִ� ����Ʈ ���� ���� ��, �׿� : �ƴ�
*/
int api_quest_ForceCompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat);

/*
desc   : Ư�� ������� ��ġ�� �̵� ��Ų��.
param  : �����ε���, X ��ǥ, Y ��ǥ, Z ��ǥ
return : -1 : ������ ã���� ����, 1 ����
*/
int api_user_ChangePos(CDNGameRoom *pRoom, UINT nUserObjectID, int nX, int nY, int nZ, int nLookX = 0, int nLookY = 0);

int api_ui_OpenUpgradeItem(CDNGameRoom *pRoom, UINT nUserObjectID,int iItemID=0);

int api_user_RequestEnterPVP(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : NPC ȣ��Point �� ���´�.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_GetFavorPoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ����Point �� ���´�.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_GetMalicePoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ȣ��Percent �� ���´�.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_GetFavorPercent( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ����Percent �� ���´�.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_GetMalicePercent( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ȣ��Point �� �����Ѵ�.
param  : �����ε���, NpcID, ������(- ������ ������)
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_AddFavorPoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID, int val );

/*
desc   : NPC ����Point �� �����Ѵ�.
param  : �����ε���, NpcID, ������(- ������ ������)
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_AddMalicePoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID, int val );

/*
desc   : NPC ���� ���� ���� ������ ������ ������ ������� Ŭ���̾�Ʈ���� ��û.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_SendSelectedPresent( CDNGameRoom* pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ���� ���� ���� ������ ������ ������ ������� Ŭ���̾�Ʈ���� ��û.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_Rage( CDNGameRoom* pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ���� ���� ���� ������ ������ ������ ������� Ŭ���̾�Ʈ���� ��û.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_Disappoint( CDNGameRoom* pRoom, UINT uiUserObjectID, int iNpcID );

/*
desc   : Ư�� ������� ��ü ��ų�� ���½�Ų��.
param  : �����ε���
return : -1 : ������ ã���� ����, 1 : ����
*/
int api_user_ResetSkill(CDNGameRoom *pRoom, UINT nUserObjectID);

/*
desc   : Ư�� ����ڸ� 2�� ���� ��ų�� ����� �� �ִ� ���·� �ӽ÷� ������ش�.
param  : �����ε���
return : -1 : ������ ã���� ����, 1 : ����
*/
int api_user_SetSecondJobSkill( CDNGameRoom *pRoom, UINT nUserObjectID, int iSecondChangeJobID );

/*
desc   : �ӽ÷� 2�� ������ ����� �� �ִ� ���·� ������ ������ ���� ���·� �������´�. 
		 �� api �� ȣ������ �ʾƵ� ���� �̵��ص� ���µȴ�.
param  : �����ε���
return : -1 : ������ ã���� ����, 1 : ����
*/
int api_user_ClearSecondJobSkill( CDNGameRoom *pRoom, UINT nUserObjectID );

/*
desc   : ������ ���� ��� �������� �����ϰ�, ��������� �����մϴ�.
param  : �����ε���, ��ü���� ����(1-��ü����, 0-�����Ѿ�����)
return : -1 : ������ ã���� ���� / 0 : ������ ������ ���� / 1 : ������ ���� / 2 : ������ ���� �Ϸ�
*/
int api_user_RepairItem(CDNGameRoom *pRoom, UINT nUserObjectID, int iAllRepair);


/*
desc   : �������� ��ȭ���� �ٸ� ���������� �̵���ŵ�ϴ�.
param  : �����ε���
return : -1 : ������ ã���� ����, 1�̸� ���� 
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
desc   : EventArea �� NPC �� �����Ѵ�.
param  : �̺�Ʈ���� id , npc id 
return : -1 => �̺�Ʈ���� ã���� ����,  1�̸� ����
*/
int api_trigger_GenerationNpc(CDNGameRoom *pRoom, int nEventAreaHandle, int nNpcTableID);

/*
desc   : EventArea �� NPC �� �����Ѵ�.
param  : �̺�Ʈ���� id
return : -1 => �̺�Ʈ���� ã���� ����,  1�̸� ����
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
desc   : ��Ƽ �� Ư�� Ŭ������ �ο��� üũ
         [Interger1] ���� Ŭ������ �ο��� ��ȯ
param  : ����(Interger1)1 ������ 2 ����, 3 �Ҽ����� 4 Ŭ���� 5 ��ī���� 6 Į��
return : ��Ƽ�� [Interger1] Ŭ������ �ο�
*/
int api_trigger_GetPartyInClassCount( CDNGameRoom* pRoom, int nClassID );

void api_trigger_Set_AlteiaWorld( CDNGameRoom* pRoom, bool bFlag );
void api_tirgger_Set_UserHPSP( CDNGameRoom * pRoom, UINT nActorHandle, int nHP_Ratio, int nSP_Ratio );

void api_trigger_Set_ItemUseCount( CDNGameRoom* pRoom, int nItemID, int nUseCount );	// �������� ������ ���Ƚ�� ����

//rlkt_test
bool api_trigger_CheckJobChangeProcess(CDNGameRoom* pRoom, UINT nActorHandle);
int api_ui_OpenJobChange(CDNGameRoom* pRoom, UINT nUserObjectID);
int api_trigger_Emergence_GetValue(CDNGameRoom* pRoom, UINT nValueType);
int api_trigger_Emergence_Begin(CDNGameRoom* pRoom, int nEventAreaHandle);
bool api_trigger_Emergence_CheckSelectedEvent(CDNGameRoom* pRoom, int nEventAreaHandle);
bool api_trigger_CheckForceMoveComebackRoom(CDNGameRoom* pRoom);