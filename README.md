<!--
Suggested GitHub Topics:
- embedded-systems
- arm-cortex-m33
- nxp-mcx
- frdm-mcxa153
- uart-driver
- interrupt-driven
- bare-metal
- mcuxpresso
- c
- motor-control
- pmsm
- bldc
-->

# FRDM-MCXA153

Embedded C drivers and examples for the NXP FRDM-MCXA153 development board (ARM Cortex-M33 microcontroller).

## Overview

This repository contains hands-on embedded software projects for the NXP FRDM-MCXA153 evaluation board, focusing on low-level peripheral drivers and motor control applications. It serves as a practical learning resource for embedded systems development on the MCX A-series platform.

## Why This Exists

The FRDM-MCXA153 is a cost-effective development platform for the NXP MCXA153 microcontroller (Cortex-M33, 96MHz, up to 256KB flash). This repository provides:

- **Educational driver examples** - Implementing UART, GPIO, and other peripherals from scratch using register-level programming
- **Motor control reference** - PMSM (Permanent Magnet Synchronous Motor) and BLDC motor control implementations using NXP's Motor Control Advanced Algorithms (MCAA) library
- **Interview-ready code samples** - Clean, well-documented C code demonstrating embedded best practices

## Tech Stack

| Component | Technology |
|-----------|------------|
| **MCU** | NXP MCXA153 (ARM Cortex-M33, 96 MHz) |
| **SDK** | MCUXpresso SDK v2.14 |
| **Toolchain** | ARM GCC / CMake |
| **IDE** | VS Code with MCUXpresso extensions |
| **Debug** | MCU-Link (on-board debugger) |
| **Motor Control** | NXP MCAA Library (CM33 No-DSP) |

## Project Structure

```
FRDM-MCXA153/
├── Examples/
│   └── frdmmcxa153_ese_driver_examples_lpuart_interrupt/  # LPUART interrupt-driven driver with RGB LED control
├── SDK_24/
│   ├── boards/frdmmcxa153/      # Board support files
│   ├── devices/MCXA153/         # Device headers and startup
│   ├── CMSIS/                   # ARM CMSIS core files
│   ├── components/              # Additional components (serial_manager, etc.)
│   ├── middleware/
│   │   └── motor_control/       # PMSM/BLDC motor control middleware
│   └── tools/                   # CMake toolchain files
├── .gitignore
├── LICENSE
└── README.md
```

## Building & Flashing

### Prerequisites

- [ARM GCC Toolchain](https://developer.arm.com/downloads/-/gnu-rm) (v10.3.1 or later)
- [CMake](https://cmake.org/download/) (v3.10 or later)
- [MCUXpresso for VS Code](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools/mcuxpresso-for-visual-studio-code:MCUXPRESSO-VSC) (optional but recommended)

### Build (Command Line)

```bash
cd Examples/frdmmcxa153_ese_driver_examples_lpuart_interrupt/armgcc

# Debug build
./build_debug.sh

# Release build  
./build_release.sh

# Clean build artifacts
./clean.sh
```

### Flash

The build generates `.bin` and `.elf` files in the `debug/` or `release/` folder. Flash using:

- **MCU-Link** (drag-and-drop to mass storage device)
- **MCUXpresso IDE** or **VS Code** with debug configuration
- **pyOCD** or **OpenOCD** command line tools

### Running the Example

1. Connect USB-C cable to the MCU-Link port (J15)
2. Build and flash the firmware
3. Open serial terminal (115200 baud, 8-N-1)
4. Type 'r', 'g', or 'b' to toggle the RGB LED

## Key Learnings

- **Register-level programming** - Direct hardware register manipulation for UART, GPIO, and clock configuration without HAL abstraction layers
- **Interrupt-driven architecture** - Efficient FIFO-based circular buffer implementation for non-blocking UART communication using NVIC and Cortex-M33 interrupt handling
- **Embedded build systems** - CMake-based cross-compilation setup with linker scripts for ARM Cortex-M targets
- **Motor control fundamentals** - PMSM/BLDC field-oriented control (FOC) implementation using NXP's MCAA library for sensorless and encoder-based motor control

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

**Note:** The NXP SDK files in `SDK_24/` are subject to the NXP Software License Agreement (see `SDK_24/LA_OPT_NXP_Software_License.txt`). The motor control middleware has its own BSD-3-Clause license (see `SDK_24/middleware/motor_control/licenses/`).

---

*Developed for educational purposes at HAN University of Applied Sciences.*
