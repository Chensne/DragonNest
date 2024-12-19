#pragma once

class CFileStream;
class CEtWorldSound;
class CEtWorldSoundEnvi {
public:
	CEtWorldSoundEnvi( CEtWorldSound *pSound );
	virtual ~CEtWorldSoundEnvi();

protected:
	CEtWorldSound *m_pSound;
	std::string m_szName;
	std::string m_szFileName;
	EtVector3 m_vPosition;
	bool m_bStream;
	float m_fRange;
	float m_fRollOff;
	float m_fVolume;

public:
	virtual bool Load( CStream *pStream );
	virtual void Play() {}
	virtual void Stop() {}

	const char *GetName() { return m_szName.c_str(); }
	const char *GetFileName() { return m_szFileName.c_str(); }
	EtVector3 *GetPosition() { return &m_vPosition; }
	float GetRange() { return m_fRange; }
	float GetRollOff() { return m_fRollOff; }
	float GetVolume() { return m_fVolume; }
	bool IsStream() { return m_bStream; }
};