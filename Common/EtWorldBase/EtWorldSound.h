#pragma once

class CEtWorldSoundEnvi;
class CEtWorldSector;
class CEtWorldSound {
public:
	CEtWorldSound( CEtWorldSector *pSector );
	virtual ~CEtWorldSound();

protected:
	CEtWorldSector *m_pSector;
	std::string m_szBGM;
	float m_fVolume;

	std::vector<CEtWorldSoundEnvi *> m_pVecEnvi;

public:
	virtual bool Load( const char *szInfoFile );
	virtual void Play() {}
	virtual void Stop() {}
	virtual void SetVolume( float fValue ) {}

	virtual CEtWorldSoundEnvi *AllocSoundEnvi();

	void InsertSoundEnvi( CEtWorldSoundEnvi *pEnvi );
	void DeleteSoundEnvi( CEtWorldSoundEnvi *pEnvi );

	const char *GetBGM() { return m_szBGM.c_str(); }
	float GetVolume() { return m_fVolume; }

	DWORD GetEnviCount();
	CEtWorldSoundEnvi *GetEnviFromIndex( DWORD dwIndex );
	CEtWorldSoundEnvi *GetEnviFromName( const char *szName );

	CEtWorldSector *GetSector() { return m_pSector; }
};