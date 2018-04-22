//
// Created by jes130330 on 3/8/2018.
//
#include "../inc/Corners.h"
#include "../inc/Config.h"
#include "../inc/Settings.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

bool Settings::video_output = false;   //can be shown through Idle Process
bool Settings::undistort = true;
bool Settings::calibrateCorners = false;
bool Settings::preview = true;

struct threshold_s Settings::puckLimits = threshold_s(false);
struct threshold_s Settings::malletLimits = threshold_s(true);
bool Settings::threadFindingThings = puckLimits.debug & malletLimits.debug;
bool Settings::network_video = true;

bool writeConfigValues(Table::Corners corners) {
    std::fstream configFile;
    char buffer[50];
    std::vector<cv::Point_<int>> cornersVector = corners.getCorners();
    std::vector<cv::Point_<int>> offsetsVector = corners.getOffsets();
    //configFile.open("C:/Users/mdl150330/blah.txt");
    configFile.open("../config.txt");
    if (configFile.is_open()) {
//        printf("File Opened Succesfully\n");
        if (cornersVector.size() == 4) {
            for (int i = 0; i <= 3; i++) {
                std::sprintf(buffer, "Calibrated %d: %d, %d\n", i+1, cornersVector[i].x + offsetsVector[i].x, cornersVector[i].y + offsetsVector[i].y);
                configFile << buffer;
            }
            for (int i = 0; i <= 3; i++) {
                std::sprintf(buffer, "Corner %d: %d, %d\n", i+1, cornersVector[i].x, cornersVector[i].y);
                configFile << buffer;
            }
            for (int i = 0; i <=3; i++) {
                std::sprintf(buffer, "Offset %d: %d, %d\n", i + 1, offsetsVector[i].x, offsetsVector[i].y);
                configFile << buffer;
            }
//            printf("Wrote values to config file successfully\n");
            configFile.close();
            return true;
        } else {
//            printf("Not enough corners identified to write to file\n");
            return false;
        }
    } else {
//        printf("Could not open config file\n");
        return false;
    }

}



