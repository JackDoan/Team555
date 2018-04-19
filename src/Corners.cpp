//
// Created by mikel on 2/23/2018.
//
#include "../inc/Corners.h"
#include "../inc/Camera.h"
#include "../inc/Table.h"
#include "../inc/GamePieces/Puck.h"
#include "../inc/Config.h"
#include <fstream>
#include <string>
#include <iostream>


//std::vector<cv::Point_<int>> corners;





Corners::Corners(bool calibrate) {
    std::fstream configFile;
    char buffer[2000];
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
        setCalibratedCorners(calibratedCornersVector);
        if (!calibrate) {
            setCorners(cornersVector);
            setOffsets(offsetsVector);
            sortedX = getSortedX(getCalibratedCorners());
            sortedY = getSortedY(getCalibratedCorners());
        }
    } else {
        printf("Could not open config file\n");
    }
}

Corners::~Corners() = default;


 //TODO: set the offsets from a config file, right now it defaults to 40px out from each corner in each dimension as defined in the header file
void Corners::calibrateCorners(cv::Mat in, cv::Mat previewSmall, Table table, Puck puck) {
    tempCorners = puck.find(in);

    //printf("tempCorners size: %d\n", tempCorners.size());
    if (tempCorners.size() == 4) {
        drawSquareOld(previewSmall, tempCorners, getOffsets());
//        drawSquare(previewSmall, tempCorners, getOffsets());
        drawLabels(previewSmall, tempCorners);
        setCorners(tempCorners);
//        printf("4 Corners Identified!\n1: (%d, %d)\n2: (%d, %d)\n3: (%d, %d)\n4: (%d, %d)\n",
//               corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y,
//               corners[3].x, corners[3].y);

        printf("4 Corners Identified!\t1: (%d, %d)\t2: (%d, %d)\t3: (%d, %d)\t4: (%d, %d)\n",
               corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y,
               corners[3].x, corners[3].y);

        /* Do not call setCalibratedCorners() here anymore, this now gets set when running
         * the program outside of calibrate mode */


        //setOffsets(tempCorners);
//        setCalibratedCorners();
        // printf("Enter Y to save these values, enter N to continue, enter X to exit");

    } else {
        if (tempCorners.size() != 0) {
            drawLabels(previewSmall, tempCorners);
        } else {
            printf("CALIBRATE MODE - NO PUCKS DETECTED!\n");
        }

    }
}


void Corners::drawLabels(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector) {
    switch(tempCorners.size()) {
        case 1 : cv::putText(previewSmall, "1", tempCorners[0]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(255, 0, 255), 2, cv::LINE_8, false);
            //printf("Drawing labels: 1: (%d, %d)\n", tempCorners[0].x, tempCorners[0].y);
            break;
        case 2 : cv::putText(previewSmall, "1", tempCorners[0]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(255, 0, 255), 2, cv::LINE_8, false);
                 cv::putText(previewSmall, "2", tempCorners[1]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(255, 0, 0), 2, cv::LINE_8, false);
            //printf("Drawing labels: 1 2: (%d, %d), (%d, %d)\n", tempCorners[0].x, tempCorners[0].y, tempCorners[1].x, tempCorners[1].y);
            break;
        case 3 : cv::putText(previewSmall, "1", tempCorners[0]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(255, 0, 255), 2, cv::LINE_8, false);
                 cv::putText(previewSmall, "2", tempCorners[1]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(255, 0, 0), 2, cv::LINE_8, false);
                 cv::putText(previewSmall, "3", tempCorners[2]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(0, 255, 0), 2, cv::LINE_8, false);
            //printf("Drawing labels: 1 2 3: (%d, %d), (%d, %d), (%d, %d)\n", tempCorners[0].x, tempCorners[0].y, tempCorners[1].x, tempCorners[1].y, tempCorners[2].x, tempCorners[2].y);
            break;
        case 4 : cv::putText(previewSmall, "1", tempCorners[0]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(255, 0, 255), 2, cv::LINE_8, false);
                 cv::putText(previewSmall, "2", tempCorners[1]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(255, 0, 0), 2, cv::LINE_8, false);
                 cv::putText(previewSmall, "3", tempCorners[2]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(0, 255, 0), 2, cv::LINE_8, false);
                 cv::putText(previewSmall, "4", tempCorners[3]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                             2.8, cv::Scalar(0, 0, 255), 2, cv::LINE_8, false);
            //printf("Drawing labels: 1 2 3 4: (%d, %d), (%d, %d), (%d, %d), (%d, %d)\n", tempCorners[0].x, tempCorners[0].y, tempCorners[1].x, tempCorners[1].y, tempCorners[2].x, tempCorners[2].y, tempCorners[3].x, tempCorners[3].y);
            break;
        default:
            //printf("Can't see 1-4 pucks! I see %d pucks!\n", tempCorners.size());
            break;
    }

}

