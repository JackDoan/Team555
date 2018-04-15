//
// Created by mike on 4/14/18.
//

#ifndef TEAM555_DOUBLEBUFFER_H
#define TEAM555_DOUBLEBUFFER_H


#include <opencv2/core/mat.hpp>

class DoubleBuffer {
private:
    const int MAX_BUFFERS = 2;
    cv::Mat buffer[2];
    unsigned int bufferToUse = 0;
public:

    DoubleBuffer() = default;

    cv::Mat& getBuffer(const unsigned int& i) {
        if(i+1 > MAX_BUFFERS) {
            return buffer[0];
        }
        return buffer[i];
    }
    cv::Mat& active() {
        return getBuffer(bufferToUse);
    }
    cv::Mat& inactive() {
        return getBuffer(bufferToUse+1);
    }
    void toggle() {
        bufferToUse = ++bufferToUse % MAX_BUFFERS;
    }


};


#endif //TEAM555_DOUBLEBUFFER_H
