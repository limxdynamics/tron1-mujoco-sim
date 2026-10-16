[English](README.md) | 中文

# TRON1 MuJoCo 仿真器

**在 MuJoCo 中测试 LimX TRON1 的控制器和 RL 策略，确认没问题再上真机。** 仿真器与真机使用同一套 `limxsdk` 接口，在这里测试好的控制器，只需改成机器人的 IP 地址就能在真机上运行。

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
![MuJoCo](https://img.shields.io/badge/MuJoCo-3.x-0a7bbb.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20x86__64%20%7C%20aarch64-lightgrey.svg)
[![Stars](https://img.shields.io/github/stars/limxdynamics/tron1-mujoco-sim?style=social)](https://github.com/limxdynamics/tron1-mujoco-sim/stargazers)

<p align="center">
  <img src="doc/tron1-mujoco-demo.gif" width="900" alt="TRON1 点足、足板、轮足机器人在 MuJoCo 中行走与转向">
  <br>
  <sub>点足、足板、轮足 TRON1 在 MuJoCo 中运行官方 RL 策略（离屏渲染，使用了自定义场景）。</sub>
</p>

## 亮点

- **先 sim-to-sim，再 sim-to-real。** 用另一个物理引擎检验在 [Isaac Gym](https://github.com/limxdynamics/tron1-rl-isaacgym) 或 [Isaac Lab](https://github.com/limxdynamics/tron1-rl-isaaclab) 中训练的策略。
- **与真机接口一致。** 仿真器通过 `limxsdk` 发布关节状态和 IMU 数据、接收关节指令，和 TRON1 真机完全一样。
- **覆盖所有 TRON1 形态。** 点足、足板、轮足模型，包括 TRON1B 型号。
- **屏幕上的虚拟手柄。** 自带的虚拟手柄可以用键盘控制机器人行走和转向。

## 支持的机器人

把 `ROBOT_TYPE` 设置为 `robot-description/pointfoot` 下的某个目录名：

| 形态 | `ROBOT_TYPE` |
|---|---|
| 点足 | `PF_TRON1A`、`PF_TRON1B`、`PF_P441A`、`PF_P441B`、`PF_P441C`、`PF_P441C2` |
| 足板 | `SF_TRON1A`、`SF_TRON1B` |
| 轮足 | `WF_TRON1A`、`WF_TRON1B` |

## 环境要求

- x86_64 或 aarch64 架构的 Linux（`limxsdk` 只提供 Linux 版本，不支持 macOS）。虚拟手柄只有 x86_64 版本。
- 带 `venv` 的 Python 3。MuJoCo 及其他 Python 依赖会随 `limxsdk` 一起安装。

## 快速上手：运行 RL 策略

**1. 克隆仿真器和 RL 控制器**（必须带上子模块）：

```bash
git clone --recurse-submodules https://github.com/limxdynamics/tron1-mujoco-sim.git
git clone --recurse-submodules https://github.com/limxdynamics/tron1-rl-deploy-python.git
```

**2. 在虚拟环境中安装 `limxsdk`。** Ubuntu 23.04 及以后的版本不允许在虚拟环境外直接 `pip install`（会报 `externally-managed-environment`）。

```bash
sudo apt install -y python3-venv      # 如果还没装 venv
python3 -m venv ~/tron1-venv
source ~/tron1-venv/bin/activate

# Linux x86_64
pip install tron1-mujoco-sim/limxsdk-lowlevel/python3/amd64/limxsdk-*-py3-none-any.whl
# Linux aarch64
# pip install tron1-mujoco-sim/limxsdk-lowlevel/python3/aarch64/limxsdk-*-py3-none-any.whl
```

**3. 启动仿真器**（终端 1）：

```bash
source ~/tron1-venv/bin/activate
export ROBOT_TYPE=PF_TRON1A           # 见“支持的机器人”
python tron1-mujoco-sim/simulator.py
```

机器人会马上倒下——这时还没有控制器在控制它。

**4. 启动 RL 控制器**（终端 2）：

```bash
source ~/tron1-venv/bin/activate
export ROBOT_TYPE=PF_TRON1A           # 必须与仿真器一致
export RL_TYPE=isaacgym               # 或 isaaclab
python tron1-rl-deploy-python/main.py
```

**5. 复位机器人：** 点击 MuJoCo 窗口，按 **Backspace**。机器人会在策略控制下站起来。

**6. 控制机器人**（终端 3）：

```bash
./tron1-mujoco-sim/robot-joystick/robot-joystick
```

选中手柄窗口后，用**方向键**行走，用**小键盘 4 / 6** 转向。完整按键说明显示在手柄窗口中。

<p align="center">
  <img src="doc/simulator.gif" width="800" alt="带虚拟手柄的 MuJoCo 仿真器窗口">
</p>

如果想永久设置 `ROBOT_TYPE`，把 `export` 那一行加到 `~/.bashrc` 中。

## 运行 C++ SDK 示例

`limxsdk-lowlevel` 子模块中带有 C++ 示例，同样通过 SDK 与仿真器通信。

```bash
sudo apt update
sudo apt install -y cmake build-essential libeigen3-dev

cd tron1-mujoco-sim/limxsdk-lowlevel
mkdir -p build && cd build
cmake .. && make

export ROBOT_TYPE=PF_TRON1A           # 与仿真器一致
./examples/pf_groupJoints_move
```

`pf_groupJoints_move` 是点足示例：把所有关节平滑地移动到零位，不负责平衡机器人。

## 使用自己的控制器

仿真器通过 `limxsdk` 连接 `127.0.0.1`。任何使用 `limxsdk` 订阅 `RobotState` / `ImuData`、发布 `RobotCmd` 的控制器都可以接入。完整的 Python 示例见 [tron1-rl-deploy-python](https://github.com/limxdynamics/tron1-rl-deploy-python)，C++ 示例见 [limxsdk-lowlevel](https://github.com/limxdynamics/limxsdk-lowlevel)。

## 常见问题

- **机器人一直躺在地上。** 先启动控制器，再在 MuJoCo 窗口中按 Backspace。
- **`error: externally-managed-environment`。** 按上文在虚拟环境中安装。
- **`Please set the ROBOT_TYPE`。** 每个终端（仿真器、控制器、SDK 示例）都需要 `export ROBOT_TYPE=...`。
- **轮足在性能较弱的电脑或虚拟机上会倒。** 控制器按真实时间运行。如果仿真跑不到实时速度（例如在使用软件渲染的虚拟机中），轮足的平衡最先受影响，请使用支持 GPU 加速 OpenGL 的电脑。

## 相关仓库

| 仓库 | 描述 |
|---|---|
| [tron1-rl-isaacgym](https://github.com/limxdynamics/tron1-rl-isaacgym) | 在 Isaac Gym 中训练 TRON1 RL 策略 |
| [tron1-rl-isaaclab](https://github.com/limxdynamics/tron1-rl-isaaclab) | 在 Isaac Lab 中训练 TRON1 RL 策略 |
| [tron1-rl-deploy-python](https://github.com/limxdynamics/tron1-rl-deploy-python) | 在本仿真器或真机上运行 RL 策略（Python，含预训练策略） |
| [tron1-rl-deploy-ros](https://github.com/limxdynamics/tron1-rl-deploy-ros) / [ros2](https://github.com/limxdynamics/tron1-rl-deploy-ros2) | 基于 ROS / ROS 2 运行 RL 策略 |
| [robot-description](https://github.com/limxdynamics/robot-description) | TRON1 URDF / MJCF 模型（子模块） |
| [limxsdk-lowlevel](https://github.com/limxdynamics/limxsdk-lowlevel) | Python 与 C++ 底层 SDK（子模块） |
| [robot-joystick](https://github.com/limxdynamics/robot-joystick) | 虚拟手柄（子模块） |

## 许可证

[Apache License 2.0](LICENSE)。附带组件说明见 [NOTICE](NOTICE)。

## 有问题？

欢迎 [提交 Issue](https://github.com/limxdynamics/tron1-mujoco-sim/issues)。如果这个项目对你有帮助，点个 ⭐ 能让更多人看到它。
