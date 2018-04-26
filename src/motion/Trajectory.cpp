//
// Created by mike on 4/20/18.
//

#include <opencv2/core/types.hpp>
#include "../../inc/motion/Trajectory.h"
#include "../../inc/Table.h"
#include "../../inc/GameState.h"

const int Trajectory::vectorMult = 15;

std::vector<bool> Trajectory::bounceDetect(const cv::Point_<int>& startPoint, const cv::Point_<int>& endPoint, const int& bnccnt) {

    std::vector<bool> output = {false, false, false, false};
    int truecnt = 0;


    if (endPoint.x < Table::min.x) {
        output[0] = true;
        truecnt++;
//         printf("0\n");
    } else if (endPoint.x > Table::max.x) {
        output[2] = true;
        truecnt++;
//         printf("2\n");
    }

    // if there is an intersect with a horizontal wall (bottom and top walls)
    if (endPoint.y > Table::max.y) {
        output[1] = true;
        truecnt++;
//         printf("1\n");
    } else if (endPoint.y < Table::min.y) {
        output[3] = true;
        truecnt++;
//         printf("3\n");
    }

    // need to check if both a vertical and horizontal bounce are detected

    if (truecnt > 2) {
        printf("bounceDetect error, truecnt >2\n");
        return {false, false, false, false};
    }
    if (truecnt > 1) {
        // checking the top left condition
        if (output[0] && output[3]) {
            int xdif = startPoint.x - Table::min.x;
            int xvelo = abs(endPoint.x - startPoint.x);
            double xtime = (double)xdif/(double)xvelo;
            int ydif = startPoint.y - Table::min.y;
            int yvelo = abs(endPoint.y - startPoint.y);
            double ytime = (double)ydif/(double)yvelo;

            if (ytime > xtime) {
                output[3] = false;
            } else {
                output[0] = false;
            }
        }
        // checking the bottom left condition
        if (output[0] && output[1]) {
            int xdif = startPoint.x - Table::min.x;
            int xvelo = abs(endPoint.x - startPoint.x);
            double xtime = (double)xdif/(double)xvelo;
            int ydif = Table::max.y - startPoint.y;
            int yvelo = abs(endPoint.y - startPoint.y);
            double ytime = (double)ydif/(double)yvelo;
            if (ytime > xtime) {
                output[1] = false;
            } else {
                output[0] = false;
            }
        }
        // checking the bottom right condition
        if (output[1] && output[2]) {
            int xdif = Table::max.x - startPoint.x;
            int xvelo = abs(endPoint.x - startPoint.x);
            double xtime = (double)xdif/(double)xvelo;
            int ydif = Table::max.y - startPoint.y;
            int yvelo = abs(endPoint.y - startPoint.y);
            double ytime = (double)ydif/(double)yvelo;
            if (ytime > xtime) {
                output[1] = false;
            } else {
                output[2] = false;
            }
        }
        // checking the top right condition
        if (output[2] && output[3]) {
            int xdif = Table::max.x - startPoint.x;
            int xvelo = abs(endPoint.x - startPoint.x);
            double xtime = (double)xdif/(double)xvelo;
            int ydif = startPoint.y - Table::min.y;
            int yvelo = abs(endPoint.y - startPoint.y);
            double ytime = (double)ydif/(double)yvelo;
            if (ytime > xtime) {
                output[3] = false;
            } else {
                output[2] = false;
            }
        }

    }
    return output;

}

