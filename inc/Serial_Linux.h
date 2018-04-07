//
// Created by jack on 4/6/18.
//

#ifndef TEAM555_SERIAL_LINUX_H
#define TEAM555_SERIAL_LINUX_H

#include <stdio.h>
#include <stdlib.h>

class Serial
{
private:
    //Serial comm handler
    int hSerial;
    //Connection status
    bool connected;


public:
    //Initialize Serial communication with the given COM port
    Serial(const char *portName);
    //Close the connection
    ~Serial();
    //Read data in a buffer, if nbChar is greater than the
    //maximum number of bytes available, it will return only the
    //bytes available. The function return -1 when nothing could
    //be read, the number of bytes actually read.
    int ReadData(char *buffer, unsigned int nbChar);
    //Writes data from a buffer through the Serial connection
    //return true on success.
    bool WriteData(const char *buffer, unsigned int nbChar);
    //Check if we are actually connected
    bool IsConnected();
    int Available();



};

#endif //TEAM555_SERIAL_LINUX_H
