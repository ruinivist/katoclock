#pragma once

#include <opencv2/core.hpp>
#include <string>

class Renderer {
   public:
    Renderer();

    // Load assets from disk. Returns false if any fail to load.
    bool load_assets(const std::string& bg_path,
                     const std::string& hour_hand_path,
                     const std::string& minute_hand_path);

    // Composites the hands onto the background at the given angles (degrees).
    // Returns the result image.
    cv::Mat render(double hour_angle, double minute_angle);

   private:
    cv::Mat background_;
    cv::Mat hour_hand_;
    cv::Mat minute_hand_;

    // Helpers
    cv::Mat transform_hand(const cv::Mat& hand_sprite, double angle_deg,
                           double scale, const cv::Point2f& pivot_fraction);
    void blend_overlay(cv::Mat& bg, const cv::Mat& fg);
};
