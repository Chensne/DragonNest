#include "StdAfx.h"
#include "DNGMCommand.h"
#include "DNCheatCommand.h"
#include "DNUserSendManager.h"
#include "DNUserQuest.h"
#include "DNGameDataManager.h"
#include "DNLogConnection.h"
#include "DNUserSession.h"



#if defined(_VILLAGESERVER)

#include "DNMasterConnection.h"
#include "DNFieldDataManager.h"
#include "DNScriptAPI.h"
#include "DNPartyManager.h"
#include "DNUserSessionManager.h"
#include "DNField.h"
#include "DNAuthManager.h"
#include "DNCountryUnicodeSet.h"
#include "DNGuildVillage.h"

extern TVillageConfig g_Config;

#elif defined(_GAMESERVER)

#include "DNGameRoom.h"
#include "DNActor.h"
#include "TaskManager.h"
#include "DnItemTask.h"
#include "DnDropItem.h"
#include "EtActionCoreMng.h"
#include "DNGameServerScriptApi.h"
#include "DnGameTask.h"
#include "DNMasterConnection.h"
#include "DNMasterConnectionManager.h"
#include "PvPRespawnLogic.h"
#include "DnPlayerActor.h"

#if defined( PRE_WORLDCOMBINE_PVP )
#include "PvPGameMode.h"
#endif
#endif

enum eGMCommandType
{
	GM_Help = 0,
#if defined(_CH)
	GM_HideOn,			// ����
	GM_HideOff,			// ��Ÿ��
	GM_ChangeMap,		// ���̵�
	GM_Trace,			// ���� ����
	GM_Warp,
	GM_Trace_AID,		// ��ī��Ʈ���̵�� ���� ����
	GM_Trace_CID,		// ĳ���;��̵�� ���� ����
	GM_Recall,			// ���� ��ȯ
	GM_UserBan,			// ���� ����
	GM_Fatigue,
	GM_HP,
	GM_WEventClear,		// �ָ��̺�Ʈ Ŭ����

#elif defined (_US)
	GM_HideOn,
	GM_HideOff,
	GM_ChangeMap,
	GM_Trace,
	GM_UserBan,
	GM_SummonMob,
	GM_AllKillMob,
	GM_ZoneNotice,
	GM_HP,
	GM_SetLevel,
	GM_MakeItem,
	GM_Recall,
	GM_Notice,
	GM_MakeCoin,
	GM_WhisperOn,		// �ӼӸ�����
	GM_WhisperOff,		// �ӼӸ�����
	GM_Warp,
#elif defined (_SG) || defined (_TH)
	GM_Notice,
	GM_WhisperOn,
	GM_WhisperOff,
	GM_HideOn,
	GM_HideOff,
	GM_SummonMob,
	GM_AllKillMob,
	GM_MakeItem,
	GM_MakeCoin,
	GM_SetLevel,
	GM_ChangeMap,
	GM_SetQuest,
	GM_ZoneNotice,
	GM_Trace,
	GM_Warp,
	GM_Trace_AID,
	GM_Trace_CID,
	GM_Recall,
	GM_UserBan,
	GM_Fatigue,
	GM_DestroyPvP,
	GM_RepairAll,
	GM_GCreate,
	GM_GDismiss,
#elif defined (_ID)
	GM_Notice,
	GM_WhisperOn,
	GM_WhisperOff,
	GM_HideOn,
	GM_HideOff,
	GM_SummonMob,
	GM_AllKillMob,
	GM_MakeItem,
	GM_MakeCoin,
	GM_SetLevel,
	GM_ChangeMap,
	GM_SetQuest,
	GM_ZoneNotice,
	GM_Trace,	
	GM_Warp,	
	GM_Trace_AID,
	GM_Trace_CID,	
	GM_Recall,		
	GM_UserBan,
	GM_Fatigue,	
	GM_DestroyPvP,
#elif defined (_EU)
	GM_Notice,
	GM_WhisperOn,		// �ӼӸ�����
	GM_WhisperOff,		// �ӼӸ�����
	GM_HideOn,			// ����
	GM_HideOff,			// ��Ÿ��
	GM_SummonMob,		// ����ȯ
	GM_AllKillMob,		// ���Ҹ�
	GM_MakeItem,		// �����ۻ���
	GM_MakeCoin,		// ��������
	GM_SetLevel,		// ����
	GM_ChangeMap,		// ���̵�
	GM_SetQuest,		// ����Ʈ����
	GM_ZoneNotice,		// gm�� �ִ� ���� ��� ä�ο� ����
	GM_Trace,			// ���� ����
	GM_Warp,
	GM_Trace_AID,		// ��ī��Ʈ���̵�� ���� ����
	GM_Trace_CID,		// ĳ���;��̵�� ���� ����
	GM_Recall,			// ���� ��ȯ
	GM_UserBan,			// ���� ����
	GM_Fatigue,
	GM_DestroyPvP,		// ���� �Ǻ��Ƿ��ı�
#else
	GM_Notice,
	GM_WhisperOn,		// �ӼӸ�����
	GM_WhisperOff,		// �ӼӸ�����
	GM_HideOn,			// ����
	GM_HideOff,			// ��Ÿ��
	GM_SummonMob,		// ����ȯ
	GM_AllKillMob,		// ���Ҹ�
	GM_MakeItem,		// �����ۻ���
	GM_MakeCoin,		// ��������
	GM_SetLevel,		// ����
	GM_ChangeMap,		// ���̵�
	GM_SetQuest,		// ����Ʈ����
	GM_ZoneNotice,		// gm�� �ִ� ���� ��� ä�ο� ����
	GM_Trace,			// ���� ����
	GM_Warp,
	GM_Trace_AID,		// ��ī��Ʈ���̵�� ���� ����
	GM_Trace_CID,		// ĳ���;��̵�� ���� ����
	GM_Recall,			// ���� ��ȯ
	GM_UserBan,			// ���� ����
	GM_Fatigue,
	GM_DestroyPvP,		// ���� �Ǻ��Ƿ��ı�
	GM_WEventClear,		// �ָ��̺�Ʈ Ŭ����
#if defined( PRE_WORLDCOMBINE_PVP )
	GM_WORLDPVPROOMALLKILLREDTEAM, // ������������ ��ų�� ������
	GM_WORLDPVPROOMALLKILLBLUETEAM,
	GM_WORLDPVPROOMTOURNAMENTSETTING,	// ������������ ��ʸ�Ʈ ����
#endif
#if defined( PRE_ADD_MUTE_USERCHATTING )
	GM_MuteUser,		// [JPN]���� ü�� ����
#endif	// #if defined( PRE_ADD_MUTE_USERCHATTING )
#endif
	GM_END
};

