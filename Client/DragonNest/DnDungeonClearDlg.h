#pragma once
#include "DnCustomDlg.h"
#include "DnGaugeFace.h"
#include "DnGameTask.h"
#include "DnBlindDlg.h"
#include "DnDungeonClearImp.h"

class CDnDungeonClearMoveDlg;

class CDnDungeonClearDlg : public CDnCustomDlg, public CDnBlindCallBack
{
public:
	enum
	{
		DUNGEON_CLEAR_MOVE_DIALOG,
	};

protected:
	struct SPlayerRecordInfo
	{
		CEtUIStatic *m_pStaticTitle;
		CEtUIStatic *m_pStatic00;
		CEtUIStatic *m_pStatic01;
		CEtUIStatic *m_pStatic02;

		void ClearText()
		{
			m_pStaticTitle->ClearText();
			m_pStatic00->ClearText();
			m_pStatic01->ClearText();
			m_pStatic02->ClearText();
		}
	};

	struct SMemberRecordInfo
	{
		CEtUITextureControl *m_pFaceImage;
		CEtUIStatic *m_pStaticLevel;
		CEtUIStatic *m_pStaticName;
		CEtUIStatic *m_pStaticRank;
		CEtUIStatic *m_pStaticA;
		CEtUIStatic *m_pStaticB;
		CEtUIStatic *m_pStaticC;
		CEtUIStatic *m_pStaticResult;
		CEtUITextureControl *m_pItemImage;
		CEtUIStatic *m_pStaticCheck;

		void ClearAll()
		{
			m_pFaceImage->Show(false);
			m_pStaticLevel->ClearText();
			m_pStaticName->ClearText();
			m_pStaticRank->ClearText();
			m_pStaticA->ClearText();
			m_pStaticB->ClearText();
			m_pStaticC->ClearText();
			m_pStaticResult->ClearText();
			m_pItemImage->Show(false);
			m_pStaticCheck->Show(false);
		}

		void ClearChangePart()
		{
			m_pStaticA->ClearText();
			m_pStaticB->ClearText();
			m_pStaticC->ClearText();
			m_pStaticResult->ClearText();
		}
	};

	enum
	{
		PLAYER_RECORD_COUNT = 3,
		MEMBER_RECORD_COUNT = 8,
	};

	enum
	{
		TAB_FINAL = 1,
		TAB_PLAYER,
		TAB_PARTY,
		TAB_DEMERIT,
	};

	enum
	{
		RANK_SSS,
		RANK_SS,
		RANK_S,
		RANK_A,
		RANK_B,
		RANK_C,
		RANK_D,
		RANL_MAX,
	};

public:
	CDnDungeonClearDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDungeonClearDlg(void);

protected:
	//CDnDungeonClearMoveDlg *m_pDungeonClearMoveDlg;
	CDnGaugeFace *m_pPlayerFace;
	CEtUIButton *m_pButtonBonusBox;
	CEtUIButton *m_pButtonOK;
	CEtUIRadioButton *m_pTabFinal;
	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticType;
	CEtUIStatic *m_pStaticExp;
	CEtUIStatic *m_pStaticCp;
	CEtUIStatic *m_pStaticResultA;
	CEtUIStatic *m_pStaticResultB;
	CEtUIStatic *m_pStaticResultC;
	CEtUITextureControl *m_pRankImage;

	std::vector<SPlayerRecordInfo> m_vecPlayerRecordInfo;
	std::vector<SMemberRecordInfo> m_vecMemberRecordInfo;

	EtTextureHandle m_hRankTexture;
	EtTextureHandle m_hFaceTexture;

	EtTextureHandle m_hItemIconList[ITEM_TEXTURE_COUNT];

	CEtUIStatic *m_pStaticTimer;
	float m_fElapsedTime;
	int m_nCount;
	wchar_t m_wszTimer[80];
	int m_nCurrentTabID;
	bool m_bTimer;

	CDnDungeonClearImp::SDungeonClearInfo *m_pPlayerDungeonClearInfo;
	CDnDungeonClearImp::SDungeonClearValue *m_pDungeonClearValue;

protected:
	void SetPlayerInfo();
	void SetPlayerRecord( int nTabID );
	void SetMemberBaseRecordInfo();
	void SetMemberRecordInfo( int nTabID );
	void SetTitle( int nTabID );
	void ChangedTabButton( int nTabID );
	void SetTime( int nTime );

public:
	void OpenDungeonMoveDialog();
	void SetCheckRecordInfo( int nSessionID );
	void SetCallbackEx( CEtUICallback *pCallBack );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

public:
	virtual void OnBlindOpen();
	virtual void OnBlindOpened();
	virtual void OnBlindClose();
	virtual void OnBlindClosed();
};