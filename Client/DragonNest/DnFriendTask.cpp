#include "StdAfx.h"
#include "DnFriendTask.h"
#include "FriendSendPacket.h"
#include "DnInterface.h"
#include "DnFriendDlg.h"
#include "DnMainMenuDlg.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__) 
#endif 

CDnFriendTask::SFriendRequestInfo::SFriendRequestInfo() : fAcceptTime(_INVALID_TIME_VALUE) {}

CDnFriendTask::CDnFriendTask(void)
	: m_pFriendDialog(NULL), CTaskListener(true)
{
}

CDnFriendTask::~CDnFriendTask(void)
{
	FRIEND_MAP_ITER iter = m_mapFriend.begin();
	for( ; iter != m_mapFriend.end(); ++iter )
	{
		SAFE_DELETE(iter->second);
	}
	m_mapFriend.clear();
	m_pFriendDialog = NULL;
}

bool CDnFriendTask::Initialize()
{
	m_vGroupInfo.reserve(FRIEND_GROUP_MAX);

	// Note : 그룹없음 생성
	//
	FRIENDINFO_MAP *pFriendInfo = new FRIENDINFO_MAP;
	m_mapFriend.insert( make_pair( 0, pFriendInfo ) );

	return true;
}

void CDnFriendTask::AddFriend( SFriendInfo &friendInfo )
{
	FRIEND_MAP_ITER iter = m_mapFriend.find( friendInfo.nGroupDBID );
	if( iter != m_mapFriend.end() )
	{
		FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( !pFriendInfo )
		{
			CDebugSet::ToLogFile( "CDnFriendTask::AddFriend, pFriendInfo is NULL!" );
			return;
		}

		if( !pFriendInfo->insert( make_pair( friendInfo.biFriendCharDBID, friendInfo ) ).second )
		{
			// Note : 동일한 키가 존재하여 추가 실패
			//
			CDebugSet::ToLogFile( "CDnFriendTask::AddFriend, Duplication Friend ID(%I64d)!", friendInfo.biFriendCharDBID );
		}

		return;
	}
	
	// Note : 잘못된 그룹 ID
	//
	CDebugSet::ToLogFile( "CDnFriendTask::AddFriend, Invalid Group ID(%d)!", friendInfo.nGroupDBID );
	return;
}

void CDnFriendTask::DelFriend( INT64 biCharDBID )
{
	FRIENDINFO_MAP_ITER _iter;
	FRIEND_MAP_ITER iter = m_mapFriend.begin();

	for( ; iter != m_mapFriend.end(); ++iter )
	{
		FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( !pFriendInfo )
			continue;

		_iter = pFriendInfo->find( biCharDBID );
		if( _iter != pFriendInfo->end() )
		{
			pFriendInfo->erase( _iter );
			return;
		}
	}

	CDebugSet::ToLogFile( "CDnFriendTask::DelFriend, (%I64d) Failed!", biCharDBID );
}

void CDnFriendTask::UpdateFriend( const SCFriendUpdated *pFriendUpdated )
{
	_ASSERT(pFriendUpdated&&"CDnFriendTask::UpdateFriend, pFriendUpdated is NULL!");

	SFriendInfo *pFriendInfo = GetFriendInfo( pFriendUpdated->biFriendCharacterDBID[0] );
	if( !pFriendInfo ) return;

	SFriendInfo friendInfo;
	friendInfo = (*pFriendInfo);

	if( pFriendInfo->nGroupDBID != pFriendUpdated->nGroupDBID )
	{
		friendInfo.nGroupDBID = pFriendUpdated->nGroupDBID;

		INT64 biFriendCharDBID = pFriendInfo->biFriendCharDBID;
		DelFriend( pFriendInfo->biFriendCharDBID );
		AddFriend( friendInfo );

		m_pFriendDialog->DeleteFriend( biFriendCharDBID );
		m_pFriendDialog->AddFriend( friendInfo.nGroupDBID, friendInfo.biFriendCharDBID, (eLocationState)friendInfo.Location.cServerLocation, friendInfo.wszFriendName.c_str() );
	}
}

