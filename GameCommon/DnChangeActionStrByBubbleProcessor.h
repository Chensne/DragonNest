#pragma once
#include "IDnSkillProcessor.h"


// 버블 갯수에 따라 스킬 테이블에서 설정된 스킬의 액션을 바꾸고 설정된 버블의 갯수만큼 버블을 소모 시킨다.
// 플레이어의 스킬은 클라이언트에서 스킬을 사용하고 서버로 패킷을 보내는 구조상 어쩔 수 없이 클라에서도
// 체크해서 액션 바꾸고 버블 제거하고 서버도 패킷 받아서 똑같이 처리한다. (클라/서버 각각 처리)
// 스킬이 끝나면 다시 복구 시켜준다.
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

	CDnChangeActionStrByBubbleProcessor() { m_iType = CHANGE_ACTIONSTR_BY_BUBBLE; }; // 임시 스킬 백업용으로 만들어놓은 생성자. 반드시 필요한 변수들 초기화.
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
