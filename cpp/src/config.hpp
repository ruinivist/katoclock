#pragma once

#include <opencv2/core.hpp>

// file full of magic numbers
// most are hand tuned based on the cat image I have

namespace Config {
const cv::Point2f CLOCK_CENTER(300.0f, 286.0f);

const double HAND_SCALE = 0.22;
const double HOUR_HAND_SCALE = HAND_SCALE * 0.7;
const double MINUTE_HAND_SCALE = HAND_SCALE * 0.7;

const cv::Point2f HOUR_HAND_PIVOT_FRACTION(0.126f, 0.502f);
const cv::Point2f MINUTE_HAND_PIVOT_FRACTION(0.101f, 0.488f);
// Asset convention: unrotated hand points right (+X).
// Clock convention: 12:00 points up (-Y), hence +90 degrees.
const double HAND_BASE_ANGLE_OFFSET = 90.0;

}  // namespace Config
