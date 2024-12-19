#include "StdAfx.h"
#include "RLKTAuth.h"
#include "RLKTPackets.h"
#include "Singleton.h"

#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#pragma comment(lib, "user32.lib")


RLKTAuth::RLKTAuth()
{
}


RLKTAuth::~RLKTAuth()
{
}
bool isConnected = false;
bool isLicenseValid = false;
HANDLE hPipe;

#define PACKET_LEN sizeof(Header)
#define SERVICE_TYPE VillageServer

//o zona periculoasa.
void DangerZone()
{
	isConnected = false;
	isLicenseValid = false;
	//printf("Danger Zone Called.\n");
	ExitProcess(0);
}


//o simpla encriptie
byte XOR_KEYS[] = { 0x12, 0x58, 0xA2, 0x91, 0xDF, 0xAB, 0x92, 0xFC };
void encdec(BYTE *data, DWORD size)
{
	for (int i = 0; i < size; i++)
	{
		data[i] ^= XOR_KEYS[i%sizeof(XOR_KEYS)];
	}
}

BYTE * CraftPacket(int id, BYTE* data)
{
	Header *head = new Header;
	head->id = id;
	if (data != 0)
		memcpy(head->data, data, sizeof(head->data));

	return (BYTE*)head;
}
///
bool SendPacket(int id, BYTE* data)
{
	DWORD dwWritten = 0;
	WriteFile(hPipe,
		CraftPacket(id, data),
		PACKET_LEN,   // = length of string + terminating '\0' !!!
		&dwWritten,
		NULL);

	if (dwWritten == PACKET_LEN)
	{
		return true;
	}
	else{
		return false;
	}
}


Header *RecvPacket()
{
	DWORD totalread;
	Header *head = new Header;
	ReadFile(hPipe, head, PACKET_LEN, &totalread, 0);
	if (totalread != PACKET_LEN)
	{
		printf("Err. recv packet size != \n");
	}

	//decriptare.
	encdec((BYTE*)head, PACKET_LEN);

	return head;
}

void PacketHandler(int ID, BYTE * data)
{
	switch (ID)
	{
	case SC_CONNECT:
	{
					   PSC_CONNECT *pack = new PSC_CONNECT;
					   pack = (PSC_CONNECT*)data;
					   if (pack->OK)
					   {
						   isConnected = true;
					   }
	}
		break;

	case SC_CHECKLICENSE:
	{
							if (!isConnected)
							{
								DangerZone();
								return;
							}

							PSC_CHECKLICENSE *pack = new PSC_CHECKLICENSE;
							pack = (PSC_CHECKLICENSE*)data;
							if (pack->valid)
							{
								isLicenseValid = true;
							}
							else{
								DangerZone();
							}
	}
		break;

	case SC_PING:
	{
					//check
					if (!isConnected || !isLicenseValid)
					{
						DangerZone();
						return;
					}
	}
		break;

	case SC_CLIENT_CLOSE:
		PSC_CLOSE *d = new PSC_CLOSE;
		d = (PSC_CLOSE*)data;

		if (d->OK)
		{
			DangerZone();
			return;
		}
		break;
	}
}
//
void RLKTAuth::Main()
{
	return;
	DWORD dwWritten;
	bool isConnected = false;

	hPipe = CreateFile(TEXT("\\\\.\\pipe\\RLKT"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hPipe != INVALID_HANDLE_VALUE)
	{
		PCS_CONNECT *ConnPacket = new PCS_CONNECT;
		ConnPacket->ServiceType = SERVICE_TYPE;
		SendPacket(CS_CONNECT, (BYTE*)ConnPacket);
		Sleep(500);
		Header *headConnect = RecvPacket();
		PacketHandler(headConnect->id, headConnect->data);



		PCS_CHECKLICENSE *LicensePacket = new PCS_CHECKLICENSE;
		LicensePacket->ServiceType = SERVICE_TYPE;
		SendPacket(CS_CHECKLICENSE, (BYTE*)LicensePacket);
		Sleep(500);
		Header *headLicense = RecvPacket();
		PacketHandler(headLicense->id, headLicense->data);


		//while (1) {
			//send
			SendPacket(CS_PING, 0);
			Sleep(500);
			//recv
			Header *head = RecvPacket();
			PacketHandler(head->id, head->data);
		//}
		CloseHandle(hPipe);
	}


}

bool RLKTAuth::isValidated()
{
	


	return true;
}