const cv::Point_<int> Trajectory::findIntersection(const std::vector<bool>& bounces, const cv::Point_<int>& startPoint, const cv::Point_<int>& endPoint) {
    cv::Point_<int> intersection = {0, 0};
    double prediction[3];
    double determinant;
    prediction[0] = endPoint.y - startPoint.y;    //A
    prediction[1] = startPoint.x - endPoint.x;    //B
    prediction[2] = prediction[0]*startPoint.x + prediction[1]*startPoint.y;   //C

    // if we are intersecting wall 0 (left)
    if (bounces[0]) {
        determinant = prediction[0] * Table::walls[0][1] - Table::walls[0][0] * prediction[1];
        if (determinant != 0) {
            intersection.x = static_cast<int>((Table::walls[0][1] * prediction[2] - prediction[1] * Table::walls[0][2])/determinant);
            intersection.y = static_cast<int>((prediction[0] * Table::walls[0][2] - Table::walls[0][0] * prediction[2])/determinant);
        }
    }
    // if we are intersecting wall 1 (bottom)
    else if (bounces[1]) {
        determinant = prediction[0] * Table::walls[1][1] - Table::walls[1][0] * prediction[1];
        if (determinant != 0) {
            intersection.x = static_cast<int>((Table::walls[1][1] * prediction[2] - prediction[1] * Table::walls[1][2])/determinant);
            intersection.y = static_cast<int>((prediction[0] * Table::walls[1][2] - Table::walls[1][0] * prediction[2])/determinant);
         }
    }
    // if we are intersecting wall 2 (right)
    else if (bounces[2]) {
        determinant = prediction[0] * Table::walls[2][1] - Table::walls[2][0] * prediction[1];
        if (determinant != 0) {
            intersection.x = static_cast<int>((Table::walls[2][1] * prediction[2] - prediction[1] * Table::walls[2][2])/determinant);
            intersection.y = static_cast<int>((prediction[0] * Table::walls[2][2] - Table::walls[2][0] * prediction[2])/determinant);
        }
    }
    // if we are intersecting wall 3 (top)
    else if (bounces[3]) {
        determinant = prediction[0] * Table::walls[3][1] - Table::walls[3][0] * prediction[1];
        if (determinant != 0) {
            intersection.x = static_cast<int>((Table::walls[3][1] * prediction[2] - prediction[1] * Table::walls[3][2])/determinant);
            intersection.y = static_cast<int>((prediction[0] * Table::walls[3][2] - Table::walls[3][0] * prediction[2])/determinant);
        }
    }
    return intersection;
}

std::vector<std::vector<cv::Point_<int>>> Trajectory::calculate(GameState& gs) {
    // need to set left and right goal flags to false at the start of this
    // remove grabbed once done testing and debugging
    cv::Point_<int>intersect = {0, 0};
    cv::Point_<int> dist;
    cv::Point_<int> leftover;
    std::vector<std::vector<cv::Point_<int>>> trajs;
    int bnccnt = 0;
    int bnccntmax;
    if (GameStateManager::getLostCnt() > 0) {
        bnccntmax = 1;
    } else {
        bnccntmax = 3;
    }

    auto prediction = gs.puck.location + ((gs.puck.location - gs.puck.lastLocation)*vectorMult);
    std::vector<cv::Point_<int>> temp = {gs.puck.location, prediction};
    trajs.emplace_back(temp);
    while (bnccnt <= bnccntmax) {
        cv::Point_<int> intersection;
        cv::Point_<int> newEndPoint;
        // detect a bounce
        auto bounces = bounceDetect(trajs.back()[0], trajs.back()[1], bnccnt);
        // if all of bounces are false, set done = true and break
        if (bounces[0] || bounces[1] || bounces[2] || bounces[3]) {
            // determine from bounces which wall is going to be intersected
            // and calculate the intersection point
            intersection = findIntersection(bounces, trajs.back()[0], trajs.back()[1]);

            // determine whether or not this is a goal
            // depending on which goal is getting intersected
            if (bounces[0] || bounces[2]) {
                gs.goalFlag = Table::goals.detect(intersection, (trajs.back()[1].x - trajs.back()[0].x));
                if (Table::goals.detect(intersection, trajs.back()[1].x - trajs.back()[0].x) != Table::Goals::NO_GOAL) {
                    // set endPoint equal to intersection and break
                    trajs.back()[1] = intersection;
                    break;
                }
            }


            // determine distance from location to intersect point, this becomes the length of the current leg
            // subtract that from the original length and this becomes the length of the next leg
            dist = {abs(intersection.x - trajs.back()[0].x), abs(intersection.y - trajs.back()[0].y)};
            leftover = {abs(trajs.back()[1].x - dist.x), abs(trajs.back()[1].y - dist.y)};
            //             determine ratios of leftover/total
            double magorig = sqrt(pow(trajs.back()[1].x - trajs.back()[0].x, 2) +
                                  pow(trajs.back()[1].y - trajs.back()[0].y, 2));
            double magclipped = sqrt(pow(intersection.x - trajs.back()[0].x, 2) +
                                     pow(intersection.y - trajs.back()[0].y, 2));
            double magrat = magclipped / magorig;

            //emplace back next trajs, which has start point at the point of intersect
            if (bounces[0] || bounces[2]) {
                newEndPoint.x = (int)(round((trajs.back()[0].x - trajs.back()[1].x) * (1 - magrat))) + intersection.x;
                newEndPoint.y = (int)(round((trajs.back()[1].y - trajs.back()[0].y) * (1 - magrat))) + intersection.y;
            } else if (bounces[1] || bounces[3]) {
                newEndPoint.x = (int)(round((trajs.back()[1].x - trajs.back()[0].x) * (1 - magrat))) + intersection.x;
                newEndPoint.y = (int)(round((trajs.back()[0].y - trajs.back()[1].y) * (1 - magrat))) + intersection.y;
            }
            // and appropriate endpoint

            // set trajs.back()[1] to the point of intersection
            trajs.back()[1] = intersection;
            std::vector<cv::Point_<int>> pts = {intersection, newEndPoint};
            trajs.emplace_back(pts);
        }
        bnccnt++;
    }

    if(trajs.size() >= bnccntmax+1 && GameStateManager::getLostCnt() < 1) { ///if we have an 'extra' bounce, clip it
    std::vector<std::vector<cv::Point_<int>>> toReturn = {trajs[0],trajs[1],trajs[2],trajs[3]};
    return toReturn;
    }
    else {
    return trajs;
    }
}

