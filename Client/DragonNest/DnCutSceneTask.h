#pragma once
#include "Task.h"
//#include "IDnCutSceneDataReader.h"
#include "DnCutScenePlayer.h"
#include "InputReceiver.h"
#include "DnWorldActProp.h"

class IDnCutSceneDataReader;
class ICustomActorProcessor;


#include "DnLoadingTask.h"

class CDnFadeInOutDlg;
// 실시간 동영상 관련 Task - 한기가 작성
class CDnCutSceneTask : public CTask, public CInputReceiver, public CDnLoadingStateMachine
{
protected:
	struct S_ACT_PROP_DEFAULT
	{
		DnPropHandle hProp;
		CDnWorldActProp* pActProp;
		string strDefaultAction;
		bool bShow;

		S_ACT_PROP_DEFAULT( void ) : pActProp( NULL ), bShow( true ) {}
	};


	IDnCutSceneDataReader*	m_pCutSceneData;
	
	ICustomActorProcessor*	m_pActorProcessor;
	CDnCutScenePlayer		m_CutScenePlayer;
	
#if defined(TEST_CUTSCENE)
	bool					m_bFirstLoop;
#endif

	deque<DnActorHandle>	m_dqhLiveActors;
	vector<MatrixEx>	m_vlOriMatExWorlds;
	vector<std::string>		m_szOriAction;
	vector<bool>			m_bVecPlayerBattleMode;
	vector<DnActorHandle>	m_vlhAdditionalLoadedActors;
	deque<DnActorHandle>	m_dqhExceptActors;
	vector<DnActorHandle>	m_vlhDeadMonster;		// 죽은 몹이 컷신에서 쓰였을 경우 hp 를 임시로 채워주는데 컷신 끝난 후 hide 시키기 위해 따로 갖고 있는다.
	
	// 플레이어 액터들이 원래 전투모드였는지 받아뒀다가 컷신 끝나고 무기 다시 꺼내주던가 말던가 하자.
	// 벡터 리스트는 플레이어 액터 여부없이 m_dqhLiveActor 와 같은 인덱스로 셋팅된다.
	// 1인 경우가 전투모드. 아닌 경우는 0이다.
	vector<int>				m_vlIsPlayerBattleMode;		
												

	CDnWorld*				m_pWorld;
	vector<S_ACT_PROP_DEFAULT> m_vlActPropDefault;

	SCameraInfo				m_OriginalCameraInfo;

	CEtDOFFilter*			m_pDOFFilter;
	float					m_fNearStart;
	float					m_fNearEnd;
	float					m_fFarStart;
	float					m_fFarEnd ;
	float					m_fFocusDist;
	float					m_fNearBlurSize;
	float					m_fFarBlurSize;

	bool m_bAutoFadeIn;
	bool m_bRequestSkip;
	CDnFadeInOutDlg *m_pSkipFade;
	bool m_bSetFadeModeCalledForSkip;			// 마을에서 퀘스트 스크립트로 컷신 재생되는 경우 ESC 입력 한 번 받아서 페이드 되면 다시 입력받지 않도록. (#11892)
	bool m_bLoadingComplete;
	bool m_bCheatCutScene;						// 지금 컷신이 치트로 보이는건가?

	DnActorHandle m_hActor;

	// 한벌옷 장착할때 예외처리때문에 입고 있던 파츠에 대한 정보를 따로 가지고 있는다.
	typedef struct _SPartsInfo {
		int nType;
		int nItemID;
	} SPartsInfo;
	std::vector<SPartsInfo> m_vecPartsInfo;

	vector< std::string > m_arrCharacterName; // #60295 칼리 모습이 보이지 않음.

public:
	CDnCutSceneTask(void);
	virtual ~CDnCutSceneTask(void);

	bool Initialize( const char* pFileName, int nQuestIndex = -1, int nQuestStep = -1, bool bAutoFadeIn = true, DWORD dwQuestPlayerUniqueID = UINT_MAX );

	void PrepareNormalCutScene( DNTableFileFormat*  pActorTable );
	void PrepareQuestCutScene( DNTableFileFormat*  pActorTable, int nQuestIndex, int nQuestStep, DWORD dwQuestPlayerUniqueID );
	void StartPlay( void );
	void Finalize( void );			// 컷신 재생 중에 클라이언트를 종료할 때는 이 함수가 먼저 외부에서 호출됨.

	// from CTask
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	
	// from CInputReceiver
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime = -1 );

	bool IsPlayEnd( void ) { return m_CutScenePlayer.IsEndScene(); };

	void FadeOutSkip();
	bool RenderScreen( LOCAL_TIME LocalTime, float fDelta );

	void FromCheat( void ) { m_bCheatCutScene = true; };

	// #60295 - 인챈트 레벨. 무기의 경우 강화이펙트가 붙기때문에 강화레벨 인자로 줄 수 있다. 방어구의 경우 그냥 무시.
	bool AttachItem( int nItemID, int nEnchantLevel = 0, int nSubArg1 = 0, int nLookItemID = ITEMCLSID_NONE );

private:
	void AddClonePlayer( DnActorHandle & hFindedActor ); // #60295 - Player 액터를 2개이상 생성한 경우 플레이어액터를 복제한다.
};
