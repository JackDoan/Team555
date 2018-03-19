//
// Created by jad140230 on 2/2/2018.
//

#ifndef AIRHOCKEYROBOT_HELPERS_H
#define AIRHOCKEYROBOT_HELPERS_H

#include <utility>

class Coordinate {

public:
   /* template <class T> T x() {
        return std::pair::first;
    }

    template <class T> T y() {
        return std::pair::second;
    }

    template <class T> Coordinate(T mx, T my) {
        std::pair<T,T>(mx,my);
    }*/

    double x;
    double y;
    Coordinate(double mx, double my) {
        x = mx;
        y = my;
    }
    Coordinate() {
        x = 0;
        y = 0;
    }

};

class Vector {

public:
    double dx;
    double dy;
    Vector(double vx, double vy){
        dx = vx;
        dy = vy;
    }
    Vector(){
        dx = 0;
        dy = 0;
    }
};



#define dbgPrint(fmt, ...) \
            do { printf(fmt, ##__VA_ARGS__); } while (0)

#endif //AIRHOCKEYROBOT_HELPERS_H
