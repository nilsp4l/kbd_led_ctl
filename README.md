# kbd_led_ctl
HID ctl program to set the firmware of your keyboard LEDs onto a controllable state. Also allows to set a color.
Built for the ASUS Vivobook S15 (ITE5570 controller) under Wayland, but adding a device is easily achievable.
For more info refer to https://nilsp4l.dev/blog/linux_stuff/keyboard_led_control/

## Why
The goal is to offer a small minimal bloat ctl program to fix the problems with ITE5570E under wayland. Other tools exist that fix it as well, yet not
as minimal and fast. While that comes in handy when running it as an init service.

## Supported devices

| VID:PID | Device |
| :--- | :--- |
| `0b05:19b6` | ASUS Vivobook S15 (ITE5570) |

Requires `asus-nb-wmi` to be loaded.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release`
```


## Usage

```bash
kbd_led_ctl --list                     # list HID devices (a row giving you info on the device tells you that it's supported)
kbd_led_ctl --filtered                 # only supported devices
kbd_led_ctl --vid 0b05 --pid 19b6      # take host control, with undefined led color 
kbd_led_ctl --vid 0b05 --pid 19b6 --color 5b00ffff # take host control, set the color 
```
