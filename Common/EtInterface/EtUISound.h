#pragma once

class CEtUISound : public CSingleton<CEtUISound>
{
public:
	enum emUI_SOUND
	{
		UI_WINDOW_OPEN,
		UI_WINDOW_CLOSE,
		UI_SOUND_MAX,
		UI_SOUND_NONE = UI_SOUND_MAX,
	};

public:
	CEtUISound(void);
	virtual ~CEtUISound(void);

protected:
	int m_uiSoundIndex[UI_SOUND_MAX];

public:
	virtual void Initialize();
	void Finalize();
	void Play( emUI_SOUND uiSound );
};

#define GetUISound()		CEtUISound::GetInstance()