# Copyright information
#
# © [2024] LimX Dynamics Technology Co., Ltd. All rights reserved.

import os
import sys
import time
import mujoco
import mujoco.viewer as viewer
from functools import partial
import limxsdk
import limxsdk.robot.Rate as Rate
import limxsdk.robot.Robot as Robot
import limxsdk.robot.RobotType as RobotType
import limxsdk.datatypes as datatypes

class SimulatorMujoco:
    def __init__(self, asset_path, joint_sensor_names, robot): 
        self.robot = robot
        self.joint_sensor_names = joint_sensor_names
        self.joint_num = len(joint_sensor_names)
        
        # Load the MuJoCo model and data from the specified XML asset path
        self.mujoco_model = mujoco.MjModel.from_xml_path(asset_path)
        self.mujoco_data = mujoco.MjData(self.mujoco_model)
        
        # Launch the MuJoCo viewer in passive mode with custom settings
        self.viewer = viewer.launch_passive(self.mujoco_model, self.mujoco_data, key_callback=self.key_callback, show_left_ui=True, show_right_ui=True)
        self.viewer.cam.distance = 10  # Set camera distance
        self.viewer.cam.elevation = -20  # Set camera elevation
    
        self.dt = self.mujoco_model.opt.timestep  # Get simulation timestep
        self.fps = 1 / self.dt  # Calculate frames per second (FPS)

        # Initialize robot command data with default values
        self.robot_cmd = datatypes.RobotCmd()
        self.robot_cmd.mode = [0. for x in range(0, self.joint_num)]
        self.robot_cmd.q = [0. for x in range(0, self.joint_num)]
        self.robot_cmd.dq = [0. for x in range(0, self.joint_num)]
        self.robot_cmd.tau = [0. for x in range(0, self.joint_num)]
        self.robot_cmd.Kp = [0. for x in range(0, self.joint_num)]
        self.robot_cmd.Kd = [0. for x in range(0, self.joint_num)]

        # Initialize robot state data with default values
        self.robot_state = datatypes.RobotState()
        self.robot_state.tau = [0. for x in range(0, self.joint_num)]
        self.robot_state.q = [0. for x in range(0, self.joint_num)]
        self.robot_state.dq = [0. for x in range(0, self.joint_num)]

        # Initialize IMU data structure
        self.imu_data = datatypes.ImuData()

        # Set up callback for receiving robot commands in simulation mode
        self.robotCmdCallbackPartial = partial(self.robotCmdCallback)
        self.robot.subscribeRobotCmdForSim(self.robotCmdCallbackPartial)

    # Callback function for receiving robot command data
    def robotCmdCallback(self, robot_cmd: datatypes.RobotCmd):
        self.robot_cmd = robot_cmd

    # Callback for keypress events in the MuJoCo viewer (currently does nothing)
    def key_callback(self, keycode):
        pass

    def run(self):
        frame_count = 0
        self.rate = Rate(self.fps)  # Set the update rate according to FPS
        while self.viewer.is_running():    
            # Step the MuJoCo physics simulation
            mujoco.mj_step(self.mujoco_model, self.mujoco_data)

            # Update robot state data from simulation
            for i in range(self.joint_num):
                self.robot_state.q[i] = self.mujoco_data.qpos[i + 7]
                self.robot_state.dq[i] = self.mujoco_data.qvel[i + 6]
                self.robot_state.tau[i] = self.mujoco_data.ctrl[i]

                # Apply control commands to the robot based on the received robot command data
                self.mujoco_data.ctrl[i] = (
                    self.robot_cmd.Kp[i] * (self.robot_cmd.q[i] - self.robot_state.q[i]) + 
                    self.robot_cmd.Kd[i] * (self.robot_cmd.dq[i] - self.robot_state.dq[i]) + 
                    self.robot_cmd.tau[i]
                )
        
            # Set the timestamp for the current robot state and publish it
            self.robot_state.stamp = time.time_ns()
            self.robot.publishRobotStateForSim(self.robot_state)

            # Extract IMU data (orientation, gyro, and acceleration) from simulation
            imu_quat_id = mujoco.mj_name2id(self.mujoco_model, mujoco.mjtObj.mjOBJ_SENSOR, "quat")
            self.imu_data.quat[0] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_quat_id] + 0]
            self.imu_data.quat[1] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_quat_id] + 1]
            self.imu_data.quat[2] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_quat_id] + 2]
            self.imu_data.quat[3] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_quat_id] + 3]

            imu_gyro_id = mujoco.mj_name2id(self.mujoco_model, mujoco.mjtObj.mjOBJ_SENSOR, "gyro")
            self.imu_data.gyro[0] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_gyro_id] + 0]
            self.imu_data.gyro[1] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_gyro_id] + 1]
            self.imu_data.gyro[2] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_gyro_id] + 2]

            imu_acc_id = mujoco.mj_name2id(self.mujoco_model, mujoco.mjtObj.mjOBJ_SENSOR, "acc")
            self.imu_data.acc[0] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_acc_id] + 0]
            self.imu_data.acc[1] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_acc_id] + 1]
            self.imu_data.acc[2] = self.mujoco_data.sensordata[self.mujoco_model.sensor_adr[imu_acc_id] + 2]

            # Set the timestamp for the current IMU data and publish it
            self.imu_data.stamp = time.time_ns()
            self.robot.publishImuDataForSim(self.imu_data)

            # Sync the viewer every 20 frames for smoother visualization
            if frame_count % 20 == 0:
                self.viewer.sync()

            frame_count += 1
            self.rate.sleep()  # Maintain the simulation loop at the correct rate

