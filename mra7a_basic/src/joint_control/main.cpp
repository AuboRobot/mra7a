#include <ros/ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <mra_api.h>
#include <mra7a_basic/config.h>
#include <math.h>
#include <sensor_msgs/JointState.h>
#include <mra_core_msgs/AssemblyState.h>
#include <mra_core_msgs/JointCommand.h>
#include <std_msgs/Bool.h>



UserControlOnCan *userControlOnCan = new UserControlOnCan();
mra_core_msgs::AssemblyState mra_state;

void joint_command_callback(const mra_core_msgs::JointCommandConstPtr &msg)
{
    ROS_INFO("[%f,%f,%f,%f,%f,%f,%f]",
             msg->command[0],msg->command[1],msg->command[2],msg->command[3],
            msg->command[4],msg->command[5],msg->command[6]);

    if(mra_state.canbus_state==mra_core_msgs::AssemblyState::CANBUS_STATE_NORMAL) {
        for(int i=0; i<jointID.size(); i++) {
            bool isSent = userControlOnCan->setJointTagPos(jointID[i],msg->command[i]);
            if (isSent==false) {
                ROS_ERROR("Sent is failure");
                //set canbus state = CANBUS_STATE_INTERRUPT
                mra_state.canbus_state = mra_core_msgs::AssemblyState::CANBUS_STATE_INTERRUPT;
            }
        }
    }
}

void MRA_API_INIT(const std_msgs::Bool &reset) {
    if(reset.data == 1) {
        delete userControlOnCan;
        userControlOnCan = new UserControlOnCan();
        if(userControlOnCan->Init(DEFAULT_NODE)) {
            mra_state.canbus_state = mra_core_msgs::AssemblyState::CANBUS_STATE_NORMAL;
            mra_state.enabled = true;
        } else {
            ROS_ERROR("Can't Open the pcanusb32");
        }
    } else {
        if(userControlOnCan->Init(DEFAULT_NODE)) {
            mra_state.canbus_state = mra_core_msgs::AssemblyState::CANBUS_STATE_NORMAL;
            mra_state.enabled = true;
        } else {
            ROS_ERROR("Can't Open the pcanusb32");
        }
    }
}

int main(int argc, char **argv)
{

    ros::init(argc, argv, "joint_control");
    ros::NodeHandle n;
    ros::Rate loop_rate(CONTROL_RATE);//default 100Hz

    /*mra's state information*/
    mra_state.enabled = false;
    mra_state.ready = true;
    mra_state.error = false;
    mra_state.canbus_state = mra_core_msgs::AssemblyState::CANBUS_STATE_INTERRUPT;
    mra_state.interruptJoints.resize(jointID.size());

    /*MRA_API first init*/
    std_msgs::Bool reset;
    reset.data = 0;
    MRA_API_INIT(reset);

    /*sub other node's joint control command*/
    ros::Subscriber sub_joint_command = n.subscribe(JOINT_COMMAND_TOPIC, 1, &joint_command_callback);
    /*pub joint state*/
    ros::Publisher joint_state_pub = n.advertise<sensor_msgs::JointState> (JOINT_STATE_TOPIC, 1000);
    /*pub mra's state, such as whether canbus is normal or not*/
    ros::Publisher state_pub = n.advertise<mra_core_msgs::AssemblyState> (STATE_TOPIC, 1);
    /*sub reset */
    ros::Subscriber sub_reset_MRA_API = n.subscribe(RESET_MRA_API_TOPIC, 10, &MRA_API_INIT);


    sensor_msgs::JointState joint_state;
    joint_state.position.resize(jointID.size());
    joint_state.velocity.resize(jointID.size());
    joint_state.effort.resize(jointID.size());
    joint_state.name.resize(jointID.size());
    joint_state.name = joint_names;

    while (ros::ok() && mra_state.canbus_state==mra_core_msgs::AssemblyState::CANBUS_STATE_NORMAL) {

        /*get joint states and publish it*/
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

        /*pub mra_state*/
        for(int i=0; i<jointID.size(); i++) {
            //If any joint position>0.01 radio, the mra's joints are not in home position.Set ready = false.
            joint_state.position[i] > 0.01 ? mra_state.ready=false : mra_state.ready=true;
        }
        state_pub.publish(mra_state);

        /*loop_rate default 100HZ*/
        loop_rate.sleep();
        ros::spinOnce();

    }

    ros::spin();

    return 0;
}
