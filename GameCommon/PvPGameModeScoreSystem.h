
#pragma once

class IPVPGameModeScoreSystem
{
public:

	IPVPGameModeScoreSystem()
	{
		for( UINT i=0 ; i<PvPCommon::TeamIndex::Max ; ++i )
			m_uiScore[i] = 0;
	}

	virtual ~IPVPGameModeScoreSystem(){}

	void SetGameModeScore( const UINT uiATeamScore, const UINT uiBTeamScore )
	{
		m_uiScore[PvPCommon::TeamIndex::A] = uiATeamScore;
		m_uiScore[PvPCommon::TeamIndex::B] = uiBTeamScore;
	}

	void GetGameModeScore( UINT& uiATeamScore, UINT& uiBTeamScore )
	{
		uiATeamScore	= m_uiScore[PvPCommon::TeamIndex::A];
		uiBTeamScore	= m_uiScore[PvPCommon::TeamIndex::B];
	}

	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter ){}

#if defined( _GAMESERVER )
	virtual void OnFinishRound( CDNGameRoom* pGameRoom, UINT uiWinTeam ){}
#endif // #if defined( _GAMESERVER )

protected:

	UINT	m_uiScore[PvPCommon::TeamIndex::Max];
};
