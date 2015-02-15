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

### Name Query

* Send: "NAME"
* Receive: "NAME: <name>"

Allow the user to name the device in case several are connected to the same
machine, this can be used for discovery and fine control in a larger system.

### Name Set

* Send: "SNAME"
* Receive: "SNAME: <name>"

Set the name to what the user gave. The size is limited to 16 characters and
they must be printable characters.

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

* Send: "VSET X.XXXX"
* Receive: "VOLTAGE: SET X.XXXX"

Set the maximum voltage level.

### Current Set

* Send: "CSET X.XXXX"
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
an over-voltage situation.

### Over current protection

* Send: "CSHUTDOWN 1" or "CSHUTDOWN 0"
* Receive: "CSHUTDOWN: ENABLED" or "CSHUTDOWN: DISABLED"

When a CSHUTDOWN is set and reached the unit will shutdown, current overload is
considered when we reach the maximum current and the voltage drops by 10%
already to handle the current control, at that point it is assumed the load is
in short and the output is shutdown.

### Query configuration

* Send: "CONFIG"
* Receive: "CONFIG: Output/Voutmax/Ioutmax/Vshutdown/Cshutdown"

Report all the config variables:

* Output -- Output enabled "1" or disabled "0"
* Voutmax -- Voltage output maximum
* Ioutmax -- Current output max as set
* Vshutdown -- Voltage set for shutdown, or "0" for feature disabled
* Cshutdown -- Current shutdown enabled "1" or disabled "0"

### Status Report

* Send: "STATUS"
* Receive: "STATUS: Output/Vin/Vout/Iout/CCCV"

Reports all state variables:

* Output -- Output enabled "1" or disabled "0"
* Vin -- Voltage Input to the unit
* Vout -- Actual voltage output
* Iout -- Actual current output
* CCCV -- "1" if we are in constant current, "0" if we are in constant voltage

## Missing features

* Calibration
* Lock keys
* Save settings to EEPROM, also load them at startup
* Watchdog, can we output something on the serial?
* Internal notification logic, monitor the output values and notify immediately
  on non-trivial change (i.e. apply hysteresis so minutiae errors are not
  noisy)
