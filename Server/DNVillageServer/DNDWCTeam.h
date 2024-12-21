#pragma once
#if defined(PRE_ADD_DWC)
class CDNDWCSystem;

class CDnDWCTeam : public TBoostMemoryPool<CDnDWCTeam>
{
public:
	CDnDWCTeam(UINT nDWCTeamID, TDWCTeam *pInfo, CDNDWCSystem* pManager);
	~CDnDWCTeam(){}

	void UpdateTeamInfo(TDWCTeam *pInfo);
	bool AddMember(TDWCTeamMember *pMember, TCommunityLocation* pLocation = NULL);
	bool DelMember(INT64 biCharacterDBID);
	void UpdateMemberList(int nCount, TDWCTeamMember *MemberList);	
	bool ChangeDWCTeamMemberState(INT64 biCharacterDBID, TCommunityLocation* pLocation, bool bSend = true, bool bLogin = false);
	bool CheckRemoveResource();
	void RemoveDWCCharacters();
	void UpdateDWCScore(MADWCUpdateScore *pPacket);
	const WCHAR *GetDWCTeamName() { return m_Info.wszTeamName; }

	bool CheckDuplicationJob(BYTE cJobCode);
	bool IsMemberAddable() { return m_VectTotalMember.size() < DWC::DWC_MAX_MEMBERISZE; }
	UINT GetTeamID() { return m_nTeamID; }
	bool GetRecvMemberList(){ return m_bRecvMemberList; }
	const TDWCTeam* GetTeamInfo() {return &m_Info; }
	void GetMemberList(std::vector<TDWCTeamMember> &vMemberList);
	void GetMemberCharacterDBIDList(std::vector<INT64> &vMemberCharacterDBIDList);
	INT64 GetLeaderCharacterDBID() { return m_biLeaderCharacterDBID; }
	bool CheckCanLeaveTeam();

	void ResetTeamScore();

	void SendAddDWCTeamMember(UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszCharacterName, TP_JOB nJob, TCommunityLocation* pLocation);
	void SendLeaveDWCTeamMember(UINT nTeamID, INT64 biLeaveUserCharacterDBID, LPCWSTR lpwszCharacterName, int nRet);
	void SendDismissDWCTeam(UINT nTeamID, int nRet);
	void SendChangeDWCTeamMemberState(TDWCTeamMember *pDWCTeamMember, bool bLogin);

private:
	UINT				m_nTeamID;
	TDWCTeam			m_Info;
	BYTE				m_cDBThreadID;	//처리 안되어 있음
	std::vector<TDWCTeamMember> m_VectTotalMember;
	CDNDWCSystem*		m_pManager;
	INT64				m_biLeaderCharacterDBID;
	bool				m_bRecvMemberList;
};

#endif