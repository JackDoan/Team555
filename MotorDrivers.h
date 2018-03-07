//
// Created by mdl150330 on 3/6/2018.
//

#ifndef TEAM555_DRIVERINTERFACE_H
#define TEAM555_DRIVERINTERFACE_H
#include "SerialClass.h"

class MotorDrivers {
private:

public:
    Serial* SPx;
    short xCom = 4;
    Serial* SPy;
    short yCom = 5;
    char incomingData[256];
    int dataLength = 255;
    int readResult;
    bool writeResult;

    MotorDrivers();
    ~MotorDrivers();
    int getSteps(char axis);
    bool moveSteps(int steps, char axis);
};


#endif //TEAM555_DRIVERINTERFACE_H
