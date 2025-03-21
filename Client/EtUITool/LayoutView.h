#pragma once

#include "resource.h"
#include "EtUIDialog.h"
#include "undo.h"

class CUIToolTemplate;
class CEtUIControl;
class CUnionValueProperty;

typedef
enum SELECT_TYPE_
{
	SELECT_NONE,
	SELECT_DONE,
	SELECT_LAST
} SELECT_TYPE;

// NOTE : 컨트롤의 정렬 타입
//
enum ALLIGN_TYPE
{
	ALLIGN_LEFT,
	ALLIGN_CENTER,
	ALLIGN_RIGHT,
	ALLIGN_TOP,
	ALLIGN_MIDDLE,
	ALLIGN_BOTTOM,
	ALLIGN_HORIZONTAL,
	ALLIGN_VERTICAL
};

// NOTE : 컨트롤의 사이즈 맞추기 타입
//
enum MAKESIZE_TYPE
{
	MAKESIZE_WIDTH,
	MAKESIZE_HEIGHT,
	MAKESIZE_BOTH
};

struct SCreateControlInfo
{
	SCreateControlInfo()
	{
		pControl = NULL;
		emSelect = SELECT_NONE;
		nTabOrder = 0;
	}

	CEtUIControl *pControl;
	SELECT_TYPE emSelect;
	SUIControlProperty Property;
	int nTabOrder;
};

// CLayoutView form view

class CLayoutView : public CFormView, public CUndo
{
	DECLARE_DYNCREATE(CLayoutView)

protected:
	CLayoutView();           // protected constructor used by dynamic creation
	virtual ~CLayoutView();

public:
	enum { IDD = IDD_LAYOUTVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	enum
	{
		NUM_CONTROL_GROUP = 8,
	};

protected:
	CPoint m_CusorPt;
	RECT m_CurDragRect;
	CEtUIControl *m_pMoveControl;
	bool m_bMoveMode;
	bool m_bDragMode;
	int m_nCurTabOrder;
	int m_nWideScreen;
	
	bool IsCurTabOrder() { return m_nCurTabOrder != -1;}

	CString m_szCurFileName;
	CEtUIDialog m_LayoutDlg;

	SUIDialogInfo m_DlgInfo;
	std::vector< CUIToolTemplate * > m_vecLayoutTemplate;
	std::vector< SCreateControlInfo > m_vecLayoutControl;
	std::vector< SCreateControlInfo > m_vecLayoutControlCopy;

	// 인덱스로 두면 중간에 바뀔수도 있어서, 컨트롤 포인터 들고있으려 했는데, Undo때문에 안된다. 이름 들고 비교. 이름 같으면 할수없다.
	std::vector< std::string > m_vecControlGroup[NUM_CONTROL_GROUP];
	int m_nLastSelectGroup;

	bool m_bTabOrderMode;
	bool m_bInitStateMode;

	int m_nViewCrosshair;

	// 그냥 임시로 보이는 뒷배경 다이얼로그.
	CEtUIDialog m_BackgroundDlg;

	// 그냥 임시로 보이는 뒷배경 다이얼로그 리스트.
	std::list<CEtUIDialog *> m_listBackgroundDlg;

	// SaveCustomUI

protected:
	virtual void UndoLoad(CMemFile* pFile);
	virtual void UndoStore(CMemFile* pFile);

	std::vector< std::string > m_vecNotUseTemplate;
	void LoadNotUseTemplateList();
	bool IsNotUseTemplate( CEtUITemplate *pTemplate );

public:
	void StartTabOrder();
	void EndTabOrder();
	bool SetTabOrder( int nOrder, CEtUIControl *pControl );
	void AddTabOrder( int nStart, int nEnd, int nAdd );

	void StartInitStateMode();
	void EndInitStateMode();

	void AllignControl( ALLIGN_TYPE emAllign, int nInterval = 0 );
	void MakeSameSizeControl( MAKESIZE_TYPE emMakeSize );

	CEtUIDialog& GetDialog() { return m_LayoutDlg; }
	void GetLayoutDlgPosition( float &fX, float &fY );
	void SetDefaultControl( SUIControlProperty *pProperty );
	int GetControlCount() { return ( int )m_vecLayoutControl.size(); }
	int FindControl( CEtUIControl *pControl );
	CEtUIControl *GetControl( POINT Point );
	CEtUIControl *GetSelectedControl();
	
	CEtUIControl* SelectControl( POINT Point, bool bMultiSelect );
	void SelectControl( int nIndex );
	void SelectAllControl( bool bSelect );
	void SelectControlRect();
	
