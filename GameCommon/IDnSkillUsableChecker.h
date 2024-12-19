#pragma once


// 스킬 사용 가능 여부를 체크하기 위한 인터페이스
class IDnSkillUsableChecker
{
public:
	enum
	{
		OP_GREATER,
		OP_EQUAL,
		OP_NOT_EQUAL,
		OP_LESS,
		OP_GREATER_EQUAL,
		OP_LESS_EQUAL,
		
		OP_AND,
		OP_NOT_AND,			// !( A & B )
		OP_COUNT,
	};

	enum
	{
		STATE_CHECKER,
		HP_CHECKER,
		PROB_CHECKER,				
		TARGET_STATE_CHECKER,
		MOVABLE_CHECKER,
		JUMPABLE_CHECKER,
		HIT_CHECKER,
		TOGGLE_CHECKER,				// 특정 토글 스킬이 토글 된 상태인지.
		ENERGY_CHARGE_CHECKER,		// 클러릭의 에너지 차지가 된 상태인지. 에너지 차지가 되었을 때에만 쓸 수 있는 스킬이 있다.
		GROUNDMOVABLE_CHECKER,		// 지상 이동 가능한지 체크
		RANGE_CHECKER,				// 무기+스킬사거리 안에 적이 있는지 체크. 적이 있어야 스킬 사용 가능
		BUBBLE_COUNT_CHECKER,		// 일정 버블 갯수 이상이 되어야 스킬 사용 가능.
		SUMMON_CHECKER,             // 소환체가 있으며 소환체가 일정거리안에 있을때 스킬 사용가능
		TARGET_STATEEFECT_CHECKER,	// 대상의 상태효과가 있는지 확인
		TYPE_COUNT,
	};

protected:
	int						m_iType;
	DnActorHandle			m_hHasActor;

	IDnSkillUsableChecker( void );
	IDnSkillUsableChecker( DnActorHandle hActor ) : m_hHasActor(hActor), m_iType( TYPE_COUNT ) {};

public:
	virtual ~IDnSkillUsableChecker(void);

	void SetHasActor( DnActorHandle hActor ) { m_hHasActor = hActor; };

	virtual bool CanUse( void ) = 0;

	// Checker 가 사용하는 인자의 갯수. 테이블에서 넘어오는 데이터의 검증.
	virtual int GetNumArgument( void ) = 0;

	int GetType( void ) { return m_iType; };

	// factory
	static IDnSkillUsableChecker* Create( DnActorHandle hActor, int iCheckerIndex, int iSkillLevelTableID, /*IN OUT*/ int* pIParamOffset );
	static int MapOperator( char* pOperator );

	virtual IDnSkillUsableChecker* Clone() = 0;
};
