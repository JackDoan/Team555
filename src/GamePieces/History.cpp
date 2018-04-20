//
// Created by mike on 4/19/18.
//

#include "../../inc/GamePieces/History.h"

template <class T>
void History<T>::insert(const T& in) {
    data.insert(data.begin(), in);
    data.resize(dataLen);
}

template<>
void History<cv::Point_<int>>::insert(const cv::Point_<int>& in) {

}