CDnFriendTask::SFriendInfo* CDnFriendTask::GetFriendInfo( INT64 biCharDBID )
{
	FRIENDINFO_MAP_ITER _iter;
	FRIEND_MAP_ITER iter = m_mapFriend.begin();

	for( ; iter != m_mapFriend.end(); ++iter )
	{
		FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( !pFriendInfo )
			continue;

		_iter = pFriendInfo->find( biCharDBID );
		if( _iter != pFriendInfo->end() )
		{
			return &(_iter->second);
		}
	}

	return NULL;
}

CDnFriendTask::SFriendInfo* CDnFriendTask::GetFriendInfo( const wchar_t* name )
{
	FRIEND_MAP_ITER iter = m_mapFriend.begin();
	for( ; iter != m_mapFriend.end(); ++iter )
	{
		FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( pFriendInfo == NULL)
			continue;

		FRIENDINFO_MAP_ITER fmIter = pFriendInfo->begin();
		for (; fmIter != pFriendInfo->end(); ++fmIter)
		{
			SFriendInfo& info = fmIter->second;
			if (!__wcsicmp_l(name, info.wszFriendName.c_str()))
				return &info;
		}
	}

	return NULL;
}

void CDnFriendTask::OnDisconnectTcp( bool bValidDisconnect )
{
}

void CDnFriendTask::OnDisconnectUdp( bool bValidDisconnect )
{
}

void CDnFriendTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	bool bProcessDispatch = false;
	switch (nMainCmd)
	{
		case SC_FRIEND:	OnRecvFriendMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
	}
	if( bProcessDispatch ) {
		WriteLog( 1, ", Info, CDnFriendTask::OnDispatchMessage End : nMainCmd(%d), nSubCmd(%d), nSize(%d)\n", nMainCmd, nSubCmd, nSize );
	}
}

void CDnFriendTask::OnRecvFriendMessage( int nSubCmd, char *pData, int nSize )
{
	switch (nSubCmd)
	{
	case eFriend::SC_FRIEND_GROUP_LIST:		OnRecvFriendGroupList(pData, nSize);	break;
	case eFriend::SC_FRIEND_GROUP_ADDED:	OnRecvFriendGroupAdded(pData, nSize);	break;
	case eFriend::SC_FRIEND_LIST: OnRecvFriendList(pData, nSize); break;
	case eFriend::SC_FRIEND_LOCATIONLIST: OnRecvFriendLocationList(pData, nSize); break;
	case eFriend::SC_FRIEND_ADDED:			OnRecvFriendAdded(pData, nSize);		break;
	case eFriend::SC_FRIEND_GROUP_DELETED:	OnRecvFriendGroupDeleted(pData, nSize);	break;
	case eFriend::SC_FRIEND_ADDED_DELETED:	OnRecvFriendDeleted(pData, nSize);		break;
	case eFriend::SC_FRIEND_GROUP_UPDATED:	OnRecvFriendGroupUpdated(pData, nSize);	break;
	case eFriend::SC_FRIEND_UPDATED:		OnRecvFriendUpdated(pData, nSize);		break;
	case eFriend::SC_FRIEND_INFO:			OnRecvFriendDetailInfo(pData, nSize);	break;
	case eFriend::SC_FRIEND_RESULT:			OnRecvFriendResultMessage(pData, nSize); break;
	case eFriend::SC_FRIEND_ADDNOTICE:		OnRecvFriendAddNotice(pData, nSize); break;
	default:
		CDebugSet::ToLogFile( "CDnFriendTask::OnRecvFriendMessage, Invalid Message(%d)!", nSubCmd );
	}
}

