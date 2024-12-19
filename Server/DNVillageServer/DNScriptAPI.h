#pragma once

// VillageServer Script API
//  [4/1/2008 nextome]

class CDNUserBase;

int CheckInvenForQuestReward(CDNUserBase* pUser, const TQuestReward& QuestReward, const bool bSelectedArray[]);

// ��� �Լ��� ��ƿ� ����Ѵ�.
void DefAllAPIFunc(lua_State* pLuaState);

/*
desc   : ���� ���� �����Ѵ�.
param  : �����ε���, NPC�ε���, ��ũ�ε���, ��ũ�����̸�
return : void
*/
void api_npc_NextTalk(UINT nUserObjectID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

/*
desc   : ���� ��ũ��Ʈ�� �����Ѵ�.
param  : �����ε���, NPC�ε���, ��ũ�ε���, ��ũ�����̸�
return : void
*/
void api_npc_NextScript(UINT nUserObjectID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

/*
desc   : ��� �Ķ���͸� �����Ѵ�(��Ʈ��).  
param  : �Ķ����Ű, �Ķ���͹����� �ε���
return : -1 ������ ã���� ����, -2 �Ķ���� ������ �Ѿ�� ���̻� �����Ҽ� ����. 1 �̸� ����
sample : 
			���� {%color} ���� �����մϴ�.  ��� ������ ���鶧.
			api_SetParamString("{%color}, math.random(2) );  �̷������� ������ش�.
			1�� TalkParamTable.xls �� 1 : ����, 2: �Ķ� ���� ��� �Ǿ��־���Ѵ�.

		����: api_SetParamString ���� ������ �Ķ���ʹ� api_NextTalk ȣ���� clear �ǹǷ� api_NextTalk ȣ������ �����Ѵ�.
*/
int api_npc_SetParamString(UINT nUserObjectID, const char* szParamKey, int nParamID);

/*
desc   : ��� �Ķ���͸� �����Ѵ�(����).  
param  : �Ķ����Ű, ������
return : -1 ������ ã���� ����, -2 �Ķ���� ������ �Ѿ�� ���̻� �����Ҽ� ����. 1 �̸� ����
sample : 
			���� {%age} ���뿡 ������ �;��.  ��� ������ ���鶧.
			api_SetParamInt("{%age}, math.random(30)+40 );  �̷������� ������ش�.

		����: api_SetParamInt ���� ������ �Ķ���ʹ� api_NextTalk ȣ���� clear �ǹǷ� api_NextTalk ȣ������ �����Ѵ�.
*/
int api_npc_SetParamInt(UINT nUserObjectID, const char* szParamKey, int nValue);


/*
desc   : �α׸� �����.
param  : �α� ��Ʈ�� �ٹٲ��� ���� \n �� �����Ұ�.
return : void
*/
void api_log_AddLog(const char* szLog);

/*
desc   : �������� �α׸� �����. ����ä�ø޼����� ��������.
param  : �α� ��Ʈ�� �ٹٲ��� ���� \n �� �����Ұ�.
return : void
*/
void api_log_UserLog(UINT nUserObjectID, const char* szLog);

/*
desc   : �������� ����Ʈ ���� ������ ä������ �����ش�.
param  : 
return : void
*/
void api_quest_DumpQuest(UINT nUserObjectID);


/*
desc   : �������� ����Ʈ�� �ο� �Ѵ�.
param  : �����ε���, ����Ʈ�ε���, ����Ʈ Ÿ�� QuestType_SubQuest = 1, QuestType_MainQuest = 2,
return : 1 : ����, -1 : ������ ã���� ����, -2 : ������ ����Ʈ�� ������ ���� ����
*/
int api_quest_AddHuntingQuest(UINT nUserObjectID, int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt);
int api_quest_AddQuest(UINT nUserObjectID, int nQuestID, int nQuestType);

/*
desc   : �������� ����Ʈ�� �Ϸ� ��Ų��.
param  : �����ε���, ����Ʈ�ε���, ���� ������ �ִ� ����Ʈ���� ��������� (������� �÷��� Ƚ���� ���� ��Ų��.), �ݺ� ���� ����
return : 1 : ����, -1 : ������ ã���� ����, -2 : ������ ����Ʈ�� ������ ���� ����
*/
int api_quest_CompleteQuest(UINT nUserObjectID, int nQuestID, bool bDelPlayList, bool bRepeat);

/*
desc   : �������� ����Ʈ�� �Ϸ��Ͽ� �߰� ��Ų��. ( DB �� ���!! ) 
param  : �����ε���, ����Ʈ�ε���
return : -1 ������ ã���� ����, -2 ����Ʈ�� �������� �ƴ� 
*/
int api_quest_MarkingCompleteQuest(UINT nUserObjectID, int nQuestID);

/*
desc   : �Ϸ��Ͽ� �ϷḶŷ�� �Ǿ��ִ� ����Ʈ����?
param  : �����ε���, ����Ʈ�ε���
return : -1 ������ ã���� ����, 1 �̸� �ϷḶŷ�� ����Ʈ, 0 �̸� �ϷḶŷ �ȵ�
*/
int api_quest_IsMarkingCompleteQuest(UINT nUserObjectID, int nQuestID);




/*
desc   : ������ ����Ʈ�� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, ����Ʈ�ε���
return : -1 ������ ã���� ����, -2 ������ ����Ʈ�� ������ ���� ����, -1 ���� ũ�� ����Ʈ�� ������ ���� ( ����Ʈ ������ ���� )
*/
int api_quest_UserHasQuest(UINT nUserObjectID, int nQuestID);

/*
desc   : ������ ��� ����Ʈ�� ���������� Ȯ���Ѵ�.
param  : �����ε���, ����Ʈ�ε���
return : -1 ������ ã���� ����, �׿ܿ� ���� �������� ����Ʈ ���� ����..
*/
int api_quest_GetPlayingQuestCnt(UINT nUserObjectID);


/*
desc   : npc �ε����� �����Ѵ�.
param  : �����ε���, NpcObjId ( npc �ε����� npcobject �ε����� �ٸ� ��. )
return : -1 ������ ã���� ����, -2 npc �� ã���� ����, �׿ܿ��� npc�ε���
*/
int api_npc_GetNpcIndex(UINT nUserObjectID, UINT nNpcObjID);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ����, ���� ������ �����Ѵ�.
param  : �����ε���, ����Ʈ�ε���, ���� ����
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����.  -3 ���� ���� ������ ���. ������ ��� 1�� ����
*/
int api_quest_SetQuestStepAndJournalStep(UINT nUserObjectID, int nQuestID, short nQuestStep, int nJournalStep);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ����Ʈ ������ �����Ѵ�.
param  : �����ε���, ����Ʈ�ε���, ����Ʈ ����
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����. ������ ��� 1�� ����
*/
int api_quest_SetQuestStep(UINT nUserObjectID, int nQuestID, short nQuestStep);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ����Ʈ ������ ��´�.
param  : �����ε���, ����Ʈ�ε���
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����., -1 ���� ū��� ���� ����Ʈ�� ����
*/
int api_quest_GetQuestStep(UINT nUserObjectID, int nQuestID);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ���� ������ �����Ѵ�.
param  : �����ε���, ����Ʈ�ε���, ���� ����
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����.  -3 ���� ���� ������ ���. ������ ��� 1�� ����
*/
int api_quest_SetJournalStep(UINT nUserObjectID, int nQuestID, int nJournalStep);

/*
desc   : ������ ������ �ִ� ����Ʈ�� ���� ������ ��´�.
param  : �����ε���, ����Ʈ�ε���
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����., -1 ���� ū��� ���� ���� ����
*/
int api_quest_GetJournalStep(UINT nUserObjectID, int nQuestID);

/*
desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� �����Ѵ�.
param  : �����ε���, ����Ʈ��ȣ, �޸��ε���, �޸�
return : -1 => ������ ã���� ����. -2 => ������ ����Ʈ�� ������ ���� ����. -3 �޸� �ε��� ������ ���. ������ ��� 1�� ����
*/
int api_quest_SetQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex, TP_QUESTMEMO nVal);

/*
desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� ��´�.
param  : �����ε���, ����Ʈ��ȣ, �޸��ε���
return : -1 => ������ ã���� ����. -2 => ������ ����Ʈ�� ������ ���� ����. -3 �޸� �ε��� ������ ���. -1 ���� ū��� Ư�� �޸� �ε����� ��
*/
int api_quest_GetQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : ���� ��ų�, ������ ��� ���� ī���� ������ �����ϴ� �Լ� �̴�.
param  : �����ε���, 
		����Ʈ�ε���, 
		ī�������� ���� ����(0���� 49���� 50���� ���ý����� ����) , 
		ī������ Ÿ��(	CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3),
		ī������ ���� �ε��� 0 ~ 60000 ������ ����, 
		ī������ ���� 
return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����. -3 => ī�����������ý��� ���� �ʰ� -4 => ī����Ÿ�� or Ÿ��ī��Ʈ ���� �ʰ�

example: 

	api_SetCountingInfo(�������̵�, ����Ʈ���̵�, 0, CountingType_MyMonsterKill, 10001, 10);
	api_SetCountingInfo(�������̵�, ����Ʈ���̵�, 1, CountingType_MyMonsterKill, 10002,  5);
	api_SetCountingInfo(�������̵�, ����Ʈ���̵�, 2, CountingType_MyMonsterKill, 10003, 20);
	api_SetCountingInfo(�������̵�, ����Ʈ���̵�, 3, Item,	   1001,  5);
	api_SetCountingInfo(�������̵�, ����Ʈ���̵�, 4, Item,     1002,  1);

	���� ���� ��ũ��Ʈ���� ȣ�� ������ ��� 5���� ī���� ������ ���� �ǰ� �ȴ�. 
	�� ������ ���� 0�� ���Կ� 10001�� ���͸� 10���� ������ ������ �����Ѵٰ� �����Ѵ�.

	���Ͱ��� 5���� ���Կ� 10001�� ���� 10����, 10002�� ���� 5����, 10003�� ���� 20����, 1001�� ������ 5��, 1002�� ������ 1�� ��� ���� �Ͽ���
	�� 5���� ������ �����ϰ� �Ǹ�. ����Ʈ ��ũ��Ʈ�� OnComplete() �Լ��� ȣ���ϰ� �ȴ�.

*** ī���� ������ �ִ� 30000 ���̴�.
*/
int api_quest_SetCountingInfo(UINT nUserObjectID, int nQuestID,  int nSlot, int nCountingType, int nCountingIndex, int nTargetCnt); 


/*
desc   : ���õ� ī���� ������ Ŭ���� �Ѵ�. 
param  : �����ε���, ����Ʈ �ε���
return : -1 => ������ ã���� ���� -2 => ����Ʈ�� ã���� ����,
*/
int api_quest_ClearCountingInfo(UINT nUserObjectID, int nQuestID);

/*
desc   : ���õ� ī���õ��� ��� �Ϸ� �Ǿ����� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => ����Ʈ�� ã���� ����,  -3�̸� �Ϸ� �ȵ�, 1 �̸� ��� �Ϸ�
*/
int api_quest_IsAllCompleteCounting(UINT nUserObjectID, int nQuestID);

/*
desc   : ��������Ʈ�� ĵ����Ų��.
param  : �����ε���,  ����Ʈ ���̵�
return : -1 => ������ ã���� ����, -2�� ���� 1�̸� ����
 */
int api_quest_CancelQuest(UINT nUserObjectID, int nQuestID);


/*
desc   : �������� �������� �������� �κ������� �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����
*/
int api_user_CheckInvenForAddItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

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
int api_user_CheckInvenForAddItemList(UINT nUserObjectID, lua_tinker::table ItemTable);

/*
desc   : �������� �������� �����Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ, ����Ʈ ���̵�
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����
*/
int api_user_AddItem(UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID);

/*
desc   : �������Լ� �������� �����.
param  : �����ε���, �������ε���, ������ ī��Ʈ, ����Ʈ ���̵�
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����
*/
int api_user_DelItem(UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID);

/*
desc   : �������Լ� �������� ��� �����.
param  : �����ε���, �������ε���
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����
*/
int api_user_AllDelItem(UINT nUserObjectID, int nItemIndex);

/*
desc   : ������ �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_user_HasItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);


/*
desc   : ������ Ŭ���� ID �� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� ClasssID
*/
int api_user_GetUserClassID(UINT nUserObjectID);


/*
desc   : ������ Job ID �� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� Job ID
*/
int api_user_GetUserJobID(UINT nUserObjectID);


/*
desc   : ������ level�� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� Level
*/
int api_user_GetUserLevel(UINT nUserObjectID);

/*
desc   : ���� �κ��丮�� ��ĭ���� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� ��ĭ��
*/
int api_user_GetUserInvenBlankCount(UINT nUserObjectID);

/*
desc   : ���� ����Ʈ �κ��丮�� ��ĭ���� ��´�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� ��ĭ��
*/
int api_quest_GetUserQuestInvenBlankCount(UINT nUserObjectID);

/*
desc   : �������� ����ġ�� �߰��Ѵ�.
param  : �����ε���, ����ġ
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_user_AddExp(UINT nUserObjectID, int nQuestID, int nAddExp);

/*
desc   : �������� ���� �߰��Ѵ�.
param  : �����ε���, ��
return : -1 => ������ ã���� ����,  1�̸� ����
*/
int api_user_AddCoin(UINT nUserObjectID, int nAddCoin, int nQuestID);

/*
desc   : �������� �������� ��� ��Ų��.
param  : �����ε���, �����������̸�
return : -1 => ������ ã���� ����,  1�̸� ����
*/
int api_user_PlayCutScene(UINT nUserObjectID, UINT nNpcObjectID, int nCutSceneTableID, bool bIgnoreFadeIn);

/*
desc   : �������� â�� ����� �˸���.
param  : �����ε���
return : -1 => ������ ã���� ����,  1�̸� ����
*/
int api_ui_OpenWareHouse(UINT nUserObjectID, int iItemID/*=0*/ );

/*
desc   : �������� ������ ����� �˸���.
param  : �����ε���, ���� ID
return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����
*/
int api_ui_OpenShop(UINT nUserObjectID, int nShopID, Shop::Type::eCode Type=Shop::Type::Normal );

/*
desc   : �������� ��ų ������ ����� �˸���.
param  : �����ε���, ��ų ���� ID
return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����
*/
int api_ui_OpenSkillShop(UINT nUserObjectID);

/*
desc   : �������� ���λ����� ����� �˸���.
param  : �����ε���
return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����
*/
int api_ui_OpenMarket(UINT nUserObjectID);

/*
desc   : �������� ���庸�� �ռ� â�� ����� �Ѵ�.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenCompoundEmblem(UINT nUserObjectID);


/*
desc   : �������� ���� ���׷��̵� â�� ����� �˸���.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenUpgradeJewel(UINT nUserObjectID);


/*
desc   : �������� �������� ����� ��Ų��.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenMailBox(UINT nUserObjectID);

/*
desc   : �������� ���� ������ ���� �������� ����� ��Ų��.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenGlyphLift(UINT nUserObjectID);

/*
desc   : �������� ������ ��ȭâ�� ����� ��Ų��.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenUpgradeItem(UINT nUserObjectID,int iItemID=0);

#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
/*
desc   : �ؽ��� ���̾�α׸� ����.
param  : �����ε���, �����ε���, ȭ�鿡���� ��ġ, ������ �ð�, �̹��� �߾���,
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenTextureDialog(UINT nUserObjectID, int nFileIndex, float fX, float fY, int nPos, int nTime, int nDialogIndex, bool bAutoCloseDialog);

/*
desc   : �ؽ��� ���̾�α׸� �ݴ´�.
param  : �����ε���, ���̾�α� ID
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_CloseTextureDialog(UINT nUserObjectID, int nDlgID);
#endif


int api_ui_OpenDisjointItem(UINT nUserObjectID);


/*
desc   : �������� ������ �ռ�â�� ����� ��Ų��.
param  : �����ε���, ������ �ռ� ���� ID
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenCompoundItem(UINT nUserObjectID, int nCompoundShopID);

/*
desc   : �������� ������ �ռ�â2�� ����� ��Ų��.
param  : �����ε���, ������ �ռ� ���� ID
return : -1 => ������ ã���� ����, 1�̸� ����
*/

int api_ui_OpenCompound2Item(UINT nUserObjectID, int nCompoundGroupID, int iItemID/*=0*/ );

/*
desc   : �������� ĳ�� ������ ������ ����� ��Ų��.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenCashShop(UINT nUserObjectID);


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
int api_ui_OpenGuildMgrBox(UINT nUserObjectID, int nGuildMgrNo);

/*
desc   : �Ϻ� ��í�� ���̾�α׸� ����.
param  : ���� ��í�� �ε���
return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����, -3 �̸� �����ο� �ɷ���..
*/
int api_ui_OpenGacha_JP( UINT nUserObjectID, int nGachaShopID );

/*
desc   : NPC �����ϱ� ���̾�α׸� ����.
param  : ��ȭ���� Npc ID 
return : -1 => ������ ã���� ����. 1 �̸� ����.
*/
int api_ui_OpenGiveNpcPresent( UINT nUserObjectID, int nNpcID );

/*
desc   : ��ũ���ŷ�Խ��� ���̾�α׸� ����.
param  : ��ȭ���� Npc ID 
return : -1 => ������ ã���� ����. 1 �̸� ����.
*/
int api_ui_OpenDarkLairRankBoard( UINT nUserObjectID, int nNpcID, int iMapIndex, int iPlayerCount );


/*
desc   : PvP������ŷ�Խ��� ���̾�α׸� ����.
param  : ��ȭ���� Npc ID 
return : -1 => ������ ã���� ����. 1 �̸� ����.
*/
int api_ui_OpenPvPLadderRankBoard( UINT nUserObjectID, int nNpcID, int iPvPLadderCode );
/*
desc   : �������� �޼����� ����ϰ� �Ѵ�.
param  : nType �� �޼��� ��� Ÿ��, 
		CHATTYPE_NORMAL   => 1,	// �Ϲ����� ��ȭ
		CHATTYPE_PARTY    => 2, // ��Ƽ ��ȭ
		CHATTYPE_PRIVATE  => 3,	// �ӼӸ�
		CHATTYPE_GUILD    => 4,	// ��� ��ȭ
		CHATTYPE_CHANNEL  => 5,	// �ŷ�/ä�� ��ȭ
		CHATTYPE_SYSTEM   => 6, // �ý��� �޼���
		nBaseStringIdx �� �⺻ ��Ʈ��
		ParamTable �� �� Ű������ ġȯ ������ ����ִ� ��� ���̺�
return : -1 => ������ ã���� ����, -2 Ÿ���� ���� -3 �޼����� �ʹ� ��

example :

{%Hitter} �� {item:2}�� ������ä {%Target}�� ���Ƚ��ϴ�.  <-- uistring 6000��
"����_���"	 <-- uistring 6001��
"Ÿ��_�縶��"<-- uistring 6002��

function grandmother_OnTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)

    
	local TableParam =
	{
		// �Ʒ��� ���� {%Hitter} ��� Ű����� uistring.xml �� 6001������ ġȯ �ȴ�. 
		{ "{%Hitter}", 1, 6001 },
		{ "{%Target}", 1, 6002 },
	};

	// ����  { "{%Hitter}", 2, 6001 }, ��� 6001 �̶� ������ ���ڿ� "6001" �� �Ǿ� ġȯ�ȴ�.

	api_UserMessage(userObjID, 1, 6000, TableParam);
end
*/

int api_user_UserMessage(UINT nUserObjectID, int nType, int nBaseStringIdx, lua_tinker::table ParamTable);

/*
desc   : �������� �ɺ��������� �߰��Ѵ�.
param  : �����ε���,  �ɺ������۾��̵�, ����
return : -1 => ������ ã���� ����, -2�� ���� 1�̸� ����
*/
int api_quest_AddSymbolItem(UINT nUserObjectID, int nItemID, short wCount);

/*
desc   : �������� �ɺ��������� �����Ѵ�.
param  : �����ε���,  �ɺ������۾��̵�, ����
return : -1 => ������ ã���� ����, -2 �� ����, 1�̸� ����
*/
int api_quest_DelSymbolItem(UINT nUserObjectID, int nItemID, short wCount);

/*
desc   : ������ �ɺ� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_quest_HasSymbolItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : �������� ����Ʈ �������� �������� �κ������� �ִ��� Ȯ���Ѵ�.
param  : �����ε���, ����Ʈ ������ �ε���, ����Ʈ ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����
*/
int api_quest_CheckQuestInvenForAddItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

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
int api_quest_CheckQuestInvenForAddItemList(UINT nUserObjectID, lua_tinker::table ItemTable);

/*
desc   : �������� ����Ʈ �������� �߰��Ѵ�.
param  : �����ε���,  ����Ʈ�����۾��̵�, ����, ����Ʈ ���̵�
return : -1 => ������ ã���� ����, -2�� ���� 1�̸� ����
*/
int api_quest_AddQuestItem(UINT nUserObjectID, int nItemID, short wCount, int nQuestID);

/*
desc   : �������� ����Ʈ �������� �����Ѵ�.
param  : �����ε���,  ����Ʈ�����۾��̵�, ����, ����Ʈ ���̵�
return : -1 => ������ ã���� ����, -2 �� ����, 1�̸� ����
*/
int api_quest_DelQuestItem(UINT nUserObjectID, int nItemID, short wCount, int nQuestID);

/*
desc   : �������� ����Ʈ �������� ��� �����Ѵ�.
param  : �����ε���,  ����Ʈ�����۾��̵�
return : -1 => ������ ã���� ����, -2 �� ����, 1�̸� ����
*/
int api_quest_AllDelQuestItem(UINT nUserObjectID, int nItemID);

/*
desc   : ������ ����Ʈ �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_quest_HasQuestItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : �������� ��� ����â�� ����� �˸���.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenGuildCreate(UINT nUserObjectID);

/*
desc   : �������� ����Ʈ ����â�� ����� �˸���.
param  : �����ε���, �������̺�, bActivate �� true �� ���� �ְ� �̸� �׳� ���� �뵵�� false�� ����.
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenQuestReward(UINT nUserObjectID, int nRewardTableIndex, bool bActivate);

/*
desc   : �������� ����Ʈ ������ �Ѵ�. 
param  : �����ε���, �������̺�, ����Ʈ ���̵�, ����üũ��ȣ
return : -1 => ������ ã���� ����, -2 ���������۰�ü ����, -3 �κ� ����, -4 �̸� ���̺��� ã���� ����, -5 ���� ���� üũ ����, 1 �̸� ����
*/
int api_quest_RewardQuestUser(UINT nUserObjectID, int nRewardTableIndext, int nQuestID, int nRewardCheck);

/*
desc   : ������ ä���̵���Ų��.
param  : �����ε���, ä��Ÿ��
return : -1 => ������ ã���� ����, -2 ���� ä��Ÿ�� -3 ���� ä��
*/
int api_user_ChangeChannel(UINT nUserObjectID, int ChannelType );

/*
desc   : ������ �̵���Ų��.
param  : �����ε���, �̵��� �� �ε���, �̵��� ����Ʈ �ε���
return : -1 => ������ ã���� ����, -2 ���� ����
*/
int api_user_ChangeMap(UINT nUserObjectID, int nMapIndex, int nGateNo);

/*
desc   : �ش� ������ ��Ƽ������ �����Ѵ�.
param  : �����ε���
return : -1 => ������ ã���� ����, �׿ܴ� ��Ƽ���� (���� !!!> ��Ƽ�� ���Ե��� ���� ����ڵ� 1 �� ��ȯ)
*/
int api_user_GetPartymemberCount(UINT nUserObjectID);

/*
desc   : �ش� ������ ��Ƽ�� ���Ե� �������� üũ�Ѵ�.
param  : �����ε���
return : -1 => ������ ã���� ����, 0 => ��Ƽ�� ���� �ʵǾ�����, 1 => ��Ƽ�� ���� �Ǿ�����
*/
int api_user_IsPartymember(UINT nUserObjectID);

//blondy
/*
desc   : �ش������� PVP�κ�� �ȳ�.
param  : �����ε���
return : 1 ���� -1 => ������ ã���� ����
*/
int api_user_RequestEnterPVP( UINT nUserObjectID );

/*
desc   : �ش� ������ �������� ���̵��� ����
param  : �����ε���
return : 0 ~ 4 => ���� ���̵� (0:���� / 1:���� / 2:����� / 3:������ : 4:���), �� �ܴ� ���� (�⺻ -1)
*/
int api_user_GetStageConstructionLevel(UINT nUserObjectID);

/*
desc   : �ش� ������ ���� �ʹ�ȣ�� ����
param  : �����ε���
return : �ʹ�ȣ (1 �̻�), �� �ܴ� ���� ? (0 ����)
*/
int api_user_GetMapIndex(UINT nUserObjectID);

/*
desc   : �ش� ������ �ֱ� �������� �Ϸ� ��ũ�� ����
param  : �����ε���
return : �������� �Ϸ� ��ũ (SSS : 0 / SS : 1 / S : 2 / A : 3 / B : 4 / C : 5 / D : 6), �� �ܴ� ���� ? (�⺻ -1)
*/
int api_user_GetLastStageClearRank(UINT nUserObjectID);

/*
desc   : ������ ���� �������� Ȯ���Ѵ�.
param  : �����ε���, üũ�� ��
return : -1 => ������ ã�� �� ����, -2 => �������� ������, 1 => �������� �����
*/
int api_user_EnoughCoin(UINT nUserObjectID, int nCoin );

/*
desc   : ������ ���� �������� Ȯ���Ѵ�.
param  : �����ε���
return : -1 => ������ ã���� ����, 0 �̻� => ���� ������
*/
int api_user_GetCoin(UINT nUserObjectID);

/*
desc   : �������� ���� �����Ѵ�.
param  : �����ε���, ��
return : -1 => ������ ã���� ����, 1 => ����
*/
int api_user_DelCoin(UINT nUserObjectID, int nDelCoin);

/*
desc   : ������ Ư�� �̼� ��� ���θ� ��ȯ
param  : �����ε���, �̼��ε��� (MISSIONMAX (���� 5000 �� 0 ~ 4999))
return : -1 => ������ ã���� ����, 0 => �ش� �̼� ����, 1 ���� �̿� => �ش� �̼� ����
*/
int api_user_IsMissionGained(UINT nUserObjectID, int nMissionIndex);

/*
desc   : ������ Ư�� �̼� �Ϸ� ���θ� ��ȯ
param  : �����ε���, �̼��ε��� (MISSIONMAX (���� 5000 �� 0 ~ 4999))
return : -1 => ������ ã���� ����, 0 => �ش� �̼� ����, 1 ���� �̿� => �ش� �̼� ����
*/
int api_user_IsMissionAchieved(UINT nUserObjectID, int nMissionIndex);

/*
desc   : ������ â�� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_user_HasItemWarehouse(UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : ������ ��� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, 1 ���� �׿� true
*/
int api_user_HasItemEquip(UINT nUserObjectID, int nItemIndex);

/*
desc   : ������ ���� ��� ������ ��ȯ
param  : �����ε���
return : -1 : ������ ã���� ���� / 0 : ��� ���� / 1 : ����� / 2 : �α���� / 3 : ���ӱ��� / 4 : �Ϲݱ��� / 5 : ���Ա���
*/
int api_guild_GetGuildMemberRole(UINT nUserObjectID);

/*
desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� 1 ������Ŵ
param  : �����ε���, ����Ʈ��ȣ, �޸��ε���
return : ������ ����Ʈ �޸� (������ ��쿡�� LONG_MIN ��ȯ)
*/
int api_quest_IncQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� 1 ���ҽ�Ŵ
param  : �����ε���, ����Ʈ��ȣ, �޸��ε���
return : ���ҵ� ����Ʈ �޸� (������ ��쿡�� LONG_MAX ��ȯ)
*/
int api_quest_DecQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex);

