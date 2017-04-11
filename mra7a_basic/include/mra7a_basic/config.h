#ifndef CONFIG_H_
#define CONFIG_H_

#include <iostream>
#include <vector>

//const std::string DEFAULT_NODE = "/dev/pcanusb32"; //use:DEFAULT_NODE.c_str()
#define DEFAULT_NODE "/dev/pcanusb32"
const std::vector<int> jointID{21,22,23,24,25,26,27};
const std::vector<std::string> joint_names{"Joint1","Joint2","Joint3","Joint4","Joint5","Joint6","Joint7"};

#define CONTROL_RATE 100 //100HZ

/*Topic name definition*/
#define JOINT_STATE_TOPIC "/mra/joint_states"  //-->pub joint state
#define STATE_TOPIC "/mra/state"               //Robot State Related-->pub
#define JOINT_COMMAND_TOPIC "joint_command"    //-->sub joint control command
#define RESET_MRA_API_TOPIC "reset_MRA_API"    //-->sub, When canbus interrupts, we need to reset MRA_API after reconnecting the usb-can.

#endif