void CDnFriendTask::OnRecvFriendGroupList(char * pData, int nSize)
{
	SCFriendGroupList * pPacket = (SCFriendGroupList*)pData;
	WCHAR wszName[FRIEND_GROUP_NAMELENMAX];

	m_vGroupInfo.clear();

	int nLen = 0;
	for( int i = 0; i < pPacket->cGroupCount && i < FRIEND_GROUP_MAX; i++ )
	{
		memset(wszName, 0, sizeof(wszName));
		_wcscpy(wszName, _countof(wszName), pPacket->wszBuf + nLen, pPacket->cGroupNameLen[i]);
		nLen += pPacket->cGroupNameLen[i];

		AddGroup( pPacket->nGroupDBID[i], wszName );
	}

	if( m_pFriendDialog ) m_pFriendDialog->RefreshFriendGroup();
}

void CDnFriendTask::ClearFriends()
{
	FRIEND_MAP_ITER iter = m_mapFriend.begin();
	for( ; iter != m_mapFriend.end(); ++iter )
	{
		FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( pFriendInfo == NULL)
			continue;

		pFriendInfo->clear();
	}

}

void CDnFriendTask::OnRecvFriendList(char * pData, int nSize)
{
	SCFriendList * pPacket = (SCFriendList*)pData;

#ifdef _TEST_CODE_KAL
	SCFriendList test;
	test.cCount = 16;
	test.nRetCode = ERROR_NONE;

	int i = 0;
	std::wstring str[16] =
	{
		L"다랑어",
		L"아랑어",
		L"카랑어",
		L"가랑어",
		L"네랑어",
		L"오랑어",
		L"두랑어",
		L"파랑어",
		L"하랑어",
		L"고랑어",
		L"수랑어",
		L"우랑어",
		L"미랑어",
		L"바랑어",
		L"자랑어",
		L"츄랑어"
	};
	for (; i < 16; ++i)
	{
		TCommunityLocation loc;
		test.Info[i].biFriendCharacterDBID = i;
		test.Info[i].Location = loc;
		test.Info[i].nGroupDBID = 0;
		wcscpy_s(test.Info[i].wszFriendName, str[i].c_str());
	}

	i = 0;
	int friendListCount = test.cCount;

	ClearFriends();

	for (; i < friendListCount; ++i)
	{
		TFriendInfo& info = test.Info[i];
		FRIEND_MAP_ITER iter = m_mapFriend.find( info.nGroupDBID );
		if( iter != m_mapFriend.end() )
		{
			FRIENDINFO_MAP* infoMap = iter->second;
			if (infoMap)
			{
				FRIENDINFO_MAP_ITER infoIter = infoMap->find(info.biFriendCharacterDBID);
				if (infoIter != infoMap->end())
				{
					SFriendInfo& friendInfo		= (*infoIter).second;
					friendInfo.Set(info);
				}
				else
				{
					SFriendInfo newFriendInfo;
					newFriendInfo.Set(info);
					infoMap->insert(std::make_pair(info.biFriendCharacterDBID, newFriendInfo));
				}
			}
		}
		else
		{
			_ASSERT(0);
		}
	}


	if( m_pFriendDialog ) m_pFriendDialog->RefreshFriendList();
#else

	if (pPacket)
	{
		int i = 0;
		int friendListCount = pPacket->cCount;

		ClearFriends();

		for (; i < friendListCount; ++i)
		{
			TFriendInfo& info = pPacket->Info[i];
			FRIEND_MAP_ITER iter = m_mapFriend.find( info.nGroupDBID );
			if( iter != m_mapFriend.end() )
			{
				FRIENDINFO_MAP* infoMap = iter->second;
				if (infoMap)
				{
					FRIENDINFO_MAP_ITER infoIter = infoMap->find(info.biFriendCharacterDBID);
					if (infoIter != infoMap->end())
					{
						SFriendInfo& friendInfo		= (*infoIter).second;
						friendInfo.Set(info);
					}
					else
					{
						SFriendInfo newFriendInfo;
						newFriendInfo.Set(info);
						infoMap->insert(std::make_pair(info.biFriendCharacterDBID, newFriendInfo));
					}
				}
			}
			else
			{
				_ASSERT(0);
			}
		}

		if( m_pFriendDialog ) m_pFriendDialog->RefreshFriendList();
	}
#endif
}