/*
desc   : �������� Ư�� ���� ������ ����
param  : �����ε���, ���� ���� ����
return : -1 : ������ ã���� ���� / -2 : ���� ���� ��ȣ�� / -3 : ���� �Ұ� ������ / 0 : �̹� ���� ���� ������ / 1 : ���� ���� ���� ����
*/
int api_user_SetUserJobID(UINT nUserObjectID, int nJobID);

/*
desc   : ������ ���� ������ Ư�� ���� �迭�� �ִ��� Ȯ��
param  : �����ε���, ���� ���� ����
return : -1 : ������ ã���� ���� / 0 : ���� �迭�� �ƴ� / 1 : ���� �迭�� ����
*/
int api_user_IsJobInSameLine(UINT nUserObjectID, int nBaseJobID);

/*
desc   : ������ ĳ�� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���, ������ ī��Ʈ
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
*/
int api_user_HasCashItem(UINT nUserObjectID, int nItemIndex, int nItemCnt);

/*
desc   : ������ ��� ĳ�� �������� ������ �ִ��� Ȯ���Ѵ�.
param  : �����ε���, �������ε���
return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, 1 ���� �׿� true
*/
int api_user_HasCashItemEquip(UINT nUserObjectID, int nItemIndex);

/*
desc   : Ư�� ī���� Ÿ��, ID �� ���ؼ� ���Ƿ� ī������ ������Ų��.
param  : �����ε���, 
		ī������ Ÿ�� (CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3),
		ī������ ���� �ε��� (������ ID, ���� ID ���)
return : -1 => ������ ã���� ����, 1 => ����
*/
int api_quest_IncCounting(UINT nUserObjectID, int nCountingType, int nCountingIndex);

