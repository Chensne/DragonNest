#pragma once

#include "EtActionBase.h"
#include "Timer.h"
#include "SignalHeader.h"
#include "EtActionSignal.h"
#include "DnActionSpecificInfo.h"

class CDnRenderBase;
class CDnActionBase : public CEtActionBase 
{
public:
	CDnActionBase();
	virtual ~CDnActionBase();

protected:
	CDnRenderBase *m_pRender;
	std::vector<int> m_nVecAniIndexList;

	std::string m_szAction;
	LOCAL_TIME m_LocalTime;
	LOCAL_TIME m_ActionTime;
	float m_fFrame;
	float m_fPrevFrame;
	int m_nActionIndex;
	int m_nPrevActionIndex;
	std::string m_szActionQueue;
	float m_fQueueBlendFrame;
	float m_fQueueStartFrame;

	std::string m_szCustomAction;
	LOCAL_TIME m_CustomActionTime;
	int m_nCustomActionIndex;
	float m_fCustomPrevFrame;
	bool m_bCustomProcessSignal;

	int m_nLoopCount;
	float m_fFPS;

	// �׼� �ε������� �߻�ü�� �� �� �ִ� ������ ���س��´�.
	CDnActionSpecificInfo::S_PROJECTILE_COUNT_INFO m_ProjectileCountInfoForInit;
	const CDnActionSpecificInfo::S_PROJECTILE_COUNT_INFO* m_pProjectileCountInfo;

	// �߱����� ��ų ü�� �÷��׸� ���ؼ� ��ų �����ϴ� ���� ���� ���� ��ų ü�� �÷��װ�
	// �ִ� STE_Input �ñ׳��� ��󳻼� �׼� �ε����� ��Ƴ��´�.
	CDnActionSpecificInfo::S_SKILLCHAIN_INPUT_INFO m_SkillChainInfoForInit;
	const CDnActionSpecificInfo::S_SKILLCHAIN_INPUT_INFO* m_pSetSkillChainAction;

	// �нú� ��ų ��Ŷ�� �̿��� ���� ���� ���� ����
	CDnActionSpecificInfo::S_INPUTHASPASSIVESKILL_INFO m_PassiveSkillInfoForInit;
	const CDnActionSpecificInfo::S_INPUTHASPASSIVESKILL_INFO* m_pPassiveSkillInfo;

	CDnActionSpecificInfo::S_BASIC_ATTACK_INFO m_BasicAttackInfoForInit;
	const CDnActionSpecificInfo::S_BASIC_ATTACK_INFO* m_pBasicAttackInfo;

protected:
	void ProcessSignal( ActionElementStruct *pStruct, float fFrame, float fPrevFrame );
	void ResetActionBase();

	void InsertBasicAttackInfo( int nElementIndex, CEtActionSignal *pSignal );
	void InsertSkillChainInfo( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal );
	void InsertBasicShootActionCoolTime( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal );
	void InsertStandChangeSEShootSkillCoolTime( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal );

