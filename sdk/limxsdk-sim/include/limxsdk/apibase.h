/**
 * @file apibase.h
 *
 * @brief This file contains the declarations of interface related to the control of robots.
 *
 * © [2025] LimX Dynamics Technology Co., Ltd. All rights reserved.
 */

#ifndef _LIMX_SDK_INTERFACE_H_
#define _LIMX_SDK_INTERFACE_H_

#include <string>
#include <functional>
#include "limxsdk/macros.h"
#include "limxsdk/datatypes.h"

namespace limxsdk
{
  /**
   * @brief Base class for the LIMX SDK API.
   *        Provides interface methods for controlling and accessing robot functionality.
   */
  class LIMX_SDK_API ApiBase
  {
  public:
    ApiBase();

    virtual ~ApiBase();

    /**
     * @brief Virtual initialization method.
     *        This method should specify the operations to be performed before using the object in the main function.
     * @param robot_ip_address The IP address of the robot.
     *                         For simulation, it is typically set to "127.0.0.1",
     *                         while for a real robot, it may be set to "10.192.1.2".
     * @return True if init successfully, otherwise false.
     */
    virtual bool init(const std::string &robot_ip_address);

    /**
     * @brief Virtual method to get the number of motors in the robot.
     * @return The total number of motors.
     */
    virtual uint32_t getMotorNumber();

    /**
     * @brief Virtual method to obtain the names of all motors in the robot.
     *
     * This method is used to fetch a list of names corresponding to each motor in the robot.
     * It can be overridden in derived classes to implement custom logic for getting the motor names.
     *
     * @return A vector containing the names of all motors in the robot.
     */
    virtual std::vector<std::string> getMotorNames();

    /**
     * @brief Virtual method to subscribe to updates of the robot's IMU (Inertial Measurement Unit) data.
     * @param cb The callback function to be invoked when new IMU data is received.
     */
    virtual void subscribeImuData(std::function<void(const ImuDataConstPtr &)> cb);

    /**
     * @brief Virtual method to subscribe to robot state updates.
     * @param cb The callback function to be invoked when a robot state update is received.
     */
    virtual void subscribeRobotState(std::function<void(const RobotStateConstPtr &)> cb);

    /**
     * @brief Virtual method to publish a command to control the robot's actions.
     * @param cmd The RobotCmd object representing the desired robot command.
     * @return True if the command was successfully published, otherwise false.
     */
    virtual bool publishRobotCmd(const RobotCmd &cmd);

    /**
     * @brief Virtual method to subscribe to robot commands for simulation.
     * @param cb The callback function to be invoked when a robot command is received in simulation mode.
     */
    virtual void subscribeRobotCmdForSim(std::function<void(const RobotCmdConstPtr &)> cb);

    /**
     * @brief Virtual method to publish the robot state for simulation.
     * @param state The RobotState object representing the current state of the robot in simulation.
     * @return True if the state was successfully published, otherwise false.
     */
    virtual bool publishRobotStateForSim(const RobotState &state);

    /**
     * @brief Virtual method to publish IMU (Inertial Measurement Unit) data to the motion control algorithm for simulation.
     * The order of IMU data is as follows:
     *        Accelerometer: imu.acc
     *        Gyroscope: imu.gyro
     *        Quaternion: imu.quat
     *
     * @param imu The IMU data to be published.
     * @return True if publishing is successful, false otherwise.
     */
    virtual bool publishImuDataForSim(const ImuData &imu);

    /**
     * @brief Virtual method to subscribe to sensor inputs related to a joystick from the robot.
     * @param cb The callback function to be invoked when sensor input from a joystick is received from the robot.
     */
    virtual void subscribeSensorJoy(std::function<void(const SensorJoyConstPtr &)> cb);

