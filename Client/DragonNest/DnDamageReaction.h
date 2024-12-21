#pragma once

class CDnDamageReaction
{
public:
	CDnDamageReaction();
	virtual ~CDnDamageReaction();

	enum{
		ALARM_TIME = 5,
	};

protected:
	EtTextureHandle m_hHealthWarningTexture;
	EtTextureHandle m_hDamageDirectionTexture[ 2 ];
	int	m_nHealthWarningSound[ 2 ];		// 0 : normal, 1 :fast 
	EtSoundChannelHandle m_hHealthWarningChannel[ 2 ];

	float		m_fHealthWarningPercent;
	float		m_fHealthWarningTime;
	int			m_nPrevHP;	
	int			m_nWarningState;
	
	float		m_fWarningSoundTime;

	bool		m_bGuildWarAlarm;
	float		m_fGuildWarAlarmTime;		

	std::vector< boost::tuple< DnActorHandle, int, float >  > m_Hitters;

public:
	void Initialize();
	void Clear();

	void Process( float fDelta, int nHP, int nHPPercent );
	void Render( float fDelta, int nHP, float fHPPercentFloat, EtVector3 vPos );

	void SetHitterHandle( DnActorHandle hHitter, int nDistanceType );

	void GuildWarAlarm()	{ m_bGuildWarAlarm = true; m_fGuildWarAlarmTime = ALARM_TIME; }
};
