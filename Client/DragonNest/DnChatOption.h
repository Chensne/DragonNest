#pragma once

const int CHAT_NO			= 0x00000000;	//	�޽��� ����

const int CHAT_NORMAL		= 0x00000001;	//	�Ϲ�
const int CHAT_PARTY		= 0x00000002;	//	��Ƽ
const int CHAT_GUILD		= 0x00000004;	//	���
const int CHAT_PRIVATE		= 0x00000008;	//	�ӼӸ�
const int CHAT_CHANNEL		= 0x00000010;	//	ä�θ�(ĳ��, Ŭ������ ����)
const int CHAT_WORLD		= 0x00000020;	//	���帻
#ifdef PRE_PRIVATECHAT_CHANNEL
const int CHAT_PRIVATE_CHANNEL = 0x00000040;	// �缳 ä��
#ifdef PRE_ADD_WORLD_MSG_RED
const int CHAT_WORLD_RED	= 0x00000060;	//	���帻 (����-������)
const int CHAT_MODE_MAX		= 0x00000080;	//	�Է� ����� �ִ밪
#else // PRE_ADD_WORLD_MSG_RED
const int CHAT_MODE_MAX		= 0x00000060;	//	�Է� ����� �ִ밪
#endif // PRE_ADD_WORLD_MSG_RED
#else // PRE_PRIVATECHAT_CHANNEL
const int CHAT_MODE_MAX		= 0x00000040;	//	�Է� ����� �ִ밪
#endif // PRE_PRIVATECHAT_CHANNEL

// ä�� ���� ������ ������. ä�� ��尡 �ý��ۺ��� Ŭ ���� ����.
const int CHAT_SYSTEM		= 0x00000100;	//	�ý���
const int CHAT_NOTICE		= 0x00000200;	//	����

// ä�ù� �˸���
const int CHAT_CHATROOM1	= 0x00001000;	// �Ϲ�ä�ù�
const int CHAT_CHATROOM2	= 0x00002000;	// �ŷ�ä�ù�
const int CHAT_CHATROOM3	= 0x00004000;	// PRä�ù�
const int CHAT_REPUTE		= 0x00008000;	// NPC ȣ���� ��ǳ��
const int CHAT_RAIDNOTICE	= 0x00010000;

const int CHAT_GUILDWAR		= 0x00020000;

// ���� ���ʿ������� ��� �ּ�ó�� �մϴ�.
// ������ �ʿ���ԵǸ� �����ϰڽ��ϴ�.

//const int CHAT_FRIEND_LOG	= 0x00000200;
//const int CHAT_GUILD_LOG	= 0x00000400;

//const int CHAT_QUEST		= 0x00001000;	//	�� ������ ����Ʈ�½�ũ���� �������� SYSTEM���� ��ü�Ǿ���.
//const int CHAT_DAMAGE		= 0x00002000;
//const int CHAT_MONSTER		= 0x00004000;
//const int CHAT_LEVEL		= 0x00008000;
//const int CHAT_BUFF			= 0x00010000;
//const int CHAT_SKILL		= 0x00020000;
//const int CHAT_PARTY_DEATEH	= 0x00040000;
//const int CHAT_ITEM			= 0x00080000;

const int CHAT_PRIVATE_REPLY = 0x00100000;

//const int CHAT_ALL_MSG	= CHAT_NORMAL|CHAT_PARTY|CHAT_GUILD|CHAT_FRIEND|CHAT_PRIVATE;

