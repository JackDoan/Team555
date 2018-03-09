//
// Created by mdl150330 on 3/6/2018.
//

#include "Serial.h"
#include "MotorDriver.h"
#include <string.h>
#include "stdio.h"

MotorDriver::MotorDriver() {

}
MotorDriver::~MotorDriver() = default;

bool MotorDriver::initComPort(char comPort, char axis) {
    char temp[12] = "\\\\.\\COM0";
    if (axis == 'x') {
        temp[11] = comPort;
        SPx = new Serial(temp);
        if (SPx->IsConnected()) {
            printf("ComPort: %c, axis %c, has connected\n", comPort, axis);
            return true;
        } else {
            printf("Comport: %c, axis %c, failed to connect!!", comPort, axis);
            return false;
        }
    } else if (axis == 'y') {
        temp[11] = comPort;
        SPy = new Serial(temp);
        if (SPy ->IsConnected()) {
            printf("ComPort: %c, axis %c, has connected\n", comPort, axis);
            return true;
        } else {
            printf("Comport: %c, axis %c, failed to connect!!", comPort, axis);
            return false;
        }
    } else {
        printf("axis: %c, does not exist.\n", axis);
        return false;
    }


}

//MotorDriver::MotorDriver();
//MotorDriver::~MotorDriver() = default;

//bool MotorDriver::initComPort(int comPort, char axis) {
//    std::string name = "\\\\\\\\.\\\\COM";
//    if (axis == 'x') {
//        name.append(std::to_string(comPort));
//        Serial* SPx = new Serial::Serial(name);
//    }
//    if (axis == 'y') {
//        name.append(std::to_string(comPort));
//    }
//
//}


int MotorDriver::getSteps(char axis) {
    if (axis == 'x') {
        writeResult = SPx->WriteData("???", sizeof("???"));
        if (!writeResult) {
            printf("Error writing to axis: %c\n", axis);
            return -2147483648;
        }
        readResult = SPx->ReadData(incomingData, dataLength);
        if (!readResult) {
            printf("Error reading from axis: %c\n", axis);
            return -2147483648;
        } else {
            //TODO: parse incoming 'string' data to an int and return it
        }
    } else if (axis == 'y') {
        writeResult = SPy->WriteData("???", sizeof("???"));
        if (!writeResult) {
            printf("Error writing to axis: %c\n", axis);
            return -2147483648;
        }
        readResult = SPy->ReadData(incomingData, dataLength);
        if (!readResult) {
            printf("Error reading from axis: %c\n", axis);
            return -2147483648;
        } else {
            //TODO: parse incoming 'string' data to an int and return it
        }
    } else {
        printf("Inavlid Axis identifier in MotorDrivers\n");
        return -2147483648;
    }
}

bool MotorDriver::moveSteps(int steps, char axis) {
    if (axis == 'x') {
        //TODO: xCom port 'move' command with steps appended to the end and parse result and return as an int
    } else if (axis == 'y') {
        //TODO: yCom port 'steps?' command and parse result and return as an int
    } else {
        printf("Inavlid Axis identifier in MotorDrivers\n");
        return false;
    }
}