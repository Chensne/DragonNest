#pragma once
#include "Task.h"
#include "MessageListener.h"
#include "DNProtocol.h"
#include "DNPacket.h"

class CDnFriendDlg;

class CDnFriendTask : public CTask, public CTaskListener, public CSingleton<CDnFriendTask>
{
public:
	//	DNServerDef.h 소스 참고
	enum eLocationState
	{
		_LOCATION_NONE = 0,		//옵후 라인 입니다
		_LOCATION_MOVE,			//서버 이동중인 상태도 체킹해야 할까? 해서 일단 맹글어 봅뉘다
		_LOCATION_LOGIN,
		_LOCATION_VILLAGE,
		_LOCATION_GAME,
	};

	enum eResultFlag
	{
		eNONE,
		eSUCCESS,
		eFAIL
	};

	struct SFriendGroupInfo
	{
		UINT nGroupDBID;
		std::wstring wszGroupName;
	};

	struct SFriendInfo
	{
		bool bHaveDetailInfo;
		INT64 biFriendCharDBID;			//CharacterDBID
		UINT nGroupDBID;
		TCommunityLocation Location;	//일단 넣어 놓습니다. 바꾸실꺼면 바꾸어 주세요.
		std::wstring wszFriendName;

		//follow detail Info(이하 정보는 디테일 정보입니다. 온라인 사용자의 경우 요청하면 하나씩 보내줍니다. 리스트와는 별개)
		int nClass;
		int nLevel;
		int nJob;
		bool bIsParty;
		std::wstring wszGuild;

		SFriendInfo() : bHaveDetailInfo( false ), biFriendCharDBID( 0 ), nGroupDBID( 0 ), 
						nClass( 0 ), nLevel( 0 ), nJob( 0 ), bIsParty( false )
		{
		}

		void Set(const TFriendInfo& info)
		{
			nGroupDBID			= info.nGroupDBID;
			biFriendCharDBID	= info.biFriendCharacterDBID;
			Location			= info.Location;
			wszFriendName		= info.wszFriendName;
		}
	};

	struct SFriendRequestInfo : public SFriendInfo
	{
		float fAcceptTime;

		SFriendRequestInfo();
	};

	struct SFriendGroupCountInfo 
	{
		int logonFriends;
		int wholeFriends;

		SFriendGroupCountInfo() : logonFriends(0), wholeFriends(0) {}
	};

	CDnFriendTask(void);
	virtual ~CDnFriendTask(void);

	// Note : Key is FriendCharDBID
	//
	typedef std::map<INT64,SFriendInfo>		FRIENDINFO_MAP;
	typedef FRIENDINFO_MAP::iterator		FRIENDINFO_MAP_ITER;

	// Note : Key is GroupDBID
	//
	typedef std::map<int,FRIENDINFO_MAP*>	FRIEND_MAP;
	typedef FRIEND_MAP::iterator			FRIEND_MAP_ITER;

	typedef std::vector<SFriendGroupInfo>	FRIENDGROUPINFO_VEC;
	typedef FRIENDGROUPINFO_VEC::iterator	FRIENDGROUPINFO_VEC_ITER;

	typedef std::vector<SFriendRequestInfo>	FRIEND_REQ_LIST;

protected:
	FRIEND_MAP			m_mapFriend;
	FRIENDGROUPINFO_VEC m_vGroupInfo;
	CDnFriendDlg		*m_pFriendDialog;
	FRIEND_REQ_LIST		m_FriendRequestList;

public:
	bool Initialize();
	void SetFriendDialog( CDnFriendDlg *pDialog )			{ m_pFriendDialog = pDialog; }

	SFriendGroupCountInfo	GetGroupCountInfo(UINT nGroupDBID) const;
	const FRIEND_MAP&		GetFriendList() const			{ return m_mapFriend; }

	FRIENDGROUPINFO_VEC& GetFriendGroupInfoList() { return m_vGroupInfo; }
	FRIENDINFO_MAP* GetFriendInfoList( int nGroupID );
	SFriendInfo* GetFriendInfo( INT64 biCharDBID );
	SFriendInfo* GetFriendInfo( const wchar_t* name );

	int GetFriendCount();
	bool IsFriend(const WCHAR* name);
	void GetMapName(std::wstring& result, int mapIdx); // todo : extract by kalliste

	void RequestFriendGroupAdd( const wchar_t *wszGroupName );
	void RequestFriendGroupDel( UINT nGroupDBID );
	void RequestFriendGroupUpdate( UINT nGroupDBID, const wchar_t *wszGroupName );
	void RequestFriendAdd( UINT nGroupDBID, const WCHAR *pCharacterName );
	void RequestFriendDelete( INT64 nFriendDBID );
	void RequestFriendDetailInfo( INT64 nFriendDBID );
	void RequestFriendUpdate( INT64 biCharDBID, int nGroupID );
	void RequestFriendListUpdate();
	void RequestFriendLocationList();

	virtual void OnDisconnectTcp( bool bValidDisconnect );
	virtual void OnDisconnectUdp( bool bValidDisconnect );

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	void OnRecvFriendGroupList(char * pData, int nSize);
	void OnRecvFriendList(char * pData, int nSize);
	void OnRecvFriendLocationList(char * pData, int nSize);
	void OnRecvFriendGroupAdded(char * pData, int nSize);
	void OnRecvFriendAdded(char * pData, int nSize);
	void OnRecvFriendGroupDeleted(char * pData, int nSize);
	void OnRecvFriendDeleted(char * pData, int nSize);
	void OnRecvFriendGroupUpdated(char * pData, int nSize);
	void OnRecvFriendUpdated(char * pData, int nSize);
	void OnRecvFriendDetailInfo(char * pData, int nSize);
	void OnRecvFriendResultMessage(char * pData, int nLen);
	void OnRecvFriendAddNotice(char * pData, int nLen);

protected:
	void AddFriend( SFriendInfo &friendInfo );
	void DelFriend( INT64 biCharDBID );
	void ClearFriends();
	void UpdateFriend( const SCFriendUpdated *pFriendUpdated );

	void AddGroup( int nGroupID, const wchar_t *wszGroupName );
	void DelGroup( int nGroupID );	
	void RenameGroup( int nGroupID, const wchar_t *wszGroupName );
	int	 GetGroupCount() { return (int)m_vGroupInfo.size(); }

	bool IsDupGroupID( UINT nGroupDBID );
	bool IsDupGroupName( const wchar_t *szGroupName );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnRecvFriendMessage( int nSubCmd, char *pData, int nSize );
};

#define GetFriendTask()	CDnFriendTask::GetInstance()