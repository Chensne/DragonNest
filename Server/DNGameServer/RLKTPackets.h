
enum ServiceType{
	LoginServer,
	VillageServer,
	GameServer,
	MasterServer,
	LogServer,
	DBServer,
	TEST,
	MAX
};


enum ClientPackets{
	CS_CONNECT,
	CS_CHECKLICENSE,
	CS_PING
};

enum ServerPackets{
	SC_CONNECT,
	SC_CHECKLICENSE,
	SC_PING,
	SC_CLIENT_CLOSE
};

struct Header
{
	int id;
	unsigned char data[32];
};

//Conectare.
struct PCS_CONNECT
{
	ServiceType ServiceType;
};

struct PSC_CONNECT
{
	bool OK;
};


//Verificare licenta
struct PCS_CHECKLICENSE{
	ServiceType ServiceType;
};

struct PSC_CHECKLICENSE{
	bool valid;
};

//Ping.
struct PCS_PING;

struct PSC_PING{
	bool OK;
};

//Close.
struct PSC_CLOSE{
	bool OK;
};