void CDnFriendTask::OnRecvFriendLocationList(char * pData, int nSize)
{
	SCFriendLocationList * pPacket = (SCFriendLocationList*)pData;

	if (pPacket)
	{
		int i = 0;
		for (; i < pPacket->cCount; ++i)
		{
			const TFriendLocation& loc = pPacket->FriendLocation[i];
			FRIEND_MAP_ITER iter = m_mapFriend.find(loc.nGroupDBID);
			if (iter != m_mapFriend.end())
			{
				FRIENDINFO_MAP* info = (*iter).second;
				if (info)
				{
					FRIENDINFO_MAP_ITER infoIter = info->find(loc.biFriendCharacterDBID);
					if (infoIter != info->end())
					{
						SFriendInfo& friendInfo = (*infoIter).second;
						friendInfo.Location = loc.Location;
					}
					else
						_ASSERT(0);
				}
				else
					_ASSERT(0);
			}
			else
				_ASSERT(0);
		}
		if( m_pFriendDialog ) m_pFriendDialog->RefreshFriendList();
	}
	else
		_ASSERT(0);
}

void CDnFriendTask::OnRecvFriendGroupAdded(char * pData, int nSize)
{
	if (m_vGroupInfo.size() >= FRIEND_GROUP_MAX)
	{
		_ASSERT(0&&"CDnFriendTask::OnRecvFriendGroupAdded, m_vGroupInfo.size() >= FRIEND_GROUP_MAX");
		return;		//서버야 너 잘 못 했다.
	}

	SCFriendGroupAdded * pPacket = (SCFriendGroupAdded*)pData;

	if (pPacket->nRetCode == ERROR_NONE)
	{
		WCHAR wszName[FRIEND_GROUP_NAMELENMAX];
		memset(wszName, 0, sizeof(FRIEND_GROUP_NAMELENMAX));
		_wcscpy(wszName, _countof(wszName), pPacket->wszBuf, pPacket->cNameLen);

		AddGroup( pPacket->nGroupDBID, wszName );

		if( m_pFriendDialog ) {
			m_pFriendDialog->AddFriendGroup( pPacket->nGroupDBID, wszName );
			m_pFriendDialog->OnAddFriendGroup();
		}
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRetCode);
}

void CDnFriendTask::OnRecvFriendAdded(char * pData, int nSize)
{
	SCFriendAdded * pPacket = (SCFriendAdded*)pData;	

	if (pPacket->nRetCode == ERROR_NONE)
	{
		SFriendRequestInfo Info;
		Info.fAcceptTime = _REQUEST_ACCEPT_TOTALTIME;

		Info.nGroupDBID = pPacket->Info.nGroupDBID;
		Info.biFriendCharDBID = pPacket->Info.biFriendCharacterDBID;
		Info.Location = pPacket->Info.Location;
		Info.wszFriendName = pPacket->Info.wszFriendName;

		//이하 detail 요청시 보내줌
		Info.nClass = 0;
		Info.nLevel = 0;
		Info.bIsParty = false;

		AddFriend( Info );
		if( m_pFriendDialog )
			m_pFriendDialog->AddFriend( pPacket->Info.nGroupDBID, pPacket->Info.biFriendCharacterDBID, (eLocationState)pPacket->Info.Location.cServerLocation, pPacket->Info.wszFriendName );

		wchar_t wszAdded[255];
		swprintf_s(wszAdded, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1419 ), pPacket->Info.wszFriendName );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszAdded, false );
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRetCode);
}