/*
desc   : ������ ���� ���� �ִ� ����Ʈ ���� �����Ͽ����� ���θ� ��ȯ�Ѵ�.
param  : �����ε���, ����Ʈ�ε���
return : -1 : ������ ã���� ����, 1 : �ִ� ����Ʈ ���� ���� ��, �׿� : �ƴ�
*/
int api_quest_IsPlayingQuestMaximum(UINT nUserObjectID);

/*
desc   : Ư�� ����Ʈ�� ���� �Ϸ� ��Ų��. (���� ���� �ƴ� ����Ʈ�� �߰��Ͽ� �Ϸ�)
param  : �����ε���, ����Ʈ�ε���, ����Ʈ �ڵ� (0:�Ϲ� / 1:�ð���), ��ŷ���� (1:��ŷ/0:����), �������� (1:����/0:����), �ݺ����� (1:�ݺ�/0:����)
return : -1 : ������ ã���� ����, 1 : �ִ� ����Ʈ ���� ���� ��, �׿� : �ƴ�
*/
int api_quest_ForceCompleteQuest(UINT nUserObjectID, int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat);


/*
desc   : ��ų�� �ʱ�ȭ�Ѵ�
param  : �����ε���, ����Ʈ�ε���, 
return : -1 : ������ ã���� ����, -2 : ����Ʈ�� ã���� ����, -3: �̹� �Ϸ��� ����Ʈ��, 1 : ��ų�ʱ�ȭ ����
*/
int api_quest_ResetSkill(UINT nUserObjectID, int nQuestID);

