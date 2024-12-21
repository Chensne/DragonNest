#pragma once

#if defined(PRE_ADD_DWC)
#include "Task.h"
#include "MessageListener.h"
//#include "LoginClientSession.h"

class CDnDWCTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDnDWCTask>
{
public:
	CDnDWCTask();
	virtual ~CDnDWCTask();

	enum eMatchTime {
		eDWC_First_MatchTime = 0,
		eDWC_Second_MatchTime
	};

protected:
	enum {
		CALLBACK_MEMBER_INVITE_MSG = 30,
	};

	struct sInvitedDwcTeamMemberInfo
	{
		UINT nTeamID;
		bool bAccept;
		UINT nFromAcountID;

		sInvitedDwcTeamMemberInfo()
			: nTeamID(0)
			, bAccept(false)
			, nFromAcountID(0)
		{}
	};
	sInvitedDwcTeamMemberInfo	m_InvitedDwcTeamMemberInfo;
	
	std::vector<TDWCTeam>		m_vDwcTeamInfoList;
	std::vector<TDWCTeamMember> m_vDwcTeamMemberList;
	std::list<TDWCTeamMember>	m_listDwcInOutMemeberList;

	bool						m_bIsDWCChar;
	bool						m_bDwcLeader;
	bool						m_bIHaveDWCTeam;
	std::wstring				m_wszMyName;
	SCDWCTeamNameInfo			m_DWCTeamNameInfo;
	TDWCChannelInfo				m_DWCChannelInfo; // DWC 채널 정보

protected:
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	
	void OnRecvDWCMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvDWCNpcMessage( int nSubCmd, char *pData, int nSize );

	void OnRecvShowDwcTeamCreateDlg();									// 팀 생성 UI Open
	void OnRecvCreatedDwcTeam(SCCreateDWCTeam* pPacket);				// 팀생성
	void OnRecvDisMissDwcTeam(SCDismissDWCTeam* pPacket);				// 팀해체
	void OnRecvLeaveDwcMember(SCLeaveDWCTeamMember* pPacket);			// 팀탈퇴
	void OnRecvInviteDwcMember(SCInviteDWCTeamMemberReq* pPacket);		// 멤버 초대 요청	
	void OnRecvInviteDwcMemberReq(SCInviteDWCTeamMemberAck* pPacket);	// 멤버 초대 피드백
	void OnRecvDwcTeamInfo(SCGetDWCTeamInfo* pPacket);					// 팀 정보
	void OnRecvDwcTeamMemberInfo(SCGetDWCTeamMember* pPacket);			// 팀원 정보		
	void OnRecvDwcInviteDwcMemberAckResult(SCInviteDWCTeamMemberAckResult* pPacket);
	void OnRecvDwcRefreshMemberState(SCChangeDWCTeamMemberState* pPacket); // 멤버 Location 새로고침
	void OnRecvDWCTeamNameInfo(SCDWCTeamNameInfo* pPacket);				// 팀 이름 저장
	void OnRecvDWCRankList(SCGetDWCRankPage* pPacket);					// 랭킹 게시판 요청결과
	void OnRecvDWCFindRank(SCGetDWCFindRank* pPacket);					// 랭킹 게시판 검색결과
	void OnRecvDWCChannelInfo(SCDWCChannelInfo *pPacket);
	void OnRecvChangeDWCTeam(SCChangeDWCTeam* pPacket);

	void SendDwcMemberInviteRequest(bool bAccept);

public:
	const bool IsDWCRankSession();		// DWC 기간인지 체크한다.
	const bool IsDWCPvPModePlaying();	// DWC 게임모드로 플레이중인지 체크.
	
	const TDWCChannelInfo GetDWCChannelInfo() { return m_DWCChannelInfo; }
	void GetDWCSessionPeriod(std::wstring& wszStartTime, std::wstring& wszEndTime);							// 남은 일/월 가져오기
	void GetDWCRemainMatchTime(eMatchTime eType , std::wstring& wszStartTime , std::wstring& wszEndTime);	// 남은 시간 가져오기
	
	const bool IsMyName(wchar_t* pName);	
	const bool IsInPartyList(INT64 biCharacterDBID);	
	const bool IsDWCTeamLeader()	{ return m_bDwcLeader;}
	const bool IsDWCChar()			{ return m_bIsDWCChar;}		// 현재 선택한 캐릭이 DWC용인지 체크.
	const bool HasDWCTeam()			{ return m_bIHaveDWCTeam;}
	void	SetDWCChar(bool b)		{ m_bIsDWCChar = b; }
	WCHAR*	GetMyDWCTeamName();	
	WCHAR*	GetEnemyTeamName();
	const BYTE GetDWCCharCreateLvLimit() { return m_DWCChannelInfo.cLimitCharacterLevel; }
	
	const SCDWCTeamNameInfo				GetDWCTeamNameInfo()	{ return m_DWCTeamNameInfo;  }	// 팀   이름을 저장해둔다 (상대팀 포함)
	const std::vector<TDWCTeam>			GetDwcTeamInfoList()	{ return m_vDwcTeamInfoList; }	// 팀   정보를 저장해둔다
	const std::vector<TDWCTeamMember>	GetDwcTeamMemberList()	{ return m_vDwcTeamMemberList;} // 멤버 정보를 저장해둔다
	
	void ClearPushComeInPartyMember()			 { m_listDwcInOutMemeberList.clear(); }
	void PushComeInPartyMember(TDWCTeamMember D) { m_listDwcInOutMemeberList.push_back(D); }
	bool PopComeInPartyMember(TDWCTeamMember D);

	void MakeDwcCharacterData(); // CDnActor::s_hLocalActor가 생성된후 들어오는 데이터 저장.
};

#define GetDWCTask()	CDnDWCTask::GetInstance()

#endif // PRE_ADD_DWC