	bool IsSelectedControl( CEtUIControl *pControl );
	int GetSelectedControlCount();
	int GetLastSelectedControlIndex();
	void MoveSelectedControl( POINT &Point );

	// 멀티셀렉트 상태에서 컨트롤 속성 변경시 멀티셀렉트 된 컨트롤들 속성 한번에 바꾸기.
	void ChangeValueSelectedControl( CUnionValueProperty *pVariable, DWORD dwIndex );

	void ResizeSelectedControl( SUICoord &ControlCoord );
	void ResizeDialog( SUICoord &ControlCoord );
	void SortDialogControl();
	
	void GenerateControlID( UI_CONTROL_TYPE Type, char *pszControlID );
	bool IsExistControlID( const char *pszControlID );
	void DeleteSelectedControl();
	void AddLayoutControl( SUICoord &ControlCoord, bool bUseTemplateCoord);
	BOOL CheckMultiSelectedCount();

	const char *GetTemplateName( SUIControlProperty *pProperty );

	void DrawSelectControl();
	void DrawTabOrder();
	void DrawInitState();

	void ReinitAllControl();
	void ReinitAllTemplate();
	void RegenerateTexture();
	void ReconstructLayout();
	void RefreshLayoutDlg();
	int AddLayoutTemplate( CUIToolTemplate *pTemplate, bool &bAdd );

	void NewLayoutDlg();
	void SaveLayoutDlg( const char *pszFileName, bool bDoNotCompressTexture = false, bool bMessageBoxSaveFailed = true );
	void LoadLayoutDlg( const char *pszFileName );

	// 일부 특정 템플릿을 사용하는 UI파일들을 찾아 보여주고 새로 저장
	void SaveCustomUI();
	void _SearchSubFolderUIFile( const char *pszFolderName );
	int m_nCurCustomControlIDforCustomSave;

	// 커스텀 템플릿이 아닌 일반 템플릿을 사용하는 UI파일들을 찾아 보여주고 새로 저장
	void SaveUIWithUsedTemplate();
	void _SaveUIWithUsedTemplate( const char *pszFolderName );
	std::string m_nCurSelTemplateNameforCustomSave;

	// 몽땅 열고 다시 저장하기.
	void SaveAllUI();
	void _SaveAllUI( const char *pszFolderName );

	void AddBackgroundDialog( const char *pFileName );
	void ClearBackgroundDialog();
	void DeleteBackgroundDialog( int nIndex );

	// 그룹화
	void MakeGroupSelectedControl( int nGroupIndex );
	void SelectControlGroup( int nGroupIndex );
	bool IsGroupControl( int nGroupIndex, CEtUIControl *pControl );
	void DrawControlGroupInfo( CDC* pDC, int nX, int nY );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnOpenView( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCloseView( WPARAM wParam, LPARAM lParam );
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();

	afx_msg void OnUpdateAllignBottom(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAllignCenter(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAllignLeft(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAllignMiddle(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAllignRight(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAllignTop(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMakesamesizeBoth(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMakesamesizeHeight(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMakesamesizeWidth(CCmdUI *pCmdUI);
	afx_msg void OnAllignBottom();
	afx_msg void OnAllignCenter();
	afx_msg void OnAllignLeft();
	afx_msg void OnAllignMiddle();
	afx_msg void OnAllignRight();
	afx_msg void OnAllignTop();
	afx_msg void OnMakesamesizeBoth();
	afx_msg void OnMakesamesizeHeight();
	afx_msg void OnMakesamesizeWidth();
	afx_msg void OnFormatTaborder();
	afx_msg void OnUpdateFormatTaborder(CCmdUI *pCmdUI);
	virtual void OnInitialUpdate();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();

protected:
	virtual void PostNcDestroy();

public:
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnEditSelectall();
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnCenterindialogVertical();
	afx_msg void OnCenterindialogHorizontal();
	afx_msg void OnEditDeletedialogtexture();
	afx_msg void OnAllignHorizontal();
	afx_msg void OnAllignVertical();
	afx_msg void OnUpdateAllignHorizontal(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAllignVertical(CCmdUI *pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAddCoinTemplate();
	afx_msg void OnViewCrosshair();
	afx_msg void OnUpdateViewCrosshair(CCmdUI *pCmdUI);
	afx_msg void OnViewWidescreen();
	afx_msg void OnUpdateViewWidescreen(CCmdUI *pCmdUI);
	afx_msg void OnFormatMakesameproperty();
	afx_msg void OnUpdateFormatMakesameproperty(CCmdUI *pCmdUI);
	afx_msg void OnDelNotusetemplate();
	afx_msg void OnFormatInitstate();
	afx_msg void OnUpdateFormatInitstate(CCmdUI *pCmdUI);
};


