#include "renderer.hpp"

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "config.hpp"

Renderer::Renderer() {}

bool Renderer::load_assets(const std::string& bg_path,
                           const std::string& hour_hand_path,
                           const std::string& minute_hand_path) {
    background_ = cv::imread(bg_path, cv::IMREAD_UNCHANGED);
    if (background_.empty()) {
        std::cerr << "failed to load background: " << bg_path << std::endl;
        return false;
    }
    if (background_.channels() != 4) {
        std::cerr << "expected rgba png for background";
        return false;
    }

    // Load hands with alpha channel.
    hour_hand_ = cv::imread(hour_hand_path, cv::IMREAD_UNCHANGED);
    if (hour_hand_.empty()) {
        std::cerr << "failed to load hour hand: " << hour_hand_path
                  << std::endl;
        return false;
    }

    minute_hand_ = cv::imread(minute_hand_path, cv::IMREAD_UNCHANGED);
    if (minute_hand_.empty()) {
        std::cerr << "failed to load minute hand: " << minute_hand_path
                  << std::endl;
        return false;
    }

    if (hour_hand_.channels() != 4 || minute_hand_.channels() != 4) {
        std::cerr << "expected rgba png for hands" << std::endl;
        return false;
    }

    return true;
}

cv::Mat Renderer::render(double hour_angle, double minute_angle) {
    auto clock_to_cv_rotation = [](double clock_angle) {
        return Config::HAND_BASE_ANGLE_OFFSET - clock_angle;
    };

    cv::Mat warped_hour = transform_hand(
        hour_hand_, clock_to_cv_rotation(hour_angle), Config::HOUR_HAND_SCALE,
        Config::HOUR_HAND_PIVOT_FRACTION);
    cv::Mat warped_minute = transform_hand(
        minute_hand_, clock_to_cv_rotation(minute_angle),
        Config::MINUTE_HAND_SCALE, Config::MINUTE_HAND_PIVOT_FRACTION);

    cv::Mat keyframe = background_.clone();

    // Layer order matters: hour underneath minute.
    blend_overlay(keyframe, warped_hour);
    blend_overlay(keyframe, warped_minute);

    return keyframe;
}

cv::Mat Renderer::transform_hand(const cv::Mat& hand_sprite, double angle_deg,
                                 double scale,
                                 const cv::Point2f& pivot_fraction) {
    cv::Mat oriented_sprite;
    cv::flip(hand_sprite, oriented_sprite, 0);

    if (scale <= 0.0) {
        return cv::Mat::zeros(background_.size(), CV_8UC4);
    }

    cv::Point2f pivot(oriented_sprite.cols * pivot_fraction.x,
                      oriented_sprite.rows * pivot_fraction.y);

    cv::Mat rot_mat = cv::getRotationMatrix2D(pivot, angle_deg, scale);
    rot_mat.at<double>(0, 2) += Config::CLOCK_CENTER.x - pivot.x;
    rot_mat.at<double>(1, 2) += Config::CLOCK_CENTER.y - pivot.y;

    cv::Mat overlay = cv::Mat::zeros(background_.size(), CV_8UC4);
    cv::warpAffine(oriented_sprite, overlay, rot_mat, background_.size(),
                   cv::INTER_CUBIC, cv::BORDER_CONSTANT,
                   cv::Scalar(0, 0, 0, 0));

    return overlay;
}

void Renderer::blend_overlay(cv::Mat& bg, const cv::Mat& fg) {
    // Blend hand RGB onto background RGB using hand alpha
    for (int y = 0; y < fg.rows; ++y) {
        const cv::Vec4b* fg_ptr = fg.ptr<cv::Vec4b>(y);
        cv::Vec4b* bg_ptr = bg.ptr<cv::Vec4b>(y);

        for (int x = 0; x < fg.cols; ++x) {
            const cv::Vec4b& fg_pixel = fg_ptr[x];
            const double alpha = fg_pixel[3] / 255.0;

            if (alpha > 0.0) {
                const cv::Vec4b bg_pixel = bg_ptr[x];
                for (int c = 0; c < 3; ++c) {
                    bg_ptr[x][c] = static_cast<uchar>(
                        fg_pixel[c] * alpha + bg_pixel[c] * (1.0 - alpha));
                }
            }
        }
    }
}
