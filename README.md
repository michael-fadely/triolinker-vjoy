# triolinker-vjoy

*triolinker-vjoy* is a feeder for the vJoy driver to enable EMS Trio Linker to work on modern systems as the driver for the original Trio Linker does not support 64-bit Windows.

### Compatibility
- EMS Trio Linker model 0403 (USB VID_7701 PID_0003) only
- Dreamcast and Gamecube controllers (PS2 controller not tested)
- Cobalt Flux dance mat

### Prerequisites
- EMS Trio Linker model 0403 with a compatible controller
- vJoy driver http://vjoystick.sourceforge.net/site/

### Configuring vJoy
- Download and install the latest version of vJoy from the link above
- Run "Configure vJoy" from the Start Menu, tick "Enable vJoy" and add a new vJoy device

Configure the device to have the following:
- Axes: X and Y only for the Dreamcast controller; X, Y, Rx and Ry for the Gamecube controller
- Buttons: 8 buttons for Dreamcast/Gamecube controllers, 12 buttons for a dance mat
- POV: Continuous, 1 for Dreamcast/Gamecube controllers, 0 for a dance mat

### Configuring the feeder
You can edit config.ini in the feeder's folder to add the following options:
- Show or hide the debug window: HideWindow=true or HideWindow=false
- Experimental XInput support: XInput=true or XInput=false (unlikely to work at the moment)

### Using the D-Pad as individual buttons
If you need to be able to press Up+Down or Left+Right simultaneously (for example, with a dance mat), you need to disable D-Pad emulation and map Up/Down/Left/Right to individual buttons.
- Enable mapping of D-Pad directions to buttons in config.ini: DPadAsButtons=true
- Make sure your vJoy device has 12 buttons and 0 POVs
- Run the feeder and use buttons 9 (Up), 10 (Down), 11 (Left) and 12 (Right) to set up the game(s).

### Linking and unlinking the D-Pad with the analog stick
When using a Dreamcast controller, the Trio Linker synchronizes its analog stick's axes with the D-Pad. For games that use both the analog stick and the D-Pad it may be necessary to remove the analog stick adjustment. To do that, edit the following config options in the feeder's config.ini:
- Unlink the D-Pad with the analog stick: UnlinkDPad=true or UnlinkDPad=false
- Center X value for the analog stick when the D-Pad is pressed: DefaultX = 50.5
- Center Y value for the analog stick when the D-Pad is pressed: DefaultY = 50.5

The above settings are only useful for a Dreamcast controller. If you have a Gamecube controller, keep the "UnlinkDPad" option disabled, which will let you use both the D-Pad and the analog stick simultaneously.

Unfortunately with the Dreamcast controller it is not possible to use both the D-Pad and the analog stick at the same time. This is a hardware limitation. The original driver for the Trio Linker has an option to disable the analog stick adjustment when the D-Pad is pressed, but enabling that option does not allow for simultaneous use of the analog stick with the D-Pad - it only centers the analog stick's axes whenever the D-Pad is used, which is equivalent to the "UnlinkDPad" setting in this feeder.

### Running the feeder
- Make sure the vJoy device is enabled and properly configured
- Plug in the Trio Linker
- Run triolinker-vjoy.exe
- If you unplug the Trio Linker at any point, the program will terminate. Rerun triolinker-vjoy.exe after plugging it back in.

### Testing the controller ###
You can check whether the controller is working properly by running the Game Controllers applet from the Control Panel (joy.cpl), or by using vJoy's monitoring program ("Monitor vJoy" in the Start Menu).