#pragma once
#include <string>
#include "ReceivedData.h"
class Transaction
{
public:
    // Returns a + b
    static __declspec(dllexport) double Add(double a, double b);
    static __declspec(dllexport) ReceivedData GetBalance(const char* AuthToken, char ServerID, const char* CherryID, unsigned int OrderID);
    static __declspec(dllexport) ReceivedData GetPurchase(const char* AuthToken, char ServerID, const char* CherryID, unsigned int OrderID, unsigned int ProductID, unsigned int UnitPrice, unsigned int Quantity, unsigned int TotalAmount, const char*  IPAddress);
    static __declspec(dllexport) ReceivedData GetSendGift(const char* AuthToken, char ServerID, const char* CherryID, unsigned int OrderID, unsigned int ProductID, unsigned int UnitPrice, unsigned int Quantity, unsigned int TotalAmount, const char*  ToCherryID, const char*  IPAddress);
    static __declspec(dllexport) ReceivedData GetCompleted(const char* AuthToken, char ServerID, const char* CherryID, unsigned int OrderID, unsigned int TransactionID);
    static __declspec(dllexport) ReceivedData GetCancel(const char* AuthToken, char ServerID, const char* CherryID, unsigned int OrderID, unsigned int ProductID, unsigned int TotalAmount);
    static __declspec(dllexport) ReceivedData GetStatus(const char* AuthToken, char ServerID, const char* CherryID, unsigned int OrderID, unsigned int ProductID, unsigned int TotalAmount);
    static __declspec(dllexport) ReceivedData GetRefund(const char* AuthToken, char ServerID, const char* CherryID, unsigned int OrderID, unsigned int TransactionID);
};


