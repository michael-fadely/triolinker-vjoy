# triolinker-vjoy

[![Build status](https://ci.appveyor.com/api/projects/status/hn5r40bxivy3evto?svg=true)](https://ci.appveyor.com/project/SonicFreak94/triolinker-vjoy)

*triolinker-vjoy* is a feeder for the vJoy driver to enable any HID compliant device without proper drivers to work on modern systems. The original purpose of this program was to enable EMS Trio Linker to work on x64 versions of Windows.

### Prerequisites
- Any HID-compatible device, such as EMS Trio Linker/Trio Linker Plus with a Dreamcast controller
- vJoy driver http://vjoystick.sourceforge.net/site/
- Latest version of this feeder: https://dcmods.unreliable.network/owncloud/data/PiKeyAr/files/Various/TrioLinker/feeder-latest.zip

### Configuring vJoy
- Download and install the latest version of vJoy from the link in Prerequisites 
- Run "Configure vJoy" from the Start Menu, tick "Enable vJoy" and add a new vJoy device

Configure the device to have the following:
- Axes: X and Y only for Trio Linker (Dreamcast); X, Y, Rx and Ry for for Trio Linker (Gamecube)
- Buttons: 8 buttons for Dreamcast/Gamecube controllers, 12 buttons for a dance mat
- POV: Continuous, 1 for Dreamcast/Gamecube controllers, 0 for a dance mat

### Installing the feeder
- Download the latest version of the feeder from the link in Prerequisites 
- Extract the archive to any folder, such as C:\TrioLinker
- Edit `config.ini` in the feeder's folder for additional configuration

### Running the feeder
- Make sure the vJoy device is enabled and properly configured
- Plug in the Trio Linker
- Run triolinker-vjoy.exe
- If you unplug the device at any point, the program will terminate. Rerun triolinker-vjoy.exe after plugging it back in.

### Testing the controller ###
You can check whether the controller is working properly by running the Game Controllers applet from the Control Panel (joy.cpl), or by using vJoy's monitoring program ("Monitor vJoy" in the Start Menu).

### Advanced configuration ###

#### `[General]` section in `config.ini`

Use this section to configure general settings for the feeder.
##### 
| Field                    | Type        | Range        | Default     | Description |
| ------------------------ | ----------- | ------------ | -----------:| ----------- |
| `HideWindow`             | boolean     |     `0`, `1` |         `0` | Hide console output when the feeder is running. |
| `UnlinkDPad`             | boolean     |     `0`, `1` |         `1` | Center X and Y analog axes whenever the D-Pad is pressed. |
| `DefaultX`               | float       |   `0`, `255` |      `50.1` | Set analog X to this value whenever the D-Pad is pressed. |
| `DefaultY`               | float       |   `0`, `255` |      `50.1` | Set analog Y to this value whenever the D-Pad is pressed. |
| `DPadAsButtons`          | boolean     |     `0`, `1` |         `0` | Treat the D-Pad as individual buttons instead of a POV. |
| `VendorID`               | string      |`0000`, `FFFF`|      `7701` | USB device VID to be passed to vJoy. |
| `ProductID`              | string      |`0000`, `FFFF`|      `0003` | USB device PID to be passed to vJoy. |

#### `[Buffers]` section in `config.ini`

This section allows to configure the device's raw input buffers as analog axes, buttons or the D-Pad.

| Field                    | Type        | Range        | Default     | Description |
| ------------------------ | ----------- | ------------ | -----------:| ----------- |
| `X`         		   | integer     |     `0`, `8` |         `3` | Raw input buffer to assign to the X axis. |
| `Y`              	   | integer     |     `0`, `8` |         `4` | Raw input buffer to assign to the Y axis. |
| `Z`		           | integer     |     `0`, `8` |         `0` | Raw input buffer to assign to the Z axis. |
| `RX`              	   | integer     |     `0`, `8` |         `0` | Raw input buffer to assign to the RX axis. |
| `RY`       		   | integer     |     `0`, `8` |         `0` | Raw input buffer to assign to the RY axis. |
| `RZ`          	   | integer     |     `0`, `8` |         `0` | Raw input buffer to assign to the RZ axis. |
| `Buttons1`               | integer     |     `0`, `8` |         `1` | Raw input buffer to assign to buttons. |
| `Buttons2`               | integer     |     `0`, `8` |         `2` | Raw input buffer to assign to buttons. |
| `DPad`                   | integer     |     `0`, `8` |         `2` | Raw input buffer to assign to the D-Pad.|

#### `[DPad]` section in `config.ini`

This section allows to configure exact raw input values for each direction of the D-Pad.

| Field                    | Type        | Range        | Default     | Description |
| ------------------------ | ----------- | ------------ | -----------:| ----------- |
| `DPad North`             | string      |    `0`, `FF` |        `10` | Raw input value for D-Pad North (0 degrees). |
| `DPad NorthEast`         | string      |    `0`, `FF` |        `30` | Raw input value for D-Pad North-East (45 degrees). |
| `DPad East`              | string      |    `0`, `FF` |        `20` | Raw input value for D-Pad East (90 degrees). |
| `DPad SouthEast`         | string      |    `0`, `FF` |        `60` | Raw input value for D-Pad South-East (135 degrees). |
| `DPad South`             | string      |    `0`, `FF` |        `40` | Raw input value for D-Pad South (180 degrees). |
| `DPad SouthWest`         | string      |    `0`, `FF` |        `C0` | Raw input value for D-Pad South-West (225 degrees). |
| `DPad West`              | string      |    `0`, `FF` |        `80` | Raw input value for D-Pad West (270 degrees). |
| `DPad NorthWest`         | string      |    `0`, `FF` |        `90` | Raw input value for D-Pad North-West (315 degrees). |
| `DPad Center`            | string      |    `0`, `FF` |        `00` | Raw input value for D-Pad Center. |

### Identifying correct input buffers and values for your device ###

By default, this feeder is configured for EMS Trio Linker with a Dreamcast controller. For the feeder to work correctly with other devices you need to identify which raw input buffers on your device correspond to axes, buttons and the D-Pad/POV. Use the [HID tester utility](https://dcmods.unreliable.network/owncloud/data/PiKeyAr/files/Various/TrioLinker/hid-tester.exe) to find out which inputs are associated with raw input buffers on your device.


Usage: `hid-tester.exe --vendor-id XXXX --product-id YYYY`, where `XXXX` and `YYYY` are your device hardware IDs (VID and PID).

If the program detects your controller, you will see a list of two-digit values at the bottom. Each of those values corresponds to a raw input buffer. The first value is buffer 0, the second is buffer 1 etc. Move analog sticks and press buttons on your controller and take note which buffers react to them. For example, if the third value becomes `FF` when you move the analog stick down, it means the Y axis should be mapped to buffer 2. If your controller has more than 8 buttons it may use two input buffers for them, in which case you can use both `Buttons1` and `Buttons2` settings in `config.ini`. The D-Pad/POV may share a buffer with other buttons. To map the D-Pad properly, check each of the values corresponding to each direction of the D-Pad, including neutral/centered. Make the necessary changes in `config.ini` and restart the feeder.

Here is a sample `config.ini` mapped to a generic no-brand USB controller. This controller's analog stick uses buffers 1 and 2, the second analog stick (RX/RY in vJoy) uses buffers 4 and 5, the buttons use buffers 6 and 7, and the D-Pad shares the 6th buffer with half of the buttons. The D-Pad's neutral/centered value is `0F`.'

```ini
[General]
HideWindow=false
UnlinkDPad=false
DPadAsButtons=false
DefaultX=50.5
DefaultY=50.5
VendorID=0079
ProductID=0006

[Buffers]
X=1
Y=2
RX=4
RY=5
Buttons1=6
Buttons2=7
DPad=6

[DPad]
DPad North=0
DPad South=4
DPad West=6
DPad East=2
DPad NorthWest=7
DPad SouthWest=5
DPad NorthEast=1
DPad SouthEast=3
DPad Center=F
```