__GMCmdLineDefineStruct g_GMCmdList[] = 
{
	{ GM_Help,			_T("/?"),			CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("��ڸ�ɾ��"), _T("Help") },
#if defined(_CH)
	{ GM_HideOn,		_T("/hideon"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/hideon"), _T("Hide On") },
	{ GM_HideOff,		_T("/hideoff"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/hideoff"), _T("Hide Off") },
	{ GM_ChangeMap,		_T("/go"),			CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/go (MapID) (0-4 : ����, ����, �����, ������, ���)"),  _T("/go (MapID) (0-4 : Easy, Normal, Hard, Master, Abyss)")},
	{ GM_Trace,			_T("/trace"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/trace ĳ�����̸�),����(�Է��� ĳ�����̸��� ������ �����Ѵ�.)"), _T("Usage(/trace CharacterName), Description(Trace character's dungeon)") },
	{ GM_Warp,			_T("/warp"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/warp ĳ�����̸�),����(�Է��� ĳ�����̸��� ��ġ�� �̵��Ѵ�.���Ӽ��������� ��밡��)"), _T("/wapr CharacterName), (Move Character in Only GameServer)") },
	{ GM_Trace_AID,		_T("/trace_aid"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/trace_aid ��ī��Ʈ�����̵�),����(�Է��� ��ī��Ʈ���̵��� ������ �����Ѵ�.)"), _T("/trace_aid AccountDBID) - (Trace AccountDBID's position)") },
	{ GM_Trace_CID,		_T("/trace_cid"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/trace_cid ĳ���͵����̵�),����(�Է��� ĳ���͵����̵��� ������ �����Ѵ�.)"), _T("/trace_cid CharacterDBID) - (Trace CharacterDBID's position)") },
	{ GM_Recall,		_T("/recall"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/recall ĳ�����̸�),����(�Է��� ĳ���͸� GM������ ��ȯ�մϴ�."), _T("Usage(/recall CharacterName), Description(Recall character to GM Village") },
	{ GM_UserBan,		_T("/userban"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/userban ĳ�����̸�),����(�Է��� ĳ���͸� ���������Ų��."),  _T("/userban CharacterName)")},
	{ GM_Fatigue,		_T("/fatigue"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/fatigue"),  _T("/fatigue")},
	{ GM_HP,			_T("/hp"),			CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/hp (1 - 100)"), _T("/hp (1 - 100)") },
	{ GM_WEventClear,	_T("/clearevent"),	CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,									_T("/clearevent"), _T("/clearevent") },

#elif defined (_US)
	{ GM_HideOn,		_T("/hideon"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/hideon"), _T("Hide On") },
	{ GM_HideOff,		_T("/hideoff"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/hideoff"), _T("Hide Off") },
	{ GM_ChangeMap,		_T("/go"),			CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/go (MapID) (0-4 : ����, ����, �����, ������, ���)"),  _T("/go (MapID) (0-4 : Easy, Normal, Hard, Master, Abyss)")},
	{ GM_Trace,			_T("/trace"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/trace ĳ�����̸�),����(�Է��� ĳ�����̸��� ������ �����Ѵ�.)"), _T("Usage(/trace CharacterName), Description(Trace character's dungeon)") },
	{ GM_UserBan,		_T("/userban"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/userban ĳ�����̸�),����(�Է��� ĳ���͸� ���������Ų��."),  _T("/userban CharacterName)")},
	{ GM_SummonMob,		_T("/summonmob"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/summonmob (MonsterID) (MonsterCount)"), _T("/summonmob (MonsterID) (MonsterCount)") },
	{ GM_AllKillMob,	_T("/allkillmon"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/allkillmon"), _T("/allkillmon") },
	{ GM_ZoneNotice,	_T("/zone"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/zone (Notice)"), _T("/zone (Notice)") },
	{ GM_HP,			_T("/hp"),			CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/hp (1 - 100)"), _T("/hp (1 - 100)") },
	{ GM_SetLevel,		_T("/setlevel"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setlevel (Level)"), _T("/setlevel (Level)") },
	{ GM_MakeItem,		_T("/makeitem"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/makeitem (ItemIndex) (Count) (Enchant) (Potential) (Cash Period)"), _T("/makeitem (ItemIndex) (Count) (Enchant) (Potential) (Cash Period)") },
	{ GM_Recall,		_T("/recall"),		CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,									_T("/recall (CharacterName)"), _T("/recall (CharacterName)") },
	{ GM_Notice,		_T("/notice"),		CmdAuth_Master|CmdAuth_QA,														_T("/notice (Notice)"), _T("/notice (Notice)")  },
	{ GM_MakeCoin,		_T("/makecoin"),	CmdAuth_Master|CmdAuth_QA,														_T("/makecoin (Coin)"), _T("/makecoin (Coin)") },
	{ GM_WhisperOn,		_T("/w_on"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_on (CharacterName)"), _T("/w_on (CharacterName) - Whispser On") },
	{ GM_WhisperOff,	_T("/w_off"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_off (CharacterName)"), _T("/w_off (CharacterName) - Whisper Off") },
	{ GM_Warp,			_T("/warp"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/warp ĳ�����̸�),����(�Է��� ĳ�����̸��� ��ġ�� �̵��Ѵ�.���Ӽ��������� ��밡��)"), _T("/wapr CharacterName), (Move Character in Only GameServer)") },
#elif defined (_SG)
	{ GM_Notice,		_T("/notice"),		CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,		_T("/notice (Notice)"), _T("/notice (Notice)")  },
	{ GM_WhisperOn,		_T("/w_on"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_on (CharacterName)"), _T("/w_on (CharacterName) - Whispser On") },
	{ GM_WhisperOff,	_T("/w_off"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_off (CharacterName)"), _T("/w_off (CharacterName) - Whisper Off") },
	{ GM_HideOn,		_T("/hideon"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideon"), _T("/hideon") },
	{ GM_HideOff,		_T("/hideoff"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideoff"), _T("/hideoff") },
	{ GM_SummonMob,		_T("/summonmob"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("/summonmob (MonsterID) (MonsterCount)"), _T("/summonmob (MonsterID) (MonsterCount)") },
	{ GM_AllKillMob,	_T("/allkillmon"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/allkillmon"), _T("/allkillmon") },
	{ GM_MakeItem,		_T("/makeitem"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/makeitem (�������ε���) (�����۰���) (��ȭ) (�����) (ĳ�����϶� �Ⱓ)"), _T("/makeitem (ItemIndex) (Count) (Enchant) (Potential) (Cash Period)") },
	{ GM_MakeCoin,		_T("/makecoin"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/makecoin (Coin)"), _T("/makecoin (Coin)") },
	{ GM_SetLevel,		_T("/setlevel"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setlevel (Level)"), _T("/setlevel (Level)") },
	{ GM_ChangeMap,		_T("/go"),			CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/go (MapID) (0-4 : ����, ����, �����, ������, ���)"), _T("/go (MapID) (0-4 : Easy, Normal, Hard, Master, Abyss)") },
	{ GM_SetQuest,		_T("/setquest"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setquest (QuestID) (0: �̿Ϸ�, 1: �Ϸ�)"), _T("/setquest (QuestID) (0: Not Yet, 1:Complete)") },
	{ GM_ZoneNotice,	_T("/zone"),		CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/zone (Notice)"), _T("/zone (Notice)") },
	{ GM_Trace,			_T("/trace"),		CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("����(/trace ĳ�����̸�),����(�Է��� ĳ�����̸��� ������ �����Ѵ�.)"), _T("Usage(/trace CharacterName), Description(Trace character's dungeon)") },
	{ GM_Warp,			_T("/warp"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/warp ĳ�����̸�),����(�Է��� ĳ�����̸��� ��ġ�� �̵��Ѵ�.���Ӽ��������� ��밡��)") },
	{ GM_Trace_AID,		_T("/trace_aid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_aid ��ī��Ʈ�����̵�),����(�Է��� ��ī��Ʈ���̵��� ������ �����Ѵ�.)"), _T("/trace_aid AccountDBID) - (Trace AccountDBID's position)") },
	{ GM_Trace_CID,		_T("/trace_cid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_cid ĳ���͵����̵�),����(�Է��� ĳ���͵����̵��� ������ �����Ѵ�.)"), _T("/trace_cid CharacterDBID) - (Trace CharacterDBID's position)") },
	{ GM_Recall,		_T("/recall"),		CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("����(/recall ĳ�����̸�),����(�Է��� ĳ���͸� GM������ ��ȯ�մϴ�."), _T("Usage(/recall CharacterName), Description(Recall character to GM Village") },
	{ GM_UserBan,		_T("/userban"),		CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,		_T("����(/userban ĳ�����̸�),����(�Է��� ĳ���͸� ���������Ų��."), _T("/userban CharacterName)") },
	{ GM_Fatigue,		_T("/fatigue"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/fatigue"), _T("/fatigue") },
	{ GM_DestroyPvP,	_T("/destroypvp"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/destroypvp"), _T("/destroypvp(PvP�ε���)(������ �̱���� ����̸�)") },
	{ GM_RepairAll,		_T("/repairall"),	CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,		_T("����(/repairall), ����(���������� �����մϴ�.)"), _T("Usage(/repairall), Description(Repaire all items your character has)") },
	{ GM_GCreate,		_T("/gcreate"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("����(/gcreate ����̸�), ����(��� â��)"), _T("Usage(/gcreate GuildName), Description(Create Guild)") },
	{ GM_GDismiss,		_T("/gdismiss"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("����(/gdismiss), ����(��� �ػ�)"), _T("Usage(/gdismiss), Description(Dismiss Guild)") },		
#elif defined (_TH)
	{ GM_Notice,		_T("/notice"),		CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,		_T("/notice (Notice)"), _T("/notice (Notice)")  },
	{ GM_WhisperOn,		_T("/w_on"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_on (CharacterName)"), _T("/w_on (CharacterName) - Whispser On") },
	{ GM_WhisperOff,	_T("/w_off"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_off (CharacterName)"), _T("/w_off (CharacterName) - Whisper Off") },
	{ GM_HideOn,		_T("/hideon"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideon"), _T("/hideon") },
	{ GM_HideOff,		_T("/hideoff"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideoff"), _T("/hideoff") },
	{ GM_SummonMob,		_T("/summonmob"),	CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,									_T("/summonmob (MonsterID) (MonsterCount)"), _T("/summonmob (MonsterID) (MonsterCount)") },
	{ GM_AllKillMob,	_T("/allkillmon"),	CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,									_T("/allkillmon"), _T("/allkillmon") },
	{ GM_MakeItem,		_T("/makeitem"),	CmdAuth_Master|CmdAuth_QA,														_T("/makeitem (�������ε���) (�����۰���) (��ȭ) (�����) (ĳ�����϶� �Ⱓ)"), _T("/makeitem (ItemIndex) (Count) (Enchant) (Potential) (Cash Period)") },
	{ GM_MakeCoin,		_T("/makecoin"),	CmdAuth_Master|CmdAuth_QA,														_T("/makecoin (Coin)"), _T("/makecoin (Coin)") },
	{ GM_SetLevel,		_T("/setlevel"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setlevel (Level)"), _T("/setlevel (Level)") },
	{ GM_ChangeMap,		_T("/go"),			CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/go (MapID) (0-4 : ����, ����, �����, ������, ���)"), _T("/go (MapID) (0-4 : Easy, Normal, Hard, Master, Abyss)") },
	{ GM_SetQuest,		_T("/setquest"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setquest (QuestID) (0: �̿Ϸ�, 1: �Ϸ�)"), _T("/setquest (QuestID) (0: Not Yet, 1:Complete)") },
	{ GM_ZoneNotice,	_T("/zone"),		CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/zone (Notice)"), _T("/zone (Notice)") },
	{ GM_Trace,			_T("/trace"),		CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("����(/trace ĳ�����̸�),����(�Է��� ĳ�����̸��� ������ �����Ѵ�.)"), _T("Usage(/trace CharacterName), Description(Trace character's dungeon)") },
	{ GM_Warp,			_T("/warp"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/warp ĳ�����̸�),����(�Է��� ĳ�����̸��� ��ġ�� �̵��Ѵ�.���Ӽ��������� ��밡��)") },
	{ GM_Trace_AID,		_T("/trace_aid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_aid ��ī��Ʈ�����̵�),����(�Է��� ��ī��Ʈ���̵��� ������ �����Ѵ�.)"), _T("/trace_aid AccountDBID) - (Trace AccountDBID's position)") },
	{ GM_Trace_CID,		_T("/trace_cid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_cid ĳ���͵����̵�),����(�Է��� ĳ���͵����̵��� ������ �����Ѵ�.)"), _T("/trace_cid CharacterDBID) - (Trace CharacterDBID's position)") },
	{ GM_Recall,		_T("/recall"),		CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("����(/recall ĳ�����̸�),����(�Է��� ĳ���͸� GM������ ��ȯ�մϴ�."), _T("Usage(/recall CharacterName), Description(Recall character to GM Village") },
	{ GM_UserBan,		_T("/userban"),		CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,		_T("����(/userban ĳ�����̸�),����(�Է��� ĳ���͸� ���������Ų��."), _T("/userban CharacterName)") },
	{ GM_Fatigue,		_T("/fatigue"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/fatigue"), _T("/fatigue") },
	{ GM_DestroyPvP,	_T("/destroypvp"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/destroypvp"), _T("/destroypvp(PvP�ε���)(������ �̱���� ����̸�)") },
	{ GM_RepairAll,		_T("/repairall"),	CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,		_T("����(/repairall), ����(���������� �����մϴ�.)"), _T("Usage(/repairall), Description(Repaire all items your character has)") },
	{ GM_GCreate,		_T("/gcreate"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("����(/gcreate ����̸�), ����(��� â��)"), _T("Usage(/gcreate GuildName), Description(Create Guild)") },
	{ GM_GDismiss,		_T("/gdismiss"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("����(/gdismiss), ����(��� �ػ�)"), _T("Usage(/gdismiss), Description(Dismiss Guild)") },
#elif defined (_ID)
	{ GM_Notice,		_T("/notice"),		CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,					_T("/notice (Notice)"), _T("/notice (Notice)")  },
	{ GM_WhisperOn,		_T("/w_on"),		CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_on (CharacterName)"), _T("/w_on (CharacterName) - Whispser On") },
	{ GM_WhisperOff,	_T("/w_off"),		CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_off (CharacterName)"), _T("/w_off (CharacterName) - Whisper Off") },
	{ GM_HideOn,		_T("/hideon"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideon"), _T("/hideon") },
	{ GM_HideOff,		_T("/hideoff"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideoff"), _T("/hideoff") },
	{ GM_SummonMob,		_T("/summonmob"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("/summonmob (MonsterID) (MonsterCount)"), _T("/summonmob (MonsterID) (MonsterCount)") },
	{ GM_AllKillMob,	_T("/allkillmon"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("/allkillmon"), _T("/allkillmon") },
	{ GM_MakeItem,		_T("/makeitem"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/makeitem (�������ε���) (�����۰���) (��ȭ) (�����) (ĳ�����϶� �Ⱓ)"), _T("/makeitem (ItemIndex) (Count) (Enchant) (Potential) (Cash Period)") },
	{ GM_MakeCoin,		_T("/makecoin"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/makecoin (Coin)"), _T("/makecoin (Coin)") },
	{ GM_SetLevel,		_T("/setlevel"),	CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,		_T("/setlevel (Level)"), _T("/setlevel (Level)") },
	{ GM_ChangeMap,		_T("/go"),			CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/go (MapID) (0-4 : ����, ����, �����, ������, ���)"), _T("/go (MapID) (0-4 : Easy, Normal, Hard, Master, Abyss)") },
	{ GM_SetQuest,		_T("/setquest"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setquest (QuestID) (0: �̿Ϸ�, 1: �Ϸ�)"), _T("/setquest (QuestID) (0: Not Yet, 1:Complete)") },
	{ GM_ZoneNotice,	_T("/zone"),		CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,					_T("/zone (Notice)"), _T("/zone (Notice)") },
	{ GM_Trace,			_T("/trace"),   	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("����(/trace ĳ�����̸�),����(�Է��� ĳ�����̸��� ������ �����Ѵ�.)"), _T("Usage(/trace CharacterName), Description(Trace character's dungeon)") },
	{ GM_Warp,			_T("/warp"),    	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/warp ĳ�����̸�),����(�Է��� ĳ�����̸��� ��ġ�� �̵��Ѵ�.���Ӽ��������� ��밡��)") },
	{ GM_Trace_AID,		_T("/trace_aid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_aid ��ī��Ʈ�����̵�),����(�Է��� ��ī��Ʈ���̵��� ������ �����Ѵ�.)"), _T("/trace_aid AccountDBID) - (Trace AccountDBID's position)") },
	{ GM_Trace_CID,		_T("/trace_cid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_cid ĳ���͵����̵�),����(�Է��� ĳ���͵����̵��� ������ �����Ѵ�.)"), _T("/trace_cid CharacterDBID) - (Trace CharacterDBID's position)") },
	{ GM_Recall,		_T("/recall"),		CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("����(/recall ĳ�����̸�),����(�Է��� ĳ���͸� GM������ ��ȯ�մϴ�."), _T("Usage(/recall CharacterName), Description(Recall character to GM Village") },
	{ GM_UserBan,		_T("/userban"),		CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,					_T("����(/userban ĳ�����̸�),����(�Է��� ĳ���͸� ���������Ų��."), _T("/userban CharacterName)") },
	{ GM_Fatigue,		_T("/fatigue"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/fatigue"), _T("/fatigue") },
	{ GM_DestroyPvP,	_T("/destroypvp"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/destroypvp(PvP�ε���)(������ �̱���� ����̸�)"), _T("/destroypvp(PvPIndex)(ForceWinGuildName)") },
#elif defined(_EU)		//���������� CmdAuth_New�� RGM�������� ����մϴ� 
	{ GM_Notice,		_T("/notice"),		CmdAuth_Master|CmdAuth_QA,														_T("/notice (Notice)"), _T("/notice (Notice)")  },
	{ GM_WhisperOn,		_T("/w_on"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_on (CharacterName)"), _T("/w_on (CharacterName) - Whispser On") },
	{ GM_WhisperOff,	_T("/w_off"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_off (CharacterName)"), _T("/w_off (CharacterName) - Whisper Off") },
	{ GM_HideOn,		_T("/hideon"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideon"), _T("/hideon") },
	{ GM_HideOff,		_T("/hideoff"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideoff"), _T("/hideoff") },
	{ GM_SummonMob,		_T("/summonmob"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("/summonmob (MonsterID) (MonsterCount)"), _T("/summonmob (MonsterID) (MonsterCount)") },
	{ GM_AllKillMob,	_T("/allkillmon"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("/allkillmon"), _T("/allkillmon") },
	{ GM_MakeItem,		_T("/makeitem"),	CmdAuth_Master|CmdAuth_QA,														_T("/makeitem (�������ε���) (�����۰���) (��ȭ) (�����) (ĳ�����϶� �Ⱓ)"), _T("/makeitem (ItemIndex) (Count) (Enchant) (Potential) (Cash Period)") },
	{ GM_MakeCoin,		_T("/makecoin"),	CmdAuth_Master|CmdAuth_QA,														_T("/makecoin (Coin)"), _T("/makecoin (Coin)") },
	{ GM_SetLevel,		_T("/setlevel"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setlevel (Level)"), _T("/setlevel (Level)") },
	{ GM_ChangeMap,		_T("/go"),			CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/go (MapID) (0-4 : ����, ����, �����, ������, ���)"), _T("/go (MapID) (0-4 : Easy, Normal, Hard, Master, Abyss)") },
	{ GM_SetQuest,		_T("/setquest"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setquest (QuestID) (0: �̿Ϸ�, 1: �Ϸ�)"), _T("/setquest (QuestID) (0: Not Yet, 1:Complete)") },
	{ GM_ZoneNotice,	_T("/zone"),		CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/zone (Notice)"), _T("/zone (Notice)") },
	{ GM_Trace,			_T("/trace"),   	CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("����(/trace ĳ�����̸�),����(�Է��� ĳ�����̸��� ������ �����Ѵ�.)"), _T("Usage(/trace CharacterName), Description(Trace character's dungeon)") },
	{ GM_Warp,			_T("/warp"),    	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/warp ĳ�����̸�),����(�Է��� ĳ�����̸��� ��ġ�� �̵��Ѵ�.���Ӽ��������� ��밡��)") },
	{ GM_Trace_AID,		_T("/trace_aid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_aid ��ī��Ʈ�����̵�),����(�Է��� ��ī��Ʈ���̵��� ������ �����Ѵ�.)"), _T("/trace_aid AccountDBID) - (Trace AccountDBID's position)") },
	{ GM_Trace_CID,		_T("/trace_cid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_cid ĳ���͵����̵�),����(�Է��� ĳ���͵����̵��� ������ �����Ѵ�.)"), _T("/trace_cid CharacterDBID) - (Trace CharacterDBID's position)") },
	{ GM_Recall,		_T("/recall"),		CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("����(/recall ĳ�����̸�),����(�Է��� ĳ���͸� GM������ ��ȯ�մϴ�."), _T("Usage(/recall CharacterName), Description(Recall character to GM Village") },
	{ GM_UserBan,		_T("/userban"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,		_T("����(/userban ĳ�����̸�),����(�Է��� ĳ���͸� ���������Ų��."), _T("/userban CharacterName)") },
	{ GM_Fatigue,		_T("/fatigue"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/fatigue"), _T("/fatigue") },
	{ GM_DestroyPvP,	_T("/destroypvp"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/destroypvp(PvP�ε���)(������ �̱���� ����̸�)"), _T("/destroypvp(PvPIndex)(ForceWinGuildName)") },
#else
	{ GM_Notice,		_T("/notice"),		CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,					_T("/notice (Notice)"), _T("/notice (Notice)")  },
	{ GM_WhisperOn,		_T("/w_on"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_on (CharacterName)"), _T("/w_on (CharacterName) - Whispser On") },
	{ GM_WhisperOff,	_T("/w_off"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/w_off (CharacterName)"), _T("/w_off (CharacterName) - Whisper Off") },
	{ GM_HideOn,		_T("/hideon"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideon"), _T("/hideon") },
	{ GM_HideOff,		_T("/hideoff"),		CmdAuth_Intern|CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/hideoff"), _T("/hideoff") },
	{ GM_SummonMob,		_T("/summonmob"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("/summonmob (MonsterID) (MonsterCount)"), _T("/summonmob (MonsterID) (MonsterCount)") },
	{ GM_AllKillMob,	_T("/allkillmon"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("/allkillmon"), _T("/allkillmon") },
	{ GM_MakeItem,		_T("/makeitem"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/makeitem (�������ε���) (�����۰���) (��ȭ) (�����) (ĳ�����϶� �Ⱓ)"), _T("/makeitem (ItemIndex) (Count) (Enchant) (Potential) (Cash Period)") },

	{ GM_MakeCoin,		_T("/makecoin"),	CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/makecoin (Coin)"), _T("/makecoin (Coin)") },
	{ GM_SetLevel,		_T("/setlevel"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setlevel (Level)"), _T("/setlevel (Level)") },
	{ GM_ChangeMap,		_T("/go"),			CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/go (MapID) (0-4 : ����, ����, �����, ������, ���)"), _T("/go (MapID) (0-4 : Easy, Normal, Hard, Master, Abyss)") },
	{ GM_SetQuest,		_T("/setquest"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/setquest (QuestID) (0: �̿Ϸ�, 1: �Ϸ�)"), _T("/setquest (QuestID) (0: Not Yet, 1:Complete)") },
	{ GM_ZoneNotice,	_T("/zone"),		CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,									_T("/zone (Notice)"), _T("/zone (Notice)") },
	{ GM_Trace,			_T("/trace"),		CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("����(/trace ĳ�����̸�),����(�Է��� ĳ�����̸��� ������ �����Ѵ�.)"), _T("Usage(/trace CharacterName), Description(Trace character's dungeon)") },
	{ GM_Warp,			_T("/warp"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/warp ĳ�����̸�),����(�Է��� ĳ�����̸��� ��ġ�� �̵��Ѵ�.���Ӽ��������� ��밡��)") },
	{ GM_Trace_AID,		_T("/trace_aid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_aid ��ī��Ʈ�����̵�),����(�Է��� ��ī��Ʈ���̵��� ������ �����Ѵ�.)"), _T("/trace_aid AccountDBID) - (Trace AccountDBID's position)") },
	{ GM_Trace_CID,		_T("/trace_cid"),	CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,	_T("/trace_cid ĳ���͵����̵�),����(�Է��� ĳ���͵����̵��� ������ �����Ѵ�.)"), _T("/trace_cid CharacterDBID) - (Trace CharacterDBID's position)") },
	{ GM_Recall,		_T("/recall"),		CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,					_T("����(/recall ĳ�����̸�),����(�Է��� ĳ���͸� GM������ ��ȯ�մϴ�."), _T("Usage(/recall CharacterName), Description(Recall character to GM Village") },
	{ GM_UserBan,		_T("/userban"),		CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,					_T("����(/userban ĳ�����̸�),����(�Է��� ĳ���͸� ���������Ų��."), _T("/userban CharacterName)") },
	{ GM_Fatigue,		_T("/fatigue"),		CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA,						_T("/fatigue"), _T("/fatigue") },
	{ GM_DestroyPvP,	_T("/destroypvp"),	CmdAuth_New|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,		_T("/destroypvp(PvP�ε���)(������ �̱���� ����̸�)"), _T("/destroypvp(PvPIndex)(ForceWinGuildName)") },
	{ GM_WEventClear,	_T("/clearevent"),	CmdAuth_Master|CmdAuth_QA|CmdAuth_Developer,									_T("/clearevent"), _T("/clearevent") },
#if defined( PRE_WORLDCOMBINE_PVP )
	{ GM_WORLDPVPROOMALLKILLREDTEAM,	_T("/teamred"),	CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA, _T("����(/TeamRed ĳ���͸�),����(�Է��� ĳ���͸� ���� �շ���Ų��."), _T("/TeamRed CharacterName...)") },
	{ GM_WORLDPVPROOMALLKILLBLUETEAM,	_T("/teamblue"),	CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA, _T("����(/Teamblue ĳ���͸�),����(�Է��� ĳ���͸� ���� �շ���Ų��."), _T("/Teamblue CharacterName...)") },
	{ GM_WORLDPVPROOMTOURNAMENTSETTING,	_T("/tournament"),	CmdAuth_New|CmdAuth_Intern|CmdAuth_Monitoring|CmdAuth_Master|CmdAuth_QA, _T("����(/tournament ����ȣ ĳ���͸�),����(�Է��� ĳ���͸� ���� �շ���Ų��."), _T("/tournament 1 CharacterName...)") },
#endif
#if defined( PRE_ADD_MUTE_USERCHATTING )
	{ GM_MuteUser, _T("/mute"), CmdAuth_Master|CmdAuth_QA, _T("����(/mute ĳ���͸� �ð�),����(�Է��� ĳ���͸� ������ �ð����� ħ����Ų��)"), _T("/mute CharacterName Mutetime") },	// [JPN]���� ü�� ����
#endif	// #if defined( PRE_ADD_MUTE_USERCHATTING )
#endif // _CH
};

CDNGMCommand::CDNGMCommand(CDNUserSession *pSession): m_pSession(pSession)
{
	BOOST_STATIC_ASSERT( GM_END == _countof(g_GMCmdList) );
	m_VecWhisperName.clear();
}

CDNGMCommand::~CDNGMCommand()
{

}

void CDNGMCommand::AddWhisperName(wstring wstrName)
{
	if (wstrName.size() == 0) return;
	if (IsWhisperName(wstrName.c_str())) return;	// �̹� �ִ�

#if defined(_VILLAGESERVER)
	CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByName(wstrName.c_str());
	if (!pUserObj) return;
#elif defined(_GAMESERVER)
	
#endif
	m_VecWhisperName.push_back(wstrName.c_str());
}

void CDNGMCommand::DelWhisperName(wstring wstrName)
{
	if (m_VecWhisperName.empty()) return;
	
	VecWString::iterator iter = find(m_VecWhisperName.begin(), m_VecWhisperName.end(), wstrName.c_str());
	if (iter != m_VecWhisperName.end())
		m_VecWhisperName.erase(iter);
}

bool CDNGMCommand::IsWhisperName(wstring wstrName)
{
	if (m_VecWhisperName.empty()) return false;

	VecWString::iterator iter = find(m_VecWhisperName.begin(), m_VecWhisperName.end(), wstrName.c_str());
	if (iter != m_VecWhisperName.end()) return true;

	return false;
}


bool CDNGMCommand::GMCommand(wstring& wszCommand)
{
	if ( wszCommand.find(L"/") != 0 ) return false;

	wstring szResultMsg;
	VecWString tokens;

	TokenizeW(wszCommand, tokens, L" ");
	ToLowerW(tokens[0]);
	int nCount = sizeof(g_GMCmdList) / sizeof(__GMCmdLineDefineStruct);

	for ( int i = 0 ; i < nCount; i++ )
	{
		if( wcscmp( tokens[0].c_str(), g_GMCmdList[i].szString ) == NULL )
		{
			if (!OnGMCommand(i, tokens)){
				szResultMsg = L"Command param Error";
				m_pSession->SendChat(CHATTYPE_SYSTEM, (int)szResultMsg.size()*sizeof(WCHAR), m_pSession->GetCharacterName(), (WCHAR*)szResultMsg.c_str());
			}

			return true;
		}
	}

	return false;
}

bool CDNGMCommand::OnGMCommand(int nCmdID, VecWString& tokens)
{
	wstring wStr;

	// ���� �˻�

	int nCheckMask = 0;
	switch( m_pSession->GetAccountLevel() )
	{
	case AccountLevel_New :			nCheckMask = CmdAuth_New; break;
	case AccountLevel_Intern :		nCheckMask = CmdAuth_Intern; break;
	case AccountLevel_Monitoring :	nCheckMask = CmdAuth_Monitoring; break;
	case AccountLevel_Master :		nCheckMask = CmdAuth_Master; break;
	case AccountLevel_QA :			nCheckMask = CmdAuth_QA; break;
	case AccountLevel_Developer :	nCheckMask = CmdAuth_Developer; break;		
	}
	if ( !(nCheckMask & g_GMCmdList[nCmdID].nAccountLevel) )
	{
		wStr = FormatW( L"Do not have permission");
		m_pSession->SendChat(CHATTYPE_SYSTEM, (int)wStr.size()*sizeof(WCHAR), m_pSession->GetCharacterName(), (WCHAR*)wStr.c_str());
		return true;
	}	

	switch( nCmdID )
	{
		case GM_Help:
		{
			for ( int i = 0 ; i < GM_END ; i++ )
			{
#if defined (_KR) || defined(_KRAZ) || defined (_WORK)
				wStr = FormatW( L"[%s] - %s", g_GMCmdList[i].szString, g_GMCmdList[i].szComment);
#else
				wStr = FormatW( L"[%s] - %s", g_GMCmdList[i].szString, g_GMCmdList[i].szCommentEn);
#endif
				m_pSession->SendChat(CHATTYPE_SYSTEM, (int)wStr.size()*sizeof(WCHAR), m_pSession->GetCharacterName(), (WCHAR*)wStr.c_str());
			}
		}
		break;
#if defined(_CH)
		case GM_HideOn: return CmdHide(tokens, true);
		case GM_HideOff: return CmdHide(tokens, false);
		case GM_ChangeMap: return CmdChangeMap(tokens);		// ���̵�
		case GM_Trace: return CmdTrace(TRACE_CHARACTERNAME, tokens );
		case GM_Trace_AID: return CmdTrace(TRACE_ACCOUNTDBID, tokens );
		case GM_Trace_CID: return CmdTrace(TRACE_CHRACTERDBID, tokens );
		case GM_Warp: return CmdWarp( tokens );
		case GM_Recall: return CmdRecall( tokens );
		case GM_UserBan: return CmdBanUser(tokens);
		case GM_Fatigue: return CmdFatigue(tokens);
		case GM_HP: return CmdHp(tokens);
		case GM_WEventClear: return CmdWeeklyEventClear();

#elif defined (_US)
		case GM_HideOn: return CmdHide(tokens, true);
		case GM_HideOff: return CmdHide(tokens, false);
		case GM_ChangeMap: return CmdChangeMap(tokens);		
		case GM_Trace: return CmdTrace(TRACE_CHARACTERNAME, tokens );
		case GM_UserBan: return CmdBanUser(tokens);
		case GM_SummonMob: return CmdSummonMob(tokens);
		case GM_AllKillMob: return CmdAllkillMob(tokens);
		case GM_ZoneNotice: return CmdZoneNotice(tokens);	
		case GM_HP: return CmdHp(tokens);
		case GM_SetLevel: return CmdSetLevel(tokens);		// ����
		case GM_MakeItem: return CmdMakeItem(tokens);
		case GM_Recall: return CmdRecall( tokens );
		case GM_Notice: return CmdNotice(tokens);
		case GM_MakeCoin: return CmdMakeCoin(tokens);		// ��������
		case GM_WhisperOn: return CmdWhisper(tokens, true);
		case GM_WhisperOff: return CmdWhisper(tokens, false);
		case GM_Warp: return CmdWarp( tokens );
#elif defined( _SG ) || defined (_TH)
		case GM_Notice: return CmdNotice(tokens);
		case GM_WhisperOn: return CmdWhisper(tokens, true);
		case GM_WhisperOff: return CmdWhisper(tokens, false);
		case GM_HideOn: return CmdHide(tokens, true);
		case GM_HideOff: return CmdHide(tokens, false);
		case GM_SummonMob: return CmdSummonMob(tokens);
		case GM_AllKillMob: return CmdAllkillMob(tokens);
		case GM_MakeItem: return CmdMakeItem(tokens);
		case GM_MakeCoin: return CmdMakeCoin(tokens);		// ��������
		case GM_SetLevel: return CmdSetLevel(tokens);		// ����
		case GM_ChangeMap: return CmdChangeMap(tokens);		// ���̵�
		case GM_SetQuest: return CmdSetQuest(tokens);		// ����Ʈ����
		case GM_ZoneNotice: return CmdZoneNotice(tokens);
		case GM_Trace: return CmdTrace(TRACE_CHARACTERNAME, tokens );
		case GM_Trace_AID: return CmdTrace(TRACE_ACCOUNTDBID, tokens );
		case GM_Trace_CID: return CmdTrace(TRACE_CHRACTERDBID, tokens );
		case GM_Warp: return CmdWarp( tokens );
		case GM_Recall: return CmdRecall( tokens );
		case GM_UserBan: return CmdBanUser(tokens);
		case GM_Fatigue: return CmdFatigue(tokens);
		case GM_DestroyPvP: return CmdDestroyPvP(tokens);
		case GM_RepairAll: return CmdRepairAll(tokens);
		case GM_GCreate: return CmdGCreate(tokens);
		case GM_GDismiss: return CmdGDismiss(tokens);
#elif defined (_ID)
		case GM_Notice: return CmdNotice(tokens);			
		case GM_WhisperOn: return CmdWhisper(tokens, true);
		case GM_WhisperOff: return CmdWhisper(tokens, false);
		case GM_HideOn: return CmdHide(tokens, true);
		case GM_HideOff: return CmdHide(tokens, false);
		case GM_SummonMob: return CmdSummonMob(tokens);
		case GM_AllKillMob: return CmdAllkillMob(tokens);
		case GM_MakeItem: return CmdMakeItem(tokens);
		case GM_MakeCoin: return CmdMakeCoin(tokens);		// ��������
		case GM_SetLevel: return CmdSetLevel(tokens);		// ����
		case GM_ChangeMap: return CmdChangeMap(tokens);		// ���̵�
		case GM_SetQuest: return CmdSetQuest(tokens);		// ����Ʈ����
		case GM_ZoneNotice: return CmdZoneNotice(tokens);
		case GM_Trace: return CmdTrace(TRACE_CHARACTERNAME, tokens );
		case GM_Warp: return CmdWarp( tokens );
		case GM_Trace_AID: return CmdTrace(TRACE_ACCOUNTDBID, tokens );
		case GM_Trace_CID: return CmdTrace(TRACE_CHRACTERDBID, tokens );
		case GM_Recall: return CmdRecall( tokens );
		case GM_UserBan: return CmdBanUser(tokens);
		case GM_Fatigue: return CmdFatigue(tokens);
		case GM_DestroyPvP: return CmdDestroyPvP(tokens);
#else
		case GM_Notice: return CmdNotice(tokens);
		case GM_WhisperOn: return CmdWhisper(tokens, true);
		case GM_WhisperOff: return CmdWhisper(tokens, false);
		case GM_HideOn: return CmdHide(tokens, true);
		case GM_HideOff: return CmdHide(tokens, false);
		case GM_SummonMob: return CmdSummonMob(tokens);
		case GM_AllKillMob: return CmdAllkillMob(tokens);
		case GM_MakeItem: return CmdMakeItem(tokens);
		case GM_MakeCoin: return CmdMakeCoin(tokens);		// ��������
		case GM_SetLevel: return CmdSetLevel(tokens);		// ����
		case GM_ChangeMap: return CmdChangeMap(tokens);		// ���̵�
		case GM_SetQuest: return CmdSetQuest(tokens);		// ����Ʈ����
		case GM_ZoneNotice: return CmdZoneNotice(tokens);
		case GM_Trace: return CmdTrace(TRACE_CHARACTERNAME, tokens );
		case GM_Trace_AID: return CmdTrace(TRACE_ACCOUNTDBID, tokens );
		case GM_Trace_CID: return CmdTrace(TRACE_CHRACTERDBID, tokens );
		case GM_Warp: return CmdWarp( tokens );
		case GM_Recall: return CmdRecall( tokens );
		case GM_UserBan: return CmdBanUser(tokens);
		case GM_Fatigue: return CmdFatigue(tokens);
		case GM_DestroyPvP: return CmdDestroyPvP(tokens);
		case GM_WEventClear: return CmdWeeklyEventClear();
#if defined( PRE_WORLDCOMBINE_PVP )
		case GM_WORLDPVPROOMALLKILLREDTEAM: return CmdWorldPvPRoomAllKillTeamSetting(PvPCommon::Team::B, tokens);
		case GM_WORLDPVPROOMALLKILLBLUETEAM: return CmdWorldPvPRoomAllKillTeamSetting(PvPCommon::Team::A, tokens);
		case GM_WORLDPVPROOMTOURNAMENTSETTING: return CmdWorldPvPRoomTournamentSetting(tokens);
#endif
#if defined( PRE_ADD_MUTE_USERCHATTING )
		case GM_MuteUser: return CmdMuteUser(tokens);
#endif	// #if defined( PRE_ADD_MUTE_USERCHATTING )
#endif	// #if defined(_CH)
	}

	return true;
}

bool CDNGMCommand::CmdNotice(VecWString& tokens)
{
	if (tokens.size() < 2) return false;

	WCHAR szMsg[CHATLENMAX] = { 0, };
	int nLen = 0;
	for (int i = 1; i < (int)tokens.size(); i++)
	{
		int nTempLen = (int)wcslen(tokens[i].c_str());
		if (nTempLen + nLen >= CHATLENMAX) return false;
		_wcscpy(szMsg+nLen, CHATLENMAX-nLen, tokens[i].c_str(), nTempLen);
		nLen += nTempLen;
		szMsg[nLen++] = ' ';
	}
	if( nLen < CHATLENMAX )
		szMsg[nLen] = '\0';
	else
		return false;

#if defined (_VILLAGESERVER)
	g_pMasterConnection->SendNoticeFromClient(szMsg, nLen);
#elif defined (_GAMESERVER)
	g_pMasterConnectionManager->SendNoticeFromClinet(szMsg, nLen);
#endif
	return true;
}

bool CDNGMCommand::CmdWhisper(VecWString& tokens, bool bWhisper)
{
	if (tokens.size() < 2) return false;

	if (bWhisper){	// �ӼӸ� ����
		AddWhisperName(tokens[1]);
	}
	else {	// �ӼӸ� ����
		DelWhisperName(tokens[1]);
	}

	return true;
}

bool CDNGMCommand::CmdHide(VecWString& tokens, bool bHide)
{
	if (tokens.size() < 1) return false;
	m_pSession->SetHide(bHide);

	return true;
}

bool CDNGMCommand::CmdSummonMob(VecWString& tokens)
{
#if defined(_GAMESERVER)
	if ( tokens.size() != 3 ) return false;

	int nMonsterID = _ttoi(tokens[1].c_str());
	int nCount = _ttoi(tokens[2].c_str());

	CDNGameRoom *pGameRoom = m_pSession->GetGameRoom();
	if (!pGameRoom) return false;
	if (!pGameRoom->GetTaskMng()) return false;

	CDnGameTask *pTask = (CDnGameTask *)pGameRoom->GetTaskMng()->GetTask( "GameTask" );
	if (!pTask) return false;

	EtVector3* pvPos = m_pSession->GetActorHandle()->GetPosition();

	for ( int i  = 0 ; i < nCount ; i++ )
	{
		pTask->RequestGenerationMonsterFromMonsterID( nMonsterID, *pvPos, EtVector3( 0.f, 0.f, 0.f ), EtVector3( 0.f, 0.f, 0.f ));
	}
#endif
	return true;
}

bool CDNGMCommand::CmdAllkillMob(VecWString& tokens)
{
#if defined(_GAMESERVER)
	CDNGameRoom *pGameRoom = m_pSession->GetGameRoom();
	if (!pGameRoom) return false;
	if (!pGameRoom->GetTaskMng()) return false;

	CDnGameTask *pTask = (CDnGameTask *)pGameRoom->GetTaskMng()->GetTask( "GameTask" );
	if (!pTask) return false;

	pTask->RequestDestroyAllMonster(false);
#endif		
	return true;
}

bool CDNGMCommand::CmdMakeItem(VecWString& tokens)
{
	wstring wszString;

	if ( tokens.size() < 2 ){
		wszString = FormatW(L"�Ķ���͸� ����� Ȯ�����ּ���!!!\r\n");
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
		return false;
	}

	int nItemID = _wtoi(tokens[1].c_str());

	int nCount = ( tokens.size() >= 3 ) ? _wtoi(tokens[2].c_str()) : 1;
	if (nCount <= 0) nCount = 1;

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;

	if (pItemData->nType == ITEMTYPE_QUEST){
		m_pSession->GetItem()->AddQuestInventory(nItemID, nCount, 0, DBDNWorldDef::AddMaterializedItem::Cheat);
		return true;
	}

	int nItemLevel = ( tokens.size() >= 4 ) ? _wtoi(tokens[3].c_str()) : 0;
	int nItemPotential = ( tokens.size() >= 5 ) ? _wtoi(tokens[4].c_str()) : 0;
	int nItemOption = ( tokens.size() >= 6 ) ? _wtoi(tokens[5].c_str()) : 0;

	if (pItemData->IsCash){
		int nPeriod = ( tokens.size() >= 7 ) ? _wtoi(tokens[6].c_str()) : 0;
		//if (nPeriod <= 0) nPeriod = 1;

		m_pSession->GetItem()->CreateCashInvenItemByCheat(nItemID, nCount, nPeriod, DBDNWorldDef::AddMaterializedItem::Cheat);
		return true;
	}

#if defined(_VILLAGESERVER)
	if ((nItemLevel > 0) || (nItemPotential > 0) || (nItemOption > 0))
	{
		TItem Item = { 0, };
		if( CDNUserItem::MakeItemStruct(nItemID, Item) == false )
			return false;

		Item.wCount = nCount;
		Item.cLevel = nItemLevel;
		Item.cPotential = nItemPotential;
		Item.cOption = nItemOption;

		m_pSession->GetItem()->CreateInvenWholeItem(Item, DBDNWorldDef::AddMaterializedItem::Cheat, 0);
	}
	else
	{
		m_pSession->GetItem()->CreateInvenItem1( nItemID, nCount, -1, -1, DBDNWorldDef::AddMaterializedItem::Cheat, 0, CREATEINVEN_ETC );
	}

#elif defined(_GAMESERVER)

	if( !m_pSession || !m_pSession->GetActorHandle() ) return false;
	CDNGameRoom *pRoom = m_pSession->GetGameRoom();
	if (!pRoom) return false;
	if (!pRoom->GetTaskMng()) return false;

	CDnItemTask *pTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask( "ItemTask" );
	if (!pTask) return false;

	CMultiRoom *pMultiRoom = pTask->GetRoom();
	pTask->RequestDropItem(STATIC_INSTANCE_(CDnDropItem::s_dwUniqueCount)++, *(m_pSession->GetActorHandle()->GetPosition()), nItemID, _rand(pMultiRoom), nCount, 0);

	char cGrade = 0;
	if (pItemData) cGrade = pItemData->cRank;

#endif
	return true;
}
bool CDNGMCommand::Cmdhuanhua(VecWString& tokens)
{

	wstring wszString;

	if (tokens.size() < 2) {
		wszString = FormatW(L"�������Ʒ��������");
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size() * sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
		return false;
	}
	int nItemID = _wtoi(tokens[1].c_str());

	if (m_pSession->GetItem()->GetInventoryItemCount(335545346) <= 1) 
	{
		wszString = FormatW(L"������Ҫ���š��û����ǡ�");
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size() * sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
		return false;
	}

	m_pSession->GetItem()->DeleteInventoryByItemID(335545346,2,0,0);


	WCHAR szMsg[CHATLENMAX] = { 0, };
	wsprintfW(szMsg, L"���[%ws]���ڻû�����", m_pSession->GetCharacterName());


//	g_pUserSessionManager->SendChat(static_cast<eChatType>(007), 50, m_pSession->GetCharacterName(), szMsg);






	int nCount = (tokens.size() >= 3) ? _wtoi(tokens[2].c_str()) : 1;
	if (nCount <= 0) nCount = 1;

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;

	if (pItemData->nType == ITEMTYPE_QUEST) {
		m_pSession->GetItem()->AddQuestInventory(nItemID, nCount, 0, DBDNWorldDef::AddMaterializedItem::Cheat);
		return true;
	}

	int nItemLevel = (tokens.size() >= 4) ? _wtoi(tokens[3].c_str()) : 0;
	int nItemPotential = (tokens.size() >= 5) ? _wtoi(tokens[4].c_str()) : 0;
	int nItemOption = (tokens.size() >= 6) ? _wtoi(tokens[5].c_str()) : 0;

	if (pItemData->IsCash) {
		int nPeriod = (tokens.size() >= 7) ? _wtoi(tokens[6].c_str()) : 0;
		//if (nPeriod <= 0) nPeriod = 1;

		m_pSession->GetItem()->CreateCashInvenItemByCheat(nItemID, nCount, nPeriod, DBDNWorldDef::AddMaterializedItem::Cheat);
		return true;
	}

#if defined(_VILLAGESERVER)
	if ((nItemLevel > 0) || (nItemPotential > 0) || (nItemOption > 0))
	{
		TItem Item = { 0, };
		if (CDNUserItem::MakeItemStruct(nItemID, Item) == false)
			return false;

		Item.wCount = nCount;
		Item.cLevel = nItemLevel;
		Item.cPotential = nItemPotential;
		Item.cOption = nItemOption;

		m_pSession->GetItem()->CreateInvenWholeItem(Item, DBDNWorldDef::AddMaterializedItem::Cheat, 0);
	}
	else
	{
		m_pSession->GetItem()->CreateInvenItem1(nItemID, nCount, -1, -1, DBDNWorldDef::AddMaterializedItem::Cheat, 0, CREATEINVEN_ETC);
	}

#elif defined(_GAMESERVER)

	if (!m_pSession || !m_pSession->GetActorHandle()) return false;
	CDNGameRoom *pRoom = m_pSession->GetGameRoom();
	if (!pRoom) return false;
	if (!pRoom->GetTaskMng()) return false;

	CDnItemTask *pTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask("ItemTask");
	if (!pTask) return false;

	CMultiRoom *pMultiRoom = pTask->GetRoom();
	pTask->RequestDropItem(STATIC_INSTANCE_(CDnDropItem::s_dwUniqueCount)++, *(m_pSession->GetActorHandle()->GetPosition()), nItemID, _rand(pMultiRoom), nCount, 0);

	char cGrade = 0;
	if (pItemData) cGrade = pItemData->cRank;

#endif
	return true;
}
bool CDNGMCommand::CmdMakeCoin(VecWString& tokens)
{
	if (tokens.size() != 2)	return false;
	if (!m_pSession->AddCoin(_ttoi64(tokens[1].c_str()), DBDNWorldDef::CoinChangeCode::Cheat, 0)) return false;
	return true;
}

bool CDNGMCommand::CmdSetLevel(VecWString& tokens)
{
	if (tokens.size() != 2) return false;

	int nLevel = _wtoi(tokens[1].c_str());
	if (nLevel < 1 || nLevel > CHARLEVELMAX) return false;

#if defined(_VILLAGESERVER)
	int nExp = g_pDataManager->GetExp(m_pSession->GetUserJob(), nLevel);
	m_pSession->SetExp(1, DBDNWorldDef::CharacterExpChangeCode::Cheat, 0, false);
	m_pSession->SetLevel(1, DBDNWorldDef::CharacterLevelChangeCode::Cheat, false);	
	m_pSession->SetExp(nExp, DBDNWorldDef::CharacterExpChangeCode::Cheat, 0, true);	// exp�������ָ� �������� �˾Ƽ� �ٲ��

#elif defined(_GAMESERVER)
	int nExp = g_pDataManager->GetExp(m_pSession->GetUserJob(), nLevel);
	m_pSession->GetPlayerActor()->SetExperience( 1 );
	m_pSession->GetActorHandle()->SetLevel( 1 );
	m_pSession->GetPlayerActor()->SetExperience(nExp);		// exp �������ָ� �˾Ƽ� �ٲ��.
#endif

	return true;
}

bool CDNGMCommand::CmdChangeMap(VecWString& tokens)
{
	if ( tokens.size() < 2 ) return false;
	if (_ttoi(tokens[1].c_str()) < 1) return false;

	wstring wszString;

#if defined(_VILLAGESERVER)
	CDNField* pField = m_pSession->GetField();
	if( pField )
	{
		if( pField->bIsPvPVillage() || pField->bIsPvPLobby() || pField->bIsDarkLairVillage() )
		{
			wszString = FormatW(L"PvP����, PvP�κ�, ��ũ����� ������ �� ��ɾ ����� �� �����ϴ�.");
			m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
			return true;
		}
	}

#elif defined(_GAMESERVER)
	if( m_pSession->GetGameRoom() )
	{
		if( m_pSession->GetGameRoom()->bIsPvPRoom() )
		{
			wszString = FormatW(L"PvP�߿��� �� ��ɾ ����� �� �����ϴ�.");
			m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
			return true;
		}
	}
#endif

	int nMapIndex = _ttoi(tokens[1].c_str());	// �̵��� mapindex
	TDUNGEONDIFFICULTY StageConstrucionLevel = ( tokens.size() <= 2 ) ? Dungeon::Difficulty::Easy : static_cast<TDUNGEONDIFFICULTY>(_ttoi(tokens[2].c_str()));
	if (StageConstrucionLevel >= Dungeon::Difficulty::Max) 
		return false;

	int nGateNo = ( tokens.size() <= 3 ) ? 1 : _ttoi(tokens[3].c_str());

	if ( g_pDataManager->GetMapType( nMapIndex ) == GlobalEnum::MAP_UNKNOWN )
	{
		std::wstring wszLocalString;
		wszLocalString = FormatW( L"Cannot found map index %d ",  nMapIndex );
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszLocalString.size()*sizeof(WCHAR), L"", (WCHAR*)wszLocalString.c_str());
		return false;
	}

#if defined(_VILLAGESERVER)
	if (g_pDataManager->GetMapType(nMapIndex) != GlobalEnum::MAP_VILLAGE){		// �׼��� ���
		char cGateNo = g_pFieldDataManager->GetRandomGateNo(nMapIndex);

		if( m_pSession->GetPartyID() > 0 ) {
			CDNParty *pParty = g_pPartyManager->GetParty(m_pSession->GetPartyID());
			if (!pParty) return false;
			pParty->SetRandomSeed( timeGetTime() );

			if (g_pMasterConnection && g_pMasterConnection->GetActive())
				g_pMasterConnection->SendReqGameID( GameTaskType::Normal, REQINFO_TYPE_PARTY, pParty->GetPartyID(), m_pSession->GetChannelID(), pParty->GetRandomSeed(), nMapIndex, nGateNo, StageConstrucionLevel, true, pParty );		// GameID�� ��û
			else
				return false;
		}
		else {
			m_pSession->m_cGateNo = nGateNo;
			m_pSession->m_nSingleRandomSeed = timeGetTime();

			if (g_pMasterConnection && g_pMasterConnection->GetActive())
				g_pMasterConnection->SendReqGameID( GameTaskType::Normal, REQINFO_TYPE_SINGLE, m_pSession->GetAccountDBID(), m_pSession->GetChannelID(), m_pSession->m_nSingleRandomSeed, nMapIndex, nGateNo, StageConstrucionLevel, true );	// GameID�� ��û
			else
				return false;
		}
	}
	else
	{	// ���������� ���
		char cGateNo = ( tokens.size() == 2 ) ? -1 : _ttoi(tokens[2].c_str());
		if( m_pSession->GetPartyID() > 0 ) {
			CDNParty *pParty = g_pPartyManager->GetParty(m_pSession->GetPartyID());
			if (!pParty) return false;
			if (g_pMasterConnection && g_pMasterConnection->GetActive())
				g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_PARTY, pParty->GetPartyID(), nMapIndex, cGateNo, -1, pParty);
			else
				return false;
		}
		else
		{
			if (g_pMasterConnection && g_pMasterConnection->GetActive())
				g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_SINGLE, m_pSession->GetAccountDBID(), nMapIndex, cGateNo);
			else
				return false;
		}
	}

#elif defined(_GAMESERVER)
	CDNGameRoom *pRoom = m_pSession->GetGameRoom();
	if (!pRoom) return false;
	if( pRoom->bIsPvPRoom() )	// PvP�濡���� changemap ġƮ ���´�.
		return false;

	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( g_pDataManager->GetMapType( nMapIndex ) == GlobalEnum::eMapTypeEnum::MAP_VILLAGE ) {

		CDNUserSession *pLeaderSession = NULL;
		for( DWORD i=0; i<pRoom->GetUserCount(); i++ ) {
			CDNUserSession *pSession = pRoom->GetUserData(i);
			pSession->SetNextVillageData( NULL, 0, pTask->GetMapTableID(), nMapIndex, nGateNo );

			if( pRoom->GetPartyData(i)->bLeader ) pLeaderSession = pSession;
		}

		if( pLeaderSession ) {
			g_pMasterConnectionManager->SendRequestNextVillageInfo(pLeaderSession->GetWorldSetID(), nMapIndex, nGateNo, true, pRoom );
		}
	}
	else {
		for( DWORD i=0; i<pRoom->GetUserCount(); i++ )
		{
			CDNUserSession *pSession = pRoom->GetUserData(i);
			if( !pSession ) continue;
			DnActorHandle hActor = pSession->GetActorHandle();
			if( !hActor ) continue;
			CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>(hActor.GetPointer());
			pPlayer->CmdRemoveStateEffect(STATE_BLOW::BLOW_099);
		}
#if defined(PRE_FIX_INITSTATEANDSYNC)
		pRoom->ReserveInitStateAndSync( nMapIndex, nGateNo, CRandom::Seed(pRoom), StageConstrucionLevel, true );
#else
		pRoom->InitStateAndSync( nMapIndex, nGateNo, CRandom::Seed(pRoom), StageConstrucionLevel, true );
#endif	//#if defined(PRE_FIX_INITSTATEANDSYNC)
	}
#endif

	return true;
}

bool CDNGMCommand::CmdSetQuest(VecWString& tokens)
{
	if (tokens.size() != 4) return false;

	int nQuestID = _wtoi(tokens[1].c_str());
	int nFlag = _wtoi(tokens[2].c_str());
	if (nFlag < 0 || nFlag > 1) return false;

	int nResult = 0;
	if (nFlag == 1){
		nResult = m_pSession->GetQuest()->MarkingCompleteQuest(nQuestID);
		if ( nResult < 0 )
			g_Log.Log(LogType::_ERROR, m_pSession, L"GM_SetQuest Error: %d MarkingCompleteQuest (Ret:%d)\r\n", nQuestID, nResult);
	}

	nResult = m_pSession->GetQuest()->CompleteQuest(nQuestID, true, true);

	if (nResult < 0)
		g_Log.Log(LogType::_ERROR, m_pSession, L"GM_SetQuest Error: %d CompleteQuest (Ret:%d)", nQuestID, nResult);

	return true;
}

bool CDNGMCommand::CmdZoneNotice(VecWString& tokens)
{
	if (tokens.size() < 2) return false;

	WCHAR szMsg[CHATLENMAX] = { 0, };
	int nLen = 0;
	for (int i = 1; i < (int)tokens.size(); i++)
	{
		int nTempLen = (int)wcslen(tokens[i].c_str());
		if (nTempLen + nLen >= CHATLENMAX) return false;
		_wcscpy(szMsg+nLen, CHATLENMAX-nLen, tokens[i].c_str(), nTempLen);
		nLen += nTempLen;
		szMsg[nLen++] = ' ';
	}
	if( nLen < CHATLENMAX )
		szMsg[nLen] = '\0';
	else
		return false;

#if defined (_VILLAGESERVER)
	g_pMasterConnection->SendZoneNoticeFromClient(m_pSession->GetMapIndex(), szMsg, nLen);
#endif
	return true;
}

bool CDNGMCommand::CmdFatigue(VecWString& tokens)
{
	if (tokens.size() < 1) return false;
	m_pSession->SetCheatMaxFatigue();
	return true;
}

bool CDNGMCommand::CmdHp(VecWString& tokens)
{
#if defined(_GAMESERVER)
	wstring wszString;

	if (tokens.size() < 2){
		wszString = FormatW(L"HP ��(����0~100)�� �־��ּ���.\r\n");
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
		return false;
	}
	CDnPlayerActor *pPlayer = m_pSession->GetPlayerActor();
	if( !pPlayer ) return false;
	INT64 nHP = (INT64)( pPlayer->GetMaxHP() * ( _ttoi( tokens[1].c_str() ) / 100.f ) );
	if( nHP > pPlayer->GetMaxHP() ) nHP = pPlayer->GetMaxHP();
	else if( nHP < 1 ) nHP = 1;
	pPlayer->CmdRefreshHPSP( nHP, pPlayer->GetSP() );
#endif

	return true;
}

bool CDNGMCommand::CmdDestroyPvP(VecWString& tokens)
{
#if defined(_VILLAGESERVER)

	wstring wszString;

	if (tokens.size() < 2){
		wszString = FormatW(L"PvP���� �ε����� �־��ּ���.\r\n");
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
		return false;
	}

	g_pMasterConnection->SendForceDestroyPvP(m_pSession->GetAccountDBID(), _wtoi(tokens[1].c_str()), tokens.size() > 2 ? tokens[2].c_str() : NULL);	

#endif		//#if defined(_VILLAGESERVER)
	return true;
}

bool CDNGMCommand::CmdTrace( int nType, const VecWString& tokens )
{
#if defined( _VILLAGESERVER )

	if( tokens.size() != 2 )
	{
		m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wcslen(g_GMCmdList[GM_Trace].szComment)), L"", g_GMCmdList[GM_Trace].szComment );
		return false;
	}

	bool bIsSelf = true;
	switch (nType)
	{
	case TRACE_CHARACTERNAME:
		{
			bIsSelf = (wcscmp( m_pSession->GetCharacterName(), tokens[1].c_str() ) == 0) ? true : false;
			break;
		}

	case TRACE_ACCOUNTDBID:
		{
			bIsSelf = (m_pSession->GetAccountDBID() == _wtoi(tokens[1].c_str())) ? true : false;
			break;
		}

	case TRACE_CHRACTERDBID:
		{
			bIsSelf = (m_pSession->GetCharacterDBID() == _wtoi64(tokens[1].c_str())) ? true : false;
			break;
		}
	}

	if (bIsSelf)
	{
		std::wstring wString = L"�ڱ� �ڽ��� ������ �� �����ϴ�..";
		m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wString.size()), L"", wString.c_str() );
		return false;
	}

	if( m_pSession->GetPartyID() > 0 )
	{
		std::wstring wString = L"��Ƽ ���¿����� ����� �� ���� ġƮ�Դϴ�.";
		m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wString.size()), L"", wString.c_str() );
		return false;
	}

	if( !g_pMasterConnection || !g_pMasterConnection->GetActive() )
		return false;

#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnection->SendTraceBreakInto( nType, m_pSession->GetAccountDBID(), tokens[1].c_str(), m_pSession->m_eSelectedLanguage );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnection->SendTraceBreakInto( nType, m_pSession->GetAccountDBID(), tokens[1].c_str() );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

#else
	//std::wstring wString = boost::io::str( boost::wformat( L"���������� ��밡���� ġƮ�Դϴ�.") );
#if defined(PRE_ADD_MULTILANGUAGE)
	std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4429, m_pSession->m_eSelectedLanguage) ) );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4429) ) );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wString.size()), L"", wString.c_str() );
	return false;
#endif // #if defined( _VILLAGESERVER )

	return true;
}

bool CDNGMCommand::CmdWarp( const VecWString& tokens )
{
#if defined( _GAMESERVER )

	if( tokens.size() != 2 )
	{
		//m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wcslen(g_GMCmdList[GM_Warp].szComment)), L"", g_GMCmdList[GM_Warp].szComment );
		return false;
	}

	CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
	if( pGameRoom == NULL )
		return false;

	CDNGameRoom::PartyStruct* pStruct = pGameRoom->GetPartyData( const_cast<WCHAR*>(tokens[1].c_str()) );
	if( pStruct == NULL || pStruct->pSession == NULL )
		return false;

	DnActorHandle hTarget = pStruct->pSession->GetActorHandle();
	if( !hTarget )
		return false;

	DnActorHandle hActor = m_pSession->GetActorHandle();
	if( !hActor )
		return false;

	hActor->CmdWarp( *hTarget->GetPosition(), EtVec3toVec2(*hTarget->GetLookDir()) );

#endif // #if defined( _GAMESERVER )

	return true;
}

bool CDNGMCommand::CmdRecall( const VecWString& tokens )
{
#if defined( _VILLAGESERVER )
	if( tokens.size() != 2 )
	{
		m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wcslen(g_GMCmdList[GM_Recall].szComment)), L"", g_GMCmdList[GM_Recall].szComment );
		return false;
	}

	// GM������ �ִ��� �˻�
	CDNField* pField = m_pSession->GetField();
	if( !pField || !pField->bIsGMVillage() )
	{
		std::wstring wString = L"GM���������� ��밡���� ġƮ�Դϴ�.";
		m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wString.size()), L"", wString.c_str() );
		return false;
	}

	if( wcscmp( m_pSession->GetCharacterName(), tokens[1].c_str() ) == 0 )
	{
		std::wstring wString = L"�ڱ� �ڽ��� ��ȯ�� �� �����ϴ�..";
		m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wString.size()), L"", wString.c_str() );
		return false;
	}

	if( !g_pMasterConnection || !g_pMasterConnection->GetActive() )
		return false;

#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnection->SendReqRecall( m_pSession->GetAccountDBID(), pField->GetChnnelID(), tokens[1].c_str(), m_pSession->m_eSelectedLanguage);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnection->SendReqRecall( m_pSession->GetAccountDBID(), pField->GetChnnelID(), tokens[1].c_str() );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

#else
	//std::wstring wString = boost::io::str( boost::wformat( L"���������� ��밡���� ġƮ�Դϴ�.") );
#if defined(PRE_ADD_MULTILANGUAGE)
	std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4429, m_pSession->m_eSelectedLanguage) ) );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4429) ) );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wString.size()), L"", wString.c_str() );
	return false;
#endif // #if defined( _VILLAGESERVER )
	
	return true;
}

bool CDNGMCommand::CmdBanUser(const VecWString& tokens)
{
	if( tokens.size() != 2 )
	{
		m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wcslen(g_GMCmdList[GM_UserBan].szComment)), L"", g_GMCmdList[GM_UserBan].szComment );
		return false;
	}

	// GM������ �ִ��� �˻�
	if( wcscmp( m_pSession->GetCharacterName(), tokens[1].c_str() ) == 0 )
	{
		std::wstring wString = L"�ڱ� �ڽ��� ���� ������ �� �����ϴ�.";
		m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wString.size()), L"", wString.c_str() );
		return false;
	}

#if defined( _VILLAGESERVER )
	if( !g_pMasterConnection || !g_pMasterConnection->GetActive() )
		return false;

#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnection->SendBanUser(m_pSession->GetAccountDBID(), tokens[1].c_str(), m_pSession->m_eSelectedLanguage);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnection->SendBanUser(m_pSession->GetAccountDBID(), tokens[1].c_str());
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#elif defined (_GAMESERVER)
	if (g_pMasterConnectionManager == NULL)
		return false;

#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnectionManager->SendBanUser(m_pSession->GetAccountDBID(), m_pSession->GetWorldSetID(), tokens[1].c_str(), m_pSession->m_eSelectedLanguage);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnectionManager->SendBanUser(m_pSession->GetAccountDBID(), m_pSession->GetWorldSetID(), tokens[1].c_str());
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

#endif
	return true;
}

bool CDNGMCommand::CmdRepairAll(VecWString& tokens)
{
#if defined( _VILLAGESERVER )
	if( !m_pSession->GetItem() )
		return false;
	m_pSession->GetItem()->OnRecvRepairAll( true );
#else // #if defined( _VILLAGESERVER )
	wstring wszString = FormatW(L"������������밡���� ġƮŰ�Դϴ�.\r\n");
	m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )	
	return true;
}

bool CDNGMCommand::CmdGCreate(VecWString& tokens)
{
	if(2 > tokens.size()) {
		return false;
	}
	const std::wstring aGuildName = tokens[1].c_str();
	if(aGuildName.empty() || GUILDNAME_MAX <= aGuildName.size()) {
		return false;
	}

#if defined(_VILLAGESERVER)

	// 20100629 ���� �ۼ� �� ������ �����ڵ� ���ڴ뿪 Ȯ���ϵ��� ��� �߰�
	if(!g_CountryUnicodeSet.Check(aGuildName.c_str())) 
	{	
		wstring wszString = FormatW(L"���� ����� �� ���� ���ڰ� ���ԵǾ����ϴ�.\r\n");
		m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
		return false;
	}

	const TGuildUID GuildUID = m_pSession->GetGuildUID();
	if(GuildUID.IsSet()) {
		wstring wszString = FormatW(L"�̹� ��忡 ���Ե� �����Դϴ�.\r\n");
		m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
		return false;
	}

	int wGuildRoleAuth[GUILDROLE_TYPE_CNT] = { 0, };
	g_pGuildManager->SetAuthBase(wGuildRoleAuth);	// ��� ���� ����(����)

	m_pSession->GetDBConnection()->QueryCreateGuild(m_pSession->GetDBThreadID(), m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), aGuildName.c_str(), m_pSession->GetMapIndex(), m_pSession->GetLevel(), 0, g_Config.nWorldSetID, wGuildRoleAuth);

	wstring wszString = FormatW(L"��� â���� ���������� ��û�Ǿ����ϴ�.\r\n");
	m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#else // #if defined( _VILLAGESERVER )
	wstring wszString = FormatW(L"���������� ��� ������ ġƮŰ�Դϴ�.\r\n");
	m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
	return true;
}

bool CDNGMCommand::CmdGDismiss(VecWString& tokens)
{
	if(1 > tokens.size()) {
		return false;
	}

#if defined(_VILLAGESERVER)
	const TGuildUID GuildUID = m_pSession->GetGuildUID();
	if(!GuildUID.IsSet()) {
		wstring wszString = FormatW(L"��忡 ���Ե� ���°� �ƴմϴ�.\r\n");
		m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
		return false;
	}

	// �̹� ��ü�� ������� üũ
	if(g_pGuildManager->IsDismissExist(GuildUID)) {
		m_pSession->ResetGuildSelfView();
		m_pSession->SendDismissGuild(m_pSession->GetSessionID(), ERROR_GUILD_ALREADYDISMISS_GUILD, NULL);
		return false;
	}

	CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
	if(!pGuild) 
	{
		wstring wszString = FormatW(L"��� ������ �������� �ʽ��ϴ�.\r\n");
		m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
		return false;
	}

#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
	if(FALSE == pGuild->IsEnable()) return false;
#endif


	if(GUILDROLE_TYPE_MASTER != m_pSession->GetGuildSelfView().btGuildRole) 
	{
		wstring wszString = FormatW(L"���� ����常 �����մϴ�.\r\n");
		m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
		return false;
	}


	if(!pGuild->GetMemberInfo(m_pSession->GetCharacterDBID())) 
	{
		wstring wszString = FormatW(L"�ش� ����� ������ �ƴմϴ�.\r\n");
		m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
		return false;
	}

	if(pGuild->GetMemberCount() > 1) 
	{
		wstring wszString = FormatW(L"������ ���� �����մϴ�.\r\n");
		m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
		return false;
	}

	m_pSession->GetDBConnection()->QueryDismissGuild( m_pSession );

	wstring wszString = FormatW(L"��� �ػ��� ���������� ��û�Ǿ����ϴ�.\r\n");
	m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#else // #if defined( _VILLAGESERVER )
	wstring wszString = FormatW(L"���������� ��� ������ ġƮŰ�Դϴ�.\r\n");
	m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
	return true;
}

bool CDNGMCommand::CmdWeeklyEventClear()
{
#if defined(PRE_ADD_WEEKLYEVENT)
	g_pDataManager->WeeklyEventClear();
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)
	return true;
}

#if defined( PRE_WORLDCOMBINE_PVP )
bool CDNGMCommand::CmdWorldPvPRoomAllKillTeamSetting( PvPCommon::Team::eTeam eType, VecWString& tokens )
{
#if defined( _GAMESERVER )
	if(2 > tokens.size()) {
		return false;
	}

	if( m_pSession->GetGameRoom() )
	{
		CDNGameRoom *pRoom = m_pSession->GetGameRoom();
		if( pRoom && pRoom->GetWorldPvPRoomReqType() == WorldPvPMissionRoom::Common::GMRoom )
		{		
			for( int i=1;i<tokens.size();i++ )
			{
				CDNGameRoom::PartyStruct* pStruct =  pRoom->GetPartyData( const_cast<WCHAR*>(tokens[i].c_str()) );
				if( pStruct )
				{
					CDNUserSession * pSession = pStruct->pSession;
					if( pSession->GetActorHandle() && pSession->GetTeam() != PvPCommon::Team::Observer )
					{
						pSession->SetTeam( eType );
						pSession->GetActorHandle()->SetTeam(eType);
					}
				}
				else
				{
					wstring wszString = FormatW(L"%s ĳ������ ��ȿ���� �ʽ��ϴ�.\r\n", (WCHAR*)tokens[i].c_str() );
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );					
				}
			}			
		}
		else
			return false;
	}
	else
		return false;

	return true;
#endif
	return false;
}

bool CDNGMCommand::CmdWorldPvPRoomTournamentSetting(VecWString& tokens)
{
#if defined( _GAMESERVER )
	if(4 != tokens.size()) 
	{
		return false;
	}
	
	if( m_pSession->GetGameRoom() )
	{
		CDNGameRoom *pRoom = m_pSession->GetGameRoom();
		if( pRoom && pRoom->GetWorldPvPRoomReqType() == WorldPvPMissionRoom::Common::GMRoom )
		{		

			int nTournamentIndex = _wtoi(tokens[1].c_str());
			if( pRoom->GetPvPMaxUser() <= 0 )
			{
				wstring wszString = FormatW(L"���ӷ� �ƽ����� ����.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
				return false;
			}
			if( nTournamentIndex > pRoom->GetPvPMaxUser() / 2 )
			{
				wstring wszString = FormatW(L"����ȣ ����.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
				return false;
			}

			nTournamentIndex = ((nTournamentIndex - 1) * 2);
			int nCount = 0;
			for( int i=2;i<tokens.size();i++ )
			{
				CDNGameRoom::PartyStruct* pStruct =  pRoom->GetPartyData( const_cast<WCHAR*>(tokens[i].c_str()) );
				if( pStruct )
				{
					CDNUserSession * pSession = pStruct->pSession;
					if( pSession->GetActorHandle() && pSession->GetTeam() != PvPCommon::Team::Observer )
					{
						pRoom->GetPvPGameMode()->SetTournamentUserInfo( nTournamentIndex+nCount, pSession );
						nCount++;
					}
					else
					{
						wstring wszString = FormatW(L"%s ĳ���� ��ȿ���� �ʽ��ϴ�.\r\n", (WCHAR*)tokens[i].c_str() );
						m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
						return false;
					}
				}
			}			
		}
		else
			return false;
	}
	else
		return false;

	return true;
#endif
	return false;
}
#endif
#if defined( PRE_ADD_MUTE_USERCHATTING )
bool CDNGMCommand::CmdMuteUser(VecWString& tokens)
{
	if( 3 != tokens.size() )
		return false;
	
	if( _wtoi(tokens[2].c_str()) < 0)
		return false;

	//������� �� ã�Ƽ� �뺸
#if defined( _VILLAGESERVER )
	if( !g_pMasterConnection || !g_pMasterConnection->GetActive() )
		return false;
#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnection->SendMuteUserFind(m_pSession->GetAccountDBID(), m_pSession->GetWorldSetID(), tokens[1].c_str(), _wtoi(tokens[2].c_str()), m_pSession->m_eSelectedLanguage);
#else	// #if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnection->SendMuteUserFind(m_pSession->GetAccountDBID(), m_pSession->GetWorldSetID(), tokens[1].c_str(), _wtoi(tokens[2].c_str()));
#endif	// #if defined(PRE_ADD_MULTILANGUAGE)
#elif defined (_GAMESERVER)
	if (g_pMasterConnectionManager == NULL)
		return false;
#if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnectionManager->SendMuteUserFind(m_pSession->GetAccountDBID(), m_pSession->GetWorldSetID(), tokens[1].c_str(), _wtoi(tokens[2].c_str()), m_pSession->m_eSelectedLanguage);
#else	// #if defined(PRE_ADD_MULTILANGUAGE)
	g_pMasterConnectionManager->SendMuteUserFind(m_pSession->GetAccountDBID(), m_pSession->GetWorldSetID(), tokens[1].c_str(), _wtoi(tokens[2].c_str()));
#endif	// #if defined(PRE_ADD_MULTILANGUAGE)
#endif
	return true;
}
#endif	// #if defined( PRE_ADD_MUTE_USERCHATTING )