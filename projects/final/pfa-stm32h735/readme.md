# STM32H735G-DK TouchGFX application

This directory contains the Actia STM32H735G-DK graphical application. It is
one of the two final deliverables in the repository; the repository-wide
overview is available at [`../../../README.md`](../../../README.md).

## Platform and features

- STM32H735G-DK target.
- 480 x 272 display at 24 bpp.
- TouchGFX user interface.
- FreeRTOS task integration.
- LTDC, DMA2D, OSPI, and I2C board support.
- LwIP networking configuration.

## Opening and building

The default IDE is STM32CubeIDE. Open
[`PFA_STM32H735.ioc`](./PFA_STM32H735.ioc) with STM32CubeMX to select one of
the supported IDEs:

- STM32CubeIDE
- EWARM 8.50.9 or newer
- MDK-ARM

The project supports flashing the STM32H735G-DK directly from TouchGFX
Designer using GCC and STM32CubeProgrammer. STM32CubeProgrammer is required
for flashing from that workflow.

## Performance measurement

Performance testing can be performed using the GPIO pins assigned to:

- `VSYNC_FREQ` - CN8-D3 (PA0)
- `RENDER_TIME` - CN8-D2 (PG3)
- `FRAME_RATE` - CN8-D1 (PB14)
- `MCU_ACTIVE` - CN8-D0 (PB15)

## Directory guide

- `Core/`: application and CubeMX-generated peripheral initialization.
- `TouchGFX/`: GUI source, assets, target integration, and generated output.
- `Drivers/`: STM32 HAL, CMSIS, BSP, and component drivers.
- `STM32CubeIDE/`, `gcc/`, `MDK-ARM/`, and `EWARM/`: toolchain metadata.

Keep generated files associated with this project and use the `.ioc` file when
changing peripheral configuration.
