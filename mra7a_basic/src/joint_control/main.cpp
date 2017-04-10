#include <ros/ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <mra_api.h>
#include <mra7a_basic/config.h>
#include <math.h>
#include <sensor_msgs/JointState.h>
#include <mra_core_msgs/AssemblyState.h>
#include <mra_core_msgs/JointCommand.h>



UserControlOnCan *userControlOnCan = new UserControlOnCan();

void chatterCallback(const mra_core_msgs::JointCommandConstPtr &msg)
{
    ROS_INFO("[%f,%f,%f,%f,%f,%f,%f]",
             msg->command[0],msg->command[1],msg->command[2],msg->command[3],
            msg->command[4],msg->command[5],msg->command[6]);

    for(int i=0; i<jointID.size(); i++) {
        bool isSent = userControlOnCan->setJointTagPos(jointID[i],msg->command[i]*180/M_PI);
        //ROS_INFO("Sent ---- %s", isSent ? "success" : "failure");
        if (isSent==false) {
            ROS_ERROR("Sent is failure");
            //do something
        }
    }

}

int main(int argc, char **argv)
{

    ros::init(argc, argv, "joint_control");
    ros::NodeHandle n;
    ros::Rate loop_rate(CONTROL_RATE);//default 100Hz

    if (! userControlOnCan->Init(DEFAULT_NODE)) {
        ROS_ERROR("Can't Open the pcanusb32");
        return 0;
    }
    for(int i=0; i<jointID.size(); i++) {
        userControlOnCan->setJointAutoUpdateCurPos(jointID[i],true);
    }


    /*subscribe other node's joint control command*/
    ros::Subscriber sub = n.subscribe(JOINT_COMMAND_TOPIC, 1, chatterCallback);

    ros::Publisher joint_state_pub = n.advertise<sensor_msgs::JointState> (JOINT_STATE_TOPIC, 1000);
    ros::Publisher state_pub = n.advertise<mra_core_msgs::AssemblyState> (STATE_TOPIC, 1);

    sensor_msgs::JointState joint_state;
    joint_state.position.resize(jointID.size());
    joint_state.velocity.resize(jointID.size());
    joint_state.effort.resize(jointID.size());
    joint_state.name.resize(jointID.size());
    joint_state.name = joint_names;

    while (ros::ok()) {
        for(int i=0; i<jointID.size(); i++) {
            joint_state.position[i] = userControlOnCan->readJointCurPos(jointID[i]);
        }
        for(int i=0; i<jointID.size(); i++) {
            joint_state.velocity[i] = userControlOnCan->readJointCurSpd(jointID[i]);
        }
        for(int i=0; i<jointID.size(); i++) {
            joint_state.effort[i] = userControlOnCan->readJointCurI(jointID[i]);
        }
        joint_state_pub.publish(joint_state);
        loop_rate.sleep();
        ros::spinOnce();
    }

    ros::spin();

    return 0;
}
