#pragma once

class CEtUIDialog;

namespace EtInterface
{
	namespace textcolor
	{
		const DWORD WHITESMOKE		= D3DCOLOR_ARGB(0xFF,0xF5,0xF5,0xF5);
		const DWORD WHITE			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
		const DWORD hongse = D3DCOLOR_ARGB(0xFF, 0xFF, 0x00, 0x00);
		const DWORD chengse = D3DCOLOR_ARGB(0xFF, 0xFF, 0xD7, 0x00);
		const DWORD DARKGRAY		= D3DCOLOR_ARGB(0xFF,0xA9,0xA9,0xA9);
		const DWORD	LIGHTSKYBLUE	= D3DCOLOR_ARGB(0xFF,0x87,0xCE,0xFA);
		const DWORD	PALEGREEN		= D3DCOLOR_ARGB(0xFF,0x98,0xFB,0x98);
		const DWORD RED				= D3DCOLOR_ARGB(0xFF,0xFF,0x00,0x00);
		const DWORD GOLD			= D3DCOLOR_ARGB(0xFF,0xFF,0xD7,0x00);
		const DWORD PALEGOLDENROD	= D3DCOLOR_ARGB(0xFF,0xEE,0xE8,0xAA);	
		const DWORD DODGERBLUE		= D3DCOLOR_ARGB(0xFF,0x1E,0x90,0xFF);
		const DWORD DARKTURQUOISE	= D3DCOLOR_ARGB(0xFF,0x00,0xCE,0xD1);
		const DWORD TOMATO			= D3DCOLOR_ARGB(0xFF,0xFF,0x63,0x47);
		const DWORD AQUA			= D3DCOLOR_ARGB(0xFF,0x00,0xFF,0xFF);
		const DWORD ORANGE			= D3DCOLOR_ARGB(0xFF,0xFF,0xA5,0x00);
		const DWORD PALETURQUOISE	= D3DCOLOR_ARGB(0xFF,0xAF,0xEE,0xEE);
		const DWORD SILVER			= D3DCOLOR_ARGB(0xFF,0xC0,0xC0,0xC0);
		const DWORD LIGHTGREY		= D3DCOLOR_ARGB(0xFF,0xD3,0xD3,0xD3);	
		const DWORD GREENYELLOW		= D3DCOLOR_ARGB(0xFF,0xAD,0xFF,0x2F);
		const DWORD ORANGERED		= D3DCOLOR_ARGB(0xFF,0xFF,0x45,0x00);
		const DWORD HONEYDEW		= D3DCOLOR_ARGB(0xFF,0xF0,0xFF,0xF0);
		const DWORD PERU			= D3DCOLOR_ARGB(0xFF,0xCD,0x85,0x3F);
		const DWORD BURLYWOOD		= D3DCOLOR_ARGB(0xFF,0xDE,0xB8,0x87);
		const DWORD GOLDENROD		= D3DCOLOR_ARGB(0xFF,0xDA,0xA5,0x20);
		const DWORD DARKORANGE		= D3DCOLOR_ARGB(0xFF,0xFF,0x8C,0x00);	
		const DWORD LIGHTCYAN		= D3DCOLOR_ARGB(0xFF,0xE0,0xFF,0xFF);
		const DWORD IVORY			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xF0);
		const DWORD YELLOW			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0x00);
		const DWORD CHARTREUSE		= D3DCOLOR_ARGB(0xFF,0x7F,0xFF,0x00);
		const DWORD THISTLE			= D3DCOLOR_ARGB(0xFF,0xD8,0xBF,0xD8);	
		const DWORD LIGHTSALMON		= D3DCOLOR_ARGB(0xFF,0xFF,0xA0,0x7A);
		const DWORD TURQUOISE		= D3DCOLOR_ARGB(0xFF,0x40,0xE0,0xD0);
		const DWORD VIOLET			= D3DCOLOR_ARGB(0xFF,0xEE,0x82,0xEE);
		const DWORD DARKVIOLET		= D3DCOLOR_ARGB(0xFF,0xCA,0x6A,0xEA);
		const DWORD AQUAMARINE		= D3DCOLOR_ARGB(0xFF,0x7F,0xFF,0xD4);
		const DWORD FORESTGREEN		= D3DCOLOR_ARGB(0xFF,0x22,0x8B,0x22);
		const DWORD HOTPINK			= D3DCOLOR_ARGB(0xFF,0xFF,0x69,0xB4);
		const DWORD MAROON			= D3DCOLOR_ARGB(0xFF,0x80,0x00,0x00);
		const DWORD NAVY			= D3DCOLOR_ARGB(0xFF,0x00,0x00,0x80);
		const DWORD DARK			= D3DCOLOR_ARGB(0xFF,0x77,0x77,0x77);
		const DWORD BLACK			= D3DCOLOR_ARGB(0xFF,0x00,0x00,0x00);

		const DWORD NPC_SPEECH		= D3DCOLOR_ARGB(0xFF,0x2B,0x27,0x23);

		const DWORD PVP_F_PLAYER	= D3DCOLOR_ARGB(0xFF,0x35,0xC8,0xFF);											
		const DWORD PVP_F_PLAYER_S  = D3DCOLOR_ARGB(0xFF,0x00,0x2D,0x3E);								
		const DWORD PVP_E_PLAYER	= D3DCOLOR_ARGB(0xFF,0xEB,0x5D,0x56);
		const DWORD PVP_E_PLAYER_SH = D3DCOLOR_ARGB(0xFF,0x3F,0x08,0x05);

		const DWORD PVP_MY_SLOTCOLOR = D3DCOLOR_ARGB(0xFF,0x89,0xFE,0x00);
		const DWORD PVP_REVENGE_TARGET = D3DCOLOR_ARGB(0xFF,0xEC,0x71,0x15);
		const DWORD PVP_REVENGE_ME	= D3DCOLOR_ARGB(0xFF,0x00,0xB0,0x50);
		
		//	���ó
		//const DWORD ENABLE			= LIGHTGREY;	
		//const DWORD DISABLE			= RED;

		//const DWORD DEFAULT			= ENABLE;
		//const DWORD DIALOGUE_MOUSEOVER = GOLD;
		//const DWORD ITEMMONEY		= GOLD;
		//const DWORD SELF			= PALEGOLDENROD;	
		//const DWORD SYS_ERROR		= RED;
		//const DWORD PARTY			= DARKTURQUOISE;
		//const DWORD GUILD			= TOMATO;
		//const DWORD PRIVATE			= GOLD;	
		//const DWORD PLUSOPTION		= PALEGOLDENROD;
		//const DWORD PARTYNAME		= PLUSOPTION;
		//const DWORD IDCOLOR			= LIGHTSKYBLUE;
		//const DWORD ADMIN_COLOR		= DISABLE;	
		//const DWORD NEGATIVE		= RED;
		//const DWORD FRIEND_OFF		= MAROON;

		const DWORD VALUE			= D3DCOLOR_ARGB(0xFF,0xE0,0xB0,0x78);

		const DWORD PLAYERNAME		= D3DCOLOR_ARGB(0xFF,137,254,0);
		const DWORD PLAYERNAME_S	= D3DCOLOR_ARGB(0xFF,45,83,0);
		const DWORD PARTYMEMBERNAME	= D3DCOLOR_ARGB(0xFF,32,195,255);
		const DWORD PARTYMEMBERNAME_S	= D3DCOLOR_ARGB(0xFF,0,45,62);
		const DWORD PARTYMASTERNAME	= D3DCOLOR_ARGB(0xFF,109,243,255);
		const DWORD PARTYMASTERNAME_S	= D3DCOLOR_ARGB(0xFF,0,58,63);
		const DWORD CHARNAME		= D3DCOLOR_ARGB(0xFF,255,254,232);
		const DWORD CHARNAME_S		= D3DCOLOR_ARGB(0xFF,59,59,53);
		const DWORD NPCNAME			= D3DCOLOR_ARGB(0xFF,255,252,33);
		const DWORD NPCNAME_S			= D3DCOLOR_ARGB(0xFF,72,71,0);
		const DWORD GUILDMEMBERNAME		= D3DCOLOR_ARGB(0xFF,200,240,75);
		const DWORD GUILDMEMBERNAME_S	= D3DCOLOR_ARGB(0xFF,45,83,0);
		const DWORD FRIENDNAME			= D3DCOLOR_ARGB(0xFF,160,105,250);
		const DWORD FRIENDNAME_S		= D3DCOLOR_ARGB(0xFF,55,40,80);
		const DWORD ISOLATENAME			= D3DCOLOR_ARGB(0xFF,240,80,90);
		const DWORD ISOLATENAME_S		= D3DCOLOR_ARGB(0xFF,50,25,25);
