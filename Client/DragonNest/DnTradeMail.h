#pragma once
#include "DNPacket.h"

class CDnMailDlg;

#define MAIL_ATTACH_SINGLE_SLOT_INDEX 2

class CDnTradeMail
{
public:
	CDnTradeMail(void);
	virtual ~CDnTradeMail(void);

public:
	struct SSendAttachItemInfo
	{
		int			index;
		ITEMCLSID	itemClassID;
		int			count;
		INT64		biSerial;

		SSendAttachItemInfo()
			: index(0)
			, itemClassID(ITEMCLSID_NONE)
			, count(0)
			, biSerial(0)
		{}
	};

	struct SSendMailInfo
	{
		std::wstring	strReceiver;
		std::wstring	strTitle;
		std::wstring	strText;
		INT64			money;
		TAX_TYPE		tax;
		std::vector<SSendAttachItemInfo>	attachItems;
		MailType::Delivery					isPremium;

		SSendMailInfo()
			: money(0)
			, tax(0)
			, isPremium(MailType::Premium)
		{
			int i = 0;
			SSendAttachItemInfo info;
			for (; i < MAILATTACHITEMMAX; ++i)
			{
				info.index = info.count = 0;
				info.itemClassID = ITEMCLSID_NONE;
				attachItems.push_back(info);
				info.biSerial = 0;
			}
		}
	};

	struct SReadMailInfo
	{
		int					nMailID;
		std::wstring		strSender;
		std::wstring		strTitle;
		//std::wstring		strDate;
		tm					date;
		std::wstring		strText;
		INT64				money;
		bool				bPremium;
		std::vector<TItemInfo>	attachItems;
		MailType::Kind		mailKind;
		bool				bNewFlag;
#ifdef PRE_ADD_CADGE_CASH
		bool				bCadgeComplete;
#endif // PRE_ADD_CADGE_CASH

		SReadMailInfo()
			: money(0),
			nMailID(-1),
			bPremium(false)
			, mailKind(MailType::NormalMail)
			,bNewFlag(false)
#ifdef PRE_ADD_CADGE_CASH
			, bCadgeComplete( false )
#endif // PRE_ADD_CADGE_CASH
		{
			int i = 0;
			TItemInfo info;

			for (; i < MAILATTACHITEMMAX; ++i)
			{
				ZeroMemory(&info, sizeof(info));
				attachItems.push_back(info);
			}
			memset(&date, 0, sizeof(date));
		}

		void Clear()
		{
			nMailID = -1;
			strSender.clear();
			strTitle.clear();
			memset(&date, 0, sizeof(tm));
			strText.clear();
			money = 0;
			bPremium = false;
			mailKind = MailType::NormalMail;
			bNewFlag = false;
#ifdef PRE_ADD_CADGE_CASH
			bCadgeComplete = false;
#endif // PRE_ADD_CADGE_CASH
			ClearAttachItems();
		}

		void ClearAttachItems()
		{
			std::vector<TItemInfo>::iterator iter = attachItems.begin();
			for (; iter != attachItems.end(); ++iter)
			{
				TItemInfo& info = *iter;
				ZeroMemory(&info, sizeof(info));
			}
		}
	};

	struct SMailBoxInfo
	{
		int						nMailID;
		int						nIndex;
		int						nRemainDay;
		MailType::ReadState		bRead;
		bool					bItem;
		MailType::Delivery		deliveryType;
		std::wstring			strName;
		std::wstring			strTitle;
		BYTE					remain;
		MailType::Kind			mailKind;
		bool					bNewFlag;	// 제목, 내용 코드화 여부

		SMailBoxInfo()
			: nMailID(0)
			, nIndex(0)
			, nRemainDay(0)
			, bRead(MailType::NoRead)
			, bItem(false)
			, deliveryType(MailType::Premium)
			, mailKind(MailType::NormalMail)
			, remain(0)
			,bNewFlag(false)
		{
		}

