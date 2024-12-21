#pragma once

class CDnPlayerTestFlag
{
public:
	CDnPlayerTestFlag() {
		m_cForceDungeonClearRank = -1;
	};
	virtual ~CDnPlayerTestFlag() {}

public:
	char m_cForceDungeonClearRank;
};