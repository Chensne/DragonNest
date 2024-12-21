#pragma once
#include "IDnSkillProcessor.h"


// ���� ������ ���� ��ų ���̺��� ������ ��ų�� �׼��� �ٲٰ� ������ ������ ������ŭ ������ �Ҹ� ��Ų��.
// �÷��̾��� ��ų�� Ŭ���̾�Ʈ���� ��ų�� ����ϰ� ������ ��Ŷ�� ������ ������ ��¿ �� ���� Ŭ�󿡼���
// üũ�ؼ� �׼� �ٲٰ� ���� �����ϰ� ������ ��Ŷ �޾Ƽ� �Ȱ��� ó���Ѵ�. (Ŭ��/���� ���� ó��)
// ��ų�� ������ �ٽ� ���� �����ش�.
class CDnChangeActionStrByBubbleProcessor : public IDnSkillProcessor, public TBoostMemoryPool< CDnChangeActionStrByBubbleProcessor >
{
protected:

	struct S_ACTION_BY_BUBBLE
	{
		int iBubbleID;
		int iBubbleCount;
		string strActionName;

		S_ACTION_BY_BUBBLE( void ) : iBubbleID( 0 ), iBubbleCount( 0 ) {};
	};
	vector<S_ACTION_BY_BUBBLE> m_vlActionByBubble;

public:

	CDnChangeActionStrByBubbleProcessor() { m_iType = CHANGE_ACTIONSTR_BY_BUBBLE; }; // �ӽ� ��ų ��������� �������� ������. �ݵ�� �ʿ��� ������ �ʱ�ȭ.
	CDnChangeActionStrByBubbleProcessor( DnActorHandle hActor, const char* pArg );
	virtual ~CDnChangeActionStrByBubbleProcessor( void );

	// from IDnSkillProcessor.h
	virtual void SetHasActor( DnActorHandle hActor );

	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	bool IsFinished( void );
	int GetNumArgument( void ) { return 1; };

	const char *GetChangeActionNameAndRemoveNeedBubble( bool* pBChanged = NULL );

	virtual void CopyFrom( IDnSkillProcessor* pProcessor );
	vector<S_ACTION_BY_BUBBLE> *GetBubbleActionList(){ return &m_vlActionByBubble; }

};
