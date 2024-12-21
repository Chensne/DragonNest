#include "stdafx.h"
#include "ItemData.h"


ItemData::ItemData(const wstring& name, int id, ItemData::Type type, ItemData::State state)
	: item(NULL), name(name), id(id), type(type), state(state), final(false), bFail(false)
{
}

ItemData::~ItemData()
{
}

wstring ItemData::TypeToString(ItemData::Type type)
{
	switch (type)
	{
	case ItemData::Partition:
		return L"Partition";

	case ItemData::World:
		return L"World";

	case ItemData::Launcher:
		return L"Launcher";

	case ItemData::Server:
		return L"Server";

	default:
		return L"";
	}
}

wstring ItemData::StateToString(ItemData::State state)
{
	switch (state)
	{
	case ItemData::Off:
		return L"Off";

	case ItemData::Activity:
		return L"Activity";

	case ItemData::Deactivity:
		return L"Deactivity";

	default:
		return L"";
	}
}

wstring ItemData::FinalToString(bool bFinal)
{
	if (bFinal)
		return L"Yes";

	return L"No";
}