/*
desc   : �����̺�Ʈ �Ϸὺ�� �˷��ش�.
param  : �����ε���, ������ID
return : -1 : ����, 0: �Ϸ�, 1 : ���۾ȳ�, 2: �����Ⱓ, 3: ����ȳ�, 4: ����Ⱓ
*/
int api_quest_GetWorldEventStep(UINT nUserObjectID, int nScheduleID);

/*
desc   : �����̺�Ʈ �Ϸ�Ƚ�� �˷��ش�.
param  : �����ε���, ������ID
return : -1 : ����, �׿� ���� ī��Ʈ
*/
int api_quest_GetWorldEventCount (UINT nUserObjectID, int nScheduleID);


/*
desc   : �������� NPC���� �ݳ����� ī��Ʈ�� ����Ʈ�� �����Ѵ�.
param  : �����ε���, ������ID
return : -1 : ����, �׿� �ݳ��� ī��Ʈ
*/
int api_quest_ReturnItemToNpc (UINT nUserObjectID, int nScheduleID);

/*
desc   : �������� NPC���� �ݳ��� ���� ī��Ʈ ��Ȳ�� �˷��ش�.
param  : �����ε���, ������ID
*/
void api_quest_OpenScoreWorldEvent (UINT nUserObjectID, int nScheduleID);

