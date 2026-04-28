#include <memory>
#include <cmath>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"

#include "first_project/msg/tf_error_msg.hpp"

using namespace std::chrono_literals;

class TfError : public rclcpp::Node
{
public:
    TfError() : Node("tf_error"), start_time_(this->now()), travelled_distance_(0.0), prev_x_(0.0), prev_y_(0.0), first_(true)
    {
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

        pub_ = this->create_publisher<first_project::msg::TfErrorMsg>("/tf_error_msg", 10);

        timer_ = this->create_wall_timer(
                100ms,
                std::bind(&TfError::compute_error, this)
        );

        RCLCPP_INFO(this->get_logger(), "TF Error node started.");
    }

private:
    void compute_error()
    {
        try {
            auto t1 = tf_buffer_->lookupTransform(
                    "odom",
                    "base_link",
                    tf2::TimePointZero
            );

            auto t2 = tf_buffer_->lookupTransform(
                    "odom",
                    "base_link2",
                    tf2::TimePointZero
            );

            double x1 = t1.transform.translation.x;
            double y1 = t1.transform.translation.y;

            double x2 = t2.transform.translation.x;
            double y2 = t2.transform.translation.y;

            double dx = x1 - x2;
            double dy = y1 - y2;
            double error = std::sqrt(dx * dx + dy * dy);

            if (first_) {
                prev_x_ = x2;
                prev_y_ = y2;
                first_ = false;
            } else {
                double dist = std::sqrt(
                        std::pow(x2 - prev_x_, 2) +
                        std::pow(y2 - prev_y_, 2)
                );
                travelled_distance_ += dist;

                prev_x_ = x2;
                prev_y_ = y2;
            }

            int time_from_start = (this->now() - start_time_).seconds();

            // Message
            auto msg = first_project::msg::TfErrorMsg();
            msg.header.stamp = this->now();
            msg.tf_error = error;
            msg.time_from_start = time_from_start;
            msg.travelled_distance = travelled_distance_;

            pub_->publish(msg);

        } catch (tf2::TransformException &ex) {
            RCLCPP_WARN(this->get_logger(), "%s", ex.what());
        }
    }

    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    rclcpp::Publisher<first_project::msg::TfErrorMsg>::SharedPtr pub_;

    rclcpp::TimerBase::SharedPtr timer_;

    rclcpp::Time start_time_;
    double travelled_distance_;
    double prev_x_, prev_y_;
    bool first_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TfError>());
    rclcpp::shutdown();
    return 0;
}//
// Created by lydia on 4/28/2026.
//
