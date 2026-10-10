# Actia STM32 Embedded Projects

This repository contains the Actia embedded-systems workbench for STM32H7
microcontrollers. It includes two deliverables and a set of supporting
experiments, staging projects, and network-test evidence.

The repository is intentionally kept in its existing filesystem layout. STM32
CubeIDE, STM32CubeMX, TouchGFX, Keil MDK, and IAR project files contain
relative paths and workspace metadata; moving those files without regenerating
the projects can make an otherwise working firmware project fail to open or
build.

## Deliverables

| Deliverable | Location | Hardware/software focus |
|---|---|---|
| PFA STM32H735 graphical application | [`PFA_STM32H735-main/`](./PFA_STM32H735-main/) | STM32H735G-DK, TouchGFX, FreeRTOS, external memory, display pipeline, and LwIP |
| Final Actia Ethernet/CAN application | [`ethernet/Final_Actia/`](./ethernet/Final_Actia/) | Dual-core STM32H747, Ethernet over RMII, Mongoose UDP handling, and FDCAN |

The folder names above are the canonical locations of the final projects.
Other similarly named folders are experiments or intermediate work and should
not be confused with the deliverables.

## Repository map

```text
ALL_Actia_prject/
├── README.md
├── docs/
│   ├── PROJECT_STRUCTURE.md
│   └── DEVELOPMENT_WORKFLOW.md
├── PFA_STM32H735-main/          # Final H735 TouchGFX application
├── ethernet/
│   ├── Final_Actia/             # Final H747 Ethernet/CAN application
│   ├── 000Ethernet_Mongoose/    # Ethernet/Mongoose experiment
│   ├── 001Test/                 # Ethernet test project
│   └── ...                      # Other Ethernet experiments
├── actia_stage/                 # Staging and exploratory CubeIDE projects
└── stage_doc/                   # Packet captures and test evidence
```

The detailed ownership of each directory is documented in
[`docs/PROJECT_STRUCTURE.md`](./docs/PROJECT_STRUCTURE.md).

## Final project: PFA STM32H735

Open [`PFA_STM32H735-main/PFA_STM32H735.ioc`](./PFA_STM32H735-main/PFA_STM32H735.ioc)
with STM32CubeMX or STM32CubeIDE. The project targets the STM32H735G-DK and
provides:

- TouchGFX user interface for a 480 x 272, 24-bit display.
- FreeRTOS application tasks, including the TouchGFX and video tasks.
- LTDC and DMA2D display support.
- OSPI external-memory support for the board.
- LwIP networking configuration.
- Build and project metadata for STM32CubeIDE, GCC, Keil MDK, and IAR EWARM.

Important subdirectories are:

- `Core/`: generated application startup, peripheral initialization, and user
  application source.
- `gui/`: project-owned GUI include files and custom GUI declarations.
- `TouchGFX/`: TouchGFX application, GUI, target, assets, and generated code.
- `Drivers/`: CMSIS, STM32H7 HAL, board support, and component drivers.
- `Middlewares/`, `LWIP/`, and `LIBJPEG/`: middleware and protocol/library
  integration.
- `STM32CubeIDE/`, `gcc/`, `MDK-ARM/`, and `EWARM/`: toolchain-specific
  project/build files.

See [`PFA_STM32H735-main/readme.md`](./PFA_STM32H735-main/readme.md) for the
project-specific board and display notes.

## Final project: Ethernet and CAN

Open [`ethernet/Final_Actia/Final_Actia.ioc`](./ethernet/Final_Actia/Final_Actia.ioc)
with STM32CubeIDE or STM32CubeMX. This is a dual-core STM32H747 project:

- Cortex-M7 hosts the main Ethernet application.
- Cortex-M4 contains the companion core project and dual-core boot path.
- Ethernet uses the RMII interface.
- FDCAN1 is configured for classic CAN at a nominal 500 kbit/s.
- Mongoose supplies the embedded IP/UDP handling.
- UART1 is used by the application for diagnostic output.
- The application bridges UDP requests and CAN frames for sensor queries.

The core-specific boundaries are deliberately preserved:

- `CM7/`: Cortex-M7 application, headers, linker script, and debug output.
- `CM4/`: Cortex-M4 companion project and linker script.
- `Common/`: shared dual-core startup/support code.
- `Drivers/`: STM32H7 HAL and CMSIS support.

## Recommended workflow

1. Start from the relevant final-project directory, not from an experiment.
2. Open the `.ioc` file in STM32CubeIDE or STM32CubeMX.
3. Check the selected toolchain and board/debug configuration.
4. Build and flash from the IDE.
5. Keep generated output (`Debug/`, `Release/`, simulator output, and map
   files) associated with the project that produced it.
6. Record network/CAN test evidence in `stage_doc/` without mixing it into
   firmware source directories.

Detailed build and debugging guidance is in
[`docs/DEVELOPMENT_WORKFLOW.md`](./docs/DEVELOPMENT_WORKFLOW.md).

## Source and generated files

Generated files are part of the current STM32 tool projects and have not been
removed or rewritten. Treat them according to their owner:

- Edit CubeMX-controlled peripheral configuration through the `.ioc` file.
- Put application changes inside the existing `USER CODE` sections when
  regenerating CubeMX output.
- Treat TouchGFX generated files as generated output unless the TouchGFX
  workflow explicitly identifies them as user-editable.
- Do not hand-edit linker output, map files, object files, dependency files,
  or simulator binaries.

## Scope of this cleanup

This cleanup is organizational and documentation-only. No firmware source,
header, linker script, CubeMX configuration, IDE project file, generated
artifact, or protocol behavior has been changed.
