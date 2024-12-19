#pragma once

#include "Util.h"

class CDNUserSession;

struct __GMCmdLineDefineStruct
{
	int nIndex;
	TCHAR* szString;
	int	nAccountLevel;
	TCHAR* szComment;
	TCHAR* szCommentEn;
};

typedef vector<wstring> VecWString;

class CDNUserSendManager;
class CDNGMCommand: public TBoostMemoryPool<CDNGMCommand>
{
private:
	CDNUserSession *m_pSession;

	VecWString m_VecWhisperName;	// 庇加富 包访 蜡历甸 包府

	void AddWhisperName(wstring wstrName);
	void DelWhisperName(wstring wstrName);

	bool OnGMCommand(int nCmdID, VecWString& tokens);

	bool CmdNotice(VecWString& tokens);
	bool CmdWhisper(VecWString& tokens, bool bWhisper);
	bool CmdHide(VecWString& tokens, bool bHide);
	bool CmdSummonMob(VecWString& tokens);
	bool CmdAllkillMob(VecWString& tokens);
	bool CmdMakeCoin(VecWString& tokens);
	bool CmdSetLevel(VecWString& tokens);
	bool CmdChangeMap(VecWString& tokens);
	bool CmdSetQuest(VecWString& tokens);
	bool CmdZoneNotice(VecWString& tokens);
	bool CmdFatigue(VecWString& tokens);
	bool CmdHp(VecWString& tokens);
	bool CmdDestroyPvP(VecWString& tokens);
	bool CmdRepairAll(VecWString& tokens);
	bool CmdGCreate(VecWString& tokens);
	bool CmdGDismiss(VecWString& tokens);
	bool CmdWeeklyEventClear();
#if defined( PRE_WORLDCOMBINE_PVP )
	bool CmdWorldPvPRoomAllKillTeamSetting(PvPCommon::Team::eTeam eType,  VecWString& tokens);
	bool CmdWorldPvPRoomTournamentSetting(VecWString& tokens);
#endif

public:
	bool CmdMakeItem(VecWString& tokens);
	bool Cmdhuanhua(VecWString& tokens);
	bool CmdTrace( int nType, const VecWString& tokens );
	bool CmdWarp( const VecWString& tokens );
	bool CmdRecall( const VecWString& tokens );
	bool CmdBanUser(const VecWString& tokens);
#if defined( PRE_ADD_MUTE_USERCHATTING )
	bool CmdMuteUser(VecWString& tokens);
#endif	// #if defined( PRE_ADD_MUTE_USERCHATTING )
public:
	CDNGMCommand(CDNUserSession *pSession);
	~CDNGMCommand();

	bool IsWhisperName(wstring wstrName);

	bool GMCommand(wstring& wszCommand);
};
