#pragma once

///////////////////////////////////////////
//treectrl icon index
#define ICON_PARTITION_OFF			(int)0
#define ICON_PARTITION_DEACTIVITY	(int)1
#define ICON_PARTITION_ACTIVITY		(int)2

#define ICON_WORLD_OFF				(int)3
#define ICON_WORLD_DEACTIVITY		(int)4
#define ICON_WORLD_ACTIVITY			(int)5

#define ICON_LAUNCHER_OFF			(int)6
#define ICON_LAUNCHER_DEACTIVITY	(int)7
#define ICON_LAUNCHER_ACTIVITY		(int)8

#define ICON_SERVER_OFF				(int)9
#define ICON_SERVER_DEACTIVITY		(int)10
#define ICON_SERVER_ACTIVITY		(int)11
///////////////////////////////////////////

class ItemData
{
public:
	enum Type
	{
		Partition,
		World,
		Launcher,
		Server
	};

	enum State
	{
		Off,
		Deactivity,
		Activity
	};

public:
	ItemData(const wstring& name, int id, ItemData::Type type, ItemData::State state);
	~ItemData();

public:
	static wstring TypeToString(ItemData::Type);
	static wstring StateToString(ItemData::State);
	static wstring FinalToString(bool bFinal);

public:
	HTREEITEM item;
	wstring name;
	int id;
	wstring world;
	Type type;
	State state;
	wstring version;
	bool final;
	bool bFail;
};