    /**
     * @brief Virtual method to subscribe to diagnostic values from the robot.
     *
     * Examples:
     * | name        | level  | code | msg
     * |-------------|--------|------|--------------------
     * | imu         | OK     | 0    | - IMU is functioning properly.
     * | imu         | ERROR  | -1   | - Error in IMU.
     * |-------------|--------|------|--------------------
     * | ethercat    | OK     | 0    | - EtherCAT is working fine.
     * | ethercat    | ERROR  | -1   | - EtherCAT error.
     * |-------------|--------|------|--------------------
     * | calibration | OK     | 0    | - Robot calibration successful.
     * | calibration | WARN   | 1    | - Robot calibration in progress.
     * | calibration | ERROR  | -1   | - Robot calibration failed.
     * |-------------|--------|------|--------------------
     *
     * @param cb he callback function to be invoked when diagnostic values are received from the robot.
     */
    virtual void subscribeDiagnosticValue(std::function<void(const DiagnosticValueConstPtr &)> cb);

    /**
     * @brief Set the robot light effect.
     *
     * This method configures the robot light effect based on the provided effect parameter.
     * The effect parameter should be an integer that specifies the desired robot light effect.
     *
     * @param effect An integer representing the desired robot light effect.
     * @return A boolean value indicating whether the robot light effect was successfully set.
     */
    virtual bool setRobotLightEffect(int effect);

    /**
     * @brief Publish a diagnostic message about the robot's status.
     *
     * This method sends a diagnostic message to the system, providing information
     * about the robot's status. Diagnostic messages are used to report issues,
     * warnings, or normal operation of various components.
     *
     * @param name The name of the diagnostic message or the reporting component.
     * @param part The specific part or subsystem of the robot that the message relates to.
     * @param code A numeric code identifying the specific diagnostic condition.
     * @param level The severity level of the diagnostic message:
     *              - 0: OK (normal operation, no issues)
     *              - 1: Warning (a non-critical issue that should be noted)
     *              - 2: Error (a critical issue that requires attention)
     * @param message A human-readable string providing additional details about the condition.
     */
    virtual void publishDiagnostic(const std::string &name, const std::string &part, int code, int level = 0, const std::string &message = "");

    /**
     * @brief Sends a JSON-formatted message to the robot's high-level API.
     * 
     * Used for custom commands or interactions not covered by the standard API.
     * 
     * @param json_payload JSON string adhering to the robot's API protocol.
     *             Example: {"accid": "xxx", "title": "request_xxx", "timestamp": xxx, "guid": "xxx", "data": {}}
     */
    virtual void publishJsonMessage(const std::string &json_payload);

    /**
     * @brief Registers a callback to handle JSON-formatted responses and notifications from the robot.
     * 
     * The callback will be invoked in two scenarios:
     * 1. When the robot sends a response to a previously sent JSON command (publishJsonMessage)
     * 2. When the robot initiates a notification (unsolicited message)
     * 
     * @param cb Callback function with signature:
     *           void(const std::string &json_payload)
     *           where json_payload contains:
     *           - Response to a command: {"accid": "xxx", "title": "response_xxx", "timestamp": xxx, "guid": "xxx", "data": {}}
     *           - Notification: {"accid": "xxx", "title": "notify_xxx", "timestamp": xxx, "guid": "xxx", "data": {}}
     */
    virtual void subscribeJsonMessage(std::function<void(const std::string &)> cb);

  protected:
    std::vector<std::function<void(const ImuDataConstPtr &)>> imu_data_callback_;           // Callback function for handling IMU data updates.
    std::vector<std::function<void(const RobotStateConstPtr &)>> robot_state_callback_;     // Callback function for handling robot state updates.
    std::vector<std::function<void(const RobotCmdConstPtr &)>> robot_cmd_callback_;         // Callback function for handling robot commands in simulation mode.
    std::vector<std::function<void(const SensorJoyConstPtr &)>> sensor_joy_callback_;       // Callback for handling joystick sensor inputs from the robot.
    std::vector<std::function<void(const DiagnosticValueConstPtr &)>> diagnostic_callback_; // Callback for handling diagnostic values from the robot.
    std::vector<std::function<void(const std::string &)>> json_message_callback_;           // Callback for handling JSON API responses and notification from the robot.
  };
}

#endif