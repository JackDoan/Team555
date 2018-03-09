//
// Created by mdl150330 on 3/6/2018.
//

#include "Serial.h"
#include "MotorDriver.h"
#include <string.h>
#include "stdio.h"

// TODO: test this

MotorDriver::MotorDriver() {

}
MotorDriver::~MotorDriver() = default;

bool MotorDriver::initComPort(char comPort, char axis) {
    char temp[] = "\\\\.\\COM0";
    if (axis == 'x') {
        temp[sizeof(temp) - 2] = comPort;
        SPx = new Serial(temp);
        if (SPx->IsConnected()) {
            printf("ComPort: %c, axis %c, has connected\n", comPort, axis);
            return true;
        } else {
            printf("Comport: %c, axis %c, failed to connect!!", comPort, axis);
            return false;
        }
    } else if (axis == 'y') {
        temp[sizeof(temp) - 2] = comPort;
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
        writeResult = SPx->WriteData("?", sizeof("???"));
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
        writeResult = SPy->WriteData("?", sizeof("???"));
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
    char bytes[4];
    bytes[0] = (steps >> 24) & 0xFF;
    bytes[1] = (steps >> 16) & 0xFF;
    bytes[2] = (steps >> 8) & 0xFF;
    bytes[3] = (steps) & 0xFF;
    if (axis == 'x') {
        writeResult = SPx->WriteData(bytes, sizeof(bytes));
        if (!writeResult) {
            printf("Error writing steps: %d, to axis %c\n", steps, axis);
            return false;
        }
        readResult = SPx->ReadData(incomingData, dataLength);
        if (!readResult) {
            printf("Error reading steps: %d, from axis %c\n", steps, axis);
            return false;
        } else {
            if (incomingData == bytes) {
                return true;
            } else {
                printf("Error, returned value in loopback did not match sent value\n");
                return false;
            }
        }


    } else if (axis == 'y') {
        writeResult = SPy->WriteData(bytes, sizeof(bytes));
        if (!writeResult) {
            printf("Error writing steps: %d, to axis %c\n", steps, axis);
            return false;
        }
        readResult = SPy->ReadData(incomingData, dataLength);
        if (!readResult) {
            printf("Error reading steps: %d, from axis %c\n", steps, axis);
            return false;
        } else {
            if (incomingData == bytes) {
                return true;
            } else {
                printf("Error, returned value in loopback did not match sent value\n");
                return false;
            }
        }
    } else {
        printf("Inavlid Axis identifier in MotorDrivers\n");
        return false;
    }
}