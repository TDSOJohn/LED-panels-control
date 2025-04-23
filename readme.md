# DMX HUB12 LED driver

Use any HUB12 display with DMX control.

### Schematics

The board is a simple Arduino Nano --> HUB12 adapter with DMX input using the SN75176 differential bus transciver.
XLR 3 pin is used for DMX input.
The power supply is +5V via screw terminals.

### Dependencies

- [DMD2](https://github.com/freetronics/DMD2)
- [DMXSerial](https://github.com/mathertel/DmxSerial)

### DMX Control

| Channel | Function |
| ------- | -------- |
| 1 | speed (slow to high) |
| 2 | pattern |
| 3 | reset / control |
| 4 | Play / Pause |