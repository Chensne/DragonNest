#pragma once
#include <string>


using namespace std;

class Packetizer
{
private:
     int    i_index;
     char*  c_buffer;

public:
    Packetizer(void)
		: i_index(0), c_buffer(NULL)
    {
    }

    Packetizer(char* buffer)
		: i_index(0), c_buffer(buffer)
    {
    }

    void SetBufferSize(char*);
    void SetBufferIndex(int position) ;
    void SetBytes(char);
    void SetBytes(int);
    void SetBytes(unsigned int);
    void SetBytes(string);
    void SetBytes(int,string);
    int GetBufferSize();
    int GetBytes();
    unsigned int GetBytes(int);
    int  GetHexCode();
    string  GetString(int);
    //~Packetizer(void);
};