void Corners::drawSquareOld(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector, std::vector<cv::Point_<int>> offsetsVector) {
    cv::Point_<int> zero; zero.x = cornersVector[0].x + offsetsVector[0].x; zero.y = cornersVector[0].y + offsetsVector[0].y;
    cv::Point_<int> one; one.x = cornersVector[1].x + offsetsVector[1].x; one.y = cornersVector[1].y + offsetsVector[1].y;
    cv::Point_<int> two; two.x = cornersVector[2].x + offsetsVector[2].x; two.y = cornersVector[2].y + offsetsVector[2].y;
    cv::Point_<int> three; three.x = cornersVector[3].x + offsetsVector[3].x; three.y = cornersVector[3].y +offsetsVector[3].y;


    cv::line(previewSmall, zero / 2, one / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, one / 2, three / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, three / 2, two / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, two / 2, zero / 2, cv::Scalar(255, 255, 255), 4);

}

void Corners::drawSquareNew(cv::Mat previewSmall, std::vector<cv::Point_<int>> calibratedVector) {
//    std::sort (calibratedVector.begin(), calibratedVector.end());


//    printf("result: (%d, %d), (%d, %d), (%d, %d), (%d, %d)\n",
//           sortedY[0].x, sortedY[0].y,
//           sortedY[1].x, sortedY[1].y,
//           sortedY[2].x, sortedY[2].y,
//           sortedY[3].x, sortedY[3].y);
    cv::line(previewSmall, sortedX[0]/2, sortedX[1]/2, cv::Scalar(100, 200, 255), 4);
    cv::line(previewSmall, sortedX[2]/2, sortedX[3]/2, cv::Scalar(100, 200, 255), 4);
    cv::line(previewSmall, sortedY[0]/2, sortedY[1]/2, cv::Scalar(100, 200, 255), 4);
    cv::line(previewSmall, sortedY[2]/2, sortedY[3]/2, cv::Scalar(100, 200, 255), 4);

    //TODO:Drawing Goal Lines?
    //cv::Point_<int> midY1 = (sortedY[0]/2 - sortedY[1]/2)/2 + sortedY[0]/2;
    //cv::Point_<int> G_offset = midY1 + 0.5*((sortedY[0]/2 - sortedY[1]/2));
    //cv::line(previewSmall, midY1, G_offset, cv::Scalar(255,0,0), 4);
}

std::vector<cv::Point_<int>> Corners::getSortedX(std::vector<cv::Point_<int>> calibrated) {
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

std::vector<cv::Point_<int>> Corners::getSortedY(std::vector<cv::Point_<int>> calibrated) {
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

std::vector<cv::Point_<int>> getSortedY() {

}

void Corners::setCorners(std::vector<cv::Point_<int>> cornersVector) {
    /*for (int i = 0; i <= 3; i++) {
        corners[i] = cornersVector[i];
    }*/
    const int size = 4;

    corners = cornersVector;
}

std::vector<cv::Point_<int>> Corners::getCorners() {
    return corners;
}



void Corners::setOffsets(std::vector<cv::Point_<int>> offsetsVector) {
    /*for (int i = 0; i <= 3; i++) {
        offsets[i] = offsetsVector[i];
    }*/
    offsets = offsetsVector;
}

std::vector<cv::Point_<int>> Corners::getOffsets() {

    return offsets;
}

void Corners::setCalibratedCorners(std::vector<cv::Point_<int>> calibratedVector) {
    CalibratedCorners = calibratedVector;
}


std::vector<cv::Point_<int>> Corners::getCalibratedCorners() {

    return CalibratedCorners;
}

