#pragma once
#include "IDnSkillProcessor.h"



// 구간 별로 애니메이션을 재생시키는 스킬.. 예를 들어 "휠윈드" 같은 거.
class CDnPartialPlayProcessor : public IDnSkillProcessor, public TBoostMemoryPool< CDnPartialPlayProcessor >
{
protected:
	enum
	{
		START_ACTION,
		LOOP_ACTION,
		END_ACTION,
	};

	string					m_strStartActionName;
	string					m_strLoopActionName;
	string					m_strEndActionName;

	float					m_fMoveSpeed;
	int						m_iTime;
	int						m_iNowActionState;
	//bool					m_bStartedLastAction;
	float					m_fStartActionLength;
	float					m_fLoopActionLength;
	float					m_fElapsedTime;
	float					m_fLoopActionElapsedTime;

	// 종료 액션 까지 전부 재생 끝.
	bool					m_bPlayEnd;

#if defined( PRE_FIX_71333 )
	bool					m_bLoopActionEnd;	// 루프 액션인 경우 엔드액션이 큐에 정상적으로 들어갔는지 체크
#endif

protected:
	void _UpdatePlayTime( void );

public:
	CDnPartialPlayProcessor( void );
	CDnPartialPlayProcessor( DnActorHandle hActor, const char* pStartActionName, 
							 const char* pLoopActionName, const char* pEndActionName, int iTime, float fMoveSpeed );
	virtual ~CDnPartialPlayProcessor(void);

	// from IDnSkillProcessor
	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	int GetNumArgument( void ) { return 5; };
	void CopyFrom( IDnSkillProcessor* pProcessor );

	const char* GetStartActionName( void ) { return m_strStartActionName.c_str(); }
	const char* GetLoopActionName( void ) { return m_strLoopActionName.c_str(); };
	const char* GetEndActionName( void ) { return m_strEndActionName.c_str(); };


#if defined(PRE_FIX_68898)
protected:
	bool m_isSkipEndAction;
public:
	void SetSkipEndAction(bool isSkipEndAction) { m_isSkipEndAction = isSkipEndAction; }
#endif // PRE_FIX_68898

};
