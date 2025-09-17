/**
 * @file tron2_sim.h
 *
 * @brief This file contains the declarations of classes related to the control of tron2 robots in simulation.
 *
 * © [2025] LimX Dynamics Technology Co., Ltd. All rights reserved.
 */

#ifndef _LIMX_SDK_TRON2_SIM_H_
#define _LIMX_SDK_TRON2_SIM_H_

#include <string>
#include <functional>
#include "limxsdk/macros.h"
#include "limxsdk/datatypes.h"
#include "limxsdk/apibase.h"

namespace limxsdk
{
  /**
   * @class Tron2Sim
   * @brief Class for controlling tron2 robots in simulation.
   */
  class LIMX_SDK_API Tron2Sim : public ApiBase
  {
  public:
    /**
     * @brief Get an instance of the Tron2Sim class.
     * @return A pointer to a Tron2Sim instance (Singleton pattern).
     */
    static Tron2Sim *getInstance();

    /**
     * @brief Initialize the simulation with the given robot IP address.
     * @param robot_ip_address The IP address of the robot. Default is "127.0.0.1".
     * @return True if initialization is successful, false otherwise.
     */
    bool init(const std::string &robot_ip_address = "127.0.0.1") override;

    /**
     * @brief Subscribe to the robot control command.
     *
     * @param cb The callback function to be called when a control command is received.
     */
    void subscribeRobotCmdForSim(std::function<void(const RobotCmdConstPtr &)> cb) override;

    /**
     * @brief Publish the robot state to the motion control algorithm.
     *
     * @param state The robot state to be published.
     * @return True if publishing is successful, false otherwise.
     */
    bool publishRobotStateForSim(const RobotState &state) override;

    /**
     * @brief Publishes IMU (Inertial Measurement Unit) data to the motion control algorithm for simulation.
     * The order of IMU data is as follows:
     *        Accelerometer: imu.acc
     *        Gyroscope: imu.gyro
     *        Quaternion: imu.quat
     *
     * @param imu The IMU data to be published.
     * @return True if publishing is successful, false otherwise.
     */
    bool publishImuDataForSim(const ImuData &imu) override;

    /**
     * @brief Destructor for the Tron2Sim class.
     */
    virtual ~Tron2Sim();

  private:
    /**
     * @brief Constructor for the Tron2Sim class.
     */
    Tron2Sim();
  };
}

#endif