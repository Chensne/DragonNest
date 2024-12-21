#pragma once

class CDNUserBase;
class CDnNpc;

namespace DNScriptAPI
{
	int CheckInvenForQuestReward(CDNUserBase* pUser, const TQuestReward& QuestReward, const bool bSelectedArray[]);

	/*
	desc   : ���� ���� �����Ѵ�.
	param  : ����(�� CDNUserSession*), NPC�ε���, ��ũ�ε���, ��ũ�����̸�
	return : void
	*/
	void api_npc_NextTalk(CDNUserBase* pUser, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

	/*
	desc   : ���� ��ũ��Ʈ�� �����Ѵ�.
	param  : ����(�� CDNUserSession*), NPC, ��ũ�ε���, ��ũ�����̸�
	return : void
	*/
	void api_npc_NextScript(CDNUserBase* pUser, CDnNpc* pNpc, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile);

	/*
	desc   : ��� �Ķ���͸� �����Ѵ�(��Ʈ��).  
	param  : ����, �Ķ����Ű, �Ķ���͹����� �ε���
	return : -1 ������ ã���� ����, -2 �Ķ���� ������ �Ѿ�� ���̻� �����Ҽ� ����. 1 �̸� ����
	sample : 
	���� {%color} ���� �����մϴ�.  ��� ������ ���鶧.
	api_npc_SetParamString("{%color}, math.random(2) );  �̷������� ������ش�.
	1�� TalkParamTable.xls �� 1 : ����, 2: �Ķ� ���� ��� �Ǿ��־���Ѵ�.

	����: api_npc_SetParamString ���� ������ �Ķ���ʹ� api_npc_NextTalk ȣ���� clear �ǹǷ� api_npc_NextTalk ȣ������ �����Ѵ�.
	*/
	int api_npc_SetParamString(CDNUserBase* pUser, const char* szParamKey, int nParamID);

	/*
	desc   : ��� �Ķ���͸� �����Ѵ�(����).  
	param  : ����, �Ķ����Ű, ������
	return : -1 ������ ã���� ����, -2 �Ķ���� ������ �Ѿ�� ���̻� �����Ҽ� ����. 1 �̸� ����
	sample : 
	���� {%age} ���뿡 ������ �;��.  ��� ������ ���鶧.
	api_npc_SetParamInt("{%age}, math.random(30)+40 );  �̷������� ������ش�.

	����: api_npc_SetParamInt ���� ������ �Ķ���ʹ� api_npc_NextTalk ȣ���� clear �ǹǷ� api_npc_NextTalk ȣ������ �����Ѵ�.
	*/
	int api_npc_SetParamInt(CDNUserBase* pUser, const char* szParamKey, int nValue);

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
	void api_log_UserLog(CDNUserBase* pUser, const char* szLog);

	/*
	desc   : �������� ����Ʈ ���� ������ ä������ �����ش�.
	param  : 
	return : void
	*/
	void api_quest_DumpQuest(CDNUserBase* pUser);

	/*
	desc   : �������� ����Ʈ�� �ο� �Ѵ�.
	param  : ����, ����Ʈ�ε���, ����Ʈ Ÿ�� QuestType_SubQuest = 1, QuestType_MainQuest = 2,
	return : 1 : ����, -1 : ������ ã���� ����, -2 : ������ ����Ʈ�� ������ ���� ����
	*/
	int api_quest_AddHuntingQuest(CDNUserBase* pUser, int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt);
	int api_quest_AddQuest(CDNUserBase* pUser, int nQuestID, int nQuestType);

	/*
	desc   : �������� ����Ʈ�� �Ϸ� ��Ų��.
	param  : ����, ����Ʈ�ε���, ���� ������ �ִ� ����Ʈ���� ��������� (������� �÷��� Ƚ���� ���� ��Ų��.), �ݺ� ���� ����
	return : -1 ������ ã���� ����, -2 ����Ʈ�� �������� �ƴ� 
	*/
	int api_quest_CompleteQuest(CDNUserBase* pUser, int nQuestID, bool bDelPlayList);

	/*
	desc   : �Ϸ��Ͽ� �ϷḶŷ�� �Ǿ��ִ� ����Ʈ����?
	param  : ����, ����Ʈ�ε���
	return : -1 ������ ã���� ����, 1 �̸� �ϷḶŷ�� ����Ʈ, 0 �̸� �ϷḶŷ �ȵ�
	*/
	int api_quest_IsMarkingCompleteQuest(CDNUserBase* pUser, int nQuestID);

	/*
	desc   : ������ ����Ʈ�� ������ �ִ��� Ȯ���Ѵ�.
	param  : ����, ����Ʈ�ε���
	return : -1 ������ ã���� ����, -2 ������ ����Ʈ�� ������ ���� ����, -1 ���� ũ�� ����Ʈ�� ������ ���� ( ����Ʈ ������ ���� )
	*/
	int api_quest_UserHasQuest(CDNUserBase* pUser, int nQuestID);

	/*
	desc   : ������ ��� ����Ʈ�� ���������� Ȯ���Ѵ�.
	param  : ����
	return : -1 ������ ã���� ����, �׿ܿ� ���� �������� ����Ʈ ���� ����..
	*/
	int api_quest_GetPlayingQuestCnt(CDNUserBase* pUser);
	
	/*
	desc   : npc �ε����� �����Ѵ�.
	param  : Npc
	return : -2 npc �� ã���� ����, �׿ܿ��� npc�ε���
	*/
	int api_npc_GetNpcIndex(CDnNpc* pNpc);

	/*
	desc   : ������ ������ �ִ� ����Ʈ�� ����, ���� ������ �����Ѵ�.
	param  : ����, ����Ʈ�ε���, ���� ����
	return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����.  -3 ���� ���� ������ ���. ������ ��� 1�� ����
	*/
	int api_quest_SetQuestStepAndJournalStep(CDNUserBase* pUser, int nQuestID, short nQuestStep, int nJournalStep);
	
	/*
	desc   : ������ ������ �ִ� ����Ʈ�� ����Ʈ ������ �����Ѵ�.
	param  : ����, ����Ʈ�ε���, ����Ʈ ����
	return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����. ������ ��� 1�� ����
	*/
	int api_quest_SetQuestStep(CDNUserBase* pUser, int nQuestID, short nQuestStep);
	
	/*
	desc   : ������ ������ �ִ� ����Ʈ�� ����Ʈ ������ ��´�.
	param  : ����, ����Ʈ�ε���
	return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����., -1 ���� ū��� ���� ����Ʈ�� ����
	*/
	int api_quest_GetQuestStep(CDNUserBase* pUser, int nQuestID);

	/*
	desc   : ������ ������ �ִ� ����Ʈ�� ���� ������ �����Ѵ�.
	param  : ����, ����Ʈ�ε���, ���� ����
	return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����.  -3 ���� ���� ������ ���. ������ ��� 1�� ����
	*/
	int api_quest_SetJournalStep(CDNUserBase* pUser, int nQuestID, int nJournalStep);

	/*
	desc   : ������ ������ �ִ� ����Ʈ�� ���� ������ ��´�.
	param  : ����, ����Ʈ�ε���
	return : -1 => ������ ã���� ���� -2 => ������ ����Ʈ�� ������ ���� ����., -1 ���� ū��� ���� ���� ����
	*/
	int api_quest_GetJournalStep(CDNUserBase* pUser, int nQuestID);
	
	/*
	desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� �����Ѵ�.
	param  : ����, �޸��ε���, �޸�
	return : -1 => ������ ã���� ����. -2 => ������ ����Ʈ�� ������ ���� ����. -3 �޸� �ε��� ������ ���. ������ ��� 1�� ����
	*/
	int api_quest_SetQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex, int iVal);

