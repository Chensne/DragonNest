#pragma once


// ��ų ��� ���� ���θ� üũ�ϱ� ���� �������̽�
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
		TOGGLE_CHECKER,				// Ư�� ��� ��ų�� ��� �� ��������.
		ENERGY_CHARGE_CHECKER,		// Ŭ������ ������ ������ �� ��������. ������ ������ �Ǿ��� ������ �� �� �ִ� ��ų�� �ִ�.
		GROUNDMOVABLE_CHECKER,		// ���� �̵� �������� üũ
		RANGE_CHECKER,				// ����+��ų��Ÿ� �ȿ� ���� �ִ��� üũ. ���� �־�� ��ų ��� ����
		BUBBLE_COUNT_CHECKER,		// ���� ���� ���� �̻��� �Ǿ�� ��ų ��� ����.
		SUMMON_CHECKER,             // ��ȯü�� ������ ��ȯü�� �����Ÿ��ȿ� ������ ��ų ��밡��
		TARGET_STATEEFECT_CHECKER,	// ����� ����ȿ���� �ִ��� Ȯ��
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

	// Checker �� ����ϴ� ������ ����. ���̺��� �Ѿ���� �������� ����.
	virtual int GetNumArgument( void ) = 0;

	int GetType( void ) { return m_iType; };

	// factory
	static IDnSkillUsableChecker* Create( DnActorHandle hActor, int iCheckerIndex, int iSkillLevelTableID, /*IN OUT*/ int* pIParamOffset );
	static int MapOperator( char* pOperator );

	virtual IDnSkillUsableChecker* Clone() = 0;
};
