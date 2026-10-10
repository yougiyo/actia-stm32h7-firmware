# Project structure

## Ownership rules

The repository has three practical zones:

1. **Deliverables**: the two projects that should be built and reviewed for
   product work.
2. **Experiments**: earlier or focused tests used to validate a peripheral,
   middleware component, or board feature.
3. **Evidence**: captures and images produced while validating communication.

Keeping these zones conceptually separate prevents test applications from
being mistaken for the final firmware.

## Deliverables

### `projects/final/pfa-stm32h735/`

Single-core STM32H735G-DK TouchGFX application.

| Directory | Responsibility |
|---|---|
| `Core/` | CubeMX-generated application and HAL integration |
| `Drivers/` | CMSIS, HAL, BSP, and component drivers |
| `gui/` | Project-owned GUI include files and custom GUI declarations |
| `TouchGFX/` | GUI source, assets, target integration, and generated GUI code |
| `Middlewares/` | ST middleware, including TouchGFX dependencies |
| `LWIP/` | LwIP integration |
| `LIBJPEG/` | JPEG support used by the display application |
| `STM32CubeIDE/` | STM32CubeIDE launch/project metadata |
| `gcc/` | GCC build support |
| `MDK-ARM/` | Keil MDK project and scatter file |
| `EWARM/` | IAR EWARM project and linker configuration |
| `cmake/` | CMake-related build support |

Project-level files include `PFA_STM32H735.ioc` for CubeMX configuration,
`STM32H735XG_FLASH.ld` for the GCC memory layout, `readme.md` for
project-specific notes, and `changelog.txt` for project history.

### `projects/final/ethernet-final-actia/`

Dual-core STM32H747 Ethernet/CAN application.

| Directory | Responsibility |
|---|---|
| `CM7/` | Ethernet/Mongoose application running on Cortex-M7 |
| `CM4/` | Companion Cortex-M4 project |
| `Common/` | Shared dual-core startup/support sources |
| `Drivers/` | CMSIS and STM32H7 HAL |
| `.ioc`, `.project`, `.mxproject` | CubeMX/CubeIDE project configuration |

## Experiments and staging

### `projects/experiments/ethernet/`

Contains earlier Ethernet, Mongoose, CAN, and STM32H7 experiments. The final
Ethernet/CAN project is kept separately under `projects/final/`.

### `projects/experiments/actia-stage/`

Contains exploratory CubeIDE projects for graphics, Ethernet, CAN, UART, Wi-Fi,
and combined peripheral tests. These projects are useful for reference and
hardware bring-up, but are not the final deliverables.

### `test-evidence/network/`

Contains packet captures and screenshots from TCP/Ethernet validation. These
files are evidence, not firmware inputs.

## Safe editing boundaries

- Edit application logic in the user-owned portions of `Core/` and the
  application-specific source directories.
- Regenerate peripheral setup from the owning `.ioc` file.
- Do not copy generated code between projects unless the owning project and
  toolchain are also updated.
- Do not split the contents of a project directory.
- If a project directory is moved again, update repository documentation and
  validate that the IDE can reopen the `.ioc` and build metadata.