	/*
	desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� ��´�.
	param  : ����, �޸��ε���
	return : -1 => ������ ã���� ����. -2 => ������ ����Ʈ�� ������ ���� ����. -3 �޸� �ε��� ������ ���. -1 ���� ū��� Ư�� �޸� �ε����� ��
	*/
	int api_quest_GetQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex);
	
	/*
	desc   : ���� ��ų�, ������ ��� ���� ī���� ������ �����ϴ� �Լ� �̴�.
	param  : ����, 
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
	int api_quest_SetCountingInfo(CDNUserBase* pUser, int nQuestID,  int nSlot, int nCountingType, int nCountingIndex, int nTargetCnt); 


	/*
	desc   : ���õ� ī���� ������ Ŭ���� �Ѵ�. 
	param  : ����, ����Ʈ �ε���
	return : -1 => ������ ã���� ���� -2 => ����Ʈ�� ã���� ����,
	*/
	int api_quest_ClearCountingInfo(CDNUserBase* pUser, int nQuestID);
	
	
	/*
	desc   : ���õ� ī���õ��� ��� �Ϸ� �Ǿ����� Ȯ���Ѵ�.
	param  : ����, �������ε���, ������ ī��Ʈ
	return : -1 => ������ ã���� ���� -2 => ����Ʈ�� ã���� ����,  -3�̸� �Ϸ� �ȵ�, 1 �̸� ��� �Ϸ�
	*/
	int api_quest_IsAllCompleteCounting(CDNUserBase* pUser, int nQuestID);

	/*
	desc   : �������� �������� �������� �κ������� �ִ��� Ȯ���Ѵ�.
	param  : ����, �������ε���, ������ ī��Ʈ
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����
	*/
	int api_user_CheckInvenForAddItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);
	
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
	int api_user_CheckInvenForAddItemList(CDNUserBase* pUser, lua_tinker::table ItemTable);
	
	/*
	desc   : �������� �������� �����Ѵ�.
	param  : ����, �������ε���, ������ ī��Ʈ, ����Ʈ ���̵�
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����
	*/
	int api_user_AddItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt, int nQuestID);

	/*
	desc   : �������Լ� �������� �����.
	param  : ����, �������ε���, ������ ī��Ʈ, ����Ʈ ���̵�
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����
	*/
	int api_user_DelItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt, int nQuestID);

	/*
	desc   : �������Լ� �������� ��� �����.
	param  : ����, �������ε���, ������ ī��Ʈ, ����Ʈ ���̵�
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����
	*/
	int api_user_AllDelItem(CDNUserBase* pUser, int nItemIndex);

	/*
	desc   : ������ �������� ������ �ִ��� Ȯ���Ѵ�.
	param  : ����, �������ε���, ������ ī��Ʈ
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
	*/
	int api_user_HasItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);

	/*
	desc   : ������ Ŭ���� ID �� ��´�.
	param  : ����
	return : -1 => ������ ã���� ����, �׿ܴ� ClasssID
	*/
	int api_user_GetUserClassID(CDNUserBase* pUser);

	/*
	desc   : ������ Job ID �� ��´�.
	param  : ����
	return : -1 => ������ ã���� ����, �׿ܴ� Job ID
	*/
	int api_user_GetUserJobID(CDNUserBase* pUser);

	/*
	desc   : ������ level�� ��´�.
	param  : ����
	return : -1 => ������ ã���� ����, �׿ܴ� Level
	*/
	int api_user_GetUserLevel(CDNUserBase* pUser);

	/*
	desc   : ���� �κ��丮�� ��ĭ���� ��´�.
	param  : ����
	return : -1 => ������ ã���� ����, �׿ܴ� ��ĭ��
	*/
	int api_user_GetUserInvenBlankCount(CDNUserBase* pUser);

	/*
	desc   : ���� ����Ʈ �κ��丮�� ��ĭ���� ��´�.
	param  : ����
	return : -1 => ������ ã���� ����, �׿ܴ� ��ĭ��
	*/
	int api_quest_GetUserQuestInvenBlankCount(CDNUserBase* pUser);
	
	/*
	desc   : �������� �������� ��� ��Ų��.
	param  : ����, �����������̸�
	return : -1 => ������ ã���� ����,  1�̸� ����
	*/
	int api_user_PlayCutScene(CDNUserBase* pUser, UINT nNpcObjectID, int nCutSceneTableID, bool bIgnoreFadeIn);

	/*
	desc   : �������� â�� ����� �˸���.
	param  : ����
	return : -1 => ������ ã���� ����,  1�̸� ����
	*/
	int api_ui_OpenWareHouse(CDNUserBase* pUser, int iItemID=0 );

