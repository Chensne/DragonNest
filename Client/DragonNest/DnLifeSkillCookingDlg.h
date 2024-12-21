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
		CDnItemSlotButton*	m_pCookingSlotBtn;		// �丮 ������
		CEtUIStatic*		m_pStaticTitle;			// �丮����
		CEtUIStatic*		m_pStaticLevel;			// ��������
		CEtUIStatic*		m_pStaticPercent;		// ���ô޼���
		CEtUIStatic*		m_pStaticSelect;		// ����ǥ��
		CEtUIStatic*		m_pStaticBackground;	// ��׶��� �̹���
		CEtUIStatic*		m_pStaticLevelText;		// �������� �ؽ�Ʈ
		CEtUIProgressBar*	m_pProgressAchievement;	// ���ô޼� ���α׷�����

		stUICookingListInfo() : m_pCookingSlotBtn( NULL ), m_pStaticTitle( NULL ), m_pStaticLevel( NULL ), m_pStaticPercent( NULL ), 
			m_pStaticSelect( NULL ), m_pStaticBackground( NULL ), m_pStaticLevelText( NULL ), m_pProgressAchievement( NULL )
		{
		}
	};

	stUICookingListInfo m_stUICookingListInfo[ MAX_COOKING_LIST ];

	CEtUIButton* m_pButtonAddRecipe;		// ������ �߰�
	CEtUIButton* m_pButtonRemoveRecipe;		// ������ ����
	CEtUIButton* m_pButtonCook;				// �丮�ϱ�
	CEtUIButton* m_pButtonCookAll;			// ��ο丮�ϱ�
	CEtUIButton* m_pButtonExtract;			// �丮 ����
	CEtUIButton* m_pButtonCancel;			// �丮 ��� (â �ݱ�)
	CEtUIButton* m_pButtonPrevious;			// ���� �丮����Ʈ
	CEtUIButton* m_pButtonNext;				// ���� �丮����Ʈ
	CEtUIStatic* m_pStaticPage;				// �丮 ������

	// ���õ� ������ ���� ǥ��
	CDnItemSlotButton*	m_pCookingSlotBtn;		// �丮 ������
	CEtUIStatic*		m_pStaticTitle;			// �丮����
	CEtUIStatic*		m_pStaticLevel;			// ��������
	CEtUIStatic*		m_pStaticCompletion;	// �丮 �ϼ���

	CDnItemSlotButton*	m_pRequiredToolSlotBtn[ MAX_REQUIRED_NUM ];			// �ʿ䵵�� ������
	CDnItemSlotButton*	m_pRequiredIngredientSlotBtn[ MAX_REQUIRED_NUM ];	// �ʿ���� ������

	int m_nCurrentPage;				// ���� ������
	int m_nMaximumPage;				// �ִ� ������
	int m_nCurrentPageListCount;	// ���� ������ �׸� �� ��
	int m_nSelectRecipe;			// ������ �׸�
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
	std::vector<CDnItem*>	m_vecRecipeIDList;					// ������ ������ ����Ʈ

	CDnItemSlotButton*	m_pRecipeSlotBtn[ MAX_RECIPE_LIST ];	// ������ ������
	CEtUIStatic*		m_pRecipeSelect[ MAX_RECIPE_LIST ];		// ������ ���� ǥ��
	CEtUIButton*		m_pButtonPrevious;						// ���� �����Ǹ���Ʈ
	CEtUIButton*		m_pButtonNext;							// ���� �����Ǹ���Ʈ
	CEtUIStatic*		m_pStaticPage;							// �丮 ������
	CEtUIButton*		m_pButtonAddRecipe;						// ������ �߰�

	int m_nCurrentPage;				// ���� ������
	int m_nMaximumPage;				// �ִ� ������
	int m_nCurrentPageListCount;	// ���� ������ �׸� �� ��
	int m_nSelectRecipe;			// ������ �׸�
};

#endif // PRE_ADD_COOKING_SYSTEM