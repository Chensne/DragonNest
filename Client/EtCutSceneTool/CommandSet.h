#pragma once
#include "ICommand.h"
#include "EternityEngine.h"
#include "IDnCutSceneDataReader.h"


enum
{
	CMD_NULL,

	CMD_UPDATE_VIEW,
	
	CMD_REGISTER_RES,
	CMD_UNREGISTER_RES,

	CMD_MODIFY_SCENE_INFO,
	
	CMD_INSERT_ACTION,
	CMD_REMOVE_ACTION,
	CMD_ACTION_PROP_CHANGE,
	CMD_KEY_PROP_CHANGE,
	CMD_INSERT_KEY,
	CMD_REMOVE_KEY,

	CMD_INSERT_EVENT,
	CMD_REMOVE_EVENT,
	CMD_EVENT_PROP_CHANGE,

	CMD_ACTOR_PROP_CHANGE,

	CMD_COPY_TO_CLIPBOARD,
	CMD_PASTE_FROM_CLIPBOARD,

	CMD_CHANGE_ACTOR_RES,

	CMD_BATCH_EDIT,
};


// 뷰를 모조리 새로 업데이트
class CUpdateViewCmd : public ICommand
{
public:
	CUpdateViewCmd( ICmdReceiver* pCmdReceiver ) : ICommand(pCmdReceiver) {};
	virtual ~CUpdateViewCmd(void) {};

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void ) { return new CUpdateViewCmd( m_pCmdReceiver ); };

	virtual const wxChar* GetDesc( void ) const { return wxT("Update view"); };
	virtual int GetTypeID( void ) { return CMD_UPDATE_VIEW; };

	virtual void Excute( void );
	virtual void Undo( void ) {};

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return false; };
};


// 리소스 사용 등록 커맨드
class CRegisterResCmd : public ICommand
{
private:
	wxString			m_strResName;
	int					m_iResourceKind;
	EtVector3			m_vPos;
	float				m_fRotation;

public:
	CRegisterResCmd( ICmdReceiver* pCmdReceiver, const wxChar* pResourceName, int iResourceKind, 
					 EtVector3& vPos, float fRotation );
	virtual ~CRegisterResCmd(void);

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_REGISTER_RES; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};



// 사용되는 리소스 제거 커맨드
class CRemoveRegResCmd : public ICommand
{
private:
	wxString			m_strResName;
	int					m_iResourceKind;
	EtVector3			m_vPos;
	float				m_fRotation;

public:

	// 액터인 경우에는 딸려있는 액션 리스트까지 전부 보관해 놨다가 UNDO 가 일어나면 복구 시켜준다.
	vector<ActionInfo*> m_vlpActionInfo;
	vector<KeyInfo*>	m_vlpKeyInfo;

public:
	CRemoveRegResCmd( ICmdReceiver* pCmdReceiver, const wxChar* pResourceName, int iResourceKind );
	virtual ~CRemoveRegResCmd( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_UNREGISTER_RES; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};



// 장면 속성 변경 커맨드
class CSceneInfoModify : public ICommand
{
private:
	SceneInfo* m_pSceneInfo;
	SceneInfo* m_pPrevSceneInfo;

public:
	CSceneInfoModify( ICmdReceiver* pCmdReceiver, SceneInfo* pSceneInfo );
	virtual ~CSceneInfoModify( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );
	
	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_MODIFY_SCENE_INFO; };

	virtual void Excute( void );
	virtual void Undo( void );

	virtual bool Recordable( void ) { return true; };
};



// 액션 추가 커맨드
class CActionInsertCmd : public ICommand
{
private:
	ActionInfo* m_pActionInfo;

public:
	CActionInsertCmd( ICmdReceiver* pCmdReceiver, ActionInfo* pAction );
	virtual ~CActionInsertCmd( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_INSERT_ACTION; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };

};



// 액터 리소스 변경 커맨드
class CActorResPropChange : public ICommand
{
private:
	wxString		m_strNewActorName;
	EtVector3		m_vPos;
	float			m_fRotation;
	bool			m_bFitYPosToMap;
	bool			m_bInfluenceLightmap;
	int				m_iMonsterTableIDAsBoss;		// 이 액터 리소스를 쓰는 보스 리스트 중에 선택된 인덱스. 없으면 -1임.
	bool			m_bScaleLock;