		void Assign( const TMailBoxInfo &mailBoxInfo )
		{
			nMailID		= mailBoxInfo.nMailDBID;
			nIndex		= mailBoxInfo.wIndex;
			nRemainDay	= mailBoxInfo.cRemainDay;
			bRead		= (MailType::ReadState)mailBoxInfo.cReadType;
			bItem		= mailBoxInfo.cAttachType ? true : false;
			deliveryType = (MailType::Delivery)mailBoxInfo.cDeliveryType;
			strName		= mailBoxInfo.wszFromCharacterName;
			strTitle	= mailBoxInfo.wszTitle;
			remain		= mailBoxInfo.cRemainDay;
			mailKind	=(MailType::Kind) mailBoxInfo.cMailType;
			bNewFlag   = mailBoxInfo.bNewFlag;
		}

		bool IsAttached() const
		{
			return bItem;
		}
	};

protected:
	std::vector<SMailBoxInfo> m_vecMailBoxInfoList;
	int m_nTotalMailCount;			// Note : 총 우편수
	int m_nUnreadMailCount;			// Note : 읽지 않은 메일
	int m_nItemMailCount;			// Note : 아이템 보유 메일

	CDnMailDlg*		m_pMailDialog;
	SReadMailInfo	m_CurReadMailInfoCache;
	INT64			m_RecvAttachItemSerialCache;
	bool m_bDelMailSoReqLastPage;
	int m_nDaySendEnableCount;
	int m_nRecvAttachGoldSoundIdx;

public:
	bool Initialize();
	void Finalize();

	void SetMailDialog( CDnMailDlg *pDialog)	{ m_pMailDialog = pDialog; }
	bool IsOnMailMode() const;
	bool IsLockedDlgs();
	void LockMailDlg(bool bLock);

	int GetTotalMailCount()		{ return (int)m_vecMailBoxInfoList.size(); }
	int GetMaxPage() const;
	int GetCurrentPage() const;
	int GetUnreadMailCount()	{ return m_nUnreadMailCount; }
	int GetItemMailCount()		{ return m_nItemMailCount; }
	int GetMailBoxIndex(int nMailID) const;
	int	GetCurReadMailID() const;

	void SetDaySendEnableCount(int count);
	int GetDaySendEnableCount() const { return m_nDaySendEnableCount; }

	//void SetCheckMail( int nMailID, bool bCheck );

	std::vector<SMailBoxInfo>&	GetMailBoxInfoList() { return m_vecMailBoxInfoList; }
	void						ClearMailBoxInfoList() { return m_vecMailBoxInfoList.clear(); }
	SMailBoxInfo*				FindMailBoxInfo( int nMailID );
	void						DeleteMailBoxInfo( int nMailID );
	void						ClearCurReadMailInfoCache();

public:
	void RequestMailBox( int nPageNum );
	void RequestMailSend( const SSendMailInfo &sendMailInfo );
	void RequestMailRead( int nMailDBID );
	void RequestMailDelete(int* mailDBIDList, int delCount);
	void RequestMailAttachAll(int* mailDBIDList);
	void RequestMailAttach(int mailDBID, INT64 attachItemSerial = -1, int attachSlotIdx = -1);

public:
	void OnRecvMailBox( SCMailBox *pPacket );
	void OnRecvMailSend( SCSendMail *pPacket );
	void OnRecvMailRead( SCReadMail *pPacket );
#ifdef PRE_ADD_CADGE_CASH
	void OnRecvCadgeMailRead( SCReadCadgeMail* pPacket );
#endif // PRE_ADD_CADGE_CASH
	void OnRecvMailDelete( SCMailResult *pPacket );
	void OnRecvMailAttachAll( SCAttachAllMailResult *pPacket );
	void OnRecvMailAttach(SCMailResult *pPacket);
	void OnRecvMailNotify(SCNotifyMail* pPacket);
};