/*
desc   : Ư�� ����ڿ��� ���屸�� ����Ʈ�� �����ش�
param  : �����ε���
return : -1 : ������ ã���� ����, 1 ����
*/
int api_quest_OpenFarmAreaList (UINT nUserObjectID);

/*
desc   : Ư�� ������� ��ġ�� �̵� ��Ų��.
param  : �����ε���, X ��ǥ, Y ��ǥ, Z ��ǥ
return : -1 : ������ ã���� ����, 1 ����
*/
int api_user_ChangePos(UINT nUserObjectID, int nX, int nY, int nZ , int nLookX = 0, int nLookY = 0);

/*
desc   : NPC ȣ��Point �� ���´�.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_GetFavorPoint( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ����Point �� ���´�.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_GetMalicePoint( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ȣ��Percent �� ���´�.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_GetFavorPercent( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ����Percent �� ���´�.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_GetMalicePercent( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC ȣ��Point �� �����Ѵ�.
param  : �����ε���, NpcID, ������(- ������ ������)
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_AddFavorPoint( UINT uiUserObjectID, int iNpcID, int val );

/*
desc   : NPC ����Point �� �����Ѵ�.
param  : �����ε���, NpcID, ������(- ������ ������)
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_AddMalicePoint( UINT uiUserObjectID, int iNpcID, int val );

/*
desc   : NPC ���� ���� ���� ������ ������ ������ ������� Ŭ���̾�Ʈ���� ��û.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_SendSelectedPresent( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC �� �г� ����Ʈ�� ����ϵ��� Ŭ���̾�Ʈ���� ��û.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_Rage( UINT uiUserObjectID, int iNpcID );

/*
desc   : NPC �� �Ǹ� ����Ʈ�� ����ϵ��� Ŭ���̾�Ʈ���� ��û.
param  : �����ε���, NpcID
return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
*/
int api_npc_Disappoint( UINT uiUserObjectID, int iNpcID );


