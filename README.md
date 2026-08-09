# Custompad
****
A lightweight Linux daemon for gamepad customization.

it works by grabing a physical gamepad's raw input device (via `evdev`), reading its events, and re-emits them through a **virtual Xbox 360 controller** created with the kernel's `uinput` subsystem. The original device is grabbed so only the virtual controller is visible to other applications. While the events pass through, it can:

- **Remap buttons** — reroute any button to a different one.
- **Apply radial deadzone and/or anti-deadzone** to the left/right analog sticks.

Everything is configured through a `.toml` file. By default, it only creates a virtual controller with default buttons and 0 deadzone/anti-deadzone. you can modify the default.toml or create a custom profile:

```toml
# deadzones use raw values from 0 to 32767
# 1600 is about 5% deadzone
[deadzone]
radial_LS = 3000   # left stick radial deadzone
radial_RS = 3000   # right stick radial deadzone

[anti-deadzone]
radial_LS = 0      # left stick radial anti-deadzone
radial_RS = 0      # right stick radial anti-deadzone

[remap]
# here you will use the names defined in linux input-event-codes.h.
# you can only remap to controller buttons, 
# trying to set tomething like BTN_X = "KEY_F12" won't work.
# (https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h) 
BTN_X = "BTN_NORTH"  
BTN_Y  = "BTN_WEST"
```

Here's a list of available buttons:
| Category | Event codes |
|---|---|
| Face buttons | `BTN_SOUTH`, `BTN_EAST`, `BTN_NORTH`, `BTN_WEST` |
| Shoulder buttons | `BTN_TL`, `BTN_TR`, `BTN_TL2`, `BTN_TR2` |
| Menu buttons | `BTN_SELECT`, `BTN_START`, `BTN_MODE` |
| Stick clicks | `BTN_THUMBL`, `BTN_THUMBR` |
| Digital D-pad | `BTN_DPAD_UP`, `BTN_DPAD_DOWN`, `BTN_DPAD_LEFT`, `BTN_DPAD_RIGHT` |
| Rear grips/paddles | `BTN_GRIPL`, `BTN_GRIPL2`, `BTN_GRIPR`, `BTN_GRIPR2` |
| Extra | `BTN_TRIGGER_HAPPY`, `BTN_TRIGGER_HAPPY1` |
****
## Dependencies

To build and run `custompad` you'll need:

- **[libevdev](https://www.freedesktop.org/wiki/Software/libevdev/)** — used to resolve event code names from the config file.
  - Debian/Ubuntu: `sudo apt install libevdev-dev`
  - Arch: `sudo pacman -S libevdev`
  - Fedora: `sudo dnf install libevdev-devel`
- **[tomlc17](https://github.com/cktan/tomlc17)** — the TOML parser used to read the config file. Go to its repo on how to install.
- A physical gamepad plugged in.
****
## How to use it

### 1. Build
Use the following commands:
```bash
cd src/
make custompad
```

### 2. Run it

With no arguments, `custompad` reads `default.toml` from the current directory:

```bash
./custompad
```
To use a custom config file, pass its path as the only argument:

```bash
./custompad myconfig.toml
```