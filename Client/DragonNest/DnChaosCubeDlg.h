#pragma once


#ifdef PRE_ADD_CHAOSCUBE

#include "DnCustomDlg.h"

//-----------------------------------------------
// ī����ť�� ����â.

class CDnChaosCubeDlg : public CDnCustomDlg, public CEtUICallback
{

private:

	// Controls ------------
	CDnItemSlotButton * m_pSlotBtnSelected; // ��������ώ��.
	std::vector< CDnItemSlotButton * > m_vSlotBtnMaterials; // ��ώ�Ե�.
	std::vector< CEtUIStatic * > m_vStaticSelect; // ��ώ�� ���� Static.

	CEtUIStatic * m_pStaticLight; // ��¦��.
	CEtUIStatic * m_pStaticTitle; // Ÿ��Ʋ��.
	CEtUIStatic * m_pStaticHelp;  // ����.
	CEtUIStatic * m_pStaticPage;  // Page/MaxPage.

	CEtUIButton * m_pBtnPrev;     // ����.
	CEtUIButton * m_pBtnNext;     // ����.
	CEtUIButton * m_pBtnMaterial; // ���.
	CEtUIButton * m_pBtnProduct;  // �����.
	CEtUIButton * m_pBtnConfirm;  // Ȯ��.
	// -----------------------


	//------------------------
	// Child Dlg
	//class CDnChaosCubeStuffDlg * m_pChildDlg; // ���Dlg or �����Dlg.
	//class CDnChaosCubeProgressDlg * m_pChaosCubeProgressDlg; // ���α׷��� Dlg.
	//class CDnChaosCubeResultDlg * m_pChaosCubeResultDlg; // ȹ������ Dlg.
	//------------------------


	class CDnItem * m_pItemChaos; // UI���¿� ī����������.
	INT64 m_nChaosSerial;
	
	std::map< int, std::vector< CDnItem * > > m_mapStuffItems; // <ItemID, �κ������� �����۵�> �κ����� �˻��� �������۵�.
															   // ����ID�� �������� �������Կ� ������������ ���� �Ҹ𰳼��� �޶��� �� �����Ƿ� ��������.

	std::vector< int > m_vStuffItemIDs;     // �������۵�.
	std::map< int, int > m_vNeedStuffCount; // <ItemID, ����> �ͽ��� �ʿ��� ��ᰳ��.

	// Pageó��.
	const int m_CountPerPage; // �������� ���԰���.
	int m_crrPage;			  // ����������.
	int m_maxPage;			  // ��ü������.

	// ���õ���� ����ó��.
	float m_fAlphaColor; // ���İ�.
	short m_signLight;   // ������ώ�� ����ó�� ��ȣ.

	bool m_bSelfItemList; // ����۰� ���� ������ �ε�����.

	// ������ ��ώ�� �ε���.
	int m_SelectedSlotIndex;

public:

	enum EChildDlgMode
	{
		NONE = 0,
		STUFF = 1,   // ���â.  - ī����ť��
		PRODUCT = 2, // �����â.- ī����ť��

		STUFF_CHOICE = 3, // ���â   - ���̽�ť��
		PRODUCT_CHOICE = 4, // �����â - ���̽�ť��
	};

public:

	CDnChaosCubeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnChaosCubeDlg(){
		ReleaseDlg();
	}
	
	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Show( bool bShow );

	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

public:

	// ī����������.
	void SetChaoseItem( class CDnItem * pItem );
	class CDnItem * GetChaosItem(){
		return m_pItemChaos; // UI���¿� ī����������.
	}

	// Open ProgressDlg.
	//void OpenProgressDlg( bool bShow, struct SCChaosCubeRequest * pData );

	// Open ResultDlg;
	//void OpenResultDlg( bool bShow, int nItemID, int nCount );

	void UpdateItemList(); // �����۸�ϰ���.

	void MixComplete(); // �ͽ��Ϸ�.

	void RecvResultItemComplet(); // ��������� ���ɿϷ�.

	void RecvMixStart( bool bEnableBtn ); // �ͽ����� ����.

private:

	// Dlg Open�ÿ� ��Ḧ �˻��Ͽ� ����Ѵ�.
	void OpenDlg();

	// Dlg Close�ÿ� ����.
	void CloseDlg();

	// ��ώ�Կ� �����.
	void RefreshStuffItemSlot();

	// �������ۼ���.
	void SelectionStuffItem( const char * strCtlName );

	// Mix.
	void MixStart();
	
};


#endif