#pragma once
class ConnectedToGame : public CSingleton<ConnectedToGame>
{
public:
	ConnectedToGame();
	~ConnectedToGame();

	void onConnectToVillage();
};

