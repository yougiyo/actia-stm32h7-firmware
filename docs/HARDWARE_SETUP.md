# Hardware setup

This document describes the minimum bench setup for the two final Actia
firmware projects. Confirm the board revision, wiring, and debug probe before
powering the hardware. The repository does not replace the board's official
schematic or safety instructions.

## Common equipment

- Target STM32 evaluation board.
- ST-LINK or the debug probe supported by the selected IDE.
- USB cable for power/debug.
- Isolated Ethernet test network for Ethernet validation.
- CAN interface and CAN analyzer for FDCAN validation.
- Serial terminal connected to the configured diagnostic UART.
- STM32CubeProgrammer when flashing through the TouchGFX workflow.

## PFA STM32H735G-DK

Project entry point:

[`projects/final/pfa-stm32h735/PFA_STM32H735.ioc`](../projects/final/pfa-stm32h735/PFA_STM32H735.ioc)

The project is configured for:

- STM32H735G-DK.
- 480 x 272 display.
- 24-bit RGB display format.
- TouchGFX, FreeRTOS, LTDC, DMA2D, and OSPI.

### Performance-test signals

Use the board connector pins documented by the project:

| Signal | MCU pin | Board location |
|---|---|---|
| `VSYNC_FREQ` | PA0 | CN8-D3 |
| `RENDER_TIME` | PG3 | CN8-D2 |
| `FRAME_RATE` | PB14 | CN8-D1 |
| `MCU_ACTIVE` | PB15 | CN8-D0 |

Connect these signals to a logic analyzer or oscilloscope only after
confirming the instrument ground and voltage levels are appropriate.

## Final Actia Ethernet/CAN project

Project entry point:

[`projects/final/ethernet-final-actia/Final_Actia.ioc`](../projects/final/ethernet-final-actia/Final_Actia.ioc)

The project uses:

- STM32H747XIHx dual-core device.
- Cortex-M7 application and Cortex-M4 companion project.
- Ethernet RMII interface.
- FDCAN1 at a nominal 500 kbit/s.
- UART1 for diagnostics.

### Ethernet bench

Use an isolated switch or direct connection to the test PC. The current M7
application configures the Mongoose interface with:

| Parameter | Value |
|---|---|
| IP address | `192.168.0.10` |
| Netmask | `255.255.255.0` |
| Gateway | `192.168.0.1` |

Configure the test PC with a non-conflicting address on the same subnet. Do
not connect this static test configuration to a production network without
reviewing the address and gateway settings.

### CAN bench

Configure the CAN analyzer for classic CAN at 500 kbit/s. Connect CAN
high/low and a suitable common reference according to the board and analyzer
documentation. Termination must match the physical bus topology.

The application uses standard CAN identifiers documented in
[`TEST_PROCEDURES.md`](./TEST_PROCEDURES.md).

## Flashing and first power-up

1. Connect the debug probe and target board.
2. Select the intended core and launch configuration.
3. Build before downloading.
4. Confirm the target MCU detected by the programmer.
5. Download the image and reset the board.
6. Check UART output before starting network or CAN traffic.

Never flash an image built for another MCU, core, memory layout, or board
revision.
