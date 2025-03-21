#pragma once

#if defined(PRE_ADD_DWC)

class CDnDWCTeam;
typedef std::map<UINT, CDnDWCTeam*> TMapDWCTeams;
typedef std::map<INT64, UINT> TMapDWCCharacter;

class CDNDWCSystem
{
public:
	CDNDWCSystem();
	~CDNDWCSystem();
	
	CDnDWCTeam* GetDWCTeam(UINT nDWCTeamID);
	CDnDWCTeam* AddDWCTeamResource(UINT nDWCTeamID, TDWCTeam *pTeamInfo);	
	CDnDWCTeam* UpdateDWCTeamResource(TAGetDWCTeamInfo *pPacket);
	CDnDWCTeam* UpdateDWCTeamResource(UINT nTeamID, TDWCTeam *Info);
	CDnDWCTeam* UpdateTeamMember(TAGetDWCTeamMember *pPacket);
	CDnDWCTeam* UpdateTeamMember(int nTeamID, int nCount, TDWCTeamMember *MemberList );
	const WCHAR *GetDWCTeamName(UINT nDWCTeamID);
	bool DelDWCTeamResource(UINT nDWCTeamID);

	UINT IsRegisterdCharacter(INT64 biCharacterDBID);
	bool RegistDWCCharacter(INT64 biCharacterDBID, UINT nTeamID);
	bool RemoveDWCCharacter(INT64 biCharacterDBID);
	bool ChangeDWCTeamMemberState(INT64 biCharacterDBID, bool bDel, int nLocation, int nChannelID, int nMapIdx);
	
	bool CheckStatus();
	bool CheckDWCSeason();
	bool CheckDWCMatchTime(LadderSystem::MatchType::eCode MatchType);
	void Initialize(TAGetDWCChannelInfo* pPacket);
	bool GetInitFlag() { return m_bInit; }
	void GetDWCChannelInfo(TDWCChannelInfo & DWCChannelInfo) { DWCChannelInfo = m_DWCChannelInfo; }
	bool bIsIncludeDWCVillage() { return m_bIsIncludeDWCVillage; }
	void SetIncludeDWCVillage(bool bFlag) { m_bIsIncludeDWCVillage = bFlag; }

	void SetDWCMatchTimeForCheat(bool bSet);
private:

	struct MatchDuration
	{
		BYTE cStartHour;
		BYTE cStartMin;
		BYTE cEndHour;
		BYTE cEndMin;
	};

	TMapDWCTeams m_MapDWCTeamList;
	TMapDWCCharacter m_MapDWCCharacter;

	bool m_bInit;
	bool m_bIsIncludeDWCVillage;
	bool m_bCloseMatchByCheat;

	TDWCChannelInfo m_DWCChannelInfo;
	std::vector<MatchDuration> m_VecLadderTime;
};

extern CDNDWCSystem * g_pDWCTeamManager;

#endif