/*
desc   : Ư�� ������� ��ü ��ų�� ���½�Ų��.
param  : �����ε���
return : -1 : ������ ã���� ����, 1 : ����
*/
int api_user_ResetSkill(UINT nUserObjectID);



/*
desc   : Ư�� ����ڰ� ���� ��ų�� ���Ӱ� ����.
param  : �����ε���, ������ųID
return : -1 : ������ ã���� ����, 1 : ����, 0 : ����
*/
int api_npc_CreateSecondarySkill( UINT uiObjectUserID, int iSkillID );

/*
desc   : Ư�� ����ڰ� ���� ��ų�� ������� üũ
param  : �����ε���,  ������ųID
return : -1 : ������ ã���� ����, 1 : ����, 0 : ����
*/
int api_npc_CheckSecondarySkill( UINT uiObjectUserID, int iSkillID );



/*
desc	: �������� ���â�� ����� �˸���.
param	: �����ε���
return  : -1 => ������ ã�� �� ����, -2=>������ �ƴ�, 1=>����
*/
int api_ui_OpenGuildWareHouse (UINT nUserObjectID);

/*
desc   : ������ �������� â��������θ� �˷��ش�.
param  : �����ε���
return : -1 : ������ã��������/ 0 : ���â�����/ 1 : ���â������
*/
int api_guild_HasGuildWare(UINT nUserObjectID);