class CDnChatOption
{
public:
	CDnChatOption(void);
	~CDnChatOption(void);

protected:
	int m_nOption;

public:
	int GetOption()						{ return m_nOption; }

public:
	void SetNomal( bool bCheck )		{ bCheck?m_nOption|=CHAT_NORMAL:m_nOption&=~CHAT_NORMAL; }
	void SetParty( bool bCheck )		{ bCheck?m_nOption|=CHAT_PARTY:m_nOption&=~CHAT_PARTY; }
	void SetGuild( bool bCheck )		{ bCheck?m_nOption|=CHAT_GUILD:m_nOption&=~CHAT_GUILD; }
	void SetPrivate( bool bCheck )		{ bCheck?m_nOption|=CHAT_PRIVATE:m_nOption&=~CHAT_PRIVATE; }
#ifdef PRE_PRIVATECHAT_CHANNEL
	void SetPrivateChannel( bool bCheck ) { bCheck?m_nOption|=CHAT_PRIVATE_CHANNEL:m_nOption&=~CHAT_PRIVATE_CHANNEL; }
#endif // PRE_PRIVATECHAT_CHANNEL
	// ä�θ��� ���帻�� �⺻������ �� �� ���� �Ǿ��ִ�.
	void SetChannel( bool bCheck )		{ bCheck?m_nOption|=CHAT_CHANNEL:m_nOption&=~CHAT_CHANNEL; }
	void SetWorld( bool bCheck )		{ bCheck?m_nOption|=CHAT_WORLD:m_nOption&=~CHAT_WORLD; }
	// �ý���
	void SetSystem( bool bCheck )		{ bCheck?m_nOption|=CHAT_SYSTEM:m_nOption&=~CHAT_SYSTEM; }

	//void SetFriendLog( bool bCheck )	{ bCheck?m_nOption|=CHAT_FRIEND_LOG:m_nOption&=~CHAT_FRIEND_LOG; }
	//void SetGuildLog( bool bCheck )		{ bCheck?m_nOption|=CHAT_GUILD_LOG:m_nOption&=~CHAT_GUILD_LOG; }
	//
	//void SetQuest( bool bCheck )		{ bCheck?m_nOption|=CHAT_QUEST:m_nOption&=~CHAT_QUEST; }
	//void SetDamage( bool bCheck )		{ bCheck?m_nOption|=CHAT_DAMAGE:m_nOption&=~CHAT_DAMAGE; }
	//void SetMonster( bool bCheck )		{ bCheck?m_nOption|=CHAT_MONSTER:m_nOption&=~CHAT_MONSTER; }
	//void SetLevel( bool bCheck )		{ bCheck?m_nOption|=CHAT_LEVEL:m_nOption&=~CHAT_LEVEL; }
	//void SetBuff( bool bCheck )			{ bCheck?m_nOption|=CHAT_BUFF:m_nOption&=~CHAT_BUFF; }
	//void SetSkill( bool bCheck )		{ bCheck?m_nOption|=CHAT_SKILL:m_nOption&=~CHAT_SKILL; }
	//void SetPartyDeath( bool bCheck )	{ bCheck?m_nOption|=CHAT_PARTY_DEATEH:m_nOption&=~CHAT_PARTY_DEATEH; }
	//void SetItem( bool bCheck )			{ bCheck?m_nOption|=CHAT_ITEM:m_nOption&=~CHAT_ITEM; }

public:
	BOOL IsNomal()		{ return m_nOption&CHAT_NORMAL; }
	BOOL IsParty()		{ return m_nOption&CHAT_PARTY; }
	BOOL IsGuild()		{ return m_nOption&CHAT_GUILD; }
	BOOL IsPrivate()	{ return m_nOption&CHAT_PRIVATE; }
#ifdef PRE_PRIVATECHAT_CHANNEL
	BOOL IsPrivateChannel()	{ return m_nOption&CHAT_PRIVATE_CHANNEL; }
#endif // PRE_PRIVATECHAT_CHANNEL
	BOOL IsChannel()	{ return m_nOption&CHAT_CHANNEL; }
	BOOL IsWorld()		{ return m_nOption&CHAT_WORLD; }
	BOOL IsSystem()		{ return m_nOption&CHAT_SYSTEM; }

	//BOOL IsFriendLog()	{ return m_nOption&CHAT_FRIEND_LOG; }
	//BOOL IsGuildLog()	{ return m_nOption&CHAT_GUILD_LOG; }

	//BOOL IsQuest()		{ return m_nOption&CHAT_QUEST; }
	//BOOL IsDamage()		{ return m_nOption&CHAT_DAMAGE; }
	//BOOL IsMonster()	{ return m_nOption&CHAT_MONSTER; }
	//BOOL IsLevel()		{ return m_nOption&CHAT_LEVEL; }
	//BOOL IsBuff()		{ return m_nOption&CHAT_BUFF; }
	//BOOL IsSkill()		{ return m_nOption&CHAT_SKILL; }
	//BOOL IsPartyDeath()	{ return m_nOption&CHAT_PARTY_DEATEH; }
	//BOOL IsItem()		{ return m_nOption&CHAT_ITEM; }
};