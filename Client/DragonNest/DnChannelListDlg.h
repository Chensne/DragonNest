#pragma once
#include "EtUIDialog.h"
#include "DnChannelListXML.h"

const float CHANNEL_REFRESH_TIME = 60.0f;

struct sChannelInfo;

class CDnChannelListDlg : public CEtUIDialog
{
protected:
	struct SChannelInfo
	{
		int m_nChannelID;
		int m_nCongestion;
		//blondy 5.13
		int m_nChannelType;//해당 채널이 어떤 채널인지의 정보(채널인지 투기장 인지 일반 마을인지 19세채널인지 등등)
		int m_nChannelAttribute;
		std::wstring m_szServerFullName;
		
		SChannelInfo( int nChannelID, int nCongestion , int nChannelType = 0, int nChannelAttribute = 0, wchar_t * szServerFullName = NULL )
			: m_nChannelID(nChannelID)
			, m_nCongestion(nCongestion)
			, m_nChannelType(nChannelType)
			, m_nChannelAttribute(nChannelAttribute)
		{
			if( szServerFullName )
				m_szServerFullName = std::wstring( szServerFullName );
		}
		//blondy end 
	};

	enum
	{
		NO_CONGESTION,
		LOW_CONGESTION,
		MID_CONGESTION,	
		HIGH_CONGESTION,
	};

public:
	CDnChannelListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnChannelListDlg(void);

protected:
	CEtUIListBox *m_pChannelList;
	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;

	CEtUIRadioButton * m_pRadioVillage;
	CEtUIRadioButton * m_pRadioArena;
	CEtUIRadioButton * m_pRadioDarklair;
	CEtUIRadioButton * m_pRadioFarm;

	float m_fElapsedTime;
	eChannelAttribute m_eChannelAttribute;
	std::vector<SChannelInfo *>	m_vChannelInfo;

protected:
	virtual void SelectChannel();
	void RemoveList();
	void ResetList();

public:
	void AddChannelList( sChannelInfo *pChannelInfo );
	void SetDefaultList();

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};
