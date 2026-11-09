# kbd_led_ctl
HID ctl program to set the firmware of your keyboard LEDs onto a controllable state. Also allows to set a color.
Built for the ASUS Vivobook S15 (ITE5570 controller) under Wayland, but adding a device is easily achievable.
For more info refer to https://nilsp4l.dev/blog/linux_stuff/keyboard_led_control/

## Why
The goal is to offer a small minimal bloat ctl program to fix the problems with ITE5570E under wayland. Other tools exist that fix it as well, yet not
as minimal and fast.

## Supported devices

| VID:PID | Device |
| :--- | :--- |
| `0b05:19b6` | ASUS Vivobook S15 (ITE5570) |

Requires `asus-nb-wmi` to be loaded.

## Build

```sh
cmake -B build -DCMAKE_BUILD_TYPE=MinSizeRel
cmake --build build
```

## Usage

```bash
kbd_led_ctl --list                     # enumerate HID devices (★ = supported)
kbd_led_ctl --filtered                 # only supported devices
kbd_led_ctl --vid 0b05 --pid 19b6      # take host control, with undefined led color 
kbd_led_ctl --vid 0b05 --pid 19b6 --color 5b00ffff # take host control, set the color 
```
