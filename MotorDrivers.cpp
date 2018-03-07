//
// Created by mdl150330 on 3/6/2018.
//

#include "SerialClass.h"
#include "MotorDrivers.h"




int MotorDrivers::getSteps(char axis) {
    if (axis == 'x') {
        //TODO: send xCom port 'steps?' command and parse result and return as an int
    } else if (axis == 'y') {
        //TODO: yCom port 'steps?' command and parse result and return as an int
    } else {
        printf("Inavlid Axis identifier in MotorDrivers\n");
        return -2147483648;
    }
}

bool MotorDrivers::moveSteps(int steps, char axis) {
    if (axis == 'x') {
        //TODO: xCom port 'move' command with steps appended to the end and parse result and return as an int
    } else if (axis == 'y') {
        //TODO: yCom port 'steps?' command and parse result and return as an int
    } else {
        printf("Inavlid Axis identifier in MotorDrivers\n");
        return false;
    }
}