if __name__ == '__main__': 
    robot_type = os.getenv("ROBOT_TYPE")

    # Check if the ROBOT_TYPE environment variable is set, otherwise exit with an error
    if not robot_type:
        print("Error: Please set the ROBOT_TYPE using 'export ROBOT_TYPE=<robot_type>'.")
        sys.exit(1)

    # Create a Robot instance of the PointFoot type
    robot = Robot(RobotType.PointFoot, True)

    # Default IP address for the robot
    robot_ip = "127.0.0.1"
    
    # Check if command-line argument is provided for robot IP
    if len(sys.argv) > 1:
        robot_ip = sys.argv[1]

    # Initialize the robot with the provided IP address
    if not robot.init(robot_ip):
        sys.exit()

    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Define the path to the robot model XML file based on the robot type
    model_path = f'{script_dir}/robot-description/pointfoot/{robot_type}/xml/robot.xml'

    # Check if the model file exists, otherwise exit with an error
    if not os.path.exists(model_path):
        print(f"Error: The file {model_path} does not exist. Please ensure the ROBOT_TYPE is set correctly.")
        sys.exit(1)

    print(f"*** Model File Loaded: robot-description/pointfoot/{robot_type}/xml/robot.xml ***")
    print("*** The robot falls until a controller is running. Start your controller, then press Backspace in the MuJoCo window to reset the robot. ***")

    # Define the names of the joint sensors used in the robot
    if robot_type.startswith("WF"):
        joint_sensor_names = [
            "abad_L_Joint", "hip_L_Joint", "knee_L_Joint", "wheel_L_Joint", "abad_R_Joint", "hip_R_Joint", "knee_R_Joint", "wheel_R_Joint"
        ]
    elif robot_type.startswith("SF"):
        joint_sensor_names = [
            "abad_L_Joint", "hip_L_Joint", "knee_L_Joint", "ankle_L_Joint", "abad_R_Joint", "hip_R_Joint", "knee_R_Joint", "ankle_R_Joint"
        ]
    else:
        joint_sensor_names = [
            "abad_L_Joint", "hip_L_Joint", "knee_L_Joint", "abad_R_Joint", "hip_R_Joint", "knee_R_Joint"
        ]

    # Create and run the MuJoCo simulator instance
    simulator = SimulatorMujoco(model_path, joint_sensor_names, robot)
    simulator.run()
