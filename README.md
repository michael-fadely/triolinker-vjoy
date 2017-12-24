# triolinker-vjoy

*triolinker-vjoy* is a feeder for the VJoy driver to enable EMS Trio Linker to work on modern systems as the drivers for the original Trio Linker do not support 64-bit Windows.

### Compatibility
- EMS Trio Linker model 0403 (USB VID_7701 PID_0003) only
- Dreamcast controller only, other controllers not tested

### Prerequisites
- EMS Trio Linker with a compatible controller
- VJoy driver http://vjoystick.sourceforge.net/site/

### Configuring vJoy
- Download and install the latest version of vJoy from the link above
- Run "Configure vJoy" from the Start Menu, Tick "Enable vJoy" and add a new VJoy device
- Configure the device to have the following (for a Dreamcast controller):
Axes: X and Y only
Buttons: 8 buttons
POV: Continuous, 1

### Configuring the feeder
You can edit config.ini in the feeder's folder to add the following options:
- HideWindow=true (hides the command prompt window)
- XInput=true (experimental XInput support)
- UnlinkDpad=true (prevents the analog stick from being moved when the D-Pad is pressed)
- DefaultX = 50.5 (center value for the analog X axis)
- DefaultY = 50.5 (center value for the analog Y axis)

### Running the feeder
- Make sure the vJoy device is enabled and properly configured
- Plug in the Trio Linker
- Run triolinker-vjoy.exe
- If you unplug the Trio Linker at any point, the program will terminate. Rerun triolinker-vjoy.exe after plugging it back in.
