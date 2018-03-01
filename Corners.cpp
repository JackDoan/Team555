//
// Created by mikel on 2/23/2018.
//
#include "Corners.h"
#include "Camera.h"
#include "Table.h"
#include "puck.h"

//std::vector<cv::Point_<int>> corners;



void setupGUI() {

}

static void Corners::calibrateCorners(cv::Mat in, cv::Mat previewSmall, Table table) {
    std::vector<cv::Point_<int>> corners;
    std::vector<cv::Point_<int>> offsets;
    corners = Puck::findPucks(in, table);
}
/* TODO: need some sort of calibrate process that gets called when the user pressed the calibrate corner button on the debug GUI
    Needs to call puck.findpucks and set the corners vector to findpucks' return value
    Then pop up with the little GUI and show the border to be used with the defualt
    offset values (defined in the header file 40px outware for each square)
    then update the square drawn after resetting the offsets with setOffsets
    every time the user changes one of the settings in the little prompt

    Needs to do a final call to setOffsets after user clicks the 'finalize' button
 */
void drawSquare(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector, std::vector<cv::Point_<int>> offsetsVector) {
    cv::Point_<int> zero; zero.x = cornersVector[0].x + offsetsVector[0].x; zero.y = cornersVector[0].y + offsetsVector[0].y;
    cv::Point_<int> one; one.x = cornersVector[1].x + offsetsVector[1].x; one.y = cornersVector[1].y + offsetsVector[1].y;
    cv::Point_<int> two; two.x = cornersVector[2].x + offsetsVector[2].x; two.y = cornersVector[2].y + offsetsVector[2].y;
    cv::Point_<int> three; three.x = cornersVector[3].x + offsetsVector[3].x; three.y = cornersVector[3].y +offsetsVector[3].y;


    /*cv::line(previewSmall, cornersVector[0] / 2, cornersVector[1] / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, cornersVector[1] / 2, cornersVector[3] / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, cornersVector[3] / 2, cornersVector[2] / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, cornersVector[2] / 2, cornersVector[0] / 2, cv::Scalar(255, 255, 255), 4);*/

    cv::line(previewSmall, zero / 2, one / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, one / 2, three / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, three / 2, two / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, two / 2, zero / 2, cv::Scalar(255, 255, 255), 4);

}

void Corners::setCorners(std::vector<cv::Point_<int>> cornersVector, std::vector<cv::Point_<int>> offsetsVector) {
    for (int i = 0; i <= 3; i++) {
        corners[i] = cornersVector[i] + offsetsVector[i];
    }
}

std::vector<cv::Point_<int>> Corners::getCorners() {
    return corners;
}

void Corners::setOffsets(std::vector<cv::Point_<int>> offsetsVector) {
    for (int i = 0; i <= 3; i++) {
        offsets[i] = offsetsVector[i];
    }
}

std::vector<cv::Point_<int>> Corners::getOffsets() {
    return offsets;
}