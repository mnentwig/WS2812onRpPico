#pragma once
#include <cstdlib>

namespace util {
float frand(float minVal, float maxVal) {
    const float span = maxVal - minVal;
    return minVal + span * (float)rand() / (float)RAND_MAX;
}
}  // namespace util