void CDnFriendTask::OnRecvFriendGroupDeleted(char * pData, int nSize)
{
	SCFriendGroupDeleted * pPacket = (SCFriendGroupDeleted*)pData;
	if (pPacket->nRetCode == ERROR_NONE)
	{
		DelGroup( pPacket->nGroupDBID );
		if( m_pFriendDialog ) m_pFriendDialog->DeleteFriendGroup( pPacket->nGroupDBID );
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRetCode);
}

void CDnFriendTask::OnRecvFriendDeleted(char * pData, int nSize)
{
	SCFriendDeleted * pPacket = (SCFriendDeleted*)pData;
	if (pPacket->nRetCode == ERROR_NONE)
	{
		DelFriend( pPacket->biFriendCharacterDBID[0] );
		if( m_pFriendDialog ) m_pFriendDialog->OnRecvDeleteFriend();
	}
	else
	{
		GetInterface().ServerMessageBox(pPacket->nRetCode);
	}
}

void CDnFriendTask::OnRecvFriendGroupUpdated(char * pData, int nSize)
{
	SCFriendGroupUpdated * pPacket = (SCFriendGroupUpdated*)pData;

	if (pPacket->nRetCode == ERROR_NONE)
	{
		WCHAR wszName[FRIEND_GROUP_NAMELENMAX]={0};
		_wcscpy(wszName, _countof(wszName), pPacket->wszBuf, pPacket->cNameLen);

		RenameGroup( pPacket->nGroupDBID, wszName );
		if( m_pFriendDialog ) m_pFriendDialog->RenameFriendGroup( pPacket->nGroupDBID, wszName );
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRetCode);
}

void CDnFriendTask::OnRecvFriendUpdated(char * pData, int nSize)
{
	SCFriendUpdated * pPacket = (SCFriendUpdated*)pData;
	if (pPacket->nRetCode == ERROR_NONE)
		UpdateFriend( pPacket );
	else
		GetInterface().ServerMessageBox(pPacket->nRetCode);

	// Note : 친구 정보가 변경되었다. 친구 정보를 반영.
	//
}

void CDnFriendTask::OnRecvFriendDetailInfo(char * pData, int nSize)
{
	SCFriendDetailInfo * pPacket = (SCFriendDetailInfo*)pData;

	SFriendInfo *pFriendInfo = GetFriendInfo( pPacket->biFriendCharacterDBID );
	if( !pFriendInfo )
	{
		CDebugSet::ToLogFile( "CDnFriendTask::OnRecvFriendDetailInfo, pFriendInfo is NULL!" );
		return;
	}

	pFriendInfo->nClass = pPacket->nClass;
	pFriendInfo->nLevel = pPacket->cCharacterLevel;
	pFriendInfo->nJob = pPacket->nJob;
	pFriendInfo->Location = pPacket->Location;

	pFriendInfo->bHaveDetailInfo = true;

	if( m_pFriendDialog ) m_pFriendDialog->RefreshFriendInfo( pPacket->biFriendCharacterDBID );
}

void CDnFriendTask::OnRecvFriendResultMessage(char * pData, int nLen)
{
	SCFriendResult * pPacket = (SCFriendResult*)pData;
	GetInterface().ServerMessageBox(pPacket->nRet);
}

void CDnFriendTask::OnRecvFriendAddNotice(char * pData, int nLen)
{
	SCFriendAddNotice * pPacket = (SCFriendAddNotice*)pData;

	if (pPacket && pPacket->wszName && pPacket->wszName[0] != '\0')
	{
		std::wstring str;
		str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4312), pPacket->wszName); // UISTRING : '%s'님이 친구 등록하였습니다.
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), str.c_str() );
	}
	else
	{
		_ASSERT(0);
	}
}

void CDnFriendTask::RequestFriendAdd( UINT nGroupDBID, const WCHAR *pCharacterName )
{
	SendAddFriend( nGroupDBID, pCharacterName );
}