	void InsertPassiveSkillInfo( int nElementIndex, CEtActionSignal *pSignal );
	void InsertProjectileCountInfo( int nElementIndex, CEtActionSignal *pSignal );
	void InsertSendActionProjectileCountInfo( int nElementIndex, CEtActionSignal *pSignal );

public:
	// �ѱ�: �̵� �Ұ� ����ȿ������ ���� �������� �׼��� ��ٷ� ��ҽ�Ű�� ���ؼ� protected ���� public���� ���ϴ�.
	virtual void SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop = false );

	bool Initialize( CDnRenderBase *pRender );
	virtual bool LoadAction( const char *szFullPathName );
	virtual void FreeAction();
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex ) {}
	static void CheckPreSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase );
	static void CheckPostSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase );
	virtual bool IsIgnoreSignal( int nSignalIndex );


	const char *GetCurrentAction();
	int GetCurrentActionIndex( void ) { return m_nActionIndex; };
	inline std::string& GetCurrentPlayAction() { return m_szAction; }

	float GetCurFrame() { return m_fFrame; }
	void CacheAniIndex();
	LOCAL_TIME GetLocalTime() { return m_LocalTime; }

	__inline bool IsSignalRange( SignalTypeEnum Type ) {
		if( m_nActionIndex == -1 ) return false;
		ActionElementStruct *pStruct = GetElement( m_nActionIndex );
		if (pStruct == NULL) return false;

		std::map<int, std::vector<CEtActionSignal *>>::iterator it = pStruct->MapSearch.find( Type );
		if( it == pStruct->MapSearch.end() ) return false;

		CEtActionSignal *pSignal;
		for( DWORD i=0; i<it->second.size(); i++ ) {
			pSignal = it->second[i];
			if( m_fFrame >= pSignal->GetStartFrame() && 
				m_fFrame < pSignal->GetEndFrame() ) 
				return true;
		}
		return false;
	}

	void ProcessAction( LOCAL_TIME LocalTime, float fDelta );

	virtual void SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f );

	void SetFPS( float fValue );
	float GetFPS();

	// ���� �׼ǰ� �ٸ� ������ ������ ���
	virtual void SetCustomAction( const char *szActionName, float fFrame );
	bool IsCustomAction();
	void ResetCustomAction();
	bool IsCustomProcessSignal() { return m_bCustomProcessSignal; }
	int GetCustomActionIndex() { return m_nCustomActionIndex; }

	// ����ü �и� �׼ǽ� ���� ��ü �׼��� �������� ���´�.
	float GetCustomActionCurrentFrame( void ) 
	{  
		if( IsCustomAction() )
		{
			float fFrame = ((m_LocalTime - m_CustomActionTime) / 1000.f) * m_fFPS;
			return fFrame;
		}

		return 0.0f;
	};

	virtual void OnNextAction( const char* szPrevAction, const char *szNextAction ) {}
	virtual void OnFinishAction(const char* szPrevAction, LOCAL_TIME time) {}
	virtual void OnChangeAction( const char *szPrevAction ) {}
	virtual void OnChangeActionQueue( const char *szPrevAction ) {}
	virtual void OnChangeCustomAction() {}
	virtual void OnLoopAction( float fFrame, float fPrevFrame ) {}

	int GetMaxProjectileCountInAction( int nActionIndex ) const
	{ 
		int iResult = 0;

		if( m_pProjectileCountInfo )
		{
			map<int, int>::const_iterator iterProj = m_pProjectileCountInfo->mapMaxProjectileCountInAction.find( nActionIndex );
			if( m_pProjectileCountInfo->mapMaxProjectileCountInAction.end() != iterProj )
		 		iResult = iterProj->second;
		}

		return iResult;
	};

	void AddUsingProjectileWeaponTableIDs( int nActionIndex, /*IN OUT*/ multiset<int>& setWeaponTableIDs ) const
	{
		if( m_pProjectileCountInfo )
		{
			map<int, multiset<int> >::const_iterator iter = m_pProjectileCountInfo->mapUsingProjectileWeaponTableIDs.find( nActionIndex );
			if( m_pProjectileCountInfo->mapUsingProjectileWeaponTableIDs.end() != iter )
			{
				setWeaponTableIDs.insert( iter->second.begin(), iter->second.end() );
			}
		}
	}

	// ���� Ŭ�����϶��� ȣ���. ���⿡ �߻�ü �ñ׳��� �ְ� �÷��̾ ������ �׼��� �����ϴ� �ñ׳��� 
	// ������ ���� �� SendWeaponAction �ñ׳��� ���� ������ �������� ���� �Ѱ��ش�.
	// ���� ����� ���� �Ǵ� �������� �÷��̾� ������ �������� �ȴ�.
	void AddProjectileSignalOffset( int nActionIndex, int iSendWeaponActionOffset, /*IN OUT*/ deque<int>& dqProjectileSignalOffset ) const
	{
		if( m_pProjectileCountInfo )
		{
			map<int, deque<int> >::const_iterator iterProjOffset = m_pProjectileCountInfo->mapProjectileSignalFrameOffset.find( nActionIndex );
			if( m_pProjectileCountInfo->mapProjectileSignalFrameOffset.end() != iterProjOffset )
			{
				const deque<int>& vlMyProjectileSignalOffset = iterProjOffset->second;
				for( int i = 0; i < (int)vlMyProjectileSignalOffset.size(); ++i )
				{
					dqProjectileSignalOffset.push_back( iSendWeaponActionOffset + vlMyProjectileSignalOffset.at(i) );
				}
			}
		}
	}

	bool IsValidSkillChain( int iElementIndex, int iElementIndexToChange )
	{
		bool bResult = false;
		if( m_pSetSkillChainAction )
		{
			if( m_pSetSkillChainAction->setSkillChainAction.end() != 
				m_pSetSkillChainAction->setSkillChainAction.find( iElementIndex ) )
			{
				map<int, vector<string> >::const_iterator iter = m_pSetSkillChainAction->mapCanChainToThisAction.find( iElementIndex );
				if( m_pSetSkillChainAction->mapCanChainToThisAction.end() != iter )
				{
					const vector<string>& vlCanChangeActionNames = iter->second;
					for( int i = 0; i < (int)vlCanChangeActionNames.size(); ++i )
					{
						int iCanChangeActionIndex = GetElementIndex( vlCanChangeActionNames.at(i).c_str() );
						if( iCanChangeActionIndex == iElementIndexToChange )
						{
							bResult = true;
							break;
						}
					}
				}
			}
			
		}

		return bResult;
	}
};