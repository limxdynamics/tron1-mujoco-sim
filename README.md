English | [中文](README_cn.md)

# TRON1 MuJoCo Simulator

**Test LimX TRON1 controllers and RL policies in MuJoCo before they touch the real robot.** The simulator speaks the same `limxsdk` interface as the robot, so a controller tested here runs on hardware by pointing it at the robot's IP address.

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
![MuJoCo](https://img.shields.io/badge/MuJoCo-3.x-0a7bbb.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20x86__64%20%7C%20aarch64-lightgrey.svg)
[![Stars](https://img.shields.io/github/stars/limxdynamics/tron1-mujoco-sim?style=social)](https://github.com/limxdynamics/tron1-mujoco-sim/stargazers)

<p align="center">
  <img src="doc/tron1-mujoco-demo.gif" width="900" alt="TRON1 point-foot, sole-foot and wheel-foot robots walking and turning in MuJoCo">
  <br>
  <sub>Point-foot, sole-foot and wheel-foot TRON1 running the official RL policies in MuJoCo (rendered offscreen with a custom scene).</sub>
</p>

## Highlights

- **Sim-to-sim before sim-to-real.** Check policies trained in [Isaac Gym](https://github.com/limxdynamics/tron1-rl-isaacgym) or [Isaac Lab](https://github.com/limxdynamics/tron1-rl-isaaclab) in a second physics engine.
- **Same interface as the robot.** The simulator publishes joint states and IMU data and accepts joint commands through `limxsdk`, exactly like TRON1.
- **All TRON1 configurations.** Point-foot, sole-foot and wheel-foot models, including the TRON1B variants.
- **Drive it with a gamepad on screen.** The bundled virtual joystick lets you walk and turn the robot with the keyboard.

## Supported robots

Set `ROBOT_TYPE` to one of the folders in `robot-description/pointfoot`:

| Configuration | `ROBOT_TYPE` |
|---|---|
| Point-foot | `PF_TRON1A`, `PF_TRON1B`, `PF_P441A`, `PF_P441B`, `PF_P441C`, `PF_P441C2` |
| Sole-foot | `SF_TRON1A`, `SF_TRON1B` |
| Wheel-foot | `WF_TRON1A`, `WF_TRON1B` |

## Requirements

- Linux on x86_64 or aarch64 (the `limxsdk` wheels are built for Linux; macOS is not supported). The virtual joystick is x86_64 only.
- Python 3 with `venv`. MuJoCo and the other Python dependencies are installed together with `limxsdk`.

## Quick start: run an RL policy

**1. Clone the simulator and the RL controller** (submodules are required):

```bash
git clone --recurse-submodules https://github.com/limxdynamics/tron1-mujoco-sim.git
git clone --recurse-submodules https://github.com/limxdynamics/tron1-rl-deploy-python.git
```

**2. Install `limxsdk` in a virtual environment.** On Ubuntu 23.04 and later a plain `pip install` outside a virtual environment is refused (`externally-managed-environment`).

```bash
sudo apt install -y python3-venv      # if venv is not installed yet
python3 -m venv ~/tron1-venv
source ~/tron1-venv/bin/activate

# Linux x86_64
pip install tron1-mujoco-sim/limxsdk-lowlevel/python3/amd64/limxsdk-*-py3-none-any.whl
# Linux aarch64
# pip install tron1-mujoco-sim/limxsdk-lowlevel/python3/aarch64/limxsdk-*-py3-none-any.whl
```

**3. Start the simulator** (terminal 1):

```bash
source ~/tron1-venv/bin/activate
export ROBOT_TYPE=PF_TRON1A           # see "Supported robots"
python tron1-mujoco-sim/simulator.py
```

The robot falls over right away — nothing is controlling it yet.

**4. Start the RL controller** (terminal 2):

```bash
source ~/tron1-venv/bin/activate
export ROBOT_TYPE=PF_TRON1A           # must match the simulator
export RL_TYPE=isaacgym               # or isaaclab
python tron1-rl-deploy-python/main.py
```

**5. Reset the robot:** click the MuJoCo window and press **Backspace**. The robot stands up under the policy's control.

**6. Drive it** (terminal 3):

```bash
./tron1-mujoco-sim/robot-joystick/robot-joystick
```

With the joystick window focused, use the **arrow keys** to walk and **Numpad 4 / 6** to turn. All key bindings are listed in the joystick window.

<p align="center">
  <img src="doc/simulator.gif" width="800" alt="MuJoCo simulator window with the virtual joystick">
</p>

To make `ROBOT_TYPE` permanent, add the `export` line to `~/.bashrc`.

## Run the C++ SDK example

The `limxsdk-lowlevel` submodule includes C++ examples that talk to the simulator the same way.

```bash
sudo apt update
sudo apt install -y cmake build-essential libeigen3-dev

cd tron1-mujoco-sim/limxsdk-lowlevel
mkdir -p build && cd build
cmake .. && make

export ROBOT_TYPE=PF_TRON1A           # same as the simulator
./examples/pf_groupJoints_move
```

`pf_groupJoints_move` is a point-foot example: it moves every joint smoothly to the zero position and does not balance the robot.

## Use your own controller

The simulator connects to `127.0.0.1` through `limxsdk`. Any controller that subscribes to `RobotState` / `ImuData` and publishes `RobotCmd` with `limxsdk` works — see [tron1-rl-deploy-python](https://github.com/limxdynamics/tron1-rl-deploy-python) for a complete Python example and the [limxsdk-lowlevel](https://github.com/limxdynamics/limxsdk-lowlevel) examples for C++.

## Troubleshooting

- **The robot lies on the floor.** Start the controller first, then press Backspace in the MuJoCo window.
- **`error: externally-managed-environment`.** Install into a virtual environment as shown above.
- **`Please set the ROBOT_TYPE`.** Every terminal (simulator, controller, SDK examples) needs `export ROBOT_TYPE=...`.
- **Wheel-foot falls on a slow machine or VM.** The controller runs on wall-clock time. If the simulator runs slower than real time (for example in a VM with software rendering), wheel-foot balance suffers first; use a machine with GPU-accelerated OpenGL.

## Related repositories

| Repository | Description |
|---|---|
| [tron1-rl-isaacgym](https://github.com/limxdynamics/tron1-rl-isaacgym) | Train TRON1 RL policies in Isaac Gym |
| [tron1-rl-isaaclab](https://github.com/limxdynamics/tron1-rl-isaaclab) | Train TRON1 RL policies in Isaac Lab |
| [tron1-rl-deploy-python](https://github.com/limxdynamics/tron1-rl-deploy-python) | Run RL policies in this simulator or on the robot (Python, includes pre-trained policies) |
| [tron1-rl-deploy-ros](https://github.com/limxdynamics/tron1-rl-deploy-ros) / [ros2](https://github.com/limxdynamics/tron1-rl-deploy-ros2) | Run RL policies with ROS / ROS 2 |
| [robot-description](https://github.com/limxdynamics/robot-description) | TRON1 URDF / MJCF models (submodule) |
| [limxsdk-lowlevel](https://github.com/limxdynamics/limxsdk-lowlevel) | Low-level SDK for Python and C++ (submodule) |
| [robot-joystick](https://github.com/limxdynamics/robot-joystick) | Virtual joystick (submodule) |

## License

[Apache License 2.0](LICENSE). See [NOTICE](NOTICE) for bundled components.

## Questions?

Please [open an issue](https://github.com/limxdynamics/tron1-mujoco-sim/issues). If this project helps your work, a ⭐ helps others find it.
