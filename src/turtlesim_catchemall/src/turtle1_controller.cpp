#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include "turtlesim/srv/kill.hpp"
#include "turtlesim/srv/spawn.hpp"

using namespace std::chrono_literals;
using namespace std::placeholders;

/*
    turtle1_controller.cpp

    This is the main turtle robot controller, that will
    make turtle1 move in a square-like path, gets despawned
    and a new turtle is spawned, which gets controlled as well
*/

class turtle1_controller : public rclcpp::Node{

    public:

        double angular_velocity;
        double linear_velocity;

        turtle1_controller() : Node("turtle1_controller"){
            
            this->declare_parameter("angular_velocity", 0.8);
            this->declare_parameter("linear_velocity", 2.0);

            angular_velocity = this->get_parameter("angular_velocity").as_double();
            linear_velocity = this->get_parameter("linear_velocity").as_double();

            pub_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);

            sub_ = this->create_subscription<turtlesim::msg::Pose>("turtle1/pose", 10, 
                                                                    std::bind(&turtle1_controller::myPosition, this, _1));
            
            // registering clients to the kill and spawn services
            client_kill = this->create_client<turtlesim::srv::Kill>("kill");
            client_spawn = this->create_client<turtlesim::srv::Spawn>("spawn");

            // turtlesim runs at 30Hz, period of 0.033s so we update state faster than rendering
            timer_ = this->create_wall_timer(0.01s, std::bind(&turtle1_controller::moveTurtle1, this));

            RCLCPP_INFO(this->get_logger(), "turtle1_controller has started!");
        }

        void moveTurtle1(){

            // to detect that the turtlesim node has started, so movement becomes valid
            while(!client_spawn->wait_for_service(1s)){

                // prevents very weird closing behaviour when you ctrl+c the client
                if(!rclcpp::ok()){
                    RCLCPP_INFO(this->get_logger(), "Shutting down...");
                    return;
                }

                RCLCPP_WARN(this->get_logger(), "Waiting for turtlesim node to start...");
            }

            // these are velocities, not positions
            auto msg = geometry_msgs::msg::Twist();

            t = std::fmod(t + 0.01, 17);

            if (t > 0 && t <= 2 ){

                //move right
                msg.linear.x = linear_velocity;
            } else if (t > 2 && t <= 4){

                // rotate 90 degrees to the left
                if (angle < 1.5){

                    msg.angular.z = angular_velocity;
                }
            } else if (t > 4 && t <= 6){

                // move up
                msg.linear.x = linear_velocity;
            } else if(t > 6 && t <= 8){

                // rotate 90 deg to left, angle will become -ve
                if (angle < 3.15 && !(angle < 0)){

                    msg.angular.z = angular_velocity;
                }
            } else if (t > 8 && t < 10){

                // move left
                msg.linear.x = linear_velocity;
            } else if (t >= 10 && t < 12){
                
                // rotate 90 deg to point downwards
                if(angle < -1.5){
                    msg.angular.z = angular_velocity;
                }
            } else if (t >= 12 && t < 14){

                // move downwards
                msg.linear.x = linear_velocity;
            } else if (t >=14 && t < 16){

                // rotate 90 deg to point rightwards
                if(angle < 0){
                    msg.angular.z = angular_velocity;
                }
            } else {

                // prevents unwanted kill requests
                if (!turtle1_killed){

                    // turtle1 gets despawned at the end of the path
                    // the service callbacks handle the respawn behaviour
                    callbackClientRequestKill("turtle1");
                    turtle1_killed = true;
                }
            }

            pub_->publish(msg);
        }

        void callbackClientRequestKill(std::string name){

            auto request = std::make_shared<turtlesim::srv::Kill::Request>();

            while(!client_kill->wait_for_service(1s)){

                // prevents very weird closing behaviour when you ctrl+c the client
                if(!rclcpp::ok()){
                    RCLCPP_INFO(this->get_logger(), "Shutting down...");
                    return;
                }

                RCLCPP_WARN(this->get_logger(), "Waiting for service /kill...");
            }

            request->name = name;
            client_kill->async_send_request(request, std::bind(&turtle1_controller::callbackClientReceiveKill, this, _1));
        }

        void callbackClientRequestSpawn(float x, float y, float theta, std::string name){

            auto request = std::make_shared<turtlesim::srv::Spawn::Request>();

            while(!client_spawn->wait_for_service(1s)){

                // prevents very weird closing behaviour when you ctrl+c the client
                if(!rclcpp::ok()){
                    RCLCPP_INFO(this->get_logger(), "Shutting down...");
                    return;
                }

                RCLCPP_WARN(this->get_logger(), "Waiting for service /spawn...");
            }

            // spawning at the same coordinates that the turtlesim node uses
            request->x = x;
            request->y = y;
            request->theta = theta;
            request->name = name;

            client_spawn->async_send_request(request, std::bind(&turtle1_controller::callbackClientReceiveSpawn, this, _1));
        }

    private:

        float angle;
        bool turtle1_killed = false;

        // publisher, subscriber, service and timer objects
        
        //pub directly manipulates the controllable turtle robot, that being turtle1
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;

        // two services for spawning and despawning the turtle
        rclcpp::Client<turtlesim::srv::Kill>::SharedPtr client_kill;
        rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr client_spawn;

        // timer to drive movement
        rclcpp::TimerBase::SharedPtr timer_;

        // time accumulation
        double t;
        
        void myPosition(const turtlesim::msg::Pose::SharedPtr msg){

            angle = msg->theta;
        }

        // the kill interface has no data on the response
        // this guarantees the spawn only occurs after a kill
        void callbackClientReceiveKill(rclcpp::Client<turtlesim::srv::Kill>::SharedFuture future){

            auto response = future.get();
            RCLCPP_INFO(this->get_logger(), "Turtle1 kill complete!");
            callbackClientRequestSpawn(5.544445, 5.544445, 0, "turtle1");
        }

        // the spawn interface indicates the created name, so the kill flag gets lifted
        // it's also safe to restart the time count to repeat the loop
        void callbackClientReceiveSpawn(rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture future){

            auto response = future.get();
            turtle1_killed = false;
            t = 0;

            RCLCPP_INFO(this->get_logger(), "%s has been spawned!", response->name.c_str());
        }

};

int main(int argc, char **argv){

    rclcpp::init(argc, argv);

    auto node = std::make_shared<turtle1_controller>();

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}