#ifdef PRE_ADD_VIP
		const DWORD VIPNAME				= D3DCOLOR_ARGB(0xFF,235,190,0);
		const DWORD VIPNAME_S			= D3DCOLOR_ARGB(0xFF,50,40,0);
#endif
		const DWORD MASTERNAME			= D3DCOLOR_ARGB(0xFF, 0, 255, 140);
		const DWORD MASTERNAME_S		= D3DCOLOR_ARGB(0xFF, 5, 40, 25);

		const DWORD MINIONMONSTER		= D3DCOLOR_ARGB(0xFF,110,240,15);
		const DWORD MINIONMONSTER_S		= D3DCOLOR_ARGB(0xFF,33,61,0);
		const DWORD MONSTER			= D3DCOLOR_ARGB(0xFF,110,240,15);
		const DWORD MONSTER_S		= D3DCOLOR_ARGB(0xFF,33,61,0);
#ifdef PRE_MOD_MONSTER_NAME_COLOR
		const DWORD CHAMPIONMONSTER	= D3DCOLOR_ARGB(0xFF,100,205,255);
#else
		const DWORD CHAMPIONMONSTER	= D3DCOLOR_ARGB(0xFF,255,142,71);
#endif
		const DWORD CHAMPIONMONSTER_S	= D3DCOLOR_ARGB(0xFF,0x00,0x00,0x00);
		const DWORD ELITEMONSTER	= D3DCOLOR_ARGB(0xFF,255,236,71);
		const DWORD ELITEMONSTER_S	= D3DCOLOR_ARGB(0xFF,0x00,0x00,0x00);
		const DWORD NAMEDMONSTER	= D3DCOLOR_ARGB(0xFF,221,71,255);
		const DWORD NAMEDMONSTER_S	= D3DCOLOR_ARGB(0xFF,0x00,0x00,0x00);
		const DWORD BOSSMONSTER		= D3DCOLOR_ARGB(255,224,46,41);
		const DWORD BOSSMONSTER_S	= D3DCOLOR_ARGB(0xFF,0x00,0x00,0x00);
		const DWORD NESTBOSSMONSTER		= D3DCOLOR_ARGB(255,224,46,41);
		const DWORD NESTBOSSMONSTER_S	= D3DCOLOR_ARGB(0xFF,0x00,0x00,0x00);

		const DWORD FONT_GREEN		= D3DCOLOR_ARGB(0xFF,0xAD,0xDD,0x51);
		const DWORD FONT_ORANGE		= D3DCOLOR_ARGB(0xFF,0xF0,0xA5,0x4D);

		const DWORD FONT_ALERT		= D3DCOLOR_ARGB(0xFF,251,86,71);

		const DWORD APPELLATION_SINGLE = D3DCOLOR_ARGB(255, 106, 195, 0 );
		const DWORD APPELLATION_SINGLE_S = D3DCOLOR_ARGB(255, 25, 46, 0 );

		const DWORD APPELLATION_LEADER = D3DCOLOR_ARGB(255, 77, 193, 203 );
		const DWORD APPELLATION_LEADER_S = D3DCOLOR_ARGB(255, 0, 28, 31 );

		const DWORD APPELLATION_MEMBER = D3DCOLOR_ARGB(255, 17, 149, 198 );
		const DWORD APPELLATION_MEMBER_S = D3DCOLOR_ARGB(255, 0, 23, 31 );

		const DWORD APPELLATION_ANY = D3DCOLOR_ARGB(255, 200, 199, 187 );
		const DWORD APPELLATION_ANY_S = D3DCOLOR_ARGB(255, 26, 26, 23 );

		const DWORD APPELLATION_GUILD = D3DCOLOR_ARGB(255, 235, 250, 155 );
		const DWORD APPELLATION_GUILD_S = D3DCOLOR_ARGB(255, 45, 83, 0 );

		const DWORD APPELLATION_FRIEND = D3DCOLOR_ARGB(255, 215, 170, 250 );
		const DWORD APPELLATION_FRIEND_S = D3DCOLOR_ARGB(255, 55, 40, 80 );

		const DWORD APPELLATION_ISOLATE = D3DCOLOR_ARGB(255, 245, 130, 135 );
		const DWORD APPELLATION_ISOLATE_S = D3DCOLOR_ARGB(255, 50, 25, 25 );

		const DWORD APPELLATION_MASTER = D3DCOLOR_ARGB(255, 130, 255, 160 );
		const DWORD APPELLATION_MASTER_S = D3DCOLOR_ARGB(255, 5, 40, 25 );

		const DWORD MAIL_READ			= D3DCOLOR_ARGB(255, 111, 90, 58);
		const DWORD MAIL_PREMIUM_READ	= D3DCOLOR_ARGB(255, 46, 66, 87);

		const DWORD TOOLTIP_STAT_UP		= D3DCOLOR_ARGB(255, 75, 180, 75);
		const DWORD TOOLTIP_STAT_DOWN	= D3DCOLOR_ARGB(255, 200, 70, 60);
		const DWORD TOOLTIP_STAT_UPDOWN	= D3DCOLOR_ARGB(255, 170, 100, 210);