#if defined (PRE_MOD_GAMESERVERSHOP)
	/*
	desc   : �������� ������ ����� �˸���.
	param  : �����ε���, ���� ID
	return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����
	*/
	int api_ui_OpenShop(CDNUserBase* pUser, int nShopID, Shop::Type::eCode Type);

	/*
	desc   : �������� ��ų ������ ����� �˸���.
	param  : �����ε���, ��ų ���� ID
	return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����
	*/
	int api_ui_OpenSkillShop(CDNUserBase* pUser);
#endif
	
	/*
	desc   : �������� ���庸�� �ռ� â�� ����� �Ѵ�.
	param  : ����
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenCompoundEmblem(CDNUserBase* pUser);
	
	/*
	desc   : �������� ���� ���׷��̵� â�� ����� �˸���.
	param  : ����
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenUpgradeJewel(CDNUserBase* pUser);

	/*
	desc   : �������� �������� ����� ��Ų��.
	param  : ����
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenMailBox(CDNUserBase* pUser);

	/*
	desc   : �������� ���� ������ ���� �������� ����� ��Ų��.
	param  : �����ε���
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenGlyphLift(CDNUserBase* pUser);
	
	/*
	desc   : �������� ����â�� ����� ��Ų��.
	param  : ����
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenDisjointItem(CDNUserBase* pUser);
	
	/*
	desc   : �������� ������ �ռ�â�� ����� ��Ų��.
	param  : ����, ������ �ռ� ���� ID
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenCompoundItem(CDNUserBase* pUser, int nCompoundShopID);
	
	/*
	desc   : �������� ĳ�� ������ ������ ����� ��Ų��.
	param  : ����
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenCashShop(CDNUserBase* pUser);

	/*
	desc   : �������� ��� ���� UI ��ȭ���ڸ� ����� ��Ų��.
	param  : ����, ��������ȣ
	return : -1 => ������ ã���� ����, 1�̸� ����

	��������ȣ>
		0 : ���â��
		1 : ����ػ�
		2 : ���Ż��
		3 : �������Ʈ
		4 : ��巹����
	*/
	int api_ui_OpenGuildMgrBox(CDNUserBase* pUser, int nGuildMgrNo);
	
	/*
	desc   : �Ϻ� ��í�� ���̾�α׸� ����.
	param  : ���� ��í�� �ε���
	return : -1 => ������ ã���� ����, -2 �� ���� ����. 1�̸� ����, -3 �̸� �����ο� �ɷ���..
	*/
	int api_ui_OpenGacha_JP(CDNUserBase* pUser, int nGachaShopID);
	
	/*
	desc   : NPC �����ϱ� ���̾�α׸� ����.
	param  : ��ȭ���� Npc ID 
	return : -1 => ������ ã���� ����. 1 �̸� ����.
	*/
	int api_ui_OpenGiveNpcPresent(CDNUserBase* pUser, int nNpcID);

	/*
	desc	: ��� â�� ����.
	param	: 
	return	: -1 => ������ ã���� ����. 1�̸� ����.
	*/
	int api_ui_OpenDonation(CDNUserBase* pUser);


	/*
	desc	: �κ��丮 â�� ����.
	param	: 
	return	: -1 => ������ ã���� ����. 1�̸� ����.
	*/
	int api_ui_OpenInventory(CDNUserBase* pUser);

