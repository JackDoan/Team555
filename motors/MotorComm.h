//
// Created by jes130330 on 2/16/2018.
//

#ifndef TEAM555_MOTORCOMM_H
#define TEAM555_MOTORCOMM_H
#include <iostream>
#include <windows.h>

class MotorComm {

};

HANDLE createPipe(int pipeType);
int testMessage(HANDLE pipe);


#endif //TEAM555_MOTORCOMM_H