/*
desc   : �������� ���� Ŭ�� �������� ����� ��Ų��.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenMovieBrowser(UINT nUserObjectID);

/*
desc   : ����� ��û ���θ� �˷��ش�.
param  : �����ε���
return : -1 : ������ ã���� ���� / 0 : ����� ��û�Ұ��� / 1 : ����� ��û����
*/
int api_guildwar_IsPreparation(UINT nUserObjectID);
/*
desc   : ����� �Ⱓ üũ.
param  : �����ε���
return : -1 : ������ ã���� ���� / eGuildWarStepType / 0 : ����� �Ⱓ �ƴ� / 1 : ����� ��û �Ⱓ / 2 : ����� ���� �Ⱓ / 3 : ���� �� ����Ⱓ
*/
int api_guildwar_GetStep(UINT nUserObjectID);
/*
desc   : ����� ���� ���� ��Ȳ ����
param  : �����ε���
return : return : -1 => ������ ã���� ����, -2 => ������ �Ⱓ�� �ƴ�, 0 : ����
*/
int api_guildwar_GetTrialCurrentState(UINT nUserObjectID);
/*
desc   : ����� ���� ���
param  : �����ε���
return : return : -1 => ������ ã���� ����, -2 => ���� �� ����Ⱓ�� �ƴ�, -3 => ���� ������� ���谡 �Ϸ���� ����, 0 : ����
*/
int api_guildWar_GetTrialStats(UINT nUserObjectID);
/*
desc   : ����� ��� ����ǥ ����
param  : �����ε���
return : return : -1 => ������ ã���� ����, -2 => ���� �� ����Ⱓ�� �ƴ�, -3 => ����ǥ�� ���õ��� �ʾ���, 0: ����
*/
int api_guildWar_GetMatchList(UINT nUserObjectID);
/*
desc   : ����� �α� ��ǥ ���
param  : �����ε���
return : return : -1 => ������ ã���� ����, -2 => ���� �α� ��ǥ ��� ����, -3 => ������� ��������, 0: ����
*/
int api_guildWar_GetPopularityVote(UINT nUserObjectID);

