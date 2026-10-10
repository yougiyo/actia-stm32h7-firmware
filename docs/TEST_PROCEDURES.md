# Test procedures

These procedures provide repeatable bench checks for the two final projects.
They are intentionally separate from the firmware source and from captured
evidence in [`test-evidence/network/`](../test-evidence/network/).

## Before testing

- Confirm the correct `.ioc` file is open.
- Confirm the intended board and core.
- Build successfully with the selected toolchain.
- Check that the debug probe, serial terminal, Ethernet host, and CAN analyzer
  are connected to the intended hardware.
- Record the firmware revision and test date.

## PFA STM32H735 display smoke test

1. Open the H735 project and build it.
2. Flash the STM32H735G-DK.
3. Reset the board.
4. Confirm that the TouchGFX application starts and the display is active.
5. Exercise the available screens and verify that input and rendering remain
   responsive.
6. If measuring performance, monitor the documented GPIO signals:
   `VSYNC_FREQ`, `RENDER_TIME`, `FRAME_RATE`, and `MCU_ACTIVE`.
7. Record board revision, display result, and any visual or timing anomalies.

## Ethernet link test

1. Connect the Final Actia board and test PC to an isolated Ethernet network.
2. Configure the PC on `192.168.0.0/24` with an address other than
   `192.168.0.10`.
3. Flash and reset the Cortex-M7 image.
4. Confirm link activity and monitor UART1 diagnostics.
5. Capture traffic with Wireshark or an equivalent tool.
6. Verify that packets are exchanged only with the intended test host.
7. Store the capture in `test-evidence/network/` with a descriptive filename.

The M7 application currently listens through the Mongoose network integration.
Use the application source and test-client configuration to confirm the
transport and port before running an automated test.

## UDP-to-CAN sensor request test

The current application defines two example UDP request strings:

| Request | Sensor selected | CAN request ID |
|---|---:|---:|
| `logical=0001` | 1 | `0x123` |
| `logical=0002` | 2 | `0x124` |

Procedure:

1. Complete the Ethernet link test.
2. Connect a CAN analyzer configured for classic CAN at 500 kbit/s.
3. Send one request to the board.
4. Confirm that the corresponding CAN request is transmitted.
5. Send the expected four-byte CAN response from the analyzer.
6. Verify that the firmware produces a UDP response for the requesting peer.
7. Repeat for both logical sensor requests.

The response-validation frame currently uses standard CAN identifier `0x321`
and the four-byte payload `DE AD BE EF`. Values above the configured threshold
produce an error response and an additional CAN alert on identifier `0x1FF`.

## Host unit-test scenario matrix

The pure UDP/CAN protocol rules are covered by
[`tests/app_protocol_test.c`](../tests/app_protocol_test.c). The suite covers:

- Exact requests `logical=0001` and `logical=0002`, including CAN IDs `0x123`
  and `0x124`.
- Rejection of unknown request values, short requests, long requests, and null
  parser arguments.
- Valid response frames at sensor values `0` and `100` (success), and at `101`
  (threshold exceeded).
- Rejection of the wrong CAN ID, every incorrect data length, each incorrect
  payload byte, null response data, and unknown sensor numbers.

Run it from the repository root with a host C compiler:

```text
gcc -std=c11 -Wall -Wextra -Werror ^
  -I projects\final\ethernet-final-actia\CM7\Core\Inc ^
  projects\final\ethernet-final-actia\CM7\Core\Src\app_protocol.c ^
  tests\app_protocol_test.c -o app_protocol_test.exe
app_protocol_test.exe
```

## Evidence and failure reporting

For each failed test, record:

- Project and core.
- Firmware commit or revision.
- Board and hardware setup.
- Exact input sent.
- UART output.
- Ethernet/CAN analyzer output.
- Expected result and observed result.
- Capture or screenshot filename.

Keep packet captures and screenshots under `test-evidence/network/`. Do not add generated
build output, credentials, or unrelated personal workstation files to the
repository.
