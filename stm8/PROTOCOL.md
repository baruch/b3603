# Serial Protocol

This is a description of the serial protocol for this alternative firmware.

## Configuration

The serial works at 9600 8N1, the low speed is intended to allow MCU to handle
the flow without interfering too much with the rest of the work.

## Startup

The controller will send a welcome message "B3603 alternative firmware vX.XX"

## Commands

All commands are line-feed or carriage-return terminated, if the input buffer
is filled with no LF or CR character the entire line is dumped and a message is
sent on the output to indicate this failure.

### Model Query

* Send: "MODEL"
* Receive: "MODEL: B3603"

This is just the model information, it is fixed for this firmware at this time
as it only runs on a single device. There is however another device that seems
to use the same controller board and then the model returned may be different.

### Version Query

* Send: "VERSION"
* Receive: "VERSION: X.XX"

### System Configuration Query

* Send: "SYSTEM"
* Receive: "SYSTEM:\r\nMODEL: <model>\r\nVERSION: <version>\r\nNAME: <name>\r\nONSTARTUP: <ON/OFF>\r\nAUTOCOMMIT: <YES/NO>\r\n"

Get the system information: model, version, name, auto-on on startup and auto commit.

### Commit configuration

* Send: "COMMIT"
* Receive: "COMMIT: DONE\r\n"

If auto commit is off this command will change the operating parameters according to the changes done since the last commit.
If auto commit is on, this command is not going to change anything.

### Auto-commit Set

* Send: "AUTOCOMMIT <YES/NO>"
* Receive: "AUTOMMIT: YES" or "AUTOCOMMIT: NO"

Set the auto commit setting.

### Name Set

* Send: "SNAME"
* Receive: "SNAME: <name>"

Set the name to what the user gave. The size is limited to 16 characters and
they must be printable characters.

### Calibration Values

* Send: "CALIBRATION"
* Receive: detailed on calibration, mostly useful to debug and comparison of units

### Voltage Capabilities Query

* Send: "VLIST"
* Receive: "VLIST: 1.0000/12.0000/0.0001"

Returns minimum voltage, maximum voltage and step size.

### Current Capabilities Query

* Send: "CLIST"
* Receive: "CLIST: 0.001/3.000/0.001"

Returns minimum current, maximum current and step size.

### Output Enable/Disable

* Send: "OUTPUT 0" or "OUTPUT 1"
* Receive: "OUTPUT: DISABLED" or "OUTPUT: ENABLED"

OUTPUT0 disables the output and OUTPUT1 enables the output.

### Voltage Set

* Send: "VOLTAGE X.XXXX"
* Receive: "VOLTAGE: SET X.XXXX"

Set the maximum voltage level.

### Current Set

* Send: "CURRENT X.XXXX"
* Receive: "CURRENT: SET X.XXXX"

Set the maximum current level.

### Default at startup

* Send: "DEFAULT 0" or "DEFAULT 1"
* Receive: "DEFAULT: DISABLED" or "DEFAULT: ENABLED"

Set the default for the output at startup, either enabled or disabled.

Default disabled is better for safety, default enabled is useful when the unit
powers an always on device and needs to always work without a manual
intervention when the power comes back after a power outage.

### Over voltage protection

* Send: "VSHUTDOWN X.XXXX" or "VSHUTDOWN 0"
* Receive: "VSHUTDOWN: X.XXXX" or "VSHUTDOWN: DISABLED"

When a VSHUTDOWN is set and reached the unit will turn off the output to avoid
an over-voltage situation. This would be used in a constant current situation
by setting the shutdown voltage lower than the max controlled voltage and let
the microcontroller shutdown the entire output if the limit is reached.

### Over current protection

* Send: "CSHUTDOWN 1" or "CSHUTDOWN 0"
* Receive: "CSHUTDOWN: ENABLED" or "CSHUTDOWN: DISABLED"

When a CSHUTDOWN is set and reached the unit will shutdown, current overload is
considered when we reach the maximum current and the voltage drops by 10%
already to handle the current control, at that point it is assumed the load is
in short and the output is shutdown.

### Query configuration

* Send: "CONFIG"
* Receive: "CONFIG:\r\nOUTPUT: <Output>\r\nVOLTAGE SET: <Voutmax>\r\nCURRENT SET: <Ioutmax>\r\nVOLTAGE SHUTDOWN: <Vshutdown>\r\nCURRENT SHUTDOWN: <Cshutdown>\r\n"

Report all the config variables:

* Output -- Output enabled "ON" or disabled "OFF"
* Voutmax -- Voltage output maximum
* Ioutmax -- Current output max as set
* Vshutdown -- Voltage set for shutdown, or "DISABLED" for feature disabled
* Cshutdown -- Current shutdown enabled "ON" or disabled "OFF"

### Status Report

* Send: "STATUS"
* Receive: "STATUS:\r\nOUTPUT: <Output>\r\nVOLTAGE IN: <Vin>\r\nVOLTAGE OUT: <Vout>\r\nVOLTAGE OUT: <Iout>\r\nCONSTANT: <CCCV>\r\n"

Reports all state variables:

* Output -- Output enabled "ON" or disabled "OFF"
* Vin -- Voltage Input to the unit
* Vout -- Actual voltage output
* Iout -- Actual current output
* CCCV -- "CURRENT" if we are in constant current, "VOLTAGE" if we are in constant voltage

## Missing features

* Calibration
* Lock keys
* Save settings to EEPROM, also load them at startup
* Watchdog, can we output something on the serial?
* Internal notification logic, monitor the output values and notify immediately
  on non-trivial change (i.e. apply hysteresis so minutiae errors are not
  noisy)
