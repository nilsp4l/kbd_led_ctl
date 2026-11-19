# kbd_led_ctl
HID ctl program to set the firmware of your keyboard LEDs onto a controllable state. Also allows to set a color.
Initially built for the ASUS Vivobook S15 (ITE5570 controller), but adding a device is easily achievable.
For more info refer to https://nilsp4l.dev/blog/linux_stuff/keyboard_led_control/

## Why
The goal is to offer a small minimal bloat ctl program to fix problems with leds or to simply change their color. Other tools exist that offer that functionality as well, yet not as minimal and fast, while that comes in handy when running it as an init service.

## Supported devices

| VID:PID | Device |
| :--- | :--- |
| `0b05:19b6` | ASUS Vivobook S15 (ITE5570) |
| `1e7d:2fee` | Roccat Vulcan TKL |

Requires `asus-nb-wmi` to be loaded.

## Build
The main purpose of this repository is to leave as much functionality out as possible. So, it is up to you what handlers are compiled in.

Device handlers are compiled in individually and are **disabled by default**
(a build with no flags can only list devices, not control them).

| CMake flag | Enables |
| :--- | :--- |
| `-DDEVICE_IT5570E=ON` | `0b05:19b6` |
| `-DDEVICE_VULCAN_TKL=ON` | `1e7d:2fee` |
| `-DALL_DEVICES=ON` | all of the above |


```sh
# Single device
cmake -B build -DCMAKE_BUILD_TYPE=Release -DDEVICE_IT5570E=ON

# Multiple devices
cmake -B build -DCMAKE_BUILD_TYPE=Release -DDEVICE_IT5570E=ON -DDEVICE_VULCAN_TKL=ON

# Everything
cmake -B build -DCMAKE_BUILD_TYPE=Release -DALL_DEVICES=ON

# No handlers (list-only)
cmake -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build
```

> **Note:** CMake caches option values. To change the selection, pass
> `-D...=OFF` explicitly or start from a fresh build directory
> (`rm -rf build`). Separate dirs per configuration (`build-it5570e`,
> `build-vulcan`, ...) avoid the issue entirely.


## Usage

```bash
kbd_led_ctl --list                     # list HID devices (a row giving you info on the device tells you that it's supported)
kbd_led_ctl --filtered                 # only supported devices
kbd_led_ctl --vid 0b05 --pid 19b6      # take host control, with undefined led color 
kbd_led_ctl --vid 0b05 --pid 19b6 --color 5b00ffff # take host control, set the color 
```
