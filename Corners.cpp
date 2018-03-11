//
// Created by mikel on 2/23/2018.
//
#include "Corners.h"
#include "Camera.h"
#include "Table.h"
#include "puck.h"


//std::vector<cv::Point_<int>> corners;





Corners::Corners() {

}

Corners::~Corners() = default;


 //TODO: set the offsets from a config file, right now it defaults to 40px out from each corner in each dimension as defined in the header file
void Corners::calibrateCorners(cv::Mat in, cv::Mat previewSmall, Table table, Puck puck) {
    tempCorners = puck.findPucks(in, table);

    //printf("tempCorners size: %d\n", tempCorners.size());
    if (tempCorners.size() == 4) {
        drawSquare(previewSmall, tempCorners, getOffsets());
//        drawSquare(previewSmall, tempCorners, getOffsets());
        drawLabels(previewSmall, tempCorners);
        setCorners(tempCorners);
//        printf("4 Corners Identified!\n1: (%d, %d)\n2: (%d, %d)\n3: (%d, %d)\n4: (%d, %d)\n",
//               corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y,
//               corners[3].x, corners[3].y);

        printf("4 Corners Identified!\t1: (%d, %d)\t2: (%d, %d)\t3: (%d, %d)\t4: (%d, %d)\n",
               corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y,
               corners[3].x, corners[3].y);

        //setOffsets(tempCorners);
        setCalibratedCorners();

        // printf("Enter Y to save these values, enter N to continue, enter X to exit");
        // TODO: save values when user enters Y, continue if N and exit program if X

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

void Corners::drawSquare(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector, std::vector<cv::Point_<int>> offsetsVector) {
    cv::Point_<int> zero; zero.x = cornersVector[0].x + offsetsVector[0].x; zero.y = cornersVector[0].y + offsetsVector[0].y;
    cv::Point_<int> one; one.x = cornersVector[1].x + offsetsVector[1].x; one.y = cornersVector[1].y + offsetsVector[1].y;
    cv::Point_<int> two; two.x = cornersVector[2].x + offsetsVector[2].x; two.y = cornersVector[2].y + offsetsVector[2].y;
    cv::Point_<int> three; three.x = cornersVector[3].x + offsetsVector[3].x; three.y = cornersVector[3].y +offsetsVector[3].y;

    cv::line(previewSmall, zero / 2, one / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, one / 2, three / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, three / 2, two / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, two / 2, zero / 2, cv::Scalar(255, 255, 255), 4);

}

void Corners::setCorners(std::vector<cv::Point_<int>> cornersVector) {
    /*for (int i = 0; i <= 3; i++) {
        corners[i] = cornersVector[i];
    }*/
    const int size = 4;
    //std::vector<cv::Point_<int>> fcorners[size];
        //fcorners:     Top Left:       1 (0)       //need to double check
        //              Top Right:      3 (2)       //BL actually
        //              Bottom Left:    1 (0)
        //              Bottom Right:   2 (1)

    //std::vector<cv::Point_<int>> hold
//    for (int i = 0; i < 4; i++) {
//        tempXa[i].x = corners[i].x;
//        tempYa[i] = corners[i];
//    }
//    int T1_flag = 0;    //highest
//    int T2_flag = 0;    //2nd highest
//    int B1_flag = 0;    //2nd Lowest
//    int B2_flag = 0;    //Lowest
//
//    std::sort(tempXa.begin(),tempXa.end());     //can switch to quicksort() if we need efficiency
//    std::sort(tempYa.begin(),tempYa.end());
//                                                    //Assuming Top(+) Left(+) and Bottom(-) Right(-)
//    for (int l = 0; l < 4; l++){                    //l=0:Left_far, l=1:Left_short, l=2:Right_short, 1=3:Right_far
//        for (int t = 0; t < 4; t++){                //l=0:Top_far, l=1:Top_short, l=2:Bottom_short, 1=3:Bottom_far
//            if(tempXa[l].x == corners[t].x){
//                switch(l){
//                    case 0  :       //Highest t
//                        fcorners[l].x = tempXa[0].x;        //zeros can be replaced with l & vice versa
//                        if(corners[t].y == tempYa[0].y) {        //Furthest Left and Highest
//                            fcorners[0].y = tempYa[0].y;
//                            T1_flag = 1;
//                        }
//                        else if(corners[t].y == tempYa[1].y) {   //Furthest Left and second highest etc..
//                            fcorners[0].y = tempYa[1].y;
//                            T2_flag = 1;
//                        }
//                        else if(corners[t].y == tempYa[2].y) {
//                            fcorners[0].y = tempYa[2].y;
//                            B1_flag = 1;
//                        }
//                        else if(corners[t].y == tempYa[3].y) {
//                            fcorners[0].y = tempYa[3].y;
//                            B2_flag = 1;
//                        }
//                        break;
//                    case 1  :
//                        fcorners[l].x = tempXa[1].x;
//                        if(T1_flag == 0 && corners[t].y == tempYa[0].y) {        //Furthest Left and Highest
//                            fcorners[0].y = tempYa[0].y;
//                            T1_flag = 1;
//                        }
//                        else if(T2_flag == 0 && corners[t].y == tempYa[1].y) {   //Furthest Left and second highest etc..
//                            fcorners[0].y = tempYa[1].y;
//                            T2_flag = 1;
//                        }
//                        else if(B1_flag == 0 && corners[t].y == tempYa[2].y) {
//                            fcorners[0].y = tempYa[2].y;
//                            B1_flag = 1;
//                        }
//                        else if(B2_flag == 0 && corners[t].y == tempYa[3].y) {
//                            fcorners[0].y = tempYa[3].y;
//                            B2_flag = 1;
//                        }
//                        break;
//                    case 2  :
//                        fcorners[l].x = tempXa[2].x;
//                        if(T1_flag == 0 && corners[t].y == tempYa[0].y) {        //Furthest Left and Highest
//                            fcorners[0].y = tempYa[0].y;
//                            T1_flag = 1;
//                        }
//                        else if(T2_flag == 0 && corners[t].y == tempYa[1].y) {   //Furthest Left and second highest etc..
//                            fcorners[0].y = tempYa[1].y;
//                            T2_flag = 1;
//                        }
//                        else if(B1_flag == 0 && corners[t].y == tempYa[2].y) {
//                            fcorners[0].y = tempYa[2].y;
//                            B1_flag = 1;
//                        }
//                        else if(B2_flag == 0 && corners[t].y == tempYa[3].y) {
//                            fcorners[0].y = tempYa[3].y;
//                            B2_flag = 1;
//                        }
//                        break;
//                    case 3  :
//                        fcorners[l].x = tempXa[3].x;
//                        if(T1_flag == 0 && corners[t].y == tempYa[0].y) {        //Furthest Left and Highest
//                            fcorners[0].y = tempYa[0].y;
//                            T1_flag = 1;
//                        }
//                        else if(T2_flag == 0 && corners[t].y == tempYa[1].y) {   //Furthest Left and second highest etc..
//                            fcorners[0].y = tempYa[1].y;
//                            T2_flag = 1;
//                        }
//                        else if(B1_flag == 0 && corners[t].y == tempYa[2].y) {
//                            fcorners[0].y = tempYa[2].y;
//                            B1_flag = 1;
//                        }
//                        else if(B2_flag == 0 && corners[t].y == tempYa[3].y) {
//                            fcorners[0].y = tempYa[3].y;
//                            B2_flag = 1;
//                        }
//                    break;
//                }
//            }
//        }
//
//    }
//    corners = fcorners;
//
    corners = cornersVector;
}

std::vector<cv::Point_<int>> Corners::getCorners() {
    return corners;
}
/*std::vector<cv::Point_<int>> Corners::getCorners(cv::Mat in, cv::Mat previewSmall, Table table, Puck puck){
 * tempCorners = puck.findPucks(in, table);

    //printf("tempCorners size: %d\n", tempCorners.size());
    if (tempCorners.size() == 4) {
        drawSquare(previewSmall, tempCorners, getOffsets());
//        drawSquare(previewSmall, tempCorners, getOffsets());
        drawLabels(previewSmall, tempCorners);
        setCorners(tempCorners);
        printf("4 Corners Identified!\n1: (%d, %d)\n2: (%d, %d)\n3: (%d, %d)\n4: (%d, %d)\n",
               corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y,
               corners[3].x, corners[3].y);

        // printf("Enter Y to save these values, enter N to continue, enter X to exit");

    } else {
        if (tempCorners.size() != 0) {
            drawLabels(previewSmall, tempCorners);
        } else {
            printf("CORNER DETECTION MODE - NO PUCKS DETECTED!\n");
        }
    }
 * return corners;
 * }
 */


void Corners::setOffsets(std::vector<cv::Point_<int>> offsetsVector) {
    /*for (int i = 0; i <= 3; i++) {
        offsets[i] = offsetsVector[i];
    }*/
    offsets = offsetsVector;
}

std::vector<cv::Point_<int>> Corners::getOffsets() {

    return offsets;
}


void Corners::setCalibratedCorners(){
    int tempx;
    int tempy;
    for (int z = 0; z <= 3; z++){
        tempx = tempCorners[z].x + offsets[z].x;
        tempy = tempCorners[z].y + offsets[z].y;
        CalibratedCorners.emplace_back(tempx, tempy);
    }
    printf("\n\nCorners Calibrated!\n1: (%d, %d)\t2: (%d, %d)\t3: (%d, %d)\t4: (%d, %d)\n",
           CalibratedCorners[0].x, CalibratedCorners[0].y, CalibratedCorners[1].x, CalibratedCorners[1].y,
           CalibratedCorners[2].x, CalibratedCorners[2].y, CalibratedCorners[3].x, CalibratedCorners[3].y);

}

std::vector<cv::Point_<int>> Corners::getCalibratedCorners() {

    return CalibratedCorners;
}