#ifdef PRE_ADD_NEW_MONEY_SEED
		const DWORD MONEY_CASH = D3DCOLOR_ARGB( 255, 255, 155, 0 );
		const DWORD MONEY_RESERVE = D3DCOLOR_ARGB( 255, 100, 205, 255 );
		const DWORD MONEY_SEED = D3DCOLOR_ARGB( 255, 204, 204, 0 );
#endif // PRE_ADD_NEW_MONEY_SEED
	}

	namespace itemcolor
	{
		const DWORD NORMAL			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
		const DWORD MAGIC			= D3DCOLOR_ARGB(0xFF,0x88,0xAB,0x07);
		const DWORD RARE			= D3DCOLOR_ARGB(0xFF,0x79,0x92,0xFE);
		const DWORD EPIC			= D3DCOLOR_ARGB(0xFF,0xFF,0x91,0x48);
		const DWORD UNIQUE			= D3DCOLOR_ARGB(0xFF,0xBF,0x8B,0xFF);
		const DWORD HEROIC			= D3DCOLOR_ARGB(0xFF,0xDC,0x23,0x55);
		const DWORD NAMED           = D3DCOLOR_ARGB(0xFF,0xFF,0xD2,0x32);

		const DWORD EXPIRE			= D3DCOLOR_ARGB(0xFF,0xFF,0x88,0x88);
	}

	namespace chatcolor
	{
		const DWORD NORMAL			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
		const DWORD PARTY			= D3DCOLOR_ARGB(0xFF,0x85,0xA2,0xFF);
		const DWORD GUILD			= D3DCOLOR_ARGB(0xFF,0x9D,0xED,0x4F);
		const DWORD FRIEND			= D3DCOLOR_ARGB(0xFF,0xFE,0x7B,0xF6);	// �켱 �ӼӸ��� ���� ���� ������ �ٲ۴�.
		const DWORD PRIVATE			= D3DCOLOR_ARGB(0xFF,0xFE,0x7B,0xF6);
		const DWORD CHANNEL			= D3DCOLOR_ARGB(0xFF,0xFA,0xA6,0x44);
		const DWORD WORLD			= D3DCOLOR_ARGB(0xFF,0x86,0xFC,0xF5);
		const DWORD PRIVATECHANNEL	= D3DCOLOR_ARGB(0xFF,0xD7,0xC8,0xB4);
		const DWORD WORLD_RED		= D3DCOLOR_ARGB(0xFF,0xFF,0x5A,0x5A);

		const DWORD CHANNELBACK		= D3DCOLOR_ARGB(0x80,0xAF,0x75,0x30);
		const DWORD WORLDBACK		= D3DCOLOR_ARGB(0x80,0x5A,0xA3,0xA3);
		const DWORD OBSERVER        = D3DCOLOR_ARGB(0xFF,0x80,0x80,0x80);   // ������ �뵵�� �߰��մϴ� < Grey >
		const DWORD RAIDNOTICE		= D3DCOLOR_ARGB(255,255,120,90);
		const DWORD WORLD_REDBACK	= D3DCOLOR_ARGB(0x64,0x78,0x00,0x00);
	}

	namespace difficultycolor
	{
		const DWORD EASY			= D3DCOLOR_ARGB(0xFF,0x90,0xDD,0x16);
		const DWORD NORMAL			= D3DCOLOR_ARGB(0xFF,0xBF,0xDD,0x16);
		const DWORD HARD			= D3DCOLOR_ARGB(0xFF,0xFF,0xEF,0x42);
		const DWORD MASTER			= D3DCOLOR_ARGB(0xFF,0xF5,0x8F,0x2C);
		const DWORD ABYSS			= D3DCOLOR_ARGB(0xFF,0xE5,0x45,0x26);
		const DWORD CHAOS			= D3DCOLOR_ARGB(0xFF, 0xEE, 0x82, 0xEE);

	}

	namespace descritioncolor
	{
		const DWORD RED				= D3DCOLOR_ARGB(0xFF,0xE5,0x45,0x26);
		const DWORD	GREEN			= D3DCOLOR_ARGB(0xFF,0x90,0xDD,0x16);
		const DWORD DODGERBLUE		= D3DCOLOR_ARGB(0xFF,0x1E,0x90,0xFF);
		const DWORD YELLOW1			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0x00);
		const DWORD YELLOW2			= D3DCOLOR_ARGB(0xFF,0xFF,0xF3,0x32);
		const DWORD VIOLET			= D3DCOLOR_ARGB(0xFF,0xEE,0x82,0xEE);
		const DWORD SKY				= D3DCOLOR_ARGB(0xFF,0x75,0xAE,0xF5);
		const DWORD ORANGE			= D3DCOLOR_ARGB(0xFF,0xFF,0x93,0x45);
		const DWORD WHITE			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
		const DWORD GREY			= D3DCOLOR_ARGB(0xFF,0x99,0x99,0x99);
	}

	namespace chatroomcolor
	{
		const DWORD NORMAL			= D3DCOLOR_ARGB(0xFF,0xD7,0xD2,0xAF);
		const DWORD	TRADE			= D3DCOLOR_ARGB(0xFF,0xF0,0xDC,0xD2);
		const DWORD PROFILE			= D3DCOLOR_ARGB(0xFF,0xE6,0xF0,0xE6);
	}

	namespace debug
	{
		const DWORD WHITE			= D3DCOLOR_ARGB(0x4F,0xFF,0xFF,0xFF);
		const DWORD RED				= D3DCOLOR_ARGB(0x4F,0xFF,0x00,0x00);
		const DWORD GREEN			= D3DCOLOR_ARGB(0x4F,0x00,0xFF,0x00);
		const DWORD BLUE			= D3DCOLOR_ARGB(0x4F,0x00,0x00,0xFF);
		const DWORD YELLOW			= D3DCOLOR_ARGB(0x4F,0xFF,0xFF,0x00);
		const DWORD BLACK			= D3DCOLOR_ARGB(0x4F,0x00,0x00,0x00);

		// Note : float���ε� UI��ǥ�� �ȼ��� Outputâ�� ������ݴϴ�.
		//		����� �ϽǶ� ����Ͻø� �˴ϴ�.
		//		fWidth, fHeight���� ���� ��ũ���� ����, ���ΰ��� �־��ֽø� �˴ϴ�.
		//		
		//		���� ���, �θ� ���̾�α��� GetScreenWidth(), GetScreenHeight() �� �ǰڳ׿�.
		//
		extern void OutputDebugUICoord( const wchar_t *pString, const SUICoord& coord, float fWidth, float fHeight );
	}
}