void CDnFriendTask::RequestFriendDelete( INT64 nFriendDBID )
{
	SendDeleteFriend( nFriendDBID );
}

void CDnFriendTask::RequestFriendGroupAdd( const wchar_t *wszGroupName )
{
	if( GetGroupCount() > FRIEND_GROUP_MAX )
	{
		// Note : 그룹 최대 개수를 초과
		//
		return;
	}

	std::wstring strGroupName( wszGroupName );
	if( strGroupName == GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1346/*그룹없음*/ ) )
	{
		// Note : '그룹없음' 중복
		//
		return;
	}

	if( IsDupGroupName( wszGroupName ) )
	{
		// Note : 그룹 이름 중복
		//
		return;
	}

	SendAddGroup( wszGroupName );
}

void CDnFriendTask::RequestFriendGroupDel( UINT nGroupDBID )
{
	if( nGroupDBID == 0 )
	{
		// Note : 그룹없음은 삭제 할 수 없음.
		//
		return;
	}

	if( !IsDupGroupID( nGroupDBID ) )
	{
		// Note : 그룹 아이디 존재하지 않음
		//
		return;
	}

	SendDelGroup( nGroupDBID );
}

void CDnFriendTask::RequestFriendDetailInfo( INT64 nFriendDBID )
{
	SendReqDetailInfo( nFriendDBID );
}

bool CDnFriendTask::IsDupGroupID( UINT nGroupDBID )
{
	for( int i=0; i<(int)m_vGroupInfo.size(); i++ )
	{
		if( m_vGroupInfo[i].nGroupDBID == nGroupDBID )
			return true;
	}

	return false;
}

bool CDnFriendTask::IsDupGroupName( const wchar_t *szGroupName )
{
	for( int i=0; i<(int)m_vGroupInfo.size(); i++ )
	{
		if( m_vGroupInfo[i].wszGroupName == szGroupName )
			return true;
	}

	return false;
}

void CDnFriendTask::RequestFriendGroupUpdate( UINT nGroupDBID, const wchar_t *wszGroupName )
{
	if( nGroupDBID == 0 )
	{
		// Note : 그룹없음은 업데이트 불가.
		//
		return;
	}

	SendGroupUpdate( nGroupDBID, wszGroupName );
}

void CDnFriendTask::RequestFriendUpdate( INT64 biCharDBID, int nGroupID )
{
	_ASSERT( biCharDBID > 0 );
	_ASSERT( nGroupID >= 0 );

	SendFriendUpdate( biCharDBID, nGroupID );
}

void CDnFriendTask::RequestFriendListUpdate()
{
	SendReqFriendList();
}

void CDnFriendTask::RequestFriendLocationList()
{
	SendReqFriendLocationList();
}

int CDnFriendTask::GetFriendCount()
{
	int nFriendCount(0);

	FRIEND_MAP_ITER iter = m_mapFriend.begin();
	for( ; iter != m_mapFriend.end(); ++iter )
	{
		FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( !pFriendInfo )
			continue;

		nFriendCount += (int)pFriendInfo->size();
	}

	return nFriendCount;
}

CDnFriendTask::FRIENDINFO_MAP* CDnFriendTask::GetFriendInfoList( int nGroupID )
{
	FRIEND_MAP_ITER iter = m_mapFriend.find( nGroupID );
	if( iter != m_mapFriend.end() )
	{
		return iter->second;
	}

	return NULL;
}

void CDnFriendTask::AddGroup( int nGroupID, const wchar_t *wszGroupName )
{
	if( nGroupID == 0 )
	{
		OutputDebug( "CDnFriendTask::AddGroup, nGroupId is 0!\n" );
		return;
	}

	SFriendGroupInfo groupInfo;
	groupInfo.nGroupDBID = nGroupID;
	groupInfo.wszGroupName = wszGroupName;
	m_vGroupInfo.push_back( groupInfo );

	FRIENDINFO_MAP *pFriendInfo = new FRIENDINFO_MAP;
	m_mapFriend.insert( make_pair( nGroupID, pFriendInfo ) );
}

