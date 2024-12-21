#pragma once

#include "EtWorld.h"

class CDNField;
class CDNFieldData
{
private:
	string m_szMapName;
	int m_nMapIndex;
	CEtWorld m_World;

	struct AreaInfo {
		int nIndex;
		TPosition Min;
		TPosition Max;
		float fAngle;
		int nType;
	};
	std::vector<AreaInfo> m_VecGateList;
	std::vector<AreaInfo> m_VecStartPositionList;

protected:
	void CalcGatePosition();

public:
	CDNFieldData(void);
	virtual ~CDNFieldData(void);

	bool Init(char *pszMapName, int nMapIndex);
	void GetWidthHeight(UINT &nWidth, UINT &nHeight);

	int CheckGateArea(TPosition &Position);
	bool GetStartPosition(char cGateNo, TPosition &Pos);
	void GetStartPositionAngle( char cGateNo, float &fAngle );

	void GetRandomStartPosition(TPosition &Pos);	// ÀÓ½Ã·Î ·»´ý °ÔÀÌÆ®¿¡¼­ ÁÂÇ¥ »Ì¾Æ³»±â
	char GetRandomGateNo();

	void LoadCreateNpc(CDNField *pField);

	float GetCollisionHeight(float fX, float fZ);
	DWORD GetCurrentTileSize();

	int GetGateType(TPosition &Position);

	inline int GetGridX() { return m_World.GetGridX(); }
	inline int GetGridY() { return m_World.GetGridY(); }
	inline int GetMapIndex() { return m_nMapIndex; }
};
