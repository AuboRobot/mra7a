#mra7a_basic package
**This package is used to control real MRA7A by ROS**

There are two executable fiels: joint control and control_panel.  

When you are using mra_ros_control package to control MRA7A, you need the first step: rosrun mra7a_basic joint_control.  

***
**Note:**
1. In the include/mra7a_basic/config.h file, you can change some config.<br>
such as:jointID, joint_names, topic_names, etc.<br>
2. In the top CMakeList.txt file, you need to change the system parameter "LONG_BIT" according to your system. You can lookup the parameter by runing "getconf LONG_BIT" in the terminal<br>
==*set(LONG_BIT "32")* ==<br>
***
Here, We provides a simple test to control MRA7A's joint independently using a QT panel.<br>
**Steps:**
1.roscore<br>
2.rosrun mra7a_basic joint_control<br>
3.rosrun mra7a_basic control_panel<br>
