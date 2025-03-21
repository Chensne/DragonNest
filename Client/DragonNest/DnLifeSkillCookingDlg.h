#pragma once
#include "DnCustomDlg.h"

#ifdef PRE_ADD_COOKING_SYSTEM

#define MAX_COOKING_LIST	4
#define MAX_REQUIRED_NUM	5

class CDnLifeSkillCookingRecipeDlg;
class CSecondarySkillRecipe;
class CDnItem;

class CDnLifeSkillCookingDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnLifeSkillCookingDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeSkillCookingDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	void ClearSelectedRecipe();
	void UpdateCookingList();
	void ResetSelectedRecipeInfo();

private:
	CSecondarySkillRecipe* GetCurrentCookRecipe();
	void SetSelectedRecipeInfo();
	void UpdatePageUI();

private:
	enum { MSGBOX_DELETE_RECIPE, MSGBOX_EXTRACT_RECIPE };

	CDnLifeSkillCookingRecipeDlg* m_pDnLifeSkillCookingRecipeDlg;

	struct stUICookingListInfo
	{
		CDnItemSlotButton*	m_pCookingSlotBtn;		// 요리 아이콘
		CEtUIStatic*		m_pStaticTitle;			// 요리제목
		CEtUIStatic*		m_pStaticLevel;			// 레벨제한
		CEtUIStatic*		m_pStaticPercent;		// 숙련달성율
		CEtUIStatic*		m_pStaticSelect;		// 선택표시
		CEtUIStatic*		m_pStaticBackground;	// 백그라운드 이미지
		CEtUIStatic*		m_pStaticLevelText;		// 레벨제한 텍스트
		CEtUIProgressBar*	m_pProgressAchievement;	// 숙련달성 프로그래스바

		stUICookingListInfo() : m_pCookingSlotBtn( NULL ), m_pStaticTitle( NULL ), m_pStaticLevel( NULL ), m_pStaticPercent( NULL ), 
			m_pStaticSelect( NULL ), m_pStaticBackground( NULL ), m_pStaticLevelText( NULL ), m_pProgressAchievement( NULL )
		{
		}
	};

	stUICookingListInfo m_stUICookingListInfo[ MAX_COOKING_LIST ];

	CEtUIButton* m_pButtonAddRecipe;		// 레시피 추가
	CEtUIButton* m_pButtonRemoveRecipe;		// 레시피 삭제
	CEtUIButton* m_pButtonCook;				// 요리하기
	CEtUIButton* m_pButtonCookAll;			// 모두요리하기
	CEtUIButton* m_pButtonExtract;			// 요리 추출
	CEtUIButton* m_pButtonCancel;			// 요리 취소 (창 닫기)
	CEtUIButton* m_pButtonPrevious;			// 이전 요리리스트
	CEtUIButton* m_pButtonNext;				// 다음 요리리스트
	CEtUIStatic* m_pStaticPage;				// 요리 페이지

	// 선택된 레시피 정보 표기
	CDnItemSlotButton*	m_pCookingSlotBtn;		// 요리 아이콘
	CEtUIStatic*		m_pStaticTitle;			// 요리제목
	CEtUIStatic*		m_pStaticLevel;			// 레벨제한
	CEtUIStatic*		m_pStaticCompletion;	// 요리 완성도

	CDnItemSlotButton*	m_pRequiredToolSlotBtn[ MAX_REQUIRED_NUM ];			// 필요도구 아이콘
	CDnItemSlotButton*	m_pRequiredIngredientSlotBtn[ MAX_REQUIRED_NUM ];	// 필요재료 아이콘

	int m_nCurrentPage;				// 현재 페이지
	int m_nMaximumPage;				// 최대 페이지
	int m_nCurrentPageListCount;	// 현재 페이지 항목 갯 수
	int m_nSelectRecipe;			// 선택한 항목
};

#define MAX_RECIPE_LIST	16

class CDnLifeSkillCookingRecipeDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnLifeSkillCookingRecipeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeSkillCookingRecipeDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	void UpdaetRecipeList();
	void SetRecipePageList();
	void UpdatePageUI();

private:
	std::vector<CDnItem*>	m_vecRecipeIDList;					// 레시피 아이템 리스트

	CDnItemSlotButton*	m_pRecipeSlotBtn[ MAX_RECIPE_LIST ];	// 레시피 아이콘
	CEtUIStatic*		m_pRecipeSelect[ MAX_RECIPE_LIST ];		// 레시피 선택 표시
	CEtUIButton*		m_pButtonPrevious;						// 이전 레시피리스트
	CEtUIButton*		m_pButtonNext;							// 다음 레시피리스트
	CEtUIStatic*		m_pStaticPage;							// 요리 페이지
	CEtUIButton*		m_pButtonAddRecipe;						// 레시피 추가

	int m_nCurrentPage;				// 현재 페이지
	int m_nMaximumPage;				// 최대 페이지
	int m_nCurrentPageListCount;	// 현재 페이지 항목 갯 수
	int m_nSelectRecipe;			// 선택한 항목
};

#endif // PRE_ADD_COOKING_SYSTEM