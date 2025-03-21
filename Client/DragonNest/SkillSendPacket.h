#pragma once

#include "DNPacket.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"




//inline void SendAddSkill(int nSkillID)
//{
//	CSAddSkill AddSkill;
//	SecureZeroMemory(&AddSkill, sizeof(CSAddSkill));
//	AddSkill.nSkillID = nSkillID;
//	CClientSessionManager::GetInstance().SendPacket(CS_SKILL, eSkill::CS_ADDSKILL, NULL, 0);
//}

//inline void SendDelSkill(int nSkillID)
//{
//	CSDelSkill DelSkill;
//	SecureZeroMemory(&DelSkill, sizeof(CSDelSkill));
//	DelSkill.nSkillID = nSkillID;
//	CClientSessionManager::GetInstance().SendPacket(CS_SKILL, eSkill::CS_DELSKILL, NULL, 0);
//}


inline void SendSkillLevelUpReq( int iSkillID, int iUsePoint )
{
	CSSkillLevelUpReq SkillLevelUpReq;
	SecureZeroMemory( &SkillLevelUpReq, sizeof(CSSkillLevelUpReq) );
	SkillLevelUpReq.nSkillID = iSkillID;
	SkillLevelUpReq.cUseSkillPoint = (char)iUsePoint;

	CClientSessionManager::GetInstance().SendPacket( CS_SKILL, eSkill::CS_SKILLLEVELUP_REQ, (char*)&SkillLevelUpReq, int(sizeof(SkillLevelUpReq)) );
}


inline void SendAcquireSkillReq( int iSkillID )
{
	CSAcquireSkillReq AcquireSkillReq;
	SecureZeroMemory( &AcquireSkillReq, sizeof(CSAcquireSkillReq) );
	AcquireSkillReq.nSkillID = iSkillID;

	CClientSessionManager::GetInstance().SendPacket( CS_SKILL, eSkill::CS_ACQUIRESKILL_REQ, (char*)&AcquireSkillReq, int(sizeof(AcquireSkillReq)) );
}


inline void SendUnlockSkillByMoneyReq( int iSkillID )
{
	CSUnlockSkillByMoneyReq UnlockSkillByMoneyReq;
	SecureZeroMemory( &UnlockSkillByMoneyReq, sizeof(UnlockSkillByMoneyReq) );
	UnlockSkillByMoneyReq.nSkillID = iSkillID;

	CClientSessionManager::GetInstance().SendPacket( CS_SKILL, eSkill::CS_UNLOCKSKILL_BY_MONEY_REQ, (char*)&UnlockSkillByMoneyReq, int(sizeof(UnlockSkillByMoneyReq)) );
}

#ifdef PRE_MOD_SKILLRESETITEM
inline void SendUseSkillResetCashItemReq(INT64 biItemSerial, int iInvenType, int iInvenIndex)
#else
inline void SendUseSkillResetCashItemReq( INT64 biItemSerial )
#endif
{
	CSUseSkillResetCashItemReq UseSkillResetCashItemReq;
#ifdef PRE_MOD_SKILLRESETITEM
	ZeroMemory( &UseSkillResetCashItemReq, sizeof(UseSkillResetCashItemReq) );
	UseSkillResetCashItemReq.biItemSerial = biItemSerial;
	UseSkillResetCashItemReq.cInvenType = (BYTE)iInvenType;
	UseSkillResetCashItemReq.cInvenIndex = (BYTE)iInvenIndex;
#else
	SecureZeroMemory( &UseSkillResetCashItemReq, sizeof(UseSkillResetCashItemReq) );
	UseSkillResetCashItemReq.biItemSerial = biItemSerial;
#endif

	CClientSessionManager::GetInstance().SendPacket( CS_SKILL, eSkill::CS_USE_SKILLRESET_CASHITEM_REQ, (char*)&UseSkillResetCashItemReq, int(sizeof(UseSkillResetCashItemReq)) );
}


inline void SendChangeSkillPage( int nSkillPage )
{
	CSChangeSkillPage ChangeSkillPage;
	SecureZeroMemory( &ChangeSkillPage, sizeof(ChangeSkillPage) );
	ChangeSkillPage.cSkillPageIndex = (char)nSkillPage;

	CClientSessionManager::GetInstance().SendPacket( CS_SKILL, eSkill::CS_CHANGE_SKILLPAGE, (char*)&ChangeSkillPage, int(sizeof(ChangeSkillPage)) );
}

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
inline void SendReservationSkillList( const std::vector<TReservationSkillReq> & vData )
{
	CSReservationSkillListReq ReservationSkillListReq;
	SecureZeroMemory( &ReservationSkillListReq, sizeof(ReservationSkillListReq) );

	int nCutCount = 0;
	DWORD itr = 0;
	while( itr < vData.size() )
	{
		if( nCutCount < ReservationSKillList::ReservationMax )
		{
			ReservationSkillListReq.tReservationSkill[nCutCount] = vData[itr];
		}

		++nCutCount;
		++itr;

		if( nCutCount == ReservationSKillList::ReservationMax 
			|| itr ==  vData.size() )
		{
			ReservationSkillListReq.nCount = nCutCount;
			int nLen = sizeof(CSReservationSkillListReq) - sizeof(ReservationSkillListReq.tReservationSkill) + (sizeof(TReservationSkillReq)*ReservationSkillListReq.nCount);
			CClientSessionManager::GetInstance().SendPacket( CS_SKILL, eSkill::CS_RESERVATION_SKILL_LIST_REQ, (char*)&ReservationSkillListReq, nLen );
			nCutCount = 0;
		}
	}
}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

#ifdef PRE_ADD_PRESET_SKILLTREE
inline void SendSkillSetListReq()
{
	CClientSessionManager::GetInstance().SendPacket( CS_SKILL, eSkill::CS_LIST_SKILLSET, NULL, 0 );
}

inline void SendSkillSetSave(BYTE cIndex, const WCHAR * pName, const std::vector< std::pair<int, BYTE> > & vecData)
{
	if (pName == NULL || true == vecData.empty() )
		return;

	CSSaveSkillSet packet;
	memset(&packet, 0, sizeof(CSSaveSkillSet));

	packet.cIndex = cIndex;
	_wcscpy(packet.wszSetName, _countof(packet.wszSetName), pName, (int)wcslen(pName));

	packet.cCount = (BYTE)vecData.size();
	for( DWORD itr = 0; itr < vecData.size(); ++itr )
	{
		packet.SKills[itr].nSkillID = vecData[itr].first;
		packet.SKills[itr].cLevel = vecData[itr].second;
	}

	CClientSessionManager::GetInstance().SendPacket( CS_SKILL, eSkill::CS_SAVE_SKILLSET, (char*)&packet, sizeof(CSSaveSkillSet) - sizeof(packet.SKills) + (sizeof(TSkillSetPartialData) * packet.cCount) );
}

inline void SendSKillSetDelete(BYTE cIndex)
{
	CSDeleteSkillSet packet;
	memset(&packet, 0, sizeof(CSDeleteSkillSet));

	packet.cIndex = cIndex;

	CClientSessionManager::GetInstance().SendPacket( CS_SKILL, eSkill::CS_DELETE_SKILLSET, (char*)&packet, sizeof(CSDeleteSkillSet) );
}
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE