//
// Created by jes130330 on 2/16/2018.
//

#include "MotorComm.h"
#include <iostream>
#include <windows.h>
using namespace std;

HANDLE createPipe(int pipeType) {
// Create a pipe to send data
    HANDLE xMotorCmd = CreateNamedPipe(
            (LPCSTR) R"(\\.\pipe\motors\x\cmd)", // name of the pipe
            PIPE_ACCESS_OUTBOUND, // 1-way pipe -- send only
            PIPE_TYPE_BYTE, // send data as a byte stream
            1, // only allow 1 instance of this pipe
            0, // no outbound buffer
            0, // no inbound buffer
            0, // use default wait time
            NULL // use default security attributes
    );

    if (xMotorCmd == NULL || xMotorCmd == INVALID_HANDLE_VALUE) {
        wcout << "Failed to create outbound pipe instance.";
        //todo look up error code here using GetLastError()
        //todo react?
    }
}

int testMessage(HANDLE pipe) {
    char test[] = "BEEP";
    int r = WriteFile(pipe, test, sizeof(test), NULL, NULL);
    return r;
}




