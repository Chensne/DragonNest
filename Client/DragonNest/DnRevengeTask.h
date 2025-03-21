#include "Task.h"
#include "MessageListener.h"

#if defined( PRE_ADD_REVENGE )
class CDnRevengeTask : public CTask, public CTaskListener, public CSingleton<CDnRevengeTask>
{
public:
	CDnRevengeTask();
	virtual ~CDnRevengeTask();

public :
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	void _OnRecvPVPSkillMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvSetRevengeTarget( SCPvPSetRevengeTarget * pData );
	void OnRecvSuccessRevenge( SCPvPSuccessRevenge * pData );

protected:
	UINT m_uiMyRevengeID;

	std::map<UINT, UINT> m_mapRevenge_User;		// 복수해야 하는 유저
	std::map<UINT, UINT> m_mapRevenge_Target;	// 복수 당할 유저

public :
	bool Initialize();
	void ClearData();

	void ClearMyRevengeUser();
	UINT GetMyRevengeUser();
	void SetMyRevengeUser( UINT uiRevengeUserID );

	void SetRevengeUser( const SCPvPSetRevengeTarget & sData );
	void GetRevengeUserID( const UINT uiSessionID, UINT & eRevenge );

	UINT GetRevengeTargetType( const UINT uiRevengeUserID );
	void RevengeMessage( const UINT uiSessionID, const UINT uiRevengeTargetSessionID, const Revenge::TargetReason::eCode eReason );
};
#endif	// #if defined( PRE_ADD_REVENGE )