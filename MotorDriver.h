//
// Created by mdl150330 on 3/6/2018.
//

#ifndef TEAM555_DRIVERINTERFACE_H
#define TEAM555_DRIVERINTERFACE_H
#include "Serial.h"

class MotorDriver {
private:

public:
//    Serial* SPx = new Serial("\\\\.\\COM4");
//    Serial* SPy = new Serial("\\\\.\\COM5");
    Serial* SPx;
    Serial* SPy;
    char incomingData[256];
    int dataLength = 255;
    int readResult;
    bool writeResult;
    int parsed;

    MotorDriver();
    ~MotorDriver();
    bool initComPort(char comPort, char axis);
    int getSteps(char axis);
    bool moveSteps(int steps, char axis);
};


#endif //TEAM555_DRIVERINTERFACE_H
