#pragma once
#include "DnCustomDlg.h"
#include "DnRenderAvatar.h"
#include "DnParts.h"
#include "MAPartsBody.h"


#ifdef PRE_ADD_GACHA_JAPAN
class CDnGachaPreview : public CDnCustomDlg
						
{
private:
	// 테이블로부터 읽어들인 캐릭터 셋트 파츠.
	// 오로지 코스튬 외형 미리 보기를 위한 것이며 아이템의 능력치나
	// 기타 다른 사항은 아무것도 사용하지 않는다. 그냥 외형 선택해서 보여주기용.
	// (일본 가챠는 전부 캐시템만 나온다.)
	struct S_COSTUME_INFO
	{
		int aiCashParts[ CDnParts::CashPartsTypeEnum_Amount ];
		tstring strSetName;
		int iGradeNameStringID;

		S_COSTUME_INFO( void ) : iGradeNameStringID( 0 )
		{
			SecureZeroMemory( aiCashParts, sizeof(aiCashParts) );
		};
	};

	// 직업별로 코스튬 셋트를 모아둠.
	struct S_COSTUME_SET_INFO
	{
		int iClassID;
		int iNowSelectedCostumeSet;
		vector<S_COSTUME_INFO> vlCostumes;

		S_COSTUME_SET_INFO( void ) : iClassID( 0 ), iNowSelectedCostumeSet( 0 )
		{

		};
	};

	// 서버로부터 받은 현재 가챠폰 샵 id
	int m_iGachaShopID;

	CEtUIButton* m_pBtnRotateLeft;
	CEtUIButton* m_pBtnRotateRight;
	CEtUIButton* m_pBtnRotateFront;
	CEtUIButton* m_pBtnRotateBack;
	CEtUIButton* m_pBtnAvatarViewArea;

	CEtUITextBox* m_pTextBoxSetName;

	CDnRenderAvatarNew m_RenderAvatar;

	// 캐릭터를 마우스로 직접 조작.
	float m_fAvatarViewMouseX;
	float m_fAvatarViewMouseY;

	// 테이블에서 읽어들인 가챠 npc 별로 보여줄 수 있는 직업별 외형 정보.
	// 이 다이얼로그가 뜰 때 서버에서 보내준 값으로 초기화된다.
	map<int, S_COSTUME_SET_INFO> m_mapCostumeSetInfoByJob;
	int m_iNowSelectedClassID;

	// 가챠폰 다이얼로그에서 뽑을 파츠를 선택하면 셋팅되는 변수.
	// 이 변수를 기준으로 파츠가 계속 바뀌는 연출 시작.
	int m_iSelectedPart;
	int m_iRouletteCostumePartIndex;
	float m_fPartChangeTime;
	float m_fElapsedTime;

	// 현재 내 캐릭터가 입고 있는 코스튬이 프리뷰에 나온 상태.
	bool m_bSelectedMyJobClassCostume;

protected:
	void _ChangeCostumeParts( const S_COSTUME_INFO& CostumeInfo );

public:
	CDnGachaPreview( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGachaPreview( void );

	// 가챠폰 샵 ID 를 서버에서 받으면 셋팅해주어야 함.
	void SetGachaponShopID( int iShopID ) { m_iGachaShopID = iShopID; };

	// UI 관련 함수들
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate( void );
	virtual void Show( bool bShow );
	virtual void Process( float fDelta );
	virtual void Render( float fDelta );
	// 이벤트 처리
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 아바타 관련 함수들
	void InitializeCostumeSetInfo( int iGachaTableID );

	void AttachCostume( int iClassID, bool bAllowAttachLocalActorCostume = true );
	void AttachPart( int iItemID );
	void UpdateJobSelection( int iRootJobID );
	void NextCostume( void );
	void PrevCostume( void );
	void SelectedPart( int iSelectedPart );

	// GachaDialog 에서 데이터 사용.
	int GetNumGachPartItem( int iClassID );
	int GetGachaSelectedPartItemID( int iClassID, int iIndex );

	// GachaDialog 에서도 사용하는 유틸 함수.
	static int GetItemNeedJob( DNTableFileFormat*  pItemTable, int iItemTableID );
};
#endif // PRE_ADD_GACHA_JAPAN