/*
desc   : ����� ���� ���� üũ.
param  : �����ε���
return : -1 : ������ ã���� ���� / 0 : ���� ���� ���� / 1 : ���� ������
*/
int api_guildWar_GetFinalProgress(UINT nUserObjectID);

/*
desc   : �������� �������� ����� ��Ų��.
param  : �����ε���
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenBrowser(UINT nUserObjectID, const char* szUrl, int nX, int nY, int nWidth, int nHeight);

/*
desc	: ��� â�� ����.
param	: 
return	: -1 => ������ ã���� ����. 1�̸� ����.
*/
int api_ui_OpenDonation(UINT nUserObjectID);


/*
desc	: �κ��丮 â�� ����.
param	: 
return	: -1 => ������ ã���� ����. 1�̸� ����.
*/
int api_ui_OpenInventory(UINT nUserObjectID);

/*
desc	: ������ PC Cafe���
param	:
return	: -1 => ������ ã���� ����, 0 => PC���� �ƴ�. ������ PC�� ���.
*/
int api_user_GetPCCafe(UINT nUserObjectID);

/*
desc	: PC�� �������� ����, Ż�Ͱ� ���� �뿩�������� ���� �־��ش�.
param	:
return	: -1 => ������ ã���� ����, 0 => ����, 1 => �κ��� ���ڸ�, 2 => �̹� �뿩�������� ������ ���� 3 => 
*/
int api_user_SetPCCafeItem(UINT nUserObjectID);

/*
desc   : ������ ���� ��� �������� �����ϰ�, ��������� �����մϴ�.
param  : ����, ��ü���� ����(1-��ü����, 0-�����Ѿ�����)
return : -1 : ������ ã���� ���� / 0 : ������ ������ ���� / 1 : ������ ���� / 2 : ������ ���� �Ϸ�
*/
int api_user_RepairItem(UINT nUserObjectID, int iAllRepair);


/*
desc   : �������� ��ȭ�̵�â�� ����� ��Ų��.
param  : ����
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenExchangeEnchant(UINT nUserObjectID);

/*
desc   : �ش� ���� �����ִ��� Ȯ���Ѵ�.
param  : ����, �� �ε���
return : 1 �̸� ����, 0 �̸� Ŭ����
*/
int api_env_CheckCloseGateByTime(int iMapIndex);

/*
desc   : �ڽ�Ƭ ���� �ռ�â�� ����
param  : ����
return : -1 => ������ ã���� ����, 1�̸� ����
*/
int api_ui_OpenCostumeRandomMix(UINT nUserObjectID);

/*
desc   : ������� �ټ� �ִ��� üũ
param  : ���� �����۾��̵�
return : -1 => ����, 1�̸� ����
*/
int api_quest_WorldBuffCheck(int nItemID);

/*
desc   : ��������� ��
param  : ���� �����۾��̵�
return : 
*/
int api_quest_ApplyWorldBuff(UINT nUserObjectID, int nItemID, int nMapIdx);

#if defined(PRE_ADD_DWC)
/*
desc   : DWC �� ���� DLg�� ����
param  : �����ε���
return : 
*/
void api_ui_OpenCreateDwcTeamDlg(UINT nUserObjectID);
#endif // PRE_ADD_DWC

int api_ui_OpenJobChange(UINT nUserObjectID);