	wxString		m_strOldActorName;
	EtVector3		m_vOldPos;
	float			m_fOldRotation;
	bool			m_bOldFitYPosToMap;
	bool			m_bOldInfluenceLightmap;
	int				m_iOldMonsterTableIDAsBoss;
	bool			m_bOldScaleLock;

public:
	CActorResPropChange( ICmdReceiver* pCmdReceiver, const wxChar* pOldActorName, const wxChar* pNewActorName, 
						 const EtVector3& vPos, float fRotation, bool bFitYPosToMap, bool bInfluenceLightmap, int iSelectedBossInfo, bool bScaleLock );
	virtual ~CActorResPropChange( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_ACTOR_PROP_CHANGE; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};




// 액션 정보 변경 커맨드
class CActionPropChange : public ICommand
{
public:
	ActionInfo			m_NewActionInfo;
	ActionInfo			m_PrevActionInfo;

	bool				m_bUndo;

public:
	CActionPropChange( ICmdReceiver* pCmdReceiver, ActionInfo* pNewActionInfo );
	virtual ~CActionPropChange( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_ACTION_PROP_CHANGE; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};


// 액션 삭제 커맨드
class CActionRemoveCmd : public ICommand
{
public:
	int					m_iActionIDToRemove;
	ActionInfo			m_RemoveActionInfo;

public:
	CActionRemoveCmd( ICmdReceiver* pCmdReceiver, int iActionID );
	virtual ~CActionRemoveCmd( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_REMOVE_ACTION; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};


// 키 추가 커맨드
class CKeyInsertCmd : public ICommand
{
private:
	KeyInfo* m_pKeyInfo;

public:
	CKeyInsertCmd( ICmdReceiver* pCmdReceiver, KeyInfo* pKey );
	virtual ~CKeyInsertCmd( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_INSERT_KEY; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};


// 키 삭제 커맨드
class CKeyRemoveCmd : public ICommand
{
public:
	int					m_iKeyIDToRemove;
	KeyInfo				m_RemoveKeyInfo;

public:
	CKeyRemoveCmd( ICmdReceiver* pCmdReceiver, int iKeyID );
	virtual ~CKeyRemoveCmd( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_REMOVE_KEY; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};


// 키 정보 변경 커맨드
class CKeyPropChange : public ICommand
{
public:
	KeyInfo				m_NewKeyInfo;
	KeyInfo				m_PrevKeyInfo;

	bool				m_bUndo;

public:
	CKeyPropChange( ICmdReceiver* pCmdReceiver, KeyInfo* pNewKeyInfo );
	virtual ~CKeyPropChange( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_KEY_PROP_CHANGE; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};



// 이벤트 추가 커맨드
class CEventInsertCmd : public ICommand
{
private:
	EventInfo* m_pEventInfo;

public:
	CEventInsertCmd( ICmdReceiver* pCmdReceiver, EventInfo* pEvent );
	virtual ~CEventInsertCmd( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_INSERT_EVENT; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};


// 이벤트 삭제 커맨드
class CEventRemoveCmd : public ICommand
{
public:
	int					m_iEventIDToRemove;
	EventInfo*			m_pRemoveEventInfo;

public:
	CEventRemoveCmd( ICmdReceiver* pCmdReceiver, int iEventID );
	virtual ~CEventRemoveCmd( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_REMOVE_EVENT; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};


// 이벤트 정보 변경 커맨드
class CEventPropChange : public ICommand
{
public:
	EventInfo*			m_pNewEventInfo;
	EventInfo*			m_pPrevEventInfo;

	bool				m_bUndo;

public:
	CEventPropChange( ICmdReceiver* pCmdReceiver, EventInfo* pNewEventInfo );
	virtual ~CEventPropChange( void );

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_EVENT_PROP_CHANGE; };

