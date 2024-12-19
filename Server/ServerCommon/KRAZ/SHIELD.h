#ifndef __SHIELD_H__
#define __SHIELD_H__

bool Atz_Checker_V1(void);
bool Atz_Checker_V2(int _iGameID, char *_szReservedFields, unsigned short _wTime = 0);
bool Atz_Checker_Result(int _iGameID, char *_szReservedFields, unsigned short _wTime = 0);

#endif
