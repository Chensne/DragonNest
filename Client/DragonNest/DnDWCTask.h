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
	TDWCChannelInfo				m_DWCChannelInfo; // DWC ä�� ����

protected:
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	
	void OnRecvDWCMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvDWCNpcMessage( int nSubCmd, char *pData, int nSize );

	void OnRecvShowDwcTeamCreateDlg();									// �� ���� UI Open
	void OnRecvCreatedDwcTeam(SCCreateDWCTeam* pPacket);				// ������
	void OnRecvDisMissDwcTeam(SCDismissDWCTeam* pPacket);				// ����ü
	void OnRecvLeaveDwcMember(SCLeaveDWCTeamMember* pPacket);			// ��Ż��
	void OnRecvInviteDwcMember(SCInviteDWCTeamMemberReq* pPacket);		// ��� �ʴ� ��û	
	void OnRecvInviteDwcMemberReq(SCInviteDWCTeamMemberAck* pPacket);	// ��� �ʴ� �ǵ��
	void OnRecvDwcTeamInfo(SCGetDWCTeamInfo* pPacket);					// �� ����
	void OnRecvDwcTeamMemberInfo(SCGetDWCTeamMember* pPacket);			// ���� ����		
	void OnRecvDwcInviteDwcMemberAckResult(SCInviteDWCTeamMemberAckResult* pPacket);
	void OnRecvDwcRefreshMemberState(SCChangeDWCTeamMemberState* pPacket); // ��� Location ���ΰ�ħ
	void OnRecvDWCTeamNameInfo(SCDWCTeamNameInfo* pPacket);				// �� �̸� ����
	void OnRecvDWCRankList(SCGetDWCRankPage* pPacket);					// ��ŷ �Խ��� ��û���
	void OnRecvDWCFindRank(SCGetDWCFindRank* pPacket);					// ��ŷ �Խ��� �˻����
	void OnRecvDWCChannelInfo(SCDWCChannelInfo *pPacket);
	void OnRecvChangeDWCTeam(SCChangeDWCTeam* pPacket);

	void SendDwcMemberInviteRequest(bool bAccept);

public:
	const bool IsDWCRankSession();		// DWC �Ⱓ���� üũ�Ѵ�.
	const bool IsDWCPvPModePlaying();	// DWC ���Ӹ��� �÷��������� üũ.
	
	const TDWCChannelInfo GetDWCChannelInfo() { return m_DWCChannelInfo; }
	void GetDWCSessionPeriod(std::wstring& wszStartTime, std::wstring& wszEndTime);							// ���� ��/�� ��������
	void GetDWCRemainMatchTime(eMatchTime eType , std::wstring& wszStartTime , std::wstring& wszEndTime);	// ���� �ð� ��������
	
	const bool IsMyName(wchar_t* pName);	
	const bool IsInPartyList(INT64 biCharacterDBID);	
	const bool IsDWCTeamLeader()	{ return m_bDwcLeader;}
	const bool IsDWCChar()			{ return m_bIsDWCChar;}		// ���� ������ ĳ���� DWC������ üũ.
	const bool HasDWCTeam()			{ return m_bIHaveDWCTeam;}
	void	SetDWCChar(bool b)		{ m_bIsDWCChar = b; }
	WCHAR*	GetMyDWCTeamName();	
	WCHAR*	GetEnemyTeamName();
	const BYTE GetDWCCharCreateLvLimit() { return m_DWCChannelInfo.cLimitCharacterLevel; }
	
	const SCDWCTeamNameInfo				GetDWCTeamNameInfo()	{ return m_DWCTeamNameInfo;  }	// ��   �̸��� �����صд� (����� ����)
	const std::vector<TDWCTeam>			GetDwcTeamInfoList()	{ return m_vDwcTeamInfoList; }	// ��   ������ �����صд�
	const std::vector<TDWCTeamMember>	GetDwcTeamMemberList()	{ return m_vDwcTeamMemberList;} // ��� ������ �����صд�
	
	void ClearPushComeInPartyMember()			 { m_listDwcInOutMemeberList.clear(); }
	void PushComeInPartyMember(TDWCTeamMember D) { m_listDwcInOutMemeberList.push_back(D); }
	bool PopComeInPartyMember(TDWCTeamMember D);

	void MakeDwcCharacterData(); // CDnActor::s_hLocalActor�� �������� ������ ������ ����.
};

#define GetDWCTask()	CDnDWCTask::GetInstance()

#endif // PRE_ADD_DWC