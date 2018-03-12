//
// Created by jes130330 on 3/8/2018.
//
#include "Corners.h"
#include "Config.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include<iostream>
#include<fstream>


bool Config::writeValues(Corners corners) {
    std::fstream configFile;
    char buffer[50];
    std::vector<cv::Point_<int>> cornersVector = corners.getCorners();
    std::vector<cv::Point_<int>> offsetsVector = corners.getOffsets();
    configFile.open("C:/Users/mikel/blah.txt");
    if (configFile.is_open()) {
        printf("File Opened Succesfully");
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
        printf("Wrote values to config file successfully\n");
        configFile.close();
        return true;
    } else {
        printf("Could not open config file\n");
        return false;
    }

}

bool Config::readValues(Corners corners) {
    std::fstream configFile;
    char buffer[500];
    std::vector<cv::Point_<int>> cornersVector;
    std::vector<cv::Point_<int>> offsetsVector;
    configFile.open("C:/Users/mikel/blah.txt");
    if (configFile.is_open()) {
        printf("File Opened Succesfully");
        while (!configFile.eof()) {
            configFile >> buffer;
        }
        configFile.close();

        //TODO: parse results, call setcorners setoffsets and setcalibrated coerners on the parsed results that get put into cornersvector and offsetsvector
    } else {
        printf("Could not open config file\n");
    }
}

void Config::WriteToFile(){

    const std::string myfile = "C://AirHockeyRobot/js_calib_test.txt";
    std::fstream outputFile(myfile);
    outputFile.open(myfile, std::ios_base::out);

    if(outputFile.bad()){
        printf("file exists.\n");
    }

    //Printing coords to txt file ------ TODO: needs more editing, writing more than expected.
    if (outputFile.is_open()) {
            bool isEmpty(true);     //will use to check if there are already calib points in file
            std::string line;
            while(outputFile >> line)
                isEmpty = false;
            printf("Is file empty (before input given)?: %s\n",isEmpty ? "true" : "false");

            //tempCorners2 = getCalibratedCorners();

            outputFile.clear();
            outputFile << "writing" << std::endl;

//            for (int i = 0; i < 4; i++) {
//                //outputFile << "Corner " <<  i+1 << ": " << CalibratedCorners[i] << std::endl;
//                //outputFile << "Corner " << i+1 << ": " << corners[i] << std::endl;
//            }
            //for (int j = 0; j < 4; j++) {
             //   outputFile << "\nCalibrated Corner " <<  j+1 << ": " << CalibratedCorners[j];
            //}
            printf("File was opened\n");
            outputFile.close();
    } else printf("unable to open file.\n");
}