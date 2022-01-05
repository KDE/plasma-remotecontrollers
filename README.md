# Plasma Remotecontrollers

This project translates events from various input devices to keypresses on a keyboard and pointer events (mouse movement).

# Device support

| Devices | Support status | Notes |
| ------- | -------------- | ----- |
| TV remotes | Fully supported through [libcec](https://github.com/Pulse-Eight/libcec) | Not all TV remotes emit the same key events when buttons are being pressed, so particular remotes might need to map their buttons in the config file |
| Wiimotes | partial, only Wiimote and Nunchuk buttons so far | Only official Wiimotes and Wii Plus remotes are supported. Devices connected through e.g. a [DolphinBar](https://www.mayflash.com/product/showproduct.php?id=6) are not supported yet |
