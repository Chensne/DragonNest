#pragma once

class CDNField;
class CDNBaseObject;
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
class CSocketContext;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

typedef map<UINT, CDNBaseObject*> TMapObjects;

class CDNBaseObject
{
private:
protected:
	TBaseData m_BaseData;
	TParamData m_ParamData;
	CDNField* m_pField;

	bool m_boAllowDelete;
	char m_cLeaveType;	// eLeaveType

#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	std::map<UINT,std::pair<CDNBaseObject*,CSocketContext*>> m_UserViewObjects;
#else
	TMapObjects m_UserViewObjects;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	TMapObjects m_NpcViewObjects;

	CSyncLock m_UserLock;
	CSyncLock m_NpcLock;

	int IsView (TPosition *pPos);
	int GetDistance (TPosition *pPos);

public:
	CDNBaseObject(void);
	virtual ~CDNBaseObject(void);

	bool AddViewObject(CDNBaseObject *pObj);
	bool DelViewObject(CDNBaseObject *pObj);
	CDNBaseObject* GetViewObject(UINT nUID, bool bIgnoreDangling=false );
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	CSocketContext* GetViewObjectContext(UINT nUID);
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

	virtual bool EnterWorld();
	virtual bool LeaveWorld();

	virtual bool InitObject(WCHAR *pName, UINT nObjUID, int nChannelID, int nMapIndex, TPosition *pCurPos);
	virtual bool FinalObject();

	virtual int FieldProcess(CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData);
	virtual void DoUpdate(DWORD CurTick);

	int SendLocalMessage(UINT nObjUID, USHORT wMsg);
	int SendUserLocalMessage(UINT nObjUID, USHORT wMsg);
	int SendNpcLocalMessage(UINT nObjUID, USHORT wMsg);

	int SendFieldMessage(USHORT wMsg);

	inline UINT GetObjectID() { return m_BaseData.nObjectID; }
	inline TPosition GetCurrentPos() { return m_BaseData.CurPos; }
	inline TPosition GetTargetPos() { return m_BaseData.TargetPos; }
	inline WCHAR* wszName() { return m_BaseData.wszName; }
	inline float GetRotate() { return m_BaseData.fRotate; }
	inline bool IsBattleMode() { return m_BaseData.bBattleMode; }
	int GetChannelAttribute();

	inline TParamData* GetParamData() { return &m_ParamData; }
	inline CDNField* GetField(){ return m_pField; }

	inline bool boAllowDelete() { return m_boAllowDelete; }
	inline void SwapPosition() { m_BaseData.CurPos = m_BaseData.TargetPos; }

	inline char GetLeaveType() { return m_cLeaveType; }
	inline void SetLeaveType(char cType) { m_cLeaveType = cType; }

	virtual void SetCharacterName(const WCHAR* pwszName);
};
