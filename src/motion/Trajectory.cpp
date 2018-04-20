//
// Created by mike on 4/20/18.
//

#include <opencv2/core/types.hpp>
#include "../../inc/motion/Trajectory.h"
#include "../GameState.h"

const int Trajectory::vectorMult = 15;

std::vector<std::vector<cv::Point_<int>>> Trajectory::calculate(const cv::Point_<int>& lastLoc, const cv::Point_<int>& loc) {
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

    auto prediction = loc + ((loc - lastLoc)*vectorMult);

    std::vector<cv::Point_<int>> temp = {loc, prediction};
    trajs.emplace_back(temp);
    while (bnccnt <= bnccntmax) {
        std::vector<bool> bounces = {false, false, false, false};
        cv::Point_<int> intersection;
        cv::Point_<int> newEndPoint;
        // detect a bounce
        bounces = bounceDetect(trajs.back()[0], trajs.back()[1], grabbed, bnccnt);
        // if all of bounces are false, set done = true and break
        if (bounces[0] || bounces[1] || bounces[2] || bounces[3]) {
            // determine from bounces which wall is going to be intersected
            // and calculate the intersection point
            intersection = findIntersection(bounces, trajs.back()[0], trajs.back()[1]);

            // determine whether or not this is a goal
            // depending on which goal is getting intersected
            if (bounces[0] || bounces[2]) {
                goalDetect(intersection, trajs.back()[1].x - trajs.back()[0].x);
                if (leftGoal || rightGoal) {
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

    if(trajs.size() >= bnccntmax+1 && lostCnt < 1) { ///if we have an 'extra' bounce, clip it
    std::vector<std::vector<cv::Point_<int>>> toReturn = {trajs[0],trajs[1],trajs[2],trajs[3]};
    return toReturn;
    }
    else {
    return trajs;
    }
}
