#pragma once
#include <string>
#include "Packetizer.h"
using namespace std;
class  ReceivedData
{
private:
    char AuthID;
    int BytesBody;
    Packetizer packet;
    void GetHeader();
    void GetBalance(int bytesize);
    void GetPurchase(int bytesize);
    void GetSendGift(int bytesize);
    void GetCompleted(int bytesize);
    void GetCancel(int bytesize);
    void GetStatus(int bytesize);
    void GetRefund(int bytesize);
public:
    ReceivedData()
    {
        ServerID = 0;
        Connections = 0;
        ConditionID = 0;
        StatusID = 45;
        OrderID = 0;
        ProductID = 0;
        CherryCredits = 0;
        TransactionID = 0;
        UserBalance = 0;
		Messages = NULL;
        ValidPacket = false;
    };
    ReceivedData(char ConditionID, char* buffer, int bytesize);
    ~ReceivedData(void)
    {
    };

    int ServerID;
    int Connections;
    int ConditionID;
    int StatusID;
    unsigned int OrderID;
    unsigned int ProductID;
    unsigned int CherryCredits;
    unsigned int TransactionID;
    unsigned int UserBalance;
    char* Messages;
    bool ValidPacket;

};

