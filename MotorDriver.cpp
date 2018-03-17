//
// Created by mdl150330 on 3/6/2018.
//

#include "Serial.h"
#include "MotorDriver.h"
#include <string.h>
#include <cstdint>
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
    }  else if (axis == 'b') {
        temp[sizeof(temp) - 2] = comPort;
        SPb = new Serial(temp);
        if (SPb ->IsConnected()) {
            printf("ComPort: %c, axis %c, has connected\n", comPort, axis);
            return true;
        } else {
            printf("Comport: %c, axis %c, failed to connect!!", comPort, axis);
            return false;
        }
    }
    else {
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
            return atoi(incomingData);
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
            return atoi(incomingData);
        }
    } else {
        printf("Inavlid Axis identifier in MotorDrivers\n");
        return -2147483648;
    }
}

bool MotorDriver::moveSteps(long steps, char cmd) {
    char bytes[7];
    bytes[0] = 0x69;
    bytes[1] = cmd;
    bytes[2] = (steps >> 24) & 0xFF;
    bytes[3] = (steps >> 16) & 0xFF;
    bytes[4] = (steps >> 8) & 0xFF;
    bytes[5] = (steps) & 0xFF;
    bytes[6] = 0;
    writeResult = SPb->WriteData(bytes, sizeof(bytes));
    //readResult = SPb->ReadData(incomingData, dataLength);
//    if(dataLength >= 6) {
//        int toRead = (((long)incomingData[2] << 24) & 0xff000000) | (((long)incomingData[3] << 16) & 0x00ff0000 )| (((long)incomingData[4] << 8) & 0x0000ff00)| (((long)incomingData[5]) & 0x000000ff) ;
//        printf("%d\n", toRead);
////        printf("%c%c%ld\n",incomingData[0],incomingData[1],toRead);
//    }

    /*if (cmd == 'x') {
        writeResult = SPx->WriteData(bytes, sizeof(bytes));
        if (!writeResult) {
            printf("Error writing steps: %ld, to axis %c\n", steps, cmd);
            return false;
        }
        readResult = SPx->ReadData(incomingData, dataLength);
        if(readResult) {
            //printf(incomingData);
        }
        *//*if (!readResult) {
            printf("Error reading steps: %ld, from axis %c\n", steps, axis);
            return false;
        } else {
            if (incomingData == bytes) {
                return true;
            } else {
                printf("Error, returned value in loopback did not match sent value\n");
                return false;
            }
        }*//*


    } else if (cmd == 'y') {
        writeResult = SPy->WriteData(bytes, sizeof(bytes));
        if (!writeResult) {
            printf("Error writing steps: %ld, to axis %c\n", steps, cmd);
            return false;
        }
        readResult = SPy->ReadData(incomingData, dataLength);
        if (!readResult) {
            printf("Error reading steps: %ld, from axis %c\n", steps, cmd);
            return false;
        } else {
            if (incomingData == bytes) {
                return true;
            } else {
                printf("Error, returned value in loopback did not match sent value\n");
                return false;
            }
        }
    } else if (cmd == 'b') {
        writeResult = SPy->WriteData(bytes, sizeof(bytes));
        if (!writeResult) {
            printf("Error writing steps: %ld, to axis %c\n", steps, cmd);
            return false;
        }
        readResult = SPy->ReadData(incomingData, dataLength);
        if (!readResult) {
            printf("Error reading steps: %ld, from axis %c\n", steps, cmd);
            return false;
        } else {
            if (incomingData == bytes) {
                return true;
            } else {
                printf("Error, returned value in loopback did not match sent value\n");
                return false;
            }
        }
    }

    else {
        printf("Inavlid Axis identifier in MotorDrivers\n");
        return false;
    }*/
}

void MotorDriver::sendRaw(char msg) {
    writeResult = SPx->WriteData(&msg, 1);
    readResult = SPx->ReadData(incomingData, dataLength);
    if (readResult) {
        printf(incomingData);
    }
}