#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
	/*
	desc   : �ؽ��� ���̾�α׸� ����
	param  : �����ε���, ���̾�α� ������
	return : -1 => ������ ã���� ����. 1�̸� ����.
	*/
	int api_Open_Texture_Dialog(CDNUserBase* pUser, SCOpenTextureDialog data);


	/*
	desc   : �ؽ��� ���̾�α׸� �ݴ´�
	param  : �����ε���, ���̾�α� ������
	return : -1 => ������ ã���� ����. 1�̸� ����.
	*/
	int api_Close_Texture_Dialog(CDNUserBase* pUser, int nDlgID);
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
	int api_user_UserMessage(CDNUserBase* pUser, int nType, int nBaseStringIdx, lua_tinker::table ParamTable);
	
	/*
	desc   : �������� �ɺ��������� �߰��Ѵ�.
	param  : ����,  �ɺ������۾��̵�, ����
	return : -1 => ������ ã���� ����, -2�� ���� 1�̸� ����
	*/
	int api_quest_AddSymbolItem(CDNUserBase* pUser, int nItemID, short wCount);

	/*
	desc   : �������� �ɺ��������� �����Ѵ�.
	param  : ����,  �ɺ������۾��̵�, ����
	return : -1 => ������ ã���� ����, -2 �� ����, 1�̸� ����
	*/
	int api_quest_DelSymbolItem(CDNUserBase* pUser, int nItemID, short wCount);

	/*
	desc   : ������ �ɺ� �������� ������ �ִ��� Ȯ���Ѵ�.
	param  : ����, �������ε���, ������ ī��Ʈ
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
	*/
	int api_quest_HasSymbolItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);
	
	/*
	desc   : �������� ����Ʈ �������� �������� �κ������� �ִ��� Ȯ���Ѵ�.
	param  : ����, ����Ʈ ������ �ε���, ����Ʈ ������ ī��Ʈ
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 �κ� ����
	*/
	int api_quest_CheckQuestInvenForAddItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);
	
	/*
	desc   : �������� ����Ʈ ������ ����� �������� �κ������� �ִ��� Ȯ���Ѵ�.
	param  : ����, ����Ʈ ������ ��� (������̺�, [<�ִ밳��>]{����Ʈ �������ε���, ����Ʈ ������ ī��Ʈ} �� �� ������ <�ִ밳�� (���� 10��)> ��ŭ ����)
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
	int api_quest_CheckQuestInvenForAddItemList(CDNUserBase* pUser, lua_tinker::table ItemTable);

	/*
	desc   : �������� ����Ʈ �������� �߰��Ѵ�.
	param  : ����,  ����Ʈ�����۾��̵�, ����, ����Ʈ ���̵�
	return : -1 => ������ ã���� ����, -2�� ���� 1�̸� ����
	*/
	int api_quest_AddQuestItem(CDNUserBase* pUser, int nItemID, short wCount, int nQuestID);

	/*
	desc   : �������� ����Ʈ �������� �����Ѵ�.
	param  : ����,  ����Ʈ�����۾��̵�, ����, ����Ʈ ���̵�
	return : -1 => ������ ã���� ����, -2 �� ����, 1�̸� ����
	*/
	int api_quest_DelQuestItem(CDNUserBase* pUser, int nItemID, short wCount, int nQuestID);

	/*
	desc   : �������� ����Ʈ �������� ��� �����Ѵ�.
	param  : ����,  ����Ʈ�����۾��̵�
	return : -1 => ������ ã���� ����, -2 �� ����, 1�̸� ����
	*/
	int api_quest_AllDelQuestItem(CDNUserBase* pUser, int nItemID);

	/*
	desc   : ������ ����Ʈ �������� ������ �ִ��� Ȯ���Ѵ�.
	param  : ����, �������ε���, ������ ī��Ʈ
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
	*/
	int api_quest_HasQuestItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);
	
	/*
	desc   : �������� ��� ����â�� ����� �˸���.
	param  : �����ε���
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenGuildCreate(CDNUserBase* pUser);

	/*
	desc   : �������� ����Ʈ ����â�� ����� �˸���.
	param  : �����ε���, �������̺�, bActivate �� true �� ���� �ְ� �̸� �׳� ���� �뵵�� false�� ����.
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenQuestReward(CDNUserBase* pUser, int nRewardTableIndex, bool bActivate);
	
	/*
	desc   : �������� ����Ʈ ������ �Ѵ�. 
	param  : ����, �������̺�, ����Ʈ ���̵�, ����üũ��ȣ
	return : -1 => ������ ã���� ����, -2 ���������۰�ü ����, -3 �κ� ����, -4 �̸� ���̺��� ã���� ����, -5 ���� ���� üũ ����, 1 �̸� ����
	*/
	int api_quest_RewardQuestUser(CDNUserBase* pUser, int nRewardTableIndext, int nQuestID, int nRewardCheck);

	/*
	desc   : �ش� ������ ���� �ʹ�ȣ�� ����
	param  : ����
	return : �ʹ�ȣ (1 �̻�), �� �ܴ� ���� ? (0 ����)
	*/
	int api_user_GetMapIndex(CDNUserBase* pUser);
	
	/*
	desc   : �ش� ������ �ֱ� �������� �Ϸ� ��ũ�� ����
	param  : �����ε���
	return : �������� �Ϸ� ��ũ (SSS : 0 / SS : 1 / S : 2 / A : 3 / B : 4 / C : 5 / D : 6), �� �ܴ� ���� ? (�⺻ -1)
	*/
	int api_user_GetLastStageClearRank(CDNUserBase* pUser);
	
	/*
	desc   : ������ ���� �������� Ȯ���Ѵ�.
	param  : ����, üũ�� ��
	return : -1 => ������ ã�� �� ����, -2 => �������� ������, 1 => �������� �����
	*/
	int api_user_EnoughCoin(CDNUserBase* pUser, int nCoin);
	
	/*
	desc   : ������ ���� �������� Ȯ���Ѵ�.
	param  : ����
	return : -1 => ������ ã���� ����, 0 �̻� => ���� ������
	*/
	int api_user_GetCoin(CDNUserBase* pUser);
	
	/*
	desc   : ������ Ư�� �̼� ��� ���θ� ��ȯ
	param  : ����, �̼��ε��� (MISSIONMAX (���� 5000 �� 0 ~ 4999))
	return : -1 => ������ ã���� ����, 0 => �ش� �̼� ����, 1 ���� �̿� => �ش� �̼� ����
	*/
	int api_user_IsMissionGained(CDNUserBase* pUser, int nMissionIndex);
	
	/*
	desc   : ������ Ư�� �̼� �Ϸ� ���θ� ��ȯ
	param  : ����, �̼��ε��� (MISSIONMAX (���� 5000 �� 0 ~ 4999))
	return : -1 => ������ ã���� ����, 0 => �ش� �̼� ����, 1 ���� �̿� => �ش� �̼� ����
	*/
	int api_user_IsMissionAchieved(CDNUserBase* pUser, int nMissionIndex);
	
	/*
	desc   : ������ â�� �������� ������ �ִ��� Ȯ���Ѵ�.
	param  : ����, �������ε���, ������ ī��Ʈ
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
	*/
	int api_user_HasItemWarehouse(CDNUserBase* pUser, int nItemIndex, int nItemCnt);

	/*
	desc   : ������ ��� �������� ������ �ִ��� Ȯ���Ѵ�.
	param  : ����, �������ε���
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, 1 ���� �׿� true
	*/
	int api_user_HasItemEquip(CDNUserBase* pUser, int nItemIndex);
	
	/*
	desc   : ������ ���� ��� ������ ��ȯ
	param  : ����
	return : 0 : ��� ���� / 1 : ����� / 2 : �α���� / 3 : ���ӱ��� / 4 : �Ϲݱ��� / 5 : ���Ա���
	*/
	int api_guild_GetGuildMemberRole(CDNUserBase* pUser);
	
	/*
	desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� 1 ������Ŵ
	param  : ����, ����Ʈ��ȣ, �޸��ε���
	return : ������ ����Ʈ �޸� (������ ��쿡�� LONG_MIN ��ȯ)
	*/
	int api_quest_IncQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex);

	/*
	desc   : ������ ������ �ִ� ����Ʈ�� �޸��� Ư�� �ε����� ���� 1 ���ҽ�Ŵ
	param  : ����, ����Ʈ��ȣ, �޸��ε���
	return : ������ ����Ʈ �޸� (������ ��쿡�� LONG_MAX ��ȯ)
	*/
	int api_quest_DecQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex);
	
	/*
	desc   : �������� Ư�� ���� ������ ����
	param  : ����, ���� ���� ����
	return : -1 : ������ ã���� ���� / -2 : ���� ���� ��ȣ�� / -3 : ���� �Ұ� ������ / 0 : �̹� ���� ���� ������ / 1 : ���� ���� ���� ����
	*/
	int api_user_SetUserJobID(CDNUserBase* pUser, int nJobID);
	
	/*
	desc   : ������ ���� ������ Ư�� ���� �迭�� �ִ��� Ȯ��
	param  : ����, ���� ���� ����
	return : -1 : ������ ã���� ���� / 0 : ���� �迭�� �ƴ� / 1 : ���� �迭�� ����
	*/
	int api_user_IsJobInSameLine(CDNUserBase* pUser, int nBaseJobID);
	
	/*
	desc   : ������ ĳ�� �������� ������ �ִ��� Ȯ���Ѵ�.
	param  : ����, ĳ���������ε���, ������ ī��Ʈ
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, -3 ������ ���� ����, 0 ���� ũ�� true ( ������ ���� )
	*/
	int api_user_HasCashItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt);

	/*
	desc   : ������ ��� ĳ�� �������� ������ �ִ��� Ȯ���Ѵ�.
	param  : ����, ĳ���������ε���
	return : -1 => ������ ã���� ���� -2 => �������� ã���� ����, 1 ���� �׿� true
	*/
	int api_user_HasCashItemEquip(CDNUserBase* pUser, int nItemIndex);
	
	/*
	desc   : Ư�� ī���� Ÿ��, ID �� ���ؼ� ���Ƿ� ī������ ������Ų��.
	param  : ����, 
			ī������ Ÿ�� (CountingType_MyMonsterKill = 1, CountingType_AllMonsterKill = 2, CountingType_Item	= 3),
			ī������ ���� �ε��� (������ ID, ���� ID ���)
	return : -1 => ������ ã���� ����, 1 => ����
	*/
	int api_quest_IncCounting(CDNUserBase* pUser, int nCountingType, int nCountingIndex);
	
	/*
	desc   : ������ ���� ���� �ִ� ����Ʈ ���� �����Ͽ����� ���θ� ��ȯ�Ѵ�.
	param  : ����, ����Ʈ�ε���
	return : -1 : ������ ã���� ����, 1 : �ִ� ����Ʈ ���� ���� ��, �׿� : �ƴ�
	*/
	int api_quest_IsPlayingQuestMaximum(CDNUserBase* pUser);
	
	/*
	desc   : Ư�� ����Ʈ�� ���� �Ϸ� ��Ų��. (���� ���� �ƴ� ����Ʈ�� �߰��Ͽ� �Ϸ�)
	param  : ����, ����Ʈ�ε���, ����Ʈ �ڵ� (0:�Ϲ� / 1:�ð���), ��ŷ���� (1:��ŷ/0:����), �������� (1:����/0:����), �ݺ����� (1:�ݺ�/0:����)
	return : -1 : ������ ã���� ����, 1 : �ִ� ����Ʈ ���� ���� ��, �׿� : �ƴ�
	*/
	int api_quest_ForceCompleteQuest(CDNUserBase* pUser, int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat);
	
	/*
	desc   : NPC ȣ��Point �� ���´�.
	param  : ����, NpcID
	return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
	*/
	int api_npc_GetFavorPoint(CDNUserBase* pUser, int iNpcID);
	
	/*
	desc   : NPC ����Point �� ���´�.
	param  : ����, NpcID
	return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
	*/
	int api_npc_GetMalicePoint(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : NPC ȣ��Percent �� ���´�.
	param  : ����, NpcID
	return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
	*/
	int api_npc_GetFavorPercent(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : NPC ����Percent �� ���´�.
	param  : ����, NpcID
	return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
	*/
	int api_npc_GetMalicePercent(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : NPC ȣ��Point �� �����Ѵ�.
	param  : ����, NpcID, ������(- ������ ������)
	return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
	*/
	int api_npc_AddFavorPoint(CDNUserBase* pUser, int iNpcID, int val);

	/*
	desc   : NPC ����Point �� �����Ѵ�.
	param  : ����, NpcID, ������(- ������ ������)
	return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
	*/
	int api_npc_AddMalicePoint(CDNUserBase* pUser, int iNpcID, int val);

	/*
	desc   : NPC ���� ���� ���� ������ ������ ������ ������� Ŭ���̾�Ʈ���� ��û.
	param  : ����, NpcID
	return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
	*/
	int api_npc_SendSelectedPresent(CDNUserBase* pUser, int iNpcID);
	
	/*
	desc   : NPC ���� ���� ���� ������ ������ ������ ������� Ŭ���̾�Ʈ���� ��û.
	param  : ����, NpcID
	return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
	*/
	int api_npc_Rage(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : NPC ���� ���� ���� ������ ������ ������ ������� Ŭ���̾�Ʈ���� ��û.
	param  : ����, NpcID
	return : -1 => ������ ã���� ����, -2 => �ý��ۿ��� �׿� => ���ϰ�
	*/
	int api_npc_Disappoint(CDNUserBase* pUser, int iNpcID);

	/*
	desc   : Ư�� ������� ��ü ��ų�� ���½�Ų��.
	param  : ����
	return : -1 : ������ ã���� ����, 1 : ����
	*/
	int api_user_ResetSkill(CDNUserBase* pUser);

	/*
	desc   : ������ ���� ��� �������� �����ϰ�, ��������� �����մϴ�.
	param  : ����, ��ü���� ����(1-��ü����, 0-�����Ѿ�����)
	return : -1 : ������ ã���� ���� / 0 : ������ ������ ���� / 1 : ������ ���� / 2 : ������ ���� �Ϸ�
	*/
	int api_user_RepairItem(CDNUserBase* pUser, int iAllRepair);

	/*
	desc   : �������� ��ȭ�̵�â�� ����� ��Ų��.
	param  : ����
	return : -1 => ������ ã���� ����, 1�̸� ����
	*/
	int api_ui_OpenExchangeEnchant(CDNUserBase* pUser);

	/*
	desc   : �ش� ���� �����ִ��� Ȯ���Ѵ�.
	param  : ����, �� �ε���
	return : 1 �̸� ����, 0 �̸� Ŭ����
	*/
	int api_env_CheckCloseGateByTime(int iMapID);

	/*
	desc   : ���� ����Ʈ �Ϸ� ������ ��� �Ѵ�
	param  : ����Ʈ�ε���, ���� �ε���
	*/
	void api_quest_SetRemoteComplete (int nQuestID, int nStep);

	/*
	desc   : ������� �ټ� �ִ��� üũ
	param  : ���� �����۾��̵�
	*/
	int api_quest_WorldBuffCheck(int nItemID);

	/*
	desc   : ��������� ��
	param  : ���� �����۾��̵�
	*/
	int api_quest_ApplyWorldBuff(CDNUserBase* pUser, int nItemID, int nMapIdx);


	//rlkt_test
	int api_Open_ChangeJobDialog(CDNUserBase* pUser);
};
