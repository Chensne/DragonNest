#pragma once
#include "DnCustomDlg.h"

const int NPC_REPUTATION_ALBUM_COUNT_PER_PAGE = 8;

class CDnNpcReputationDlg;
class CDnNpcReputationBromideDlg;

// ȣ���� �� ���̾�α�.
// ȣ���� ����Ʈ ���̾�α׿� ȣ���� �ٹ� ���̾�α׸� ������ �ִ�.
class CDnNpcReputationAlbumDlg : public CDnCustomDlg,
								 public CEtUICallback
{
private:
	enum
	{
		NPC_SORT_DEFAULT,
		NPC_SORT_FAVOR,
		NPC_SORT_MALICE,
		NPC_SORT_NAME,
	};

	struct S_ALBUM_ELEMENT
	{
		//EtTextureHandle hTexture;		// ���� �ؽ���.
		CEtUITextureControl* pPictureCtrl;
		CEtUIStatic* pStaticEventReceiver;
		CEtUIStatic* pName;

		struct S_ALBUM_ELEMENT( void ) : pPictureCtrl( NULL ), pStaticEventReceiver( NULL ), pName( NULL ) {};
		void Reset( void )
		{
			//SAFE_RELEASE_SPTR( hTexture );
			pPictureCtrl->Show( false );
			pName->ClearText();
		}
	};

	struct S_AVAILALBUM_INFO
	{
		int iReputeTableID;
		int iAlbumListOffset;		// �ٹ� ����Ʈ���� ���° �ε������� �� npc �� �ٹ������� ����Ǿ�����.

		vector<int>		vlPortraitIndex; // �ش� npc �� ��θ��̵�� �ʻ�ȭ �ؽ��Ŀ����� �ε���. ���� npc �� �ִ� 3������ ������.
		vector<string>	vlBromideTextureFileName;
#ifdef PRE_ADD_REPUTATION_EXPOSURE
		vector<int>		vIBromideQuestID;		// ��θ��̵尡 ������ ����Ʈ ID 		(�ٹ� ����:1�� üũ)
		vector<int>		vIBromideQuestEnumID;	// ��θ��̵尡 ������ ����Ʈ Enum ID	(�ٹ� ����:1�� üũ)
		vector<int>		vIBromideRepute;		// ��θ��̵尡 ������ ȣ���� ��ġ		(�ٹ� ����:2�� üũ)
		vector<bool>	vbIsFoundAlbum;			// ȣ������ üũ�Ͽ�, �ٹ��� �����ִ��� ���θ� ����.
#endif
		S_AVAILALBUM_INFO( void ) : iReputeTableID( -1 ), iAlbumListOffset( -1 ) {};
	};

	S_ALBUM_ELEMENT m_aElements[ NPC_REPUTATION_ALBUM_COUNT_PER_PAGE ];

	CEtUIComboBox* m_pComboSort;
	CEtUIStatic* m_pStaticPage;
	CEtUIButton* m_pBtnPrevPage;
	CEtUIButton* m_pBtnNextPage;

#ifdef PRE_ADD_REPUTATION_EXPOSURE 
	CEtUIStatic* m_pStaticAchievePercent;
	EtTextureHandle m_hGrayScalePortraitTexture;
	
	vector<bool> m_vbIsFoundAlbum;		// ȣ������ üũ�Ͽ�, �ٹ��� �����ִ��� ���θ� ����.
	vector<int>	 m_vIBromideQuestID;	// ��θ��̵� ȹ�� ���������� ����Ʈ ID			(�ٹ� ����:1�� üũ)
	vector<int>	 m_vIBromideQuestEnumID;// ��θ��̵� ȹ�� ���������� ����Ʈ Enum ID	(�ٹ� ����:1�� üũ)
	vector<int>  m_vIBromideRepute;		// ��θ��̵� ȹ�� ���� ȣ����					(�ٹ� ����:2�� üũ)
#endif

	int m_iNowPage;
	int m_iMaxPage;
	int m_iNowSelectedSortMethod;
	
	//vector<int> m_vlAvailNpcReputeTableIDs;
	// �ش� npc �� ��θ��̵� �ε���. ���� npc �� �ִ� 3������ ������. �� ������ ���� �� ��ģ���� �������� �� ��θ��̵��� ������ �ȴ�.
	//vector<int> m_vlBromideIndices;
	vector<S_AVAILALBUM_INFO>	m_vlAvailAlbumInfos;
	vector<int>					m_vlReputeTableID;		// ���� ���� ��θ��̵��� ���̺� id ��.
	vector<int>					m_vlPortrait;			// ���� ���� ��θ��̵��� �ʻ�ȭ �ؽ����� �ε��� 
	vector<string>				m_vlBromideFileName;	// ���� ���� ��θ��̵��� �ؽ��� �̸�.
	EtTextureHandle				m_hPortraitTexture;

	// ���� �׸��� Ŭ���ϸ� ũ�� �����ִ� ��θ��̵� ���̾�α�.
	CDnNpcReputationBromideDlg* m_pBromideDlg;
	bool m_bInitialized;
	bool m_bShowFromBromide; // ��θ��̵�κ��� �ٹ����� ���ƿ��� ���. �� ��쿣 ������ ���� �־��� �������� �������� �ش�.

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	bool IsOpenBromide(int iBromideIndex);
	void ShowBromideHelpTooltip(float fX, float fY);
#endif

protected:
	void _UpdateAvailAlbumList( int iSortMethod ); 

public:
	CDnNpcReputationAlbumDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, 
							int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnNpcReputationAlbumDlg( void );

	void Initialize( bool bShow );
	void InitialUpdate(	void );
	void Show( bool bShow );
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ );

	void Clear( void );
	void UpdatePage( int iPage );
	void RefreshToCurrentPage( void ) { UpdatePage( m_iNowPage ); };

	void SetBromideDlg( CDnNpcReputationBromideDlg* pBromoideDlg ) { m_pBromideDlg = pBromoideDlg; };
	void ShowFromBromide( void ) { m_bShowFromBromide = true; };

	// ���콺 �� ó�� �߰� [2010/11/16 semozz]
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void RefreshPage(bool bInc);
}; 