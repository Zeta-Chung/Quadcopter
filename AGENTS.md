# Repository Guidelines

## 沟通语言

日常对话、进度说明和结果反馈默认使用汉语；代码标识符、命令和文件路径保留原文。用户明确要求其他语言时，遵循用户要求。

## 项目目标与阶段范围

依据三阶段课程任务书，项目最终目标是实现可稳定飞行的 STM32 四轴飞行器：阶段 I 完成机电硬件、裸机驱动与电机基础旋转；阶段 II 移植实时操作系统并重构驱动、构建运行支撑；阶段 III 完成姿态解算、PID 控制与整机验证。当前重点为阶段 I 的 GY-86 / MPU6050 驱动。阶段目标、资料依据和进度见 `README.md`；区分已编写、已编译和已通过硬件验证，勿将规划功能描述为已完成。

## Project Structure & Module Organization

This repository contains staged STM32F401RE quadcopter firmware exercises and hardware references.

- `第一阶段/Project1-点亮STM32板载LED/`: assembly LED exercise and its Keil project.
- `第一阶段/Project4-裸机连接GY-86/代码/`: C firmware and `GY-86.uvprojx`. `User/` holds the entry point, interrupts, and configuration; `System/` holds I2C support; `Hardware/` holds the GY-86 driver; `Start/` contains startup/CMSIS files; `Library/` contains ST peripheral drivers.
- `第一阶段/Project4-裸机连接GY-86/资料/`: sensor datasheets, schematics, images, and learning notes.
- `STM32系列资料/`: MCU and board reference documents. Other numbered stage directories reserve later exercises.

## Build, Test, and Development Commands

Use Keil µVision with STM32F401 device support and ARM Compiler 5.06 update 7, as recorded in both `.uvprojx` files. Open the relevant project and select `Target 1`; use **Build**, then **Download** with the board and debug probe connected.

Example PowerShell rebuild from the repository root (adjust the executable path to your installation):

```powershell
& 'C:\Keil_v5\UV4\UV4.exe' -r '.\第一阶段\Project4-裸机连接GY-86\代码\GY-86.uvprojx' -t 'Target 1'
```

There is no repository-wide build script or local application server. Add new source files to the appropriate µVision group and update include paths when necessary.

## Coding Style & Naming Conventions

Match surrounding formatting; existing C uses both tabs and four-space indentation. Use four spaces in new modules and avoid unrelated reformatting. Keep paired `.c`/`.h` modules, header guards such as `GY_86_H`, fixed-width integer types, and existing `APP_<Peripheral>_<Action>` API names. Document units, register meanings, and error returns. Keep peripheral logic in `System/` or `Hardware/`. No formatter or linter is configured.

## Testing Guidelines

No automated test framework, test naming convention, or coverage threshold is configured. Rebuild the affected project and inspect warnings. Verify LED behavior or sensor initialization and register reads on hardware; for I2C changes, check timeout and NACK handling. Record board, wiring, clock configuration, and observed results. The current C entry point does not invoke sensor initialization, so describe any temporary test harness.

## Commit & Pull Request Guidelines

History uses short Chinese descriptions such as `完善注释`; follow that descriptive style and keep commits focused. Pull requests should identify the affected exercise, explain behavior changes, link applicable issues, and report build and hardware checks. Include wiring diagrams or captures when relevant. Exclude ignored `Objects/`, `Listings/`, `DebugConfig/`, and personal Keil settings.
