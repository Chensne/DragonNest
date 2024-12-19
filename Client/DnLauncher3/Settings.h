#pragma once
struct Data
{
	BYTE version;
	char Username[20];
	char Password[20];
	bool RememberMe;
	bool ShowNewUI;
	char RESERVED[500];
};
extern Data data;
//class DnNHNService : public CSingleton<DnNHNService>
class Settings : public CSingleton<Settings>
{
public:
	~Settings();
	Settings();
	void LoadSettings();
	void SaveSettings();
	void EncDecData(BYTE * data);


private:
	void SetVersion(BYTE ver);
};


