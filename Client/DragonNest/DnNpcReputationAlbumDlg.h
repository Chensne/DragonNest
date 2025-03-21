#pragma once
#include "DnCustomDlg.h"

const int NPC_REPUTATION_ALBUM_COUNT_PER_PAGE = 8;

class CDnNpcReputationDlg;
class CDnNpcReputationBromideDlg;

// 호감도 탭 다이얼로그.
// 호감도 리스트 다이얼로그와 호감도 앨범 다이얼로그를 가지고 있다.
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
		//EtTextureHandle hTexture;		// 작은 텍스쳐.
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
		int iAlbumListOffset;		// 앨범 리스트에서 몇번째 인덱스부터 이 npc 의 앨범들인지 저장되어있음.

		vector<int>		vlPortraitIndex; // 해당 npc 의 브로마이드용 초상화 텍스쳐에서의 인덱스. 현재 npc 당 최대 3개까지 제공됨.
		vector<string>	vlBromideTextureFileName;
#ifdef PRE_ADD_REPUTATION_EXPOSURE
		vector<int>		vIBromideQuestID;		// 브로마이드가 열리는 퀘스트 ID 		(앨범 오픈:1차 체크)
		vector<int>		vIBromideQuestEnumID;	// 브로마이드가 열리는 퀘스트 Enum ID	(앨범 오픈:1차 체크)
		vector<int>		vIBromideRepute;		// 브로마이드가 열리는 호감도 수치		(앨범 오픈:2차 체크)
		vector<bool>	vbIsFoundAlbum;			// 호감도를 체크하여, 앨범을 볼수있는지 여부를 저장.
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
	
	vector<bool> m_vbIsFoundAlbum;		// 호감도를 체크하여, 앨범을 볼수있는지 여부를 저장.
	vector<int>	 m_vIBromideQuestID;	// 브로마이드 획득 가능해지는 퀘스트 ID			(앨범 오픈:1차 체크)
	vector<int>	 m_vIBromideQuestEnumID;// 브로마이드 획득 가능해지는 퀘스트 Enum ID	(앨범 오픈:1차 체크)
	vector<int>  m_vIBromideRepute;		// 브로마이드 획득 가능 호감도					(앨범 오픈:2차 체크)
#endif

	int m_iNowPage;
	int m_iMaxPage;
	int m_iNowSelectedSortMethod;
	
	//vector<int> m_vlAvailNpcReputeTableIDs;
	// 해당 npc 의 브로마이드 인덱스. 현재 npc 당 최대 3개까지 제공됨. 이 벡터의 수를 다 합친것이 보여지게 될 브로마이드의 갯수가 된다.
	//vector<int> m_vlBromideIndices;
	vector<S_AVAILALBUM_INFO>	m_vlAvailAlbumInfos;
	vector<int>					m_vlReputeTableID;		// 현재 얻은 브로마이드의 테이블 id 들.
	vector<int>					m_vlPortrait;			// 현재 얻은 브로마이드의 초상화 텍스쳐의 인덱스 
	vector<string>				m_vlBromideFileName;	// 현재 얻은 브로마이드의 텍스쳐 이름.
	EtTextureHandle				m_hPortraitTexture;

	// 작은 그림을 클릭하면 크게 보여주는 브로마이드 다이얼로그.
	CDnNpcReputationBromideDlg* m_pBromideDlg;
	bool m_bInitialized;
	bool m_bShowFromBromide; // 브로마이드로부터 앨범으로 돌아오는 경우. 이 경우엔 직전에 보고 있었던 페이지를 유지시켜 준다.

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

	// 마우스 휠 처리 추가 [2010/11/16 semozz]
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void RefreshPage(bool bInc);
}; 