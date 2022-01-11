///************************************************************************
//Copyright (c) 2018-2019, Unitree Robotics.Co.Ltd. All rights reserved.
//Use of this source code is governed by the MPL-2.0 license, see LICENSE.
//************************************************************************/

#include <ros/ros.h>
#include <string>
#include <unitree_legged_msgs/LowCmd.h>
#include <unitree_legged_msgs/LowState.h>
#include "convert.h"
#include "transmit.h"

std::string char2hex(uint8_t c) {
  const char bits[] = "0123456789abcdef";
  char b1 = bits[c / 16], b2 = bits[c % 16];
  return {b1, b2};
}

int statePublishNode(UNITREE_LEGGED_SDK::UDP &ros_udp) {
  ros::NodeHandle nh;
  ros::Rate loop_rate(500);

  UNITREE_LEGGED_SDK::LowCmd cmd_udp{0};
  UNITREE_LEGGED_SDK::LowState state_udp{0};
  unitree_legged_msgs::LowCmd cmd_ros;
  unitree_legged_msgs::LowState state_ros;

  ros_udp.InitCmdData(cmd_udp);
  ros_udp.Send();

  float dt = 0.002;
  ros::Publisher state_pub = nh.advertise<unitree_legged_msgs::LowState>("low_state", 1);
  UNITREE_LEGGED_SDK::LoopFunc loop_udpSend("udp_send", dt, 3, [&ros_udp]() { ros_udp.udpState.SendCount++; });
//  LoopFunc loop_udpSend("udp_send", dt, 3, [&ros_udp]() { ros_udp.Send(); });
  UNITREE_LEGGED_SDK::LoopFunc loop_udpRecv("udp_recv", dt, 3, [&ros_udp]() { ros_udp.Recv(); });
  loop_udpSend.start();
  loop_udpRecv.start();

  while (ros::ok()) {
    ros_udp.GetRecv(state_udp);

//    for (int i = 0; i < 100; ++i) {
//      std::cout << char2hex(ros_udp.recvBuf[i]) << ' ';
//    }
//    std::cout << std::endl << std::endl;
    state_ros = ToRos(state_udp);
    state_pub.publish(state_ros);
    ros_udp.SetSend(cmd_udp);
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}

int main(int argc, char *argv[]) {
  ros::init(argc, argv, "position_ros_mode");
  std::string firmwork;
  ros::param::get("/firmwork", firmwork);

//  std::string robot_name;
//  UNITREE_LEGGED_SDK::LeggedType rname;
//  ros::param::get("/robot_name", robot_name);
//  if (strcasecmp(robot_name.c_str(), "A1") == 0)
//    rname = UNITREE_LEGGED_SDK::LeggedType::A1;
//  else if (strcasecmp(robot_name.c_str(), "Aliengo") == 0)
//    rname = UNITREE_LEGGED_SDK::LeggedType::Aliengo;

//   UNITREE_LEGGED_SDK::Control control(rname, UNITREE_LEGGED_SDK::LOWLEVEL);
//  UNITREE_LEGGED_SDK::InitEnvironment();
//  UNITREE_LEGGED_SDK::LCM ros_node(LOWLEVEL);

  UNITREE_LEGGED_SDK::UDP ros_node(UNITREE_LEGGED_SDK::LOWLEVEL);
  statePublishNode(ros_node);
}

