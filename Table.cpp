//
// Created by jad140230 on 1/21/2018.
//

#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include "Camera.h"
#include "Table.h"
#include <vector>


using namespace cv;


Table::Table(Camera camera) {
    //cam_center_x = CAM_PIX_WIDTH;
    //cam_center_y = CAM_PIX_HEIGHT;
    predict_x_old = -1;

    robot_table_center_y = robot_table_width / 2;
    robot_table_center_x = robot_table_length / 2;
    puckSize = robot_table_width / 20;  // puck size (radio) estimation
    defense_position = 60 + puckSize;  // Pusher defense position (for predictions)
//    std::vector<cv::Point_<int>> sortedX = corners.getSortedX(corners.getCalibratedCorners());
//    std::vector<cv::Point_<int>> sortedY = corners.getSortedY(corners.getCalibratedCorners());
//    min.x = sortedX[1].x;
//    max.x = sortedX[4].x;
//    min.y = sortedY[1].y;
//    max.y = sortedY[4].y;


//    max = Coordinate((int)(0.97*camera.getCenter().x), (int)(0.97* camera.getCenter().y));
    //max = Coordinate((int)(1*camera.getCenter().x), (int)(1* camera.getCenter().y));
//    min = Coordinate((int)(0.03*camera.getCenter().x), (int)(0.03*camera.getCenter().y));
    //min = Coordinate((int)(0*camera.getCenter().x), (int)(0*camera.getCenter().y));
    //table_pix_maxx =(int)((robot_table_center_y) / cam_pix_to_mm + cam_center_y) / 2;
    //table_pix_maxy = (int)((robot_table_center_x) / cam_pix_to_mm + cam_center_x) / 2;
    //table_pix_minx = (int)((robot_table_center_y - robot_table_length) / cam_pix_to_mm + cam_center_y) / 2;
    //table_pix_miny = (int)((robot_table_center_x - robot_table_width) / cam_pix_to_mm + cam_center_x) / 2;


}

void Table::annotate(cv::Mat frameGrabbed) {
    // Draw marks on image and center

    cv::Scalar color = cv::Scalar(100, 255, 200);

    cv::line(frameGrabbed, cvPoint(CAM_PIX_WIDTH / 4 - 5, CAM_PIX_HEIGHT / 4), cvPoint(CAM_PIX_WIDTH / 4 + 5, CAM_PIX_HEIGHT / 4), color, 1);
    cv::line(frameGrabbed, cvPoint(CAM_PIX_WIDTH / 4, CAM_PIX_HEIGHT / 4 - 5), cvPoint(CAM_PIX_WIDTH / 4, CAM_PIX_HEIGHT / 4 + 5), color, 1);
    //Draw table limits
    //cv::line(frameGrabbed, cvPoint(table_pixx1,table_pixy1), cvPoint(table_pixx2,table_pixy2), cvScalar(100,255,200), 1);
    //cv::line(frameGrabbed, cvPoint(table_pixx2,table_pixy2), cvPoint(table_pixx3,table_pixy3), cvScalar(100,255,200), 1);
    //cv::line(frameGrabbed, cvPoint(table_pixx3,table_pixy3), cvPoint(table_pixx4,table_pixy4), cvScalar(100,255,200), 1);
    //cv::line(frameGrabbed, cvPoint(table_pixx4,table_pixy4), cvPoint(table_pixx1,table_pixy1), cvScalar(100,255,200), 1);

//    cv::line(frameGrabbed, cvPoint(min.x, min.y), cvPoint(min.x + 20, min.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(min.x, max.y), cvPoint(min.x + 20, max.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(max.x - 20, min.y), cvPoint(max.x, min.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(max.x - 20, max.y), cvPoint(max.x, max.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(min.x, min.y), cvPoint(min.x, min.y + 20), color, 1);
//    cv::line(frameGrabbed, cvPoint(max.x, min.y), cvPoint(max.x, min.y + 20), color, 1);
//    cv::line(frameGrabbed, cvPoint(min.x, max.y - 20), cvPoint(min.x, max.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(max.x, max.y - 20), cvPoint(max.x, max.y), color, 1);

}

Table::~Table() = default;

void Table::setLimits(std::vector<cv::Point_<int>> sortedX, std::vector<cv::Point_<int>> sortedY) {
    min.x = sortedX[0].x; printf("Setting min X: %d\n", min.x);
    max.x = sortedX[3].x; printf("Setting max X: %d\n", max.x);
    min.y = sortedY[0].y; printf("Setting min Y: %d\n", min.y);
    max.y = sortedY[3].y; printf("Setting max Y: %d\n", max.y);
}