/*
778899 lightslategray ����Ǫ����ȸ�� 
696969 dimgray ĢĢ��ȸ�� 
808080 gray ȸ�� 
a9a9a9 darkgray ��ο�ȸ�� 
c0c0c0 silver ���� 
d3d3d3 lightgrey ����ȸ�� 
dcdcdc gainsboro 
ffffff white ��� 
fff5ee seashell �ٴ������� 
fffafa snow ���� 
f8f8ff ghostwhite ������� 
fffaf0 floralwhite ���ǹ�� 
f5f5f5 whitesmoke ������� 
f0f8ff aliceblue �������������Ǫ���� 
f0ffff azure �ϴû� 
fdf5e6 oldlace ������ 
f5fffa mintcream ����ũ�� 
ffefd5 papayawhip ����Ƹ޸�ī��������� 
ffdab9 peachpuff �����ƺ��ұ� 
faf0e6 linen ���� 
eee8aa palegoldenrod ������ȭ���Ǵٳ��� 
ffe4e1 mistyrose £����̺� 
ffe4b5 moccasin �罿�����Ǳ��� 
ffdead navajowhite ����ȣ����� 
d2b48c tan ����ź���� 
f5deb3 wheat �� 
fafad2 lightgoldenrodyellow ������ȭ���ٳ��ʳ�� 
ffffe0 lightyellow ������� 
fff8dc cornsilk ���������ֽ� 
faebd7 antiquewhite �����ǹ�� 
f5f5dc beige �������� 
fffacd lemonchiffon ������������������ 
fffff0 ivory ��Ѻ� 
f0e68c khaki �������������ٻ��̼��κ��� 
e6e6fa lavender �������ֻ� 
fff0f5 lavenderblush �������ֺ��ӱ� 
ffe4c4 bisque ��ȫ���̵��°������������� 
ffebcd blanchedalmond ���������Ȳ���� 
deb887 burlywood ưư�Ѹ��� 
cd853f peru ���̼����ǰ�ȭ�� 
00ced1 darkturquoise ��ο�Ǫ������� 
00bfff deepskyblue £���ϴ��Ķ� 
7fffd4 aquamarine ����Ǫ������� 
1e90ff dodgerblue���������Ķ� 
00ffff cyan Ǫ������ 
f0fff0 honeydew �Ĺ����ٿ������´ܹ� 
87cefa lightskyblue �����ϴ��Ķ� 
afeeee paleturquoise ����Ǫ������� 
e0ffff lightcyan ����Ǫ������ 
add8e6 lightblue �����Ķ� 
b0c4de lightsteelblue ����ö�����Ķ� 
40e0d0 turquoise Ǫ������� 
48d1cc mediumturquoise �߰���Ǫ������� 
00ffff aqua ����Ǫ������� 
7b68ee mediumslateblue ��Ǫ����ȸ���Ķ� 
191970 midnightblue �������Ķ� 
6495ed cornflowerblue ���������Ķ� 
0000cd mediumblue �߰����Ķ� 
6a5acd slateblue Ǫ����ȸ���Ķ� 
4682b4 steelblue ö�����Ķ� 
0000ff blue �Ķ� 
483d8b darkslateblue ��ο�Ǫ����ȸ���Ķ� 
5f9ea0 cadetblue �������Ķ� 
87ceeb skyblue �ϴ��Ķ� 
4169e1 royalblue Ȳ�����Ķ� 
b0e0e6 powderblue �����Ķ� 
000080 navy £�������������� 
00008b darkblue ��ο��Ķ� 
8a2be2 blueviolet �Ķ�����ɻ� 
8b008b darkmagenta ��ο�£����ȫ�� 
9932cc darkorchid ��ο����� 
9400d3 darkviolet ��ο�����ɻ� 
ff00ff magenta £����ȫ�� 
ff00ff fuchsia ���������̳������޺� 
c71585 mediumvioletred ������ɻ��� 
ba55d3 mediumorchid �߰��ǿ������ 
9370db mediumpurple �߰������޺� 
dc143c crimson £�Ժ������� 
ff1493 deeppink £������ȫ�� 
ffb6c1 lightpink ��������ȫ�� 
ff69b4 hotpink �����ѿ���ȫ�� 
ffc0cb pink ����ȫ�� 
dda0dd plum £�� ����� 
800080 purple ���޺� 
ee82ee violet ����ɻ� 
d8bfd8 thistle ������ 
da70d6 orchid ������� 
4b0082 indigo ���� 
a52a2a brown ���� 
e9967a darksalmon ��ο���Ȳ�� 
f08080 lightcoral ������ȣ�� 
cd5c5c indianred �ε�𻡰� 
ffa07a lightsalmon ������Ȳ�� 
db7093 palevioletred ��������ɻ��� 
f4a460 sandybrown �������� 
fa8072 salmon ��Ȳ�� 
ff6347 tomato �丶��� 
ff4500 ornagered 
ff0000 red ���� 
800000 maroon ������������ 
8b0000 darkred ��ο�� 
b22222 firebrick ��ȭ 
d2691e chocolate ���� 
8b4513 saddlebrown ���尥�� 
a0522d sienna ������������ 
bc8f8f rosybrown ��̺����� 
ff7f50 coral ��ȣ�� 
ff8c00 darkorange ��ο�������������� 
ffa500 orange �������������� 
b8860b darkgoldenrod ��пȭ���Ǵٳ��� 
ffd700 gold �ݺ� 
ffff00 yellow ��� 
7fff00 chartreuse ���λ� 
7cfc00 lawngreen �ܵ��� 
00ff00 lime ������Ƿ������Ѱ��� 
32cd32 limegreen ���ӳ�� 
00ff7f springgreen ����� 
3cb371 mediumseagreen �߰��ǹٴٳ�� 
adff2f greenyellow ����ǳ�� 
8fbc8f darkseagreen ��ο�ٴٳ�� 
90ee90 lightgreen ������� 
98fb98 palegreen ������� 
9acd32 yello: wgree: n 
2e8b57 seagreen �ٴٳ�� 
00fa9a mediumspringgreen �ߺ���� 
20b2aa lightseagreen �����ٴٳ�� 
66cdaa mediumaquamarine �߿���û�ϻ� 
228b22 forestgreen ����� 
008b8b darkcyan ��ο�Ǫ������ 
008080 teal �Ϸϻ�����û�� 
006400 darkgreen ��ο��� 
556b2f darkolivegreen ��ο�ø����� 
008000 green ��� 
808000 olive ��Ǫ���������ǻ�ϱ��� 
6b8e23 olivedrab �ø��꿯������ 
bdb76b darkkhaki ��ο�����������ٻ� 
daa520 goldenrod ��ȭ���Ǵٳ��� 
*/