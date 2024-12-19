#pragma once

class CEtWorldEventArea;
class CEtWorldEventControl;
class CFileStream;
class CEtWorldSector;

class CEtWorldEventControl
{
public:
	CEtWorldEventControl( CEtWorldSector *pSector );
	virtual ~CEtWorldEventControl();

protected:
	CEtWorldSector *m_pSector;
//	std::string m_szName;
	int m_nUniqueID;
	std::vector<CEtWorldEventArea *> m_pVecList;
	std::vector<DWORD> m_dwVecCheckList;
	std::map<int, CEtWorldEventArea *> m_nMapCreateUniqueIDSearch;

public:
	virtual bool Initialize();
	bool Load( CStream *pStream );

//	const char *GetName() { return m_szName.c_str(); }
//	void SetName( const char *szName ) { m_szName = szName; }
	void SetUniqueID( int nValue ) { m_nUniqueID = nValue; }
	int GetUniqueID() { return m_nUniqueID; }
	DWORD GetAreaCount();
	CEtWorldEventArea *GetAreaFromIndex( DWORD dwIndex );
	CEtWorldEventArea *GetAreaFromName( const char *szName, int nIndex = 0 );
	CEtWorldEventArea *GetAreaFromCreateUniqueID( int nUniqueID );
	int FindAreaFromName( const char *szName, std::vector<CEtWorldEventArea *> *pVecList );

	virtual CEtWorldEventArea *AllocArea();
	CEtWorldEventArea *InsertArea( EtVector3 &vMin, EtVector3 &vMax, const char *szName );
	void RemoveArea( const char *szName );
	void RemoveArea( DWORD dwIndex );
	void RemoveAreaFromCreateUniqueID( int nUniqueID );

	void Process( EtVector3 &vPos, bool bIgnoreY = false );

	DWORD GetCheckAreaCount();
	CEtWorldEventArea *GetCheckArea( DWORD dwIndex );
	int FindCheckAreaFromSubStr( const char *szStr, std::vector<CEtWorldEventArea *> *pVecList );
	CEtWorldSector *GetSector() { return m_pSector; }

};