	virtual void Excute( void );
	virtual void Undo( void );

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return true; };
};


// 여러 데이터를 통째로 바꾸는 커맨드. 현재는 시작 시간만 있음.
class CBatchEdit : public ICommand
{
public:
	vector<const ActionInfo*>		m_vlpActionInfo;
	vector<const KeyInfo*>			m_vlpKeyInfo;
	vector<const EventInfo*>		m_vlpEventInfo;

	float							m_fStartTimeDelta;

public:
	CBatchEdit( ICmdReceiver* pCmdReceiver, float fStartTimeDelta,
											const vector<const ActionInfo*> vlpActionInfo, 
											const vector<const KeyInfo*> vlpKeyInfo,
											const vector<const EventInfo*> vlpEventInfo );
	virtual ~CBatchEdit( void );

	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_BATCH_EDIT; };

	virtual void Excute( void );
	virtual void Undo( void );

	virtual bool Recordable( void ) { return true; };
};


// 액션/키/이벤트 복사해서 붙여넣기
// 클립보드에 넣는 커맨드인데.. 아.. 템플릿으로 할까.. -_-
class CCopyToClipboard : public ICommand
{
public:
	enum
	{
		COPY_ACTION,
		COPY_KEY,
		COPY_EVENT,
	};

	ActionInfo*			m_pCopiedActionInfo;
	KeyInfo*			m_pCopiedKeyInfo;
	EventInfo*			m_pCopiedEventInfo;

private:
	int					m_iCopyType;

public:
	CCopyToClipboard( ICmdReceiver* pCmdReceiver, const ActionInfo* pActionInfo );
	CCopyToClipboard( ICmdReceiver* pCmdReceiver, const KeyInfo* pKeyInfo );
	CCopyToClipboard( ICmdReceiver* pCmdReceiver, const EventInfo* pEventInfo );
	virtual ~CCopyToClipboard( void );

	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_COPY_TO_CLIPBOARD; };

	virtual void Excute( void );
	virtual void Undo( void );

	virtual bool Recordable( void ) { return true; };

	int GetCopyType( void ) { return m_iCopyType; };
};


class CPasteFromClipboard : public ICommand
{
public:
	ActionInfo*			m_pCopiedActionInfo;
	KeyInfo*			m_pCopiedKeyInfo;
	EventInfo*			m_pCopiedEventInfo;

private:
	int					m_iCopyType;

public:
	CPasteFromClipboard( ICmdReceiver* pCmdReceiver, int iCopyType, const char* pActorToPaste, float fTimeToPaste );
	virtual ~CPasteFromClipboard( void );

	virtual ICommand* CreateSame( void );

	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_PASTE_FROM_CLIPBOARD; };

	virtual void Excute( void );
	virtual void Undo( void );

	virtual bool Recordable( void ) { return true; };

	int GetCopyType( void ) { return m_iCopyType; };
};


class CChangeActorRes : public ICommand
{
private:
	wxString			m_strNewActorResName;		// 새로 바꿀 액터의 리소스(액터테이블) 이름
	wxString			m_strNewActorName;			// 기존 액터의 _ 이후의 사용자가 입력한 문자열을 합한 새로 바꿀 액터 이름(실제 컷신 데이터 자료구조의 키 값)
	wxString			m_strOldActorResName;		// 기존 액터의 리소스 이름
	wxString			m_strOldActorName;			// 기존 액터의 이름

public:
	CChangeActorRes( ICmdReceiver* pCmdReceiver, const wxChar* pNewActorResName, const wxChar* pOldActorName );
	virtual ~CChangeActorRes( void );

	virtual ICommand* CreateSame( void ) { return new CChangeActorRes(*this); };
	
	virtual const wxChar* GetDesc( void ) const;
	virtual int GetTypeID( void ) { return CMD_CHANGE_ACTOR_RES; };

	virtual void Excute( void );
	virtual void Undo( void );

	virtual bool Recordable( void ) { return true; };

	const wxChar* GetNewActorResName( void ) { return m_strNewActorResName.c_str(); };
	const wxChar* GetNewActorName( void ) { return m_strNewActorName.c_str(); };
	const wxChar* GetOldActorResName( void ) { return m_strOldActorResName.c_str(); };
	const wxChar* GetOldActorName( void ) { return m_strOldActorName.c_str(); };
}; 