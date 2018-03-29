//
// Created by mdl150330 on 3/6/2018.
//

#ifndef TEAM555_DRIVERINTERFACE_H
#define TEAM555_DRIVERINTERFACE_H

#include <opencv2/core/types.hpp>
#include "Serial.h"

typedef union switchesU {
    struct {
        int minY:1;
        int maxY:1;
        int minX:1;
        int maxX:1;
        int red:1;
        int white:1;
        int rsvd:2; //to fill a whole byte
    } b;
    char c;
} switches_t;

typedef struct thermal_s {
    char driverX;
    char driverY;
    char extra;
}thermal_t;


class MotorDriver {
private:
    int processPacket(char input[6]);
    int deadband = 5;
    MotorDriver();
public:
//    Serial* SPx = new Serial("\\\\.\\COM4");
//    Serial* SPy = new Serial("\\\\.\\COM5");
    static MotorDriver& getInstance() {
        static MotorDriver instance;
        return instance;
    }
    MotorDriver(MotorDriver const&)               = delete;
    void operator=(MotorDriver const&) = delete;
    Serial* SPb;
    switches_t switches;
    thermal_t thermal;
    char incomingData[256];
    char savedBuffer[6];
    int savedBufferLength = 0;
    int dataLength = 255;
    int readResult;
    bool writeResult;
    int parsed;
    bool debug = true;

    ~MotorDriver();
    bool initComPort(char comPort);
    void setHome();
    bool sendCMD(int steps, char axis);
    long getSteps(char axis);
    void setEnable(bool x, bool y);
    void sendRaw(char msg);
    void readAll();
    bool moveTo(const cv::Point_<int> &in);
    bool moveBy(const cv::Point_<int> &in);
    bool stop();
    void getButtonsCmd();
    long getButtonsResult();

};


#endif //TEAM555_DRIVERINTERFACE_H