void CDnFriendTask::DelGroup( int nGroupID )
{
	for( int i=0; i<(int)m_vGroupInfo.size(); i++ )
	{
		if( m_vGroupInfo[i].nGroupDBID == nGroupID )
		{
			m_vGroupInfo.erase( m_vGroupInfo.begin()+i );
			break;
		}
	}

	FRIEND_MAP_ITER iter = m_mapFriend.find( nGroupID );
	if( iter != m_mapFriend.end() )
	{
		// Note : 그룹에 속한 친구를 그룹없음으로 옮긴다.
		//		서버쪽에서도 그룹 삭제시 이같은 작업을 수행해야 한다.
		//		서버랑 클라이언트의 동기화를 위해서...
		//
		FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( !pFriendInfo ) return;

		FRIENDINFO_MAP_ITER _iter = pFriendInfo->begin();

		for( ; _iter != pFriendInfo->end(); ++_iter )
		{
			SFriendInfo &friendInfo = _iter->second;
			friendInfo.nGroupDBID = 0;

			AddFriend( friendInfo );
			if( m_pFriendDialog )
				m_pFriendDialog->AddFriend( friendInfo.nGroupDBID, friendInfo.biFriendCharDBID, (eLocationState)friendInfo.Location.cServerLocation, friendInfo.wszFriendName.c_str() );
		}

		// Note : 그룹을 삭제한다.
		//
		m_mapFriend.erase( iter );
	}
}

void CDnFriendTask::RenameGroup( int nGroupID, const wchar_t *wszGroupName )
{
	for( int i=0; i<(int)m_vGroupInfo.size(); i++ )
	{
		if( m_vGroupInfo[i].nGroupDBID == nGroupID )
		{
			m_vGroupInfo[i].wszGroupName = wszGroupName;
			break;
		}
	}
}

bool CDnFriendTask::IsFriend(const WCHAR* name)
{
	FRIEND_MAP_ITER iter = m_mapFriend.begin();
	for( ; iter != m_mapFriend.end(); ++iter )
	{
		FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( pFriendInfo == NULL)
			continue;

		FRIENDINFO_MAP_ITER fmIter = pFriendInfo->begin();
		for (; fmIter != pFriendInfo->end(); ++fmIter)
		{
			SFriendInfo& info = fmIter->second;
			if (!__wcsicmp_l(name, info.wszFriendName.c_str()))
				return true;
		}
	}

	return false;
}

void CDnFriendTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
}

CDnFriendTask::SFriendGroupCountInfo CDnFriendTask::GetGroupCountInfo(UINT nGroupDBID) const
{
	SFriendGroupCountInfo grpinfo;
	FRIEND_MAP::const_iterator iter = m_mapFriend.find(nGroupDBID);
	if (iter != m_mapFriend.end())
	{
		FRIENDINFO_MAP* friendMap = (*iter).second;
		if (friendMap)
		{
			grpinfo.wholeFriends = (int)friendMap->size();
			FRIENDINFO_MAP_ITER frIter = friendMap->begin();
			for (; frIter != friendMap->end(); ++frIter)
			{
				SFriendInfo& info = (*frIter).second;
				if (info.Location.cServerLocation != CTreeItem::eNONE)
					grpinfo.logonFriends++;
			}
		}
	}

	return grpinfo;
}


void CDnFriendTask::GetMapName(std::wstring& result, int mapIdx)
{
	result = _T("");
	if (mapIdx > 0)
	{
		DNTableFileFormat*  pSox = GetDNTable(CDnTableDB::TMAP);
		int nStringTableID = pSox->GetFieldFromLablePtr( mapIdx, "_MapNameID" )->GetInteger();
		result = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID );
	}
	else
	{
		_ASSERT(0);
	}
}