# Final Actia Ethernet/CAN application

This is the final dual-core Ethernet/CAN firmware project in the Actia
repository. It targets the STM32H747XIHx and preserves separate Cortex-M7 and
Cortex-M4 projects.

## Responsibilities

- **Cortex-M7**: runs the main Ethernet application and Mongoose network
  integration.
- **Cortex-M4**: provides the companion core project and follows the dual-core
  boot sequence.
- **Ethernet**: uses the RMII MAC interface with a static test network
  configuration in the current firmware.
- **CAN**: uses FDCAN1 with classic CAN at a nominal 500 kbit/s.
- **Diagnostics**: sends application diagnostics through UART1.

The current application demonstrates UDP-to-CAN sensor requests and CAN-to-UDP
responses. The M7 application maintains two example sensor values, sends CAN
requests, validates CAN replies, and reports the result to the requesting UDP
peer.

## Open the project

Open [`Final_Actia.ioc`](./Final_Actia.ioc) in STM32CubeIDE or STM32CubeMX.
The relevant project metadata is:

- [`CM7/`](./CM7/): M7 source, headers, linker configuration, launch
  configuration, and generated build output.
- [`CM4/`](./CM4/): M4 source, headers, linker configuration, and launch
  configuration.
- [`Common/`](./Common/): shared dual-core boot/support code.
- [`Drivers/`](./Drivers/): CMSIS and STM32H7 HAL.

Build and flash the intended core from the IDE. Confirm the selected debug
launch configuration before downloading to the board.

## Bench validation

Before testing UDP traffic:

1. Connect the board and the Ethernet test host to the same isolated bench
   network.
2. Confirm that the host address does not conflict with the firmware's
   configured subnet.
3. Monitor UART1 for Mongoose and application diagnostics.
4. Monitor FDCAN1 with the CAN tool and verify bitrate/frame identifiers.
5. Save packet captures under [`../../test-evidence/network/`](../../test-evidence/network/).

Do not treat the projects under `../../experiments/` as interchangeable with
this final project. They are earlier experiments and may use different MCUs,
peripherals, or memory layouts.