cv::Point_<int> Trajectory::predictLocation(GamePiece& gp, int frames) {
    cv::Point_<int> dist;
    cv::Point_<int> leftover;
    std::vector<std::vector<cv::Point_<int>>> trajs;
    int bnccnt = 0;
    int bnccntmax;
    if (GameStateManager::getLostCnt() > 0) {
        bnccntmax = 1;
    } else {
        bnccntmax = 3;
    }

    auto prediction = gp.location + ((gp.location - gp.lastLocation)*frames);
    std::vector<cv::Point_<int>> temp = {gp.location, prediction};
    trajs.emplace_back(temp);

    while (bnccnt <= bnccntmax) {
        std::vector<bool> bounces = {false, false, false, false};
        cv::Point_<int> intersection;
        cv::Point_<int> newEndPoint;
        // detect a bounce
        bounces = bounceDetect(trajs.back()[0], trajs.back()[1], bnccnt);
        if (bounces[0] || bounces[1] || bounces[2] || bounces[3]) {
            intersection = findIntersection(bounces, trajs.back()[0], trajs.back()[1]);
            if (bounces[0] || bounces[2]) {
                if (Table::goals.detect(intersection, trajs.back()[1].x - trajs.back()[0].x) != Table::Goals::NO_GOAL) {
                    // set endPoint equal to intersection and break
                    trajs.back()[1] = intersection;
                    break;
                }
            }
            dist = cvPoint(abs(intersection.x - trajs.back()[0].x),
                           abs(intersection.y - trajs.back()[0].y));
            leftover = cvPoint(abs(trajs.back()[1].x - dist.x),
                               abs(trajs.back()[1].y - dist.y));
            double magorig = sqrt(pow(trajs.back()[1].x - trajs.back()[0].x, 2) +
                                  pow(trajs.back()[1].y - trajs.back()[0].y, 2));
            double magclipped = sqrt(pow(intersection.x - trajs.back()[0].x, 2) +
                                     pow(intersection.y - trajs.back()[0].y, 2));
            double magrat = magclipped / magorig;
            if (bounces[0] || bounces[2]) {
                newEndPoint.x = (int)(round((trajs.back()[0].x - trajs.back()[1].x) * (1 - magrat))) + intersection.x;
                newEndPoint.y = (int)(round((trajs.back()[1].y - trajs.back()[0].y) * (1 - magrat))) + intersection.y;
            } else if (bounces[1] || bounces[3]) {
                newEndPoint.x = (int)(round((trajs.back()[1].x - trajs.back()[0].x) * (1 - magrat))) + intersection.x;
                newEndPoint.y = (int)(round((trajs.back()[0].y - trajs.back()[1].y) * (1 - magrat))) + intersection.y;
            }
            trajs.back()[1] = intersection;
            std::vector<cv::Point_<int>> pts = {intersection, newEndPoint};
            trajs.emplace_back(pts);
        }
        bnccnt++;
    }

    if(trajs.size() >= bnccntmax+1 && GameStateManager::getLostCnt() < 1) { ///if we have an 'extra' bounce, clip it
        std::vector<std::vector<cv::Point_<int>>> toReturn = {trajs[0],trajs[1],trajs[2],trajs[3]};
        return toReturn.back()[1];
    }
    else {
        return trajs.back()[1];
    }
}
