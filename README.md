# triolinker-vjoy

*triolinker-vjoy* is a feeder for the vJoy driver to enable EMS Trio Linker to work on modern systems as the drivers for the original Trio Linker do not support 64-bit Windows.

### Compatibility
- EMS Trio Linker model 0403 (USB VID_7701 PID_0003) only
- Dreamcast and Gamecube controllers (PS2 controller not tested)

### Prerequisites
- EMS Trio Linker with a compatible controller
- vJoy driver http://vjoystick.sourceforge.net/site/

### Configuring vJoy
- Download and install the latest version of vJoy from the link above
- Run "Configure vJoy" from the Start Menu, Tick "Enable vJoy" and add a new vJoy device

Configure the device to have the following:
- Axes: X and Y only for the Dreamcast controller; X, Y, Rx and Ry for the Gamecube controller
- Buttons: 8 buttons
- POV: Continuous, 1

### Configuring the feeder
You can edit config.ini in the feeder's folder to add the following options:
- Show or hide the debug window: HideWindow=true or HideWindow=false
- Experimental XInput support: XInput=true or XInput=false

### Linking and unlinking the D-Pad with the analog stick
Some games may not be able to detect input from the D-Pad. For these games to detect the D-Pad, the Trio Linker connects the D-Pad's input with the analog stick on the hardware level. It adjusts the analog stick's axes to match what you are pressing on the D-Pad. For games that use both the analog stick and the D-Pad it may be necessary to remove the analog stick adjustment. To do that, edit the following config options in the feeder's config.ini:

- Link or unlink the D-Pad with the analog stick: UnlinkDpad=true or UnlinkDpad=false
- Center X value for the analog stick: DefaultX = 50.5
- Center Y value for the analog stick: DefaultY = 50.5

### Running the feeder
- Make sure the vJoy device is enabled and properly configured
- Plug in the Trio Linker
- Run triolinker-vjoy.exe
- If you unplug the Trio Linker at any point, the program will terminate. Rerun triolinker-vjoy.exe after plugging it back in.

### Testing the controller ###
You can check whether the controller is working properly by running the Game Controllers applet from the Control Panel (joy.cpl), or by using vJoy's monitoring program ("Monitor vJoy" in the Start Menu).