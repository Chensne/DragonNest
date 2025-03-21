#pragma once

// Note : 부모를 가지기 않는 최상위 윈도우의 글로벌 ID
//
enum DIALOG_ID
{
	NONE_DIALOG = 0,
	REBIRTH_DIALOG,
	GATEQUESTION_DIALOG,
	NPC_DIALOG,
	FADE_DIALOG,
	ACCEPT_DIALOG,
	PRIVATE_MARKET_REQ_DIALOG,
	DUNGEON_ENTER_DIALOG,
	DUNGEON_MOVE_DIALOG,
	DUNGEON_CLEAR_MOVE_DIALOG,
	STAGEGIVEUP_MOVE_DIALOG,
	ACCEPT_REQUEST_DIALOG,
	DAKRLAIR_ENTER_DIALOG,
	PARTY_JOIN_FAIL_DIALOG,
	ACCEPT_REQUEST_PARTYASK_DIALOG,
	UI_DISABLE_DIALOG,
	GATE_MAP_INDEX_SELECT_DIALOG,
	MAINQUEST_DIALOG,
	
	// Note : 플레이 정보 다이얼로그 아이디는 
	//		기본 아이디에 캐릭터의 인덱스에 따라 증가하게 됩니다.
	//
	PLAYERINFO_DIALOG = 1000,
	PLAYERDELETEWAIT_DIALOG = 1100,

	// Note : MessageBox 텍스트와 아이디는 일치해야 합니다.
	//		텍스트는 UIString.xls 파일을 참고합니다.
	//
	MESSAGEBOX_1 = 100001,	// 아이디를 입력하세요.
	MESSAGEBOX_2,			// 비밀번호를 입력하세요.
	MESSAGEBOX_3,			// 등록되지 않은 아이디입니다.
	MESSAGEBOX_4,			// 비밀번호가 일치하지 않습니다.
	MESSAGEBOX_5 = 100005,	// 캐릭터 리스트를 받아올 수 없습니다.
	MESSAGEBOX_6,			// 삭제한 캐릭터는 다시 살릴 수 없습니다.\n삭제하시겠습니까?
	MESSAGEBOX_7,			// 캐릭터 생성을 실패하였습니다.
	MESSAGEBOX_8,			// 마을로 들어갈 수 없습니다.
	MESSAGEBOX_9,			// 로그인 서버에 접속할 수 없습니다.
	MESSAGEBOX_10,			// 서버와 접속이 끊어졌습니다.
	MESSAGEBOX_11,			// 캐릭터를 선택하세요.
	MESSAGEBOX_12,			// 캐릭터 삭제가 실패하였습니다.
	MESSAGEBOX_13,			// 이름을 입력하세요
	MESSAGEBOX_14,			// 중복되는 이름이 있습니다.
	MESSAGEBOX_15,			// 파티이름을 입력하세요.
	MESSAGEBOX_16,			// 파티 생성이 실패하였습니다.
	MESSAGEBOX_17,			// 파티에 참여할 수 없습니다.
	MESSAGEBOX_18,			// 파티에서 나올 수 없습니다.
	MESSAGEBOX_19,			// 파티 뽀개기가 실패하였습니다.
	MESSAGEBOX_20,			// 강퇴당함
	MESSAGEBOX_21,			// 게임을 종료하시겠습니까?
	MESSAGEBOX_22,			// "버전이 맞지 않습니다. \\eye-ad\ToolData\Build\2_Quarter 에서\n실행파일과 리소스를 모~두~ 다시 복사해주세요~"
	MESSAGEBOX_23,			// "아이템을 파괴합니다.\n파괴된 아이템은 영구히 복구할 수 없습니다."
	//MESSAGEBOX_32 = 100032, // 중복 로그인입니다.
	MESSAGEBOX_33 = 100033,	// 서버를 선택하세요.
	MESSAGEBOX_34,			// 모든 파티원이 퇴장 포털에 모여 있어야 합니다.
	MESSAGEBOX_35,			// 암호를 입력하세요.
	MESSAGEBOX_36,			// 채널을 선택하세요.
//blondymarry start
    MESSAGEBOX_47 = 100047,			// 채널 이동하면 파티를 탈퇴하셔야 합니다.
//blondymarry end
	MESSAGEBOX_62 = 100062,	// 튜토리얼을 플레이하시겠습니까.
	MESSAGEBOX_FRIEND_QUERY = 1418,
	MESSAGEBOX_FRIEND_ADDED = 1419,
	MESSAGEBOX_GUILD_CREATE				= 1504,		//	'%s'길드를 창설 합니다.
	MESSAGEBOX_FRIEND_GROUP_DUPNAME,				//	현재 그룹명에 존재하는 이름이므로 추가할 수 없습니다
	MESSAGEBOX_FRIEND_GROUP_FILTERWORD,				//	금칙어이므로 추가할 수 없습니다
	MESSAGEBOX_FRIEND_DELETE,						//	정말로 삭제하시겠습니까?
	MESSAGEBOX_BLOCK_ADD_USER_FAIL,					//	추가할 유저의 이름을 입력해 주세요
	MESSAGEBOX_BLOCK_FRIEND_DELETE,					//	%s님이 차단되어 친구 목록에서 삭제됩니다.
	MESSAGEBOX_BLOCK_DELETE,						//	%s님이 차단창에서 제거되었습니다
	MESSAGEBOX_BLOCK_SERVER_ERROR_MSG,				//	ServerMessageBox 처리
	MESSAGEBOX_PARTY_INVITE_NONE,					//	초대할 캐릭터의 이름을 입력해 주세요
	MESSAGEBOX_MIN_MAX_LEVEL_ERROR,					//	최대 레벨은 최저레벨보다 작을 수 없습니다.	(파티생성뿐만 아니라, 무인상점 아이템 검색 레벨입력에서도 사용함)
	MESSAGEBOX_PARTY_CREATE_FAIL_MAX_LEVEL,			//	최대 레벨은 자신의 레벨보다 작을 수 없습니다.
	MESSAGEBOX_PARTY_RECONN_LAST_DUNGEON,			//	레이드 비정상 종료 시 재접속 여부 문의
	MESSAGEBOX_PARTY_RECONN_ON_DGNCLEAR,			//  레이드 비정상 종료 후 난입 시 던전클리어 도중일 때
	MESSAGEBOX_DISABLE_ALL,							//	입력을 Disable 시키는 메시지박스
	MESSAGEBOX_REQEUST_SPECIALBOX,                  //  특수보관함 승인

	//RLKT MESSAGEBOX REBIRTH!
	MESSAGE_BOX_RLKT_REBIRTH = 5000,
	MESSAGE_BOX_RLKT_FARMPVP = 5001,
	//
};
