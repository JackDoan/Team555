//
// Created by mikel on 2/23/2018.
//
#include "../inc/Corners.h"
#include "../inc/Camera.h"
#include "../inc/Table.h"
#include "../inc/Config.h"
#include <fstream>
#include <string>
#include <iostream>


Table::Corners::Corners(bool calibrate) {
    std::fstream configFile;
    std::vector<cv::Point_<int>> calibratedCornersVector;
    std::vector<cv::Point_<int>> cornersVector;
    std::vector<cv::Point_<int>> offsetsVector;
#ifdef WINDOWS
    configFile.open("C:/AirHockeyRobot/blahnew.txt");
#else
    configFile.open("/opt/Team555/config.txt");
#endif
    std::string blue;
    int j = 0;
    if (configFile.is_open()) {
        while (std::getline(configFile, blue)) {
            char xnumber[5];
            char ynumber[5];
            bool axisFlag = false;
            std::string temp;
            char buffer2[200];
            int k = 0;
            int n = 0;
            strcpy(buffer2, blue.c_str());
            temp = buffer2;
            if (!strncmp(buffer2, "Calibrated", strlen("Calibrated"))) {
                for (int m = temp.find(":"); buffer2[m] != '\0'; m++) {
                    if ((buffer2[m] >= 48 && buffer2[m] <= 57) || buffer2[m] == 45) {
                        if (!axisFlag) {
                            xnumber[n] = buffer2[m];
                            n++;
                        } else {
                            ynumber[n] = buffer2[m];
                            n++;
                            if (buffer2[m + 1] == '\0') {
                                ynumber[n] = '\0';
                            }
                        }
                    } else if (buffer2[m] == 44) {
                        xnumber[n] = '\0';
                        axisFlag = true;
                        n = 0;
                    }
                };
                calibratedCornersVector.emplace_back(atoi(xnumber), atoi(ynumber));
            }
            if (!strncmp(buffer2, "Corner", strlen("Corner"))) {
                for (int m = temp.find(":"); buffer2[m] != '\0'; m++) {
                    if ((buffer2[m] >= 48 && buffer2[m] <= 57) || buffer2[m] == 45) {
                        if (!axisFlag) {
                            xnumber[n] = buffer2[m];
                            n++;
                        } else {
                            ynumber[n] = buffer2[m];
                            n++;
                            if (buffer2[m + 1] == '\0') {
                                ynumber[n] = '\0';
                            }
                        }
                    } else if (buffer2[m] == 44) {
                        xnumber[n] = '\0';
                        axisFlag = true;
                        n = 0;
                    }
                };
                cornersVector.emplace_back(atoi(xnumber), atoi(ynumber));
            }
            if (!strncmp(buffer2, "Offset", strlen("Offset"))) {
                for (int m = temp.find(":"); buffer2[m] != '\0'; m++) {
                    if ((buffer2[m] >= 48 && buffer2[m] <= 57) || buffer2[m] == 45) {
                        if (!axisFlag) {
                            xnumber[n] = buffer2[m];
                            n++;
                        } else {
                            ynumber[n] = buffer2[m];
                            n++;
                            if (buffer2[m + 1] == '\0') {
                                ynumber[n] = '\0';
                            }
                        }
                    } else if (buffer2[m] == 44) {
                        xnumber[n] = '\0';
                        axisFlag = true;
                        n = 0;
                    }
                };
                offsetsVector.emplace_back(atoi(xnumber), atoi(ynumber));
            }
        }
        printf("File Opened Succesfully\n");

        if (!calibrate) {
            setCorners(cornersVector);
        }
    } else {
        printf("Could not open config file\n");
    }
}

Table::Corners::~Corners() = default;



void Table::Corners::drawSquareNew(cv::Mat previewSmall, std::vector<cv::Point_<int>> calibratedVector) {
//    std::sort (calibratedVector.begin(), calibratedVector.end());
    const int ratio = 1;

//    printf("result: (%d, %d), (%d, %d), (%d, %d), (%d, %d)\n",
//           sortedY[0].x, sortedY[0].y,
//           sortedY[1].x, sortedY[1].y,
//           sortedY[2].x, sortedY[2].y,
//           sortedY[3].x, sortedY[3].y);
    cv::line(previewSmall, sortedX[0]/ratio, sortedX[1]/ratio, cv::Scalar(100, 200, 255), 4);
    cv::line(previewSmall, sortedX[2]/ratio, sortedX[3]/ratio, cv::Scalar(100, 200, 255), 4);
    cv::line(previewSmall, sortedY[0]/ratio, sortedY[1]/ratio, cv::Scalar(100, 200, 255), 4);
    cv::line(previewSmall, sortedY[2]/ratio, sortedY[3]/ratio, cv::Scalar(100, 200, 255), 4);

    //TODO:Drawing Goal Lines?
    //cv::Point_<int> midY1 = (sortedY[0]/2 - sortedY[1]/2)/2 + sortedY[0]/2;
    //cv::Point_<int> G_offset = midY1 + 0.5*((sortedY[0]/2 - sortedY[1]/2));
    //cv::line(previewSmall, midY1, G_offset, cv::Scalar(255,0,0), 4);
}

std::vector<cv::Point_<int>> Table::Corners::getSortedX(std::vector<cv::Point_<int>> calibrated) {
    std::vector<cv::Point_<int>> sortedX = calibrated;
    cv::Point_<int> temp;
    for (int i = 0; i <= 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (sortedX[j+1].x <= sortedX[j].x) {
                temp = sortedX[j+1];
                sortedX[j+1] = sortedX[j];
                sortedX[j] = temp;
            }
        }
    }
    return sortedX;
}

std::vector<cv::Point_<int>> Table::Corners::getSortedY(std::vector<cv::Point_<int>> calibrated) {
    std::vector<cv::Point_<int>> sortedY = calibrated;
    cv::Point_<int> temp;
    for (int i = 0; i <= 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (sortedY[j+1].y <= sortedY[j].y) {
                temp = sortedY[j+1];
                sortedY[j+1] = sortedY[j];
                sortedY[j] = temp;
            }
        }
    }
    return sortedY;
}

void Table::Corners::setCorners(std::vector<cv::Point_<int>> cornersVector) {
    /*for (int i = 0; i <= 3; i++) {
        corners[i] = cornersVector[i];
    }*/
    const int size = 4;

    corners = cornersVector;
    sortedX = getSortedX(corners);
    sortedY = getSortedY(corners);
}

std::vector<cv::Point_<int>> Table::Corners::getCorners() {
    return corners;
}

