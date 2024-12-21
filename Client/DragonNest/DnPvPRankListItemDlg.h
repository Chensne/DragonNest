#pragma once

#ifdef PRE_ADD_PVP_RANKING

#include "DnCustomDlg.h"


class CDnPvPRankListItemDlg : public CDnCustomDlg
{
protected:
	CEtUIStatic * m_pStaticRank;  // ����.
	CEtUIStatic * m_pStaticLevel; // ����.	
	CEtUIStatic * m_pStaticGuild; // ���.
	CEtUIStatic * m_pStaticName;  // �̸�.
	CEtUIStatic * m_pStaticResult;// ���.
	CEtUIStatic * m_pStaticPoint; // ����.
	CEtUITextureControl * m_pTextureControl; // �ݷμ�����.

	CDnJobIconStatic * m_pJobIcon; // ����������. 

	struct SClassRank
	{
		int jobCode;
		INT64	biClassRank;
		INT64	biChangedClassRank;
		INT64	biSubClassRank;	
		INT64	biChangedSubClassRank;
		SClassRank() : jobCode( 0 ), biClassRank( 0 ), biChangedClassRank( 0 ), biSubClassRank( 0 ), biChangedSubClassRank( 0 ) {}
	};
	SClassRank m_ClassRank;

	std::wstring m_strTooltip;

public:
	CDnPvPRankListItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnPvPRankListItemDlg(){}

	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	virtual void Show( bool bShow );
	
public:
	void SetInfoColosseum( struct TPvPRanking * pInfo );
	void SetInfoLadder( struct TPvPLadderRanking2 * pInfo );
	void SetInfoUserColosseum( struct TPvPRankingDetail * pInfo );	
	void SetInfoUserLadder( struct TPvPLadderRankingDetail * pInfo );
	
private:
	void SetUserRank();

	// job�� ���� ������ ��ȯ.
	int GetParentJob( int job );
};

#endif // PRE_ADD_PVP_RANKING