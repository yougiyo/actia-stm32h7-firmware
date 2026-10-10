# Development workflow

## Select the correct project

Use the project-specific `.ioc` file as the source of truth:

- Graphics work: `PFA_STM32H735-main/PFA_STM32H735.ioc`
- Ethernet/CAN work: `ethernet/Final_Actia/Final_Actia.ioc`

The similarly named projects under `ethernet/` and `actia_stage/` are
experiments. Verify the project name, MCU, and core before flashing hardware.

## STM32H735 workflow

1. Open the H735 `.ioc` file in STM32CubeIDE or STM32CubeMX.
2. Confirm the STM32H735G-DK board setup and the 480 x 272 display settings.
3. Generate code only after checking the selected toolchain.
4. Build from STM32CubeIDE, TouchGFX Designer, Keil MDK, or IAR according to
   the toolchain-specific project you intend to use.
5. Flash with the configured debug probe or STM32CubeProgrammer.
6. Use the documented performance GPIO signals when measuring rendering.

## Ethernet/CAN workflow

1. Open `ethernet/Final_Actia/Final_Actia.ioc`.
2. Confirm the dual-core context and select the Cortex-M7 or Cortex-M4 project
   as appropriate.
3. Build the core projects using the STM32CubeIDE metadata in `CM7/` and
   `CM4/`.
4. Configure the test PC for the firmware's Ethernet subnet before sending UDP
   traffic.
5. Observe UART diagnostics and CAN traffic independently when troubleshooting.
6. Save repeatable packet captures and screenshots under `stage_doc/`.

## Review checklist

- Correct `.ioc` file opened.
- Correct MCU and core selected.
- No generated output copied from another project.
- Linker script matches the selected core and memory map.
- Ethernet MAC address and test IP settings are appropriate for the bench.
- CAN bitrate and frame identifiers match the test tool.
- Flash/download target is the intended board.
