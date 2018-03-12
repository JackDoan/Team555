//
// Created by jes130330 on 3/8/2018.
//

#ifndef TEAM555_CONFIG_H
#define TEAM555_CONFIG_H



#include "Corners.h"
#include <opencv2/opencv.hpp>
#include <vector>

class Config {

public:
    void WriteToFile();
//    Config();
//    ~Config();

    bool writeValues(Corners corners);
    bool readValues(Corners corners);
};


#endif //TEAM555_CONFIG_H
