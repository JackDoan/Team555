//
// Created by mdl150330 on 3/6/2018.
//

#include "inc/Serial.h"
#include "inc/MotorDriver.h"
#include <string.h>
#include <cstdint>
#include <opencv2/core/types.hpp>
#include "stdio.h"
#include "inc/helpers.h"


typedef union {
    struct {
        char lowest:8;
        char low:8;
        char high:8;
        char highest:8;
    } b;
    long l;
} longbytes;

MotorDriver::MotorDriver() {
    initComPort('5');
}
MotorDriver::~MotorDriver() = default;

bool MotorDriver::initComPort(char comPort) {
    char portPath[] = R"(\\.\COM0)";
    portPath[sizeof(portPath) - 2] = comPort; //replace the COM port number
    auto comHandle = new Serial(portPath);
    bool toReturn = comHandle->IsConnected();
    if(toReturn) {
        printf("ComPort: %c has connected\n", comPort);
        SPb = comHandle;
    }
    else {
        printf("Comport: %c failed to connect!!", comPort);
    }
    return toReturn;
}

void MotorDriver::setEnable(bool x, bool y) {
    char bytes[6] = {0x69,'P',0,0,y,x};
    writeResult = SPb->WriteData(bytes, sizeof(bytes));
}

void MotorDriver::setHome() {
    char bytes[6] = {0x69,'H',0,0,0,0};
    writeResult = SPb->WriteData(bytes, sizeof(bytes));
}

bool MotorDriver::stop() {
    return sendCMD(0, '!');
}

bool MotorDriver::moveTo(const cv::Point_<int> &in) {
    return sendCMD(in.x, 'X') && sendCMD(in.y, 'Y');
}

bool MotorDriver::moveBy(const cv::Point_<int> &in) {
    return sendCMD(in.x, 'y') && sendCMD(in.y, 'y');
}

bool MotorDriver::sendCMD(int steps, char cmd) {
    longbytes toSend;
    toSend.l = steps;
    char bytes[7];
    bytes[0] = 0x69;
    bytes[1] = cmd;
    bytes[2] = toSend.b.highest;
    bytes[3] = toSend.b.high;
    bytes[4] = toSend.b.low;
    bytes[5] = toSend.b.lowest;
    bytes[6] = 0;
    SPb->WriteData(bytes, sizeof(bytes));
}

void MotorDriver::readAll() {
    char buffer[6] = {0};
    int result = SPb->ReadData(buffer, sizeof(buffer));
    if(result >= 6) {
        //we got a packet! process it.
    }
    else if(result >= 1) {
        //maybe save these bytes for later
        if(savedBufferLength >= 1) { //have we saved bytes before?
            processPacket(buffer);
        }
        else if(buffer[0] == 0x69) { //no saved bytes, but this is the start of a frame
            if(savedBufferLength == 0) {
                savedBufferLength = result;
                memcpy(savedBuffer,buffer,(size_t)result);
            }
            else if(savedBufferLength + result >= 6){ //we finished a packet!
                int x = 6-savedBufferLength;
                memcpy(savedBuffer+savedBufferLength, buffer, (size_t)(x));
                processPacket(savedBuffer);
                //now we need to save the leftovers
                savedBufferLength = result-x;
                memcpy(savedBuffer,&buffer[x],(size_t)savedBufferLength);
            }
            else { //we didn't finish a packet. We'll probably never get here.
                savedBufferLength += result;
                memcpy(savedBuffer,buffer,(size_t)result);
            }
        }
        else { //these bytes are trash, and we should probably invalidate our buffer
            savedBufferLength = 0;
        }
    }
}

int MotorDriver::processPacket(char input[6]) {
    int toReturn = -1;
    if(input[0] == 0x69) {
        longbytes data;
        data.b.highest = input[2];
        data.b.high = input[3];
        data.b.low = input[4];
        data.b.lowest = input[5];
        switch(input[1]) {
            case 'B':
                //rx.b.lowest = (white << 5) | (red << 4) | (maxX << 3) | (minX << 2) | (maxY << 1) | (minY);
                switches.c = data.b.lowest;
                thermal.driverX = data.b.low;
                thermal.driverY = data.b.high;
                thermal.extra = data.b.highest;
                break;
            case 'x':
                dbgPrint("MotorDriver::processPacket got %d and did %ld\n",input[1],data.l);
                break;
            default:
                dbgPrint("MotorDriver::processPacket invalid cmd: %d\n",input[1]);
                break;
        }
    }
    else {
        dbgPrint("MotorDriver::processPacket invalid header: %d\n",input[0]);
    }
    return toReturn;
}

long MotorDriver::getSteps(char axis) {
    char bytes[6] = {0x69,'?',0,0,0,axis};
    char buffer[6] = {0};
    writeResult = SPb->WriteData(bytes, sizeof(bytes));
    _sleep(10);
    readResult = SPb->ReadData(buffer, sizeof(buffer));
    if(readResult >= 6) {
        longbytes data;
        data.b.highest = buffer[2];
        data.b.high = buffer[3];
        data.b.low = buffer[4];
        data.b.lowest = buffer[5];
        return data.l;
    }
    else {
        return 8675309;
    }
}